// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_MESH_GEOMETRY_EUCLIDIANGEOMETRY_EUCLIDIANGEOMETRY_H
#error "Template implementation has to be used with template declaration"
#endif 

#define GEOMETRY_OLD_FORMULA

/*---------------------------------------------------------------------------*/


template<Integer Dimension, typename CoordinatesVariable>
EuclidianGeometryT<Dimension, CoordinatesVariable>::
EuclidianGeometryT(const CoordinatesVariable & coords,
                   ITraceMng * traceMng, 
                   bool fatal_on_error,
                   Integer normal_2d_id)
  : m_coords(coords)
  , m_trace_mng(traceMng)
  , m_fatal_on_error(fatal_on_error)
  , m_2d_normal_id(normal_2d_id)
{ 
  
}

/*---------------------------------------------------------------------------*/

template<Integer Dimension, typename CoordinatesVariable>
Real3
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeCenter(const ItemWithNodes & item)
{
  Real3 orientation, mass_center, center;
  Real measure;
  SharedArray<Real> coords;

  switch (item.type()) {
  case IT_Line2:
    ComputeLine2(this).computeOrientedMeasureAndCenter(item,orientation,center);
    return center;
  case IT_Triangle3:
    ComputeTriangle3(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Quad4:
    ComputeQuad4(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Pentagon5:
    ComputePentagon5(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Hexagon6:
    ComputeHexagon6(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Tetraedron4:
    ComputeTetraedron4(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Pyramid5:
    ComputePyramid5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Pentaedron6:
    ComputePentaedron6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Hexaedron8:
    ComputeHexaedron8(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Heptaedron10:
    ComputeHeptaedron10(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Octaedron12:
    ComputeOctaedron12(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_HemiHexa5:
    ComputeHemiHexa5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_HemiHexa6:
    ComputeHemiHexa6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_HemiHexa7:
    ComputeHemiHexa7(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_AntiWedgeLeft6 :
    ComputeAntiWedgeLeft6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_AntiWedgeRight6 :
    ComputeAntiWedgeRight6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_DiTetra5 :
    ComputeDiTetra5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Enneedron14:
    ComputeEnneedron14(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_Decaedron16:
    ComputeDecaedron16(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_Heptagon7:
    ComputeHeptagon7(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Octogon8:
    ComputeOctogon8(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
#endif
  default:
    m_trace_mng->info()<<"Not supported Item Type"<<item.type();
    throw FatalErrorException(A_FUNCINFO,"Not supported Item Type");
  }
}

/*---------------------------------------------------------------------------*/

template<Integer Dimension, typename CoordinatesVariable>
Real3
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeCenter(const ItemWithNodes & item, Array<Real> & coords)
{
  Real3 orientation, mass_center, center;
  Real measure;
  //SharedArray<Real> coords;

  switch (item.type()) {
  case IT_Line2:
    ComputeLine2(this).computeOrientedMeasureAndCenter(item,orientation,center);
    return center;
  case IT_Triangle3:
    ComputeTriangle3(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Quad4:
    ComputeQuad4(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Pentagon5:
    ComputePentagon5(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Hexagon6:
    ComputeHexagon6(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Tetraedron4:
    ComputeTetraedron4(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Pyramid5:
    ComputePyramid5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Pentaedron6:
    ComputePentaedron6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Hexaedron8:
    ComputeHexaedron8(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Heptaedron10:
    ComputeHeptaedron10(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_Octaedron12:
    ComputeOctaedron12(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center;
  case IT_HemiHexa5:
    ComputeHemiHexa5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_HemiHexa6:
    ComputeHemiHexa6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_HemiHexa7:
    ComputeHemiHexa7(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_AntiWedgeLeft6 :
    ComputeAntiWedgeLeft6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_AntiWedgeRight6 :
    ComputeAntiWedgeRight6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_DiTetra5 :
    ComputeDiTetra5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Enneedron14:
    ComputeEnneedron14(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_Decaedron16:
    ComputeDecaedron16(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return center ;
  case IT_Heptagon7:
    ComputeHeptagon7(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
  case IT_Octogon8:
    ComputeOctogon8(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return center;
#endif
  default:
    m_trace_mng->info()<<"Not supported Item Type"<<item.type();
    throw FatalErrorException(A_FUNCINFO,"Not supported Item Type");
  }
}


template<Integer Dimension, typename CoordinatesVariable>
Real3
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeOrientedMeasure(const ItemWithNodes & item)
{
  Real3 orientation, mass_center, center;
  Real measure;
  SharedArray<Real> coords;

  switch (item.type()) {
  case IT_Line2:
    ComputeLine2(this).computeOrientedMeasureAndCenter(item,orientation,center);
    return orientation;
  case IT_Triangle3:
    ComputeTriangle3(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return orientation;
  case IT_Quad4:
    ComputeQuad4(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return orientation;
  case IT_Pentagon5:
    ComputePentagon5(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return orientation;
  case IT_Hexagon6:
    ComputeHexagon6(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return orientation;
  case IT_Tetraedron4:
    ComputeTetraedron4(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Pyramid5:
    ComputePyramid5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Pentaedron6:
    ComputePentaedron6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Hexaedron8:
    ComputeHexaedron8(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Heptaedron10:
    ComputeHeptaedron10(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Octaedron12:
    ComputeOctaedron12(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_HemiHexa5:
    ComputeHemiHexa5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_HemiHexa6:
    ComputeHemiHexa6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_HemiHexa7:
    ComputeHemiHexa7(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_AntiWedgeLeft6 :
    ComputeAntiWedgeLeft6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_AntiWedgeRight6 :
    ComputeAntiWedgeRight6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_DiTetra5 :
    ComputeDiTetra5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Enneedron14:
    ComputeEnneedron14(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Decaedron16:
    ComputeDecaedron16(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return Real3(0,0,measure);
  case IT_Heptagon7:
    ComputeHeptagon7(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return orientation;
  case IT_Octogon8:
    ComputeOctogon8(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return orientation;
#endif
  default:
    throw FatalErrorException(A_FUNCINFO,"Not supported Item Type");
  }
}

template<Integer Dimension, typename CoordinatesVariable>
Real
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeMeasure(const ItemWithNodes & item)
{
  Real3 orientation, mass_center, center;
  Real measure;
  SharedArray<Real> coords;

  switch (item.type()) {
  case IT_Line2:
    ComputeLine2(this).computeOrientedMeasureAndCenter(item,orientation,center);
    return math::normeR3(orientation);
  case IT_Triangle3:
    ComputeTriangle3(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Quad4:
    ComputeQuad4(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Pentagon5:
    ComputePentagon5(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Hexagon6:
    ComputeHexagon6(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Tetraedron4:
    ComputeTetraedron4(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Pyramid5:
    ComputePyramid5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Pentaedron6:
    ComputePentaedron6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Hexaedron8:
    ComputeHexaedron8(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Heptaedron10:
    ComputeHeptaedron10(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Octaedron12:
    ComputeOctaedron12(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_HemiHexa5:
    ComputeHemiHexa5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_HemiHexa6:
    ComputeHemiHexa6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_HemiHexa7:
    ComputeHemiHexa7(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_AntiWedgeLeft6 :
    ComputeAntiWedgeLeft6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_AntiWedgeRight6 :
    ComputeAntiWedgeRight6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_DiTetra5 :
    ComputeDiTetra5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Enneedron14:
    ComputeEnneedron14(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Decaedron16:
    ComputeDecaedron16(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Heptagon7:
    ComputeHeptagon7(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Octogon8:
    ComputeOctogon8(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
#endif
  default:
    throw FatalErrorException(A_FUNCINFO,"Not supported Item Type");
  }
}

template<Integer Dimension, typename CoordinatesVariable>
Real
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeLength(const ItemWithNodes & item)
{
  if(item.type() == IT_Line2){
    const Real3 d = this->m_coords[item.node(1)] - this->m_coords[item.node(0)];
    return math::sqrt(math::scaMul(d,d)) ;
  }
  else
    throw FatalErrorException(A_FUNCINFO,"Not implemented");
}

template<Integer Dimension, typename CoordinatesVariable>
Real
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeArea(const ItemWithNodes & item)
{
  Real3 orientation, mass_center, center;
  SharedArray<Real> coords;

  switch (item.type()) {
  case IT_Line2:
    return 0.;
  case IT_Triangle3:
    ComputeTriangle3(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Quad4:
    ComputeQuad4(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Pentagon5:
    ComputePentagon5(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Hexagon6:
    ComputeHexagon6(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Heptagon7:
    ComputeHeptagon7(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
  case IT_Octogon8:
    ComputeOctogon8(this).computeOrientedMeasureAndCenter(item,orientation,center, mass_center,coords);
    return math::normeR3(orientation);
#endif
  case IT_Tetraedron4:
  case IT_Pyramid5:
  case IT_Pentaedron6:
  case IT_Hexaedron8:
  case IT_Heptaedron10:
  case IT_Octaedron12:
  case IT_HemiHexa7:
  case IT_HemiHexa6:
  case IT_HemiHexa5:
  case IT_AntiWedgeLeft6:
  case IT_AntiWedgeRight6:
  case IT_DiTetra5:
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Enneedron14:
  case IT_Decaedron16:
#endif
    return 0.;
  default:
    throw FatalErrorException(A_FUNCINFO,"Not supported Item Type");
  }
}

template<Integer Dimension, typename CoordinatesVariable>
Real
EuclidianGeometryT<Dimension, CoordinatesVariable>::
computeVolume(const ItemWithNodes & item)
{
  Real3 center;
  Real measure;
  SharedArray<Real> coords;

  switch (item.type()) {
  case IT_Line2:
    return 0.;
    break;
  case IT_Triangle3:
  case IT_Quad4:
  case IT_Pentagon5:
  case IT_Hexagon6:
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Heptagon7:
  case IT_Octogon8:
#endif
    return 0.;
  case IT_Tetraedron4:
    ComputeTetraedron4(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Pyramid5:
    ComputePyramid5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Pentaedron6:
    ComputePentaedron6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Hexaedron8:
    ComputeHexaedron8(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Heptaedron10:
    ComputeHeptaedron10(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Octaedron12:
    ComputeOctaedron12(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_HemiHexa7:
    ComputeHemiHexa7(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_HemiHexa6:
    ComputeHemiHexa6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_HemiHexa5:
    ComputeHemiHexa5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_AntiWedgeLeft6:
    ComputeAntiWedgeLeft6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_AntiWedgeRight6:
    ComputeAntiWedgeRight6(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_DiTetra5:
    ComputeDiTetra5(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  case IT_Enneedron14:
    ComputeEnneedron14(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
  case IT_Decaedron16:
    ComputeDecaedron16(this).computeOrientedMeasureAndCenter(item,measure,center,coords);
    return measure;
#endif
  default:
    throw FatalErrorException(A_FUNCINFO,"Not supported Item Type");
  }
}

/*---------------------------------------------------------------------------*/

// 1d elements

template<typename CoordinatesVariable>
void
ComputeLine2T<2, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center)
{
  // normal to the segment in 2d
  orientation.y = this->m_coords[item.node(1)].z - this->m_coords[item.node(0)].z;
  orientation.x = 0.;
  orientation.z = this->m_coords[item.node(0)].y - this->m_coords[item.node(1)].y;
  center = 0.5 * (this->m_coords[item.node(1)] + this->m_coords[item.node(0)]);
}

template<typename CoordinatesVariable>
void
ComputeLine2T<3, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center)
{
  // tangent to the segment in 3d
  orientation = this->m_coords[item.node(1)] - this->m_coords[item.node(0)];
  center = 0.5 * (this->m_coords[item.node(1)] + this->m_coords[item.node(0)]);
}

// 2d elements

template<Integer Dimension, typename CoordinatesVariable>
void
ComputeTriangle3T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center, Real3 & mass_center, Array<Real> & coords)
{
  orientation =
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]);

  mass_center = center =
    this->m_geom->computeTriangleCenter(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]);

  coords.resize(3);
  coords[0] = 1/3.;
  coords[1] = 1/3.;
  coords[2] = 1/3.;

}

template<Integer Dimension, typename CoordinatesVariable>
void
ComputeQuad4T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center, Real3 & mass_center, Array<Real> & coords)
{
  //std::cout << A_FUNCINFO << "Computing on " << ItemPrinter(item) << "\n";

  orientation =
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(2)],
                                        this->m_coords[item.node(3)]);

#ifndef NDEBUG
  try {
#endif /* NDEBUG */
  Real s0, s1, s2, s3, totalS;
  mass_center = center =
      this->m_geom->computeQuadrilateralCenter(this->m_coords[item.node(0)],
                                               this->m_coords[item.node(1)],
                                               this->m_coords[item.node(2)],
                                               this->m_coords[item.node(3)],s0, s1, s2, s3, totalS);
  coords.resize(4);
  coords[0] = (s0 + s1 + s3)/totalS;
  coords[1] = (s0 + s2 + s3)/totalS;
  coords[2] = (s0 + s1 + s2)/totalS;
  coords[3] = (s1 + s2 + s3)/totalS;
#ifndef NDEBUG
  } catch (GeometryException & e) {
    mass_center = center = this->m_coords[item.node(0)];
    if (this->m_geom->m_trace_mng) {
      if (this->m_geom->m_fatal_on_error)
        this->m_geom->m_trace_mng->fatal() << "Degenerated Item " << ItemPrinter(item);
      else
        this->m_geom->m_trace_mng->error() << "Degenerated Item " << ItemPrinter(item);   
    }
  }
#endif /* NDEBUG */
}

template<Integer Dimension, typename CoordinatesVariable>
void
ComputePentagon5T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center, Real3 & mass_center, Array<Real> & coords)
{
  orientation =
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(2)],
                                        this->m_coords[item.node(3)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(3)],
                                        this->m_coords[item.node(4)]);

  mass_center = center =
    this->m_geom->computePentagonalCenter(this->m_coords[item.node(0)],
                                          this->m_coords[item.node(1)],
                                          this->m_coords[item.node(2)],
                                          this->m_coords[item.node(3)],
                                          this->m_coords[item.node(4)]);

  coords.resize(5);
  coords[0] = 1.0/5.0;
  coords[1] = 1.0/5.0;
  coords[2] = 1.0/5.0;
  coords[3] = 1.0/5.0;
  coords[4] = 1.0/5.0;

}


template<Integer Dimension, typename CoordinatesVariable>
void
ComputeHexagon6T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center, Real3 & mass_center, Array<Real> & coords)
{
  orientation =
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(2)],
                                        this->m_coords[item.node(3)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(3)],
                                        this->m_coords[item.node(4)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(4)],
                                        this->m_coords[item.node(5)]);

  mass_center = center =
    this->m_geom->computeHexagonalCenter(this->m_coords[item.node(0)],
                                         this->m_coords[item.node(1)],
                                         this->m_coords[item.node(2)],
                                         this->m_coords[item.node(3)],
                                         this->m_coords[item.node(4)],
                                         this->m_coords[item.node(5)]);

  coords.resize(6);
  coords[0] = 1.0/6.0;
  coords[1] = 1.0/6.0;
  coords[2] = 1.0/6.0;
  coords[3] = 1.0/6.0;
  coords[4] = 1.0/6.0;
  coords[5] = 1.0/6.0;

}

#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
template<Integer Dimension, typename CoordinatesVariable>
void
ComputeHeptagon7T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center, Real3 & mass_center, Array<Real> & coords)
{
  orientation =
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(2)],
                                        this->m_coords[item.node(3)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(3)],
                                        this->m_coords[item.node(4)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(4)],
                                        this->m_coords[item.node(5)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(5)],
                                        this->m_coords[item.node(6)]);

  mass_center = center =
    this->m_geom->computeHeptagonalCenter(this->m_coords[item.node(0)],
                                         this->m_coords[item.node(1)],
                                         this->m_coords[item.node(2)],
                                         this->m_coords[item.node(3)],
                                         this->m_coords[item.node(4)],
                                         this->m_coords[item.node(5)],
                                         this->m_coords[item.node(6)]);

  coords.resize(7);
  coords[0] = 1.0/7.0;
  coords[1] = 1.0/7.0;
  coords[2] = 1.0/7.0;
  coords[3] = 1.0/7.0;
  coords[4] = 1.0/7.0;
  coords[5] = 1.0/7.0;
  coords[6] = 1.0/7.0;

}

template<Integer Dimension, typename CoordinatesVariable>
void
ComputeOctogon8T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real3 & orientation, Real3 & center, Real3 & mass_center, Array<Real> & coords)
{
  orientation =
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(1)],
                                        this->m_coords[item.node(2)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(2)],
                                        this->m_coords[item.node(3)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(3)],
                                        this->m_coords[item.node(4)]) +
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(4)],
                                        this->m_coords[item.node(5)])+
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(5)],
                                        this->m_coords[item.node(6)])+
    this->m_geom->computeTriangleNormal(this->m_coords[item.node(0)],
                                        this->m_coords[item.node(6)],
                                        this->m_coords[item.node(7)]);

  mass_center = center =
    this->m_geom->computeOctogonalCenter(this->m_coords[item.node(0)],
                                         this->m_coords[item.node(1)],
                                         this->m_coords[item.node(2)],
                                         this->m_coords[item.node(3)],
                                         this->m_coords[item.node(4)],
                                         this->m_coords[item.node(5)],
                                         this->m_coords[item.node(6)],
                                         this->m_coords[item.node(7)]);

  coords.resize(8);
  coords[0] = 1.0/8.0;
  coords[1] = 1.0/8.0;
  coords[2] = 1.0/8.0;
  coords[3] = 1.0/8.0;
  coords[4] = 1.0/8.0;
  coords[5] = 1.0/8.0;
  coords[6] = 1.0/8.0;
  coords[7] = 1.0/8.0;

}
#endif


template<Integer Dimension, typename CoordinatesVariable>
void
ComputeTetraedron4T<Dimension, CoordinatesVariable>::
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real & measure, Real3 & center, Array<Real> & coords)
{
  ARCANE_ASSERT((item.type() == IT_Tetraedron4),("Bad item type: %d vs %d (Tetra4)",item.type(),IT_Tetraedron4));
  measure = this->m_geom->computeTetraedronVolume(this->m_coords[item.node(0)],
                                                  this->m_coords[item.node(1)],
                                                  this->m_coords[item.node(2)],
                                                  this->m_coords[item.node(3)]);
  center = this->m_geom->computeTetraedronCenter(this->m_coords[item.node(0)],
                                                 this->m_coords[item.node(1)],
                                                 this->m_coords[item.node(2)],
                                                 this->m_coords[item.node(3)]);

  coords.resize(4);
  for(Integer j=0;j<4;++j) coords[j] = 0.25;

}

template<Integer Dimension, typename CoordinatesVariable>
void
ComputeTetraedron4T<Dimension, CoordinatesVariable>::
computeVolumeArea(const ItemWithNodes & item, Real & area)
{
  ARCANE_ASSERT((item.type() == IT_Tetraedron4),("Bad item type: %d vs %d (Tetra4)",item.type(),IT_Tetraedron4));
  const Cell cell = item.toCell();
  for(Integer i=0;i<4;++i)
    {
      const Face face = cell.face(i);
      area += math::abs(this->m_geom->computeTriangleSurface(this->m_coords[face.node(0)],
                                                             this->m_coords[face.node(1)],
                                                             this->m_coords[face.node(2)]));
    }
}





/*---------------------------------------------------------------------------*/
template<Integer Dimension, typename CoordinatesVariable>
void
ComputeGenericVolumeT<Dimension, CoordinatesVariable>::
//computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real & measure, Real3 & center, coordsType & coords)
computeOrientedMeasureAndCenter(const ItemWithNodes & item, Real & measure, Real3 & center, Array<Real> & coords)
{
  // Choix d'un point arbitraire d'toilement, mais "proche" du centre
  Real3 vcenter(0,0,0);
  for(Integer i=0;i<item.nbNode();++i) {
    vcenter += this->m_coords[item.node(i)];
  }
  vcenter /= item.nbNode();

  Real volume = 0;
  center = Real3(0,0,0);

  // Initialisations de tableaux utilises pour les coordonnees barycentriques
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
        Real v = orientation * this->m_geom->computeTetraedronVolume(this->m_coords[face.node(0)],
                                                                     this->m_coords[face.node(1)],
                                                                     this->m_coords[face.node(2)],
                                                                     vcenter);
        center += v * this->m_geom->computeTetraedronCenter(this->m_coords[face.node(0)],
                                                            this->m_coords[face.node(1)],
                                                            this->m_coords[face.node(2)],
                                                            vcenter);
        coords[NodeIds2Index[face.node(0).uniqueId()]] += v/3.;
        coords[NodeIds2Index[face.node(1).uniqueId()]] += v/3.;
        coords[NodeIds2Index[face.node(2).uniqueId()]] += v/3.;
        fvolume += v;
        volume += v;
      } else if (face.nbNode() == 4) {
        // Le tableau contient le modulo ... (utile uniquement pour GEOMETRY_OLD_FORMULA
        const Real3 coord[5] = { this->m_coords[face.node(0)],
                                  this->m_coords[face.node(1)],
                                  this->m_coords[face.node(2)],
                                  this->m_coords[face.node(3)],
                                  this->m_coords[face.node(0)] };


        Real s0, s1, s2, s3, totalS;
#ifndef NDEBUG
        Real3 fcenter(0,0,0);
        try {
          fcenter = this->m_geom->computeQuadrilateralCenter(coord[0],coord[1],coord[2],coord[3],s0, s1, s2, s3, totalS);
            /*for(Integer i=0;i<4;++i)
            {
                fcenter += coord[i] ;
            }
            fcenter /= 4 ;*/
        } catch (GeometryException & e) {
          fcenter = this->m_coords[cell.node(0)];
          if (this->m_geom->m_trace_mng) {
            if (this->m_geom->m_fatal_on_error)
              this->m_geom->m_trace_mng->fatal() << "Degenerated Item " << ItemPrinter(item);
            else
              this->m_geom->m_trace_mng->error() << "Degenerated Item " << ItemPrinter(item);   
          }
        }
#else /* NDEBUG */
        const Real3 fcenter = this->m_geom->computeQuadrilateralCenter(coord[0],coord[1],coord[2],coord[3],s0, s1, s2, s3, totalS);
        /*Real3 fcenter(0,0,0) ;
        for(Integer i=0;i<4;++i)
        {
            fcenter += coord[i] ;
        }
        fcenter /= 4 ;*/
#endif /* NDEBUG */



#ifndef GEOMETRY_OLD_FORMULA
        const Real3 face_normal = 
          this->m_geom->computeTriangleNormal(coord[0], coord[1], coord[2]) +
          this->m_geom->computeTriangleNormal(coord[0], coord[2], coord[3]);
        const Real3 pc = fcenter-vcenter;
        const Real3 lcenter = vcenter + 0.75*pc;
        const Real lvolume = orientation * math::scaMul(pc,face_normal)/3;
        center -= lcenter * lvolume; // - car orientation est normale entrante
        volume -= lvolume;
#else /* GEOMETRY_OLD_FORMULA */
        for(Integer j=0;j<4;++j) {
          const Real v = orientation * this->m_geom->computeTetraedronVolume(coord[j], coord[j+1], fcenter, vcenter);
          center += v * this->m_geom->computeTetraedronCenter(coord[j], coord[j+1], fcenter, vcenter);

          coords[NodeIds2Index[face.node(j).uniqueId()]] += v/4.;
          if (j < 3 )
              coords[NodeIds2Index[face.node(j+1).uniqueId()]] += v/4.;
          else
              coords[NodeIds2Index[face.node(0).uniqueId()]] += v/4.;
          fvolume += v;
          volume += v;
        }
        //for(Integer j=0;j<4;++j)
        //   coords[NodeIds2Index[face.node(j).uniqueId()]] += 0.25*fvolume/4.;

        coords[NodeIds2Index[face.node(0).uniqueId()]] += 0.25*fvolume*(s0 + s1 + s3)/(totalS*3);
        coords[NodeIds2Index[face.node(1).uniqueId()]] += 0.25*fvolume*(s0 + s2 + s3)/(totalS*3);
        coords[NodeIds2Index[face.node(2).uniqueId()]] += 0.25*fvolume*(s0 + s1 + s2)/(totalS*3);
        coords[NodeIds2Index[face.node(3).uniqueId()]] += 0.25*fvolume*(s1 + s2 + s3)/(totalS*3);

#endif /* GEOMETRY_OLD_FORMULA */
      } else {
        Integer nb_nodes = face.nbNode() ;
        Real3 fcenter(0,0,0) ;
        SharedArray<Real3> coord(nb_nodes+1) ;
        for(Integer i=0;i<nb_nodes;++i)
        {
          coord[i] = this->m_coords[face.node(i)] ;
          fcenter += coord[i] ;
        }
        fcenter /= nb_nodes ;
        coord[nb_nodes] = coord[0] ;
        for(Integer j=0;j<nb_nodes;++j) {
          const Real v = orientation * this->m_geom->computeTetraedronVolume(coord[j], coord[j+1], fcenter, vcenter);
          center += v * this->m_geom->computeTetraedronCenter(coord[j], coord[j+1], fcenter, vcenter);

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

  measure = volume;

#ifndef NDEBUG
  if (volume == 0.) 
    {
      center = this->m_coords[cell.node(0)];
      if (this->m_geom->m_trace_mng) {
        if (this->m_geom->m_fatal_on_error)
          this->m_geom->m_trace_mng->fatal() << "Degenerated Volume Item " << ItemPrinter(item);
        else
          this->m_geom->m_trace_mng->error() << "Degenerated Volume Item " << ItemPrinter(item);
      }
    }
  else
    {
      center /= volume;
    }
#else /* NDEBUG */
  center /= volume;
#endif /* NDEBUG */


}

/*---------------------------------------------------------------------------*/

template<Integer Dimension, typename CoordinatesVariable>
void
ComputeGenericVolumeT<Dimension, CoordinatesVariable>::
computeVolumeArea(const ItemWithNodes & item, Real & area)
{
  const Cell cell = item.toCell();
  for(Integer i=0;i<cell.nbFace();++i)
    {
      const Face face = cell.face(i);
      if (face.nbNode() == 3) {
        area += math::abs(this->m_geom->computeTriangleSurface(this->m_coords[face.node(0)],
                                                               this->m_coords[face.node(1)],
                                                               this->m_coords[face.node(2)]));
      } else if (face.nbNode() == 4) {
        area += math::abs(this->m_geom->computeTriangleSurface(this->m_coords[item.node(0)],
                                                               this->m_coords[item.node(1)],
                                                               this->m_coords[item.node(2)]))
          + math::abs(this->m_geom->computeTriangleSurface(this->m_coords[item.node(0)],
                                                           this->m_coords[item.node(2)],
                                                           this->m_coords[item.node(3)]));
      } else {
        area += math::abs(this->m_geom->computeArea(face)) ;
        //throw NotImplementedException(A_FUNCINFO,"Not implemented for face with more than 4 nodes");
      }
    }
}

/*---------------------------------------------------------------------------*/
