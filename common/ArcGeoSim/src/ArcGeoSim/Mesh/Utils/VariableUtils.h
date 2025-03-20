// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_MESH_UTILS_VARIABLEUTILS_H
#define ARCGEOSIM_MESH_UTILS_VARIABLEUTILS_H

#include <arcane/datatype/DataTypes.h>
#include <arcane/IData.h>
#include <arcane/VariableTypes.h>

#include "ArcGeoSim/Utils/ArcGeoSim.h"


using namespace Arcane;

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/

//! Utilitaire de manipulation des variables arcane
/*! Permet de manipuler les variables arcane port�es ou non par le maillage
 * (copies, extraction d'un tableau ..). Les m�thodes retournent des IVariable
 * � partir desquelles on peut cr�er des r�f�rences (VariableRef).
 * Si la r�f�rence est construite avec un VariableBuildInfo sans pr�ciser de propri�t�,
 * les propri�t�s seront perdues et en particulier la variable sera d�truite avec la r�f�rence
 * (perte de la propri�t� IVariable::PPersistant). Il est donc conseill� de construire
 * la r�f�rence directement � partir de l'IVariable ou en utilisant ivariable->property en
 * entr�e dans le VariableBuildInfo, ou alors d'utiliser la fonction createVariableRef et
 * de stocker la r�f�rence. Dans le cas des m�thodes ou l'on retrourne la r�f�rence, les
 * variables ne sont pas construites avec la propri�t� IVariable::PPersistant. Dans ce cas l�,
 * Les variables seront d�truites quand un delete sera effectu� sur le pointeur.
 * Pour d�truire la variable on peut annuler la propri�t� IVariable::Persistant � partir
 * d'une VariableRef :
 * VariableRef ref(ivariable); ref.unsetProperty(IVariable::Persistant);
 * La variable sera alors d�truit avec la r�f�rence.
 * Ces utilitaires sont test�s dans ArcaneDemo/src/UtilsTest/UtilsTestModule.cc*/
namespace VariableUtils {

//! Copie une variable. Les propri�t�s de la variable source ne sont pas conserv�es. Elles peuvent �tre red�finies.
Arcane::IVariable* cloneVariable(Arcane::IVariable* const source_variable, const String& clone_name, int clone_property=0);

//! Cr�e une variable. La variable a la propri�t� IVariable::PPersistant.
Arcane::IVariable* createVariable(const String& name, const eDataType& data_type, const Integer dimension, ISubDomain* const sub_domain, int property=0);

//! Cr�e une variable et en retourne la r�f�rence. La variable n'a la propri�t� IVariable::PPersistant. Sa persistance est obtenue en stockant le pointeur VariableRef*
Arcane::VariableRef* createVariableRef(const String& name, const eDataType& data_type, const Integer dimension, ISubDomain* const sub_domain, int property=0);

//! Cr�e une variable port�e par le maillage. La variable a la propri�t� IVariable::PPersistant.
Arcane::IVariable* createMeshVariable(const String& name, const eDataType& data_type, const eItemKind& item_kind, const Integer dimension,
                                      const bool is_partial, const ItemGroup& group, int property = 0);

//! Cr�e une variable port�e par le maillage et retourne la r�f�rence. La variable n'a pas la propri�t� IVariable::PPersistant.Sa persistance est obtenue en stockant le pointeur VariableRef*
Arcane::VariableRef* createMeshVariableRef(const String& name, const eDataType& data_type, const eItemKind& item_kind, const Integer dimension,
                                           const bool is_partial, const ItemGroup& group, int property = 0);

//! Extrait les composantes d'une variable tableau. Cr�� une variable pour chaque composante, de nom source_name_i
Arcane::SharedArray<Arcane::IVariable*> extractArrayComponents(Arcane::IVariable* const source_array_variable);

//! D�truit des variables persistantes n'�tant plus r�f�renc�es
void deleteUnreferencedVariable(Arcane::IVariable* const unreferenced_variable, Arcane::ITraceMng* const trace_mng);

//! D�truit une variable obtenue par createVariableRef ou createMeshVariableRef. Il ne doit pas y avoir d'autre r�f�rence active
//! que celle fournie par la fonction create
void deleteVariableFromRef(Arcane::VariableRef*& variable_ref, Arcane::ITraceMng* const trace_mng);

//! Tailles de la donn�e \a mesh_array_variable_data d'une variable tableau sur le maillage.
/* \a item_number donne le nombre d'item et \a array_size la dimension du tableau.*/
void meshVariableArraySize(Arcane::IData* mesh_array_variable_data, Integer& item_number, Integer& array_size);

//! Redimensionne le tableau d'une variable tableau portee par le maillage.
void resizeMeshVariableArray(Arcane::IVariable* mesh_array_variable, Arcane::Integer dim2_size);

//! Redimensionne le tableau d'une variable tableau non portee par le maillage
void resizeVariableArray(Arcane::IVariable* array_variable, Arcane::Integer size);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Private tools
inline Arcane::VariableRef * _switchVariable(const String& name, const eDataType& data_type, const Integer dimension, ISubDomain* const sub_domain, int property = 0);
inline Arcane::VariableRef * _switchMeshVariable(const String& name, const eDataType& data_type, const eItemKind& item_kind, const Integer dimension,
                                const bool is_partial, const ItemGroup& group, int property = 0);

template< template<typename> class VariableT> inline Arcane::VariableRef * _switchDataTypeT(const eDataType& data_type,const VariableBuildInfo& builder);
template< template<typename> class VariableT> inline Arcane::VariableRef * _switchMeshDataTypeT(const eDataType& data_type,const eItemKind& item_kind ,
                                                                                                const VariableBuildInfo& builder);
inline Arcane::SharedArray<Arcane::IVariable*> _extractVariableArrayComponents(Arcane::IVariable* const source_array_variable);
inline Arcane::SharedArray<Arcane::IVariable*> _extractMeshVariableArrayComponents(Arcane::IVariable* const source_array_variable);


}  // namespace VariableUtilsPrivateTools definition


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

#endif /* ARCGEOSIM_MESH_UTILS_VARIABLEUTILS_H */
