#include "IXMStaticMeshChecker.h"


#include <arcane/utils/FatalErrorException.h>


using namespace ArcGeoSim::IXMTools;





ArcGeoSim::IXMStaticMeshChecker::IXMStaticMeshChecker(IXMCellData& cell_data, IXMNodeData& node_data,IXMFaceData& face_data, IXMPropertyDataList& property_data_list, IXMGroupDataList& group_data_list, Arcane::ITraceMng* trace_mng)
: _celldata(cell_data)
, _nodedata(node_data)
, _facedata(face_data)
, _propertydatalist(property_data_list)
, _groupdatalist(group_data_list)
, _trace_mng(trace_mng)
{
  _typeMng = Arcane::ItemTypeMng::singleton();
  initialize();


  // properties

  /*

	 _trace_mng->info() << "_propertydatalist.size =" << _propertydatalist.size();

	 std::list<IXMPropertyData>::iterator
	 	    lit1 (_propertydatalist.begin()),
	 	    lend1(_propertydatalist.end());

	 IXMItemData<IXMPropertyValuesData> support_and_values;

	 for(;lit1!=lend1;++lit1)
	 {
	 		trace_mng->info() << "data_array_size = " << (*lit1).data_array_size;
	 		trace_mng->info() << "data_type = " << (*lit1).data_type;
	 		trace_mng->info() << "group_name = " << (*lit1).group_name;
	 		trace_mng->info() << "kind = " << (*lit1).kind;
	 		trace_mng->info() << "Name = " << (*lit1).name;

	 		support_and_values = (*lit1).support_and_values;

	 		//support_and_values.item_ids;

	 		trace_mng->info() << "support_and_values.item_ids.size() = " << support_and_values.item_ids.size();

	 		for(int j=0;j<support_and_values.item_ids.size();j++)
	 		{
	 			trace_mng->info() << "support[" << j << "] = " << support_and_values.item_ids[j];
	 		}

	 		//Arcane::Int64Array item_ids = support_and_values.item_ids.size();

	 		IXMPropertyValuesData _item_infos = support_and_values.item_infos;

	 		// 		for(int j=0;j<support_and_values.item_ids.size();j++)
	 		//		{

	 		//	    trace_mng->info() << "j = " << j << "_item_infos.IXMPropertyValuesData() = " << _item_infos.;

	 		//		}

	 		// support_and_values.item_infos;

	 		// trace_mng->info() << "support_and_values.item_infos.size() = " << support_and_values.item_infos.;


	 }
   */
}


void ArcGeoSim::IXMStaticMeshChecker::initNodes()
{
  _nodes_ids = _nodedata.item_ids;
  _nodes_xyz = _nodedata.item_infos;

  int nbNodes = _nodes_ids.length();

  for(int i=0;i<nbNodes;i++)
    {
      setNodeIds.insert(_nodes_ids[i]);
    }

}

void ArcGeoSim::IXMStaticMeshChecker::initFaces()
{
  // informations about faces
  _faces_ids = _facedata.faces.item_ids;
  _faces_types = _facedata.faces.item_infos;
  _facenodes_ids = _facedata.node_ids;


  facelist.clear();

  setFaceIds.clear();

  // remplissage liste des faces

  int length = _faces_ids.size();

  int cumulatedNodes=0;

  for(int i=0;i<length;i++)
    {
      FaceElt face;
      face.id = _faces_ids[i];
      face.index = i;
      face.type = _faces_types[i];

      setFaceIds.insert(face.id);

      mapFaceIds[face.id] = i;

      // _trace_mng->info() << "face ( " << face.id << ") type = " << face.type << "Nodes =" ;

      for (int j=0;j<face.type;j++)
        {
          face.nodes.add(_facenodes_ids[cumulatedNodes+j]);
          // _trace_mng->info() << _facenodes_ids[cumulatedNodes+j];
        }

      facelist.push_back(face);
      cumulatedNodes += face.type;
    }

}



