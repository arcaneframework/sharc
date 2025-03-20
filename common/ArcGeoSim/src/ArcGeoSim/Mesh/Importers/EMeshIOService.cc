// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* EMeshIOService.cc                                                (C) 2008 */
/*                                                                           */
/* Lecture/Ecriture d'un maillage au format EMesh.                           */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef WIN32
#include <ciso646>
#endif

#include <arcane/ArcaneVersion.h>
#include <arcane/utils/ArcanePrecomp.h>

#include <arcane/utils/Iostream.h>
#include <arcane/utils/StdHeader.h>
#include <arcane/utils/HashTableMap.h>
#include <arcane/utils/ValueConvert.h>
#include <arcane/utils/ScopedPtr.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/String.h>
#include <arcane/utils/StringBuilder.h>
#include <arcane/utils/IOException.h>
#include <arcane/utils/Collection.h>
#include <arcane/utils/Enumerator.h>
#include <arcane/utils/OStringStream.h>
#include <arcane/ArcaneVersion.h>
#include <arcane/FactoryService.h>
#include <arcane/IMeshReader.h>
#include <arcane/ISubDomain.h>
#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/IItemFamily.h>
#include <arcane/Item.h>
#include <arcane/ItemEnumerator.h>
#include <arcane/VariableTypes.h>
#include <arcane/IParallelMng.h>
#include <arcane/IIOMng.h>
#include <arcane/IXmlDocumentHolder.h>
#include <arcane/XmlNodeList.h>
#include <arcane/XmlNode.h>
#include <arcane/IMeshUtilities.h>
#include <arcane/IMeshWriter.h>
#include <arcane/BasicService.h>
#include <arcane/utils/UserDataList.h>
#include <arcane/utils/IUserData.h>
#include <arcane/utils/AutoDestroyUserData.h>


#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Mesh/Utils/MeshUtils.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Utils/ItemGroupBuilder.h"

#ifdef USE_ARCANE_V3
#include <arcane/mesh/DualUniqueIdMng.h>
#else
#include <arcane/utils/DualUniqueIdMng.h>
#endif


using namespace Arcane ;

//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "BUG: Les unique ids sont ici encod���������s sur un int."
//#endif
//#endif
typedef std::map<int,int> IntIntMap;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class  MeshFile;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Lecteur des fichiers de maillage au format Vtk historique (legacy).
 *
 * Il s'agit d'une version pr?liminaire qui ne supporte que les
 * DATASET de type STRUCTURED_GRID ou UNSTRUCTURED_GRID. De plus,
 * le lecteur et l'?crivain n'ont ?t? que partiellement test?s.
 *
 * L'en-t?te du fichier vtk doit ?tre:
 * # vtk DataFile Version 2.0
 *
 * Il est possible de sp?cifier un ensemble de variables dans le fichier.
 * Dans ce cas, leurs valeurs sont lues en m?me temps que le maillage
 * et servent ? initialiser les variables. Actuellement, seules les valeurs
 * aux mailles sont support?es
 *
 * Comme Vtk ne supporte pas la notion de groupe, il est possible
 * de sp?cifier un groupe comme ?tant une variable (CELL_DATA).
 * Par convention, si la variable commence par la chaine 'GROUP_', alors
 * il s'agit d'un groupe. La variable doit ?tre d?clar?e comme suit:
 * \begincode
 * CELL_DATA %n
 * SCALARS GROUP_%m int 1
 * LOOKUP_TABLE default
 * \endcode
 * avec %n le nombre de mailles, et %m le nom du groupe.
 * Une maille appartient au groupe si la valeur de la donn?e est
 * diff?rente de 0.
 *
 * Actuellement, on NE peut PAS specifier de groupes de points.
 *
 * Pour sp?cifier des groupes de faces, il faut un fichier vtk
 * additionnel, identique au fichier d'origine mais contenant la
 * description des faces au lieu des mailles. Par convention, si le
 * fichier courant lu s'appelle 'toto.vtk', le fichier d?crivant les
 * faces sera 'toto.vtkfaces.vtk'. Ce fichier est optionnel.
 */
class MeshFileIOService
  : public BasicService
  , public IMeshReader
{
public:

  MeshFileIOService(const ServiceBuildInfo& sbi);

public:

  virtual void build() {}

public:

  enum eMeshType
    {
      VTK_MT_Unknown,
      VTK_MT_StructuredGrid,
      VTK_MT_StructuredGraph,
      VTK_MT_StructuredDualGrid,
      VTK_MT_UnstructuredGrid
    };
  class VtkMesh
  {
  public:
  };

  class VtkStructuredGrid
    : public VtkMesh
  {
  public:
    int m_nb_x;
    int m_nb_y;
    int m_nb_z;
  };

public:
  
  virtual bool allowExtension(const String& str)
  {
    return str=="msh";
  }

public:
  
  virtual eReturnType readMeshFromFile(IPrimaryMesh* mesh,const XmlNode& mesh_node,const String& file_name,
                                       const String& dir_name,bool use_internal_partition);

  virtual bool hasCutInfos() const { return false; }
#if (ARCANE_VERSION >= 12201)
  virtual ConstArrayView<Integer> communicatingSubDomains() const { return ConstArrayView<Integer>(); }
#else /* ARCANE_VERSION */
  virtual ConstCArrayInteger communicatingSubDomains() const { return ConstCArrayInteger(); }
#endif /* ARCANE_VERSION */
  ~MeshFileIOService(){}
private:

  bool _readMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition);
  void _readTopoNodesUnstructuredGrid(IntIntMap & id_node,Integer & nb_node,MeshFile& vtk_file);
#if (ARCANE_VERSION >= 12201)
  void _readTopoCellsUnstructuredGrid(IntIntMap & id_cell, IntIntMap &  id_node,Integer & nb_cell,MeshFile& vtk_file,
  		SharedArray<Integer>& cells_nb_node,
  		SharedArray<Integer>& cells_type, SharedArray<Int64>& cells_connectivity,
  		SharedArray<Integer>& cells_begin);

  void _readGeoNodesUnstructuredGrid(int nb_node,MeshFile& vtk_file,SharedArray<Real3>& node_coords);
  void _readGeoCellsUnstructuredGrid(IPrimaryMesh* mesh,int nb_cell,MeshFile& vtk_file,VariableCellReal3* center_cell,VariableCellReal* volume_cell);
  void _readGeoFacesUnstructuredGrid(IPrimaryMesh* mesh,int nb_face,SharedArray<Integer>& faces_local_id,IntIntMap & id_face,MeshFile& vtk_file,
      VariableFaceReal3 * center_face, VariableFaceReal3 * normal_face, VariableFaceReal * area_face);
  bool _readUnstructuredGrid(IPrimaryMesh* mesh,MeshFile& vtk_file,bool use_internal_partition);
  bool _readTopoFacesUnstructuredGrid(IPrimaryMesh* mesh,IntIntMap & id_face, IntIntMap & id_cell, SharedArray<Integer>& cells_begin, SharedArray<Int64>& cells_connectivity,Integer & nb_face,SharedArray<Integer>& cells_type,SharedArray<Integer>& faces_local_id,MeshFile& vtk_file);
#else /* ARCANE_VERSION */
  void _readTopoCellsUnstructuredGrid(IntIntMap & id_cell, IntIntMap &  id_node,Integer & nb_cell,MeshFile& vtk_file,
  		CArrayInteger& cells_nb_node,
  		CArrayInteger& cells_type, CArrayInt64& cells_connectivity,
  		CArrayInteger& cells_begin);

  void _readGeoNodesUnstructuredGrid(int nb_node,MeshFile& vtk_file,CArrayReal3& node_coords);
  void _readGeoCellsUnstructuredGrid(IPrimaryMesh* mesh,int nb_cell,MeshFile& vtk_file,VariableCellReal3* center_cell,VariableCellReal* volume_cell);
  void _readGeoFacesUnstructuredGrid(IPrimaryMesh* mesh,int nb_face,CArrayInteger & faces_local_id,IntIntMap & id_face,MeshFile& vtk_file,
      VariableFaceReal3 * center_face, VariableFaceReal3 * normal_face, VariableFaceReal * area_face);
  bool _readUnstructuredGrid(IPrimaryMesh* mesh,MeshFile& vtk_file,bool use_internal_partition);
  bool _readTopoFacesUnstructuredGrid(IPrimaryMesh* mesh,IntIntMap & id_face, IntIntMap & id_cell, CArrayInteger& cells_begin, CArrayInt64& cells_connectivity,Integer & nb_face,CArrayInteger& cells_type,CArrayInteger & faces_local_id,MeshFile& vtk_file);
#endif /* ARCANE_VERSION */
  bool check(Integer i, Integer j, Integer k, Integer nx, Integer ny, Integer nz);
  bool check(Integer i, Integer first, Integer last) ;
#if (ARCANE_VERSION >= 12201)
  bool _readData( IPrimaryMesh* mesh,IntIntMap & id_node,
  		IntIntMap & id_cell,
  		IntIntMap & id_face,
  		MeshFile& vtk_file,
  		bool use_internal_partition,
  		SharedArray<Integer>& faces_local_id,
  		const char * buf =NULL);
#else /* ARCANE_VERSION */
  bool _readData( IPrimaryMesh* mesh,IntIntMap & id_node,
  		IntIntMap & id_cell,
  		IntIntMap & id_face,
  		MeshFile& vtk_file,
  		bool use_internal_partition,
  		CArrayInteger & faces_local_id,
  		const char * buf =NULL);
#endif /* ARCANE_VERSION */
  bool _readNodeData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
  		std::string & item,
  		int nb_item_kind,
  		istringstream& iline,
  		std::string& data_str,
  		OStringStream& created_infos_str);
  void _readNodeVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var);
  void _readNodeVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var);
  void _readNodeGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,Integer nb_item_var);
  bool _readCellData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
  		std::string & item,
  		int nb_item_kind,
  		istringstream& iline,
  		std::string& data_str,
  		OStringStream& created_infos_str);
  void _readCellVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var);
  void _readCellVariableInteger(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var);
  void _readCellVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var);
  void _readCellVariableReal3(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var);
  void _readCellGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,
  		Integer nb_item_kind,Integer nb_item_var,SharedArray<String> name_group, Integer total_nb_group = 0);
  //void _readCellGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,Integer nb_item_var);
#if (ARCANE_VERSION >= 12201)
  bool _readFaceData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
  		std::string & item,
  		int nb_item_kind,
  		istringstream& iline,
  		SharedArray<Integer>& faces_local_id,
  		std::string& data_str,
  		OStringStream& created_infos_str);
  void _readFaceVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,SharedArray<Integer>& faces_local_id,Integer nb_item_var);
  void _readFaceVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,SharedArray<Integer>& faces_local_id,Integer nb_item_var);
  void _readFaceVariableInteger(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,SharedArray<Integer>& faces_local_id,Integer nb_item_var);
  void _readFaceGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,SharedArray<Integer>& faces_local_id,Integer nb_item_var,Integer total_nb_group = 0);
  //void _readFaceGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,SharedArray<Integer>& faces_local_id,Integer nb_item_var);

  void _readTopoNoMatchingUnstructuredGrid(IPrimaryMesh* mesh,IntIntMap & id_face,SharedArray<Integer>& faces_local_id,Integer & nb_liaison, MeshFile& vtk_file,bool need_read);

  void  _readTopoUpdateNodeUnstructuredGrid(IntIntMap & id_node,IntIntMap & id_face,SharedArray<Integer>& faces_local_id,MeshFile& vtk_file);
#else /* ARCANE_VERSION */
  bool _readFaceData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
  		std::string & item,
  		int nb_item_kind,
  		istringstream& iline,
  		CArrayInteger & faces_local_id,
  		std::string& data_str,
  		OStringStream& created_infos_str);
  void _readFaceVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,CArrayInteger & faces_local_id,Integer nb_item_var);
  void _readFaceVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,CArrayInteger & faces_local_id,Integer nb_item_var);
  void _readFaceGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,CArrayInteger & faces_local_id,Integer nb_item_var,Integer total_nb_group = 0);
  //void _readFaceGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,CArrayInteger & faces_local_id,Integer nb_item_var);

  void _readTopoNoMatchingUnstructuredGrid(IPrimaryMesh* mesh,IntIntMap & id_face,CArrayInteger & faces_local_id,Integer & nb_liaison, MeshFile& vtk_file,bool need_read);

  void  _readTopoUpdateNodeUnstructuredGrid(IntIntMap & id_node,IntIntMap & id_face,CArrayInteger & faces_local_id,MeshFile& vtk_file);
#endif /* ARCANE_VERSION */
  void _readGeoNomatchingUnstructuredGrid(IPrimaryMesh* mesh,Integer  nb_liaison,MeshFile& vtk_file,bool need_read);
  void  _createItemGroup(IMesh* mesh,
  		MeshFile& vtk_file,
  		const char* buf,
  		Integer nb_item_x,
  		Integer nb_item_y,
  		Integer nb_item_z,
  		Integer first_x,
  		Integer local_nb_item_x,
  		Integer first_y,
  		Integer local_nb_item_y,
  		eItemKind ik,
  		bool use_internal_partition);
  bool _readCondLimStructuredGrid(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition);
#if  (ARCANE_VERSION >= 30003)
  DoFGroup m_contact_link_group ;
  const Arcane::IGraphConnectivity* m_connectivity = nullptr ;
#else
  LinkGroup m_contact_link_group ;
