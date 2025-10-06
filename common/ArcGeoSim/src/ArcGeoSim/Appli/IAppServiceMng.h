// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_APPLI_IAPPSERVICEMNG_H
#define ARCGEOSIM_APPLI_IAPPSERVICEMNG_H
/**
 * \author Jean-Marc GRATIEN
 * \version 1.0
 * \brief Interface permettant de definir les services uniques d'une application
 * 
 * Exemple d'utilisation :
 * \code
 * IAppServiceMng* app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
 * IGeometryMng * service = app_service_mng->find<IGeometryMng>(true) ;
 * \endcode
 */

#include <arcane/utils/ListImpl.h>
#include <arcane/utils/String.h>
#include <arcane/IService.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/CaseOptionService.h>

#include <typeinfo>
#include <map> 

namespace Arcane {
  class ITimeLoopMng;
  class IServiceMng;
}

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Appli/ArcGeoSim_appliExport.h"
#else
#define ARCGEOSIM_APPLI_EXPORT
#endif

using namespace Arcane;

class IAppServiceMng
{
private:

  class Pointer
  {
  public:
    virtual ~Pointer() {}
  };

  template<typename T>
  class PointerT : public Pointer
  {
  public:
    PointerT(T* p) 
      : m_pointer(p)
      , m_delete_in_dtr(false) {}
    PointerT(const T& p) 
      : m_pointer(new T(p))
      , m_delete_in_dtr(true) {}
    virtual ~PointerT() {
      if(m_delete_in_dtr)
        delete m_pointer; 
    }
    T* pointer() { return m_pointer; }
  private:
    T* m_pointer;
    bool m_delete_in_dtr;
  };

  typedef std::shared_ptr<Pointer> SharedPointer;

  typedef ListImplT< std::pair<SharedPointer,bool> > PointerList;
  typedef PointerList::iterator iterator;
  
public:
  
  //! Constructeur de la classe
  IAppServiceMng();
  
  //! Destructeur de la classe
  virtual ~IAppServiceMng() {}
  
protected:
  
  void registerInstance() ;
  
  //! Initialisation  
  virtual void initializeAppServiceMng() = 0;
 
public:
  
  //! Ajout d'un service 
  ARCANE_DEPRECATED void addService(Arcane::IService* service);
  
  //! Ajout d'un service 
  ARCANE_DEPRECATED void addService(Arcane::IService* service, const String & type_name);

  //! Information sur un service absent
  void addMissingServiceInfo(const String & type_id, const String & type_name);

  /* VERSION 2 => meilleure encapsulation : 
     - les typeid ne sont pas externes 
     - on peut enlever les services manquants si ajout
     - prise en charge du dynamiccast
  */

  //! Ajout d'un service partag� 
  template<typename T> void addService(T* service)
  {
    _traceMng()->debug() << "Adding shared service "
                         << typeid(*service).name() << " (" << service << ")";
    
    _addService(service);
  }
  
  //! Ajout d'un service partag� 
  template<typename T> void addService(const T& service)
  {
    _traceMng()->debug() << "Adding shared service "
                         << typeid(service).name() << " (" << &service << ")";
    
    _addService(service);
  }
  
  //! Ajout d'un service partag�
  template<typename T> void addService(T* service, const String & type_name)
  {
    _traceMng()->debug() << "Adding shared service "
                         << typeid(*service).name() << " for "
                         << type_name << " (" << service << ")";
    
    _addService(service);
  }
  
  //! Ajout d'un service partag�
  template<typename T> void addService(const T& service, const String & type_name)
  {
    _traceMng()->debug() << "Adding shared service "
                         << typeid(service).name() << " for "
                         << type_name << " (" << &service << ")";
    
    _addService(service);
  }
  
  //! Information sur un service absent
  template<typename T> void addMissingServiceInfo(const String & type_name)
  {
    // Par souci de compatibilit�, on utilise T* et non T
    addMissingServiceInfo(typeid(T*).name(), type_name);
  }
  
