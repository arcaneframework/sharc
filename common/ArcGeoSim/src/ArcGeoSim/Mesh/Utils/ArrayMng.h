// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_EVOLUTIVEMESH_UTILS_ARRAYMNG_H
#define ARCGEOSIM_MESH_EVOLUTIVEMESH_UTILS_ARRAYMNG_H

#include <iostream>

#include <arcane/IData.h>

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Utils/ParallelUtils.h"

BEGIN_ARCGEOSIM_NAMESPACE

class ArrayMng
{
  /*! Interface de manipulation de tableaux de donn�es template
   *  Les tableaux peuvent �tre cr��s � partir d'une IData ou d'un string
   *  Les tableaux permettent de remplir une IData.
   *  Il est possible d'interpoler deux tableaux cr�es.
   */
public:
  
  /** Destructeur de la classe */
  virtual ~ArrayMng() {}
  
public:

  //! Cl� pour manipuler le tableau typ�e depuis l'ext�rieur
  typedef String DataKey;

  /*! Point d'interpolation
   *    DataKey => cl� du tableau de valeur pour l'interpolation (doit �tre de type r�el : Real, Real2, ...)
   *    Real => point d'interpolation
   */

  typedef std::pair<DataKey,Real> DataInterpolationPoint;
  typedef std::pair<DataKey,ArrayMng*> DataInterpolatedPoint;

public:

  //! Cr�ation du tableau par extraction des valeurs d'une IData \a data
  virtual DataKey extractData(Arcane::IData* const data,const Int32ConstArrayView& data_item_indexes) = 0;

  /* ! Extraction des valeurs d'une IData dans le tableau reference par \a data_key. Si  \a data_key ne reference pas
   *   de tableau. Un nouveau tableau est cree et la cle est ecrase par la cle de ce nouveau tableau
   */
  virtual void extractData(DataKey& data_key, Arcane::IData* const data,const Int32ConstArrayView& data_item_indexes) = 0;

  //! Cr�ation du tableau par extraction des valeurs d'un string \a data_string
  virtual DataKey extractDataFromString(const String& data_string) = 0;

  //! Cr�ation du tableau par extraction des valeurs d'un fichier HDF5 \a filename. La position dans le fichier est donnee par \a file_position
  //! Cette operation est parallele
  virtual DataKey collectiveExtractDataFromFile(const String& filename, const String& file_position, ArcGeoSim::ParallelUtils& parallel_utils) = 0;

  //! Cr�ation du tableau par extraction d'un sous tableau pr�cedemment cr��
  virtual DataKey extractSubData(const DataKey& data_key, const Array<Integer>& extracted_value_indexes) = 0;

  /*! Copie les valeurs du tableau r�f�renc� par \a source_value_key dans l'IData \a data aux positions \a data_indexes
   * La taille du tableau r�f�renc� par \a source_value_key doit �tre �gale � la taille de \a data_indexes
   */
  virtual void fillData(Arcane::IData* const data,const Int32ConstArrayView& data_item_indexes,const DataKey& source_value_key) const = 0;

  /*! Interpole deux tableaux stock�s. Les points \a first_point et \a second_point fournissent, par l'interm�diaire
   * des cl�s deux tableux de valeurs ainsi que l'abscisse correspondant � ces valeurs. L'interpolation est r�alis�e au
   * point \a interpolation_point.
   * Un nouveau tableau contenant les donn�es interpol�es est cr��. Sa cl� est retourn�e
   */
  virtual DataInterpolatedPoint interpolateData(const Real& interpolation_point,
                                  const DataInterpolationPoint& first_point,
                                  const DataInterpolationPoint& second_point) = 0;

  //! D�truit le tableau r�f�renc� par la cl� \a data_key
  virtual void deleteData(const DataKey& data_key) = 0;

  //! Donne la taille d'un tableau r�f�renc� par \a data_key
  virtual Integer dataSize(const DataKey& data_key) const = 0;

  //! Affiche la taille et le contenu du tableau r�f�renc� par \a data_key
  virtual void print(std::ostream& o,const DataKey& data_key) const = 0;

  //! Retourne le type de la donnee
  virtual Arcane::eDataType dataType() const = 0;

};

END_ARCGEOSIM_NAMESPACE

#endif /* ARCGEOSIM_MESH_EVOLUTIVEMESH_UTILS_ARRAYMNG_H */
