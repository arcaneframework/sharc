// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_SURFACEUTILS_ISURFACEUTILS_H
#define ARCGEOSIM_SURFACEUTILS_ISURFACEUTILS_H

#include <arcane/utils/Real3.h>
#include <arcane/Item.h>

using namespace Arcane;

class ISurface;
class ISurfacePair;

class ISurfaceUtils
{
public:
  //! Structure d�crivant le contact Node-Face
  /*! Inclus les coordonn�es du point projet� pour compatibilit� des interfaces */
  struct NodeFaceContact 
  {
    NodeFaceContact() { }
    NodeFaceContact(const Node & node, const Face & face, const Real3 & pos) 
      : first(node), second(face), position(pos), position_node() { }
    NodeFaceContact(const Node & node, const Face & face, const Real3 & pos, const Node & node_pos) 
      : first(node), second(face), position(pos), position_node(node_pos) { }
    Node first;
    Face second;
    Real3 position;
    Node position_node;
  };

  typedef SharedArray<NodeFaceContact> NodeFaceContactList;

  /*---------------------------------------------------------------------------*/

  struct FaceFaceContact {
    FaceFaceContact() { }
    FaceFaceContact(const FaceFaceContact & c)
      : faceA(c.faceA), faceB(c.faceB),
        centerA(c.centerA), centerB(c.centerB),
        normalA(c.normalA), normalB(c.normalB) { }
    FaceFaceContact(const Face & fA, const Face & fB)
      : faceA(fA), faceB(fB) { }
    Face faceA, faceB;
    Real3 centerA, centerB;
    Real3 normalA, normalB;
  };
 
  typedef SharedArray<FaceFaceContact> FaceFaceContactList;

  /*---------------------------------------------------------------------------*/

  enum NodeFaceProjection { DirectProjection, ReverseProjection };

  /*---------------------------------------------------------------------------*/

public:
  //! Constructeur
	ISurfaceUtils() {}

  //! Destructeur
  virtual ~ISurfaceUtils() {} ;

  //! Initialisation
  virtual void init() = 0 ;

  //! Cr�ation d'une nouvelle surface � partir d'un groupe
  virtual ISurface* createSurface(FaceGroup face_group) = 0 ;

  //! Cr�ation d'un appariement de surfaces
  virtual ISurfacePair* createSurfacePair(ISurface * master, ISurface * slave) = 0 ;

  //! compute for each node of a surface the nearest face on the other surface  
  /*! surfaces are given by a pair (surface1, surface2) 
   *  \a direction defines if surface1 is the source (Direction) or the target (ReverseProjection) */
  virtual void computeSurfaceContact(ISurfacePair* surface_pair,
                                     NodeFaceContactList & contact,
                                     const NodeFaceProjection direction) = 0;

  //! compute for each face of master surface the nearest face of slave surface
  /*! where surface_pair describe (master,slave) surfaces pair */
  virtual void computeSurfaceContact(ISurfacePair* surface_pair,
                                     FaceFaceContactList & contact) = 0;
};

#endif /* ARCGEOSIM_SURFACEUTILS_ISURFACEUTILS_H */
