#include "PolyhedralGeometry.h"
using namespace Arcane;
#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/NotImplementedException.h>

#include <iostream>

using namespace std;

Real
PolyhedralGeometry::
computeLength(const ItemWithNodes & item)
{
  throw FatalErrorException(A_FUNCINFO,"Not implemented");
}
Real
PolyhedralGeometry::
computeLength(const Real3& m, const Real3& n)
{
  
  const Real3 d = m-n ;
  return math::sqrt(math::scaMul(d,d)) ;
}


Real3
PolyhedralGeometry::
computeFaceCenter(const ItemWithNodes & item)
{

    Real3 face_center(0,0,0);

    if( item->isFace() )
    {

        for (Arcane::NodeEnumerator inode(item->nodes()); inode(); ++inode)
        {
            face_center += m_node_coords[inode]/item->nbNode();
        }

    }


    return face_center;
}

Real3
PolyhedralGeometry::
computeFaceBarycenter(const ItemWithNodes & item)
{
    Real3 face_barycenter(0,0,0);

    Real3 face_meancenter(0,0,0);

    Real face_measure(0);

    if( item->isFace() )
    {

        face_meancenter = computeFaceCenter(item);

        for (Arcane::NodeEnumerator inode(item->nodes()); inode(); ++inode)
        {
            Arcane::Real  triarea(0);
            Arcane::Real3 tricenter(0,0,0);
            Arcane::Node inext = (inode.index() < item->nbNode()-1)? item->node(inode.index()+1):item->node(0);

            triarea = 0.5 * (Arcane::math::crossProduct3(m_node_coords[inode]-face_meancenter,
                    m_node_coords[inext]-face_meancenter).abs());
            tricenter = (1./3.)*(m_node_coords[inode] + m_node_coords[inext] +face_meancenter);
            face_measure    += triarea;
            face_barycenter += triarea * tricenter;
        }

        face_barycenter /= face_measure;

    }


    return face_barycenter;
}

Real3
PolyhedralGeometry::
computeCentroid(const ItemWithNodes & item)
    {
      Real3 face_normal(0,0,0);
      Real3 barycenter(0,0,0);
      Real volume(0);

      //int nbNode = item->nbNode();
      //Arcane::SharedArray array = Arcane::SharedArray(nbNode);


      if (m_dimension == 3 )
      {
          Cell cell = item->toCell();

          volume = computeMeasure(cell);

          Real3 face_barycenter;

          for (Arcane::FaceEnumerator iface(cell->faces()); iface(); ++iface)
          {
              //Normale sortante a la face
              int signe = (iface->backCell().uniqueId()==cell->uniqueId())? 1:-1;
              face_normal= computeNormalizedFaceNormal(* iface);

              Arcane::Real3 outward_normal = signe * face_normal;

              //Decoupage en triangles inode, inext, face_barycenter
              for (Arcane::NodeEnumerator inode(iface->nodes()); inode(); ++inode)
              {
                  Arcane::Node inext = (inode.index() < iface->nbNode()-1)? iface->node(inode.index()+1):iface->node(0);
                  face_barycenter = computeFaceBarycenter(*iface);

                  barycenter.x += (0.5/volume) * (outward_normal.x *
                          computeIntegralX2(m_node_coords[inode], m_node_coords[inext], face_barycenter)).x;
                  barycenter.y += (0.5/volume) * (outward_normal.y *
                          computeIntegralX2(m_node_coords[inode], m_node_coords[inext], face_barycenter)).y;
                  barycenter.z += (0.5/volume) * (outward_normal.z *
                          computeIntegralX2(m_node_coords[inode], m_node_coords[inext], face_barycenter)).z;
              }
          }

      }

      return barycenter;
    }



Real3
PolyhedralGeometry::
computeCenter(const ItemWithNodes & item)
{
    SharedArray<Real> coords;
    return computeCenter(item,coords);
}

