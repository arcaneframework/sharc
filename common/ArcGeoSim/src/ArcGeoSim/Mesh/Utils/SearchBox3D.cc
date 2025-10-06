// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <ciso646>
#endif

#include "ArcGeoSim/Utils/Utils.h"

#include <arcane/utils/ArcanePrecomp.h>
#include <arcane/utils/Iostream.h>
#include <arcane/utils/StdHeader.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/Real3.h>

#include <algorithm>

#include "ArcGeoSim/Mesh/Utils/SearchBox3D.h"

/*---------------------------------------------------------------------------*/

class XComparator
{
public:
  XComparator(const Real3 * coord_list)
    : m_coord_list(coord_list) 
  {
    ;
  }

  bool operator()(const Integer index1, const Integer index2)
  {
    return (m_coord_list[index1].x<m_coord_list[index2].x) ;
  }

private:
  const Real3 * m_coord_list;
};

/*---------------------------------------------------------------------------*/

class YComparator
{
public:
  YComparator(const Real3 * coord_list)
    : m_coord_list(coord_list) 
  {
    ;
  }

  bool operator()(const Integer index1, const Integer index2)
  {
    return (m_coord_list[index1].y<m_coord_list[index2].y) ;
  }

private:
  const Real3 * m_coord_list;
};

/*---------------------------------------------------------------------------*/

class ZComparator
{
public:
  ZComparator(const Real3 * coord_list)
    : m_coord_list(coord_list) 
  {
    ;
  }

