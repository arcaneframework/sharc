#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/Geometry/Temis/TemisGeometry2.h"

#include <map>

#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/SharedVariable.h>

#include "TemisGeometry2_axl.h"
#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryServiceBase.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class TemisGeometry2Service :
  public ArcaneTemisGeometry2Object,
  public GeometryServiceBase
{
public:
  /** Constructeur de la classe */
  TemisGeometry2Service(const Arcane::ServiceBuildInfo & sbi);

  /** Destructeur de la classe */
  virtual ~TemisGeometry2Service();

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
  Dimension dimension() const { return Dimension::e2Dyz; }

  //! Remove geometry reference to a given mesh
  /*! Used for deleted sub-meshes */
  void unregisterMesh(IMesh * mesh);

public:
  //@{ Extended interface for GeometryServiceBase

  //! Access to traceMng
  ITraceMng * traceMng() { return ArcaneTemisGeometry2Object::traceMng(); }

  //! Access to Mesh
  IMesh * mesh() { return ArcaneTemisGeometry2Object::mesh(); }

  //! Name of instancied class
  const char * className() const { return "TemisGeometry2"; }

  //@}

private:
  //! Shared geometry builder
  SharedTemisGeometry2 * sharedGeometry(IMesh * mesh);
  //! Uniq geometry builder
  TemisGeometry2 * uniqGeometry(IMesh * mesh);

private:
  bool m_initialized;

  typedef std::map<IMesh*,std::pair<SharedTemisGeometry2*, SharedMeshVariableScalarRefT<Node,Real3>* > > SharedGeometry2Map;
  typedef std::map<IMesh*,TemisGeometry2*> UniqGeometry2Map;
  SharedGeometry2Map m_shared_geometry;
  UniqGeometry2Map m_uniq_geometry;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/IItemFamily.h>
using namespace Arcane;

#include "ArcGeoSim/Mesh/Geometry/Temis/TemisGeometry2.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TemisGeometry2Service::
TemisGeometry2Service(const Arcane::ServiceBuildInfo & sbi) : 
  ArcaneTemisGeometry2Object(sbi),
  GeometryServiceBase(),
  m_initialized(false)
{
  ;
}

TemisGeometry2Service::
~TemisGeometry2Service()
{
  for(SharedGeometry2Map::iterator i = m_shared_geometry.begin(); i != m_shared_geometry.end(); ++i)
    {
      delete i->second.first; // SharedTemisGeometry2
      delete i->second.second; // SharedMeshVariableScalarRefT<Node,Real3>
    }

  for(UniqGeometry2Map::iterator i = m_uniq_geometry.begin(); i != m_uniq_geometry.end(); ++i)
    {
      delete i->second; // TemisGeometry2
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
TemisGeometry2Service::
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
TemisGeometry2Service::
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
TemisGeometry2Service::
reset(ItemGroup group)
{
  
}
  
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

TemisGeometry2 *
TemisGeometry2Service::
uniqGeometry(IMesh * mesh)
{
  UniqGeometry2Map::iterator finder = m_uniq_geometry.find(mesh);
  if (finder == m_uniq_geometry.end())
    { // Build new shared geometry
      UniqGeometry2Map::mapped_type & mapped = m_uniq_geometry[mesh];
      // L'implémentation actuelle se construit sur le maillage par défaut
      mapped = new TemisGeometry2(PRIMARYMESH_CAST(mesh)->nodesCoordinates());
      return mapped;
    }
  else
    {
      return finder->second;
    }
}

/*---------------------------------------------------------------------------*/

SharedTemisGeometry2 *
TemisGeometry2Service::
sharedGeometry(IMesh * mesh)
{
  SharedGeometry2Map::iterator finder = m_shared_geometry.find(mesh);
  if (finder == m_shared_geometry.end()) 
    { // Build new shared geometry
      SharedGeometry2Map::mapped_type & mapped = m_shared_geometry[mesh];
      mapped.second = new SharedMeshVariableScalarRefT<Node,Real3>(mesh->nodeFamily(),
                                                                   PRIMARYMESH_CAST(this->mesh())->nodesCoordinates());
      mapped.first = new SharedTemisGeometry2(*mapped.second);
      return mapped.first;
    }
  else
    {
      return finder->second.first;
    }
}
 
/*---------------------------------------------------------------------------*/

IGeometry *
TemisGeometry2Service::
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
TemisGeometry2Service::
unregisterMesh(IMesh * mesh)
{
  if (mesh == NULL) mesh = this->mesh();
  GeometryServiceBase::unregisterMesh(m_uniq_geometry, m_shared_geometry, mesh);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_TEMISGEOMETRY2(TemisGeometry2,TemisGeometry2Service); 

