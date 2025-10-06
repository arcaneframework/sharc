#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include <arcane/utils/Real3.h>
#include <arcane/Item.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/MeshVariable.h>
#include <arcane/VariableTypedef.h>
#include <arcane/MathUtils.h>

#include "ArcGeoSim/Mesh/Importers/IDataMeshImporter.h"
#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryServiceBase.h"
#include "StaticGeometry_axl.h"

#include "ArcGeoSim/Physics/Units/IUnitsSystem.h"
#include "ArcGeoSim/Mesh/Geometry/Euclidian/EuclidianGeometry.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class StaticGeometryService :
  public ArcaneStaticGeometryObject,
  public GeometryServiceBase,
  public ArcGeoSim::Mesh::IDataMeshImporter
{
public:
  /** Constructeur de la classe */
  StaticGeometryService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneStaticGeometryObject(sbi)
  , ArcGeoSim::Mesh::IDataMeshImporter()
  , m_geometry(NULL)
  , m_check_data_integrity(false)
  , m_units_system(NULL)
  , m_need_convert_data(false)
    {
      ;
    }

  /** Destructeur de la classe */
   ~StaticGeometryService() ;

public:

  //! update service function
  void update(ItemGroup  group);
  //! reset
  void reset(ItemGroup group) ;
  ITraceMng * traceMng() { return ArcaneStaticGeometryObject::traceMng(); }

  //! Access to Mesh
  IMesh * mesh() { return ArcaneStaticGeometryObject::mesh(); }

  //! Name of instancied class
  const char * className() const { return "StaticGeometry"; }
   void init() ;
   void update(ItemGroup group, Integer property) {};

  //! Reset property for an ItemGroup
  void reset(ItemGroup group, Integer property) {};
  void update(IGeometryPolicy * policy = NULL);
  void update(IMesh * mesh, IGeometryPolicy * policy = NULL) {
    fatal() << "Cannot update Geometry for a given mesh";
  }

  //! Reset property for all ItemGroups
  void reset() {};
  IGeometry* geometry(IMesh * mesh) {
    if (mesh==NULL or mesh==this->mesh()) return m_geometry;
    else fatal() << "Cannot manage multiple meshes";
    return NULL;
  }

  //! Get dimension associated to this geometry mng
  Dimension dimension() const { return Dimension::e3Dxyz; }

  //! Remove geometry reference to a given mesh
  /*! Used for deleted sub-meshes */
  void unregisterMesh(IMesh * mesh) {
    if (mesh == this->mesh())
      warning() << "Cannot unregister default mesh";
    else if (mesh != NULL)
      fatal() << "Cannot manage multiple meshes";
  }

  void setDataImportUnitsSystem(IUnitsSystem* system) {
    m_units_system = system ;
    if(system)
      m_need_convert_data = true ;
  }

private :
  void _convertData() ;
  void _checkDataIntegrity() ;
   void update_variable(ItemGroup & group,IGeometryProperty::eProperty property,ItemGroupGeometryProperty::StorageInfo & storage, String varName);
       //! Access to traceMng
  EuclidianGeometry::EuclidianGeometry3 * m_geometry;

 //! boolean to check imput data integrity and correct if possible
 bool m_check_data_integrity ;

 IUnitsSystem* m_units_system ;
 bool m_need_convert_data ;

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

StaticGeometryService::
~StaticGeometryService()
{
  if (m_geometry) 
    delete m_geometry;
}



void StaticGeometryService::init() {
  if (m_geometry)
    {
      info() << className() << " already initialized";
      return;
    }

  // L'implémentation actuelle se construit sur le maillage par défaut

  m_geometry = new EuclidianGeometry::EuclidianGeometry3(PRIMARYMESH_CAST(this->mesh())->nodesCoordinates());
  
  // to check and correct unfill geometric property
  m_check_data_integrity = options()->checkDataIntegrity() ;

  IServiceMng* sm = subDomain()->serviceMng();
  IAppServiceMng* app_service_mng = IAppServiceMng::instance(sm) ;
  m_units_system = app_service_mng->find<IUnitsSystem>(false) ;
  if(m_units_system)
    m_need_convert_data = true ;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* private */
void StaticGeometryService::update_variable(ItemGroup & group,IGeometryProperty::eProperty property,ItemGroupGeometryProperty::StorageInfo & storage, String varName){
  
  

 

  if (IGeometryProperty::isScalar(property)) {
    
   
    
    if (storage.storageType & IGeometryProperty::PVariable){
      
      std::shared_ptr<RealVariable> & ivar = storage.realVar;
      
      if( ivar){// on a déja une variable 
        RealVariable my_var(  VariableBuildInfo(group.mesh(),
                                                varName,
                                                group.itemFamily()->name(),
                                                IVariable::PNoRestore|IVariable::PPersistant),
                              group.itemKind());
        ENUMERATE_ITEM(iitem,group){
          (*ivar)[iitem]=my_var[iitem]; 
        }
      }
      else
        ivar.reset(new RealVariable(VariableBuildInfo(group.mesh(),
                                                      varName,
                                                      group.itemFamily()->name(),
                                                      IVariable::PNoRestore|IVariable::PPersistant),
                                    group.itemKind()));
      
    }
    
    std::shared_ptr<RealGroupMap> & imap = storage.realMap;
    if (not imap )/* pas de variable utilisateur */
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
	
        ENUMERATE_ITEM(iitem,group){
          (*imap)[iitem]=ivar[iitem]; 

        }
      }

    
  }
  else {
    ARCANE_ASSERT((IGeometryProperty::isVectorial(property)),("Vectorial property expected"));
   
    if (storage.storageType & IGeometryProperty::PVariable){
      
      std::shared_ptr<Real3Variable> & ivar = storage.real3Var;
   
    
      if( ivar){// on a déja une variable 
        Real3Variable my_var(  VariableBuildInfo(group.mesh(),
                                                 varName,
                                                 group.itemFamily()->name(),
                                                 IVariable::PNoRestore|IVariable::PPersistant),
                               group.itemKind());
        ENUMERATE_ITEM(iitem,group){
          (*ivar)[iitem]=my_var[iitem]; 
        }
      }
      else
        ivar.reset(new Real3Variable(VariableBuildInfo(group.mesh(),
                                                       varName,
                                                       group.itemFamily()->name(),
                                                       IVariable::PNoRestore|IVariable::PPersistant),
                                     group.itemKind()));
      
     
    
    }
    
    
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

        ENUMERATE_ITEM(iitem,group){
          (*imap)[iitem]=ivar[iitem]; 
        }
      }

    
  }
}