#endif
  std::map<Int64,Integer> m_link_uid_to_lid ;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class MeshFile
{
public:
  static const int BUFSIZE = 10000;
public:
  enum LINE_TYPE { OPTIONAL_LINE, MANDATORY_LINE };
public:
  MeshFile(istream* stream) : m_stream(stream) {}
  const bool isThereAnotherLine();
  const char* getNextLine(LINE_TYPE line_type);
  Real getReal();
  Integer getInteger();
  Int64 getInt64();
  std::string  getString();
  void checkString(const String& current_value,const String& expected_value);
  void checkString(const String& current_value,
                   const String& expected_value1,
                   const String& expected_value2);
  static bool isEqualString(const String& current_value,const String& expected_value);
  bool isEnd(){ (*m_stream) >> ws; return m_stream->eof(); }
private:
  istream* m_stream;
  char m_buf[BUFSIZE];
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

const char* MeshFile::
getNextLine(LINE_TYPE line_type = MANDATORY_LINE)
{
  while (m_stream->good()){
    m_stream->getline(m_buf,sizeof(m_buf)-1);
    if (m_stream->eof())
      break;
    bool is_comment = true;
    if (m_buf[0]=='\n' || m_buf[0]=='\r')
      continue;
    // Regarde si un caract?re de commentaire est pr?sent
    for( int i=0; i<BUFSIZE && m_buf[i]!='\0'; ++i ){
      if (!isspace(m_buf[i])){
        is_comment = ((m_buf[i]=='/')||(m_buf[i]=='#'));
        break;
      }
    }
    if (!is_comment){
      
      // Supprime le '\n' ou '\r' final
      for( int i=0; i<BUFSIZE && m_buf[i]!='\0'; ++i ){
        //cout << " V=" << m_buf[i] << " I=" << (int)m_buf[i] << "\n";
        if (m_buf[i]=='\n' || m_buf[i]=='\r'){
          m_buf[i] = '\0';
          break;
        }
      }
      return m_buf;
    }
  }

  if(line_type == MANDATORY_LINE)
    throw IOException("MeshFile::getNexLine()","Unexpected EndOfFile");
  return NULL;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Real MeshFile::
getReal()
{
  Real v = 0.;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("MeshFile::getReal()","Bad Real");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Integer MeshFile::
getInteger()
{
  Integer v = 0;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("MeshFile::getInteger()","Bad Integer");
}

Int64 MeshFile::
getInt64()
{
  Int64 v = 0;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("MeshFile::getInt64()","Bad Int64");
}


std::string  MeshFile::
getString()
{
  std::string var;
  (*m_stream) >> ws >> var;
  if (m_stream->good())
    return var;
  throw IOException("MeshFile::getString()","Bad String");
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MeshFile::
checkString(const String& current_value,const String& expected_value)
{
  if (current_value!=expected_value){
    String s = "Attend chaine '" + expected_value + "', trouve '" + current_value + "'";
    throw IOException("MeshFile::checkString()",s);
  }
}

void MeshFile::
checkString(const String& current_value,const String& expected_value1,const String& expected_value2)
{
  if (current_value!=expected_value1 && current_value!=expected_value2){
    String s = "Attend chaine '" + expected_value1 + "' ou '"
      + expected_value2 + "', trouve '" + current_value + "'";
    throw IOException("MeshFile::checkString()",s);
  }
}

bool  MeshFile::
isEqualString(const String& current_value,const String& expected_value)
{
  StringBuilder current_value_low(current_value); 
  current_value_low.toLower();
  StringBuilder expected_value_low(expected_value); 
  expected_value_low.toLower();
  return (current_value_low.toString()==expected_value_low.toString());
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SUB_DOMAIN_FACTORY(MeshFileIOService,IMeshReader,MeshFileIO);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
MeshFileIOService::
MeshFileIOService(const ServiceBuildInfo& sbi)
  : BasicService(sbi) 
{
 
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \todo Verifier plantage sous linux.
 */
IMeshReader::eReturnType MeshFileIOService::
readMeshFromFile(IPrimaryMesh* mesh,const XmlNode& mesh_node,
                 const String& filename,const String& dir_name,
                 bool use_internal_partition)
{
 
  bool ret = _readMesh(mesh,filename,dir_name,use_internal_partition);
  if (ret)
    return RTError;

  return RTOk;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool MeshFileIOService::
_readMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition)
{
  ifstream ifile(file_name.localstr());
  if (!ifile){
    error() << "Impossible d'ouvrir en lecture le fichier '" << file_name << "'";
    return true;
  }
 
  MeshFile vtk_file(&ifile);
  
  // Lecture du type de maillage
 
  bool ret = true;
  
  ret = _readUnstructuredGrid(mesh,vtk_file,use_internal_partition);
  if(!ret){
    ret = _readCondLimStructuredGrid(mesh,file_name+"faces.msh",dir_name,use_internal_partition);
  }
  return ret;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Lecture du nombre de noeuds et de leur num���������ros.
 */

void MeshFileIOService::
_readTopoNodesUnstructuredGrid(IntIntMap & id_node,Integer & nb_node,MeshFile& vtk_file)
{
  const char* func_name = "MeshFileIOService::_readTopoNodesUnstructuredGrid()";
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);

  
  // on lit le nombre de noeuds
  iline >> ws >> nb_node ;
  if (nb_node<=0){
    String msg = "Nombre de noeuds invalide: n=" + nb_node;
    throw IOException(func_name,msg);
  }

  // on recupere les numeros des noeuds
  if(nb_node > 0){
    Integer m = 0;
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    iline >> ws >> m ;
    id_node[m] = 0;
    for( Integer i= 1; i<nb_node; i++){
      m = vtk_file.getInteger();
      id_node[m] = i;
    }
  }
  info() << " Infos: " << nb_node;
}
#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readTopoNoMatchingUnstructuredGrid(IPrimaryMesh* mesh,
		IntIntMap & id_face,
		SharedArray<Integer>& faces_local_id,
		Integer & nb_liaison,
		MeshFile& vtk_file,
		bool need_read){
	//Pour chaque liaison, on lit les identifiants des faces

	const char* func_name = "MeshFileIOService::_readTopoNoMatchingUnstructuredGrid";
	auto graph = ArcGeoSim::Mesh::GraphMng::graph(mesh,true);
	if(need_read)
	{
		const char* buf = vtk_file.getNextLine();
		istringstream iline(buf);
		// on lit le nombre de liaisons
		iline >> ws >> nb_liaison ;
		if (nb_liaison < 0){
			String msg = "Nombre de liaisons invalide: n=" + nb_liaison;
			throw IOException(func_name,msg);
		}
	}
	else
		nb_liaison = 0 ;

	SharedArray<Int64> link_uids(nb_liaison) ;
#ifdef USE_ARCANE_V3
	Integer nb_dual_node_per_link = 2 ;
	UniqueArray<Int64> links_infos(nb_liaison*(nb_dual_node_per_link+1)) ;
#else
	SharedArray<Int64> links_infos(nb_liaison*4+1) ;
        links_infos[0] = 0 ;
#endif
	SharedArray<Integer> face_ids(nb_liaison) ;
	Integer nb_link = 0 ;
	if(need_read)
	{
		// on recupere les numeros des faces
		if(nb_liaison > 0)
		{
			Integer links_infos_index = 0;
			IItemFamily* face_family = mesh->itemFamily(IK_Face);
			std::set<Face> face_set ;
			SharedArray<Face> face1s ;
			face1s.reserve(nb_liaison) ;
			SharedArray<Face> face2s ;
			face2s.reserve(nb_liaison) ;
			for( Integer i= 0; i<nb_liaison; i++)
			{
				Integer f1 = vtk_file.getInteger();
				Integer f2 = vtk_file.getInteger();
				Integer G1 = faces_local_id[id_face[f1]];
				Integer G2 = faces_local_id[id_face[f2]];
//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "BUG: Operations tres couteuses"
//#endif
//#endif
				const Face& face1 = face_family->findOneItem( G1 );
				const Face& face2 = face_family->findOneItem( G2 );
				face_set.insert(face1) ;
				face_set.insert(face2) ;
				face1s.add(face1) ;
				face2s.add(face2) ;
			}

			{
	Int64SharedArray dnode_infos(6*nb_liaison) ;

	Integer nb_dnode = 0 ;
	Integer dnode_info = 0 ;
	for (std::set<Face>::const_iterator it(face_set.begin());it!=face_set.end();++it)
	{
		Face face = *it ;
		dnode_infos[dnode_info++] = IT_DualFace;
		dnode_infos[dnode_info++] = DualUniqueIdMng::uniqueIdOf<Face>(face) ;
		dnode_infos[dnode_info++] = face.uniqueId() ;
		++nb_dnode ;
	}
	dnode_infos.resize(3*nb_dnode) ;
	graph->modifier()->addDualNodes(nb_dnode,dnode_infos) ;
			}

			// le num���������������������������ro de face pour arcane est alors

			for( Integer i= 0; i<nb_liaison; i++)
			{
				Face face1 = face1s[i] ;
				Face face2 = face2s[i] ;
				const Int64 link_unique_id = DualUniqueIdMng::uniqueIdOf<Face,Face>(face1,face2);
				link_uids[nb_link] = link_unique_id;//face_uid ;
				m_link_uid_to_lid[link_unique_id] = nb_link ;
#ifndef USE_ARCANE_V3
				links_infos[links_infos_index] = 2; // Nombre de noeuds duaux par lien
				++links_infos_index;
#endif
				links_infos[links_infos_index] = link_unique_id;
				++links_infos_index;

				links_infos[links_infos_index+0] = DualUniqueIdMng::uniqueIdOf<Face>(face1);//iuid;
				links_infos[links_infos_index+1] = DualUniqueIdMng::uniqueIdOf<Face>(face2);//juid;
				links_infos_index += 2;
				++nb_link;
			}
		}
	}
#ifdef USE_ARCANE_V3
	links_infos.resize(nb_link*(nb_dual_node_per_link+1)) ;
	graph->modifier()->addLinks(nb_link,nb_dual_node_per_link,links_infos);
        graph->modifier()->endUpdate() ;
        m_connectivity = graph->connectivity() ;
#else
	graph->modifier()->addLinks(nb_link,links_infos);
	graph->modifier()->endUpdate(true,false) ;
#endif

	// Patch en attendant correction bug Arcane
	{
		//graph->dualNodeFamily()->prepareForDump() ;
		//graph->linkFamily()->prepareForDump() ;
	}

	IItemFamily* link_family = graph->linkFamily() ;
	SharedArray<Integer> link_ids(nb_link) ;
	IItemFamily* family = graph->linkFamily() ;

	family->itemsUniqueIdToLocalId(link_ids,link_uids) ;
	String name("ContactLinkGroup") ;
	link_family->createGroup(name,link_ids) ;
	m_contact_link_group = link_family->findGroup(name) ;

	bool print_graph = true ;
	if(print_graph) {
		IItemFamily* dnode_family = graph->dualNodeFamily() ;
		ENUMERATE_DUALNODE(inode,dnode_family->allItems())
		{
			info()<<"DualNode uid : "<<(*inode).uniqueId()<<" lid : "<<(*inode).localId() ;
			info()<<"   ditem uid : "<<DUALITEM(*inode).uniqueId();
			const Face face = DUALITEM(*inode).toFace() ;
			info()<<"   nb_cell : "<<face.nbCell() ;
			for(CellEnumerator icell(face.cells());icell();++icell)
			{
				info()<<"        cell uid : "<<(*icell).uniqueId() ;
			}
			info()<<"   BoundaryCell uid : "<<face.boundaryCell().uniqueId() ;
		}
		ENUMERATE_LINK(ilink,link_family->allItems())
		{
			info()<<"Link uid : "<<(*ilink).uniqueId();
			info()<<"     lid : "<<(*ilink).localId() ;
			DualNodeEnumerator inode(DUALNODES_ENUM(*ilink));
			const Face face1 = DUALITEM(*inode).toFace() ;
			info()<<"     dnode1 uid : "<<(*inode).uniqueId();
			info()<<"           lid : "<<(*inode).localId();
			info()<<"     ditem1 uid : "<<DUALITEM(*inode).uniqueId();
			info()<<"           lid : "<<DUALITEM(*inode).localId();
			info()<<"     cell1 uid : "<<face1.boundaryCell().uniqueId() ;
			++inode ;
			const Face face2 = DUALITEM(*inode).toFace() ;
			info()<<"     dnode2 uid : "<<(*inode).uniqueId();
			info()<<"           lid : "<<(*inode).localId();
			info()<<"     ditem2 uid : "<<DUALITEM(*inode).uniqueId();
			info()<<"           lid : "<<DUALITEM(*inode).localId();
			info()<<"     cell2 uid : "<<face2.boundaryCell().uniqueId() ;
		}
	}


	info() << " Infos: number of links " << nb_liaison;
}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readTopoNoMatchingUnstructuredGrid(IPrimaryMesh* mesh,
                                    IntIntMap & id_face,
                                    CArrayInteger & faces_local_id,
                                    Integer & nb_liaison, 
                                    MeshFile& vtk_file,
                                    bool need_read){
  //Pour chaque liaison, on lit les identifiants des faces

#if(ARCANE_VERSION < 30003)
  const char* func_name = "MeshFileIOService::_readTopoNoMatchingUnstructuredGrid";
#if (ARCANE_VERSION<11602)
  IGraph* graph = subDomain()->defaultGraph() ;
#else
  IGraph* graph = mesh->graph();
#endif
  if(need_read)
    {
      const char* buf = vtk_file.getNextLine();
      istringstream iline(buf);
      // on lit le nombre de liaisons
      iline >> ws >> nb_liaison ;
      if (nb_liaison < 0){
        String msg = "Nombre de liaisons invalide: n=" + nb_liaison;
        throw IOException(func_name,msg);
      }
    }
  else
    nb_liaison = 0 ;

  CArrayInt64 link_uids(nb_liaison) ;
  CArrayInt64 links_infos(nb_liaison*4+1) ;
  links_infos[0] = 0 ;
  CArrayInteger face_ids(nb_liaison) ;
  Integer nb_link = 0 ;
  if(need_read)
    {
      // on recupere les numeros des faces
      if(nb_liaison > 0)
        {
#if (ARCANE_VERSION<11602)
          Integer links_infos_index = 1;
#else
          Integer links_infos_index = 0;
#endif
          IItemFamily* face_family = mesh->itemFamily(IK_Face);
          std::set<Face> face_set ;
          SharedArray<Face> face1s ;
          face1s.reserve(nb_liaison) ;
          SharedArray<Face> face2s ;
          face2s.reserve(nb_liaison) ;
          for( Integer i= 0; i<nb_liaison; i++)
            {
              Integer f1 = vtk_file.getInteger();
              Integer f2 = vtk_file.getInteger();
              Integer G1 = faces_local_id[id_face[f1]];
              Integer G2 = faces_local_id[id_face[f2]];
#ifndef NO_USER_WARNING
#warning "BUG: Op���������rations tr���������s couteuses"
#endif
              const Face& face1 = face_family->findOneItem( G1 );
              const Face& face2 = face_family->findOneItem( G2 );
              face_set.insert(face1) ;
              face_set.insert(face2) ;
              face1s.add(face1) ;
              face2s.add(face2) ;
            }
      
          {
#if (ARCANE_VERSION<11602)
            CArrayInt64 face_uids(2*nb_liaison) ;
            CArrayInt64 dnode_uids(2*nb_liaison) ;
            Integer nb_dnode = 0 ;
            for (std::set<Face>::const_iterator it(face_set.begin());it!=face_set.end();++it)
              {
                Face face = *it ;
                dnode_uids[nb_dnode] = DualUniqueIdMng::uniqueIdOf<Face>(face) ;
                face_uids[nb_dnode] = face.uniqueId() ;
                ++nb_dnode ;
              }
            face_uids.resize(nb_dnode) ;
            dnode_uids.resize(nb_dnode) ;
            graph->addDualNodes(IT_DualFace,nb_dnode,dnode_uids,face_uids) ;
#else
            Int64SharedArray dnode_infos(6*nb_liaison) ;
            
            Integer nb_dnode = 0 ;
            Integer dnode_info = 0 ;
            for (std::set<Face>::const_iterator it(face_set.begin());it!=face_set.end();++it)
              {
                Face face = *it ;
                dnode_infos[dnode_info++] = IT_DualFace;
                dnode_infos[dnode_info++] = DualUniqueIdMng::uniqueIdOf<Face>(face) ;
                dnode_infos[dnode_info++] = face.uniqueId() ;
                ++nb_dnode ;
              }
            dnode_infos.resize(3*nb_dnode) ;
            graph->modifier()->addDualNodes(nb_dnode,dnode_infos) ;
#endif
          }
        
          // le num���������ro de face pour arcane est alors
  
          for( Integer i= 0; i<nb_liaison; i++)
            {
              Face face1 = face1s[i] ;
              Face face2 = face2s[i] ;
              const Int64 link_unique_id = DualUniqueIdMng::uniqueIdOf<Face,Face>(face1,face2);
              link_uids[nb_link] = link_unique_id;//face_uid ;
              m_link_uid_to_lid[link_unique_id] = nb_link ;
#if (ARCANE_VERSION<11602)       
              links_infos[links_infos_index] = IT_Line2;
#else
              links_infos[links_infos_index] = 2; // Nombre de noeuds duaux par lien
#endif
              ++links_infos_index;
              links_infos[links_infos_index] = link_unique_id;
              ++links_infos_index;
  
              links_infos[links_infos_index+0] = DualUniqueIdMng::uniqueIdOf<Face>(face1);//iuid;
              links_infos[links_infos_index+1] = DualUniqueIdMng::uniqueIdOf<Face>(face2);//juid;
              links_infos_index += 2;
              ++nb_link;
            }
        }
    }
#if (ARCANE_VERSION<11602)
  graph->allocateLinks(nb_link,links_infos,false);
  graph->endUpdate(true,false) ;
#else
  graph->modifier()->addLinks(nb_link,links_infos);
  graph->modifier()->endUpdate(true,false) ;

  // Patch en attendant correction bug Arcane
  {
    //graph->dualNodeFamily()->prepareForDump() ;
    //graph->linkFamily()->prepareForDump() ;
  }
#endif

#if (ARCANE_VERSION<11602)  
  IItemFamily* link_family = subDomain()->defaultGraph()->linkFamily() ;
#else
  IItemFamily* link_family = graph->linkFamily() ;
#endif
  SharedArray<Integer> link_ids(nb_link) ;
#if (ARCANE_VERSION<11602)  
  IItemFamily* family = subDomain()->defaultGraph()->linkFamily() ;
#else
  IItemFamily* family = graph->linkFamily() ;
#endif

  family->itemsUniqueIdToLocalId(link_ids,link_uids) ;
  String name("ContactLinkGroup") ;
  link_family->createGroup(name,link_ids) ; 
  m_contact_link_group = link_family->findGroup(name) ;

  bool print_graph = true ;
  if(print_graph) {
#if (ARCANE_VERSION<11602)  
    IItemFamily* dnode_family = subDomain()->defaultGraph()->dualNodeFamily() ;
#else
    IItemFamily* dnode_family = graph->dualNodeFamily() ;
#endif
    ENUMERATE_DUALNODE(inode,dnode_family->allItems())
      {
        info()<<"DualNode uid : "<<(*inode).uniqueId()<<" lid : "<<(*inode).localId() ;
        info()<<"   ditem uid : "<<(*inode).dualItem().uniqueId();
        const Face face = (*inode).dualItem().toFace() ;
        info()<<"   nb_cell : "<<face.nbCell() ;
        for(CellEnumerator icell(face.cells());icell();++icell)
          {
            info()<<"        cell uid : "<<(*icell).uniqueId() ;
          }
        info()<<"   BoundaryCell uid : "<<face.boundaryCell().uniqueId() ;
      }
    ENUMERATE_LINK(ilink,link_family->allItems())
      {
        info()<<"Link uid : "<<(*ilink).uniqueId();
        info()<<"     lid : "<<(*ilink).localId() ;
        DualNodeEnumerator inode((*ilink).dualNodes());
        const Face face1 = (*inode).dualItem().toFace() ;
        info()<<"     dnode1 uid : "<<(*inode).uniqueId();
        info()<<"           lid : "<<(*inode).localId();
        info()<<"     ditem1 uid : "<<(*inode).dualItem().uniqueId();
        info()<<"           lid : "<<(*inode).dualItem().localId();
        info()<<"     cell1 uid : "<<face1.boundaryCell().uniqueId() ;
        ++inode ;
        const Face face2 = (*inode).dualItem().toFace() ;
        info()<<"     dnode2 uid : "<<(*inode).uniqueId();
        info()<<"           lid : "<<(*inode).localId();
        info()<<"     ditem2 uid : "<<(*inode).dualItem().uniqueId();
        info()<<"           lid : "<<(*inode).dualItem().localId();
        info()<<"     cell2 uid : "<<face2.boundaryCell().uniqueId() ;
      }
  }

  
  info() << " Infos: number of links " << nb_liaison;
#endif
}
#endif /* ARCANE_VERSION */

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readTopoUpdateNodeUnstructuredGrid(IntIntMap & id_node,IntIntMap & id_face,SharedArray<Integer>& faces_local_id,MeshFile& vtk_file){

	const char* func_name = "MeshFileIOService::_readTopoUpdateNodeUnstructuredGrid()";
	const char* buf = vtk_file.getNextLine();
	istringstream iline(buf);

	Integer nb_noeud, /* id_noeud = 0,*/ nb_faces/*, id_faces = 0*/;

	// on lit le nombre de noeud se trouvant dans les zones de non coincidence
	iline >> ws >> nb_noeud ;
	if (nb_noeud < 0){
		String msg = "Nombre de noeuds invalide: n=" + nb_noeud;
		throw IOException(func_name,msg);
	}

	for( Integer i= 0; i<nb_noeud; i++){
		/* id_noeud = */ vtk_file.getInteger(); // identifiant du noeud
		nb_faces = vtk_file.getInteger(); // nombre de faces voisines
		for( Integer j= 0; j<nb_faces; j++){
			/* id_faces = */ vtk_file.getInteger();
		}
	}

	info() << " Infos: number of updated nodes" << nb_noeud;
}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readTopoUpdateNodeUnstructuredGrid(IntIntMap & id_node,IntIntMap & id_face,CArrayInteger & faces_local_id,MeshFile& vtk_file){
  
  const char* func_name = "MeshFileIOService::_readTopoUpdateNodeUnstructuredGrid()";
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);

  Integer nb_noeud, /* id_noeud = 0,*/ nb_faces/*, id_faces = 0*/;

  // on lit le nombre de noeud se trouvant dans les zones de non coincidence
  iline >> ws >> nb_noeud ;
  if (nb_noeud < 0){
    String msg = "Nombre de noeuds invalide: n=" + nb_noeud;
    throw IOException(func_name,msg);
  }
  
  for( Integer i= 0; i<nb_noeud; i++){
    /* id_noeud = */ vtk_file.getInteger(); // identifiant du noeud
    nb_faces = vtk_file.getInteger(); // nombre de faces voisines
    for( Integer j= 0; j<nb_faces; j++){
      /* id_faces = */ vtk_file.getInteger();
    }
  }
  
  info() << " Infos: number of updated nodes" << nb_noeud;
}
#endif /* ARCANE_VERSION */


void 
MeshFileIOService::
_readGeoNomatchingUnstructuredGrid(IPrimaryMesh* mesh,Integer  nb_liaison,MeshFile& vtk_file,bool need_read)
{

  enum eProperties { Vol, Cx, Cy, Cz, Nx, Ny, Nz, NbProperties } ;

  VariableLinkArrayReal* contact_link_properties = ArcGeoSim::Mesh::GraphMng::newVariableLinkArray<Real>(mesh,"MshContactLinkProperties",IVariable::PNoRestore);
  mesh->userDataList()->setData("MshContactLinkProperties",new Arcane::AutoDestroyUserData<Arcane::VariableLinkArrayReal>(contact_link_properties));
  (*contact_link_properties).resize(NbProperties) ;
  if(need_read) 
    {
      std::string stype;
      const char* buf = vtk_file.getNextLine();
      istringstream iline(buf);
    
      // type de calcul pour les intersections de faces
      iline >> ws >> stype ;
      SharedArray2<Real> props(nb_liaison, (Integer)NbProperties) ;
      for( Integer i= 0; i<nb_liaison; i++){
        // on lit une mesure
        Real vol = vtk_file.getReal();
        // un centre
        Real cx  = vtk_file.getReal();
        Real cy  = vtk_file.getReal();
        Real cz  = vtk_file.getReal();
        // une normale
        Real nx  = vtk_file.getReal();
        Real ny  = vtk_file.getReal();
        Real nz  = vtk_file.getReal();
        ArrayView<Real> prop = props[i] ;
        prop[Vol] = vol ;
        prop[Cx] = cx ;
        prop[Cy] = cy ;
        prop[Cz] = cz ;
        prop[Nx] = nx ;
        prop[Ny] = ny ;
        prop[Nz] = nz ;
      }
      ENUMERATE_LINK(ilink,m_contact_link_group)
        {
          ArrayView<Real> prop = (*contact_link_properties)[ilink] ;
          Integer lid = m_link_uid_to_lid[ilink->uniqueId()] ;
          prop.copy(props[lid]) ;
        }
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Lecture des mailles et de leur connectivit?.
 *
 * En retour, remplit \a cells_nb_node, \a cells_type et \a cells_connectivity.
 */

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readTopoCellsUnstructuredGrid(IntIntMap & id_cell, IntIntMap &  id_node,Integer & nb_cell,MeshFile& vtk_file,
		SharedArray<Integer>& cells_nb_node,
		SharedArray<Integer>& cells_type,
		SharedArray<Int64>& cells_connectivity,
		SharedArray<Integer>& cells_begin)
{

	std::string stype;
	const char* func_name = "MeshFileIOService::_readTopoCellsUnstructuredGrid()";
	const char* buf = vtk_file.getNextLine();
	istringstream iline(buf);


	// on lit le nombre de cellules
	iline >> ws >> nb_cell ;

	if (nb_cell<=0){
		String msg = "Nombre de noeuds invalide: n=" + nb_cell;
		throw IOException(func_name,msg);
	}

	cells_nb_node.resize(nb_cell);
	cells_type.resize(nb_cell);
	cells_begin.resize(nb_cell+1);

	if(nb_cell > 0){
		buf = vtk_file.getNextLine();
		istringstream iline(buf);
		iline >> ws >> stype;
	}

	Integer connectivity_index = 0;


	cells_connectivity.resize(nb_cell*8);//au maximum, on a huit noeud par cellules

	cells_begin[0] = 0;

	for(Integer i = 0;i<nb_cell;i++){
		Integer it = IT_NullType;
		Integer m;

		// on lit le type de maille
		if(i > 0){
			stype = vtk_file.getString();
		}





		if      (stype=="IT_Hexaedron8")      it = IT_Hexaedron8;
		else if (stype=="IT_Pentaedron6")     it = IT_Pentaedron6;
		else if (stype=="IT_Tetraedron4")     it = IT_Tetraedron4;
		else if (stype=="IT_Pyramid5")        it = IT_Pyramid5;
		else if (stype=="IT_HemiHexa7")       it = IT_HemiHexa7;
		else if (stype=="IT_HemiHexa6")       it = IT_HemiHexa6;
		else if (stype=="IT_HemiHexa5")       it = IT_HemiHexa5;
		else if (stype=="IT_AntiWedgeLeft6")  it = IT_AntiWedgeLeft6;
		else if (stype=="IT_AntiWedgeRight6") it = IT_AntiWedgeRight6;
		else if (stype== "IT_DiTetra5")       it = IT_DiTetra5;
		else
		{
			String msg = "Type de maille inconnu ou invalid: type=" + stype;
			throw IOException(func_name,msg);
		}


		cells_type[i] = it;

		//on lit l'identifiant de maille
		m = vtk_file.getInteger();
		id_cell[m] = i;

		//on lit le nombre de noeuds qui composent la maille
		Integer n = vtk_file.getInteger();

		//on recupere les donn���������������������������es de noeuds
		cells_nb_node[i] = n;
		cells_begin[i+1] = cells_begin[i]+n;
		for( Integer j=0; j<n; ++j ){
			Integer id = vtk_file.getInteger();
			cells_connectivity[connectivity_index] = id_node[id];
			++connectivity_index;
		}
	}
}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readTopoCellsUnstructuredGrid(IntIntMap & id_cell, IntIntMap &  id_node,Integer & nb_cell,MeshFile& vtk_file,
                               CArrayInteger& cells_nb_node,
                               CArrayInteger& cells_type,
                               CArrayInt64& cells_connectivity,
                               CArrayInteger& cells_begin)
{

  std::string stype;
  const char* func_name = "MeshFileIOService::_readTopoCellsUnstructuredGrid()";
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);

   
  // on lit le nombre de cellules
  iline >> ws >> nb_cell ;
 
  if (nb_cell<=0){
    String msg = "Nombre de noeuds invalide: n=" + nb_cell;
    throw IOException(func_name,msg);
  }

  cells_nb_node.resize(nb_cell);
  cells_type.resize(nb_cell);
  cells_begin.resize(nb_cell+1);

  if(nb_cell > 0){
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    iline >> ws >> stype;
  }

  Integer connectivity_index = 0;

  
  cells_connectivity.resize(nb_cell*8);//au maximum, on a huit noeud par cellules 

  cells_begin[0] = 0;

  for(Integer i = 0;i<nb_cell;i++){
    Integer it = IT_NullType;
    Integer m;

    // on lit le type de maille
    if(i > 0){
      stype = vtk_file.getString();
    }



    
   
    if      (stype=="IT_Hexaedron8")      it = IT_Hexaedron8;
    else if (stype=="IT_Pentaedron6")     it = IT_Pentaedron6;
    else if (stype=="IT_Tetraedron4")     it = IT_Tetraedron4; 
    else if (stype=="IT_Pyramid5")        it = IT_Pyramid5; 
    else if (stype=="IT_HemiHexa7")       it = IT_HemiHexa7; 
    else if (stype=="IT_HemiHexa6")       it = IT_HemiHexa6;
    else if (stype=="IT_HemiHexa5")       it = IT_HemiHexa5; 
    else if (stype=="IT_AntiWedgeLeft6")  it = IT_AntiWedgeLeft6;
    else if (stype=="IT_AntiWedgeRight6") it = IT_AntiWedgeRight6;
    else if (stype== "IT_DiTetra5")       it = IT_DiTetra5;
    else
      {
        String msg = "Type de maille inconnu ou invalid: type=" + stype;
        throw IOException(func_name,msg);
      }
  

    cells_type[i] = it;

    //on lit l'identifiant de maille
    m = vtk_file.getInteger();
    id_cell[m] = i;

    //on lit le nombre de noeuds qui composent la maille
    Integer n = vtk_file.getInteger();
    
    //on recupere les donn���������es de noeuds
    cells_nb_node[i] = n;
    cells_begin[i+1] = cells_begin[i]+n; 
    for( Integer j=0; j<n; ++j ){
      Integer id = vtk_file.getInteger();
      cells_connectivity[connectivity_index] = id_node[id];
      ++connectivity_index;
    }
  }
}
#endif /* ARCANE_VERSION */



/*---------------------------------------------------------------------------*/
/*   Les coordonn���������es des noeuds                                              */
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readGeoNodesUnstructuredGrid(int nb_node,MeshFile& vtk_file,SharedArray<Real3>& node_coords)
{
	Real nx,ny,nz;
	const char* buf = vtk_file.getNextLine();
	istringstream iline(buf);


	node_coords.resize(nb_node);
	iline >> ws >> nx >> ws >> ny >> ws >> nz ;
	node_coords[0] = Real3(nx,ny,nz);


	for(Integer i=1;i<nb_node;i++){
		nx = vtk_file.getReal();
		ny = vtk_file.getReal();
		nz = vtk_file.getReal();
		node_coords[i] = Real3(nx,ny,nz);
	}

}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readGeoNodesUnstructuredGrid(int nb_node,MeshFile& vtk_file,CArrayReal3& node_coords)
{
  Real nx,ny,nz;
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);
  
  
  node_coords.resize(nb_node);
  iline >> ws >> nx >> ws >> ny >> ws >> nz ;
  node_coords[0] = Real3(nx,ny,nz);


  for(Integer i=1;i<nb_node;i++){
    nx = vtk_file.getReal();
    ny = vtk_file.getReal();
    nz = vtk_file.getReal();
    node_coords[i] = Real3(nx,ny,nz);
  }

}
#endif /* ARCANE_VERSION */

/*---------------------------------------------------------------------------*/
/* Lit les centres de mailles et leur volumes                                */
/*---------------------------------------------------------------------------*/

void MeshFileIOService::
_readGeoCellsUnstructuredGrid(IPrimaryMesh* mesh,int nb_cell,MeshFile& vtk_file,
    VariableCellReal3* center_cell,VariableCellReal* volume_cell)
{
  Real cx,cy,cz,vol;
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);
  
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture des centres de maille: MshCooresCellCenter n=" << nb_cell;
  Real3ArrayView cellCenter = center_cell->asArray();
  
  
  info() << "Lecture des volumes de maille: MshCooresCellVolume n=" << nb_cell;
  RealArrayView cellVolume = volume_cell->asArray();

  iline >> ws >> cx >> ws >> cy >> ws >> cz ;
  vol = vtk_file.getReal();

  cellCenter[0] = Real3(cx,cy,cz);
  cellVolume[0] = vol ;
  
  for(Integer i=1;i<nb_cell;i++){
    cx  = vtk_file.getReal();
    cy  = vtk_file.getReal();
    cz  = vtk_file.getReal();
    vol = vtk_file.getReal();
    
    cellCenter[i] = Real3(cx,cy,cz);
    cellVolume[i] = vol ;
    
  }

}




/*---------------------------------------------------------------------------------------------------------------*/
/* Lit pour chaque face, le nombre de triplet (aire,centre,normale), les aires, les centres et les normales      */
/*---------------------------------------------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readGeoFacesUnstructuredGrid(IPrimaryMesh* mesh,int nb_face,SharedArray<Integer>& faces_local_id,IntIntMap & id_face,MeshFile& vtk_file,
    VariableFaceReal3 * center_face, VariableFaceReal3 * normal_face, VariableFaceReal * area_face)
{
	Real fx,fy,fz,nx,ny,nz,aire;
	Integer nb_triplet,j;
	const char* buf = vtk_file.getNextLine();
	istringstream iline(buf);

	Integer error = 0 ;

	//TODO Faire la conversion uniqueId() vers localId() correcte
	info() << "Lecture des centres de face: MshCooresFaceCenter n=" << nb_face;
	Real3ArrayView faceCenter = center_face->asArray();


	info() << "Lecture des normales de face: MshCooresFaceNormal n=" << nb_face;
	Real3ArrayView faceNormal = normal_face->asArray();

	info() << "Lecture des aires de face: MshCooresFaceArea n=" << nb_face;
	RealArrayView  faceArea = area_face->asArray();

	info()<<"Reading face properties for : "<<nb_face<<" faces";
	Integer all_faces_num = mesh->allFaces().size() ;
	if(nb_face!=all_faces_num)
	{
		++error ;
		warning()<<"Bad number of faces : "<<nb_face<<" not equal to allfaces size "<<all_faces_num;
	}


	iline >> ws >> nb_triplet;
	aire = vtk_file.getReal();
	fx  = vtk_file.getReal();
	fy  = vtk_file.getReal();
	fz  = vtk_file.getReal();
	nx  = vtk_file.getReal();
	ny  = vtk_file.getReal();
	nz  = vtk_file.getReal();
	if(aire==0)
	{
		warning()<<"Face (0) has a null measure ";
		++error ;
	}
	if((nx*nx+ny*ny+nz*nz)==0)
	{
		warning()<<"Face (0) has a null measure ";
		++error ;
	}


	j    = faces_local_id[0];
	faceArea[j]   = aire ;
	faceCenter[j] = Real3(fx,fy,fz);
	faceNormal[j] = Real3(nx,ny,nz);


	for(Integer i=1;i<nb_face;i++){

		nb_triplet   = vtk_file.getInteger();
		aire = vtk_file.getReal();
		fx   = vtk_file.getReal();
		fy   = vtk_file.getReal();
		fz   = vtk_file.getReal();
		nx   = vtk_file.getReal();
		ny   = vtk_file.getReal();
		nz   = vtk_file.getReal();

		j    = faces_local_id[i];
		faceArea[j]   = aire ;
		faceCenter[j] = Real3(fx,fy,fz);
		faceNormal[j] = Real3(nx,ny,nz);
		if(aire==0)
		{
			warning()<<"Face ("<<i<<") has a null measure ";
			++error ;
		}
		if((nx*nx+ny*ny+nz*nz)==0)
		{
			warning()<<"Face ("<<i<<") has a null measure ";
			++error ;
		}

	}

	if(error)
		warning()<<"Errors ("<<error<<") while reading face properties";
}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readGeoFacesUnstructuredGrid(IPrimaryMesh* mesh,int nb_face,CArrayInteger & faces_local_id,IntIntMap & id_face,MeshFile& vtk_file,
    VariableFaceReal3 * center_face, VariableFaceReal3 * normal_face, VariableFaceReal * area_face)
  Real fx,fy,fz,nx,ny,nz,aire;
  Integer nb_triplet,j;
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);
  
  Integer error = 0 ;

  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture des centres de face: MshCooresFaceCenter n=" << nb_face;
  Real3ArrayView faceCenter = center_face->asArray();


  info() << "Lecture des normales de face: MshCooresFaceNormal n=" << nb_face;
  Real3ArrayView faceNormal = normal_face->asArray();

  info() << "Lecture des aires de face: MshCooresFaceArea n=" << nb_face;
  RealArrayView  faceArea = area_face->asArray();
  
  info()<<"Reading face properties for : "<<nb_face<<" faces";
  Integer all_faces_num = mesh->allFaces().size() ;
  if(nb_face!=all_faces_num)
    {
      ++error ;
      warning()<<"Bad number of faces : "<<nb_face<<" not equal to allfaces size "<<all_faces_num;
    }


  iline >> ws >> nb_triplet;
  aire = vtk_file.getReal();
  fx  = vtk_file.getReal();
  fy  = vtk_file.getReal();
  fz  = vtk_file.getReal();
  nx  = vtk_file.getReal();
  ny  = vtk_file.getReal();
  nz  = vtk_file.getReal();
  if(aire==0)
    {
      warning()<<"Face (0) has a null measure ";
      ++error ;
    }
  if((nx*nx+ny*ny+nz*nz)==0)
    {
      warning()<<"Face (0) has a null measure ";
      ++error ;
    }

 
  j    = faces_local_id[0];
  faceArea[j]   = aire ;
  faceCenter[j] = Real3(fx,fy,fz);
  faceNormal[j] = Real3(nx,ny,nz);


  for(Integer i=1;i<nb_face;i++){
   
    nb_triplet   = vtk_file.getInteger();
    aire = vtk_file.getReal();
    fx   = vtk_file.getReal();
    fy   = vtk_file.getReal();
    fz   = vtk_file.getReal();
    nx   = vtk_file.getReal();
    ny   = vtk_file.getReal();
    nz   = vtk_file.getReal();

    j    = faces_local_id[i];
    faceArea[j]   = aire ;
    faceCenter[j] = Real3(fx,fy,fz);
    faceNormal[j] = Real3(nx,ny,nz);
    if(aire==0)
      {
        warning()<<"Face ("<<i<<") has a null measure ";
        ++error ;
      }
    if((nx*nx+ny*ny+nz*nz)==0)
      {
        warning()<<"Face ("<<i<<") has a null measure ";
        ++error ;
      }

  }
  
  if(error)
    warning()<<"Errors ("<<error<<") while reading face properties";
}
#endif /* ARCANE_VERSION */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
bool MeshFileIOService::
_readUnstructuredGrid(IPrimaryMesh* mesh,MeshFile& vtk_file,bool use_internal_partition)
{
	Integer nb_node = 0;
	Integer nb_cell = 0;
	Integer nb_face = 0;
	Integer nb_cell_node = 0;
	Integer nb_liaison = 0;
	Integer sid = subDomain()->subDomainId();
	SharedArray<Real3> node_coords;
	SharedArray<Int64> cells_infos;

	//Pour les cellules
	SharedArray<Integer> cells_nb_node;
	SharedArray<Int64> cells_connectivity;
	SharedArray<Integer> cells_type,cells_begin;
	SharedArray<Integer>  faces_local_id ;

	IntIntMap  id_node,id_cell,id_face;
	// Si on utilise le partitionneur interne, seul le sous-domaine lit le maillage
	bool need_read = true;
	if (use_internal_partition)
		need_read = (sid==0);

	if (need_read){

		//////////////////////////
		// donnees topologiques //
		/////////////////////////

		//Pour les noeuds

		_readTopoNodesUnstructuredGrid(id_node,nb_node,vtk_file);


		_readTopoCellsUnstructuredGrid(id_cell, id_node,nb_cell,vtk_file,cells_nb_node,cells_type,cells_connectivity,cells_begin);
		nb_cell_node = cells_connectivity.size();


		// Cr?ation des mailles
		// Infos pour la cr?ation des mailles
		// par maille: 1 pour son unique id,
		//             1 pour son type,
		//             1 pour chaque noeud

		SharedArray<Integer> cell_unique_ids(nb_cell);
		for (IntIntMap::iterator it=id_cell.begin(); it!=id_cell.end(); ++it)
			cell_unique_ids[it->second]= it->first;

		cells_infos.resize(nb_cell*2 + nb_cell_node);
		{
			Integer cells_infos_index = 0;
			Integer connectivity_index = 0;
			for( Integer i=0; i<nb_cell; ++i ){
				Integer current_cell_nb_node = cells_nb_node[i];
				Integer cell_unique_id = cell_unique_ids[i];


				cells_infos[cells_infos_index] = cells_type[i];
				++cells_infos_index;

				cells_infos[cells_infos_index] = cell_unique_id;
				++cells_infos_index;

				for( Integer z=0; z<current_cell_nb_node; ++z ){
					cells_infos[cells_infos_index+z] = cells_connectivity[connectivity_index+z];
				}
				cells_infos_index += current_cell_nb_node;
				connectivity_index += current_cell_nb_node;
			}
		}

	}

	mesh->setDimension(3);
	mesh->allocateCells(nb_cell,cells_infos,false);
	mesh->endAllocate();

	VariableCellReal3 * center_cell = new VariableCellReal3(VariableBuildInfo(mesh,"MshCooresCellCenter",IVariable::PNoRestore | IVariable::PPersistant));
        mesh->userDataList()->setData("MshCooresCellCenter",new Arcane::AutoDestroyUserData<Arcane::VariableCellReal3>(center_cell));
	VariableCellReal * volume_cell = new VariableCellReal(VariableBuildInfo(mesh,"MshCooresCellVolume",IVariable::PNoRestore | IVariable::PPersistant));
        mesh->userDataList()->setData("MshCooresCellVolume",new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(volume_cell));
	VariableFaceReal3 * center_face = new VariableFaceReal3(VariableBuildInfo(mesh,"MshCooresFaceCenter",IVariable::PNoRestore | IVariable::PPersistant));
        mesh->userDataList()->setData("MshCooresFaceCenter",new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal3>(center_face));
	VariableFaceReal3 * normal_face = new VariableFaceReal3(VariableBuildInfo(mesh,"MshCooresFaceNormal",IVariable::PNoRestore | IVariable::PPersistant));
        mesh->userDataList()->setData("MshCooresFaceNormal",new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal3>(normal_face));
	VariableFaceReal * area_face = new VariableFaceReal(VariableBuildInfo(mesh,"MshCooresFaceArea",IVariable::PNoRestore | IVariable::PPersistant));
        mesh->userDataList()->setData("MshCooresFaceArea",new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal>(area_face));


	if (need_read)
	{
		//Pour les faces
		_readTopoFacesUnstructuredGrid(mesh,id_face,id_cell,cells_begin,cells_connectivity,nb_face,cells_type,faces_local_id,vtk_file);
	}
	// lit les donn���������������������������es pour les zones de non coincidence
	_readTopoNoMatchingUnstructuredGrid(mesh,id_face,faces_local_id,nb_liaison,vtk_file,need_read);

	if (need_read)
	{
		// lit les donn���������������������������es pour les noeuds se trouvant dans les zones de non coincidence
		_readTopoUpdateNodeUnstructuredGrid(id_node,id_face,faces_local_id,vtk_file);

		///////////////////////////
		// donnees g���������������������������om���������������������������triques  //
		//////////////////////////

		//Pour les noeuds
		_readGeoNodesUnstructuredGrid(nb_node,vtk_file,node_coords);

		//Pour les mailles, ce n'est pas encore pris en compte dans Arcane, on va donc sauter ces donn���������������������������es
		_readGeoCellsUnstructuredGrid(mesh,nb_cell,vtk_file,center_cell,volume_cell);

		//Pour les faces, ce n'est pas encore pris en compte dans Arcane, on va donc sauter ces donn���������������������������es
		_readGeoFacesUnstructuredGrid(mesh,nb_face,faces_local_id,id_face,vtk_file,center_face,normal_face,area_face);
	}

	//donn���������������������������es g���������������������������om���������������������������triques pour la non-coincidence, pas pris en compte dans Arcane
	_readGeoNomatchingUnstructuredGrid(mesh,nb_liaison,vtk_file,need_read);

	// Positionne les coordonn?es
	{
		VariableNodeReal3& nodes_coord_var(PRIMARYMESH_CAST(mesh)->nodesCoordinates());
		ENUMERATE_NODE(inode,mesh->allNodes()){
			const Node& node = *inode;
			nodes_coord_var[inode] = node_coords[node.uniqueId().asInt64()];
		}
	}


#if 1
	// Maintenant, regarde s'il existe des donn���������������������������es associ���������������������������es cell aux fichiers
	bool r = _readData(   mesh,
			id_node,
			id_cell,
			id_face,
			vtk_file,
			use_internal_partition,
			faces_local_id
	);

	return r ;
#else
	return false ;
#endif



	return true;


}
#else /* ARCANE_VERSION */
bool MeshFileIOService::
_readUnstructuredGrid(IPrimaryMesh* mesh,MeshFile& vtk_file,bool use_internal_partition)
{
  Integer nb_node = 0;
  Integer nb_cell = 0;
  Integer nb_face = 0;
  Integer nb_cell_node = 0;
  Integer nb_liaison = 0;
  Integer sid = subDomain()->subDomainId();
  CArrayReal3 node_coords;
  CArrayInt64 cells_infos;
   
  //Pour les cellules
  CArrayT<Integer> cells_nb_node;
  CArrayT<Int64> cells_connectivity;
  CArrayT<Integer> cells_type,cells_begin;
  CArrayT<Integer>  faces_local_id ;

  IntIntMap  id_node,id_cell,id_face;
  // Si on utilise le partitionneur interne, seul le sous-domaine lit le maillage
  bool need_read = true;
  if (use_internal_partition)
    need_read = (sid==0);

  if (need_read){
    
    //////////////////////////
    // donnees topologiques //
    /////////////////////////
    
    //Pour les noeuds
     
    _readTopoNodesUnstructuredGrid(id_node,nb_node,vtk_file);
    
   
    _readTopoCellsUnstructuredGrid(id_cell, id_node,nb_cell,vtk_file,cells_nb_node,cells_type,cells_connectivity,cells_begin);
    nb_cell_node = cells_connectivity.size();
   
    
    // Cr?ation des mailles
    // Infos pour la cr?ation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             1 pour chaque noeud

    SharedArray<Integer> cell_unique_ids(nb_cell);
    for (IntIntMap::iterator it=id_cell.begin(); it!=id_cell.end(); ++it)
      cell_unique_ids[it->second]= it->first;

    cells_infos.resize(nb_cell*2 + nb_cell_node);
    {
      Integer cells_infos_index = 0;
      Integer connectivity_index = 0;
      for( Integer i=0; i<nb_cell; ++i ){
        Integer current_cell_nb_node = cells_nb_node[i];
        Integer cell_unique_id = cell_unique_ids[i];
        
  
        cells_infos[cells_infos_index] = cells_type[i];
        ++cells_infos_index;
  
        cells_infos[cells_infos_index] = cell_unique_id;
        ++cells_infos_index;
  
        for( Integer z=0; z<current_cell_nb_node; ++z ){
          cells_infos[cells_infos_index+z] = cells_connectivity[connectivity_index+z];
        }
        cells_infos_index += current_cell_nb_node;
        connectivity_index += current_cell_nb_node;
      }
    }
    
  }
    
  mesh->setDimension(3);
  mesh->allocateCells(nb_cell,cells_infos,false);
  mesh->endAllocate();


  VariableCellReal3 * center_cell = new VariableCellReal3(VariableBuildInfo(mesh,"MshCooresCellCenter",IVariable::PNoRestore | IVariable::PPersistant));
  mesh->userDataList()->setData("MshCooresCellCenter",new Arcane::AutoDestroyUserData<Arcane::VariableCellReal3>(center_cell));
  VariableCellReal * volume_cell = new VariableCellReal(VariableBuildInfo(mesh,"MshCooresCellVolume",IVariable::PNoRestore | IVariable::PPersistant));
  mesh->userDataList()->setData("MshCooresCellVolume",new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(volume_cell));
  VariableFaceReal3 * center_face = new VariableFaceReal3(VariableBuildInfo(mesh,"MshCooresFaceCenter",IVariable::PNoRestore | IVariable::PPersistant));
  mesh->userDataList()->setData("MshCooresFaceCenter",new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal3>(center_face));
  VariableFaceReal3 * normal_face = new VariableFaceReal3(VariableBuildInfo(mesh,"MshCooresFaceNormal",IVariable::PNoRestore | IVariable::PPersistant));
  mesh->userDataList()->setData("MshCooresFaceNormal",new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal3>(normal_face));
  VariableFaceReal * area_face = new VariableFaceReal(VariableBuildInfo(mesh,"MshCooresFaceArea",IVariable::PNoRestore | IVariable::PPersistant));
  mesh->userDataList()->setData("MshCooresFaceArea",new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal>(area_face));


  if (need_read) 
    {
      //Pour les faces
      _readTopoFacesUnstructuredGrid(mesh,id_face,id_cell,cells_begin,cells_connectivity,nb_face,cells_type,faces_local_id,vtk_file);
    }
  // lit les donn���������es pour les zones de non coincidence
  _readTopoNoMatchingUnstructuredGrid(mesh,id_face,faces_local_id,nb_liaison,vtk_file,need_read);
   
  if (need_read) 
    {
      // lit les donn���������es pour les noeuds se trouvant dans les zones de non coincidence
      _readTopoUpdateNodeUnstructuredGrid(id_node,id_face,faces_local_id,vtk_file);

      ///////////////////////////
      // donnees g���������om���������triques  //
      //////////////////////////
      
      //Pour les noeuds
      _readGeoNodesUnstructuredGrid(nb_node,vtk_file,node_coords);
        
      //Pour les mailles, ce n'est pas encore pris en compte dans Arcane, on va donc sauter ces donn���������es
      _readGeoCellsUnstructuredGrid(mesh,nb_cell,vtk_file,center_cell,volume_cell);
        
      //Pour les faces, ce n'est pas encore pris en compte dans Arcane, on va donc sauter ces donn���������es
      _readGeoFacesUnstructuredGrid(mesh,nb_face,faces_local_id,id_face,vtk_file,center_face,normal_face,area_face);
    }
   
  //donn���������es g���������om���������triques pour la non-coincidence, pas pris en compte dans Arcane
  _readGeoNomatchingUnstructuredGrid(mesh,nb_liaison,vtk_file,need_read);
    
  // Positionne les coordonn?es
  {
      VariableNodeReal3& nodes_coord_var(PRIMARYMESH_CAST(mesh)->nodesCoordinates());
    ENUMERATE_NODE(inode,mesh->allNodes()){
      const Node& node = *inode;
      nodes_coord_var[inode] = node_coords[node.uniqueId().asInt64()];
    }
  }
 
    
#if 1
  // Maintenant, regarde s'il existe des donn���������es associ���������es cell aux fichiers
  bool r = _readData(   mesh,
                        id_node,
                        id_cell,
                        id_face,
                        vtk_file,
                        use_internal_partition,
                        faces_local_id
                        ); 
      
  return r ;
#else
  return false ;
#endif   


   
  return true;
    
    
}
#endif /* ARCANE_VERSION */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
bool MeshFileIOService::
_readTopoFacesUnstructuredGrid(IPrimaryMesh* mesh,IntIntMap & id_face, IntIntMap & id_cell, SharedArray<Integer>& cells_begin, SharedArray<Int64>& cells_connectivity, Integer & nb_face,SharedArray<Integer>& cells_type,SharedArray<Integer>& faces_local_id,MeshFile& vtk_file)
{

	const char* func_name = "MeshFileIOService::_readTopoFacesUnstructuredGrid()";
	const char* buf = vtk_file.getNextLine();
	istringstream iline(buf);
	SharedArray<Integer> faces_nb_node;
	SharedArray<Int64>   faces_connectivity;
	Integer connectivity = 0;

	//definition des faces pour chaque type de mailles
	int hexa_loc[6][4]             = { {0,3,2,1 } , {0,4,7,3 } , {0,1,5,4 } , {4,5,6,7 } , {1,2,6,5 } , {2,3,7,6}  };
	int tetra_loc[4][3]            = { {0,2,1   } , {0,3,2   } , {0,1,3   } , {1,2,3   }                           };
	int pyramid_loc[5][4]          = { {0,3,2,1 } , {0,4,3,-1} , {0,1,4,-1} , {1,2,4,-1} , {2,3,4,-1}              };
	int wedge_loc[5][4]            = { {0,2,1,-1} , {0,3,5,2 } , {0,1,4,3 } , {3,4,5,-1} , {1,2,5,4 }              };
	int hemihexa_loc[6][4]         = { {0,1,2,-1} , {0,2,3,4 } , {0,5,6,1 } , {0,4,5,-1} , {1,6,3,2 } , {3,6,5,4 } };
	int hemihexa2_loc[6][4]        = { {0,1,2,-1} , {0,2,3,4 } , {0,5,3,1 } , {0,4,5,-1} , {1,3,2,-1} , {3,5,4,-1} };
	int antiwedge_left_loc[6][4]   = { {0,2,1,-1} , {0,3,5,2 } , {0,1,4,3 } , {3,4,5,-1} , {1,2,4,-1} , {2,5,4,-1} };
	int antiwedge_right_loc[6][4]  = { {0,2,1,-1} , {0,3,5,2 } , {0,1,4,3 } , {3,4,5,-1} , {1,2,5,-1} , {1,5,4,-1} };
	int ditetra_loc[6][4]          = { {0,1,3,-1} , {1,2,3,-1} , {2,0,3,-1} , {1,0,4,-1} , {2,1,4,-1} , {0,2,4,-1} };
	int hemihexa3_loc[4][4]        = { {0,1,2,-1} , {0,2,3,4 } , {0,4,3,1 } , {1,3,2,-1}                           };

	// on lit le nombre de faces
	iline >> ws >> nb_face ;

	if (nb_face<=0){
		String msg = "Nombre de faces invalide: n=" + nb_face;
		throw IOException(func_name,msg);
	}


	if(nb_face > 0){
		Integer m;
		buf = vtk_file.getNextLine();
		istringstream iline(buf);
		iline >> ws >> m;
		id_face[m] = 0;
	}


	faces_nb_node.resize(nb_face);
	faces_connectivity.resize(nb_face*4);// nombre de noeuds par face limit��������������������������� ��������������������������� 4


	for(Integer i = 0;i<nb_face;i++){
		Integer m;

		// on lit l'identifiant de face
		if(i>0){
			m = vtk_file.getInteger();
			id_face[m] = i;
		}

		// on lit l'identifiant de maille associ��������������������������� a la face
		Integer cell_face  = vtk_file.getInteger();
		//numero de maille correspondant ��������������������������� cell_face
		Integer num_cell = id_cell[cell_face];

		// on recupere le numero local de la face par rapport a cette maille
		Integer face_local = vtk_file.getInteger();


		// on met a jour les noeuds qui composent la face
		Integer cell_type =  cells_type[num_cell];

		Integer begin_cell_connectivity = cells_begin[num_cell];

		faces_nb_node[i] = 0;


		switch(cell_type){



		case IT_Hexaedron8  :{//hexaedre
			for(Integer j=0;j<4;j++){
				int node_local  = hexa_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_Tetraedron4  :{//hexaedre
			for(Integer j=0;j<3;j++){
				int node_local  = tetra_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_Pyramid5  :{//hexaedre
			for(Integer j=0;j<4;j++){
				int node_local  = pyramid_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_Pentaedron6  :{//hexaedre
			for(Integer j=0;j<4;j++){
				int node_local  = wedge_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		// traitement des types particuliers
		case IT_HemiHexa7 :{
			for(Integer j=0;j<4;j++){
				int node_local  = hemihexa_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}


		case IT_HemiHexa6 :{
			for(Integer j=0;j<4;j++){
				int node_local  = hemihexa2_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_HemiHexa5 :{
			for(Integer j=0;j<4;j++){

				int node_local  = hemihexa3_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_AntiWedgeLeft6 :{
			for(Integer j=0;j<4;j++){
				int node_local  = antiwedge_left_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_AntiWedgeRight6 :{
			for(Integer j=0;j<4;j++){
				int node_local  = antiwedge_right_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}

		case IT_DiTetra5 :{
			for(Integer j=0;j<4;j++){
				int node_local  = ditetra_loc[face_local][j] ;
				if( node_local != -1){
					faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
					connectivity++ ;
					faces_nb_node[i] ++;
				}
			}
			break;
		}


		}



	}

#if  1
	faces_local_id.resize(nb_face);
	{
		IMeshUtilities* mu = mesh->utilities();
		mu->localIdsFromConnectivity(IK_Face,faces_nb_node,faces_connectivity,faces_local_id);
	}
#endif
	if(nb_face!=mesh->allFaces().size())
	{
		warning()<<"There are missing face";
		std::set<Integer> lids ;
		for(Integer iface=0;iface<nb_face;++iface)
			lids.insert(faces_local_id[iface]) ;
		ENUMERATE_FACE(iface,mesh->allFaces())
		{
			const Face& face = *iface ;
			Integer lid = iface->localId() ;
			if(lids.find(lid)==lids.end())
			{
				info()<<"Face : "<<iface->uniqueId()<<" is missing ";
				info()<<"Node connectivity :";
				for(NodeEnumerator inode(face.nodes());inode();++inode)
				{
					info()<<"Node uid :"<<inode->uniqueId();
				}
				info()<<"Cell connectivity :";
				for(CellEnumerator icell(face.cells());icell();++icell)
				{
					info()<<"Cell uid :"<<icell->uniqueId();
				}
			}
		}
		warning()<<"Faces is missing, cannot go on simulation";
	}

	return true;
}



bool MeshFileIOService::
check(Integer i, Integer j, Integer k, Integer nx, Integer ny, Integer nz)
{
	if(i<0||i>nx)
	{
		error()<<"i:"<<i<<"out of range";
		return true ;
	}
	if(j<0||j>ny)
	{
		error()<<"j:"<<i<<"out of range";
		return true ;
	}
	if(k<0||k>nz)
	{
		error()<<"k:"<<i<<"out of range";
		return true ;
	}
	return false ;
}

bool MeshFileIOService::
check(Integer i, Integer first, Integer last)
{
	if(i<first||i>last)
	{
		error()<<"i:"<<i<<"out of range";
		return true ;
	}
	return false ;
}
#else /* ARCANE_VERSION */
bool MeshFileIOService::
_readTopoFacesUnstructuredGrid(IPrimaryMesh* mesh,IntIntMap & id_face, IntIntMap & id_cell, CArrayInteger& cells_begin, CArrayInt64& cells_connectivity, Integer & nb_face,CArrayInteger& cells_type,CArrayInteger & faces_local_id,MeshFile& vtk_file)
{

  const char* func_name = "MeshFileIOService::_readTopoFacesUnstructuredGrid()";
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);
  CArrayT<Integer> faces_nb_node;
  CArrayT<Int64>   faces_connectivity;
  Integer connectivity = 0;
  
  //definition des faces pour chaque type de mailles
  int hexa_loc[6][4]             = { {0,3,2,1 } , {0,4,7,3 } , {0,1,5,4 } , {4,5,6,7 } , {1,2,6,5 } , {2,3,7,6}  };
  int tetra_loc[4][3]            = { {0,2,1   } , {0,3,2   } , {0,1,3   } , {1,2,3   }                           };
  int pyramid_loc[5][4]          = { {0,3,2,1 } , {0,4,3,-1} , {0,1,4,-1} , {1,2,4,-1} , {2,3,4,-1}              };
  int wedge_loc[5][4]            = { {0,2,1,-1} , {0,3,5,2 } , {0,1,4,3 } , {3,4,5,-1} , {1,2,5,4 }              };
  int hemihexa_loc[6][4]         = { {0,1,2,-1} , {0,2,3,4 } , {0,5,6,1 } , {0,4,5,-1} , {1,6,3,2 } , {3,6,5,4 } };
  int hemihexa2_loc[6][4]        = { {0,1,2,-1} , {0,2,3,4 } , {0,5,3,1 } , {0,4,5,-1} , {1,3,2,-1} , {3,5,4,-1} };
  int antiwedge_left_loc[6][4]   = { {0,2,1,-1} , {0,3,5,2 } , {0,1,4,3 } , {3,4,5,-1} , {1,2,4,-1} , {2,5,4,-1} };
  int antiwedge_right_loc[6][4]  = { {0,2,1,-1} , {0,3,5,2 } , {0,1,4,3 } , {3,4,5,-1} , {1,2,5,-1} , {1,5,4,-1} };
  int ditetra_loc[6][4]          = { {0,1,3,-1} , {1,2,3,-1} , {2,0,3,-1} , {1,0,4,-1} , {2,1,4,-1} , {0,2,4,-1} };
  int hemihexa3_loc[4][4]        = { {0,1,2,-1} , {0,2,3,4 } , {0,4,3,1 } , {1,3,2,-1}                           };
     
  // on lit le nombre de faces
  iline >> ws >> nb_face ;
  
  if (nb_face<=0){
    String msg = "Nombre de faces invalide: n=" + nb_face;
    throw IOException(func_name,msg);
  }
  

  if(nb_face > 0){
    Integer m;
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    iline >> ws >> m;
    id_face[m] = 0;
  }
  
  
  faces_nb_node.resize(nb_face);
  faces_connectivity.resize(nb_face*4);// nombre de noeuds par face limit��������������������������� ��������������������������� 4


  for(Integer i = 0;i<nb_face;i++){
    Integer m;

    // on lit l'identifiant de face
    if(i>0){
      m = vtk_file.getInteger();
      id_face[m] = i;
    }

    // on lit l'identifiant de maille associ��������� a la face
    Integer cell_face  = vtk_file.getInteger();
    //numero de maille correspondant ��������� cell_face
    Integer num_cell = id_cell[cell_face];

    // on recupere le numero local de la face par rapport a cette maille
    Integer face_local = vtk_file.getInteger();

    
    // on met a jour les noeuds qui composent la face
    Integer cell_type =  cells_type[num_cell];

    Integer begin_cell_connectivity = cells_begin[num_cell];
    
    faces_nb_node[i] = 0;


    switch(cell_type){
      
      
      
    case IT_Hexaedron8  :{//hexaedre
      for(Integer j=0;j<4;j++){
        int node_local  = hexa_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }
      
    case IT_Tetraedron4  :{//hexaedre
      for(Integer j=0;j<3;j++){
        int node_local  = tetra_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }   
   
    case IT_Pyramid5  :{//hexaedre
      for(Integer j=0;j<4;j++){
        int node_local  = pyramid_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }   

    case IT_Pentaedron6  :{//hexaedre
      for(Integer j=0;j<4;j++){
        int node_local  = wedge_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] =  cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }   

      // traitement des types particuliers
    case IT_HemiHexa7 :{
      for(Integer j=0;j<4;j++){
        int node_local  = hemihexa_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }

      
    case IT_HemiHexa6 :{
      for(Integer j=0;j<4;j++){
        int node_local  = hemihexa2_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }

    case IT_HemiHexa5 :{
      for(Integer j=0;j<4;j++){

        int node_local  = hemihexa3_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }

    case IT_AntiWedgeLeft6 :{
      for(Integer j=0;j<4;j++){
        int node_local  = antiwedge_left_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }  
     
    case IT_AntiWedgeRight6 :{
      for(Integer j=0;j<4;j++){
        int node_local  = antiwedge_right_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }  

    case IT_DiTetra5 :{
      for(Integer j=0;j<4;j++){
        int node_local  = ditetra_loc[face_local][j] ;
        if( node_local != -1){
          faces_connectivity[connectivity] = cells_connectivity[begin_cell_connectivity + node_local];
          connectivity++ ;
          faces_nb_node[i] ++;
        }
      }
      break;
    }  

      
    }
    


  }

#if  1 
  faces_local_id.resize(nb_face);
  {
    IMeshUtilities* mu = mesh->utilities();
    mu->localIdsFromConnectivity(IK_Face,faces_nb_node,faces_connectivity,faces_local_id);
  }
#endif 
  if(nb_face!=mesh->allFaces().size())
    {
      warning()<<"There are missing face";
      std::set<Integer> lids ;
      for(Integer iface=0;iface<nb_face;++iface)
        lids.insert(faces_local_id[iface]) ;
      ENUMERATE_FACE(iface,mesh->allFaces())
        {
          const Face& face = *iface ;
          Integer lid = iface->localId() ;
          if(lids.find(lid)==lids.end())
            {
              info()<<"Face : "<<iface->uniqueId()<<" is missing ";
              info()<<"Node connectivity :";
              for(NodeEnumerator inode(face.nodes());inode();++inode)
                {
                  info()<<"Node uid :"<<inode->uniqueId();
                }
              info()<<"Cell connectivity :";
              for(CellEnumerator icell(face.cells());icell();++icell)
                {
                  info()<<"Cell uid :"<<icell->uniqueId();
                }
            }
        }
      warning()<<"Faces is missing, cannot go on simulation"; 
    }

  return true;
}



bool MeshFileIOService::
check(Integer i, Integer j, Integer k, Integer nx, Integer ny, Integer nz)
{
  if(i<0||i>nx) 
    {
      error()<<"i:"<<i<<"out of range";
      return true ;
    }
  if(j<0||j>ny)
    {
      error()<<"j:"<<i<<"out of range";
      return true ;
    }
  if(k<0||k>nz)
    {
      error()<<"k:"<<i<<"out of range";
      return true ;
    }
  return false ;
}

bool MeshFileIOService::
check(Integer i, Integer first, Integer last)
{
  if(i<first||i>last) 
    {
      error()<<"i:"<<i<<"out of range";
      return true ;
    }
  return false ;
}
#endif /* ARCANE_VERSION */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
bool MeshFileIOService::_readData( IPrimaryMesh* mesh,
		IntIntMap & id_node,
		IntIntMap & id_cell,
		IntIntMap & id_face,
		MeshFile& vtk_file,
		bool use_internal_partition,
		SharedArray<Integer>& faces_local_id,
		const char * buf)
{
	// Seul le sous-domain maitre lit les valeurs. Par contre, les autres
	// sous-domaines doivent connaitre la liste des variables et groupes cr??es.
	// Si une donn?e porte le nom 'GROUP_*', on consid?re qu'il s'agit d'un
	// groupe
	info()<<"READATA :****************************";
	OStringStream created_infos_str;
	created_infos_str() << "<?xml version='1.0' ?>\n";
	created_infos_str() << "<infos>";
	Integer sid = subDomain()->subDomainId();
	IParallelMng* pm = subDomain()->parallelMng();

	Integer nb_node_kind = mesh->nbItem(IK_Node);
	Integer nb_cell_kind = mesh->nbItem(IK_Cell);
	Integer nb_face_kind = mesh->nbItem(IK_Face);

	std::string node_kind = "NOEUD";
	std::string cell_kind = "MAILLE";
	std::string face_kind = "FACE";

	bool reading_node = false;
	bool reading_cell = false;
	bool reading_face = false;

	if (sid==0){
		bool continue_loop = ((buf!=NULL) or ( !vtk_file.isEnd() && (buf=vtk_file.getNextLine(MeshFile::OPTIONAL_LINE))!=0));

		while(continue_loop) {
			info() << "Read line";
			istringstream iline(buf);
			std::string data_str;
			iline >> ws >> data_str;  //recup���������������������������re le type d'item (maille,noeud,face)
			if (data_str=="NOEUD"){
				reading_node = true;
				reading_cell = false;
				reading_face = false;
			}
			else if (data_str=="MAILLE"){
				reading_cell = true;
				reading_node = false;
				reading_face = false;
			}
			else if (data_str=="FACE"){
				reading_cell = false;
				reading_node = false;
				reading_face = true;
			}
			else if (reading_node){

				if(_readNodeData( mesh,  id_node,vtk_file,
						node_kind,
						nb_node_kind,
						iline,
						data_str,
						created_infos_str))
					return true ;
			}
			else if(reading_cell){

				if(_readCellData( mesh,  id_cell,
						vtk_file,
						cell_kind,
						nb_cell_kind,
						iline,
						data_str,
						created_infos_str))
					return true ;
			}
			else if (reading_face){

				if(_readFaceData(  mesh, id_face,vtk_file,
						face_kind,
						nb_face_kind,
						iline,
						faces_local_id,
						data_str,
						created_infos_str))
					return true ;
			}
			else {
				error() << "Wait value POINT_DATA, FACE_DATA or CELL_DATAlu='" << data_str << "'";
				return true;
			}
			continue_loop = ( !vtk_file.isEnd() && (buf=vtk_file.getNextLine())!=0);
		}

	}



	created_infos_str() << "</infos>";
	if (use_internal_partition){
		ByteSharedArray bytes;
		if (sid==0){
			info()<<"READ DATA :**********PROC MASTER";
			const String str = created_infos_str.str();
			Integer len = str.len();
			bytes.resize(len+1);
			::memcpy(bytes.unguardedBasePointer(),str.localstr(),len+1);
			info() << "SEND STR=" << str;
		}
		pm->broadcastMemoryBuffer(bytes,0);
		if (sid!=0){
			String str((char*)bytes.unguardedBasePointer(),bytes.size());
			info() << "FOUND STR=" << bytes.size() << " " << str;
			IIOMng* iomng = subDomain()->ioMng();
			IXmlDocumentHolder* doc = iomng->parseXmlBuffer(bytes,"InternalBuffer");
			XmlNode doc_node = doc->documentNode();
			// Lecture des variables
			{
				XmlNodeList vars = doc_node.documentElement().children("node-variable-real");
#if (ARCANE_VERSION<11602)
				for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
					for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
						XmlNode xnode = *i;
						String name = xnode.attrValue("name");
						info() << "Creation variable: " << name;
						VariableNodeReal* var = new VariableNodeReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
					        mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal>(var));
					}
				}
				{
					XmlNodeList vars = doc_node.documentElement().children("node-variable-int64");
#if (ARCANE_VERSION<11602)
					for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
						for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
							XmlNode xnode = *i;
							String name = xnode.attrValue("name");
							info() << "Creation variable: " << name;
							VariableNodeInt64* var = new VariableNodeInt64(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
	                                                mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeInt64>(var));
						}
					}

					// Lecture des groupes
					{
						XmlNodeList vars = doc_node.documentElement().children("node-group");
						IItemFamily* node_family = mesh->itemFamily(IK_Node);
#if (ARCANE_VERSION<11602)
						for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
							for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
								XmlNode xnode = *i;
								String name = xnode.attrValue("name");
								info() << "Cr���������������������������ation group: " << name;
								node_family->createGroup(name);
							}
						}


						// Lecture des variables
						{
							XmlNodeList vars = doc_node.documentElement().children("cell-variable-real");
#if (ARCANE_VERSION<11602)
							for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
								for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
									XmlNode xnode = *i;
									String name = xnode.attrValue("name");
									info() << "Cr?ation variable: " << name;
									VariableCellReal* var = new VariableCellReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
		                                                        mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(var));
								}
							}
							{
								XmlNodeList vars = doc_node.documentElement().children("cell-variable-int64");
#if (ARCANE_VERSION<11602)
								for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
									for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
										XmlNode xnode = *i;
										String name = xnode.attrValue("name");
										info() << "Cr?ation variable: " << name;
										VariableCellInt64* var = new VariableCellInt64(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
	                                                                        mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellInt64>(var));
									}
								}
								{
	                                                                XmlNodeList vars = doc_node.documentElement().children("cell-variable-integer");
#if (ARCANE_VERSION<11602)
	                                                                for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
	                                                                        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
	                                                                                XmlNode xnode = *i;
	                                                                                String name = xnode.attrValue("name");
	                                                                                info() << "Cr?ation variable: " << name;
	                                                                                VariableCellInteger* var = new VariableCellInteger(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
	                                                                                mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellInteger>(var));
	                                                                        }
	                                                                }
	                                                                {
									XmlNodeList vars = doc_node.documentElement().children("cell-variable-real3");
#if (ARCANE_VERSION<11602)
									for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
										for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
											XmlNode xnode = *i;
											String name = xnode.attrValue("name");
											info() << "Creation variable: " << name;
											VariableCellReal3* var = new VariableCellReal3(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
                                                                                        mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal3>(var));
										}
									}

									// Lecture des groupes
									{
										XmlNodeList vars = doc_node.documentElement().children("cell-group");
										IItemFamily* cell_family = mesh->itemFamily(IK_Cell);
#if (ARCANE_VERSION<11602)
										for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
											for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
												XmlNode xnode = *i;
												String name = xnode.attrValue("name");
												info() << "Creation group: " << name;
												cell_family->createGroup(name);
											}
										}


										// Lecture des variables
										{
											XmlNodeList vars = doc_node.documentElement().children("face-variable-real");
#if (ARCANE_VERSION<11602)
											for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
												for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
													XmlNode xnode = *i;
													String name = xnode.attrValue("name");
													info() << "Creation variable: " << name;
													VariableFaceReal* var = new VariableFaceReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
		                                                                                        mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal>(var));
												}
											}
											{
												XmlNodeList vars = doc_node.documentElement().children("face-variable-integer");
#if (ARCANE_VERSION<11602)
												for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
													for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
														XmlNode xnode = *i;
														String name = xnode.attrValue("name");
														info() << "Creation variable: " << name;
														VariableFaceInteger* var = new VariableFaceInteger(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
	                                                                                                        mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceInteger>(var));
													}
												}
												{
												XmlNodeList vars = doc_node.documentElement().children("face-variable-int64");
#if (ARCANE_VERSION<11602)
												for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
													for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
														XmlNode xnode = *i;
														String name = xnode.attrValue("name");
														info() << "Creation variable: " << name;
														VariableFaceInt64* var = new VariableFaceInt64(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
                                                                                                                mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceInt64>(var));
													}
												}
												{
													XmlNodeList vars = doc_node.documentElement().children("face-variable-real3");
#if (ARCANE_VERSION<11602)
													for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
														for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
															XmlNode xnode = *i;
															String name = xnode.attrValue("name");
															info() << "Creation variable: " << name;
															VariableFaceReal3* var = new VariableFaceReal3(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
	                                                                                                                mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal3>(var));
														}
													}

													// Lecture des groupes
													{
														XmlNodeList vars = doc_node.documentElement().children("face-group");
														IItemFamily* face_family = mesh->itemFamily(IK_Face);
#if (ARCANE_VERSION<11602)
														for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
															for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
																XmlNode xnode = *i;
																String name = xnode.attrValue("name");
																info() << "Creation group: " << name;
																face_family->createGroup(name);
															}
														}


													}
												}
												return false;
											}
#else /* ARCANE_VERSION */
bool MeshFileIOService::_readData( IPrimaryMesh* mesh,
                                   IntIntMap & id_node,
                                   IntIntMap & id_cell,
                                   IntIntMap & id_face,
                                   MeshFile& vtk_file,
                                   bool use_internal_partition,
                                   CArrayInteger & faces_local_id,
                                   const char * buf)
{
  // Seul le sous-domain maitre lit les valeurs. Par contre, les autres
  // sous-domaines doivent connaitre la liste des variables et groupes cr??es.
  // Si une donn?e porte le nom 'GROUP_*', on consid?re qu'il s'agit d'un
  // groupe
  info()<<"READATA :****************************";
  OStringStream created_infos_str;
  created_infos_str() << "<?xml version='1.0' ?>\n";
  created_infos_str() << "<infos>";
  Integer sid = subDomain()->subDomainId();
  IParallelMng* pm = subDomain()->parallelMng();

  Integer nb_node_kind = mesh->nbItem(IK_Node);
  Integer nb_cell_kind = mesh->nbItem(IK_Cell);
  Integer nb_face_kind = mesh->nbItem(IK_Face);

  std::string node_kind = "NOEUD";
  std::string cell_kind = "MAILLE";
  std::string face_kind = "FACE";

  bool reading_node = false;
  bool reading_cell = false;
  bool reading_face = false;

  if (sid==0){
    bool continue_loop = ((buf!=NULL) or ( !vtk_file.isEnd() && (buf=vtk_file.getNextLine(MeshFile::OPTIONAL_LINE))!=0));

    while(continue_loop) {
      info() << "Read line";
      istringstream iline(buf);
      std::string data_str;
      iline >> ws >> data_str;  //recup���������re le type d'item (maille,noeud,face)
      if (data_str=="NOEUD"){
        reading_node = true;
        reading_cell = false;
        reading_face = false;
      }
      else if (data_str=="MAILLE"){
        reading_cell = true;
        reading_node = false;
        reading_face = false;
      }
      else if (data_str=="FACE"){
        reading_cell = false;
        reading_node = false;
        reading_face = true;
      }
      else if (reading_node){
  
        if(_readNodeData( mesh,  id_node,vtk_file,
                          node_kind,
                          nb_node_kind,
                          iline,
                          data_str,
                          created_infos_str)) 
          return true ;
      }
      else if(reading_cell){
  
        if(_readCellData( mesh,  id_cell,
                            vtk_file,
                            cell_kind,
                            nb_cell_kind,
                            iline,
                            data_str,
                            created_infos_str))
          return true ;
      }
      else if (reading_face){
  
        if(_readFaceData(  mesh, id_face,vtk_file,
                           face_kind,
                           nb_face_kind,
                           iline,
                           faces_local_id,
                           data_str,
                           created_infos_str)) 
          return true ;
      }
      else {
        error() << "Wait value POINT_DATA, FACE_DATA or CELL_DATAlu='" << data_str << "'";
        return true;
      }
      continue_loop = ( !vtk_file.isEnd() && (buf=vtk_file.getNextLine())!=0);
    }

  }



  created_infos_str() << "</infos>";
  if (use_internal_partition){
    ByteSharedArray bytes;
    if (sid==0){
      info()<<"READ DATA :**********PROC MASTER";
      const String str = created_infos_str.str();
      Integer len = str.len();
      bytes.resize(len+1);
      ::memcpy(bytes.begin(),str.localstr(),len+1);
      info() << "SEND STR=" << str;
    }
    pm->broadcastMemoryBuffer(bytes,0);
    if (sid!=0){
      String str((char*)bytes.begin(),bytes.size());
      info() << "FOUND STR=" << bytes.size() << " " << str;
      IIOMng* iomng = subDomain()->ioMng();
      IXmlDocumentHolder* doc = iomng->parseXmlBuffer(bytes,"InternalBuffer");
      XmlNode doc_node = doc->documentNode();
      // Lecture des variables
      {
        XmlNodeList vars = doc_node.documentElement().children("node-variable-real");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation variable: " << name;
          VariableNodeReal* var = new VariableNodeReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("node-variable-int64");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation variable: " << name;
          VariableNodeInt64* var = new VariableNodeInt64(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeInt64>(var));
        }
      }
      
      // Lecture des groupes
      {
        XmlNodeList vars = doc_node.documentElement().children("node-group");
        IItemFamily* node_family = mesh->itemFamily(IK_Node);
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Cr���������ation group: " << name;
          node_family->createGroup(name);
        }
      }
      

      // Lecture des variables
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-variable-real");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Cr?ation variable: " << name;
          VariableCellReal* var = new VariableCellReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-variable-int64");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Cr?ation variable: " << name;
          VariableCellInt64* var = new VariableCellInt64(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellInt64>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-variable-real3");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation variable: " << name;
          VariableCellReal3* var = new VariableCellReal3(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal3>(var));
        }
      }
      
      // Lecture des groupes
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-group");
        IItemFamily* cell_family = mesh->itemFamily(IK_Cell);
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation group: " << name;
          cell_family->createGroup(name);
        }
      }


      // Lecture des variables
      {
        XmlNodeList vars = doc_node.documentElement().children("face-variable-real");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation variable: " << name;
          VariableFaceReal* var = new VariableFaceReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("face-variable-int64");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation variable: " << name;
          VariableFaceInt64* var = new VariableFaceInt64(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceInt64>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("face-variable-real3");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation variable: " << name;
          VariableFaceReal3* var = new VariableFaceReal3(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal3>(var));
        }
      }

      // Lecture des groupes
      {
        XmlNodeList vars = doc_node.documentElement().children("face-group");
        IItemFamily* face_family = mesh->itemFamily(IK_Face);
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){   
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation group: " << name;
          face_family->createGroup(name);
        }
      }
     

    }
  }
  return false;
}
#endif /* ARCANE_VERSION */


bool MeshFileIOService::_readNodeData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
                                      std::string & item, 
                                      int nb_item_kind,
                                      istringstream& iline, 
                                      std::string& name_str,
                                      OStringStream& created_infos_str)
{
  
  
  
  int nb_component = 0;
  int nb_item_var  = 0;
  bool is_group = false;
  bool is_int64 = false;
  
  info() << "** ** ** READNAME: name=" << name_str ; // nom de variable
  nb_component = vtk_file.getInteger(); // nombre de valeur pour definir la variable
  nb_item_var  = vtk_file.getInteger(); // nombre d' items ou on affecte des valeurs
  
  if (nb_component != 1){
    error() << "Seules les donn?es 'SCALARS' sont support?es, lu=" << nb_component;
    return true;
  }


  String cstr(name_str.c_str());
  // Pour le traitement des groupes
  if (cstr.startsWith("GROUP_")){
    is_group = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** GROUP ! name=" << new_name;
    name_str = new_name;
  } else if (cstr.startsWith("INT64_")){
    is_int64 = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** int64 Property ! name=" << new_name;
    name_str = new_name;
  }

  if(is_group){
    created_infos_str() << "<node-group name='" << name_str << "'/>";
    _readNodeGroup(mesh,id_item,IK_Node,vtk_file,name_str,nb_item_kind,nb_item_var);
  }
  else if(is_int64){
    created_infos_str() << "<node-variable-int64 name='" << name_str << "'/>";
    _readNodeVariableInt64(mesh,id_item,vtk_file,name_str,nb_item_kind,nb_item_var);
  }
  else {
    created_infos_str() << "<node-variable-real name='" << name_str << "'/>";
    _readNodeVariableReal(mesh,id_item,vtk_file,name_str,nb_item_kind,nb_item_var);
  }
  return false ;
}



void MeshFileIOService::
_readNodeVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var)
{
  Real NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;
  
  VariableNodeReal * var_node = new VariableNodeReal(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal>(var_node));

  RealArrayView  values = var_node->asArray();




  //initialisation de values a non valeur
  for( Integer i=0; i<nb_item ; i++)
    values[i] = NO_VALUE;
  

  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Real    v = vtk_file.getReal();// valeur affect���������������������������e
    values[id_item[n]] = v;
  }

  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}

void MeshFileIOService::
_readNodeVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var)
{
  Int64 NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;

  VariableNodeInt64 * var_node = new VariableNodeInt64(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeInt64>(var_node));

  Int64ArrayView  values = var_node->asArray();




  //initialisation de values a non valeur
  for( Integer i=0; i<nb_item ; i++)
    values[i] = NO_VALUE;


  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Int64    v = vtk_file.getInt64();// valeur affect���������������������������e
    values[id_item[n]] = v;
  }

  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}