void ArcGeoSim::IXMStaticMeshChecker::initCells()
{
  celllist.clear();

  // id informations about cells
  _cells_ids = _celldata.item_ids;

  // clear setCellIds
  setCellIds.clear();

  // list of cells
  // _cell_types
  // _cellnodes_ids

  int length = _cells_ids.size();


  int counterInfos = -1;

  for(int i=0;i<length;i++)
    {
      CellElt cell;

      counterInfos +=1;

      //_cells_types.add(_celldata.item_infos[counterInfos]);

      Arcane::ItemTypeInfo* current_cell_type = _typeMng->typeFromId(_celldata.item_infos[counterInfos]);

      cell.type = current_cell_type;

      counterInfos += 1;

      int cellId = _celldata.item_infos[counterInfos];

      setCellIds.insert(cellId);

      cell.id = cellId;

      int nbNodes = current_cell_type->nbLocalNode();

      cell.nbNodes = nbNodes;

      for (int j=0;j<nbNodes;j++)
        {
          counterInfos += 1;
          cell.nodes.add(_celldata.item_infos[counterInfos]);
        }

      celllist.push_back(cell);
    }



}

void ArcGeoSim::IXMStaticMeshChecker::initialize()
{
  initNodes();
  initCells();
  initFaces();
}



void ArcGeoSim::IXMStaticMeshChecker::printNodes()
{


  _trace_mng->info() << '\n';

  _trace_mng->info() << "-------- NODES  -----------";

  int nbNodes = _nodes_ids.length();

  for(int i=0;i<nbNodes;i++)
    {
      _trace_mng->info() <<  "Node = " << _nodes_ids[i] << "  " << _nodes_xyz[i];
    }

}

void ArcGeoSim::IXMStaticMeshChecker::printFaces()
{
  _trace_mng->info() << '\n';

  _trace_mng->info() << "-------- FACES  -----------";

  // print setFaceIds

  /*
	_trace_mng->info() << "setFaceIds";

	std::set<int>::iterator
	lit1 (setFaceIds.begin()),
	lend1(setFaceIds.end());

	for(;lit1!=lend1;++lit1)
	{
		_trace_mng->info() << "face id =" << *lit1;

	}
   */

  FaceElt elt;

  std::vector<FaceElt>::iterator itFace (facelist.begin()), endFace(facelist.end());

  for(;itFace!=endFace;++itFace)
    {
      elt = *itFace;

      _trace_mng->info() << '\n';

      _trace_mng->info() << "Informations for face id =" << elt.id;

      _trace_mng->info() << "Type =" << elt.type;

      _trace_mng->info() << "Nodes =" ;

      for (int j=0;j<elt.type;j++)
        {
          _trace_mng->info() << elt.nodes[j];
        }
    }


  //  print mapFaceIds


  std::map<unsigned,unsigned>::const_iterator
  mit (mapFaceIds.begin()),
  mend(mapFaceIds.end());

  for(;mit!=mend;++mit) {
      _trace_mng->info() << mit->first << '\t' << mit->second;
  }

}

void ArcGeoSim::IXMStaticMeshChecker::printCells()
{

  _trace_mng->info() << '\n';

  _trace_mng->info() << "-------- CELLS  -----------";

  // print list of cells

  _trace_mng->info() << "Number of Cells =" << celllist.size();

  CellElt elt;

  std::vector<CellElt>::iterator itCell (celllist.begin()), endCell(celllist.end());

  for(;itCell!=endCell;++itCell)
    {
      elt = *itCell;

      _trace_mng->info() << '\n';

      _trace_mng->info() << "Informations for cell id =" << elt.id;

      _trace_mng->info() << "cell type =" << itCell->type->typeName();
      _trace_mng->info() << "cell number nodes =" << itCell->nbNodes;

      _trace_mng->info() << "type =" << elt.type->typeName();

      _trace_mng->info() << "number nodes =" << elt.nbNodes;

      _trace_mng->info() << "type =" << elt.type->nbLocalFace();

      for (int j=0;j<elt.type->nbLocalFace();j++)
        {
          _trace_mng->info() << "Face[" << j << "] nbNode =" << elt.type->localFace(j).nbNode();

          _trace_mng->info() << "Face[" << j << "] nbEdge =" << elt.type->localFace(j).nbEdge();

          _trace_mng->info() << "Face[" << j << "] typeId =" << elt.type->localFace(j).typeId();

          for (int k=0;k<elt.type->localFace(j).nbNode();k++)
            {
              _trace_mng->info() << "local face node[" << k << "] = " << elt.type->localFace(j).node(k);
              _trace_mng->info() << "global face node[" << k << "] = " << elt.nodes[elt.type->localFace(j).node(k)];
            }

        }

      /*for (int j=0;j<itCell->nbNodes;j++)
		{
			_trace_mng->info() << "cell node =" << itCell->nodes[j];
		}*/

    }

  // print list of cell ids

  /*
	_trace_mng->info() << '\n';

	_trace_mng->info() << "cell ids size =" << setCellIds.size();

	std::set<long>::iterator
	lit2 (setCellIds.begin()),
	lend2 (setCellIds.end());

	_trace_mng->info() << "cell ids size =" << setCellIds.size();

	for(;lit2!=lend2;++lit2)
	{
		_trace_mng->info() << "cell id =" << *lit2;
	}
   */

}


