// -*- C++ -*-
#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include <arcane/utils/Real3.h>
#include <arcane/Item.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/MeshVariable.h>
#include <arcane/VariableTypedef.h>
#include <arcane/MathUtils.h>


#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "IGeom.h"

#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryServiceBase.h"
#include "PolyhedralMeshGeometry_axl.h"

#include "ArcGeoSim/Mesh/Geometry/Euclidian/EuclidianGeometry.h"
#include "ArcGeoSim/Mesh/Geometry/PolytopicMesh/Geom3D/PolyhedralGeometry.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class PolyhedralMeshGeometryService 
  : public ArcanePolyhedralMeshGeometryObject,
    public GeometryServiceBase
{
public:
  
  /** Constructeur de la classe */
  PolyhedralMeshGeometryService(const Arcane::ServiceBuildInfo & sbi) 
    : ArcanePolyhedralMeshGeometryObject(sbi)
    , m_geometry(NULL) {;}
  
  /** Destructeur de la classe */
  ~PolyhedralMeshGeometryService();
  
public:

//! Get ITraceMng*
ITraceMng * traceMng() { return ArcanePolyhedralMeshGeometryObject::traceMng(); }

//! Get IMesh*
IMesh * mesh() { return ArcanePolyhedralMeshGeometryObject::mesh(); }

//! Get Name of instancied class
const char * className() const { return "PolyhedralMeshGeometry"; }

public:

//! Init
void init() ;

//! Update geometry
void update(IGeometryPolicy * policy = NULL) {
    GeometryServiceBase::update(policy);
}

//! Update all properties for a group
void update(ItemGroup  group);

//! Update geometry for a given mesh
void update(IMesh * mesh, IGeometryPolicy * policy = NULL) {
    GeometryServiceBase::update(mesh, policy);
  }

//! Reset all properties for a group
void reset(ItemGroup group) {

}

//! Get IGeometry*
IGeometry* geometry(IMesh* mesh) {
    if (mesh==NULL or mesh==this->mesh()) return m_geometry;
    else fatal() << "Cannot manage multiple meshes";
    return NULL;
  }

//! Get dimension
Dimension dimension() const {
return Dimension::e3Dxyz;
}

//! Remove geometry reference to a given mesh to delete sub-meshes
void unregisterMesh(IMesh * mesh) {
    if (mesh == this->mesh())
      warning() << "Cannot unregister default mesh";
    else if (mesh != NULL)
      fatal() << "Cannot manage multiple meshes";
}

private :

//! Base PolyhedralGeometry Geometry
PolyhedralGeometry * m_geometry;

//! Base  Geometry Service
IGeom * m_geom_service;

private :

void _updateGeometry();

//! Get varName assocaited to a property and group
String _getPropertyVarName(IGeometryProperty::eProperty property, ItemGroup group);

//! Update property with the content of a named variable
void _update_property_from_variable(
ItemGroup & group,
IGeometryProperty::eProperty property,
ItemGroupGeometryProperty::StorageInfo & storage,
String varName);

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/IMeshSubMeshTransition.h>
#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryTemplatesT.h"
#include <arcane/IVariableMng.h>
#include "ArcGeoSim/Appli/IAppServiceMng.h"

using namespace Arcane;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

PolyhedralMeshGeometryService::
~PolyhedralMeshGeometryService()
{
  if (m_geometry)
  {
    delete m_geometry;
  }
}

/*---------------------------------------------------------------------------*/

void
PolyhedralMeshGeometryService::
init() {

  if (m_geometry)
    {
       info() << className() << " already initialized";
       return;
    }

    // Use PolyhedralGeometry as basic geometry computation
    m_geometry = new PolyhedralGeometry(mesh()->nodesCoordinates(), mesh()->dimension());

    m_geom_service = options()->geomService();
    m_geom_service->init();

    _updateGeometry();
}

/*---------------------------------------------------------------------------*/

void
PolyhedralMeshGeometryService::
_updateGeometry() {

    // Compute Properties for a New Mesh
    m_geom_service->apply();
}

/*---------------------------------------------------------------------------*/

void
PolyhedralMeshGeometryService::
update(ItemGroup group)
{
    PropertyMap::iterator igroup = m_group_property_map.find(group.internal());

    if (igroup == m_group_property_map.end())
      throw FatalErrorException(A_FUNCINFO,"Undefined ItemGroup property");

    ItemGroupGeometryProperty & properties = igroup->second;

    info() << group.name();

    _updateGeometry();

     // Mise a jour des données géométriques
    for(ItemGroupGeometryProperty::StorageInfos::iterator i = properties.storages.begin(); i != properties.storages.end(); ++i)
      {
        IGeometryProperty::eProperty property = i->first;
        ItemGroupGeometryProperty::StorageInfo & storage = i->second;

        String varName = _getPropertyVarName(property, group);
        _update_property_from_variable(group,property,storage, varName);
      }
}

/*---------------------------------------------------------------------------*/

String
PolyhedralMeshGeometryService::
_getPropertyVarName(IGeometryProperty::eProperty property, ItemGroup group)
  {
      String varName;

      switch (property) {

      // Property = Cell Volume
      case IGeometryProperty::PVolume: {
        if (group.itemKind() != IK_Cell){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Volume, we must have a Cell type");
        }
        varName = String("Geom3DCellMeasure");
        break;
      }

      // Property = Face Area
      case IGeometryProperty::PArea: {
        // verifier que c'est un groupe de cellules
        if (group.itemKind() != IK_Face){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Area, we must have a Face type");
        }
        varName = String("Geom3DFaceMeasure");
        break;
      }

     // Property = Face Normal
     case IGeometryProperty::PNormal: {
        // verifier que c'est un groupe de cellules
        if (group.itemKind() != IK_Face){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Normal, we must have a Face type");
        }
        varName = String("Geom3DFaceNormalFlux");
        break;
      }

     // Property = Face Center | Cell Center
     case IGeometryProperty::PCenter: {
        if ((group.itemKind() != IK_Face) and (group.itemKind() != IK_Cell)){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Center, we must have a Cell type");
        }

        if (group.itemKind() == IK_Face){
          varName = String("Geom3DFaceBarycenter");
        }
        if (group.itemKind() == IK_Cell){
          varName = String("Geom3DCellBarycenter");
        }
        break;
      }

     // Property = Face Measure | Cell Measure
     case IGeometryProperty::PMeasure: {
        if ((group.itemKind() != IK_Face) and (group.itemKind() != IK_Cell)){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Measure, we must have a Cell or Face type ");
        }
        if (group.itemKind() == IK_Face){
          varName = String("Geom3DFaceMeasure");
        }
        if (group.itemKind() == IK_Cell){
          varName = String("Geom3DCellMeasure");
        }
        break;
      }

     // Invalid Property
     default:
        throw FatalErrorException(A_FUNCINFO,"Illegal use of undefined property");
      }

     return varName;
}

/*---------------------------------------------------------------------------*/

void
PolyhedralMeshGeometryService::
_update_property_from_variable(ItemGroup & group,IGeometryProperty::eProperty property,
ItemGroupGeometryProperty::StorageInfo & storage, String varName)
{
  if (IGeometryProperty::isScalar(property))
  //======= Scalar Properties
  {
    if (storage.storageType & IGeometryProperty::PVariable)
    {
      // Mesh Variable
      std::shared_ptr<RealVariable> & ivar = storage.realVar;
      if (ivar)
      {
        // Copy the variable content
        RealVariable my_var(VariableBuildInfo(group.mesh(),
                                              varName,
                                              group.itemFamily()->name(),
                                              IVariable::PNoRestore|IVariable::PPersistant),
                                              group.itemKind());
        ENUMERATE_ITEM(item, group)
        {
          (*ivar)[item] = my_var[item];
        }
      }
      else
      {
        ivar.reset(new RealVariable(VariableBuildInfo(group.mesh(),
                                                      varName,
                                                      group.itemFamily()->name(),
                                                      IVariable::PNoRestore|IVariable::PPersistant),
                                                      group.itemKind()));
       }
    }

    // ItemGroupMap Variable
    std::shared_ptr<RealGroupMap> & imap = storage.realMap;
    if (not imap )
      {
        if (storage.storageType & IGeometryProperty::PItemGroupMap)
          imap.reset(new RealGroupMap(group));
      }

    if (storage.storageType & IGeometryProperty::PItemGroupMap)
      {
       RealVariable ivar(VariableBuildInfo(group.mesh(),
                                            varName,
                                            group.itemFamily()->name(),
                                            IVariable::PNoRestore|IVariable::PPersistant),
                                            group.itemKind());
        ENUMERATE_ITEM(item,group){
          (*imap)[item] = ivar[item];
        }
      }
  }

  else
  //======= Vectorial Properties
  {
      ARCANE_ASSERT((IGeometryProperty::isVectorial(property)),("Vectorial property expected"));

    if (storage.storageType & IGeometryProperty::PVariable){

      // Mesh Variable
      std::shared_ptr<Real3Variable> & ivar = storage.real3Var;
      if (ivar) {
        Real3Variable my_var(  VariableBuildInfo(group.mesh(),
                                                 varName,
                                                 group.itemFamily()->name(),
                                                 IVariable::PNoRestore|IVariable::PPersistant),
                                                 group.itemKind());
        ENUMERATE_ITEM(item,group){
          (*ivar)[item] = my_var[item];
        }
      }
      else
      {
         ivar.reset(new Real3Variable(VariableBuildInfo(group.mesh(),
                                                       varName,
                                                       group.itemFamily()->name(),
                                                       IVariable::PNoRestore|IVariable::PPersistant),
                                                       group.itemKind()));
      }
    }

    // ItemGroupMap Variable
    std::shared_ptr<Real3GroupMap> & imap = storage.real3Map;
    if (not imap )
      {
        if (storage.storageType & IGeometryProperty::PItemGroupMap)
          imap.reset(new Real3GroupMap(group));
      }

    if (storage.storageType & IGeometryProperty::PItemGroupMap)
      {
        Real3Variable ivar(VariableBuildInfo(group.mesh(),
                                             varName,
                                             group.itemFamily()->name(),
                                             IVariable::PNoRestore|IVariable::PPersistant),
                                             group.itemKind());
        ENUMERATE_ITEM(item,group){
          (*imap)[item] = ivar[item];
        }
      }
  }
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_POLYHEDRALMESHGEOMETRY(PolyhedralMeshGeometry,PolyhedralMeshGeometryService);