void MeshFileIOService::
_readNodeGroup(IPrimaryMesh* mesh,IntIntMap & id_item, eItemKind ik,MeshFile & vtk_file,const String& var_name,Integer nb_item_kind,Integer nb_item_var){

 
  info() << "Lecture infos groupes pour le groupe: " << var_name; 
  SharedArray<Integer> ids;

  ids.resize(nb_item_kind);

  for( Integer i=0; i<nb_item_var; ++i ){
    Integer v = vtk_file.getInteger();
    Integer w = vtk_file.getInteger();
    if(!check(id_item[v],0,nb_item_kind-1)){
      ids[id_item[v]] = w;
    }
    else
      fatal()<<"Probleme while reading group";
  }
  info() << "Cr?ation groupe: " << var_name << " nb_element=" << ids.size();
  mesh->nodeFamily()->createGroup(var_name,ids);

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool MeshFileIOService::_readCellData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
                                      std::string & item, 
                                      int nb_item_kind,
                                      istringstream& iline, 
                                      std::string& name_str,
                                      OStringStream & created_infos_str)
{
  
  
  
  int nb_component = 0;
  int nb_item_var  = 0;
  int total_nb_group  = 0;
  bool is_group = false;
  bool is_int64 = false;
  bool is_integer = false;
  
  info() << "** ** ** READNAME: name=" << name_str ; // nom de variable
  nb_component = vtk_file.getInteger(); // nombre de valeur pour definir la variable
  nb_item_var  = vtk_file.getInteger(); // nombre d' items ou on affecte des valeurs
  
  if ((nb_component != 1) && (nb_component != 3)){
    error() << "Seules les donn?es 'SCALARS' and 'VECTORS' sont support?es, lu=" << nb_component;
    return true;
  }

  SharedArray<String> name_group;
  String cstr(name_str.c_str());
  // Pour le traitement des groupes
  if (cstr.startsWith("GROUP_")){
    is_group = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** GROUP ! name=" << new_name;
    name_str = new_name;

//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "CHECK: Pour traiter des zones des KrPc ou des puits, le nom du groupe doit avoir a la fin '_N' ou N est le nombre de zones. Ceci peut creer une confusion avec un utilisateur qui veut juste donner un nom avec '_N' a la fin sans avoir a traiter des zones. Cette facon de faire est temporaire en attendant que openflow soit capable de creer des zones."
//#endif
//#endif

    // On recupere la derniere case du nom et on la copie dans le nombre des groupes total_nb_group si c'est un inetegr sinon total_nb_group = 0
    std::string str(1,new_name.back());
    istringstream istr(str);
		int i;
    if (istr >> i) total_nb_group = i;
  }
  else if (cstr.startsWith("INT64_")){
    is_int64 = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** int64 Property ! name=" << new_name;
    name_str = new_name;
  }
  else if (cstr.startsWith("INT32_")){
    is_integer = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** integer Property ! name=" << new_name;
    name_str = new_name;
  }
 
  if(is_group){
    name_group.resize(total_nb_group);
    _readCellGroup(mesh,id_item,IK_Cell,vtk_file,name_str,nb_item_kind,nb_item_var, name_group, total_nb_group);
    for(Integer i = 0; i < total_nb_group; i++)
      created_infos_str() << "<cell-group name='" << name_group[i] << "'/>";
  }
  else if (is_int64){
    created_infos_str() << "<cell-variable-int64 name='" << name_str << "'/>";
    _readCellVariableInt64(mesh,id_item,vtk_file,name_str,nb_item_kind,nb_item_var);
  }
  else if (is_integer){
    created_infos_str() << "<cell-variable-integer name='" << name_str << "'/>";
    _readCellVariableInteger(mesh,id_item,vtk_file,name_str,nb_item_kind,nb_item_var);
  }
  else{
    if(nb_component == 1)
      {
        created_infos_str() << "<cell-variable-real name='" << name_str << "'/>";
        _readCellVariableReal(mesh,id_item,vtk_file,name_str,nb_item_kind,nb_item_var);
      }
    if(nb_component == 3)
      {
        created_infos_str() << "<cell-variable-real3 name='" << name_str << "'/>";
        _readCellVariableReal3(mesh,id_item,vtk_file,name_str,nb_item_kind,nb_item_var);
      }
  }
  
  return false ;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MeshFileIOService::
_readCellVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var)
{
  Real NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;


  VariableCellReal * var_cell = new VariableCellReal(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(var_cell));

  RealArrayView  values = var_cell->asArray();

  //initialisation de values a non valeur
  //  if(nb_item != nb_item_var){
  for( Integer i=0; i<nb_item ; i++){
    values[i] = NO_VALUE;
  }
  //  }

  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Real    v = vtk_file.getReal();// valeur affect���������������������������e
    values[id_item[n]] = v;
  }
  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}