  //! Ajout d'un service partag� 
  template<typename T> void addOptionalService(const Arcane::CaseOptionMultiServiceT<T>& option,
                                               const String & type_name)
  {
    if(option.size() > 0) {
      if(option.size() != 1) 
        _traceMng()->fatal() << "Multiple shared services defined";
      addService(option[0], type_name);
    } else
      addMissingServiceInfo(typeid(T*).name(), type_name);
  }
  
  //! Recherche d'un service
  /*! @param delegate_error : true si la gestion d'erreur est d�l�gu�e (ie si non trouv�)
   *  L'initialisation du service trouv� est � la charge de IAppServiceMng
   */
  template<class T>
  T* find(bool delegate_error)
  {
    if (delegate_error)
      _checkBeforeFind(typeid(T*).name());
    
    for(iterator iter=m_services.begin(); iter!=m_services.end(); ++iter)
      {
        Pointer* s = iter->first.get();
        {
          T* m = NULL;
          PointerT<Arcane::IService>* generic_m = dynamic_cast<PointerT<Arcane::IService>*>(s);
          if(generic_m == NULL) {
            // Cas enregistrement service nouvelle version
            PointerT<T>* p = dynamic_cast<PointerT<T>*>(s);
            if(p != NULL)
              m = p->pointer();
          } else {
            // Cas enregistrement service ancienne version
            m = dynamic_cast<T*>(generic_m->pointer());
          }
          if (m) 
            {
              if((*iter).second) // true si pas encore initialis�
              {
                m->init();
                (*iter).second = false ; // m�morisation de l'initialisation
              }
              return m ;
            }
        }
      }

    if (delegate_error)
      _checkNotFound(typeid(T).name());

    return NULL ;
  }

  //! D�finit le gestion de boucle pour des messages d'erreur plus pr�cis
  void setTimeLoopMng(ITimeLoopMng * time_loop_mng);

  //! Acc�s � l'instance de IAppServiceMng
  //! Si le service n'est pas trouv� :
  //! fatal_if_not_found = true  => une erreur est lanc�e
  //! fatal_if_not_found = false => retourne NULL
  static IAppServiceMng * instance(IServiceMng * sm = NULL, bool fatal_if_not_found = true);

  //! Indique si le mode est 'continue'
  //! Par d�faut, renvoie false
  //! Politique par d�faut mise en place en attendant la mise
  //! en conformit� des applications 
  virtual bool isContinue() const { return false; }

private:
  
  void _removeMissingServiceInfo(const String & type_id);

  void _checkBeforeFind(const char * name);
  void _checkNotFound(const char * name);
   
  Arcane::ITraceMng * _traceMng();
  
  template<typename T> void _addService(T* service)
  {
    ARCANE_ASSERT((service != NULL),("Service pointer null"));

    // Ajout du service
    SharedPointer p(new PointerT<T>(service));
    m_services.add(std::pair<SharedPointer,bool>(p,true));

    // On retire de la liste des services manquants
    _removeMissingServiceInfo(typeid(T*).name());
  }

  template<typename T> void _addService(const T& service)
  {
    // Ajout du service
    SharedPointer p(new PointerT<T>(service));
    m_services.add(std::pair<SharedPointer,bool>(p,true));

    // On retire de la liste des services manquants
    _removeMissingServiceInfo(typeid(T*).name());
  }

private:
  PointerList m_services; //!< liste des services avec flag d'initialisation (true => non initialis�)
  std::map<String,String> m_missing_service_infos; //!< Information descriptive des services manquant
  ITimeLoopMng * m_time_loop_mng; //!< Lien au gestionnaire de boucle en temps
  static ARCGEOSIM_APPLI_EXPORT IAppServiceMng * m_instance;
};

#endif /* ARCGEOSIM_APPLI_IAPPSERVICEMNG_H */