void ArcGeoSim::IXMStaticMeshChecker::print()
{
  _trace_mng->info() << "--------PRINT IXMMESHCHECKER -----------";

  printNodes();

  printFaces();

  printCells();

  // _trace_mng->fatal() << "End of tests ";

}

void ArcGeoSim::IXMStaticMeshChecker::run()
{
  initialize();

  if ( celllist.size() < 100 )
    {

      checkDuplicatedCellContents();

      checkDuplicatedCellIds();

      checkCellWithDuplicatedNodes();

      checkIsolatedNodes();

      checkIsolatedFaces();

    }

}

// D�finition de notre structure � trier
struct A
{
  A(int id, int index) : id(id), index(index) {}

  int id;
  int index;
};

// D�finition du foncteur servant � trier nos objets selon l'id
struct SortById
{
  bool operator ()(const A& a1, const A& a2) const
  {
    return a1.id < a2.id;
  }
};

// Surcharge de l'op�rateur << pour afficher nos A
std::ostream& operator <<(std::ostream& Stream, const A& a)
{
  return Stream << a.id << " " << a.index;
}


void ArcGeoSim::IXMStaticMeshChecker::checkDuplicatedCellIds()
{
  // first step: creation of a multiset

  std::multiset<A,SortById> setIds; // set of ordered ids

  int length = _cells_ids.size();
  for(int i = 0;i< length;i++)
    {
      setIds.insert(A(_cells_ids[i],i));
    }

  //_trace_mng->info() << "Set IDs" << setIds.size();

  //std::copy(setIds.begin(), setIds.end(), std::ostream_iterator<A>(std::cout, "\n"));

  //_trace_mng->info() << "End Set IDs";

  Arcane::Int32SharedArray identicalnodes; // array of superfluous cell id array indexes to suppress
  identicalnodes.clear();

  int id0,id1;

  for (std::multiset<A,SortById>::iterator it = setIds.begin(); it != setIds.end(); )
    {
      id0 = ( ( A ) *it).id;

      // _trace_mng->info() << "id0 =" << id0;

      bool sameid = true;
      int nb = 1;
      //int index = ( ( A ) *it).index;

      std::multiset<A,SortById>::iterator it0 = it;
      it0++;

      while (sameid  && (it0 != setIds.end()) )
        {
          id1 = ( ( A ) *it0).id;
          // _trace_mng->info() << "id1 =" << id1;

          if ( id0 == id1)
            {
              nb++;
              it0++;
            }
          else
            {
              sameid = false;
            }
        }
      if (nb > 1)
        {
          it0=it;
          it0++;
          for (int i=1;i<nb;i++)
            {
              identicalnodes.add(( ( A ) *it0).index);
              // _trace_mng->info() << "index =" << ( ( A ) *(it0++)).index;
              it0++;
            }
        }

      /*id0 = ( ( A ) *it).id;
		_trace_mng->info() << a.id;
		_trace_mng->info() << a0.index;
		it++;*/

      it++;

    }


  /*
	Arcane::Int32Array identicalnodes; // array of superfluous cell id array indexes to suppress

	std::set<int> setIds; // set of ids found one time or more

	identicalnodes.clear();

	// looking for duplicates
	int length = _cells_ids.size();


	int i = 0;
	int index;

	while ( i < length )
	{
		index = _cells_ids[i];
		if ( setIds.find(index) == setIds.end() )  // new index
		{
			int j = 0;
			while ( j < length)  // research of duplicates
			{
				if ( j != i && _cells_ids[j] == index)
				{
					identicalnodes.add(j);
				}
				j++;
			}
			setIds.insert(index);
		}
		i++;
	}
   */

  // suppression of duplicates

  /*
	int nbIdenticalNodes = identicalnodes.size();

	int i = nbIdenticalNodes - 1 ;
	while ( i >= 0 )
		{
			removeCell(identicalnodes[i]);
			i--;
		}
   */

  // reindex duplicates



  for (int i=0;i<identicalnodes.size();i++)
    {
      renameCell(identicalnodes[i]);
    }


}