Real3
PolyhedralGeometry::
computeCenter(const ItemWithNodes & item, Array<Real> & coords)
{
  // Choix d'un point arbitraire d'�toilement, mais "proche" du centre
 /*Real3 vcenter(0,0,0);
  for(Integer i=0;i<item.nbNode();++i) {
    vcenter += m_node_coords[item.node(i)];
  }
  vcenter /= item.nbNode();*/

  Real3 vcenter(-1,-1,-1);

  Real volume = 0;
  Real3 center(0,0,0);

  // Initialisations de tableaux utilis�s pour les coordonn�es barycentriques
  coords.resize(item.nbNode());

  const Cell cell = item.toCell();

  for(Integer i=0;i<cell.nbNode();++i)
      coords[i] = 0.;

  std::map<Int64, Int32> NodeIds2Index;

  for(Integer i=0;i<cell.nbNode();i++)
  {
      Node noeud = cell.node(i);
      NodeIds2Index.insert(std::pair<Int64, Int32>(noeud.uniqueId(),i));
  }


  Real fvolume = 0;

  for(Integer i=0;i<cell.nbFace();++i)
    {
      fvolume = 0;
      const Face face = cell.face(i);
      const Real orientation = (face.frontCell() == cell)?1:-1; // normale entrante dans le volume

      if (face.nbNode() == 3) {
        const Real v = orientation * computeTetraedronVolume(m_node_coords[face.node(0)],
                m_node_coords[face.node(1)], m_node_coords[face.node(2)],vcenter);
        center += v * computeTetraedronCenter(m_node_coords[face.node(0)],
                m_node_coords[face.node(1)],
                m_node_coords[face.node(2)],
                                                            vcenter);
        coords[NodeIds2Index[face.node(0).uniqueId()]] += v/3.;
        coords[NodeIds2Index[face.node(1).uniqueId()]] += v/3.;
        coords[NodeIds2Index[face.node(2).uniqueId()]] += v/3.;
        fvolume += v;
        volume += v;
      } else if (face.nbNode() == 4) {
        // Le tableau contient le modulo ... (utile uniquement pour GEOMETRY_OLD_FORMULA
        const Real3 coord[5] = { m_node_coords[face.node(0)],
                m_node_coords[face.node(1)],
                m_node_coords[face.node(2)],
                m_node_coords[face.node(3)],
                m_node_coords[face.node(0)] };


        Real s0, s1, s2, s3, totalS;

        Real3 fcenter = computeQuadrilateralCenter(coord[0],coord[1],coord[2],coord[3],s0, s1, s2, s3, totalS);
        /*Real3 fcenter(0,0,0) ;
        for(Integer i=0;i<4;++i)
        {
            fcenter += coord[i] ;
        }
        fcenter /= 4 ;*/
        for(Integer j=0;j<4;++j) {
          const Real v = orientation * computeTetraedronVolume(coord[j], coord[j+1], fcenter, vcenter);
          center += v * computeTetraedronCenter(coord[j], coord[j+1], fcenter, vcenter);
          coords[NodeIds2Index[face.node(j).uniqueId()]] += 0.25*v;
          if (j < 3 )
              coords[NodeIds2Index[face.node(j+1).uniqueId()]] += 0.25*v;
          else
              coords[NodeIds2Index[face.node(0).uniqueId()]] += 0.25*v;
          fvolume += v;
          volume += v;
        }
        //for(Integer j=0;j<4;++j)
        //   coords[NodeIds2Index[face.node(j).uniqueId()]] += 0.25*fvolume/4.;
        coords[NodeIds2Index[face.node(0).uniqueId()]] += 0.25*fvolume*(s0 + s1 + s3)/(totalS*3);
        coords[NodeIds2Index[face.node(1).uniqueId()]] += 0.25*fvolume*(s0 + s2 + s3)/(totalS*3);
        coords[NodeIds2Index[face.node(2).uniqueId()]] += 0.25*fvolume*(s0 + s1 + s2)/(totalS*3);
        coords[NodeIds2Index[face.node(3).uniqueId()]] += 0.25*fvolume*(s1 + s2 + s3)/(totalS*3);

      } else {
        Integer nb_nodes = face.nbNode() ;
        Real3 fcenter(0,0,0) ;
        SharedArray<Real3> coord(nb_nodes+1) ;
        for(Integer i=0;i<nb_nodes;++i)
        {
          coord[i] = m_node_coords[face.node(i)] ;
          fcenter += coord[i] ;
        }
        fcenter /= nb_nodes ;
        coord[nb_nodes] = coord[0] ;
        for(Integer j=0;j<nb_nodes;++j) {
          const Real v = orientation * computeTetraedronVolume(coord[j], coord[j+1], fcenter, vcenter);
          center += v * computeTetraedronCenter(coord[j], coord[j+1], fcenter, vcenter);
          coords[NodeIds2Index[face.node(j).uniqueId()]] += 0.25*v;
          if ( j < nb_nodes-1)
              coords[NodeIds2Index[face.node(j+1).uniqueId()]] += 0.25*v;
          else
              coords[NodeIds2Index[face.node(0).uniqueId()]] += 0.25*v;
          fvolume += v;
          volume += v;
        }
        for(Integer j=0;j<nb_nodes;++j)
           coords[NodeIds2Index[face.node(j).uniqueId()]] += 0.25*fvolume/nb_nodes;
      }
    }

  for(Integer i=0;i<cell.nbNode();i++)
  {
      coords[NodeIds2Index[cell.node(i).uniqueId()]] /= volume;
      coords[NodeIds2Index[cell.node(i).uniqueId()]] += 0.25/cell.nbNode();
  }


#ifndef NDEBUG
  if (volume == 0.)
    {
      center = m_node_coords[cell.node(0)];
    }
  else
    {
      center /= volume;
    }
#else /* NDEBUG */
  center /= volume;
#endif /* NDEBUG */

  // normalization

  Real sumCoords = 0.;
  for(Integer i=0;i<cell.nbNode();++i)
      sumCoords += coords[i];

  for(Integer i=0;i<cell.nbNode();++i)
  {
      coords[i] /= sumCoords;
  }

 return center;

}