void MeshFileIOService::
_readCellVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var)
{
  Int64 NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;


  VariableCellInt64 * var_cell = new VariableCellInt64(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableCellInt64>(var_cell));

  Int64ArrayView  values = var_cell->asArray();

  //initialisation de values a non valeur
  //  if(nb_item != nb_item_var){
  for( Integer i=0; i<nb_item ; i++){
    values[i] = NO_VALUE;
  }
  //  }

  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Int64    v = vtk_file.getInt64();// valeur affect���������������������������e
    values[id_item[n]] = v;
  }
  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}

void MeshFileIOService::
_readCellVariableInteger(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var)
{
  Integer NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;


  VariableCellInteger * var_cell = new VariableCellInteger(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableCellInteger>(var_cell));

  IntegerArrayView  values = var_cell->asArray();

  //initialisation de values a non valeur
  //  if(nb_item != nb_item_var){
  for( Integer i=0; i<nb_item ; i++){
    values[i] = NO_VALUE;
  }
  //  }

  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Integer    v = vtk_file.getInteger();// valeur affect���������������������������e
    values[id_item[n]] = v;
  }
  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}


void MeshFileIOService::
_readCellVariableReal3(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,Integer nb_item_var)
{
  Real NO_VALUE = 0;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;


  VariableCellReal3 * var_cell = new VariableCellReal3(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal3>(var_cell));

  Real3ArrayView  values = var_cell->asArray();

  //initialisation de values a non valeur
  //  if(nb_item != nb_item_var){
  for( Integer i=0; i<nb_item ; i++){
    values[i] = Real3(NO_VALUE,NO_VALUE,NO_VALUE);
  }
  //  }

  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������es
    Real    v1 = vtk_file.getReal();// valeur affect���������e
    Real    v2 = vtk_file.getReal();// valeur affect���������e
    Real     v3 = vtk_file.getReal();// valeur affect���������e
    values[id_item[n]] = Real3(v1,v2,v3);
  }
  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}