std::set<int> ArcGeoSim::IXMStaticMeshChecker::getCellNodeSet(int cellindex)
{
  std::set<int> nodecontent;

  Arcane::Int64SharedArray nodes = celllist[cellindex].nodes;

  for (int i=0;i<nodes.size();i++)
    {
      nodecontent.insert(nodes[i]);
    }
  return nodecontent;
}

int* ArcGeoSim::IXMStaticMeshChecker::getCellNodes(int cellindex)
{
  int* arrayNodes;

  Arcane::Int64SharedArray nodes = celllist[cellindex].nodes;

  arrayNodes = new int[nodes.size()];

  for (int i=0;i<nodes.size();i++)
    {
      arrayNodes[i] = nodes[i];
    }
  return arrayNodes;
}


std::set<int> ArcGeoSim::IXMStaticMeshChecker::getFaceNodeSetFromId(int faceid)
{
  std::set<int> nodecontent;

  bool found = false;

  std::vector<FaceElt>::iterator
  faceit (facelist.begin()),
  faceend(facelist.end());

  int faceindex = 0;

  while ( !found && faceit!=faceend)
    {
      if ( faceit->id == faceid )
        found = true;
      else
        {
          ++faceit;
          faceindex++;
        }
    }

  if (found)
    {
      nodecontent = getFaceNodeSet(faceindex);
    }

  return nodecontent;

}

std::set<int>  ArcGeoSim::IXMStaticMeshChecker::getNodesetFaceElt(FaceElt elt)
{
  std::set<int> nodecontent;

  int nbNodes = elt.type;

  for (int i=0;i<nbNodes;i++)
    {
      nodecontent.insert(elt.nodes[i]);
    }
  return nodecontent;
}


int ArcGeoSim::IXMStaticMeshChecker::getFaceIdFromNodeSet(std::set<int> nodecontent)
{
  int faceid = -1;

  bool found = false;

  std::set<int> nodecandidate;

  std::vector<FaceElt>::iterator faceit (facelist.begin()), faceend(facelist.end());

  while ( !found && faceit!=faceend)
    {
      nodecandidate = getNodesetFaceElt(*faceit);

      if ( nodecontent == nodecandidate )
        {
          found = true;
        }
      else
        {
          ++faceit;
        }
    }

  if (found)
    {
      faceid = faceit->id;
    }

  return faceid;

}


std::set<int> ArcGeoSim::IXMStaticMeshChecker::getFaceNodeSet(int faceindex)
{
  std::set<int> nodecontent;

  Arcane::Int64SharedArray nodes = facelist[faceindex].nodes;

  // std::cout << "Face index = " << faceindex << std::endl << "Nodes = " ;

  for (int i=0;i<nodes.size();i++)
    {
      // std::cout << nodes[i] << " ";
      nodecontent.insert(nodes[i]);
    }

  // std::cout << std::endl;

  return nodecontent;
}

// D�finition de notre structure � trier
struct B
{
  B(int *_nodeids, int _index)
  {

    size = sizeof(_nodeids);
    nodeids = _nodeids;
    index = _index;

  }

  int size;
  int* nodeids;
  int index;
};

// D�finition du foncteur servant � trier nos objets
struct SortCell
{
  bool operator ()(const B& b1, const B& b2) const
  {
    bool lower;

    lower = b1.size < b2.size ;
    if (b1.size == b2.size)
      {
        for (int i=0;i<b1.size;i++)
          {
            lower = lower + (b1.nodeids[i] < b2.nodeids[i]);
          }
      }
    return lower;
  }
};

// Surcharge de l'op�rateur << pour afficher nos B
std::ostream& operator <<(std::ostream& Stream, const B& b)
{
  String listNodes;

  for (int i=0;i<b.size;i++)
    {
      listNodes = listNodes + " " + b.nodeids[i];
    }

  return Stream << "cell size " << b.size << " nodes " << listNodes;
}


