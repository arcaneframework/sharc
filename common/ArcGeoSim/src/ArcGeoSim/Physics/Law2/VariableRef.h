// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_PHYSICS_LAW_VARIABLEREF_H
#define ARCGEOSIM_PHYSICS_LAW_VARIABLEREF_H

/*
 * \ingroup Law
 * \brief Outil pour la r�f�rence de variables pour les valeurs + d�riv�es pour :
 * - variables
 * - variables partielles
 * - tableaux
 * - scalaires
 *
 * Ce qui est utilis� pour le stockage dans la base de donn�es VariableDataBase
 *
 */

#include "ArcGeoSim/Physics/Law2/Property.h"
#include "ArcGeoSim/Physics/Law2/ContainerTraits.h"
#include "ArcGeoSim/Physics/Law2/VariableResizer.h"

#include <arcane/IVariableMng.h>
#include <arcane/IItemFamily.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * \ingroup Law
 * \brief R�f�rence de variable g�n�rique
 *
 * D�crit les r�f�rences stock�es :
 * - type de donn�es
 * - partielle ou non dans le cas de variables sur maillage
 * - type d'item dans le cas de variables sur maillage
 * 
 * Met � disposition des m�canismes statique + dynamique (VariableBuildInfo) pour la construction
 * des r�f�rences
 *
 */
class VariableRef
{
public:
  
  virtual ~VariableRef() {}

protected:
  
  VariableRef(const Property& p, Arcane::MeshVariableRef* v) 
    : m_property(p) 
    , m_item_kind(v->variable()->itemKind())
    , m_data_type(v->dataType())
    , m_is_mesh_reference(true)
    , m_is_partial_reference(v->variable()->isPartial())
    , m_is_scalar(false)
    , m_has_derivatives(false) 
    , m_v(v) 
  {
    ARCANE_ASSERT((m_property.isInitialized()),(String::format("Property '{0}' is not initialized",m_property.name()).localstr()));
  }
  
  VariableRef(const Property& p, Arcane::MeshVariableRef* v, Arcane::MeshVariableRef* dv)
    : m_property(p) 
    , m_item_kind(v->variable()->itemKind())
    , m_data_type(v->dataType())
    , m_is_mesh_reference(true)
    , m_is_partial_reference(v->variable()->isPartial())
    , m_is_scalar(false)
    , m_has_derivatives(true) 
    , m_v(v)
    , m_dv(dv) {}
  
  VariableRef(const Property& p, Arcane::VariableRef* v, bool is_scalar)
    : m_property(p) 
    , m_item_kind(Arcane::IK_Unknown)
    , m_data_type(v->dataType())
    , m_is_mesh_reference(false)
    , m_is_partial_reference(false)
    , m_is_scalar(is_scalar)
    , m_has_derivatives(false) 
    , m_v(v) {}
  
  VariableRef(const Property& p, Arcane::VariableRef* v, Arcane::VariableRef* dv, bool is_scalar)
    : m_property(p) 
    , m_item_kind(Arcane::IK_Unknown)
    , m_data_type(v->dataType())
    , m_is_mesh_reference(false)
    , m_is_partial_reference(false)
    , m_is_scalar(is_scalar)
    , m_has_derivatives(true) 
    , m_v(v)
    , m_dv(dv) {}
 
public:
  
  //! Propri�t� de la r�f�rence
  const Property& property() const { return m_property; }
  
  //! Teste si les d�riv�es sont pr�sentes
  bool hasDerivatives() const { return m_has_derivatives; }
  
  //! Teste si la r�f�rences est partielle (cas variable sur maillage)
  bool isPartialReference() const { return m_is_partial_reference; }
  
  //! Teste si la r�f�rences est une variable sur maillage
  bool isMeshReference() const { return m_is_mesh_reference; }
  
  //! Teste si la r�f�rences est une variable scalaire
  bool isScalar() const { return m_is_scalar; }

  //! Type d'item (cas variable sur maillage)
  Arcane::eItemKind itemKind() const { return m_item_kind; }
  