void MeshFileIOService::
_readCellGroup(IPrimaryMesh* mesh,IntIntMap & id_item,
               eItemKind ik, MeshFile & vtk_file,const String& var_name,
               Integer nb_item_kind,Integer nb_item_var,SharedArray<String> name_group, Integer total_nb_group)
{
  info() << "Reading info for the group: " << var_name; 
  
  if(total_nb_group>0)
    {
      SharedArray< SharedArray<Integer> > ids;
      ids.resize(total_nb_group);
      for(Integer i=0; i<nb_item_var; ++i)
        {
          Integer v = vtk_file.getInteger();
          Integer w = vtk_file.getInteger();  
          if(!check(w,0,total_nb_group-1))
            {
              if(!check(id_item[v],0,nb_item_kind-1))
                {
                  ids[w].add(id_item[v]);
                }
              else
                fatal()<<"Problem while reading group";
            } 
          else
            fatal()<<"Problem while reading group";
        }

      for(Integer i = 0; i < total_nb_group; i++)
        {
          std::ostringstream oss;
          oss << i;
          String name = var_name + "_" + oss.str();
          name_group[i] = name;
          info() << "Creation groupe: " << name << " nb_element=" << ids[i].size();
          mesh->cellFamily()->createGroup(name,ids[i]);
        }
    }
  else
    {
      SharedArray<Integer> ids;
      for( Integer i=0; i<nb_item_var; ++i )
        {
          Integer v = vtk_file.getInteger();
          if(!check(id_item[v],0,nb_item_kind-1))
            ids.add(id_item[v]) ;
          else
            fatal()<<"Probleme while reading group";
        }
      info() << "Cr?ation groupe: " << var_name << " nb_element=" << ids.size();
      mesh->cellFamily()->createGroup(var_name,ids);
    }    
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
bool MeshFileIOService::_readFaceData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
		std::string & item,
		int nb_item_kind,
		istringstream& iline,
		SharedArray<Integer>& faces_local_id,
		std::string& name_str,
		OStringStream& created_infos_str)
{



	int nb_component = 0;
	int nb_item_var  = 0;
	int total_nb_group = 0;
	bool is_group = false;
	bool is_int64 = false;
        bool is_integer = false;

	info() << "** ** ** READNAME: name=" << name_str ; // nom de variable
	nb_component = vtk_file.getInteger(); // nombre de valeur pour definir la variable
	nb_item_var  = vtk_file.getInteger(); // nombre d' items ou on affecte des valeurs

	if (nb_component != 1){
		error() << "Seules les donn?es 'SCALARS' sont support?es, lu=" << nb_component;
		return true;
	}


	String cstr(name_str.c_str());
	// Pour le traitement des groupes
	if (cstr.startsWith("GROUP_")){
		is_group = true;
		std::string new_name(name_str.c_str()+6);
		info() << "** ** ** GROUP ! name=" << new_name;
		name_str = new_name;

		// On recupere la derniere case du nom et on la copie dans le nombre des groupes total_nb_group si c'est un inetegr sinon total_nb_group = 0
    std::string str(1,new_name.back());
    istringstream istr(str);
		int i;
		if (istr >> i) total_nb_group = i;
	}
	else if (cstr.startsWith("INT64_")){
		is_int64 = true;
		std::string new_name(name_str.c_str()+6);
		info() << "** ** ** int64 Property ! name=" << new_name;
		name_str = new_name;
	}
        else if (cstr.startsWith("INT32_")){
                is_integer = true;
                std::string new_name(name_str.c_str()+6);
                info() << "** ** ** integer Property ! name=" << new_name;
                name_str = new_name;
        }

	if(is_group){
		created_infos_str() << "<face-group name='" << name_str << "'/>";
		_readFaceGroup(mesh,id_item,IK_Face,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var,total_nb_group);
	}
	else if (is_int64){
		created_infos_str() << "<face-variable-int64 name='" << name_str << "'/>";
		_readFaceVariableInt64(mesh,id_item,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var);
	}
        else if (is_integer){
                created_infos_str() << "<face-variable-integer name='" << name_str << "'/>";
                _readFaceVariableInteger(mesh,id_item,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var);
        }
	else{
		created_infos_str() << "<face-variable-real name='" << name_str << "'/>";
		_readFaceVariableReal(mesh,id_item,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var);
	}

	return false ;
}
#else /* ARCANE_VERSION */
bool MeshFileIOService::_readFaceData(IPrimaryMesh* mesh, IntIntMap & id_item,MeshFile& vtk_file,
                                      std::string & item, 
                                      int nb_item_kind,
                                      istringstream& iline, 
                                      CArrayInteger & faces_local_id,
                                      std::string& name_str,
                                      OStringStream& created_infos_str)
{
  
  
  
  int nb_component = 0;
  int nb_item_var  = 0;
  int total_nb_group = 0;
  bool is_group = false;
  bool is_int64 = false;
  
  info() << "** ** ** READNAME: name=" << name_str ; // nom de variable
  nb_component = vtk_file.getInteger(); // nombre de valeur pour definir la variable
  nb_item_var  = vtk_file.getInteger(); // nombre d' items ou on affecte des valeurs
  
  if (nb_component != 1){
    error() << "Seules les donn?es 'SCALARS' sont support?es, lu=" << nb_component;
    return true;
  }


  String cstr(name_str.c_str());
  // Pour le traitement des groupes
  if (cstr.startsWith("GROUP_")){
    is_group = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** GROUP ! name=" << new_name;
    name_str = new_name;

    // On recupere la derniere acse du nom et on la copie dans le nombre des groupes total_nb_group si c'est un inetegr sinon total_nb_group = 0
    std::string str(1,new_name.back());
    istringstream istr(str);
		int i;
    if (istr >> i) total_nb_group = i;
  }
  else if (cstr.startsWith("INT64_")){
    is_int64 = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** int64 Property ! name=" << new_name;
    name_str = new_name;
  }

  if(is_group){
    created_infos_str() << "<face-group name='" << name_str << "'/>";
    _readFaceGroup(mesh,id_item,IK_Face,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var,total_nb_group);
  }
  else if (is_int64){
    created_infos_str() << "<face-variable-int64 name='" << name_str << "'/>";
    _readFaceVariableInt64(mesh,id_item,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var);
  }
  else{
    created_infos_str() << "<face-variable-real name='" << name_str << "'/>";
    _readFaceVariableReal(mesh,id_item,vtk_file,name_str,nb_item_kind,faces_local_id,nb_item_var);
  }
  
  return false ;
}
#endif /* ARCANE_VERSION */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readFaceVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,SharedArray<Integer>& faces_local_id,Integer nb_item_var)
{
	Real NO_VALUE = -1;
	//TODO Faire la conversion uniqueId() vers localId() correcte
	info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;

	VariableFaceReal * var_face = new VariableFaceReal(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
	mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal>(var_face));

	RealArrayView  values = var_face->asArray();

	//initialisation de values a non valeur

	for( Integer i=0; i<nb_item ; i++)
		values[i] = NO_VALUE;


	for( Integer i=0; i<nb_item_var; i++ ){
		Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
		Real    v = vtk_file.getReal();// valeur affect���������������������������e
		Integer j = faces_local_id[id_item[n]];
		values[j] = v;
	}
	info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readFaceVariableReal(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,CArrayInteger & faces_local_id,Integer nb_item_var)
{
	Real NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;

  VariableFaceReal * var_face = new VariableFaceReal(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceReal>(var_face));

  RealArrayView  values = var_face->asArray();

  //initialisation de values a non valeur

  for( Integer i=0; i<nb_item ; i++)
    values[i] = NO_VALUE;


  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Real    v = vtk_file.getReal();// valeur affect���������������������������e
    Integer j = faces_local_id[id_item[n]];
    values[j] = v;
  }
  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}