void ArcGeoSim::IXMStaticMeshChecker::checkDuplicatedCellContents()
{

  // first step: creation of a multiset

  std::multiset<B,SortCell> setIds; // set of ordered ids

  int length = _cells_ids.size();

  for(int i = 0;i< length;i++)
    {
      setIds.insert( B(getCellNodes(i),i) );
    }

  // _trace_mng->info() << "Set IDs =" << setIds.size();

  //std::copy(setIds.begin(), setIds.end(), std::ostream_iterator<B>(std::cout, "\n"));

  // _trace_mng->info() << "End Set IDs";



  Arcane::Int32SharedArray identicalnodes; // array of superfluous cell id array indexes to suppress
  identicalnodes.clear();

  int index0,index1;

  //B b0,b1;

  for (std::multiset<B,SortCell>::iterator it = setIds.begin(); it != setIds.end(); )
    {
      //b0 = *it;

      index0 = ( ( B ) *it).index;

      // _trace_mng->info() << "index0 =" << index0;

      bool samecontent = true;
      int nb = 1;
      //int index = b0.index;

      std::multiset<B,SortCell>::iterator it0 = it;

      it0++;

      while (samecontent && (it0 != setIds.end()) )
        {

          index1 = ( ( B ) *it0).index;

          // _trace_mng->info() << "index1 =" << index1;

          if ( getCellNodeSet(index0) == getCellNodeSet(index1))
            {
              nb++;
              it0++;
            }
          else
            {
              samecontent = false;
            }
        }
      if (nb > 1)
        {
          it0=it;
          it0++;
          for (int i=1;i<nb;i++)
            {
              int ind = ( ( B ) *(it0)).index;
              identicalnodes.add(ind);
              // _trace_mng->info() << "index to remove =" << ind;
              it0++;
            }
        }

      /*id0 = ( ( A ) *it).id;
				_trace_mng->info() << a.id;
				_trace_mng->info() << a0.index;
				it++;*/

      it++;

    }

  /*
	std::set<int> setIds; //array of superfluous cell id array indexes to suppress

	// looking for duplicated content
	int length = _cells_ids.size();

	int i = 0;
	//int index;

	// content of a node
	std::set<int> nodecontentref;
	std::set<int> nodecontentcomp;

	while ( i < length )
	{
		// create nodecontentref with i cell
		nodecontentref = getCellNodeSet(i);
		if ( setIds.find(i) == setIds.end() )  // new index
		{
			int j = 0;
			while ( j < length)  // research of duplicates
			{
				// create nodecontentref with j cell
				nodecontentcomp = getCellNodeSet(j);
				if ( j != i && nodecontentref == nodecontentcomp )
				{
					setIds.insert(j);
				}
				j++;
			}
		}
		i++;
	}
   */

  // suppression of duplicates

  /*
	std::set<int>::iterator lit1 (setIds.begin()),lend1(setIds.end());

	for(;lit1!=lend1;++lit1)
	{
		removeCell(*lit1);
	}
   */

  int nbIdenticalNodes = identicalnodes.size();

  int i = nbIdenticalNodes - 1 ;
  while ( i >= 0 )
    {
      removeCell(identicalnodes[i]);
      i--;
    }

}

