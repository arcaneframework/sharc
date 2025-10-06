#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometry2DMng.h"
#include "ArcGeoSim/Mesh/Geometry/Euclidian/EuclidianGeometry.h"

#include <map>

#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/SharedVariable.h>

#include "Euclidian2Geometry_axl.h"
#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryServiceBase.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Euclidian2GeometryService :
  public ArcaneEuclidian2GeometryObject,
  public GeometryServiceBase,
  public IGeometry2DMng
{
public:
  typedef GeometryServiceBase BaseType ;

  /** Constructeur de la classe */
  Euclidian2GeometryService(const Arcane::ServiceBuildInfo & sbi, IGeometry2DMng::eType type=IGeometry2DMng::YZ);

  /** Destructeur de la classe */
  virtual ~Euclidian2GeometryService();

public:
    //! Initialisation
  void init();

  //@{ @name property management by group

  //! Update property values for all ItemGroups on all meshes
  void update(IGeometryPolicy * policy = NULL);

  //! Update property values for all ItemGroups on given mesh
  void update(IMesh * mesh, IGeometryPolicy * policy = NULL);

  //! Update property values for an ItemGroup
  void update(ItemGroup group);

  //! Reset property for an ItemGroup
  void reset(ItemGroup group);

  //@}

  void addItemGroupProperty(ItemGroup group, Integer property, Integer storage) ;

  //! Get geometric property values for a Real2 field
  const Real2Variable & getReal2VariableProperty(ItemGroup group, IGeometryProperty::eProperty property) ;

  //! Get geometric property values for a Real3 field
  const Real2GroupMap & getReal2GroupMapProperty(ItemGroup group, IGeometryProperty::eProperty property) ;

  //! Get underlying geometry
  IGeometry * geometry(IMesh * mesh = NULL);

  //! Get dimension associated to this geometry mng
  Dimension dimension() const {
	switch (m_type) {
	case IGeometry2DMng::XY :
		return Dimension::e2Dxy;
    case IGeometry2DMng::YZ :
		return Dimension::e2Dyz;
    case IGeometry2DMng::XZ :
		return Dimension::e2Dxz;
    default:
    	return Dimension::eUndefined;
	}
  }

  //! Remove geometry reference to a given mesh
  /*! Used for deleted sub-meshes */
  void unregisterMesh(IMesh * mesh);

public:
  //@{ Extended interface for GeometryServiceBase

  //! Access to traceMng
  ITraceMng * traceMng() { return ArcaneEuclidian2GeometryObject::traceMng(); }

  //! Access to Mesh
  IMesh * mesh() { return ArcaneEuclidian2GeometryObject::mesh(); }

  //! Name of instancied class
  const char * className() const { return "Euclidian2Geometry"; }

  //@}

private:

  void _update2D(CellGroup const& group) ;

  //! Shared geometry builder
  EuclidianGeometry::SharedEuclidianGeometry3 * sharedGeometry(IMesh * mesh);
  //! Uniq geometry builder
  EuclidianGeometry::EuclidianGeometry3 * uniqGeometry(IMesh * mesh);

private:
  IGeometry2DMng::eType m_type;
  Integer m_2d_normal_id ;
  bool m_initialized;

  typedef std::map<IMesh*,std::pair<EuclidianGeometry::SharedEuclidianGeometry3*, SharedMeshVariableScalarRefT<Node,Real3>* > > SharedGeometryMap;
  typedef std::map<IMesh*,EuclidianGeometry::EuclidianGeometry3*> UniqGeometryMap;
  SharedGeometryMap m_shared_geometry;
  UniqGeometryMap m_uniq_geometry;
};

