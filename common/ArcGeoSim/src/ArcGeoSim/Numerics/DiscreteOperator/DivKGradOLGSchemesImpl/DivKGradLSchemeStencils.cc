#include "arcane/VariableTypedef.h"
#include <map>
#include "ArcGeoSim/Utils/ItemGroupMap.h"
#include "ArcGeoSim/Utils/ItemComparator.h"
#include "DivKGradLSchemeStencils.h"

using namespace Arcane;

LScheme_CenteredStencil::LScheme_CenteredStencil(const Node& node, Integer icell, CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell,VariableNodeReal3 coords)
{
//#ifndef NO_USER_WARNING
//#ifndef WIN32
////#warning "CHECK: If the edges are available in Arcane, std::map should will be eliminated."
//#endif
//#endif
  std::map<Node, Integer, ItemComparator> EdgeMap;
  // const Cell & cell = node.cell(icell);
  FaceNum & face_local = NumLocfacesbyCell[icell];
  numfaces = face_local.size();
  numcells = numfaces+1;
  transm.resize(numfaces , numcells);
	NumLocEdgebyCell.resize(numfaces,2);
  cells.resize(numcells);
  faces.resize(numfaces);

  cells[0] = icell;

  for (Integer i=0; i<numfaces; i++) {
    // find cells and faces
    Integer numloc_face = face_local[i];
    faces[i] = numloc_face;
    const Face & face = node.face(numloc_face);
    if (face.isSubDomainBoundary()){
      cells[i+1] = -1;
    }
    else{
      const Cell cellback      = face.backCell();
      const Cell cellfront      = face.frontCell();
      Integer numloc_cellback  = NumLocCell[cellback];
      Integer numloc_cellfront = NumLocCell[cellfront];
      if (numloc_cellback==cells[0])
        cells[i+1] = numloc_cellfront;
      else
        cells[i+1] = numloc_cellback;
    }

    // Generate the edges data
    Integer nbnode = face.nbNode();
    Integer loc_node;
    for (loc_node=0; loc_node<nbnode;  loc_node++)
      if (face.node(loc_node) == node)
	      break;
    //Find the neighbouring nodes
    const Node & neighb_node1 = face.node((loc_node+1)%nbnode);
    const Node & neighb_node2 = face.node((loc_node-1+nbnode)%nbnode);
    if ( EdgeMap.find(neighb_node1)==EdgeMap.end())
      EdgeMap[neighb_node1] = EdgeMap.size() - 1;
    if (EdgeMap.find(neighb_node2)==EdgeMap.end())
      EdgeMap[neighb_node2] = EdgeMap.size() - 1;
    NumLocEdgebyCell[i][0] = EdgeMap[neighb_node1];
    NumLocEdgebyCell[i][1] = EdgeMap[neighb_node2];
  }
  //Calculate the edge centers
  numedges = EdgeMap.size();
  EdgeCenters.resize(numedges);
  for (std::map<Node, Integer, ItemComparator>::const_iterator p=EdgeMap.begin(); p!=EdgeMap.end() ; p++)
    {
      EdgeCenters[p->second].x = 0.5*( coords[node].x + coords[p->first].x);
      EdgeCenters[p->second].y = 0.5*( coords[node].y + coords[p->first].y);
      EdgeCenters[p->second].z = 0.5*( coords[node].z + coords[p->first].z);
    }
}

void L_Stencil::setCell(Integer i , Integer numcell){
  cells[i] = numcell;
}

Integer L_Stencil::getCell(Integer i){
  return cells[i];
}

void L_Stencil::setFace(Integer i , Integer numface){
  faces[i] = numface;
}

Integer L_Stencil::getFace(Integer i){
  return faces[i];
}

L_Stencil::L_Stencil(const Node& node, Integer icell, CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell){
  cells.resize(4);
  faces.resize(3);
  transm.resize(3,4);
  // const Cell & cell = node.cell(icell);
  FaceNum & face_local = NumLocfacesbyCell[icell];
  cells[0] = icell;
  for (Integer i = 0; i < 3; i++) {
    // find cells and faces
    Integer numloc_face = face_local[i];
    faces[i] = numloc_face;
    const Face & face = node.face(numloc_face);
    if (!face.isSubDomainBoundary()){
      const Cell cellback = face.backCell();
      if (cellback == node.cell(cells[0])){
        const Cell cellfront = face.frontCell();
        cells[i+1] = NumLocCell[cellfront];
      }
      else{
        cells[i+1] = NumLocCell[cellback];
      }
    }
    else{
      cells[i+1] = -1;
    }
  }
}
L_Stencil::L_Stencil(const Node& node, Integer icell,
                     ItemVectorT<Cell> node_cells,
                     CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell){
  cells.resize(4);
  faces.resize(3);
  transm.resize(3,4);
  // const Cell & cell = node.cell(icell);
  FaceNum & face_local = NumLocfacesbyCell[icell];
  cells[0] = icell;
  for (Integer i = 0; i < 3; i++) {
    // find cells and faces
    Integer numloc_face = face_local[i];
    faces[i] = numloc_face;
    const Face & face = node.face(numloc_face);
    if (!face.isSubDomainBoundary()){
      const Cell cellback = face.backCell();
      if (cellback == node_cells[cells[0]]){
        const Cell cellfront = face.frontCell();
        cells[i+1] = NumLocCell[cellfront];
      }
      else{
        cells[i+1] = NumLocCell[cellback];
      }
    }
    else{
      cells[i+1] = -1;
    }
  }
}
L_Stencil::L_Stencil(const Node& node, Integer icell, Integer iface, CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell){
  cells.resize(4);
  faces.resize(3);
  transm.resize(3,4);  
  // const Cell & cell = node.cell(icell);
  FaceNum & face_local = NumLocfacesbyCell[icell];
  cells[0] = icell;
  for (Integer i = 0; i < 3; i++) {
    // find cells and faces
    Integer numloc_face = face_local[(iface+i)%4];
    faces[i] = numloc_face;
    const Face & face = node.face(numloc_face);
    if (!face.isSubDomainBoundary()){
      const Cell cellback = face.backCell();
      if (cellback == node.cell(cells[0])){
        const Cell cellfront = face.frontCell();
        cells[i+1] = NumLocCell[cellfront];
      }
      else{
        cells[i+1] = NumLocCell[cellback];
      }
    }
    else{
      cells[i+1] = -1;
    }
  }
}