void ArcGeoSim::IXMStaticMeshChecker::checkCellWithDuplicatedNodes()
{
  std::set<int> setIds; //array of incorrect cell id array indexes to suppress

  std::set<int> setNodeIdsToSuppress;
  std::set<int> setNodeIdsToKeep;

  int length = _cells_ids.size();

  int i = 0;

  // content of a node
  std::set<int> nodecontent;

  while ( i < length )
    {
      // create nodecontentref with i cell
      nodecontent = getCellNodeSet(i);

      std::set<int>::iterator lit2 (nodecontent.begin()),lend2(nodecontent.end());

      unsigned nNodes = celllist[i].nbNodes;

      if ( nNodes != nodecontent.size() ) // some nodes are identical
        {
          setIds.insert(i);

          //setNodeIdsToSuppress.insert(lit2,lend2);

          for (;lit2!=lend2;++lit2)
            {
              setNodeIdsToSuppress.insert(*lit2);
            }
        }
      else
        {
          //setNodeIdsToSuppress.insert(lit2,lend2);
          for (;lit2!=lend2;++lit2)
            {
              setNodeIdsToKeep.insert(*lit2);
            }
        }

      i++;
    }

  // suppression of  cells with duplicated nodes

  std::set<int>::iterator lit1 (setIds.begin()),lend1(setIds.end());

  for(;lit1!=lend1;++lit1)
    {
      removeCell(*lit1);
    }

  // suppression of useless nodes

  std::vector<int> v1( setNodeIdsToSuppress.begin(), setNodeIdsToSuppress.end() );
  std::vector<int> v2( setNodeIdsToKeep.begin(), setNodeIdsToKeep.end() );

  int size1 = v1.size();
  int size2 = v2.size();

  std::vector<int> v( size1 + size2 );
  std::vector<int>::iterator it_set_diff;

  // Sort the vectors (essential)
  std::sort( v1.begin(), v1.end() );
  std::sort( v2.begin(), v2.end() );

  // The set difference of two sets is formed by the elements that are present in the first
  // set, but not in the second.

  it_set_diff = set_difference( v1.begin(), v1.end(), v2.begin(), v2.end(), v.begin());

  v.resize(it_set_diff-v.begin());

  // Print the set difference
  // Print<int, iter>( std::cout, v.begin(), it_set_diff, " " );

  // std::cout << "The difference has " << (v.size()) << " elements:\n";

  std::vector<int>::const_reverse_iterator itr;

  for (itr = v.rbegin(); itr != v.rend(); ++itr)
    {
      // if (itr != v.rbegin()) { std::cout << ' '; }
      // std::cout << *itr;
      removeNodeFromId(*itr);
    }

  /*std::sort(v.begin(), v.end(), std::greater<int>());
		for (it=v.begin(); it!=v.end(); ++it)
		{
			removeNode(*it);
		    std::cout << ' ' << *it;
		}*/

  // std::cout << '\n';

  // faces to suppress

  std::set<int> setFaceIds;

  // storage of faces to suppress

  std::set<int> facecontent;

  std::vector<FaceElt>::iterator
  faceit (facelist.begin()),
  faceend(facelist.end());


  for(;faceit!=faceend;++faceit)
    {
      // std::cout << "Face id = " << faceit->id << std::endl;

      facecontent = getFaceNodeSetFromId(faceit->id);

      std::set<int>::iterator it_end(facecontent.end());

      for (it_set_diff=v.begin(); it_set_diff!=v.end(); ++it_set_diff)
        {
          if ( facecontent.find( *it_set_diff) != it_end )
            {
              setFaceIds.insert( faceit->id );
            }
        }
    }

  // std::cout << "Number of faces to suppress = " << setFaceIds.size() << std::endl << "Faces =";

  // suppression of faces

  std::set<int>::iterator lit3(setFaceIds.begin()),lend3(setFaceIds.end());

  for(;lit3!=lend3;++lit3)
    {
      removeFaceFromId(*lit3);
      // std::cout << *lit3 << " ";
    }

  // std::cout << std::endl;

  // suppression of faces in support

  std::list<IXMPropertyData>::iterator
  lit4 (_propertydatalist.begin()),
  lend4(_propertydatalist.end());

  IXMItemData<IXMPropertyValuesData> support_and_values;

  std::set<int> setFaceSupportIndices;
  std::set<int>::const_reverse_iterator r_it;

  for(;lit4!=lend4;++lit4)
    {
      // _trace_mng->info() << "data_array_size = " << (*lit4).data_array_size;
      // _trace_mng->info() << "data_type = " << (*lit4).data_type;
      // _trace_mng->info() << "group_name = " << (*lit4).group_name;
      // _trace_mng->info() << "kind = " << (*lit4).kind;
      // _trace_mng->info() << "Name = " << (*lit4).name;

      if ( (*lit4).group_name == "AllFaces" )
        {

          support_and_values = (*lit4).support_and_values;

          //support_and_values.item_ids;

          // _trace_mng->info() << "support_and_values.item_ids.size() = " << support_and_values.item_ids.size();

          for(int j=0;j<support_and_values.item_ids.size();j++)
            {
              if ( setFaceIds.find(support_and_values.item_ids[j]) != setFaceIds.end() )
                setFaceSupportIndices.insert(j);
            }

          // _trace_mng->info() << "Suppression of Support Face Indices ==> ";



          for (r_it = setFaceSupportIndices.rbegin(); r_it != setFaceSupportIndices.rend(); ++r_it)
            {
              // if (r_it != setFaceSupportIndices.rbegin()) { std::cout << ' '; }
              // std::cout << *r_it;
              support_and_values.item_ids.remove(*r_it);
            }

          // std::cout << std::endl;

        }

    }
}