  //! Type de donn�es
  Arcane::eDataType dataType() const { return m_data_type; }
  
  //! Nom du conteneur de valeur
  const Arcane::VariableRef& values() const { return *m_v; }
  
  //! Nom du conteneur de valeur
  const Arcane::VariableRef& derivatives() const 
  { 
    ARCANE_ASSERT((m_has_derivatives),(String::format("VariableRef of property '{0}' hasn't derivatives",m_property.name()).localstr()));
    return *m_dv;
  }

  //! Constructeur statique des valeurs
  template<typename V>
  V& values() const 
  {
    return *(static_cast<V*>(m_v.get()));
  }
 
  //! Constructeur dynamique des valeurs
  template<typename V>
  V& derivatives() const 
  {
    ARCANE_ASSERT((m_has_derivatives),(String::format("VariableRef of property '{0}' hasn't derivatives",m_property.name()).localstr()));
    return *(static_cast<V*>(m_dv.get()));
  }
  
  //! Pour retailler les tableaux
  virtual void resize(Integer size) const {}
  
private:
  
  const Property m_property;
  const Arcane::eItemKind m_item_kind;
  const Arcane::eDataType m_data_type;
  const bool m_is_mesh_reference;
  const bool m_is_partial_reference;
  const bool m_is_scalar;
  const bool m_has_derivatives;
  const std::shared_ptr<Arcane::VariableRef> m_v;
  const std::shared_ptr<Arcane::VariableRef> m_dv;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename P, typename K>
struct VariableRefT
  : public VariableRef
{ 
  typedef K ItemType;
  typedef P PropertyType;
  typedef typename VariableContainer<P,K>::ValueType ValueType;
  typedef typename VariableContainer<P,K>::DerivType DerivType;
  
  VariableRefT(const P& p, ValueType* v)
    : VariableRef(p, v) { VariableResizer<P>::resize(p, *v); }

  VariableRefT(const P& p, ValueType* v, DerivType* dv)
    : VariableRef(p, v, dv) { VariableResizer<P>::resize(p, *v); }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename P, typename K>
struct PartialVariableRefT
  : public VariableRef
{
  typedef K ItemType;
  typedef P PropertyType;
  typedef typename PartialVariableContainer<P,K>::ValueType ValueType;
  typedef typename PartialVariableContainer<P,K>::DerivType DerivType;
  
  PartialVariableRefT(const P& p, ValueType* v)
    : VariableRef(p, v) { VariableResizer<P>::resize(p, *v); }

  PartialVariableRefT(const P& p, ValueType* v, DerivType* dv)
    : VariableRef(p, v, dv) { VariableResizer<P>::resize(p, *v); }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename P>
struct ArrayRefT
  : public VariableRef
{  
  typedef P PropertyType;
  typedef typename ArrayContainer<P>::ValueType ValueType;
  typedef typename ArrayContainer<P>::DerivType DerivType;
  
  ArrayRefT(const P& p, ValueType* v)
    : VariableRef(p, v, false) {}

  ArrayRefT(const P& p, ValueType* v, DerivType* dv)
    : VariableRef(p, v, dv, false) {}

  void resize(Integer size) const 
  {
    ArrayResizer<P>::resize(property(), size, values<ValueType>());
    
    // Pour les d�riv�es, on ne retaille que la premi�re dimension
    if(hasDerivatives()) derivatives<DerivType>().resize(size);
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename P>
struct ScalarRefT
  : public VariableRef
{
  typedef P PropertyType;
  typedef typename ScalarContainer<P>::ValueType ValueType;
  typedef typename ScalarContainer<P>::DerivType DerivType;

  ScalarRefT(const P& p, ValueType* v)
    : VariableRef(p, v, true) { ScalarResizer<P>::resize(p, *v); }

  ScalarRefT(const P& p, ValueType* v, DerivType* dv)
    : VariableRef(p, v, dv, true) { ScalarResizer<P>::resize(p, *v); }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_LAW_VARIABLEREF_H */
