#include "IXMV4_1MeshReader.h"
/* Author : crabiet at Thur Dec 01 11:40:13 2020
 */

#ifdef WIN32
#include <ciso646>
#endif

#include <boost/tuple/tuple.hpp> // Use std::tuple instead in C++11

#include <arcane/utils/OStringStream.h>
#include <arcane/DomUtils.h>

#include "ArcGeoSim/Mesh/Utils/IXMTools/ixm_v4_1_xsd.h"
#include "ArcGeoSim/Mesh/Utils/StringToItemUtils.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/ParticleReader.h"
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace ArcGeoSim::IXMTools::IXMPropertyExceptions;

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

ArcGeoSim::IXMV4_1MeshReader::IXMV4_1MeshReader(Arcane::IIOMng *io_mng, Arcane::IParallelMng *parallel_mng,
                                                Arcane::ITraceMng *trace_mng): IXMV4MeshReader(io_mng, parallel_mng,
                  trace_mng){
}

/*---------------------------------------------------------------------------*/

ArcGeoSim::IXMV4_1MeshReader::
~IXMV4_1MeshReader()
{
}

/*---------------------------------------------------------------------------*/

Arcane::String
ArcGeoSim::IXMV4_1MeshReader::
loadMeshFile(IXMMeshFile mesh_file)
{
  if(m_parallel_mng->commRank()==0)
  {
    // Get xml document from mesh file
    m_xmldoc << mesh_file(m_io_mng,"ixm_v4_1.xsd",Arcane::ByteConstArrayView(strlen((const char*) ixm_v4_1_xsd), ixm_v4_1_xsd));

    // Go to event definition
    Arcane::XmlNode rootNode = m_xmldoc->documentNode().child("ixm");


    // Up to now only unstructured grid authorized by xsd
    m_grid_type = "unstructured-grid";
    Arcane::XmlNode gridRoot = rootNode.child(m_grid_type);

    // Check grid attributes

    String dimension = gridRoot.attrValue("dimension");
    m_grid_dimension = dimension ;
    m_parallel_mng->broadcastString(dimension,0) ;

    // Check version number
    /*
    ArcGeoSim::IXMMeshVersion mesh_version = mesh_file.version(m_io_mng,m_parallel_mng);
    if (mesh_version != m_supported_version)
      m_trace_mng->fatal() << Arcane::String::format("Cannot read {0} mesh file version. Supported mesh version is {1}.",
                                                     mesh_version.str(),m_supported_version.str());
    */
    m_events_data = gridRoot.children("event");

    // Check unicity of event names
    std::set<String> event_names;
    for(Integer iEvent=0;iEvent<m_events_data.size();++iEvent)
      {
        const String current_name = m_events_data[iEvent].attr("name").value();
        const bool is_new_name = event_names.insert(current_name).second;
        if (not is_new_name) m_trace_mng->error() << "Event name '" << current_name << "' is duplicated";
      }
    if ((Integer)event_names.size() != m_events_data.size())
      m_trace_mng->fatal() << "Incorrect event definition: see errors above";

    m_mesh_data = gridRoot.child("mesh");
    m_nb_events_data_size = m_events_data.size() ;
    m_has_mesh_data = ! m_mesh_data.null() ;
    SharedArray<Integer> buffer(2) ;
    buffer[0] = m_has_mesh_data?1:0 ;
    buffer[1] = m_nb_events_data_size ;
    m_parallel_mng->broadcast(buffer,0) ;
  }
  else
  {
    String dimension ;
    m_parallel_mng->broadcastString(dimension,0) ;
    m_grid_dimension = dimension ;
    SharedArray<Integer> buffer(2) ;
    m_parallel_mng->broadcast(buffer,0) ;
    m_has_mesh_data = buffer[0]!=0 ;
    m_nb_events_data_size = buffer[1] ;
  }

  m_mesh_file_directory_name = mesh_file.path();

  return m_mesh_file_directory_name;
}