void ArcGeoSim::IXMStaticMeshChecker::checkIsolatedNodes()
{
  // set of used cell ids
  std::set<int> setUsedNodeIds;

  std::vector<CellElt>::iterator itCell (celllist.begin()), endCell (celllist.end());

  CellElt elt;

  for(;itCell!=endCell;++itCell)
    {
      elt = *itCell;

      for (int j=0;j<itCell->nbNodes;j++)
        {
          setUsedNodeIds.insert(itCell->nodes[j]);
        }
    }

  //

  std::vector<int> v1( setNodeIds.begin(), setNodeIds.end() );

  std::vector<int> v2( setUsedNodeIds.begin(), setUsedNodeIds.end() );

  int size1 = v1.size();
  int size2 = v2.size();

  std::vector<int> v( size1 + size2 );
  std::vector<int>::iterator it_set_diff;

  // Sort the vectors (essential)
  std::sort( v1.begin(), v1.end() );
  std::sort( v2.begin(), v2.end() );

  // The set difference of two sets is formed by the elements that are present in the first
  // set, but not in the second.
  it_set_diff =  set_difference( v1.begin(), v1.end(), v2.begin(), v2.end(), v.begin());


  v.resize(it_set_diff-v.begin());

  // Remove the difference

  std::vector<int>::const_reverse_iterator itr;
  for (itr = v.rbegin(); itr != v.rend(); ++itr)
    {
      //if (itr != v.rbegin()) { std::cout << ' '; }
      //std::cout << *itr;
      removeNodeFromId(*itr);
    }

  //std::cout << std::endl;

}

void ArcGeoSim::IXMStaticMeshChecker::checkIsolatedFaces()
{
  // set of used cell ids
  std::set<int> setUsedFaceIds;

  std::vector<CellElt>::iterator itCell (celllist.begin()), endCell (celllist.end());

  CellElt elt;

  for(;itCell!=endCell;++itCell)
    {
      elt = *itCell;

      for (int j=0;j<elt.type->nbLocalFace();j++)
        {
          std::set<int> nodecontent;

          for (int k=0;k<elt.type->localFace(j).nbNode();k++)
            {
              int localfacenode = elt.type->localFace(j).node(k);
              int globalfacenode = elt.nodes[localfacenode];
              nodecontent.insert(globalfacenode);
            }

          // get face id from node content

          int faceid = getFaceIdFromNodeSet(nodecontent);

          if ( faceid >= 0)
            setUsedFaceIds.insert(faceid);

        }

    }



  // print setUsedFaceIds

  /*_trace_mng->info() << "setUsedFaceIds";

	 std::set<int>::iterator
	 lit1 (setUsedFaceIds.begin()),
	 lend1(setUsedFaceIds.end());

	 for(;lit1!=lend1;++lit1)
	 {
		 _trace_mng->info() << "used face id =" << *lit1;
	 }*/

  //

  std::vector<int> v1( setFaceIds.begin(), setFaceIds.end() );

  std::vector<int> v2( setUsedFaceIds.begin(), setUsedFaceIds.end() );

  int size1 = v1.size();
  int size2 = v2.size();

  std::vector<int> v( size1 + size2 );
  std::vector<int>::iterator it_set_diff;

  // Sort the vectors (essential)
  std::sort( v1.begin(), v1.end() );
  std::sort( v2.begin(), v2.end() );

  // The set difference of two sets is formed by the elements that are present in the first
  // set, but not in the second.
  it_set_diff =  set_difference( v1.begin(), v1.end(), v2.begin(), v2.end(), v.begin());

  v.resize(it_set_diff-v.begin());


  // Remove the difference
  /*
	 std::vector<int>::const_reverse_iterator itr;
	 for (itr = v.rbegin(); itr != v.rend(); ++itr)
	 {
		 //if (itr != v.rbegin()) { std::cout << ' '; }
		 //std::cout << *itr;
		 removeFaceFromId(*itr);
		 //removeFaceFromIndex(mapFaceIds[*itr]);
	 }
   */

  // Remove the difference

  std::set<int> facetoremove;

  std::vector<int>::iterator it;

  for (it = v.begin(); it != v.end(); ++it)
    {
      facetoremove.insert(mapFaceIds[*it]);
    }

  std::set<int>::const_reverse_iterator itSet;

  for (itSet = facetoremove.rbegin(); itSet != facetoremove.rend(); ++itSet)
    {
      removeFaceFromIndex(*itSet);
    }

  //std::cout << std::endl;

  //_trace_mng->fatal() << "End of tests ";


}


