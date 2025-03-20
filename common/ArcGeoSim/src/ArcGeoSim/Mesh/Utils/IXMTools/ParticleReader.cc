#include <arcane/IParallelMng.h>
#include "ParticleReader.h"




#include <arcane/MeshItemInternalList.h>
#include "arcane/VariableTypes.h"
#include "arcane/mesh/ParticleFamily.h"
/*---------------------------------------------------------------------------*/

ArcGeoSim::ParticleReader::
ParticleReader(IMesh* mesh,
               XmlNode mesh_data,
               Arcane::IParallelMng* parallel_mng,
               Arcane::ITraceMng* trace_mng,
               const Arcane::String& directory_name)
: m_mesh(mesh)
, m_mesh_data(mesh_data)
, m_parallel_mng(parallel_mng)
, m_trace_mng(trace_mng)
, m_particle_coords(NULL)
, m_directory_name(directory_name)
{
}

/*---------------------------------------------------------------------------*/

void 
ArcGeoSim::ParticleReader::
collectiveRead()
{
  // your code here
  bool is_master = true ;
  if(m_parallel_mng && m_parallel_mng->commRank()!=m_parallel_mng->masterIORank())
    is_master = false ;
  if(is_master)
  {
    ARCANE_ASSERT((!m_mesh_data.null()), ("No mesh file loaded in IXM V4 reader."))
    XmlNode rootNode = m_mesh_data.child("topology");
    Arcane::XmlNode particles_node = rootNode.child("particles");
    Integer use_particles = particles_node.null()?0:1 ;
    if(m_parallel_mng)
      m_parallel_mng->broadcast(ArrayView<Integer>(1,&use_particles),m_parallel_mng->masterIORank()) ;
    if(use_particles==0) return ;
    // Get managers
    ItemTypeMng * typeMng = ItemTypeMng::singleton();
    { // Handle created particles
      _readParticleInfos(rootNode.children("particles"), "particles", typeMng);
    }
  }
  else
  {
    Integer use_particles = 0 ;
    m_parallel_mng->broadcast(ArrayView<Integer>(1,&use_particles),m_parallel_mng->masterIORank()) ;
    if(use_particles!=0)
    {
      //Create DefaultParticleFamily
      IItemFamily* myFam = m_mesh->findItemFamily(IK_Particle, "Particle", false);
      if(myFam==NULL)
      {
#if ARCANE_VERSION >= 12201
        myFam = m_mesh->createItemFamily(IK_Particle, mesh::ParticleFamily::defaultFamilyName()) ;
        IParticleFamily* particle_family = dynamic_cast<IParticleFamily*>(myFam) ;
        particle_family->setEnableGhostItems(true) ;
#else
        myFam = m_mesh->createItemFamily(IK_Particle, "Particle") ;
#endif
      }
      myFam->endUpdate();
      //Create ParticleCoords variable
      // ATT: on ne peut ajouter les coords qu'apres la creation des particles
      m_particle_coords = new VariableParticleReal3(VariableBuildInfo(m_mesh, "ParticleCoord", "Particle"));
      m_particle_coords->setUsed(true);
    }
  }
}
/*---------------------------------------------------------------------------*/

void 
ArcGeoSim::ParticleReader::
_readParticleInfos(
    const XmlNodeList& particle_list_node,
    const String& block_name,
    ItemTypeMng * typeMng)
{
  // Error messages
  String error_particule_ids = Arcane::String::format("Error while reading {0} particle-ids", block_name);
  String error_types = Arcane::String::format("Error while reading {0} particle-types", block_name);
  String error_cell_ids = Arcane::String::format("Error while reading {0} cell-ids", block_name);
  String error_coords = Arcane::String::format("Error while reading {0} particle-coords", block_name);

  // The particles family
  IItemFamily* myFam = m_mesh->findItemFamily(IK_Particle, "Particle", false);
  if(myFam==NULL)
  {
#if ARCANE_VERSION >= 12201
    myFam = m_mesh->createItemFamily(IK_Particle, mesh::ParticleFamily::defaultFamilyName()) ;
    IParticleFamily* particle_family = dynamic_cast<IParticleFamily*>(myFam) ;
    particle_family->setEnableGhostItems(true) ;
#else
    myFam = m_mesh->createItemFamily(IK_Particle, "Particle") ;
#endif
  }

  // Temporary variables
  Cell myCell;

  Int32SharedArray all_particle_lids;
  all_particle_lids.reserve(10);
  Real3SharedArray all_xyz;
  all_xyz.reserve(10);
  // Read each block of data
  for (XmlNodeList::const_iterator iPartList = particle_list_node.begin(); iPartList != particle_list_node.end();
      ++iPartList)
  {
    // Reading particles variables
    Int64SharedArray part_ids = IXMInt64DataNodeReader(iPartList->child("particle-ids"), error_particule_ids, m_directory_name).read();
    IntegerSharedArray part_type_ids = IXMIntegerDataNodeReader(iPartList->child("particle-types"), error_types, m_directory_name).read();
    Int64SharedArray cell_ids = IXMInt64DataNodeReader(iPartList->child("cell-ids"), error_cell_ids, m_directory_name).read();

    Real3SharedArray xyz = IXMReal3DataNodeReader(iPartList->child("particle-coords"), error_coords, m_directory_name).read();
    all_xyz.addRange(xyz);

    Int32SharedArray particle_lids(part_ids.size());
    ParticleVectorView part_vector_view = myFam->toParticleFamily()->addParticles(part_ids, particle_lids);

    all_particle_lids.addRange(particle_lids);

    Int32 temp_index = 0;
    MeshItemInternalList* items = m_mesh->meshItemInternalList();

    Arcane::Int32SharedArray cell_lids(cell_ids.size());
    m_mesh->cellFamily()->itemsUniqueIdToLocalId(cell_lids, cell_ids, true);

    // Setting the associated cell for a particle if exist
    for (Int32 i = 0; i < part_type_ids.size(); i++)
    {
      if (part_type_ids[i] > 0)
      {
        myCell = items->cells[cell_lids[temp_index]];

        myFam->toParticleFamily()->setParticleCell(part_vector_view[i], myCell);
        temp_index += part_type_ids[i];
      }
      else
      {
        myFam->toParticleFamily()->setParticleCell(part_vector_view[i], Cell());
        temp_index++;
      }
    }

  }

  // End update for the family
  myFam->endUpdate();

  // ATT: on ne peut ajouter les coords qu'apres la creation des particles
  m_particle_coords = new VariableParticleReal3(VariableBuildInfo(m_mesh, "ParticleCoord", "Particle"));
  m_particle_coords->setUsed(true);
  mesh::ParticleFamily* true_particle_family = dynamic_cast<mesh::ParticleFamily*>(m_mesh->findItemFamily("Particle"));
  ItemInternalList particles(true_particle_family->itemsInternal());
  for (Integer i = 0, is = all_particle_lids.size(); i < is; i++)
  {
    (*m_particle_coords)[particles[all_particle_lids[i]]] = all_xyz[i];
  }
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