void 
StaticGeometryService::
_convertData()
{
  if(m_units_system==NULL)
    return ;
  
  FaceGroup faces = allFaces() ;
  CellGroup cells = allCells() ;
  Real3Variable face_normal(  VariableBuildInfo(faces.mesh(),
                                               "MshCooresFaceNormal",
                                               faces.itemFamily()->name(),
                                               IVariable::PNoRestore|IVariable::PPersistant) ,
                                               IK_Face);
  Real3Variable face_center(  VariableBuildInfo(faces.mesh(),
                                               "MshCooresFaceCenter",
                                               faces.itemFamily()->name(),
                                               IVariable::PNoRestore|IVariable::PPersistant) ,
                                               IK_Face);
  RealVariable face_area(  VariableBuildInfo(faces.mesh(),
                                             "MshCooresFaceArea",
                                             faces.itemFamily()->name(),
                                             IVariable::PNoRestore|IVariable::PPersistant) ,
                                             IK_Face);

  Real3Variable cell_center(  VariableBuildInfo(cells.mesh(),
                                               "MshCooresCellCenter",
                                               cells.itemFamily()->name(),
                                               IVariable::PNoRestore|IVariable::PPersistant) ,
                                               IK_Cell);
  RealVariable cell_volume(  VariableBuildInfo(cells.mesh(),
                                             "MshCooresCellVolume",
                                             cells.itemFamily()->name(),
                                             IVariable::PNoRestore|IVariable::PPersistant) ,
                                             IK_Cell);
  Real unit_X = m_units_system->getMultFactor(IUnitsSystemProperty::X) ;
  Real unit_Y = m_units_system->getMultFactor(IUnitsSystemProperty::Y) ;
  Real unit_Z = m_units_system->getMultFactor(IUnitsSystemProperty::Z) ;
  Real area_factor = unit_X*unit_Y ;
  Real volume_factor = area_factor*unit_Z ;
  ENUMERATE_FACE(iface,allFaces())
  {
   face_area[iface] *= area_factor ;
   face_normal[iface].x *= unit_X ;
   face_normal[iface].y *= unit_Y ;
   face_normal[iface].z *= unit_Z ;
   face_center[iface].x *= unit_X ;
   face_center[iface].y *= unit_Y ;
   face_center[iface].z *= unit_Z ;
  }
  ENUMERATE_CELL(icell,allCells())
  {
   cell_volume[icell] *= volume_factor ;
   cell_center[icell].x *= unit_X ;
   cell_center[icell].y *= unit_Y ;
   cell_center[icell].z *= unit_Z ;
  }
}