void
ArcGeoSim::IXMV4_1MeshReader::
readCellData(ArcGeoSim::IXMTools::IXMCellData& cell_data, const ArcGeoSim::IXMTools::IXMMeshInfo& check_mesh_info)
{

    ARCANE_ASSERT((!m_mesh_data.null()),("No mesh file loaded in IXM V4 reader."))

    Arcane::XmlNode rootNode = m_mesh_data.child("topology");

    // Get managers
    ItemTypeMng * typeMng = ItemTypeMng::singleton();
    { // Handle created cells
        _readCellInfos(rootNode.children("cells"),
                       "cells",
                       typeMng,
                       cell_data.item_ids,
                       cell_data.item_infos,
                       check_mesh_info);
    }
}

#ifdef USE_EVOLUTIF
void
ArcGeoSim::IXMV4_1MeshReader::
readCellIncrementData(ArcGeoSim::IXMTools::IXMCellIncrementData& cell_increment_data,const ArcGeoSim::IXMTools::IXMMeshInfo& check_mesh_info)
{
    ARCANE_ASSERT((m_mesh_increment_xml_node),("No Mesh Increment loaded in the IXMV4MeshReader"))
    ARCANE_ASSERT((_isTopologyIncrementLoaded()),("Topology Increment not loaded in the IXMV4MeshReader"))

    Arcane::XmlNode rootNode = *(m_mesh_increment_xml_node->topology_node.get());

    // Get managers
    ItemTypeMng * typeMng = ItemTypeMng::singleton();

    { // Handle deleted cells
        Arcane::XmlNodeList delete_cell_list = rootNode.children("delete-cells");
        for (Arcane::XmlNodeList::const_iterator iCellList = delete_cell_list.begin();
             iCellList != delete_cell_list.end(); ++iCellList)
        { // Read delete-cells node
            Arcane::Int64SharedArray cell_ids = IXMInt64DataNodeReader(iCellList->child("cell-ids"),
                                                                       "Error while reading delete-cells uids",
                                                                       m_mesh_file_directory_name).read();
            cell_increment_data.removed_cell_data.item_ids.addRange(cell_ids);
        }
    }

    { // Handle created cells
        _readCellInfos(rootNode.children("create-cells"),
                       "create-cells",
                       typeMng,
                       cell_increment_data.new_cell_data.item_ids,
                       cell_increment_data.new_cell_data.item_infos,
                       check_mesh_info);
    }

    {// Handle modified cells
        _readCellInfos(rootNode.children("change-cells"),
                       "change-cells",
                       typeMng,
                       cell_increment_data.changed_cell_data.item_ids,
                       cell_increment_data.changed_cell_data.item_infos,
                       check_mesh_info);
    }
}
#endif
/*---------------------------------------------------------------------------*/

