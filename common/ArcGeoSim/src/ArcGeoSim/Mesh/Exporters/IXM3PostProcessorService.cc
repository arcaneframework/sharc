// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Interface du service 
#include <arcane/IPostProcessorWriter.h>

#include <arcane/VariableCollection.h>
#include <arcane/utils/Collection.h>
#include <arcane/ItemGroup.h>
#include <arcane/utils/Enumerator.h>
#include <arcane/PostProcessorWriterBase.h>
#include <arcane/IIOMng.h>
#include <arcane/IParallelMng.h>
#include <arcane/ArcaneVersion.h>

#include "ArcGeoSim/Mesh/Exporters/IXM3/IXM3DataWriter.h"
#include "ArcGeoSim/Mesh/Utils/VariableUtils.h"

#include "IXM3PostProcessor_axl.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IXM3PostProcessorService 
  : public ArcaneIXM3PostProcessorObject
{
public:
  
  /** Constructeur de la classe */
  IXM3PostProcessorService(const Arcane::ServiceBuildInfo & sbi) 
    : ArcaneIXM3PostProcessorObject(sbi), m_data_writer(NULL), m_is_master(true) {}
  
  /** Destructeur de la classe */
  ~IXM3PostProcessorService() {}
  
public:

  //! Construit l'instance
  void build();


 public:

  /*!
   * \brief Retourne l'�crivain associ� � ce post-processeur.
   */
  IDataWriter* dataWriter() {return m_data_writer;}

  /*!
   * \brief Positionne le nom du r�pertoire de sortie des fichiers.
   * Ce r�pertoire doit exister.
   */
  void setBaseDirectoryName(const String& dirname);

  //! Nom du r�pertoire de sortie des fichiers.
  const String& baseDirectoryName() {return PostProcessorWriterBase::baseDirectoryName();}

  /*!
   * \brief Positionne le nom du fichier contenant les sorties
   */
  void setBaseFileName(const String& filename) {PostProcessorWriterBase::setBaseFileName(filename);}

  //! Nom du fichier contenant les sorties.
  const String& baseFileName() {return PostProcessorWriterBase::baseFileName();}

  //! Set mesh
  void setMesh(IMesh * mesh) {m_mesh = mesh; m_mesh_set = true;}

  //! Positionne la liste des temps
  void setTimes(RealConstArrayView times);

  //! Liste des temps sauv�s
  RealConstArrayView times() {return PostProcessorWriterBase::times();}

  //! Positionne la liste des variables � sortir
  void setVariables(VariableCollection variables);

  //! Liste des variables � sauver
  VariableCollection variables() {return PostProcessorWriterBase::variables();}

  //! Positionne la liste des groupes � sortir
  void setGroups(ItemGroupCollection groups);

  //! Liste des groupes � sauver
  ItemGroupCollection groups() {return PostProcessorWriterBase::groups();}

 public:

  //! Notifie qu'une sortie va �tre effectu�e avec les param�tres courants.
  void notifyBeginWrite();

  //! Notifie qu'une sortie vient d'�tre effectu�e.
  void notifyEndWrite();

 public:

  //! Ferme l'�crivain. Apr�s fermeture, il ne peut plus �tre utilis�
  void close() {}

 private:
  IDataWriter* m_data_writer;
  IMesh* m_mesh;
  Integer m_event_number;
  bool m_is_master;
  bool m_events_initialized;
  bool m_variables_set;
  bool m_groups_set;
  bool m_times_set;
  bool m_directory_name_set;
  bool m_mesh_set;
  bool m_is_ready;
  SharedArray<IVariable*> m_extracted_variables; //!<Stockage de variables temporaires provenant de l'extraction des variables tableaux.

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
IXM3PostProcessorService::
build()
{
  // build PostProcessorWriterBase
  PostProcessorWriterBase::build();

  // initial settings to be done
  m_events_initialized = false;
  m_variables_set = false;
  m_groups_set = false;
  m_times_set = false;
  m_directory_name_set = false;
  m_mesh_set = false;
  m_is_ready = false;

  // Initialize event number
  m_event_number = 1;

  // Event global file does not exist
  m_events_initialized = false;
}

/*---------------------------------------------------------------------------*/

void
IXM3PostProcessorService::
setTimes(RealConstArrayView times)
{
  PostProcessorWriterBase::setTimes(times);
  m_times_set = true;
}

/*---------------------------------------------------------------------------*/

void
IXM3PostProcessorService::
setBaseDirectoryName(const String& dirname)
{
  PostProcessorWriterBase::setBaseDirectoryName(dirname);
  m_directory_name_set = true;
}

/*---------------------------------------------------------------------------*/
void
IXM3PostProcessorService::
setVariables(VariableCollection variables)
{
  // Temporary handling of array mesh variable (before their handling within mesh xsd)
  // Create scalar mesh variable with each component of array variables.
  VariableCollection internal_variables;
  for (VariableCollection::Enumerator var_enum = variables.enumerator(); ++var_enum;)
    {
      if ((*var_enum)->dimension() == 2)
        {
          // Extract the components
          m_extracted_variables = ArcGeoSim::VariableUtils::extractArrayComponents(*var_enum);
          for (Integer i = 0; i< m_extracted_variables.size(); ++i) internal_variables.add(m_extracted_variables[i]);
        }
      else internal_variables.add(*var_enum);
    }
  PostProcessorWriterBase::setVariables(internal_variables);
  m_variables_set = true;
}

/*---------------------------------------------------------------------------*/

void
IXM3PostProcessorService::
setGroups(ItemGroupCollection groups)
{
  PostProcessorWriterBase::setGroups(groups);
  m_groups_set = true;
}

/*---------------------------------------------------------------------------*/

void
IXM3PostProcessorService::
notifyBeginWrite()
{
  // Check if mesh set, otherwise set default mesh
  if (! m_mesh_set) m_mesh = mesh();

  // Check if Post Pro ready
  m_is_ready = (m_groups_set && m_variables_set && m_times_set && m_directory_name_set);
  if (! m_is_ready)
      fatal()<< "Post processor settings incomplete. Must set mesh, groups, variables, times and directory name. Exiting";

  // Build data writer. How to couple it with PostProcessorWriterBase?
  m_data_writer = new IXM3DataWriter(this, m_mesh, m_event_number, m_events_initialized, options()->useFaceAid());
}

/*---------------------------------------------------------------------------*/

void
IXM3PostProcessorService::
notifyEndWrite()
{
  // Increment event_number
  ++m_event_number;

  // Event global file is created
  m_events_initialized = true;

  // Delete data writer ;
  delete m_data_writer;
  // Delete temporary variables coming from mesh array extraction
  for (SharedArray<IVariable*>::iterator ivariable = m_extracted_variables.begin(); ivariable != m_extracted_variables.end(); ++ivariable)
    {
      // delete temporary variable
      ArcGeoSim::VariableUtils::deleteUnreferencedVariable(*ivariable, traceMng());
    }
  m_extracted_variables.clear();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_IXM3POSTPROCESSOR(IXM3PostProcessor,IXM3PostProcessorService);
ARCANE_REGISTER_SERVICE_IXM3POSTPROCESSOR(IfpPostProcessorV3,IXM3PostProcessorService); // alias de compatibilit�
ARCANE_REGISTER_SERVICE_IXM3POSTPROCESSOR(IfpPostProcessorNew,IXM3PostProcessorService); // alias de compatibilit�
