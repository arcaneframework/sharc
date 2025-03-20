#ifndef ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_IXMSTATICMESHCHECKER_H
#define ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_IXMSTATICMESHCHECKER_H


#include <arcane/utils/ITraceMng.h>
#include <arcane/ItemTypeMng.h>


#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IIXMMeshChecker.h"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>


BEGIN_ARCGEOSIM_NAMESPACE

USING_IXM_NAMESPACE

struct CellElt
{
	int id;
	Arcane::ItemTypeInfo* type;
	int nbNodes;
	Arcane::Int64SharedArray nodes;
};

struct FaceElt
{
	int id;
	int index;
	int type;
	Arcane::Int64SharedArray nodes;
};

typedef std::vector<int>::iterator iter;

template<typename T, typename InputIterator>
void Print(std::ostream& ostr,
           InputIterator itbegin,
           InputIterator itend,
           const std::string& delimiter)
{
    std::copy(itbegin,
              itend,
              std::ostream_iterator<T>(ostr, delimiter.c_str()));
}

class IXMStaticMeshChecker: public IIXMMeshChecker
{

public :

  /** Constructeur de la classe */
  IXMStaticMeshChecker(IXMCellData& cell_data, IXMNodeData& node_data, IXMFaceData& face_data, IXMPropertyDataList& property_data_list, IXMGroupDataList& group_data_list, Arcane::ITraceMng* trace_mng);

  /** Destructeur de la classe */
  virtual ~IXMStaticMeshChecker() {}

  void initNodes();
  void initFaces();
  void initCells();

  void initialize();

  void run();

  void printNodes();

  void printFaces();

  void printCells();

  void print();

  void checkDuplicatedCellIds();

  std::set<int> getCellNodeSet(int cellindex);

  std::set<int>  getNodesetFaceElt(FaceElt elt);

  std::set<int> getFaceNodeSetFromId(int faceid);

  std::set<int> getFaceNodeSet(int faceindex);

  int getFaceIdFromNodeSet(std::set<int> nodecontent);

  int* getCellNodes(int cellindex);

  void checkDuplicatedCellContents();

  void checkIsolatedFaces();

  void checkIsolatedNodes();

  void checkCellWithDuplicatedNodes();

  void removeFaceFromId(int faceid);

  void removeFaceFromIndex(int faceindex);

  //void removeCell(int cellid);

  void removeCell(int cellindex);

  void renameCell(int cellindex);

  void removeNodeFromId(int nodeid);

  void removeNode(int nodeindex);

private:

IXMCellData _celldata;
IXMNodeData _nodedata;
IXMFaceData _facedata;
IXMPropertyDataList _propertydatalist;
IXMGroupDataList _groupdatalist;

// informations about cells
Arcane::Int64SharedArray _cells_ids;
Arcane::Int64SharedArray _cells_types;
Arcane::Int64SharedArray _cellnodes_ids;

// informations about nodes
Arcane::Int64SharedArray _nodes_ids;
Arcane::Real3SharedArray _nodes_xyz;

// informations about faces
Arcane::Int64SharedArray _faces_ids;
Arcane::IntegerSharedArray _faces_types;
Arcane::Int64SharedArray _facenodes_ids;

Arcane::ITraceMng* _trace_mng;

// declaration list of cells
std::vector<CellElt> celllist;

// declaration set of cell ids
std::set<long> setCellIds;

// declaration list of faces
std::vector<FaceElt> facelist;

// declaration set of face ids
std::set<int> setFaceIds;

// declaration set of node ids
std::set<int> setNodeIds;

// declaration map of face ids
std::map<unsigned,unsigned> mapFaceIds;

Arcane::ItemTypeMng * _typeMng;

};

END_ARCGEOSIM_NAMESPACE

#endif /* ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_IXMSTATICMESHCHECKER_H */