void
ArcGeoSim::IXMV4_1MeshReader::
_readCellInfos(const Arcane::XmlNodeList& cell_list_node,
               const String& block_name,
               ItemTypeMng * typeMng,
               Arcane::Int64Array & cell_uids,
               Arcane::Int64Array & cell_infos,
               const ArcGeoSim::IXMTools::IXMMeshInfo& check_mesh_info)
{

    Arcane::String error_types = Arcane::String::format("Error while reading {0} cell-types",block_name);
 //   Arcane::String error_hybrid_types = Arcane::String::format("Error while reading {0} hybrid-types",block_name);
    Arcane::String error_ids = Arcane::String::format("Error while reading {0} cell-ids",block_name);
    Arcane::String error_node_ids = Arcane::String::format("Error while reading {0} node-ids",block_name);

    for (Arcane::XmlNodeList::const_iterator iCellList = cell_list_node.begin();
         iCellList != cell_list_node.end(); ++iCellList)
    {
        // Read each block of data (types, cell ids, cell node ids)
        Arcane::IntegerSharedArray cell_type_ids = IXMIntegerDataNodeReader(iCellList->child("cell-types"),error_types, m_mesh_file_directory_name).read();
//       Arcane::IntegerSharedArray cell_hybrids_type_ids = IXMIntegerDataNodeReader(iCellList->child("hybrids-types"),error_hybrid_types, m_mesh_file_directory_name).read();
        Arcane::Int64SharedArray cell_ids = IXMInt64DataNodeReader(iCellList->child("cell-ids"),error_ids,m_mesh_file_directory_name).read();
        Arcane::Int64SharedArray node_ids = IXMInt64DataNodeReader(iCellList->child("node-ids"),error_node_ids, m_mesh_file_directory_name).read();

        // Check sizes for cells types and id
        if (cell_type_ids.size() != cell_ids.size())
            m_trace_mng->fatal() << "Inconsistent number of cell types (" << cell_ids.size() << ") and cell ids (" << cell_type_ids.size() << ") in " << block_name << " block";

        const Arcane::Integer event_nb_new_cells = cell_type_ids.size();

        Arcane::Integer event_nb_new_nodes = 0;
        // Parse types
        Arcane::IntegerSharedArray cell_nb_nodes(event_nb_new_cells); // number of nodes for each cell
        const Arcane::Integer mesh_dimension = check_mesh_info.dimension(); // dimension of current mesh
        for (Arcane::Integer i = 0; i < event_nb_new_cells; ++i)
        {
            if(cell_type_ids[i]>=min_exotic_type_value){
                //case of an hybrid cell

                const Integer user_cell_type = cell_type_ids[i]-min_exotic_type_value;
                const Integer internal_cell_type = user_cell_type + ItemTypeMng::nbBuiltInItemType();
                if (user_cell_type < 0 || internal_cell_type >= ItemTypeMng::nbBasicItemType())
                    m_trace_mng->fatal() << "Bad item type " << user_cell_type;
                auto item_type = typeMng->typeFromId(internal_cell_type);
                cell_type_ids[i]=internal_cell_type;
            }
            if (cell_type_ids[i] >= typeMng->nbBasicItemType())
                m_trace_mng->fatal() << String::format("Type id {0} does not exist for arcane items.",cell_type_ids[i]);
            if (check_mesh_info.typeDimension()[cell_type_ids[i]] != mesh_dimension)
                m_trace_mng->fatal() << "Type id " << cell_type_ids[i] << " for cell is not allowed in " << mesh_dimension << "d mesh in <" << iCellList->xpathFullName() << ">";
            // increment new node number
            auto current_cell_type = typeMng->typeFromId(cell_type_ids[i]);
            cell_nb_nodes[i] = current_cell_type->nbLocalNode();
            event_nb_new_nodes += cell_nb_nodes[i];
        }
        // Check size for node id
        if (node_ids.size() != event_nb_new_nodes)
            m_trace_mng->fatal()
                    << "Inconsistent total node number not equal to cell number times cell node number. Total node number expected ="
                    << event_nb_new_nodes << " Total node number received = " << node_ids.size();
        // Add cells info
        cell_uids.addRange(cell_ids);
        cell_infos.reserve(cell_infos.size() + 2*event_nb_new_cells + event_nb_new_nodes); // optimisation d'allocation
        for (Arcane::Integer i = 0, current_node_position = 0; i < event_nb_new_cells; ++i)
        {
            cell_infos.add(cell_type_ids[i]);
            cell_infos.add(cell_ids[i]);
            cell_infos.addRange(node_ids.subConstView(current_node_position,cell_nb_nodes[i]));
            current_node_position += cell_nb_nodes[i];
        }
    }
}

#ifdef USE_EVOLUTIF
bool
ArcGeoSim::IXMV4_1MeshReader::
_isTopologyIncrementLoaded()
{
    if (m_mesh_increment_xml_node) return static_cast<bool>(m_mesh_increment_xml_node->topology_node);
    return false;
}
#endif

/*---------------------------------------------------------------------------*/
