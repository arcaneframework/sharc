// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_UTILS_SEARCHBOX3D_H
#define ARCGEOSIM_MESH_UTILS_SEARCHBOX3D_H

using namespace Arcane ;

#include <arcane/ArcaneVersion.h>
#if (ARCANE_VERSION >= 12201)
#include <arcane/utils/Array.h>
#else /* ARCANE_VERSION */
#include <arcane/utils/CArray.h>
#endif /* ARCANE_VERSION */

/*!
 * Classe de distribution spaciale de points (via k-D tree)
 * puis de recherche rapide de points dans la boite associ�e
 */
class SearchBox3D
{
public:
  //! Boite de R3
  class Box3D
  {
  public:
    Box3D() ;
    virtual ~Box3D() ;
  public:
    void init(const Real3& coord) ;
    void init(const Real3* coord,Integer size) ;
    void update(const Real3& coord) ;
    void addEpsToMax(const Real & epsilon) ;
    bool isInBox(const Real3& point) const;
    Real m_xmin, m_xmax;
    Real m_ymin, m_ymax;
    Real m_zmin, m_zmax;
  } ;

public:
  //! Constructeur
	SearchBox3D() ;

  //! Destructeur
	virtual ~SearchBox3D() ;

public:
  //! Initialisation de la structure
  /*! \param size nombre de points � injecter
   *  \param node_list index des points � injecter
   *  \param coord_list table de tous les coordonn�es du maillage (non limit� aux node_list)
   *  \param box0 boite englobante propos�e (non v�rifi�e pendant l'init)
   *  \param epsilon tolerance relative appliqu�e sur la boite englobante
   */
  void init(const Integer size,
            const Integer* node_list,
            const Real3* coord_list,
            const Box3D& box0,
            const Real & epsilon) ;

  //! test l'appartenance d'un point � la boite englobante
  inline bool isInBox(const Real3& point) const { return m_box.isInBox(point); }

  //! Retourne l'identifiant du point le plus proche du point propos�
  /*! La proximit� est d�finie au sens de m�me cellule de k-D tree
   *  L'identifiant correspond � un identifiant de node_list fourni � la m�thode init()
   */
  Integer getNearestPointId(const Real3& point) ;

private:

  //@{ @name m�thodes internes de construction de la structure
  void compute(SearchBox3D* parent) ;
  void sortX(SearchBox3D* box_list) ;
  void sortY(SearchBox3D* box_list) ;
  void sortZ(SearchBox3D* box_list) ;
  template<typename Order> void sort(Integer* array,Integer size) ;
  //@}
  
  //@{ @name m�thodes internes de recherche dans la structure
  Integer getBoxX(const Real3& point) ;
  Integer getBoxY(const Real3& point, Integer child_id) ;
  Integer getBoxZ(const Real3& point, Integer child_id) ;
  //! Comme getNearestPointId mais retourne un identifiant interne
  Integer getNearestPointIndex(const Real3& point) ;
  //@}

private:
  Box3D m_box ;
  Integer m_first ;
  Integer m_size ;
  Integer* m_sorted_list ;
  const Real3* m_coord_list ;
  SharedArray<SearchBox3D> m_child ;
  SearchBox3D* m_parent ;
};


#endif /* ARCGEOSIM_MESH_UTILS_SEARCHBOX3D_H */
