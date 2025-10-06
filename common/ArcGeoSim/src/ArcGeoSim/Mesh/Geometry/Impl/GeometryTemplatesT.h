#ifndef ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYTEMPLATES_H
#define ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYTEMPLATES_H

#ifndef ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYSERVICEBASET_H
#error "Ce fichier n'est destiné qu'à GeometryServiceBaseT.h"
#endif /* ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYSERVICEBASET_H */

#include <arcane/IMesh.h>
using namespace Arcane;

#include <arcane/IItemOperationByBasicType.h>

#include <arcane/IItemFamily.h>
#include <arcane/ArcaneVersion.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/MathUtils.h>
#include <arcane/ItemVectorView.h>

#include "ArcGeoSim/Mesh/Geometry/Impl/ItemGroupGeometryProperty.h"

template <typename GeometryT>
class GenericGSInternalUpdater : public IItemOperationByBasicType
{
private:
  GeometryT & m_geom;
  ITraceMng * m_trace_mng;
  ItemGroupGeometryProperty * m_group_property;
  GeometryServiceBase * m_service_base;

public:
  GenericGSInternalUpdater(GeometryT & geom, 
                           ITraceMng * traceMng,
                           ItemGroupGeometryProperty * group_property,
                           GeometryServiceBase * service_base)
    : m_geom(geom)
    , m_trace_mng(traceMng)
    , m_group_property(group_property)
    , m_service_base(service_base) { }
    
#define SAVE_PROPERTY(property,type,item,group,expr)                                                 \
  if (m_group_property->hasProperty((property)))                                                     \
    {                                                                                                \
      ItemGroupGeometryProperty::StorageInfo & storage = m_group_property->storages[property];       \
      if (ContainerAccessorT<type>::getVarContainer(storage))                                        \
        {                                                                                            \
          IGeometryMng::type##Variable & mMap = *ContainerAccessorT<type>::getVarContainer(storage); \
          ENUMERATE_ITEMWITHNODES((item), (group)) {                                                 \
            mMap[*(item)] = (expr);                                                                  \
          }                                                                                          \
        }                                                                                            \
      if (ContainerAccessorT<type>::getMapContainer(storage))                                        \
        {                                                                                            \
          IGeometryMng::type##GroupMap & mMap = *ContainerAccessorT<type>::getMapContainer(storage); \
          ENUMERATE_ITEMWITHNODES((item), (group)) {                                                 \
            mMap[*(item)] = (expr);                                                                  \
          }                                                                                          \
        }                                                                                            \
      m_service_base->setNewContext(storage);                                                        \
    }
  