void ArcGeoSim::IXMStaticMeshChecker::removeNodeFromId(int nodeid)
{

  // recherche de l'index de la face dans le tableau des ids

  bool found = false;
  int length = _nodes_ids.size();
  int nodeindex = 0;

  while ( !found && nodeindex < length)
    {
      if ( _nodes_ids[nodeindex] == nodeid )
        found = true;
      else
        nodeindex++;
    }
  if (found)
    {
      removeNode(nodeindex);
    }

}

void ArcGeoSim::IXMStaticMeshChecker::removeNode(int nodeindex)
{

  _nodes_ids.remove(nodeindex);

  _nodes_xyz.remove(nodeindex); // remove x,y,z components

}

void ArcGeoSim::IXMStaticMeshChecker::removeFaceFromId(int faceid)
{
  // recherche de l'index de la face dans le tableau des ids

  bool found = false;
  int length = _faces_ids.size();
  int i = 0;

  while ( !found && i < length)
    {
      if ( _faces_ids[i] == faceid )
        found = true;
      else
        i++;
    }
  if (found)
    {
      // compute cumulatedNodes

      int cumulatedNodes = 0;

      for(int j=0;j<i;j++)
        {
          cumulatedNodes += _faces_types[j];
        }

      // remove id
      _facedata.faces.item_ids.remove(i);

      // remove type
      int type =  _faces_types[i];
      _facedata.faces.item_infos.remove(i);

      // remove nodes
      for (int j=type-1; j>=0; j--)
        {
          _facedata.node_ids.remove(cumulatedNodes+j);
        }

      // reinitialize faces
      initFaces();
    }

}

void ArcGeoSim::IXMStaticMeshChecker::removeFaceFromIndex(int faceindex)
{
  // compute cumulatedNodes

  int cumulatedNodes = 0;

  for(int j=0;j<faceindex;j++)
    {
      cumulatedNodes += _faces_types[j];
    }

  // remove id
  _facedata.faces.item_ids.remove(faceindex);

  // remove type
  int type =  _faces_types[faceindex];
  _facedata.faces.item_infos.remove(faceindex);

  // remove nodes
  for (int j=0; j<type; j++)
    {
      _facedata.node_ids.remove(cumulatedNodes+j);
    }

}


void ArcGeoSim::IXMStaticMeshChecker::removeCell(int cellindex)
{

  // remove cellindex element from array _celldata.item_ids
  _celldata.item_ids.remove(cellindex);

  // compute good first index in array _celldata.item_infos

  int firstIndex = 0;

  Arcane::ItemTypeInfo* current_cell_type;

  for(int j=0;j<cellindex;j++)
    {
      current_cell_type = _typeMng->typeFromId(_celldata.item_infos[firstIndex]);
      firstIndex += 2 + current_cell_type->nbLocalNode();
    }


  // remove infos from array _celldata.item_infos

  current_cell_type = _typeMng->typeFromId(_celldata.item_infos[firstIndex]);

  int nEltsToSuppress = 2 + current_cell_type->nbLocalNode();
  for(int j=nEltsToSuppress-1;j>=0;j--)
    {

      _celldata.item_infos.remove(firstIndex+j);
    }

  // reinitialize cells
  initCells();

}

void ArcGeoSim::IXMStaticMeshChecker::renameCell(int cellindex)
{

  // creation of new index

  long newindex = *(--setCellIds.end()) + 1;

  // _trace_mng->info() << "old _celldata.item_ids =" << _celldata.item_ids[cellindex];

  // set new index
  _celldata.item_ids.setAt(cellindex,newindex);

  // compute good first index in array _celldata.item_infos

  int firstIndex = 0;

  Arcane::ItemTypeInfo* current_cell_type;

  for(int j=0;j<cellindex;j++)
    {
      current_cell_type = _typeMng->typeFromId(_celldata.item_infos[firstIndex]);
      firstIndex += 2 + current_cell_type->nbLocalNode();
    }

  // change infos from array _celldata.item_infos

  _celldata.item_infos.setAt(firstIndex+1,newindex);

  // _trace_mng->info() << "new _celldata.item_ids =" << _celldata.item_ids[cellindex];

  // reinitialize cells
  initCells();

}