template<IGeometry2DMng::eType type>
class Euclidian2TGeometryService : public Euclidian2GeometryService
{
public :
  Euclidian2TGeometryService(const Arcane::ServiceBuildInfo & sbi)
  : Euclidian2GeometryService(sbi,type) {}
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/IItemFamily.h>
using namespace Arcane;

/* Few words about possible optimisations:
 * Next possible optimizations (when necessary) are:
 * - Compute common properties on maximal group 
 *   (instead of re-computing the same property on all groups with shared items)
 * - Different properties may use the same computation. Then, maximal group factorisation
 *   may consider by a system of "what properties may compute this kernel" even if not
 *   shared by alls group in a maximal set.
 * - Mandatory storage parameter in addItemGroupProperty (up to now, this is optional)
 * - Geometry computations by constraints
 *   ex: for volume computations, we need face center on Quad4. This center is computed twice 
 *       for each shared face. Moreover, this computation may be also wished by the used.
 *  The idea is to define requirements for each geometry computations by kind and type.
 *  Then computations are done following the order of the weakest constraints.
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Euclidian2GeometryService::
Euclidian2GeometryService(const Arcane::ServiceBuildInfo & sbi,IGeometry2DMng::eType type) :
  ArcaneEuclidian2GeometryObject(sbi)
, GeometryServiceBase()
, m_initialized(false)
{
  m_type = type;
  switch(type)
  {
    case IGeometry2DMng::XY :
      m_2d_normal_id = 2;
      break ;
    case IGeometry2DMng::YZ :
      m_2d_normal_id = 0;
      break ;
    case IGeometry2DMng::XZ :
      m_2d_normal_id = 1;
      break ;
    default :
      m_2d_normal_id = 0;
      break ;
  };
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Euclidian2GeometryService::
~Euclidian2GeometryService()
{
  for(SharedGeometryMap::iterator i = m_shared_geometry.begin(); i != m_shared_geometry.end(); ++i)
    {
      delete i->second.first; // SharedEuclidianGeometry3
      delete i->second.second; // SharedMeshVariableScalarRefT<Node,Real3>
    }

  for(UniqGeometryMap::iterator i = m_uniq_geometry.begin(); i != m_uniq_geometry.end(); ++i)
    {
      delete i->second; // EuclidianGeometry3
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
Euclidian2GeometryService::
init()
{
  if (m_initialized)
    {
      info() << className() << " already initialized";
      return;
    }

  if (options()->variableSuffix.size() > 0)
    {
      setSuffix(options()->variableSuffix[0]);
    }

  m_initialized = true;
}

/*---------------------------------------------------------------------------*/

void 
Euclidian2GeometryService::
update(ItemGroup group)
{
  IMesh * mesh = group.mesh();
  const bool isUniqMesh = (mesh->parentMesh() == NULL);
  
  if (isUniqMesh)
    updateGroup(group,*uniqGeometry(mesh));
  else
    updateGroup(group,*sharedGeometry(mesh));

}

void 
Euclidian2GeometryService::
reset(ItemGroup group)
{
  
}
  
/*---------------------------------------------------------------------------*/

EuclidianGeometry::EuclidianGeometry3 *
Euclidian2GeometryService::
uniqGeometry(IMesh * mesh)
{
  UniqGeometryMap::iterator finder = m_uniq_geometry.find(mesh);
  if (finder == m_uniq_geometry.end()) 
    { // Build new shared geometry
      UniqGeometryMap::mapped_type & mapped = m_uniq_geometry[mesh];
      // L'implémentation actuelle se construit sur le maillage par défaut
      mapped = new EuclidianGeometry::EuclidianGeometry3(PRIMARYMESH_CAST(mesh)->nodesCoordinates(), traceMng(), options()->fatalOnError(),m_2d_normal_id);
      return mapped;
    }
  else
    {
      return finder->second;
    }
}

/*---------------------------------------------------------------------------*/

EuclidianGeometry::SharedEuclidianGeometry3 *
Euclidian2GeometryService::
sharedGeometry(IMesh * mesh)
{
  SharedGeometryMap::iterator finder = m_shared_geometry.find(mesh);
  if (finder == m_shared_geometry.end()) 
    { // Build new shared geometry
      SharedGeometryMap::mapped_type & mapped = m_shared_geometry[mesh];
      mapped.second = new SharedMeshVariableScalarRefT<Node,Real3>(mesh->nodeFamily(),
                                                                   PRIMARYMESH_CAST(this->mesh())->nodesCoordinates());
      mapped.first = new EuclidianGeometry::SharedEuclidianGeometry3(*mapped.second, traceMng(), options()->fatalOnError());
      return mapped.first;
    }
  else
    {
      return finder->second.first;
    }
}
 
/*---------------------------------------------------------------------------*/

IGeometry *
Euclidian2GeometryService::
geometry(IMesh * mesh)
{
  // Utilise le maillage par défaut si aucun maillage n'est fourni
  if (mesh == NULL) mesh = this->mesh();

  const bool isUniqMesh = (mesh->parentMesh() == NULL);

  if (isUniqMesh)
    return uniqGeometry(mesh);
  else
    return sharedGeometry(mesh);
}

/*---------------------------------------------------------------------------*/

void
Euclidian2GeometryService::
unregisterMesh(IMesh * mesh)
{
  if (mesh == NULL) mesh = this->mesh();
  GeometryServiceBase::unregisterMesh(m_uniq_geometry, m_shared_geometry, mesh);
}

/*---------------------------------------------------------------------------*/

void
Euclidian2GeometryService::
_update2D(CellGroup const& group)
{
  VariableNodeReal3& coords = group.mesh()->nodesCoordinates() ;
  ENUMERATE_CELL(icell,group)
  {
    Real3 center ;
    for(NodeEnumerator inode(icell->nodes());inode();++inode)
    {
      center += coords[inode] ;
    }
    center /= icell->nbNode() ;
    m_cell_center[icell] = Real2(center.x,center.y) ;
  }

  Real3 k(0,0,1) ;
  ENUMERATE_FACE(iface,group.faceGroup())
  {
    Real3 center ;
    for(NodeEnumerator inode(iface->nodes());inode();++inode)
    {
      center += coords[inode] ;
    }
    center /= iface->nbNode() ;
    m_face_center[iface] = Real2(center.x,center.y) ;
    {
      NodeEnumerator inode(iface->nodes()) ;
      Real3 u = coords[iface->node(1)] - coords[iface->node(0)] ;
      Real3 n = math::vecMul(k,u) ;
      Real2 n2(n.x,n.y) ;

      // compute normal sign :
      // sgn = 1 if n is from back cell to front cell
      // sgn = -1 otherwise
      Integer sign = 1 ;
      if(iface->isSubDomainBoundary())
      {
        Real2 cf = m_face_center[iface] - m_cell_center[iface->boundaryCell()] ;
        Real cf_dot_n = math::scaMul(cf,n2) ;
        if(iface->isSubDomainBoundaryOutside())
        {
          if(cf_dot_n<0)
            sign = -1 ;
        }
        else
        {
          if(cf_dot_n>0)
            sign = -1 ;
        }
      }
      else
      {
        Real2 cf = m_face_center[iface] - m_cell_center[iface->backCell()] ;
        Real cf_dot_n = math::scaMul(cf,n2) ;
        if(cf_dot_n<0)
            sign = -1 ;
      }
      m_face_normal[iface] = Real2(sign*n.x,sign*n.y) ;
    }
  }
}


void
Euclidian2GeometryService::
update(IGeometryPolicy * policy) {
  BaseType::update(policy) ;
  _update2D(allCells()) ;
}

//! Update property values for all ItemGroups on given mesh
void
Euclidian2GeometryService::
update(IMesh * mesh, IGeometryPolicy * policy) {
  BaseType::update(mesh,policy) ;
  _update2D(mesh->allCells()) ;
}

void
Euclidian2GeometryService::
addItemGroupProperty(ItemGroup group, Integer property, Integer storage)
{
  switch(property)
  {
    case IGeometryProperty::PCenter :
    case IGeometryProperty::PNormal :
      info()<<"add 2D property : "<<property<<" to update";
      break ;
    default :
      info()<<"add 3D property : "<<property<<" to update";
      BaseType::addItemGroupProperty(group,property,storage) ;
  }
}


//! Get geometric property values for a Real2 field
const Euclidian2GeometryService::Real2Variable &
Euclidian2GeometryService::
getReal2VariableProperty(ItemGroup group, IGeometryProperty::eProperty property)
{

  switch(property)
  {
    case IGeometryProperty::PCenter :
      switch(group.itemKind())
      {
        case IK_Cell :
          return m_cell_center ;
        case IK_Face :
          return m_face_center ;
        default :
          throw FatalErrorException(A_FUNCINFO, "Group item type error");
          break ;
      }
    case IGeometryProperty::PNormal :
      return m_face_normal ;
    default :
      throw FatalErrorException(A_FUNCINFO, "Property type error");
  }
}

const Euclidian2GeometryService::Real2GroupMap &
Euclidian2GeometryService::
getReal2GroupMapProperty(ItemGroup group, IGeometryProperty::eProperty property)
{
  throw NotImplementedException(A_FUNCINFO, "getReal2GroupMapProperty not yet implemented");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef Euclidian2TGeometryService<IGeometry2DMng::XY> Euclidian2XYGeometryService ;
typedef Euclidian2TGeometryService<IGeometry2DMng::YZ> Euclidian2YZGeometryService ;
typedef Euclidian2TGeometryService<IGeometry2DMng::XZ> Euclidian2XZGeometryService ;

ARCANE_REGISTER_SERVICE_EUCLIDIAN2GEOMETRY(Euclidian2Geometry,Euclidian2GeometryService);
ARCANE_REGISTER_SERVICE_EUCLIDIAN2GEOMETRY(Euclidian2XYGeometry,Euclidian2XYGeometryService);
ARCANE_REGISTER_SERVICE_EUCLIDIAN2GEOMETRY(Euclidian2YZGeometry,Euclidian2YZGeometryService);
ARCANE_REGISTER_SERVICE_EUCLIDIAN2GEOMETRY(Euclidian2XZGeometry,Euclidian2XZGeometryService);