void 
StaticGeometryService::
_checkDataIntegrity()
{
  if(m_geometry==NULL)
    fatal()<<"Geometry is NULL, cannot check integrity";
  
  FaceGroup faces = allFaces() ;
  Real3Variable face_normal(  VariableBuildInfo(faces.mesh(),
                                               "MshCooresFaceNormal",
                                               faces.itemFamily()->name(),
                                               IVariable::PNoRestore|IVariable::PPersistant) ,
                                               IK_Face);
  Real3Variable face_center(  VariableBuildInfo(faces.mesh(),
                                               "MshCooresFaceCenter",
                                               faces.itemFamily()->name(),
                                               IVariable::PNoRestore | IVariable::PPersistant) ,
                                               IK_Face);
  RealVariable face_area(  VariableBuildInfo(faces.mesh(),
                                             "MshCooresFaceArea",
                                             faces.itemFamily()->name(),
                                             IVariable::PNoRestore|IVariable::PPersistant) ,
                                             IK_Face);
          
  ENUMERATE_FACE(iface,allFaces())
  {
    const Face& face = *iface ;
    if(face_area[iface]==0)
      {
        face_area[iface] = m_geometry->computeArea(face) ;
        face_normal[iface] = m_geometry->computeOrientedMeasure(face) ;
        face_center[iface] = m_geometry->computeCenter(face) ;
      }
  }
}

void
StaticGeometryService::
update(IGeometryPolicy * policy)
{
  if(m_need_convert_data)
    {
      _convertData() ;
      m_need_convert_data = false ;
    }
  
  if(m_check_data_integrity)
    {
      //check input data at the first step 
      _checkDataIntegrity() ;
      m_check_data_integrity = false ;
    }
  GeometryServiceBase::update(policy) ;
}

void 
StaticGeometryService::
update(ItemGroup  group)
{
  PropertyMap::iterator igroup = m_group_property_map.find(group.internal());
  if (igroup == m_group_property_map.end())
    throw FatalErrorException(A_FUNCINFO,"Undefined ItemGroup property");

  ItemGroupGeometryProperty & properties = igroup->second;


  info() << group.name();

  /* Mise a jour des données géométriques*/
  for(ItemGroupGeometryProperty::StorageInfos::iterator i = properties.storages.begin(); i != properties.storages.end(); ++i) 
    {
      IGeometryProperty::eProperty property = i->first;
      ItemGroupGeometryProperty::StorageInfo & storage = i->second;
      String varName ;

      /* on recupere les valeurs stockées dans la fonction de lecteur EMesh pour la propriété associée*/
      /* property */
      
      switch (property) {
	
      case IGeometryProperty::PVolume: {
        // verifier que c'est un groupe de cellules
        if (group.itemKind() != IK_Cell){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Volume, we must have a Cell type");
        }
        varName = String("MshCooresCellVolume");
        break;	
      }
      case IGeometryProperty::PArea: {
        // verifier que c'est un groupe de cellules
        if (group.itemKind() != IK_Face){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Area, we must have a Face type");
        }
        varName = String("MshCooresFaceArea");
        break;	
      }	
      case IGeometryProperty::PNormal: {
        // verifier que c'est un groupe de cellules
        if (group.itemKind() != IK_Face){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Normal, we must have a Face type");
        }
        varName = String("MshCooresFaceNormal");
        break;	
      }		
	
      case IGeometryProperty::PCenter: {
        // verifier que c'est un groupe de cellules ou de faces
        if ((group.itemKind() != IK_Face) and (group.itemKind() != IK_Cell)){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Center, we must have a Cell type");
        }

        if (group.itemKind() == IK_Face){
          varName = String("MshCooresFaceCenter");
        }
        if (group.itemKind() == IK_Cell){
          varName = String("MshCooresCellCenter");
        }
        break;	
      }		
	
      case IGeometryProperty::PMeasure: {
        // verifier que c'est un groupe de cellules ou de faces

        if ((group.itemKind() != IK_Face) and (group.itemKind() != IK_Cell)){
          throw FatalErrorException(A_FUNCINFO,"Wrong item type for the property Measure, we must have a Cell or Face type ");
        }
        if (group.itemKind() == IK_Face){
          varName = String("MshCooresFaceArea");
        }
        if (group.itemKind() == IK_Cell){
          varName = String("MshCooresCellVolume");
        }
        break;	
      }	
	
      default:
        throw FatalErrorException(A_FUNCINFO,"Illegal use of undefined property");
      }


     
      update_variable(group,property,storage, varName);
  

    }

  /* mise a jour des données pour les zones de non coincidence*/
  


}

/*---------------------------------------------------------------------------*/

void 
StaticGeometryService::
reset(ItemGroup group)
{
  
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_STATICGEOMETRY(StaticGeometry,StaticGeometryService);