#endif /* ARCANE_VERSION */

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readFaceVariableInteger(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,SharedArray<Integer>& faces_local_id,Integer nb_item_var)
{
        Integer NO_VALUE = -1;
        //TODO Faire la conversion uniqueId() vers localId() correcte
        info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;

        VariableFaceInteger * var_face = new VariableFaceInteger(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
        mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceInteger>(var_face));

        IntegerArrayView  values = var_face->asArray();

        //initialisation de values a non valeur

        for( Integer i=0; i<nb_item ; i++)
                values[i] = NO_VALUE;


        for( Integer i=0; i<nb_item_var; i++ ){
                Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
                Integer    v = vtk_file.getInteger();// valeur affect���������������������������e
                Integer j = faces_local_id[id_item[n]];
                values[j] = v;
        }
        info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}void MeshFileIOService::
_readFaceVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,SharedArray<Integer>& faces_local_id,Integer nb_item_var)
{
	Int64 NO_VALUE = -1;
	//TODO Faire la conversion uniqueId() vers localId() correcte
	info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;

	VariableFaceInt64 * var_face = new VariableFaceInt64(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
        mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceInt64>(var_face));

	Int64ArrayView  values = var_face->asArray();

	//initialisation de values a non valeur

	for( Integer i=0; i<nb_item ; i++)
		values[i] = NO_VALUE;


	for( Integer i=0; i<nb_item_var; i++ ){
		Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
		Int64    v = vtk_file.getInt64();// valeur affect���������������������������e
		Integer j = faces_local_id[id_item[n]];
		values[j] = v;
	}
	info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readFaceVariableInt64(IPrimaryMesh* mesh,IntIntMap & id_item,MeshFile& vtk_file,const String& var_name,Integer nb_item,CArrayInteger & faces_local_id,Integer nb_item_var)
{
  Int64 NO_VALUE = -1;
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Lecture valeurs pour la variable: " << var_name << " n=" << nb_item_var;

  VariableFaceInt64 * var_face = new VariableFaceInt64(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore|IVariable::PPersistant));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableFaceInt64>(var_face));

  Int64ArrayView  values = var_face->asArray();

  //initialisation de values a non valeur
  
  for( Integer i=0; i<nb_item ; i++)
    values[i] = NO_VALUE;
  

  for( Integer i=0; i<nb_item_var; i++ ){
    Integer n = vtk_file.getInteger();//numero de l'item ds le fichier de donn���������������������������es
    Int64    v = vtk_file.getInt64();// valeur affect���������e
    Integer j = faces_local_id[id_item[n]];
    values[j] = v;
  }
  info() << "Fin cr?ation de la variable: " << vtk_file.isEnd();

}
#endif /* ARCANE_VERSION */

