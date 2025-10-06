/*
 * IxmMeshWriter.cpp
 *
 *  Created on: May 30, 2012
 *      Author: mesriy
 */

#include "arcane/IParallelMng.h"
#include "arcane/IIOMng.h"
#include "arcane/IXmlDocumentHolder.h"
#include "arcane/XmlNodeList.h"
#include "arcane/XmlNode.h"

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/Real3.h>
#include "arcane/utils/List.h"
#include <arcane/utils/OStringStream.h>
#include <arcane/DomUtils.h>
#include <arcane/ISubDomain.h>
#include <arcane/ItemGroup.h>
#include <arcane/Directory.h>
#include <arcane/IPostProcessorWriter.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/IVariableMng.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/IParticleFamily.h>
#include <arcane/utils/PlatformUtils.h>
#include <arcane/IRessourceMng.h>
#include <arcane/PostProcessorWriterBase.h>
#include <arcane/Directory.h>

#include "ArcGeoSim/Mesh/Utils/IXMTools/ixm_v4_xsd.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMItemConnections.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMUidToLidMng.h"

#include "ArcGeoSim/Mesh/Exporters/IXM4/IXMDataNode.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/IXM4DataNode.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/IXMPropertyInfo.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/IXMMeshWriter.h"

#include "ArcGeoSim/Mesh/Exporters/IXM4/VariableStrideFormatter.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/FixedStrideFormatter.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/**********************************************************************
 * [IxmMeshWriter]
 **********************************************************************/