  bool operator()(const Integer index1, const Integer index2)
  {
    return (m_coord_list[index1].z<m_coord_list[index2].z) ;
  }

private:
  const Real3 * m_coord_list;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename Comparator>
void 
SearchBox3D::
sort(Integer* array, Integer size)
{
  std::sort(array,array+size,Comparator(m_coord_list));
}

/*---------------------------------------------------------------------------*/

SearchBox3D::Box3D::
Box3D()
  : m_xmin(0.)
  , m_xmax(0.)
  , m_ymin(0.)
  , m_ymax(0.)
  , m_zmin(0.)
  , m_zmax(0.) 
{
  ;
}

/*---------------------------------------------------------------------------*/

SearchBox3D::Box3D::
~Box3D()
{
  ;
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::Box3D::
init(const Real3& coord)
{
  m_xmin = coord.x ;
  m_xmax = coord.x ;
  m_ymin = coord.y ;
  m_ymax = coord.y ;
  m_zmin = coord.z ;
  m_zmax = coord.z ;
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::Box3D::
init(const Real3* coord,Integer size)
{
  if(size<1) return ;
  m_xmin = coord->x ;
  m_xmax = coord->x ;
  m_ymin = coord->y ;
  m_ymax = coord->y ;
  m_zmin = coord->z ;
  m_zmax = coord->z ;
  for(Integer i=1;i<size;++i)
    {
      update(coord[i]);
    }
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::Box3D::
update(const Real3& coord)
{
  m_xmin = math::min(m_xmin,coord.x) ;
  m_xmax = math::max(m_xmax,coord.x) ;
  m_ymin = math::min(m_ymin,coord.y) ;
  m_ymax = math::max(m_ymax,coord.y) ;
  m_zmin = math::min(m_zmin,coord.z) ;
  m_zmax = math::max(m_zmax,coord.z) ;
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::Box3D::
addEpsToMax(const Real & epsilon)
{
  const Real deltax = epsilon*(m_xmax-m_xmin) ;
  m_xmin -= deltax ;
  m_xmax += deltax ;
  const Real deltay = epsilon*(m_ymax-m_ymin) ;
  m_ymin -= deltay ;
  m_ymax += deltay ;
  const Real deltaz = epsilon*(m_zmax-m_zmin) ;
  m_zmin -= deltaz ;
  m_zmax += deltaz ;
}

/*---------------------------------------------------------------------------*/

bool 
SearchBox3D::Box3D::
isInBox(const Real3& point) const
{
  const bool ret = ( (m_xmin <= point.x) and (point.x <= m_xmax) and
                     (m_ymin <= point.y) and (point.y <= m_ymax) and
                     (m_zmin <= point.z) and (point.z <= m_zmax) ) ;
  // std::cout << "isInBox" << point << " = " << ret << " ;\n\t"
  //           << m_xmin << ":" << m_xmax << " x "
  //           << m_ymin << ":" << m_ymax << " x "
  //           << m_zmin << ":" << m_zmax << "\n";
  return ret;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

SearchBox3D::
SearchBox3D() 
  : m_first(-1) // Les valeurs -1 montrent une structure non initialis�e
  , m_size(-1)
  , m_sorted_list(NULL)
  , m_coord_list(NULL)
  , m_parent(NULL) 
{
    ;
}

/*---------------------------------------------------------------------------*/

SearchBox3D::
~SearchBox3D()
{
  // C'est le p�re englobant qui a la propri�t� de ces allocations
  if (m_parent==NULL)
    {
      delete[] m_sorted_list ;
      // Les coord_list sont partag�s avec l'ext�rieur
    }
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::
init(const Integer size,
     const Integer* node_list,
     const Real3* coord_list,
     const Box3D& box0,
     const Real & epsilon)
{
  ARCANE_ASSERT((size>0),("SearchBox3D initialized with non-positve number of points"));
  m_size = size ;
  if (size < 1) return;

  m_sorted_list = new Integer[size] ;
  for(Integer i=0;i<size;++i) 
    {
      m_sorted_list[i] = node_list[i] ;
    }
  m_coord_list = coord_list ;
  m_box = box0 ;
  m_box.addEpsToMax(epsilon) ;
  //   std::cout << "initBox3D :"
  //             <<" x "<<m_box.m_xmin<<" "<<m_box.m_xmax
  //             <<" y "<<m_box.m_ymin<<" "<<m_box.m_ymax
  //             <<" z "<<m_box.m_zmin<<" "<<m_box.m_zmax << endl;

  m_first = 0;
  compute(NULL);
}

/*---------------------------------------------------------------------------*/

void
SearchBox3D::
compute(SearchBox3D* parent)
{
  if(parent)
    { // Partage avec le p�re des donn�es
      m_parent = parent ;
      m_sorted_list = parent->m_sorted_list ;
      m_coord_list = parent->m_coord_list ;
    }
  if(m_size<2) return ;
  m_child.resize(8) ;
  m_child[0].m_first = m_first ;
  m_child[0].m_size = m_size ;
  m_child[0].m_box = m_box ;
  
  sortX(m_child.unguardedBasePointer()) ;
  for(Integer i=0;i<8;++i)
    {
      m_child[i].compute(this);
      //       std::cout<<"mchild "<<i
      //                <<" : x"<<m_child[i].m_box.m_xmin<<" "<<m_child[i].m_box.m_xmax
      //                <<" : y"<<m_child[i].m_box.m_ymin<<" "<<m_child[i].m_box.m_ymax
      //                <<" : z"<<m_child[i].m_box.m_zmin<<" "<<m_child[i].m_box.m_zmax;
    } 
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::
sortX(SearchBox3D* box_list)
{
  const Integer size = box_list[0].m_size ;
  const Integer first = box_list[0].m_first ;
  const Integer end = first+size;

  if( (size>1) and (m_box.m_xmin<m_box.m_xmax) )
    {
      // On recherche un plan m�dian s�parant l'espace en 2 suivant la direction X
      sort<XComparator>(m_sorted_list+first,size) ;
      Integer next = first+size/2;
      Real x_mid = m_coord_list[m_sorted_list[next-1]].x ;
      while((next<end) and (m_coord_list[m_sorted_list[next]].x==x_mid)) ++next;
      const Integer size1 = next-first; // Taille de la premi�re partie <= x_mid
      if(size1==size)
        { // Tout l'espace est dans la premi�re partie
          x_mid = m_box.m_xmax ;
        }
      else
        { // Positionne la s�paration entre la m�diane et le point suivant
          x_mid += m_coord_list[m_sorted_list[next]].x;
          x_mid /= 2.;
        }
      box_list[0].m_size = size1 ;
      box_list[0].m_box.m_xmax = x_mid ;
      sortY(box_list) ;
      box_list[4].m_first = next ;
      box_list[4].m_size = size-size1 ;
      box_list[4].m_box = m_box ;
      box_list[4].m_box.m_xmin = x_mid ;
      sortY(box_list+4) ;
    }
  else
    { // Cas particulier o� tous les points sont dans le m�me plan x
      sortY(box_list) ;
      box_list[4].m_first = end;
      box_list[4].m_size =  0 ;
      box_list[4].m_box = m_box ;
      box_list[4].m_box.m_xmin = m_box.m_xmax ;
      sortY(box_list+4) ;
    }
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::
sortY(SearchBox3D* box_list)
{
  const Integer size = box_list[0].m_size ;
  const Integer first = box_list[0].m_first ;
  const Integer end = first+size;

  if( (size>1) and (m_box.m_ymin<m_box.m_ymax) )
    {
      // On recherche un plan m�dian s�parant l'espace en 2 suivant la direction Y
      sort<YComparator>(m_sorted_list+first,size) ;
      Integer next = first+size/2; // on recherche un plan m�dian s�parant l'espace en 2
      Real y_mid = m_coord_list[m_sorted_list[next-1]].y ;
      while((next<end) and (m_coord_list[m_sorted_list[next]].y==y_mid)) ++next;
      const Integer size1 = next-first; // Taille de la premi�re partie <= y_mid
      if(size1==size)
        { // Tout l'espace est dans la premi�re partie
          y_mid = m_box.m_ymax ;
        }
      else
        { // Positionne la s�paration entre la m�diane et le point suivant
          y_mid += m_coord_list[m_sorted_list[next]].y;
          y_mid /=2.;
        }
      box_list[0].m_size = size1 ;
      box_list[0].m_box.m_ymax = y_mid ;
      sortZ(box_list) ;
      box_list[2].m_first = next ;
      box_list[2].m_size = size-size1 ;
      box_list[2].m_box = m_box ;
      box_list[2].m_box.m_ymin = y_mid ;
      sortZ(box_list+2) ;
    }
  else
    { // Cas particulier o� tous les points sont dans le m�me plan y
      sortZ(box_list) ;
      box_list[2].m_first = end;
      box_list[2].m_size = 0 ;
      box_list[2].m_box = m_box ;
      box_list[2].m_box.m_ymin = m_box.m_ymax ;
      sortZ(box_list+2) ;
    }
}

/*---------------------------------------------------------------------------*/

void 
SearchBox3D::
sortZ(SearchBox3D* box_list)
{
  const Integer size = box_list[0].m_size ;
  const Integer first = box_list[0].m_first ;
  const Integer end = first+size;

  if( (size>1) and (m_box.m_zmin<m_box.m_zmax) )
    {  
      // On recherche un plan m�dian s�parant l'espace en 2 suivant la direction Z
      sort<ZComparator>(m_sorted_list+first,size) ;
      Integer next = first+size/2;
      Real z_mid = m_coord_list[m_sorted_list[next-1]].z ;
      while((next<end) and (m_coord_list[m_sorted_list[next]].z==z_mid)) ++next;
      const Integer size1 = next-first; // Taille de la premi�re partie <= z_mid
      if(size1==size)
        { // Tout l'espace est dans la premi�re partie
          z_mid = m_box.m_zmax ;
        }
      else
        { // Positionne la s�paration entre la m�diane et le point suivant
          z_mid += m_coord_list[m_sorted_list[next]].z;
          z_mid /=2.;
        }
      box_list[0].m_size = size1 ;
      box_list[0].m_box.m_zmax = z_mid ;
      box_list[1].m_first = next ;
      box_list[1].m_size = size-size1 ;
      box_list[1].m_box = m_box ;
      box_list[1].m_box.m_zmin = z_mid ;
    }
  else
    { // Cas particulier o� tous les points sont dans le m�me plan z
      box_list[1].m_first = end;
      box_list[1].m_size = 0 ;
      box_list[1].m_box = m_box ;
      box_list[1].m_box.m_zmin = m_box.m_zmax ;
    }
}

/*---------------------------------------------------------------------------*/

Integer 
SearchBox3D::
getNearestPointId(const Real3& point)
{
  return m_sorted_list[getNearestPointIndex(point)];
}

/*---------------------------------------------------------------------------*/

Integer 
SearchBox3D::
getNearestPointIndex(const Real3 & point)
{
  ARCANE_ASSERT((m_size > 0),("Searching in empty box"));
  if (m_size==1) return m_first ;
  const Integer child_id = getBoxX(point) ;
  return m_child[child_id].getNearestPointIndex(point) ;
}

/*---------------------------------------------------------------------------*/

Integer
SearchBox3D::
getBoxX(const Real3& point)
{
  Integer child_id = 0;
  if(m_box.m_xmin<m_box.m_xmax)
    if(point.x>m_child[4].m_box.m_xmin)
      child_id = 4 ;
  return getBoxY(point,child_id) ;
}

/*---------------------------------------------------------------------------*/

Integer 
SearchBox3D::
getBoxY(const Real3& point,Integer child_id)
{  
  if(m_box.m_ymin<m_box.m_ymax)
    if(point.y>m_child[child_id+2].m_box.m_ymin)
      child_id += 2 ;
  return getBoxZ(point,child_id) ;
}

/*---------------------------------------------------------------------------*/

Integer 
SearchBox3D::
getBoxZ(const Real3& point,Integer child_id)
{   
  if(m_box.m_zmin<m_box.m_zmax)
    if(point.z>m_child[child_id+1].m_box.m_zmin)
      child_id += 1 ;
  return child_id ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