#if (ARCANE_VERSION >= 12201)
void MeshFileIOService::
_readFaceGroup(IPrimaryMesh* mesh,IntIntMap & id_item,
		eItemKind ik,MeshFile & vtk_file, const String& var_name,
		Integer nb_item_kind, SharedArray<Integer>& faces_local_id,Integer nb_item_var,Integer total_nb_group )
{
	info() << "Reading info for the group: " << var_name;

	if(total_nb_group>0)
	{
		SharedArray< SharedArray<Integer> > ids;
		ids.resize(total_nb_group);
		for(Integer i=0; i<nb_item_var; ++i)
		{
			Integer v = vtk_file.getInteger();
			Integer w = vtk_file.getInteger();
			Integer j = faces_local_id[id_item[v]] ;
			if(!check(w,0,total_nb_group-1))
			{
				if(!check(j,0,nb_item_kind-1))
				{
					ids[w].add(j);
				}
				else
					fatal()<<"Problem while reading group";
			}
			else
				fatal()<<"Problem while reading group";
		}
		for(Integer i = 0; i < total_nb_group; i++)
		{
			std::ostringstream oss;
			oss << i;
			String name = var_name + "_" + oss.str();
			info() << "Creation groupe: " << name << " nb_element=" << ids[i].size();
			mesh->cellFamily()->createGroup(name,ids[i]);
		}
	}
	else
	{
		SharedArray<Integer> ids;
		for( Integer i=0; i<nb_item_var; ++i )
		{
			Integer v = vtk_file.getInteger();
			Integer j = faces_local_id[id_item[v]] ;
			if(!check(j,0,nb_item_kind-1))
				ids.add(j) ;
			else
				fatal()<<"Probleme while reading group";
		}
		info() << "Cr?ation groupe: " << var_name << " nb_element=" << ids.size();
		mesh->faceFamily()->createGroup(var_name,ids);
	}
}
#else /* ARCANE_VERSION */
void MeshFileIOService::
_readFaceGroup(IPrimaryMesh* mesh,IntIntMap & id_item,
               eItemKind ik,MeshFile & vtk_file, const String& var_name,
               Integer nb_item_kind, CArrayInteger & faces_local_id,Integer nb_item_var,Integer total_nb_group )
{
  info() << "Reading info for the group: " << var_name; 
  
  if(total_nb_group>0)
    {
      SharedArray< SharedArray<Integer> > ids;
      ids.resize(total_nb_group);
      for(Integer i=0; i<nb_item_var; ++i)
        {
          Integer v = vtk_file.getInteger();
          Integer w = vtk_file.getInteger();  
          Integer j = faces_local_id[id_item[v]] ; 
          if(!check(w,0,total_nb_group-1))
            {
              if(!check(j,0,nb_item_kind-1))
                {
                  ids[w].add(j);
                }
              else
                fatal()<<"Problem while reading group";
            } 
          else
            fatal()<<"Problem while reading group";
        }
      for(Integer i = 0; i < total_nb_group; i++)
        {
          std::ostringstream oss;
          oss << i;
          String name = var_name + "_" + oss.str();
          info() << "Creation groupe: " << name << " nb_element=" << ids[i].size();
          mesh->cellFamily()->createGroup(name,ids[i]);
        }
    }
  else
    {
      SharedArray<Integer> ids;
      for( Integer i=0; i<nb_item_var; ++i )
        {
          Integer v = vtk_file.getInteger();
          Integer j = faces_local_id[id_item[v]] ; 
          if(!check(j,0,nb_item_kind-1))
            ids.add(j) ;
          else
            fatal()<<"Probleme while reading group";
        }
      info() << "Cr?ation groupe: " << var_name << " nb_element=" << ids.size();
      mesh->faceFamily()->createGroup(var_name,ids);
    }    
}
#endif /* ARCANE_VERSION */