IXMMeshWriter::
#if (ARCANE_VERSION >= 30003)
IXMMeshWriter(IMesh * mesh, IGraph2* graph,
#else
IXMMeshWriter(IMesh * mesh,
#endif
              IXmlDocumentHolder* ixm_doc, const String& directory_name,const eIXMMeshWriteInfo & write_mesh,
              const bool write_faces_infos, const Integer event_number, bool event_initialized, bool write_only_active_cells)
: m_mesh(mesh)
#if (ARCANE_VERSION >= 30003)
, m_graph(graph)
#endif
, m_directory_name(directory_name)
, m_write_mesh(write_mesh)
, m_trace_mng(m_mesh->subDomain()->traceMng())
, m_ressource_mng(m_mesh->subDomain()->ressourceMng())
, m_io_mng(m_mesh->subDomain()->ioMng())
, m_ixm_mesh(NULL)
, m_ixm_event(NULL)
, m_ixm_doc(ixm_doc)
, m_event_number(event_number)
, m_events_initialized(event_initialized)
, m_write_only_active_cells(write_only_active_cells)
, m_supported_version("V4")
, m_write_mesh_properties(false)
, m_begin_time(0)
, m_write_faces_infos(write_faces_infos)
#if (ARCANE_VERSION >= 30003)
, m_connection_mng(m_mesh,m_graph)
#else
, m_connection_mng(m_mesh)
#endif
{
  ixmDataExchanger()->registerReceiver(this);
}

/**********************************************************************
 * [~IxmMeshWriter]
 **********************************************************************/
IXMMeshWriter::
~IXMMeshWriter()
{
  ixmDataExchanger()->unregisterReceiver(this);
}

/**********************************************************************
 * [beginMeshWrite]
 **********************************************************************/
void
IXMMeshWriter::
beginMeshWrite(const String& file_name, const String& format, ItemGroupCollection& groups,VariableCollection& vars){

	m_format = format;

	/****************************************
	 * Create/append Ixm files
	 ****************************************/
	createIxmFile(file_name,m_format);

	/****************************************
	 * Create/update Ixm document
	 ****************************************/
	if( !m_events_initialized ){
		createIxmDoc();
	} else {
		//loadIxmDoc();
	}

	/****************************************
	// record groups to be exported
	****************************************/
	m_groups=groups;
}


// Ecriture d'un maillage non-evolutif
/**********************************************************************
 * [writeMeshToFile]
 **********************************************************************/
void
IXMMeshWriter::
writeMeshToFile(const String& format){

	XmlElement mesh_node(m_grid_node,"mesh");
	XmlElement ixm_static_topology_node(mesh_node,"topology");

	/***************************************
	 * IxmNode to save nodes element
	 ***************************************/
	XmlElement nodes_node(ixm_static_topology_node,"nodes");
	nodeElement(format, nodes_node);
	/***************************************
	 * IxmNode to save cells element
	 ***************************************/
	XmlElement cells_node(ixm_static_topology_node,"cells");
	cellElement(format, cells_node);
	/***************************************
	 * IxmNode to save faces element
	 ***************************************/
	XmlElement faces_node(ixm_static_topology_node,"faces");
	faceElement(format, faces_node);
	/***************************************
	 * IxmNode to save particles element
	 ***************************************/
        Arcane::IItemFamily* particle_family = m_mesh->findItemFamily(IK_Particle, "Particle", false);
	if (particle_family && particle_family->nbItem()) {
          XmlElement particles_node(ixm_static_topology_node,"particles");
          particleElement(format, particles_node);
          particleCoordsElement(format, particles_node);
        }
	/***************************************
	 * IxmNode to save connections element
	 ***************************************/
	if(m_connection_mng.hasConnection()){
		XmlElement connections_node(ixm_static_topology_node,"connections");
		connectionElement(format, connections_node);
	}

	/***************************************
	 * IxmNode to save group element/
	 ***************************************/
	// for group: m_groups
	writeGroups(ixm_static_topology_node,format);


	// Prepare properties dump (done in write(ivar,idata))
		// m_mesh_node = mesh_node;
		// m_write_mesh_properties = true;
	m_mesh_node = mesh_node;
	m_write_mesh_properties = true;


}

// Ecriture d'un maillage dynamique
/**********************************************************************
 * [writeEventToFile]
 **********************************************************************/
void
IXMMeshWriter::
writeEventToFile(const String& file_name, const Real& begin_time, const Real& end_time, const Integer& event_number){

	m_begin_time = begin_time;
	updateIxmDoc(m_begin_time,end_time,event_number);

	/****************************************
	 * Build a mesh increment element if needed: TO DO
	 ****************************************/
	if (m_write_faces_infos == true)
		writeMeshIncrement(m_format);

}

void
IXMMeshWriter::
writeVoidEventToFile(const String& file_name, const Real& begin_time, const Real& end_time, const Integer& event_number){

	m_begin_time = begin_time;
	updateIxmDoc(m_begin_time,end_time,event_number);

}

/**********************************************************************
 * [endWrite]
 **********************************************************************/
void
IXMMeshWriter::
endMeshWrite()
{
  /********************
   * Output & cleanup *
   ********************/
  bool is_master = m_mesh->subDomain()->parallelMng()->isMasterIO();
  if(is_master && m_write_mesh != eIXMMeshWriteInfo::WI_MeshByMesh){
    ByteSharedArray xml_bytes;
    m_ixm_doc->save(xml_bytes);
    m_io_mng->writeXmlFile(m_ixm_doc,ixmFileName(),true);
  }
}


// VoidMesh
/**********************************************************************
 * [VoidMesh]
 **********************************************************************/
void
IXMMeshWriter::
writeVoidMesh(const String& format){

		XmlElement mesh_node(m_grid_node,"mesh");

		XmlElement ixm_static_topology_node(mesh_node,"topology");


}



// mesh-increment Element
/**********************************************************************
 * [meshIncrementElement]
 **********************************************************************/
void
IXMMeshWriter::
writeMeshIncrement(const String& format) {

	XmlElement mesh_increment_node(m_event_node,"mesh-increment");

	/****************************************
	 * IxmNode to begin a topology element
	 ****************************************/
	XmlElement ixm_dynamic_topology_node(mesh_increment_node,"topology");

	/****************************************
	 * IxmNode to save faces element
	 ****************************************/
	XmlElement faces_node(ixm_dynamic_topology_node,"faces");
	faceElement(format, faces_node);

	/****************************************
	 * IxmNode to save create-cells element
	 ****************************************/
	//XmlElement create_cells_node(ixm_dynamic_topology_node,"create-cells");
	//cellElement(format, create_cells_node);

	/****************************************
	 * IxmNode to save change-cells element
	 ****************************************/
	//XmlElement change_cells_node(ixm_dynamic_topology_node,"change-cells");
	//cellElement(format, change_cells_node);

	/****************************************
	 * IxmNode to save delete-cells element
	 ****************************************/
	//XmlElement delete_cells_node(ixm_dynamic_topology_node,"delete-cells");
	//deleteCellElement(format, delete_cells_node);

	/****************************************
	 * IxmNode to save update-nodes element
	 ****************************************/
	//XmlElement update_nodes_node(ixm_dynamic_topology_node,"update-nodes");
	//updateCoordElement(format, update_nodes_node);

	// for group: m_groups
	/****************************************
	 * IxmNode to save group element
	 ****************************************/
	//writeGroups(ixm_dynamic_topology_node,format);

	/****************************************
	 * IxmNode to begin a geometry element
	 ****************************************/
	//XmlElement ixm_dynamic_geometry_node(mesh_increment_node,"geometry");

	/****************************************
	 * IxmNode to save move-node-to element
	 ****************************************/
	//XmlElement  move_node_to_node(ixm_dynamic_geometry_node,"move_node_to");
	//nodeElement(format, move_node_to_node);

}

// mesh-increment Element
/**********************************************************************
 * [meshIncrementElement]
 **********************************************************************/
void
IXMMeshWriter::
writeCurrentMesh(const String& format, bool first_event){

	//XmlElement mesh_node(m_event_node,"mesh");

	//XmlElement topology_node(mesh_node,"topology");

	XmlElement topology_node(m_event_node,"mesh");

	/****************************************
	 * IxmNode to save faces element
	 ****************************************/
	XmlElement faces_node(topology_node,"faces");
	faceElement(format, faces_node);

	/****************************************
	 * IxmNode to save cells element
	 ****************************************/
	XmlElement cells_node(topology_node,"cells");
	cellElement(format, cells_node);

	/****************************************
	 * IxmNode to save nodes element
	 ****************************************/
	XmlElement nodes_node(topology_node,"nodes");
	nodeElement(m_format, nodes_node);
	

	/****************************************
	* IxmNode to save connections element
	****************************************/
	
	if( first_event && m_connection_mng.hasConnection() ){
		XmlElement connections_node(topology_node,"connections");
		connectionElement(m_format, connections_node);
	}

        /****************************************
         * IxmNode to save particles element
         ****************************************/
        Arcane::IItemFamily* particle_family = m_mesh->findItemFamily(IK_Particle, "Particle", false);
        if ( first_event && particle_family && particle_family->nbItem()){
          XmlElement particles_node(topology_node,"particles");
          particleElement(m_format, particles_node);
          particleCoordsElement(m_format, particles_node);
        }
	//nodeIdsElement(format, update_nodes_node,"node-ids" );
	//nodeCoordsElement(format,update_nodes_node );

	/****************************************
	 * IxmNode to save group element
	 ****************************************/
	// for group: m_groups
	if (first_event )
	   writeGroups(topology_node,m_format);

}




// coords-update Element
/**********************************************************************
 * [updateCoordsElement]
 **********************************************************************/
void
IXMMeshWriter::
updateCoordElement(const String& format, XmlNode currentNode){

	XmlElement update_coord_node(currentNode,"coord-update");

	nodeIdsElement(format, update_coord_node,"node-ids" );
	nodeCoordsElement(format,update_coord_node );
}


// clone-update Element : not used yet
/**********************************************************************
 * [cloneNodesElement]
 **********************************************************************/
void
IXMMeshWriter::
cloneNodesElement(const String& format, XmlNode currentNode){

	XmlElement clone_update_node(currentNode,"clone_update");

	nodeIdsElement(format, clone_update_node);
	clonedNodeIdsElement(format, clone_update_node);
}


// nodes Element
/**********************************************************************
 * [nodeIdsElement]
 **********************************************************************/
void
IXMMeshWriter::
nodeIdsElement(const String& format, XmlNode current_node, String node_ids){
	Int64SharedArray node_uids; // Unique nodes-IDs array
	ENUMERATE_NODE(inode,m_mesh->ownNodes()){
		node_uids.add(inode->uniqueId().asInt64());
	}
	IParallelMng* pm=  m_mesh->subDomain()->parallelMng();
	Int64SharedArray all_uids;
	if(pm->isParallel()){
		  pm->gatherVariable(node_uids.constView(),all_uids , pm->masterIORank());
		  node_uids= all_uids;
	}

	String gparent_name = getGrandParentName(current_node);;
	String event_name;

	if( gparent_name == "mesh" )
		event_name="mesh";
	else
	{
		event_name= m_event_node.attrValue("name");
		if( gparent_name == "event" )
			gparent_name = "mesh";
	}

	if(pm->isMasterIO())
	{

		XmlElement node_ids_node(current_node,node_ids);

		if(m_format == "xml")
		{
			IXM4DataNode<Int64>::createIXM4DataNode(node_ids_node, node_uids);
		}
		else
		{
			String hdata_node_ids;
			if ( event_name == "mesh")
				hdata_node_ids = String::format("{0}/{1}/{2}/node-ids",m_hdf_path,event_name,current_node.name());
			else
				hdata_node_ids = String::format("{0}/{1}/{2}/{3}/node-ids",m_hdf_path,event_name,gparent_name,current_node.name());
			IXM4DataNode<Int64>::createIXM4DataNode(node_ids_node,node_uids, HDFFormatBuildInfo(hdata_node_ids,m_directory_name));
		}
	}

}

// nodes Element
/**********************************************************************
 * [clonedNodeIdsElement]
 **********************************************************************/
void
IXMMeshWriter::
clonedNodeIdsElement(const String& format, XmlNode current_node, String node_ids){

	// TO DO
}


// nodes Element
/**********************************************************************
 * [nodeCoordsElement]
 **********************************************************************/
void
IXMMeshWriter::
nodeCoordsElement(const String format, XmlNode current_node ){

  SharedArray<Real3> coords;
  coords.reserve(m_mesh->nbNode());

  VariableItemReal3& nodes_coords = m_mesh->nodesCoordinates();
  ENUMERATE_NODE(inode,m_mesh->ownNodes()){
    coords.add(nodes_coords[inode]);
  }
  IParallelMng* pm=  m_mesh->subDomain()->parallelMng();
  Real3SharedArray all_coords;
  if(pm->isParallel()){
      pm->gatherVariable(coords.constView(),all_coords , pm->masterIORank());
      coords= all_coords;
  }

	String gparent_name = getGrandParentName(current_node);;
	String event_name;

	if( gparent_name == "mesh" )
		event_name="mesh";
	else
	{
		event_name= m_event_node.attrValue("name");
		if( gparent_name == "event" )
			gparent_name = "mesh";
	}

  // coordinates
  if(pm->isMasterIO())
  {
	  XmlElement coords_node(current_node,"coords");
	  if(m_format == "xml")
	  {
		  IXM4DataNode<Real3>::createIXM4DataNode(coords_node, coords, -1,XMLFormatBuildInfo<Real3>(FixedStrideFormatter<Real3>(1)));
	  }
	  else
	  {
		  String hdata_coords;
		  if ( event_name == "mesh")
			  hdata_coords = String::format("{0}/{1}/{2}/coords",m_hdf_path,event_name,current_node.name());
		  else
			  hdata_coords = String::format("{0}/{1}/{2}/{3}/coords",m_hdf_path,event_name,gparent_name,current_node.name());
		  IXM4DataNode<Real3>::createIXM4DataNode(coords_node,coords, HDFFormatBuildInfo(hdata_coords,m_directory_name));
	  }
  }

}

// nodes Element, move-node-to Element and coords-update
/**********************************************************************
 * [NodeElement]
 **********************************************************************/
void
IXMMeshWriter::
nodeElement(const String& format, XmlNode currentNode){
	nodeIdsElement(format, currentNode );
	nodeCoordsElement(format,currentNode );
}

// update-nodes Element
/**********************************************************************
 * [cloneNodeElement]
 **********************************************************************/
void
IXMMeshWriter::
cloneNodeElement(const String& format, XmlNode currentNode){
	nodeIdsElement(format, currentNode );
	nodeIdsElement(format, currentNode, "cloned_ids");
}

// cells (createCells,changeCells,deleteCells),
// Nous pouvons egalement factoriser, cells, faces et connections comme
// ils ont la meme structure d ecriture
// TODO: une structure a partager entre les differents items
// struct {Types,Uids,NodeIds}


/**********************************************************************
 * [cellElement]
 **********************************************************************/
void
IXMMeshWriter::
cellElement(const String format, XmlNode current_node ){

	SharedArray<Int32> cell_types;
	SharedArray<Int64> cell_uids;
	SharedArray<Int64> node_cell_uids;
	
	Integer nbCells;
	ItemGroup group;
	
	if ( m_write_only_active_cells )
	{
	   nbCells = m_mesh->ownCells().activeCellGroup().size();
	   group = m_mesh->ownCells().activeCellGroup();
	}
	else
	{
	   nbCells = m_mesh->ownCells().size();
	   group = m_mesh->ownCells();
	}
	
	
	SharedArray<Int32> strides(nbCells);

	ENUMERATE_CELL(icell,group){// Scanning the cells' nodes to get type and connectivity
	
		Cell cell = *icell;
		Integer  nb_node = cell.nbNode();
		strides[icell.index()] = nb_node;
		cell_uids.add(icell->uniqueId().asInt64());
		cell_types.add(cell.type());
		for(Integer j=0; j<nb_node;++j){
			Int64 uid=cell.node(j).uniqueId();
			node_cell_uids.add(uid);
		}
	}

	IParallelMng* pm=  m_mesh->subDomain()->parallelMng();

	String gparent_name = getGrandParentName(current_node);;
	String event_name;

	if( gparent_name == "mesh" )
		event_name="mesh";
	else
	{
		event_name= m_event_node.attrValue("name");
		if( gparent_name == "event" )
			gparent_name = "mesh";
	}

	{
		Int32SharedArray all_types;
		if(pm->isParallel()){
			pm->gatherVariable(cell_types.constView(),all_types , pm->masterIORank());
			cell_types= all_types;
		}

		if(pm->isMasterIO())
		{
			XmlElement types_node(current_node,"cell-types");
			if(m_format == "xml")
			{
				IXM4DataNode<Int32>::createIXM4DataNode(types_node, cell_types);
			}
			else
			{
				String hdata_types;
				if (event_name=="mesh")
					hdata_types = String::format("{0}/{1}/{2}/cell-types",m_hdf_path,event_name,current_node.name());
				else
					hdata_types = String::format("{0}/{1}/{2}/{3}/cell-types",m_hdf_path,event_name,gparent_name,current_node.name());
				IXM4DataNode<Int32>::createIXM4DataNode(types_node,cell_types, HDFFormatBuildInfo(hdata_types,m_directory_name));
			}
		}
	}


	{
		Int64SharedArray all_uids;
		if(pm->isParallel()){
			pm->gatherVariable(cell_uids.constView(),all_uids , pm->masterIORank());
			cell_uids= all_uids;
		}

		if(pm->isMasterIO())
		{
			XmlElement ids_node(current_node,"cell-ids");
			if(m_format == "xml")
			{
				IXM4DataNode<Int64>::createIXM4DataNode(ids_node, cell_uids);
			}
			else
			{
				String hdata_ids;
				if (event_name=="mesh")
					hdata_ids = String::format("{0}/{1}/{2}/cell-ids",m_hdf_path,event_name,current_node.name());
				else
					hdata_ids = String::format("{0}/{1}/{2}/{3}/cell-ids",m_hdf_path,event_name,gparent_name,current_node.name());
				IXM4DataNode<Int64>::createIXM4DataNode(ids_node,cell_uids, HDFFormatBuildInfo(hdata_ids,m_directory_name));
			}
		}
	}


	{
		Int64SharedArray all_uids;
		if(pm->isParallel()){
			pm->gatherVariable(node_cell_uids.constView(),all_uids , pm->masterIORank());
			node_cell_uids= all_uids;
		}
		Int32SharedArray all_strides;
		if(pm->isParallel()){
			pm->gatherVariable(strides.constView(),all_strides , pm->masterIORank());
			strides= all_strides;
		}

		if(pm->isMasterIO())
		{
			XmlElement node_ids_node(current_node,"node-ids");
			if(m_format == "xml")
			{
				IXM4DataNode<Int64>::createIXM4DataNode(node_ids_node, node_cell_uids,-1, XMLFormatBuildInfo<Int64>(VariableStrideFormatter<Int64>(strides)));
			}
			else
			{
				String hdata_node_ids;
				if (event_name=="mesh")
					hdata_node_ids = String::format("{0}/{1}/{2}/node-ids",m_hdf_path,event_name,current_node.name());
				else
					hdata_node_ids = String::format("{0}/{1}/{2}/{3}/node-ids",m_hdf_path,event_name,gparent_name,current_node.name());
				IXM4DataNode<Int64>::createIXM4DataNode(node_ids_node,node_cell_uids, HDFFormatBuildInfo(hdata_node_ids,m_directory_name));
			}
		}
	}


}


// TO DO
/**********************************************************************
 * [deleteCellElement]
 **********************************************************************/
void
IXMMeshWriter::
deleteCellElement(const String format, XmlNode current_node ){

}

//faces,
/**********************************************************************
 * [faceElement]
 **********************************************************************/
void
IXMMeshWriter::
faceElement(const String format, XmlNode current_node ){
	ItemGroup own_faces = m_mesh->ownFaces();
    SharedArray<Int32> face_types;
	SharedArray<Int64> face_uids(own_faces.size());
	SharedArray<Int64> node_face_uids;
	SharedArray<Int32> strides(own_faces.size());

	ENUMERATE_FACE(iface,own_faces){// Scanning the cells' nodes to get type and connectivity
		Face face = *iface;
		Integer  nb_node = face.nbNode();
		strides[iface.index()] = nb_node;
		face_types.add(face.type());
		for(Integer j=0; j<nb_node;++j){
			Integer uid=face.node(j).uniqueId().asInteger();
			node_face_uids.add(uid);
		}
	}

	IParallelMng* pm=  m_mesh->subDomain()->parallelMng();

	// Get Face uids (or aids if mapping exist)
	ArcGeoSim::IXMLidToUidMng lid2uid(m_mesh);
	lid2uid.itemsUniqueId(face_uids,own_faces.view(),IK_Face);

	String gparent_name = getGrandParentName(current_node);
	String event_name;

	if( gparent_name == "mesh" )
		event_name="mesh";
	else
	{
		event_name= m_event_node.attrValue("name");
		if( gparent_name == "event" )
			gparent_name = "mesh";
	}

	{
		Int32SharedArray all_types;
		if(pm->isParallel()){
			pm->gatherVariable(face_types.constView(),all_types , pm->masterIORank());
			face_types= all_types;
		}

		if(pm->isMasterIO())
		{
			XmlElement types_node(current_node,"face-types");
			if(m_format == "xml")
			{
				IXM4DataNode<Int32>::createIXM4DataNode(types_node, face_types);
			}
			else
			{
				String hdata_types;
				if (event_name=="mesh")
					hdata_types = String::format("{0}/{1}/{2}/face-types",m_hdf_path,event_name,current_node.name());
				else
					hdata_types = String::format("{0}/{1}/{2}/{3}/face-types",m_hdf_path,event_name,gparent_name,current_node.name());
				IXM4DataNode<Int32>::createIXM4DataNode(types_node,face_types, HDFFormatBuildInfo(hdata_types,m_directory_name));
			}
		}

	}

	{
		Int64SharedArray all_uids;
		if(pm->isParallel()){
			pm->gatherVariable(face_uids.constView(),all_uids , pm->masterIORank());
			face_uids= all_uids;
		}

		if(pm->isMasterIO())
		{
			XmlElement ids_node(current_node,"face-ids");
			if(m_format == "xml")
			{
				IXM4DataNode<Int64>::createIXM4DataNode(ids_node, face_uids);
			}
			else
			{
				String hdata_ids;
				if (event_name=="mesh")
					hdata_ids = String::format("{0}/{1}/{2}/face-ids",m_hdf_path,event_name,current_node.name());
				else
					hdata_ids = String::format("{0}/{1}/{2}/{3}/face-ids",m_hdf_path,event_name,gparent_name,current_node.name());
				IXM4DataNode<Int64>::createIXM4DataNode(ids_node,face_uids, HDFFormatBuildInfo(hdata_ids,m_directory_name));
			}
		}
	}


	{
		Int64SharedArray all_uids;
		if(pm->isParallel()){
			pm->gatherVariable(node_face_uids.constView(),all_uids , pm->masterIORank());
			node_face_uids= all_uids;
		}
		Int32SharedArray all_strides;
		if(pm->isParallel()){
			pm->gatherVariable(strides.constView(),all_strides , pm->masterIORank());
			strides= all_strides;
		}


		if(pm->isMasterIO())
		{
			XmlElement node_ids_node(current_node,"node-ids");
			if(m_format == "xml")
			{
				IXM4DataNode<Int64>::createIXM4DataNode(node_ids_node, node_face_uids,-1, XMLFormatBuildInfo<Int64>(VariableStrideFormatter<Int64>(strides)));
			}
			else
			{
				String hdata_node_ids;
				if (event_name=="mesh")
					hdata_node_ids =String::format("{0}/{1}/{2}/node-ids",m_hdf_path,event_name,current_node.name());
				else
					hdata_node_ids = String::format("{0}/{1}/{2}/{3}/node-ids",m_hdf_path,event_name,gparent_name,current_node.name());
				IXM4DataNode<Int64>::createIXM4DataNode(node_ids_node,node_face_uids, HDFFormatBuildInfo(hdata_node_ids,m_directory_name));
			}
		}

	}
}

//particles,
/**********************************************************************
 * [particleElement]
 **********************************************************************/
void
IXMMeshWriter::
particleElement(const String format, XmlNode current_node )
{
  // The particles family
  IItemFamily* myFam = m_mesh->findItemFamily(IK_Particle, "Particle", false);

  ParticleGroup myFamGroup = myFam->toParticleFamily()->allItems().own();
  Integer nb_particles = myFamGroup.size();
  SharedArray<Int32> particle_types(nb_particles, 1); // Nb cells for each particles = 1
  SharedArray<Int64> particle_uids(nb_particles);
  SharedArray<Int64> particle_cell_uids(nb_particles);

  Integer current_particle = 0;
  ENUMERATE_PARTICLE(iparticle,myFam->toParticleFamily()->allItems().own())
  {
    const Particle& particle = *iparticle;
    particle_uids[current_particle] = particle.uniqueId().asInt64();
    particle_cell_uids[current_particle] = particle.cell().uniqueId().asInt64();
    current_particle ++;
  }

  IParallelMng* pm = m_mesh->subDomain()->parallelMng();
  String gparent_name = getGrandParentName(current_node);
  String event_name;

  if (gparent_name == "mesh")
    event_name = "mesh";
  else
  {
    event_name = m_event_node.attrValue("name");
    if (gparent_name == "event")
      gparent_name = "mesh";
  }

  {
    Int64SharedArray all_uids;
    if (pm->isParallel())
    {
      pm->gatherVariable(particle_uids.constView(), all_uids, pm->masterIORank());
      particle_uids = all_uids;
    }

    if (pm->isMasterIO())
    {
      XmlElement ids_node(current_node, "particle-ids");
      if (m_format == "xml")
      {
        IXM4DataNode<Int64>::createIXM4DataNode(ids_node, particle_uids);
      }
      else
      {
        String hdata_ids;
        if (event_name == "mesh")
          hdata_ids = String::format("{0}/{1}/{2}/particle-ids", m_hdf_path, event_name, current_node.name());
        else
          hdata_ids = String::format("{0}/{1}/{2}/{3}/particle-ids", m_hdf_path, event_name, gparent_name,
              current_node.name());
        IXM4DataNode<Int64>::createIXM4DataNode(ids_node, particle_uids,
            HDFFormatBuildInfo(hdata_ids, m_directory_name));
      }
    }
  }

  {
    Int32SharedArray all_types;
    if (pm->isParallel())
    {
      pm->gatherVariable(particle_types.constView(), all_types, pm->masterIORank());
      particle_types = all_types;
    }

    if (pm->isMasterIO())
    {
      XmlElement types_node(current_node, "particle-types");
      if (m_format == "xml")
      {
        IXM4DataNode<Int32>::createIXM4DataNode(types_node, particle_types);
      }
      else
      {
        String hdata_types;
        if (event_name == "mesh")
          hdata_types = String::format("{0}/{1}/{2}/particle_types", m_hdf_path, event_name, current_node.name());
        else
          hdata_types = String::format("{0}/{1}/{2}/{3}/particle_types", m_hdf_path, event_name, gparent_name,
              current_node.name());
        IXM4DataNode<Int32>::createIXM4DataNode(types_node, particle_types,
            HDFFormatBuildInfo(hdata_types, m_directory_name));
      }
    }
  }

  {
    Int64SharedArray all_cell_uids;
    if (pm->isParallel())
    {
      pm->gatherVariable(particle_cell_uids.constView(), all_cell_uids, pm->masterIORank());
      particle_cell_uids = all_cell_uids;
    }

    if (pm->isMasterIO())
    {
      XmlElement types_node(current_node, "cell-ids");
      if (m_format == "xml")
      {
        IXM4DataNode<Int64>::createIXM4DataNode(types_node, particle_cell_uids);
      }
      else
      {
        String hdata_types;
        if (event_name == "mesh")
          hdata_types = String::format("{0}/{1}/{2}/cell-ids", m_hdf_path, event_name, current_node.name());
        else
          hdata_types = String::format("{0}/{1}/{2}/{3}/cell-ids", m_hdf_path, event_name, gparent_name,
              current_node.name());
        IXM4DataNode<Int64>::createIXM4DataNode(types_node, particle_cell_uids,
            HDFFormatBuildInfo(hdata_types, m_directory_name));
      }
    }
  }

}

/**********************************************************************
 * [particleCoordsElement]
 **********************************************************************/
void
IXMMeshWriter::
particleCoordsElement(const String format, XmlNode current_node )
{

  // The particles family
  IItemFamily* myFam = m_mesh->findItemFamily(IK_Particle, "Particle", false);

  ParticleGroup myFamGroup = myFam->toParticleFamily()->allItems().own();
  Integer nb_particles = myFamGroup.size();

  SharedArray<Real3> coords;
  coords.reserve(nb_particles);

  VariableParticleReal3 particle_coords(VariableBuildInfo(m_mesh, "ParticleCoord", "Particle"));
  ENUMERATE_PARTICLE(iparticle,myFamGroup)
  {
    coords.add(particle_coords[iparticle]);
  }

  IParallelMng* pm = m_mesh->subDomain()->parallelMng();
  Real3SharedArray all_coords;
  if (pm->isParallel())
  {
    pm->gatherVariable(coords.constView(), all_coords, pm->masterIORank());
    coords = all_coords;
  }

  String gparent_name = getGrandParentName(current_node);
  String event_name;

  if (gparent_name == "mesh")
    event_name = "mesh";
  else
  {
    event_name = m_event_node.attrValue("name");
    if (gparent_name == "event")
      gparent_name = "mesh";
  }

  // coordinates
  if (pm->isMasterIO())
  {
    XmlElement coords_node(current_node, "particle-coords");
    if (m_format == "xml")
    {
      IXM4DataNode<Real3>::createIXM4DataNode(coords_node, coords,-1,XMLFormatBuildInfo<Real3>(FixedStrideFormatter<Real3>(1)));
    }
    else
    {
      String hdata_coords;
      if (event_name == "mesh")
        hdata_coords = String::format("{0}/{1}/{2}/particle-coords", m_hdf_path, event_name, current_node.name());
      else
        hdata_coords = String::format("{0}/{1}/{2}/{3}/particle-coords", m_hdf_path, event_name, gparent_name,
            current_node.name());
      IXM4DataNode<Real3>::createIXM4DataNode(coords_node, coords, HDFFormatBuildInfo(hdata_coords, m_directory_name));
    }
  }

}

// connections
/**********************************************************************
 * [connectionElement]
 **********************************************************************/
void
IXMMeshWriter::
connectionElement(const String format, XmlNode current_node ){

  SharedArray<Int32> connection_types;
  SharedArray<Int64> connection_uids;
  SharedArray<Int64> item_uids;
  String gparent_name;

  IParallelMng* pm =  m_mesh->subDomain()->parallelMng();

  ArcGeoSim::ItemConnectionGroup own_connections = m_connection_mng.ownConnections();
  Integer nb_connections = own_connections.size();

  // Get connection data
  connection_types.reserve(nb_connections);
  connection_uids.resize(nb_connections);
  item_uids.reserve(2*nb_connections);

  ArcGeoSim::IXMLidToUidMng lid2uid(m_mesh);

  lid2uid.itemsUniqueId(connection_uids,own_connections.view(),ArcGeoSim::ItemConnectionImpl::kind());
  ArcGeoSim::IXMItemUidToAidMapper face_uid2aid(m_mesh,pm,m_mesh->subDomain()->variableMng(),IK_Face);

  auto getUid = [=](Item const& item) -> Int64 {
      switch(item.kind())
      {
      case IK_Face :
      {
        return face_uid2aid.itemUniqueIdToAbstractId(item.uniqueId().asInt64()) ;
      }
      default :
        return item.uniqueId().asInt64() ;
      }
    } ;
  ENUMERATE_CONNECTION(iconnection,own_connections){
#if (ARCANE_VERSION >= 30003)
    connection_types.add(iconnection->type(m_graph));
    item_uids.add(getUid(iconnection->first(m_graph)));
    item_uids.add(getUid(iconnection->second(m_graph)));
#else
    connection_types.add(iconnection->type());
    item_uids.add(getUid(iconnection->first()));
    item_uids.add(getUid(iconnection->second()));
#endif
  }

  if(pm->isParallel()){
	  Int32SharedArray all_connection_types;
	  pm->gatherVariable(connection_types.constView(),all_connection_types, pm->masterIORank());
	  connection_types = all_connection_types;
  }

  String event_name;

  	if(getGrandParentName(current_node) == "mesh")
  		event_name="mesh";
  	else
  		event_name= m_event_node.attrValue("name");

  if(pm->isMasterIO())
  {
	  XmlElement types_node(current_node,"connection-types");
	  if(m_format == "xml")
	  {
		  IXM4DataNode<Int32>::createIXM4DataNode(types_node, connection_types);
	  }
	  else
	  {
		  gparent_name=getGrandParentName(current_node);
		  String hdata_types;
		  if (event_name=="mesh")
			  hdata_types = String::format("{0}/{1}/{2}/connection-types",m_hdf_path,event_name,current_node.name());
		  else
			  hdata_types = String::format("{0}/{1}/{2}/{3}/connection-types",m_hdf_path,event_name,gparent_name,current_node.name());
		  IXM4DataNode<Int32>::createIXM4DataNode(types_node,connection_types, HDFFormatBuildInfo(hdata_types,m_directory_name));
	  }

  }

  if(pm->isParallel()){
	  Int64SharedArray all_connection_uids;
	  pm->gatherVariable(connection_uids.constView(),all_connection_uids, pm->masterIORank());
	  connection_uids = all_connection_uids;
  }

  if(pm->isMasterIO())
  {
	  XmlElement connection_ids_node(current_node,"connection-ids");
	  if(m_format == "xml")
	  {
		  IXM4DataNode<Int64>::createIXM4DataNode(connection_ids_node, connection_uids);
	  }
	  else
	  {
		  String hdata_ids;
		  if (event_name=="mesh")
			  hdata_ids = String::format("{0}/{1}/{2}/connections-ids",m_hdf_path,event_name,current_node.name());
		  else
			  hdata_ids = String::format("{0}/{1}/{2}/{3}/connections-ids",m_hdf_path,event_name,gparent_name,current_node.name());
		  IXM4DataNode<Int64>::createIXM4DataNode(connection_ids_node,connection_uids, HDFFormatBuildInfo(hdata_ids,m_directory_name));
	  }

  }

  if(pm->isParallel()){
	  Int64SharedArray all_item_uids;
	  pm->gatherVariable(item_uids.constView(),all_item_uids, pm->masterIORank());
	  item_uids = all_item_uids;
  }

  if(pm->isMasterIO())
  {
	  XmlElement item_ids_node(current_node,"item-ids");
	  if(m_format == "xml")
	  {
		  IXM4DataNode<Int64>::createIXM4DataNode(item_ids_node, item_uids,-1, XMLFormatBuildInfo<Int64>(FixedStrideFormatter<Int64>(2)));
	  }
	  else
	  {
		  String hdata_node_ids;
		  if (event_name=="mesh")
			  hdata_node_ids = String::format("{0}/{1}/{2}/item-ids",m_hdf_path,gparent_name,current_node.name());
		  else
			  hdata_node_ids = String::format("{0}/{1}/{2}/{3}/item-ids",m_hdf_path,event_name,gparent_name,current_node.name());
		  IXM4DataNode<Int64>::createIXM4DataNode(item_ids_node,item_uids, HDFFormatBuildInfo(hdata_node_ids,m_directory_name));
	  }
  }


}

// properties Element : implement IDataWriter interface
/**********************************************************************
 * [propertiesElement]
 **********************************************************************/

void
IXMMeshWriter::
beginWrite(const VariableCollection& vars)
{


	if (m_write_mesh_properties)
		m_mesh_properties_node = XmlElement(m_mesh_node,"properties");
	else
		m_event_properties_node = XmlElement(m_event_node,"properties");

}

void
IXMMeshWriter::
write(IVariable* ivar, IData* idata)
{
	Trace::Setter trace(m_trace_mng,"IXMMeshWriter");
	m_trace_mng->debug() << "Write Variable " << ivar->name();

	IXMPropertyInfo* prop = IXMPropertyInfo::create(ivar);
	if (m_format == "hdf" || m_format == "split_hdf") prop->setPath(m_hdf_path,m_directory_name);
	if (m_write_mesh_properties) {
		prop->writeProperty(m_mesh_properties_node,m_format,fileinfo,m_write_only_active_cells);
		fileinfo.supportSet.clear();
	}
	else
		prop->writeProperty(m_event_properties_node,m_format,fileinfo,m_write_only_active_cells,m_begin_time);

}


// write groups Element
/**********************************************************************
 * [writeGroups]
 **********************************************************************/
void
IXMMeshWriter::
writeGroups(XmlNode current_node, const String format ){

	if(m_groups.empty())
		return;

	for( ItemGroupCollection::Enumerator i(m_groups); ++i; ){
		ItemGroup group(*i);
		Arcane::String group_name = group.name(); // Necessary cause group = group.own() changes group name (=> MyGroup_Own)
		if (group.null()) // Le groupe n'est pas du type souhait���.
			continue;
		if (!group.isOwn()) // Si possible, prend le groupe d'���l���ments propres
			group = group.own();

		XmlElement group_node(current_node,"group");
		groupElement(group, group_name, format, group_node );
	}

}

// group Element
/**********************************************************************
 * [groupElement]
 **********************************************************************/
void
IXMMeshWriter::
groupElement(const ItemGroup& group, const String group_name, const String format, XmlNode current_node ){

	IParallelMng* pm=  m_mesh->subDomain()->parallelMng();
	Int64SharedArray group_uids(group.size());
  // Get group uids (or aids if mapping exist)
  ArcGeoSim::IXMLidToUidMng lid2uid(m_mesh);
  lid2uid.itemsUniqueId(group_uids,group.view(),group.itemKind());

//  ENUMERATE_ITEM(iitem, group){
//		group_uids.add(iitem->uniqueId());
//	}

	Int64SharedArray all_uids;
	if(pm->isParallel()){
		  pm->gatherVariable(group_uids.constView(),all_uids , pm->masterIORank());
		  group_uids= all_uids;
	}


	String _kind = ArcGeoSim::IXMTools::itemKindIXMName(group.itemKind());
	_kind = _kind.lower();
	current_node.setAttrValue("name",group_name);
	current_node.setAttrValue("kind",_kind);

	String event_name;

	if(getGrandParentName(current_node) == "mesh")
		event_name="mesh";
	else
		event_name= m_event_node.attrValue("name");

	if(pm->isMasterIO())
	{
		XmlElement group_ids_node(current_node,"group-ids");

		if(m_format == "xml")
		{
			IXM4DataNode<Int64>::createIXM4DataNode(group_ids_node,group_uids);
		}
		else
		{
			bool writeHDFFile = false;

			if (  fileinfo.supportSet.find(group_name) == fileinfo.supportSet.end() )
			{
				fileinfo.supportSet.insert(group_name);
				writeHDFFile = true;
			}

			const String gparent_name=getGrandParentName(current_node);
			//String hdata_ids(String::format("{0}/{1}/{2}/{3}/group-ids",m_hdf_path,gparent_name,current_node.name(),group_name));
			String hdata_ids(String::format("{0}/{1}/groups/{2}/group-ids",m_hdf_path,event_name,group_name));
			IXM4DataNode<Int64>::createIXM4DataNode(group_ids_node,group_uids, HDFFormatBuildInfo(hdata_ids,m_directory_name,writeHDFFile));
		}
	}

}

//
/**********************************************************************
 * [getGrandParentName]
 **********************************************************************/
const String
IXMMeshWriter::
getGrandParentName(XmlNode currentNode){

	XmlNode parent= currentNode.parent();
	XmlNode gparent= parent.parent();
	return gparent.name();
}

//
/**********************************************************************
 * [createIxmFile]
 **********************************************************************/
void
IXMMeshWriter::
createIxmFile(const String& file_name, const String& format)
{
  /****************************
   * IXM-side initialisation *
   ****************************/
  m_file_name=file_name;
  //bool separate_hdf_files = true;

  m_trace_mng->info() << "createIxmFile m_format: " << m_format;

  if(m_format== "hdf" || m_format == "split_hdf")
    {
      String hdf_file_name_with_path = hdfFileName();
      Arcane::StringSharedArray str_array;
      hdf_file_name_with_path.split(str_array,'/');
      String hdf_file_name = str_array.back();
      m_trace_mng->info() << "hdf_file_name: " << hdf_file_name;
      //
      str_array.clear();
      String event_hdf_file_name;
      hdf_file_name.split(str_array,'.');

      event_hdf_file_name = String::format("{0}{1}.h5",str_array[0],m_event_number+1);
      m_trace_mng->info() << "event_hdf_file_name: " << event_hdf_file_name;

      // m_hdf_path is relative to ixm file

      if (m_format == "split_hdf")
    	  m_hdf_path=String::format("{0}:",event_hdf_file_name);
      else
    	  m_hdf_path=String::format("{0}:",hdf_file_name);

      m_trace_mng->info() << "HDF PATH: " << m_hdf_path;

      Hdf5Utils::HFile hdf_file;
      if(m_mesh->subDomain()->parallelMng()->isMasterIO())
        {
    	  // File creation : path relative to execution path
    	  if (m_format == "split_hdf")
    		  hdf_file.openTruncate(Arcane::String::concat(m_directory_name,"/",event_hdf_file_name));
    	  else
    	  {
    		  if(m_event_number == 0)
    			  hdf_file.openTruncate(Arcane::String::concat(m_directory_name,"/",hdf_file_name));
    		  else
    			  hdf_file.openAppend(Arcane::String::concat(m_directory_name,"/",hdf_file_name));

    	  }
        }
  }
}


/**********************************************************************
 * [createIxmDoc]
 **********************************************************************/
void
IXMMeshWriter::
createIxmDoc()
{
	//m_ixm_doc = m_ressource_mng->createXmlDocument();
	XmlNode ixm_doc = m_ixm_doc->documentNode();
	// Create ixm root node with supported version
	XmlElement ixm_root(ixm_doc,"ixm");
	ixm_root.setAttrValue("version",m_supported_version);
	// Create unstructured-grid node
	XmlElement ixm_grid_tmp(ixm_root,"unstructured-grid");

	// Get mesh dimension - kind of brute version : this is the behavior of ArcTem and affiliates (user of ixmV4)
	// For a smart dimension handling move to ixmV5.

	if (m_mesh->dimension() == 3) ixm_grid_tmp.setAttrValue("dimension", "3Dxyz");
	else if (m_mesh->dimension() == 2)
	{
	  if(!m_mesh_type.empty())
	    ixm_grid_tmp.setAttrValue("dimension", m_mesh_type);
	  else
	    ixm_grid_tmp.setAttrValue("dimension", "2Dyz");
	}
	// If 1D : do not export dimension attribute.

	m_grid_node=ixm_grid_tmp;


}

/**********************************************************************
 * [loadIxmDoc]
 **********************************************************************/
void
IXMMeshWriter::
loadIxmDoc()
{
	m_mesh->subDomain()->parallelMng()->barrier();

	String ixm_file=ixmFileName();
	/*
 	Arcane::ByteArray bytes;
	if(m_mesh->subDomain()->parallelMng()->isMasterIO()){
		m_ixm_doc = m_io_mng->parseXmlFile(ixm_file, "ixm_v4.xsd",
	  ByteConstArrayView(strlen((const char*) ixm_v4_xsd), ixm_v4_xsd));
		Arcane::OStringStream oss;
		Arcane::domutils::writeNode(oss(),(*m_ixm_doc).documentNode().domNode());
		bytes = oss.str().utf8();
	}

	m_mesh->subDomain()->parallelMng()->broadcast(bytes.view(),m_mesh->subDomain()->parallelMng()->masterIORank());
	//if(!m_mesh->subDomain()->parallelMng()->isMasterIO())
		//m_ixm_doc = m_io_mng->parseXmlBuffer(bytes,"ixm");
	 */
	// suppose l'acces a tous les procs au systeme de fichiers
	//m_ixm_doc = m_io_mng->parseXmlFile(ixm_file, "ixm_v4.xsd",
	//	  ByteConstArrayView(strlen((const char*) ixm_v4_xsd), ixm_v4_xsd));

	dom::DOMImplementation domimp;
	const char* cschemaname = 0;
	m_ixm_doc =  domimp._load(ixm_file,m_trace_mng,cschemaname);

	// Check
	if (m_ixm_doc == NULL)
		m_trace_mng->fatal() << "Could not load Ixm file " << ixm_file;
}


/**********************************************************************
 * [updateIxmDoc]
 **********************************************************************/
void
IXMMeshWriter::
updateIxmDoc(const Real& begin_time, const Real& end_time, const Integer& event_number)
{
	// Get event parent node
	XmlNode root_node = m_ixm_doc->documentNode();
	XmlNode ixm_node = root_node.child("ixm");
	XmlNode grid_node = ixm_node.child("unstructured-grid");

	// Create event tag in global events doc
	XmlElement current_event_node(grid_node,"event");

	// Set event tag attributes
	current_event_node.setAttrValue("name",String::format("Event{0}",event_number+1));

	const String btime = String::format("{0}",begin_time);
	const String etime = String::format("{0}",end_time);
	current_event_node.setAttrValue("begin-time",btime);
	current_event_node.setAttrValue("end-time",etime);
	m_event_node=current_event_node;
}

/**********************************************************************
 * [updateIxmDoc]
 **********************************************************************/
void
IXMMeshWriter::
updateIxmDoc(const Integer& event_number)
{
    /********************
     * Output & cleanup *
     ********************/
    bool is_master = m_mesh->subDomain()->parallelMng()->isMasterIO();
    if(is_master){
        ByteSharedArray xml_bytes;
        m_ixm_doc->save(xml_bytes);
        m_io_mng->writeXmlFile(m_ixm_doc,ixmFileName(event_number),true);
        m_ixm_doc->createNull();
    }

    // Get event parent node
    XmlNode root_node = m_ixm_doc->documentNode();
    XmlNode ixm_node = root_node.child("ixm");
    XmlNode grid_node = ixm_node.child("unstructured-grid");

    m_grid_node = grid_node;
}


/**********************************************************************
 * [baseFileName]
 **********************************************************************/
const String
IXMMeshWriter::
baseFileName() const
{
	// File name
	return String::format("{0}/{1}",m_directory_name,m_file_name);
}

/**********************************************************************
 * [IxmFileName]
 **********************************************************************/
const String
IXMMeshWriter::
ixmFileName() const
{
	// File name
	return String::format("{0}/{1}.ixm",m_directory_name,m_file_name);
}

/**********************************************************************
 * [IxmFileName]
 **********************************************************************/
String
IXMMeshWriter::
ixmFileName(const Integer& event_number)
{
    // File name
    return String::format("{0}/{1}{2}.ixm",m_directory_name,m_file_name,event_number);
}


/**********************************************************************
 * [hdf5FileName]
 **********************************************************************/
const String
IXMMeshWriter::
hdfFileName() const
{
	// File name
//	return String::format("{0}/{1}.h5",m_directory_name,m_file_name);
	return String::format("{0}.h5",m_file_name); // SDC hdf file is relative to ixm file => no directory
}

void
IXMMeshWriter::
getMeshType(const Arcane::String& mesh_type)
{
  m_mesh_type = mesh_type;
}



