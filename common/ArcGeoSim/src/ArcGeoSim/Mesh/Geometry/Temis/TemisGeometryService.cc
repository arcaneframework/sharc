#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/Geometry/Temis/TemisGeometry.h"

#include <map>

#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/SharedVariable.h>

#include "TemisGeometry_axl.h"
#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryServiceBase.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class TemisGeometryService :
  public ArcaneTemisGeometryObject,
  public GeometryServiceBase
{
public:
  /** Constructeur de la classe */
  TemisGeometryService(const Arcane::ServiceBuildInfo & sbi);

  /** Destructeur de la classe */
  virtual ~TemisGeometryService();

public:

  //! Initialisation
  void init();

  //@{ @name property management by group

    //! Update property values for all ItemGroups on all meshes
  /*! C++ requieres explicit redefinition since update method is overloaded here */
  void update(IGeometryPolicy * policy = NULL) { return GeometryServiceBase::update(policy); }

  //! Update property values for all ItemGroups on given mesh
  /*! C++ requieres explicit redefinition since update method is overloaded here */
  void update(IMesh * mesh, IGeometryPolicy * policy = NULL) { return GeometryServiceBase::update(mesh, policy); }

  //! Update property values for an ItemGroup
  void update(ItemGroup group);

  //! Reset property for all ItemGroups
  /*! C++ requieres explicit redefinition since reset method is overloaded here */
  void reset() { return GeometryServiceBase::reset(); }

  //! Reset property for an ItemGroup
  void reset(ItemGroup group);

  //@}

  //! Get underlying geometry
  IGeometry * geometry(IMesh * mesh = NULL);

  //! Get dimension associated to this geometry mng
  Dimension dimension() const { return Dimension::e3Dxyz; }

  //! Remove geometry reference to a given mesh
  /*! Used for deleted sub-meshes */
  void unregisterMesh(IMesh * mesh);

public:
  //@{ Extended interface for GeometryServiceBase

  //! Access to traceMng
  ITraceMng * traceMng() { return ArcaneTemisGeometryObject::traceMng(); }

  //! Access to Mesh
  IMesh * mesh() { return ArcaneTemisGeometryObject::mesh(); }

  //! Name of instancied class
  const char * className() const { return "TemisGeometry"; }

  //@}

private:
  //! Shared geometry builder
  SharedTemisGeometry * sharedGeometry(IMesh * mesh);
  //! Uniq geometry builder
  TemisGeometry * uniqGeometry(IMesh * mesh);

private:
  bool m_initialized;

  typedef std::map<IMesh*,std::pair<SharedTemisGeometry*, SharedMeshVariableScalarRefT<Node,Real3>* > > SharedGeometryMap;
  typedef std::map<IMesh*,TemisGeometry*> UniqGeometryMap;
  SharedGeometryMap m_shared_geometry;
  UniqGeometryMap m_uniq_geometry;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/IItemFamily.h>
using namespace Arcane;

#include "ArcGeoSim/Mesh/Geometry/Temis/TemisGeometry.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TemisGeometryService::
TemisGeometryService(const Arcane::ServiceBuildInfo & sbi) : 
  ArcaneTemisGeometryObject(sbi),
  GeometryServiceBase(),
  m_initialized(false)
{
  ;
}

TemisGeometryService::
~TemisGeometryService()
{
  for(SharedGeometryMap::iterator i = m_shared_geometry.begin(); i != m_shared_geometry.end(); ++i)
    {
      delete i->second.first; // SharedTemisGeometry
      delete i->second.second; // SharedMeshVariableScalarRefT<Node,Real3>
    }

  for(UniqGeometryMap::iterator i = m_uniq_geometry.begin(); i != m_uniq_geometry.end(); ++i)
    {
      delete i->second; // TemisGeometry
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
TemisGeometryService::
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
TemisGeometryService::
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
TemisGeometryService::
reset(ItemGroup group)
{
  
}
  
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

TemisGeometry *
TemisGeometryService::
uniqGeometry(IMesh * mesh)
{
  UniqGeometryMap::iterator finder = m_uniq_geometry.find(mesh);
  if (finder == m_uniq_geometry.end())
    { // Build new shared geometry
      UniqGeometryMap::mapped_type & mapped = m_uniq_geometry[mesh];
      // L'implémentation actuelle se construit sur le maillage par défaut
      mapped = new TemisGeometry(PRIMARYMESH_CAST(mesh)->nodesCoordinates());
      return mapped;
    }
  else
    {
      return finder->second;
    }
}

/*---------------------------------------------------------------------------*/

SharedTemisGeometry *
TemisGeometryService::
sharedGeometry(IMesh * mesh)
{
  SharedGeometryMap::iterator finder = m_shared_geometry.find(mesh);
  if (finder == m_shared_geometry.end()) 
    { // Build new shared geometry
      SharedGeometryMap::mapped_type & mapped = m_shared_geometry[mesh];
      mapped.second = new SharedMeshVariableScalarRefT<Node,Real3>(mesh->nodeFamily(),
                                                                   PRIMARYMESH_CAST(this->mesh())->nodesCoordinates());
      mapped.first = new SharedTemisGeometry(*mapped.second);
      return mapped.first;
    }
  else
    {
      return finder->second.first;
    }
}
 
/*---------------------------------------------------------------------------*/

IGeometry *
TemisGeometryService::
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
TemisGeometryService::
unregisterMesh(IMesh * mesh)
{
  if (mesh == NULL) mesh = this->mesh();
  GeometryServiceBase::unregisterMesh(m_uniq_geometry, m_shared_geometry, mesh);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_TEMISGEOMETRY(TemisGeometry,TemisGeometryService);