bool MeshFileIOService::
_readCondLimStructuredGrid(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition)
{
  ifstream ifile(file_name.localstr());
  if (!ifile){
    error() << "Impossible d'ouvrir en lecture le fichier '" << file_name << "'";
    return false;
  }
  MeshFile vtk_file(&ifile);
  const char* buf = 0;
  // Lecture de la description
  buf = vtk_file.getNextLine();
  String format = vtk_file.getNextLine();
  if (not  MeshFile::isEqualString(format,"ASCII")){
    error() << "Seul le format 'ASCII' est reconnu (format='" << format << "')";
    return true;
  }
  eMeshType mesh_type = VTK_MT_Unknown;
  // Lecture du type de maillage
  // TODO: en parall?le, avec use_internal_partition vrai, seul le processeur 0
  // lit les donn?es. Dans ce cas, inutile que les autres ouvre le fichier.
  {
    buf = vtk_file.getNextLine();
    istringstream mesh_type_line(buf);
    std::string dataset_str;
    std::string mesh_type_str;
    mesh_type_line >> ws >> dataset_str >> ws >> mesh_type_str;
    vtk_file.checkString(dataset_str,"DATASET");
    if ( MeshFile::isEqualString(mesh_type_str,"STRUCTURED_GRID")){
      mesh_type = VTK_MT_StructuredGrid;
    }
    if (mesh_type==VTK_MT_Unknown){
      error() << "Seul les formats 'STRUCTURED_GRID' et 'UNSTRUCTURED_GRID' sont implementes (format=" << mesh_type_str << "')";
      return true;
    }
  }
 
  ////////////////////////////////////////////////////////////
  // Lecture des param���������tres obligatoires

  // Lecture du nombre de points: DIMENSIONS nx ny nz
  Integer nb_node_x = 0;
  Integer nb_node_y = 0;
  Integer nb_node_z = 0;
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string dimension_str;
    iline >> ws >> dimension_str >> ws >> nb_node_x
          >> ws >> nb_node_y >> ws >> nb_node_z;
    if (!iline){
      error() << "Erreur de syntaxe lors de la lecture des dimensions de la grille";
      return true;
    }
    vtk_file.checkString(dimension_str,"DIMENSIONS");
    if (nb_node_x<0 || nb_node_y<0 || nb_node_z<0 || nb_node_x+nb_node_y+nb_node_z<1){
      error() << "Dimensions invalides: x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
      return true;
    }
  }
  
  info() << "Nombre de noeuds (x, y, z) = (" 
         << nb_node_x << ", "
         << nb_node_y << ", " 
         << nb_node_z << ")";

  SharedArray<Real> dzf ;
  std::string top_file_str("Undefined") ;
  std::string dz_file_str("Undefined") ;
 

  Integer sub_domain_id = subDomain()->subDomainId();

  Integer nb_cell_x = nb_node_x-1;
  Integer nb_cell_y = nb_node_y-1;
  Integer nb_cell_z = nb_node_z-1;

  if (use_internal_partition && sub_domain_id!=0){
    nb_node_x = 0;
    nb_node_y = 0;
    nb_node_z = 0;
    nb_cell_x = 0;
    nb_cell_y = 0;
    nb_cell_z = 0;
  }

  

  bool ok = true ;
  do {
    buf = vtk_file.getNextLine( MeshFile::OPTIONAL_LINE);
    if( buf == NULL ) break;
    istringstream iline(buf);

    // Lis le mot cl���������
    std::string keyword;
    iline >> ws >> keyword;

    if(  MeshFile::isEqualString(keyword,"CELLGROUPS") ) {
      Integer nb_groups = 0 ;
      iline >> ws >> nb_groups;
      if (!iline){
        error() << "Erreur de syntaxe lors de la lecture des groupes de cellules";
        return true;
      }
      for(Integer igroup = 0; igroup < nb_groups; igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_cell_x, nb_cell_y, nb_cell_z, 
                         0,nb_cell_x,0,nb_cell_y,IK_Cell,
                         use_internal_partition);
    } else if(  MeshFile::isEqualString(keyword,"NODEGROUPS") ) {
      Integer nb_groups = 0 ;
      if (!iline){
        error() << "Erreur de syntaxe lors de la lecture des groupes de noeuds";
        return true;
      }
      for(Integer igroup=0;igroup<nb_groups;igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_node_x, nb_node_y, nb_node_z, 
                         0,nb_node_x,0,nb_node_y,IK_Node,
                         use_internal_partition);
    } else {
      error() << "Mot cl��������� [" << keyword << "] inconnu";
      return true;
    }
  } while( ok );

  

  
  return false ;
}




void  MeshFileIOService::
_createItemGroup(IMesh* mesh,
                 MeshFile& vtk_file,
                 const char* buf,
                 Integer nb_item_x,
                 Integer nb_item_y,
                 Integer nb_item_z,
                 Integer first_x,
                 Integer local_nb_item_x,
                 Integer first_y,
                 Integer local_nb_item_y,
                 eItemKind ik,
                 bool use_internal_partition)
{
  Integer i1,i2,j1,j2,k1,k2;
  std::string group_name ;
  buf = vtk_file.getNextLine() ;
  istringstream iline(buf) ;
  iline >> ws >> group_name 
        >> ws >> i1 >> ws >> i2
        >> ws >> j1 >> ws >> j2
        >> ws >> k1 >> ws >> k2 ;
  SharedArray<Int64> uids;
  Integer nb_item = 0 ;
  if((subDomain()->subDomainId()==0)||(!use_internal_partition))
    {
      uids.reserve((i2+1-i1)*(j2+1-j1)*(k2+1-k1)) ;
      for(Integer k=k1-1;k<k2;k++)
        for(Integer j=j1-1;j<j2;j++)
          if((j>=first_y)&&(j<first_y+local_nb_item_y))
            for(Integer i=i1-1;i<i2;i++)
              if((i>=first_x)&&(i<first_x+local_nb_item_x))
                {
                  Int64 uid = k*nb_item_x*nb_item_y+j*nb_item_x+i ;
                  uids.add(uid) ;
                  nb_item++ ;
                }
    }
  IItemFamily* item_family = mesh->itemFamily(ik);
  SharedArray<Integer> ids(nb_item) ;
  item_family->itemsUniqueIdToLocalId(ids,uids) ;
  info() << "Creation groupe: " << group_name << " nb_element=" << ids.size();
  item_family->createGroup(group_name,ids);
}




ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