Real
PolyhedralGeometry::
computeArea(const ItemWithNodes & item)
{
    Arcane::Real3 center(0,0,0);
    Arcane::Real area(0);

    //Calcul du centre moyen des faces
    for (Arcane::NodeEnumerator inode(item->nodes()); inode(); ++inode)
    {
        center += m_node_coords[inode]/item->nbNode();
    }

    for (Arcane::NodeEnumerator inode(item->nodes()); inode(); ++inode)
    {
        Arcane::Real  triarea(0);

        Arcane::Node inext = (inode.index() < item->nbNode()-1)? item->node(inode.index()+1):item->node(0);

        triarea = 0.5 * (Arcane::math::crossProduct3(m_node_coords[inode]-center, m_node_coords[inext]-center).abs());

        area    += triarea;
    }

  return area ;

}


Real
PolyhedralGeometry::
computeMeasure(const ItemWithNodes & item)
{
    Arcane::Real3 face_normal(0,0,0);
    Arcane::Real cell_measure(0);

    if (m_dimension == 3 )
    {
        Cell cell = item->toCell();
        for (Arcane::FaceEnumerator iface(cell->faces()); iface(); ++iface)
        {
            face_normal= computeNormalizedFaceNormal(*iface);
            const auto back_cell = iface->backCell();
            int signe = (!back_cell.null() && iface->backCell().uniqueId()==item->uniqueId())? 1:-1;
            Arcane::Real3 outward_normal = signe * face_normal;
            cell_measure += computeArea(*iface)*Arcane::math::dot(outward_normal, m_node_coords[iface->node(0)]);
        }
        cell_measure = (1./3.) * fabs(cell_measure);

    }
    else
    {
        cell_measure = computeArea(item);
    }

    return cell_measure;

}


/*---------------------------------------------------------------------------*/
Real3
PolyhedralGeometry::
computeIntegralX2(Arcane::Real3 node1, Arcane::Real3 node2,Arcane::Real3 node3)
{
    //A partir de trois vecteurs Real3 representant les coordonnees d'un triangle,
    //integre la fonction (x^2, y^2, z^2) par une formule de gauss d'ordre 2
    Arcane::Real triarea = 0.5 * (Arcane::math::crossProduct3(node1-node3, node2-node3).abs());
    Arcane::Real3 PGauss1(0.5*(node2+node3)), PGauss2(0.5*(node1+node2)), PGauss3(0.5*(node1+node3));
    Arcane::Real3 integral_x2(0,0,0);
    integral_x2.x = (2*triarea/6.) * (pow(PGauss1.x,2) + pow(PGauss2.x, 2) + pow(PGauss3.x, 2));
    integral_x2.y = (2*triarea/6.) * (pow(PGauss1.y,2) + pow(PGauss2.y, 2) + pow(PGauss3.y, 2));
    integral_x2.z = (2*triarea/6.) * (pow(PGauss1.z,2) + pow(PGauss2.z, 2) + pow(PGauss3.z, 2));
    return integral_x2;
}

Real3 PolyhedralGeometry::computeNormalizedFaceNormal(const ItemWithNodes & iface)
{
    Arcane::Real3 face_normal(0,0,0);
    if(!(iface->isFace()))
    {
        return face_normal;
    }
    Arcane::Real3 center=computeFaceCenter(iface);
    for (Arcane::NodeEnumerator inode(iface->nodes()); inode(); ++inode)
    {
        Arcane::Node inext = (inode.index() < iface->nbNode()-1)? iface->node(inode.index()+1):iface->node(0);

        face_normal+= Arcane::math::crossProduct3(m_node_coords[inode] - center,
                                                  m_node_coords[inext] - center);


    }
    face_normal= face_normal.normalize();

    return face_normal;

}