  template<typename ComputeLineFunctor>
  void applyLineTemplate(ItemVectorView group)
  {
    // Utilise des tableaux locaux plutot qu'une spécialisation par type de propriété (moins de code, plus de souplesse)
    SharedArray<Real3> centers(group.size());
    SharedArray<Real3> orientations(group.size());

    ComputeLineFunctor functor(&m_geom);
    ENUMERATE_ITEMWITHNODES(item, group) {
      functor.computeOrientedMeasureAndCenter(*item,orientations[item.index()],centers[item.index()]);
    }

    SAVE_PROPERTY(IGeometryProperty::PMeasure,Real,item,group,math::normeR3(orientations[item.index()]));
    SAVE_PROPERTY(IGeometryProperty::PLength,Real,item,group,math::normeR3(orientations[item.index()]));
    SAVE_PROPERTY(IGeometryProperty::PArea,Real,item,group,0);
    SAVE_PROPERTY(IGeometryProperty::PVolume,Real,item,group,0);
    SAVE_PROPERTY(IGeometryProperty::PCenter,Real3,item,group,centers[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PNormal,Real3,item,group,orientations[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PVolumeSurfaceRatio,Real,item,group,0);
  }

  template<typename ComputeSurfaceFunctor>
  void applySurfaceTemplate(ItemVectorView group)
  {
    // Utilise des tableaux locaux plutot qu'une spécialisation par type de propriété (moins de code, plus de souplesse)
    SharedArray<Real3> centers(group.size());
    SharedArray<Real3> mass_centers(group.size());
    SharedArray<Real3> normals(group.size());
    SharedArray<Real> coords;

    ComputeSurfaceFunctor functor(&m_geom);
    ENUMERATE_ITEMWITHNODES(item, group) {
      functor.computeOrientedMeasureAndCenter(*item,normals[item.index()],centers[item.index()], mass_centers[item.index()],coords);
    }

    SAVE_PROPERTY(IGeometryProperty::PLength,Real,item,group,0);
    SAVE_PROPERTY(IGeometryProperty::PVolume,Real,item,group,0);
    SAVE_PROPERTY(IGeometryProperty::PCenter,Real3,item,group,centers[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PMassCenter,Real3,item,group,mass_centers[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PNormal,Real3,item,group,normals[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PVolumeSurfaceRatio,Real,item,group,0);

    if (m_service_base->mesh()->dimension() == 3)
      {
        SAVE_PROPERTY(IGeometryProperty::PMeasure,Real,item,group,math::normeR3(normals[item.index()]));
        SAVE_PROPERTY(IGeometryProperty::PArea,Real,item,group,math::normeR3(normals[item.index()]));
      }
    else
      {
        SAVE_PROPERTY(IGeometryProperty::PMeasure,Real,item,group,normals[item.index()][m_geom.m_2d_normal_id]);
        SAVE_PROPERTY(IGeometryProperty::PArea,Real,item,group,normals[item.index()][m_geom.m_2d_normal_id]);
      }
  }

  template<typename ComputeVolumeFunctor>
  void applyVolumeTemplate(ItemVectorView group)
  {
    // Utilise des tableaux locaux plutot qu'une spécialisation par type de propriété (moins de code, plus de souplesse)
    SharedArray<Real3> centers(group.size());
    SharedArray<Real> volumes(group.size());
    SharedArray<Real> coords;

    ComputeVolumeFunctor functor(&m_geom);
    ENUMERATE_ITEMWITHNODES(item, group) {
      functor.computeOrientedMeasureAndCenter(*item,volumes[item.index()],centers[item.index()],coords);
    }

    SAVE_PROPERTY(IGeometryProperty::PMeasure,Real,item,group,volumes[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PLength,Real,item,group,0);
    SAVE_PROPERTY(IGeometryProperty::PArea,Real,item,group,0);
    SAVE_PROPERTY(IGeometryProperty::PVolume,Real,item,group,volumes[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PCenter,Real3,item,group,centers[item.index()]);
    SAVE_PROPERTY(IGeometryProperty::PNormal,Real3,item,group,0);

    if (m_group_property->hasProperty((IGeometryProperty::PVolumeSurfaceRatio)))
      {
        SharedArray<Real> areas(group.size());
        ENUMERATE_ITEMWITHNODES(item, group) {
          functor.computeVolumeArea(*item,areas[item.index()]);
        }
        SAVE_PROPERTY(IGeometryProperty::PVolumeSurfaceRatio,Real,item,group,volumes[item.index()]/areas[item.index()]);
      }
  }

  void applyVertex(ItemVectorView group) { }

  void applyLine2(ItemVectorView group) {
    applyLineTemplate<typename GeometryT::ComputeLine2>(group);
  }

  void applyTriangle3(ItemVectorView group) {
    applySurfaceTemplate<typename GeometryT::ComputeTriangle3>(group);
  }

  void applyQuad4(ItemVectorView group) {
    applySurfaceTemplate<typename GeometryT::ComputeQuad4>(group);
  }

  void applyPentagon5(ItemVectorView group) {
    applySurfaceTemplate<typename GeometryT::ComputePentagon5>(group);
  }

  void applyHexagon6(ItemVectorView group) {
    applySurfaceTemplate<typename GeometryT::ComputeHexagon6>(group);
  }

  void applyTetraedron4(ItemVectorView group)
  {
    applyVolumeTemplate<typename GeometryT::ComputeTetraedron4>(group);
  }

  void applyPyramid5(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputePyramid5>(group);
  }

  void applyPentaedron6(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputePentaedron6>(group);
  }

  void applyHexaedron8(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeHexaedron8>(group);
  }

  void applyHeptaedron10(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeHeptaedron10>(group);
  }

  void applyOctaedron12(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeOctaedron12>(group);
  }

  void applyHemiHexa7(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeHemiHexa7>(group);
  }

  void applyHemiHexa6(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeHemiHexa6>(group);
  }

  void applyHemiHexa5(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeHemiHexa5>(group);
  }

  void applyAntiWedgeLeft6(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeAntiWedgeLeft6>(group);
  }

  void applyAntiWedgeRight6(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeAntiWedgeRight6>(group);
  }

  void applyDiTetra5(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeDiTetra5>(group);
  }

  void applyDualNode(ItemVectorView group) { }
  void applyDualEdge(ItemVectorView group) { }
  void applyDualFace(ItemVectorView group) { }
  void applyDualCell(ItemVectorView group) { }
#if (ARCANE_VERSION<11602)
  void applyLine3(ItemVectorView group) { }
  void applyLine4(ItemVectorView group) { }
  void applyLine5(ItemVectorView group) { }
  void applyLine9(ItemVectorView group) { }
#else
  void applyLink(ItemVectorView group) { }
#endif
#if (ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0))
  void applyEnneedron14(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeEnneedron14>(group);
  }
  void applyDecaedron16(ItemVectorView group) {
    applyVolumeTemplate<typename GeometryT::ComputeDecaedron16>(group);
  }
  void applyHeptagon7(ItemVectorView group) {
    applySurfaceTemplate<typename GeometryT::ComputeHeptagon7>(group);
  }
  void applyOctogon8(ItemVectorView group) {
    applySurfaceTemplate<typename GeometryT::ComputeOctogon8>(group);
  }
#endif
};

#endif /* ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYTEMPLATES_H */
