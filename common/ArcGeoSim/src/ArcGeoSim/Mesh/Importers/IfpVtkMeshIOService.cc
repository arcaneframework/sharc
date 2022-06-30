// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* IfpVtkMeshIOService.cc                                      (C) 2000-2017 */
/*                                                                           */
/* Lecture/Ecriture d'un maillage au format Vtk historique (legacy).         */
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
#ifndef USE_ARCANE_V3
#include <arcane/IGraph.h>
#include <arcane/IGraphModifier.h>
#endif
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

#include "arcane/utils/PlatformUtils.h"
#include "arcane/utils/Convert.h"
#include "arcane/utils/NotImplementedException.h"

#include <arcane/utils/UserDataList.h>
#include <arcane/utils/IUserData.h>
#include <arcane/utils/AutoDestroyUserData.h>

#include "ArcGeoSim/Utils/Utils.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IfpVtkFile;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Lecteur des fichiers de maillage au format Vtk historique (legacy).
 *
 * Il s'agit d'une version preliminaire qui ne supporte que les
 * DATASET de type STRUCTURED_GRID ou UNSTRUCTURED_GRID. De plus,
 * le lecteur et l'ecrivain n'ont ete que partiellement testes.
 *
 * L'en-tete du fichier vtk doit etre:
 * # vtk DataFile Version 2.0
 *
 * Il est possible de specifier un ensemble de variables dans le fichier.
 * Dans ce cas, leurs valeurs sont lues en meme temps que le maillage
 * et servent a initialiser les variables. Actuellement, seules les valeurs
 * aux mailles sont supportees
 *
 * Comme Vtk ne supporte pas la notion de groupe, il est possible
 * de specifier un groupe comme etant une variable (CELL_DATA).
 * Par convention, si la variable commence par la chaine 'GROUP_', alors
 * il s'agit d'un groupe. La variable doit etre declaree comme suit:
 * \begincode
 * CELL_DATA %n
 * SCALARS GROUP_%m int 1
 * LOOKUP_TABLE default
 * \endcode
 * avec %n le nombre de mailles, et %m le nom du groupe.
 * Une maille appartient au groupe si la valeur de la donnee est
 * differente de 0.
 *
 * Actuellement, on NE peut PAS specifier de groupes de points.
 *
 * Pour specifier des groupes de faces, il faut un fichier vtk
 * additionnel, identique au fichier d'origine mais contenant la
 * description des faces au lieu des mailles. Par convention, si le
 * fichier courant lu s'appelle 'toto.vtk', le fichier decrivant les
 * faces sera 'toto.vtkfaces.vtk'. Ce fichier est optionnel.
 */
class IfpVtkMeshIOService
  : public BasicService
  , public IMeshReader
{
public:

  IfpVtkMeshIOService(const ServiceBuildInfo& sbi);

public:

  virtual void build() {}
  ~IfpVtkMeshIOService() {}

public:

  enum eMeshType
    {
      VTK_MT_Unknown,
      VTK_MT_StructuredGrid,
      VTK_MT_DistStructuredGrid,
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
    return str=="vt2";
  }

public:

  virtual eReturnType readMeshFromFile(IPrimaryMesh* mesh,const XmlNode& mesh_node,const String& file_name,
                                       const String& dir_name,bool use_internal_partition);

  virtual bool hasCutInfos() const { return false; }

private:

  bool _readMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition);
  bool _readStructuredGrid(IPrimaryMesh* mesh,IfpVtkFile&,bool use_internal_partition);
  bool _readStructured3D(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition,
                         Integer nb_node_x, Integer nb_node_y, Integer nb_node_z,
                         Array<Integer>& cells_local_id, Array<Integer>& nodes_local_id) ;
  bool _readStructured2D(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition,
                         Integer nb_node_x, Integer nb_node_y,
                         Array<Integer>& cells_local_id, Array<Integer>& nodes_local_id) ;

  bool _readDistStructuredGrid(IPrimaryMesh* mesh,IfpVtkFile&,bool use_internal_partition);
  bool _readStructuredGraph(IPrimaryMesh* mesh,IfpVtkFile&,bool use_internal_partition);
  bool _readStructuredDualGrid(IPrimaryMesh* mesh,IfpVtkFile&,bool use_internal_partition);
  bool _readUnstructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition, const Integer mesh_dimension);
  void _readNodeVariable(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& name_str,Integer nb_node);
  void _readNodeVectorVariable(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& name_str,Integer nb_node);
  void _readCellVariable(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& name_str,Integer nb_cell);
  void _readCellVectorVariable(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& name_str,Integer nb_cell);
  void _readItemGroup(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& name_str,Integer nb_item,
                      eItemKind ik,ConstArrayView<Integer> local_ids);
  void _createFaceGroup(IMesh* mesh,const String& name,ConstArrayView<Integer> faces_lid);
  void _createNodeGroup(IMesh* mesh,const String& name,ConstArrayView<Integer> faces_lid);
  bool _readData(   IPrimaryMesh* mesh,
                    IfpVtkFile& vtk_file,
                    bool use_internal_partition,
                    Array<Integer>& node_local_ids,
                    Array<Integer>& cell_local_ids,
                    const char * buf = NULL);
  bool _readNodeData(   IPrimaryMesh* mesh,
                        IfpVtkFile& vtk_file,
                        int nb_node_kind,
                        Array<Integer>& local_ids,
                        const char* buf,
                        istringstream& iline,
                        std::string& data_str,
                        OStringStream& created_infos_str) ;
  bool _readCellData(   IPrimaryMesh* mesh,
                        IfpVtkFile& vtk_file,
                        int nb_cell_kind,
                        Array<Integer>& local_ids,
                        const char* buf,
                        istringstream& iline,
                        std::string& data_str,
                        OStringStream& created_infos_str);
  bool _readFaceData(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition,eItemKind cell_kind,
                     ConstArrayView<Integer> local_ids);
  bool _createGraphLinkType(    IPrimaryMesh* mesh,
                                bool use_internal_partition,
                                Array<Integer>& cell_local_ids,
                                Array<Integer>& link_type) ;
  void _writeMeshToFile(IPrimaryMesh* mesh,const String& file_name,eItemKind cell_kind);
  void _readNodesUnstructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,Array<Real3>& node_coords);
  void _readCellsUnstructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,
                                  Array<Integer>& cells_nb_node,
                                  Array<Integer>& cells_type,
                                  Array<Int64>& cells_connectivity);
  void _readFacesMesh(IPrimaryMesh* mesh,const String& file_name,
                      const String& dir_name,bool use_internal_partition);
  void _readStructuredFacesMesh(IPrimaryMesh* mesh,const String& file_name,
                                const String& dir_name,bool use_internal_partition);

  void _readDistStructuredFacesMesh(IPrimaryMesh* mesh,const String& file_name,
                                const String& dir_name,bool use_internal_partition);
  bool check(Integer i, Integer j, Integer k,Integer nx, Integer ny, Integer nz) ;
  void _createItemGroup(IMesh* mesh,
                        IfpVtkFile& vtk_file,
                        const char* buf,
                        Integer nb_item_x,
                        Integer nb_item_y,
                        Integer nb_item_z,
                        Integer first_x,
                        Integer local_nb_item_x,
                        Integer first_y,
                        Integer local_nb_item_y,
                        eItemKind ik,
                        bool use_internal_partition) ;
  void _computePartition(Integer nproc,Integer* np1, Integer* np2) ;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IfpVtkFile
{
public:
  static const int BUFSIZE = 10000;
public:
  enum LINE_TYPE { OPTIONAL_LINE, MANDATORY_LINE };
public:
  IfpVtkFile(istream* stream) : m_stream(stream) {}
  bool isThereAnotherLine();
  const char* getNextLine(LINE_TYPE line_type);
  Real getReal();
  Integer getInteger();
  void checkString(const String& current_value,const String& expected_value);
  void checkString(const String& current_value,
                   const String& expected_value1,
                   const String& expected_value2);
  void checkString(const String& current_value,
                   const String& expected_value1,
                   const String& expected_value2,
                   const String& expected_value3,
                   const String& expected_value4,
                   const String& expected_value5,
                   const String& expected_value6);
  static bool isEqualString(const String& current_value,const String& expected_value);

  bool isEnd(){ (*m_stream) >> ws; return m_stream->eof(); }
private:
  istream* m_stream;
  char m_buf[BUFSIZE];
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

const char* IfpVtkFile::
getNextLine(LINE_TYPE line_type = MANDATORY_LINE)
{
  while (m_stream->good()){
    m_stream->getline(m_buf,sizeof(m_buf)-1);
    if (m_stream->eof())
      break;
    bool is_comment = true;
    if (m_buf[0]=='\n' || m_buf[0]=='\r')
      continue;
    // Regarde si un caractere de commentaire est present
    for( int i=0; i<BUFSIZE && m_buf[i]!='\0'; ++i ){
      if (!isspace(m_buf[i])){
        is_comment = (m_buf[i]=='#');
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
    throw IOException("IfpVtkFile::getNexLine()","Unexpected EndOfFile");
  return NULL;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Real IfpVtkFile::
getReal()
{
  Real v = 0.;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("IfpVtkFile::getReal()","Bad Real");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Integer IfpVtkFile::
getInteger()
{
  Integer v = 0;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("IfpVtkFile::getInteger()","Bad Integer");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkFile::
checkString(const String& current_value,const String& expected_value)
{
  StringBuilder current_value_low(current_value);
  current_value_low.toLower();
  StringBuilder expected_value_low(expected_value);
  expected_value_low.toLower();

  if (current_value_low.toString()!=expected_value_low.toString()){
    String s = "Wait for string '" + expected_value + "', found '" + current_value + "'";
    throw IOException("IfpVtkFile::checkString()",s);
  }
}

void IfpVtkFile::
checkString(const String& current_value,const String& expected_value1,const String& expected_value2)
{
  StringBuilder current_value_low(current_value);
  current_value_low.toLower();
  StringBuilder expected_value1_low(expected_value1);
  expected_value1_low.toLower();
  StringBuilder expected_value2_low(expected_value2);
  expected_value2_low.toLower();

  if (current_value_low.toString()!=expected_value1_low.toString()
      and current_value_low.toString()!=expected_value2_low.toString()){
    String s = "Wait for string '" + expected_value1 + "' or '"
      + expected_value2 + "', found '" + current_value + "'";
    throw IOException("IfpVtkFile::checkString()",s);
  }
}

void IfpVtkFile::
checkString(const String& current_value,const String& expected_value1,
            const String& expected_value2,const String& expected_value3,
            const String& expected_value4,const String& expected_value5, const String& expected_value6)
{
  StringBuilder current_value_low(current_value);
  current_value_low.toLower();
  StringBuilder expected_value1_low(expected_value1);
  expected_value1_low.toLower();
  StringBuilder expected_value2_low(expected_value2);
  expected_value2_low.toLower();
  StringBuilder expected_value3_low(expected_value3);
  expected_value3_low.toLower();
  StringBuilder expected_value4_low(expected_value4);
  expected_value4_low.toLower();
  StringBuilder expected_value5_low(expected_value5);
  expected_value5_low.toLower();
  StringBuilder expected_value6_low(expected_value6);
  expected_value6_low.toLower();

  if (current_value_low.toString()!=expected_value1_low.toString()
      and current_value_low.toString()!=expected_value2_low.toString()
      and current_value_low.toString()!=expected_value3_low.toString()
      and current_value_low.toString()!=expected_value4_low.toString()
      and current_value_low.toString()!=expected_value5_low.toString()
      and current_value_low.toString()!=expected_value6_low.toString()){
    String s = "Wait for string '" + expected_value1 + "' or '"
      + expected_value2 + "' or '" + expected_value3 + "' or '"
      + expected_value4 + "' or'" + expected_value5 + "' or '"
      + expected_value6  
      + "', found '" + current_value + "'";
    throw IOException("IfpVtkFile::checkString()",s);
  }
}

bool IfpVtkFile::
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

ARCANE_REGISTER_SUB_DOMAIN_FACTORY(IfpVtkMeshIOService,IMeshReader,IfpVtkMeshIO);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
IfpVtkMeshIOService::
IfpVtkMeshIOService(const ServiceBuildInfo& sbi)
  : BasicService(sbi)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \todo Verifier plantage sous linux.
 */
IMeshReader::eReturnType IfpVtkMeshIOService::
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

bool IfpVtkMeshIOService::
_readMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition)
{
  ifstream ifile(file_name.localstr());
  if (!ifile){
    error() << "It is not possible to open file for reading '" << file_name << "'";
    return true;
  }
  IfpVtkFile vtk_file(&ifile);
  const char* buf = 0;
  // Lecture de la description
  String description = vtk_file.getNextLine();
  // Teste si le maillage est 2D en cherchant le mot cle 2d dans la description vtk. Tout les procs lisent, ne necessite
  // pas de synchronisation mais utilise la variable de classe supplementaire mesh_dim. De plus le keyword doit
  // etre ajoute dans les maillage non strucures vtk
  Integer mesh_dimension(3);
  if (description.lower().contains("2d") || description.lower().contains("2 d")) mesh_dimension = 2;
  String format = vtk_file.getNextLine();
  if (not IfpVtkFile::isEqualString(format,"ASCII")){
    error() << "Only the 'ASCII' format is recognized (format='" << format << "')";
    return true;
  }
  eMeshType mesh_type = VTK_MT_Unknown;
  // Lecture du type de maillage
  // TODO: en parallele, avec use_internal_partition vrai, seul le processeur 0
  // lit les donnees. Dans ce cas, inutile que les autres ouvre le fichier.
  {
    buf = vtk_file.getNextLine();
    istringstream mesh_type_line(buf);
    std::string dataset_str;
    std::string mesh_type_str;
    mesh_type_line >> ws >> dataset_str >> ws >> mesh_type_str;
    vtk_file.checkString(dataset_str,"DATASET");
    if (IfpVtkFile::isEqualString(mesh_type_str,"STRUCTURED_GRID")){
      mesh_type = VTK_MT_StructuredGrid;
    }
    if (IfpVtkFile::isEqualString(mesh_type_str,"DIST_STRUCTURED_GRID")){
      mesh_type = VTK_MT_DistStructuredGrid;
    }
    if (IfpVtkFile::isEqualString(mesh_type_str,"STRUCTURED_GRAPH")){
      mesh_type = VTK_MT_StructuredGraph;
    }
    if (IfpVtkFile::isEqualString(mesh_type_str,"STRUCTURED_DUALGRID")){
      mesh_type = VTK_MT_StructuredDualGrid;
    }
    if (IfpVtkFile::isEqualString(mesh_type_str,"UNSTRUCTURED_GRID")){
      mesh_type = VTK_MT_UnstructuredGrid;
    }
    if (mesh_type==VTK_MT_Unknown){
      error() << "Only the formats 'STRUCTURED_GRID' and 'UNSTRUCTURED_GRID' are implemented (format=" << mesh_type_str << "')";
      return true;
    }
  }
  bool ret = true;
  switch(mesh_type){
  case VTK_MT_StructuredGrid:
    ret = _readStructuredGrid(mesh,vtk_file,use_internal_partition);
    if (!ret){
      // Tente de lire le fichier des faces s'il existe
      _readStructuredFacesMesh(mesh,file_name+"faces.vt2",dir_name,use_internal_partition);
    }
    break;
  case VTK_MT_DistStructuredGrid:
    ret = _readDistStructuredGrid(mesh,vtk_file,use_internal_partition);
    if (!ret){
      // Tente de lire le fichier des faces s'il existe
      _readDistStructuredFacesMesh(mesh,file_name+"faces.vt2",dir_name,use_internal_partition);
    }
    break;
  case VTK_MT_StructuredGraph:
    ret = _readStructuredGraph(mesh,vtk_file,use_internal_partition);
    break;
  case VTK_MT_StructuredDualGrid:
    ret = _readStructuredDualGrid(mesh,vtk_file,use_internal_partition);
    break;
  case VTK_MT_UnstructuredGrid:
    ret = _readUnstructuredGrid(mesh,vtk_file,use_internal_partition,mesh_dimension);
    if (!ret){
      // Tente de lire le fichier des faces s'il existe
      _readFacesMesh(mesh,file_name+"faces.vtk2",dir_name,use_internal_partition);
    }
    break;
  case VTK_MT_Unknown:
    break;
  }
  /*while ( (buf=vtk_file.getNextLine()) != 0 ){
    info() << " STR " << buf;
    }*/
  return ret;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool IfpVtkMeshIOService::
_readStructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition)
{
  // IParallelMng* pm = subDomain()->parallelMng();

  ////////////////////////////////////////////////////////////
  // Lecture des parametres obligatoires

  // Lecture du nombre de points: DIMENSIONS nx ny nz
  const char* buf = 0;
  Integer nb_node_x = 0;
  Integer nb_node_y = 0;
  Integer nb_node_z = 0;
  SharedArray<Integer> cells_local_id;
  SharedArray<Integer> nodes_local_id;
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string dimension_str;
    iline >> ws >> dimension_str >> ws >> nb_node_x
          >> ws >> nb_node_y >> ws >> nb_node_z;
    if (!iline){
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(dimension_str,"DIMENSIONS");

    bool flag ;
    if(nb_node_z==0)
       flag = _readStructured2D(mesh,vtk_file,use_internal_partition,
                                nb_node_x,nb_node_y,
                                cells_local_id,nodes_local_id) ;
    else
       flag = _readStructured3D(mesh,vtk_file,use_internal_partition,
                                nb_node_x,nb_node_y,nb_node_z,
                                cells_local_id,nodes_local_id) ;
    if(flag)
      return true ;
  }
  // Cree les groupes de faces des cetes du parallelepipede
  SharedArray<Integer> xmin_surface_lid;
  SharedArray<Integer> xmax_surface_lid;
  SharedArray<Integer> ymin_surface_lid;
  SharedArray<Integer> ymax_surface_lid;
  SharedArray<Integer> zmin_surface_lid;
  SharedArray<Integer> zmax_surface_lid;

  Integer sub_domain_id = subDomain()->subDomainId();

  Integer nb_cell_x = nb_node_x-1;
  Integer nb_cell_y = nb_node_y-1;
  Integer nb_cell_z = 0 ;
  if(nb_node_z>0) nb_cell_z = nb_node_z-1;

  if (use_internal_partition && sub_domain_id!=0){
    nb_node_x = 0;
    nb_node_y = 0;
    nb_node_z = 0;
    nb_cell_x = 0;
    nb_cell_y = 0;
    nb_cell_z = 0;
  }

  Integer nb_node_xy = nb_node_x*nb_node_y;

  Integer nb_cell = nb_cell_x * nb_cell_y ;
  if(nb_cell_z>0)
    nb_cell *= nb_cell_z ;

  ENUMERATE_FACE(iface,mesh->allFaces()){
    const Face& face = *iface;
    Integer face_local_id = face.localId();
    bool is_xmin = true;
    bool is_xmax = true;
    bool is_ymin = true;
    bool is_ymax = true;
    bool is_zmin = true;
    bool is_zmax = true;
    for( NodeEnumerator inode(face.nodes()); inode(); ++inode ){
      const Node& node = *inode;
      Int64 node_unique_id = node.uniqueId();
      Int64 node_z = node_unique_id / nb_node_xy;
      Int64 node_y = (node_unique_id - node_z*nb_node_xy) / nb_node_x;
      Int64 node_x = node_unique_id - node_z*nb_node_xy - node_y*nb_node_x;
      if (node_x!=0)
        is_xmin = false;
      if (node_x!=(nb_node_x-1))
        is_xmax = false;
      if (node_y!=0)
        is_ymin = false;
      if (node_y!=(nb_node_y-1))
        is_ymax = false;
      if (node_z!=0)
        is_zmin = false;
      if (node_z!=(nb_node_z-1))
        is_zmax = false;
    }
    if (is_xmin)
      xmin_surface_lid.add(face_local_id);
    if (is_xmax)
      xmax_surface_lid.add(face_local_id);
    if (is_ymin)
      ymin_surface_lid.add(face_local_id);
    if (is_ymax)
      ymax_surface_lid.add(face_local_id);
    if (is_zmin)
      zmin_surface_lid.add(face_local_id);
    if (is_zmax)
      zmax_surface_lid.add(face_local_id);

  }
  _createFaceGroup(mesh,"XMIN",xmin_surface_lid);
  _createFaceGroup(mesh,"XMAX",xmax_surface_lid);
  _createFaceGroup(mesh,"YMIN",ymin_surface_lid);
  _createFaceGroup(mesh,"YMAX",ymax_surface_lid);
  _createFaceGroup(mesh,"ZMIN",zmin_surface_lid);
  _createFaceGroup(mesh,"ZMAX",zmax_surface_lid);

  ////////////////////////////////////////////////////////////
  // Lecture des parametres optionnels

  info() << "Reading optional parameters";

  std::string tag_zmin = "DEFAULT";
  std::string tag_zmax = "DEFAULT";
  std::string tag_ymin = "DEFAULT";
  std::string tag_ymax = "DEFAULT";
  std::string tag_xmin = "DEFAULT";
  std::string tag_xmax = "DEFAULT";

  bool ok = true ;
  do {
    buf = vtk_file.getNextLine(IfpVtkFile::OPTIONAL_LINE);
    if( buf == NULL ) break;
    istringstream iline(buf);

    // Lis le mot cle
    std::string keyword;
    iline >> ws >> keyword;

    if( IfpVtkFile::isEqualString(keyword,"TAGS") ) {
      // Lecture des etiquettes associees aux faces du parallepipede
      // pour le conditions au bord dans l'ordre suivant:
      // z = zmin, z = zmax, y = ymin, y = ymax, x = xmin, x = xmax

      iline >> ws >> tag_zmin >> ws >> tag_zmax
            >> ws >> tag_ymin >> ws >> tag_ymax
            >> ws >> tag_xmin >> ws >> tag_xmax;
      if(!iline) {
        error() << "Syntax error in reading tags associated to boundary faces";
        return true;
      }
      info() << "Tags associated to boundary faces:";
      info() << "(z = zmin, z = zmax, y = ymin, y = ymax, x = xmin, x = xmax) = ("
             << tag_zmin << ", " << tag_zmax << ", "
             << tag_ymin << ", " << tag_ymax << ", "
             << tag_xmin << ", " << tag_xmax << ")";
    } else if( IfpVtkFile::isEqualString(keyword,"CELLGROUPS") ) {
      Integer nb_groups = 0 ;
      iline >> ws >> nb_groups;
      if (!iline){
        error() << "Syntax error in reading cell groups";
        return true;
      }
      for(Integer igroup = 0; igroup < nb_groups; igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_cell_x, nb_cell_y, nb_cell_z,
                         0,nb_cell_x,0,nb_cell_y,IK_Cell,
                         use_internal_partition);
    } else if( IfpVtkFile::isEqualString(keyword,"NODEGROUPS") ) {
      Integer nb_groups = 0 ;
      iline >> ws >> nb_groups;
      if (!iline){
        error() << "Syntax error in reading node groups";
        return true;
      }
      for(Integer igroup=0;igroup<nb_groups;igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_node_x, nb_node_y, nb_node_z,
                         0,nb_node_x,0,nb_node_y,IK_Node,
                         use_internal_partition);
    } else if( IfpVtkFile::isEqualString(keyword,"FACEGROUPS") ) {
      Integer nb_groups = 0 ;
      iline >> ws >> nb_groups;
      if (!iline){
        error() << "Syntax error in reading face groups";
        return true;
      }
      for(Integer igroup=0;igroup<nb_groups;igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_cell_x, nb_cell_y, nb_cell_z,
                         0,nb_cell_x,0,nb_cell_y,IK_Face,
                         use_internal_partition);
    } else if( (IfpVtkFile::isEqualString(keyword,"CELL_DATA")  ) ||
               (IfpVtkFile::isEqualString(keyword,"POINT_DATA") )   ) {
      // Maintenant, regarde s'il existe des donnees associees cell aux fichiers
        ok = _readData( mesh,
                            vtk_file,
                            use_internal_partition,
                            nodes_local_id,
                            cells_local_id,
                            buf );
        if(ok) return true ;
    } else {
      error() << "Keyword [" << keyword << "] unknown";
      return true;
    }
  } while( ok );

  // Create face groups for boundary conditions
  // The prefix "BC_" is added to the boundary condition name by default
  // in order to avoid group name collisions
  info() << "Build boundary conditions faces groups";

  if( mesh->findGroup( "BC_" + tag_zmin ).null() )
    _createFaceGroup(mesh, "BC_" + tag_zmin, zmin_surface_lid);
  else
    error() << "FATAL: Group with reserved name BC_" + tag_zmin
            << " already exists";

  if( mesh->findGroup( "BC_" + tag_zmax ).null() )
    _createFaceGroup(mesh, "BC_" + tag_zmax, zmax_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_zmax ).addItems( zmax_surface_lid );

  if( mesh->findGroup( "BC_" + tag_ymin ).null() )
    _createFaceGroup(mesh, "BC_" + tag_ymin, ymin_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_ymin ).addItems( ymin_surface_lid );

  if( mesh->findGroup( "BC_" + tag_ymax ).null() )
    _createFaceGroup(mesh, "BC_" + tag_ymax, ymax_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_ymax ).addItems( ymax_surface_lid );

  if( mesh->findGroup( "BC_" + tag_xmin ).null() )
    _createFaceGroup(mesh, "BC_" + tag_xmin, xmin_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_xmin ).addItems( xmin_surface_lid );

  if( mesh->findGroup( "BC_" + tag_xmax ).null() )
    _createFaceGroup(mesh, "BC_" + tag_xmax, xmax_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_xmax ).addItems( xmax_surface_lid );

  return false ;
}


bool IfpVtkMeshIOService::
_readStructured3D(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition,
                  Integer nb_node_x, Integer nb_node_y, Integer nb_node_z,
                  Array<Integer>& cells_local_id, Array<Integer>& nodes_local_id)
{

  if (nb_node_x<0 || nb_node_y<0 || nb_node_z<0 || nb_node_x+nb_node_y+nb_node_z<1){
    error() << "Invalid Dimensions : x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
    return true;
  }

  info() << "Number of nodes (x, y, z) = ("
         << nb_node_x << ", "
         << nb_node_y << ", "
         << nb_node_z << ")";

  Integer nb_node = nb_node_x * nb_node_y * nb_node_z;

  const char* buf ;
  Real Ox = 0. ;
  Real Oy = 0. ;
  Real Oz = 0. ;
  Real dx = 0. ;
  Real dy = 0. ;
  Real dz = 0. ;
  SharedArray<Real> dxf ;
  SharedArray<Real> dyf ;
  SharedArray<Real> dzf ;
  bool top_def = false ;
  bool dx_def = false ;
  bool dy_def = false ;
  bool dz_def = false ;
  bool top_file_read = false ;
  bool dx_file_read = false ;
  bool dy_file_read = false ;
  bool dz_file_read = false ;
  bool layer_top = false ;
  std::string top_file_str("Undefined") ;
  std::string dx_file_str("Undefined") ;
  std::string dy_file_str("Undefined") ;
  std::string dz_file_str("Undefined") ;
  // Lecture du nombre de points: POINTS nb float
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string origin_str;
    iline >> ws >> origin_str ;
    if(IfpVtkFile::isEqualString(origin_str,"ORIGIN"))
    {
      iline >> ws >> Ox >> ws >> Oy >> ws >> Oz;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
    }
    else
    {
      top_def = true ;
      if(IfpVtkFile::isEqualString(origin_str,"TOP"))
        {
          iline >> ws >> top_file_str ;
          if (!iline){
            error() << "Syntax error in reading top surface data";
            return true;
          }
        }
    }
    vtk_file.checkString(origin_str,"ORIGIN","TOP");
  }
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string step_str;
    iline >> ws >> step_str ;
    if(IfpVtkFile::isEqualString(step_str,"DXYZ"))
    {
      iline >> ws >> dx >> ws >> dy >> ws >> dz;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
      layer_top = (dz<0) ;
    }
    else if(IfpVtkFile::isEqualString(step_str,"STEP"))
    {
      warning() << "STEP keyword is deprecated; use DXYZ instead.";
      iline >> ws >> dx >> ws >> dy >> ws >> dz;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
      layer_top = (dz<0) ;
    }
    else if(IfpVtkFile::isEqualString(step_str,"DXY"))
      {
        iline >> ws >> dx >> ws >> dy  ;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepz_str;
        iline2 >> ws >> stepz_str ;
        if(IfpVtkFile::isEqualString(stepz_str,"DZ"))
          {
            iline2 >> ws >> dz;
            if (!iline2){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dz<0) ;
          }
        else
          {
            if(IfpVtkFile::isEqualString(stepz_str,"DZFILE"))
              {
                dz_def = true ;
                iline2 >> ws >> dz_file_str;
                if (!iline2){
                  error() << "Syntax error in reading grid dimensions";
                  return true;
                }
                dzf.resize(nb_node_z) ;
                dz_file_read = readBufferFromFile(dz_file_str,dzf,nb_node_z);
                if (!dz_file_read) {
                  error() << "Error in reading file DZFILE : " << dz_file_str;
                  return true;
                }
                layer_top = (dzf[0]<0) ;
              }
          }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DYZ"))
      {
        iline >> ws >> dy >> ws >> dz  ;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepx_str;
        iline2 >> ws >> stepx_str ;
        if(IfpVtkFile::isEqualString(stepx_str,"DX"))
          {
            iline2 >> ws >> dx;
            if (!iline2){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dx<0) ;
          }
        else
          {
            if(IfpVtkFile::isEqualString(stepx_str,"DXFILE"))
              {
                dx_def = true ;
                iline2 >> ws >> dx_file_str;
                if (!iline2){
                  error() << "Syntax error in reading grid dimensions";
                  return true;
                }
                dxf.resize(nb_node_x) ;
                dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x);
                if (!dx_file_read) {
                  error() << "Error in reading file DXFILE : " << dx_file_str;
                  return true;
                }
                layer_top = (dxf[0]<0) ;
              }
          }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DXZ"))
      {
        iline >> ws >> dx >> ws >> dz  ;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepy_str;
        iline2 >> ws >> stepy_str ;
        if(IfpVtkFile::isEqualString(stepy_str,"DY"))
          {
            iline2 >> ws >> dy;
            if (!iline2){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dy<0) ;
          }
        else
          {
            if(IfpVtkFile::isEqualString(stepy_str,"DYFILE"))
              {
                dy_def = true ;
                iline2 >> ws >> dy_file_str;
                if (!iline2){
                  error() << "Syntax error in reading grid dimensions";
                  return true;
                }
                dyf.resize(nb_node_y) ;
                dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y);
                if (!dy_file_read) {
                  error() << "Error in reading file DYFILE : " << dy_file_str;
                  return true;
                }
                layer_top = (dyf[0]<0) ;
              }
          }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DX"))
      {
        iline >> ws >> dx;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepy_str;
        iline2 >> ws >> stepy_str ;
        {
          if(IfpVtkFile::isEqualString(stepy_str,"DY"))
            {
              iline2 >> ws >> dy;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dy<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepy_str,"DYFILE"))
                {
                  dy_def = true ;
                  iline2 >> ws >> dy_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dyf.resize(nb_node_y) ;
                  dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y);
                  if (!dy_file_read) {
                    error() << "Error in reading file DYFILE : " << dy_file_str;
                    return true;
                  }
                  layer_top = (dyf[0]<0) ;
                }
            }
        }
        buf = vtk_file.getNextLine();
        istringstream iline3(buf);
        std::string stepz_str;
        iline3 >> ws >> stepz_str ;
        {
          if(IfpVtkFile::isEqualString(stepz_str,"DZ"))
          {
            iline3 >> ws >> dz;
            if (!iline3){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dz<0) ;
          }
        else
          {
            if(IfpVtkFile::isEqualString(stepz_str,"DZFILE"))
              {
                dz_def = true ;
                iline3 >> ws >> dz_file_str;
                if (!iline3){
                  error() << "Syntax error in reading grid dimensions";
                  return true;
                }
                dzf.resize(nb_node_z) ;
                dz_file_read = readBufferFromFile(dz_file_str,dzf,nb_node_z);
                if (!dz_file_read) {
                  error() << "Error in reading file DZFILE : " << dz_file_str;
                  return true;
                }
                layer_top = (dzf[0]<0) ;
              }
          }
        }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DY"))
      {
        iline >> ws >> dy;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepx_str;
        iline2 >> ws >> stepx_str ;
        {
          if(IfpVtkFile::isEqualString(stepx_str,"DX"))
            {
              iline2 >> ws >> dx;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dx<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepx_str,"DXFILE"))
                {
                  dx_def = true ;
                  iline2 >> ws >> dx_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dxf.resize(nb_node_x) ;
                  dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x);
                  if (!dx_file_read) {
                    error() << "Error in reading file DXFILE : " << dx_file_str;
                    return true;
                  }
                  layer_top = (dxf[0]<0) ;
                }
            }
        }
        buf = vtk_file.getNextLine();
        istringstream iline3(buf);
        std::string stepz_str;
        iline3 >> ws >> stepz_str ;
        {
          if(IfpVtkFile::isEqualString(stepz_str,"DZ"))
          {
            iline3 >> ws >> dz;
            if (!iline3){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dz<0) ;
          }
        else
          {
            if(IfpVtkFile::isEqualString(stepz_str,"DZFILE"))
              {
                dz_def = true ;
                iline3 >> ws >> dz_file_str;
                if (!iline3){
                  error() << "Syntax error in reading grid dimensions";
                  return true;
                }
                dzf.resize(nb_node_z) ;
                dz_file_read = readBufferFromFile(dz_file_str,dzf,nb_node_z);
                if (!dz_file_read) {
                  error() << "Error in reading file DZFILE : " << dz_file_str;
                  return true;
                }
                layer_top = (dzf[0]<0) ;
              }
          }
        }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DZ"))
      {
        iline >> ws >> dz;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepx_str;
        iline2 >> ws >> stepx_str ;
        {
          if(IfpVtkFile::isEqualString(stepx_str,"DX"))
            {
              iline2 >> ws >> dx;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dx<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepx_str,"DXFILE"))
                {
                  dx_def = true ;
                  iline2 >> ws >> dx_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dxf.resize(nb_node_x) ;
                  dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x);
                  if (!dx_file_read) {
                    error() << "Error in reading file DXFILE : " << dx_file_str;
                    return true;
                  }
                  layer_top = (dxf[0]<0) ;
                }
            }
        }
        buf = vtk_file.getNextLine();
        istringstream iline3(buf);
        std::string stepy_str;
        iline3 >> ws >> stepy_str ;
        {
          if(IfpVtkFile::isEqualString(stepy_str,"DY"))
            {
              iline3 >> ws >> dy;
              if (!iline3){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dy<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepy_str,"DYFILE"))
                {
                  dy_def = true ;
                  iline3 >> ws >> dy_file_str;
                  if (!iline3){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dyf.resize(nb_node_y) ;
                  dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y);
                  if (!dy_file_read) {
                    error() << "Error in reading file DYFILE : " << dy_file_str;
                    return true;
                  }
                  layer_top = (dyf[0]<0) ;
                }
            }
        }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DXFILE"))
      {
        {
          dx_def = true ;
          iline >> ws >> dx_file_str;
          if (!iline){
            error() << "Syntax error in reading grid dimensions";
            return true;
          }
          dxf.resize(nb_node_x) ;
          dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x);
          if (!dx_file_read) {
            error() << "Error in reading file DXFILE : " << dx_file_str;
            return true;
          }
          layer_top = (dxf[0]<0) ;
        }

        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepy_str;
        iline2 >> ws >> stepy_str ;
        {
          if(IfpVtkFile::isEqualString(stepy_str,"DY"))
            {
              iline2 >> ws >> dy;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dy<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepy_str,"DYFILE"))
                {
                  dy_def = true ;
                  iline2 >> ws >> dy_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dyf.resize(nb_node_y) ;
                  dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y);
                  if (!dy_file_read) {
                    error() << "Error in reading file DYFILE : " << dy_file_str;
                    return true;
                  }
                  layer_top = (dyf[0]<0) ;
                }
            }
        }
        buf = vtk_file.getNextLine();
        istringstream iline3(buf);
        std::string stepz_str;
        iline3 >> ws >> stepz_str ;
        {
          if(IfpVtkFile::isEqualString(stepz_str,"DZ"))
          {
            iline3 >> ws >> dz;
            if (!iline3){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dz<0) ;
          }
        else
          {
            if(IfpVtkFile::isEqualString(stepz_str,"DZFILE"))
              {
                dz_def = true ;
                iline3 >> ws >> dz_file_str;
                if (!iline3){
                  error() << "Syntax error in reading grid dimensions";
                  return true;
                }
                dzf.resize(nb_node_z) ;
                dz_file_read = readBufferFromFile(dz_file_str,dzf,nb_node_z);
                if (!dz_file_read) {
                  error() << "Error in reading file DZFILE : " << dz_file_str;
                  return true;
                }
                layer_top = (dzf[0]<0) ;
              }
          }
        }
      }
    else
    {
      // STEP est volontairement non liste dans l'ensemble des mots clefs car deprecated
      vtk_file.checkString(step_str,"DXYZ","DXY","DX","DY","DZ", "DXFILE");
    }
  }
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

  Integer nb_node_yz = nb_node_y*nb_node_z;
  Integer nb_node_xy = nb_node_x*nb_node_y;

  Integer nb_cell = nb_cell_x * nb_cell_y * nb_cell_z;
  cells_local_id.resize(nb_cell);
  nodes_local_id.resize(nb_node);

  ////////////////////////////////////////////////////////////
  // Creation du maillage

  {
    SharedArray<Integer> nodes_unique_id(nb_node);

    info() << " NODE YZ = " << nb_node_yz;
    // Creation des noeuds
    //Integer nb_node_local_id = 0;
    {
      Integer node_local_id = 0;
      for( Integer z=0; z<nb_node_z; ++z ){
        for( Integer y=0; y<nb_node_y; ++y ){
          for( Integer x=0; x<nb_node_x; ++x ){

            Integer node_unique_id = x + (y)*nb_node_x + z*nb_node_x*nb_node_y;

            nodes_unique_id[node_local_id] = node_unique_id;
            //Integer owner = sub_domain_id;
            //nodes_local_id[node_local_id] = node_local_id;

            ++node_local_id;
          }
        }
      }
      //nb_node_local_id = node_local_id;
      warning() << " NB NODE LOCAL ID=" << node_local_id;
    }

    // Creation des mailles

    // Infos pour la creation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             8 pour chaque noeud
    SharedArray<Int64> cells_infos(nb_cell*10);
    Integer off_layer = 0 ;
    if(layer_top)
      off_layer = 4 ;
    {
      Integer cell_local_id = 0;
      Integer cells_infos_index = 0;

      for( Integer z=0; z<nb_cell_z; ++z ){
        for( Integer y=0; y<nb_cell_y; ++y ){
          for( Integer x=0; x<nb_cell_x; ++x ){
            Integer current_cell_nb_node = 8;

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Int64 cell_unique_id = x + y*nb_cell_x + z*nb_cell_x*nb_cell_y;

            cells_infos[cells_infos_index] = IT_Hexaedron8;
            ++cells_infos_index;

            cells_infos[cells_infos_index] = cell_unique_id;
            ++cells_infos_index;

            //Integer base_id = y + z*nb_node_y + x*nb_node_yz;
            Integer base_id = x + y*nb_node_x + z*nb_node_xy;
            cells_infos[cells_infos_index+0+off_layer] = nodes_unique_id[base_id];
            cells_infos[cells_infos_index+1+off_layer] = nodes_unique_id[base_id + 1];
            cells_infos[cells_infos_index+2+off_layer] = nodes_unique_id[base_id + nb_node_x + 1];
            cells_infos[cells_infos_index+3+off_layer] = nodes_unique_id[base_id + nb_node_x + 0];
            cells_infos[cells_infos_index+4-off_layer] = nodes_unique_id[base_id + nb_node_xy];
            cells_infos[cells_infos_index+5-off_layer] = nodes_unique_id[base_id + nb_node_xy + 1];
            cells_infos[cells_infos_index+6-off_layer] = nodes_unique_id[base_id + nb_node_xy + nb_node_x + 1];
            cells_infos[cells_infos_index+7-off_layer] = nodes_unique_id[base_id + nb_node_xy + nb_node_x + 0];
            cells_infos_index += current_cell_nb_node;
            cells_local_id[cell_local_id] = cell_local_id;
            ++cell_local_id;
          }
        }
      }
    }

    mesh->setDimension(3) ;
    mesh->allocateCells(nb_cell,cells_infos,false);
    mesh->endAllocate();

    // Positionne les coordonnees
    {
      SharedArray<Real3> coords(nb_node);
      SharedArray<Real> topz ;
      SharedArray<Real> dzf ;
      if (sub_domain_id==0){
        if(top_def)
          {
            topz.resize(nb_node_y*nb_node_x) ;
            top_file_read = readBufferFromFile(top_file_str,topz,nb_node_y*nb_node_x);
            if (!top_file_read)
              {
                error() << "Error in reading file TOP : " << top_file_str;
                return true;
              }
          }
        if(dx_def)
          {
            dxf.resize(nb_node_x) ;
            dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x) ;
            if (!dx_file_read)
              {
                error() << "Error in reading file DXFILE : " << dx_file_str;
                return true;
              }
          }
        if(dy_def)
          {
            dyf.resize(nb_node_y) ;
            dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y) ;
            if (!dy_file_read)
              {
                error() << "Error in reading file DYFILE : " << dy_file_str;
                return true;
              }
          }
        if(dz_def)
          {
            dzf.resize(nb_node_z) ;
            dz_file_read = readBufferFromFile(dz_file_str,dzf,nb_node_z) ;
            if (!dz_file_read)
              {
                error() << "Error in reading file DZFILE : " << dz_file_str;
                return true;
              }
          }
        Real nz = Oz ;
        Real tz = 0. ;
        for( Integer z=0; z<nb_node_z; ++z )
          {
            Real ny = Oy ;
            for( Integer y=0; y<nb_node_y; ++y )
              {
                Real nx = Ox ;
                for( Integer x=0; x<nb_node_x; ++x )
                  {
                    Integer node_unique_id = x + y*nb_node_x + z*nb_node_xy;
                    if(top_def) tz = topz[y*nb_node_x+x] ;
                    coords[node_unique_id] = Real3(nx,ny,nz+tz);
                    nx += dx ;
                    if(dx_def) nx += dxf[x] ;
                  }
                ny += dy ;
                if(dy_def) ny += dyf[y] ;
              }
            nz += dz ;
            if(dz_def) nz += dzf[z] ;
          }
      }
      VariableNodeReal3& nodes_coord_var(mesh->nodesCoordinates());
      ENUMERATE_NODE(inode,mesh->allNodes()){
        const Node& node = *inode;
        nodes_coord_var[inode] = coords[node.uniqueId().asInteger()];
      }
    }
  }
  return false ;
}


bool IfpVtkMeshIOService::
_readStructured2D(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition,
                  Integer nb_node_x, Integer nb_node_y,
                  Array<Integer>& cells_local_id, Array<Integer>& nodes_local_id)
{
  if (nb_node_x<0 || nb_node_y<0 || nb_node_x+nb_node_y<1){
    error() << "Invalid dimensions: x=" << nb_node_x << " y=" << nb_node_y ;
    return true;
  }

  info() << "Number of nodes (x, y) = ("
         << nb_node_x << ", "
         << nb_node_y << ")";
  Integer nb_node = nb_node_x * nb_node_y ;

  const char* buf ;
  Real Ox = 0. ;
  Real Oy = 0. ;
  Real dx = 0. ;
  Real dy = 0. ;
  SharedArray<Real> dxf ;
  SharedArray<Real> dyf ;
  bool dx_def = false ;
  bool dy_def = false ;
  bool top_file_read = false ;
  bool dx_file_read = false ;
  bool dy_file_read = false ;
  bool layer_top = false ;
  bool top_def = false ;
  std::string top_file_str("Undefined") ;
  std::string dx_file_str("Undefined") ;
  std::string dy_file_str("Undefined") ;
  // Lecture du nombre de points: POINTS nb float
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string origin_str;
    iline >> ws >> origin_str ;
    if(IfpVtkFile::isEqualString(origin_str,"ORIGIN"))
    {
      iline >> ws >> Ox >> ws >> Oy ;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
    }
    else
    {
      top_def = true ;
      if(IfpVtkFile::isEqualString(origin_str,"TOP"))
        {
          iline >> ws >> top_file_str ;
          if (!iline){
            error() << "Syntax error in reading top surface datas";
            return true;
          }
        }
    }
    vtk_file.checkString(origin_str,"ORIGIN","TOP");
  }
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string step_str;
    iline >> ws >> step_str ;
    if(IfpVtkFile::isEqualString(step_str,"DXY"))
      {
        iline >> ws >> dx >> ws >> dy  ;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine(IfpVtkFile::OPTIONAL_LINE);
      }
    else if(IfpVtkFile::isEqualString(step_str,"DX"))
      {
        iline >> ws >> dx;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepy_str;
        iline2 >> ws >> stepy_str ;
        {
          if(IfpVtkFile::isEqualString(stepy_str,"DY"))
            {
              iline2 >> ws >> dy;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dy<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepy_str,"DYFILE"))
                {
                  dy_def = true ;
                  iline2 >> ws >> dy_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dyf.resize(nb_node_y) ;
                  dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y);
                  if (!dy_file_read) {
                    error() << "Error in reading file DYFILE : " << dy_file_str;
                    return true;
                  }
                  layer_top = (dyf[0]<0) ;
                }
            }
        }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DY"))
      {
        iline >> ws >> dy;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepx_str;
        iline2 >> ws >> stepx_str ;
        {
          if(IfpVtkFile::isEqualString(stepx_str,"DX"))
            {
              iline2 >> ws >> dx;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dx<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepx_str,"DXFILE"))
                {
                  dx_def = true ;
                  iline2 >> ws >> dx_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dxf.resize(nb_node_x) ;
                  dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x);
                  if (!dx_file_read) {
                    error() << "Error in reading file DXFILE : " << dx_file_str;
                    return true;
                  }
                  layer_top = (dxf[0]<0) ;
                }
            }
        }
      }
    else if(IfpVtkFile::isEqualString(step_str,"DXFILE"))
      {
        {
          dx_def = true ;
          iline >> ws >> dx_file_str;
          if (!iline){
            error() << "Syntax error in reading grid dimensions";
            return true;
          }
          dxf.resize(nb_node_x) ;
          dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x);
          if (!dx_file_read) {
            error() << "Error in reading file DXFILE : " << dx_file_str;
            return true;
          }
          layer_top = (dxf[0]<0) ;
        }

        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepy_str;
        iline2 >> ws >> stepy_str ;
        {
          if(IfpVtkFile::isEqualString(stepy_str,"DY"))
            {
              iline2 >> ws >> dy;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              layer_top = (dy<0) ;
            }
          else
            {
              if(IfpVtkFile::isEqualString(stepy_str,"DYFILE"))
                {
                  dy_def = true ;
                  iline2 >> ws >> dy_file_str;
                  if (!iline2){
                    error() << "Syntax error in reading grid dimensions";
                    return true;
                  }
                  dyf.resize(nb_node_y) ;
                  dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y);
                  if (!dy_file_read) {
                    error() << "Error in reading file DYFILE : " << dy_file_str;
                    return true;
                  }
                  layer_top = (dyf[0]<0) ;
                }
            }
        }
      }
    else
    {
      // STEP est volontairement non liste dans l'ensemble des mots clefs car deprecated
      vtk_file.checkString(step_str,"DXYZ","DXY","DX","DY","DZ", "DXFILE");
    }
  }
  Integer sub_domain_id = subDomain()->subDomainId();

  Integer nb_cell_x = nb_node_x-1;
  Integer nb_cell_y = nb_node_y-1;

  if (use_internal_partition && sub_domain_id!=0){
    nb_node_x = 0;
    nb_node_y = 0;
    nb_cell_x = 0;
    nb_cell_y = 0;
  }

  Integer nb_cell = nb_cell_x * nb_cell_y ;
  cells_local_id.resize(nb_cell);
  nodes_local_id.resize(nb_node);

  ////////////////////////////////////////////////////////////
  // Creation du maillage

  {
    SharedArray<Integer> nodes_unique_id(nb_node);

    // Creation des noeuds
    //Integer nb_node_local_id = 0;
    {
      Integer node_local_id = 0;
      for( Integer y=0; y<nb_node_y; ++y ){
        for( Integer x=0; x<nb_node_x; ++x ){

          Integer node_unique_id = x + (y)*nb_node_x ;

          nodes_unique_id[node_local_id] = node_unique_id;

          ++node_local_id;
        }
      }
      //nb_node_local_id = node_local_id;
      warning() << " NB NODE LOCAL ID=" << node_local_id;
    }

    // Creation des mailles

    // Infos pour la creation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             4 pour chaque noeud
    SharedArray<Int64> cells_infos(nb_cell*6);
    Integer off_layer = 0 ;
    if(layer_top)
      off_layer = 2 ;
    {
      Integer cell_local_id = 0;
      Integer cells_infos_index = 0;

      for( Integer y=0; y<nb_cell_y; ++y ){
        for( Integer x=0; x<nb_cell_x; ++x ){
          Integer current_cell_nb_node = 4;

          Int64 cell_unique_id = x + y*nb_cell_x ;

          cells_infos[cells_infos_index] = IT_Quad4;
          ++cells_infos_index;

          cells_infos[cells_infos_index] = cell_unique_id;
          ++cells_infos_index;

          Integer base_id = x + y*nb_node_x ;
          cells_infos[cells_infos_index+0+off_layer] = nodes_unique_id[base_id];
          cells_infos[cells_infos_index+1+off_layer] = nodes_unique_id[base_id + 1];
          cells_infos[cells_infos_index+2-off_layer] = nodes_unique_id[base_id + nb_node_x + 1];
          cells_infos[cells_infos_index+3-off_layer] = nodes_unique_id[base_id + nb_node_x + 0];
          cells_infos_index += current_cell_nb_node;
          cells_local_id[cell_local_id] = cell_local_id;
          ++cell_local_id;
        }
      }
    }


    mesh->setDimension(2) ;
    mesh->allocateCells(nb_cell,cells_infos,false);
    mesh->endAllocate();

    // Positionne les coordonnees
    {
      SharedArray<Real3> coords(nb_node);
      SharedArray<Real> topy ;
      SharedArray<Real> dyf ;
      if (sub_domain_id==0){
        if(top_def)
          {
            topy.resize(nb_node_x) ;
            top_file_read = readBufferFromFile(top_file_str,topy,nb_node_x);
            if (!top_file_read)
              {
                error() << "Error in reading file TOP : " << top_file_str;
                return true;
              }
          }
        if(dx_def)
          {
            dxf.resize(nb_node_x) ;
            dx_file_read = readBufferFromFile(dx_file_str,dxf,nb_node_x) ;
            if (!dx_file_read)
              {
                error() << "Error in reading file DXFILE : " << dx_file_str;
                return true;
              }
          }
        if(dy_def)
          {
            dyf.resize(nb_node_y) ;
            dy_file_read = readBufferFromFile(dy_file_str,dyf,nb_node_y) ;
            if (!dy_file_read)
              {
                error() << "Error in reading file DYFILE : " << dy_file_str;
                return true;
              }
          }
        Real ny = Oy ;
        Real ty = 0. ;
        for( Integer y=0; y<nb_node_y; ++y )
          {
            Real nx = Ox ;
            for( Integer x=0; x<nb_node_x; ++x )
              {
                Integer node_unique_id = x + y*nb_node_x ;
                if(top_def) ty = topy[x] ;
                coords[node_unique_id] = Real3(nx,ny+ty,0.);
                nx += dx ;
                if(dx_def) nx += dxf[x] ;
              }
            ny += dy ;
            if(dy_def) ny += dyf[y] ;
          }
      }
      VariableNodeReal3& nodes_coord_var(mesh->nodesCoordinates());
      ENUMERATE_NODE(inode,mesh->allNodes()){
        const Node& node = *inode;
        nodes_coord_var[inode] = coords[node.uniqueId().asInteger()];
      }
    }
  }
  return false ;
}

bool IfpVtkMeshIOService::
_readStructuredGraph(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition)
{
  // IParallelMng* pm = subDomain()->parallelMng();
  // Lecture du nombre de points: DIMENSIONS nx ny nz
  info()<<"readStructuredGraph :"<<use_internal_partition;
  const char* buf = 0;
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
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(dimension_str,"DIMENSIONS");
    if (nb_node_x<0 || nb_node_y<0 || nb_node_z<0 || nb_node_x+nb_node_y+nb_node_z<1){
      error() << "Invalid dimensions: x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
      return true;
    }
  }
  info() << " Infos: " << nb_node_x << " " << nb_node_y << " " << nb_node_z;
  Real Ox = 0. ;
  Real Oy = 0. ;
  Real Oz = 0. ;
  Real dx = 0. ;
  Real dy = 0. ;
  Real dz = 0. ;
  // Lecture du nombre de points: POINTS nb float
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string origin_str;
    iline >> ws >> origin_str >> ws >> Ox >> ws >> Oy >> ws >> Oz;
    if (!iline){
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(origin_str,"ORIGIN");
  }
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string step_str;
    iline >> ws >> step_str >> ws >> dx >> ws >> dy >> ws >> dz;
    if (!iline){
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(step_str,"STEP");
  }

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

  Integer nb_node_xz = nb_node_x*nb_node_z;
  Integer nb_node_yz = nb_node_y*nb_node_z;
  Integer nb_node_xy = nb_node_x*nb_node_y;

  Integer nb_node = nb_node_x * nb_node_y * nb_node_z;
  Integer nb_cell = nb_cell_x * nb_node_yz +
    nb_cell_y * nb_node_xz +
    nb_cell_z * nb_node_xy ;
  SharedArray<Integer> cells_local_id(nb_cell);
  SharedArray<Integer> links_type(nb_cell);
  SharedArray<Integer> nodes_local_id(nb_node);

  // Creation du maillage
  {
    SharedArray<Integer> nodes_unique_id(nb_node);

    info() << " NODE YZ = " << nb_node_yz;
    // Creation des noeuds
    //Integer nb_node_local_id = 0;
    {
      Integer node_local_id = 0;
      for( Integer z=0; z<nb_node_z; ++z ){
        for( Integer y=0; y<nb_node_y; ++y ){
          for( Integer x=0; x<nb_node_x; ++x ){

            Integer node_unique_id = x + (y)*nb_node_x + z*nb_node_x*nb_node_y;

            nodes_unique_id[node_local_id] = node_unique_id;
            nodes_local_id[node_local_id] = node_local_id;

            ++node_local_id;
          }
        }
      }
      //nb_node_local_id = node_local_id;
      warning() << " NB NODE LOCAL ID=" << node_local_id;
    }


    // Creation des mailles

    // Infos pour la creation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             2 pour chaque noeud
    SharedArray<Int64> cells_infos(nb_cell*4) ;
    {
      Integer cell_local_id = 0;
      Integer cells_infos_index = 0;
      Integer current_cell_nb_node = 2;

      for( Integer z=0; z<nb_node_z; ++z ){
        for( Integer y=0; y<nb_node_y; ++y ){
          for( Integer x=0; x<nb_cell_x; ++x ){

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Integer cell_unique_id = x + y*nb_cell_x + z*nb_cell_x*nb_node_y ;

            cells_infos[cells_infos_index] = IT_Line2;
            ++cells_infos_index;

            cells_infos[cells_infos_index] = cell_unique_id;
            ++cells_infos_index;

            //Integer base_id = y + z*nb_node_y + x*nb_node_yz;
            Integer base_id = x + y*nb_node_x + z*nb_node_xy;
            cells_infos[cells_infos_index+0] = nodes_unique_id[base_id];
            cells_infos[cells_infos_index+1] = nodes_unique_id[base_id + 1];
            cells_infos_index += current_cell_nb_node;
            cells_local_id[cell_local_id] = cell_local_id;
            links_type[cell_local_id] = 0 ;
            ++cell_local_id;
          }
        }
      }
      Integer ptr_j = nb_cell_x * nb_node_yz ;
      for( Integer z=0; z<nb_node_z; ++z ){
        for( Integer y=0; y<nb_cell_y; ++y ){
          for( Integer x=0; x<nb_node_x; ++x ){

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Integer cell_unique_id = ptr_j + x + y*nb_node_x + z*nb_node_x*nb_cell_y;

            cells_infos[cells_infos_index] = IT_Line2;
            ++cells_infos_index;

            cells_infos[cells_infos_index] = cell_unique_id;
            ++cells_infos_index;

            //Integer base_id = y + z*nb_node_y + x*nb_node_yz;
            Integer base_id = x + y*nb_node_x + z*nb_node_xy;
            cells_infos[cells_infos_index+0] = nodes_unique_id[base_id];
            cells_infos[cells_infos_index+1] = nodes_unique_id[base_id + nb_node_x + 0];
            cells_infos_index += current_cell_nb_node;
            cells_local_id[cell_local_id] = cell_local_id;
            links_type[cell_local_id] = 1 ;
            ++cell_local_id;
          }
        }
      }
      Integer ptr_k = nb_cell_x * nb_node_yz + nb_cell_y*nb_node_xz ;
      for( Integer z=0; z<nb_cell_z; ++z ){
        for( Integer y=0; y<nb_node_y; ++y ){
          for( Integer x=0; x<nb_node_x; ++x ){

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Integer cell_unique_id = ptr_k + x + y*nb_node_x + z*nb_node_x*nb_node_y;

            cells_infos[cells_infos_index] = IT_Line2;
            ++cells_infos_index;

            cells_infos[cells_infos_index] = cell_unique_id;
            ++cells_infos_index;

            //Integer base_id = y + z*nb_node_y + x*nb_node_yz;
            Integer base_id = x + y*nb_node_x + z*nb_node_xy;
            cells_infos[cells_infos_index+0] = nodes_unique_id[base_id];
            cells_infos[cells_infos_index+1] = nodes_unique_id[base_id + nb_node_xy];
            cells_infos_index += current_cell_nb_node;
            cells_local_id[cell_local_id] = cell_local_id;
            links_type[cell_local_id] = 2 ;
            ++cell_local_id;
          }
        }
      }
    }

    mesh->setDimension(3);
    mesh->allocateCells(nb_cell,cells_infos,false);
    mesh->endAllocate();

    //Real Ox = vtk_file.getReal();
    //Real Oy = vtk_file.getReal();
    //Real Oz = vtk_file.getReal();
    //Real dx = vtk_file.getReal();
    //Real dy = vtk_file.getReal();
    //Real dz = vtk_file.getReal();

    // Positionne les coordonnees
    {
      SharedArray<Real3> coords(nb_node);
      Real nz = Oz ;
      for( Integer z=0; z<nb_node_z; ++z )
        {
          Real ny = Oy ;
          for( Integer y=0; y<nb_node_y; ++y )
            {
              Real nx = Ox ;
              for( Integer x=0; x<nb_node_x; ++x )
                {
                  Integer node_unique_id = x + y*nb_node_x + z*nb_node_xy;
                  coords[node_unique_id] = Real3(nx,ny,nz);
                  nx += dx ;
                }
              ny += dy ;
            }
          nz += dz ;
        }
      VariableNodeReal3& nodes_coord_var(mesh->nodesCoordinates());
      ENUMERATE_NODE(inode,mesh->allNodes()){
        const Node& node = *inode;
        nodes_coord_var[inode] = coords[node.uniqueId().asInteger()];
      }
    }
  }

  // Cree les groupes de faces des cetes du parallelepipede
  {
    SharedArray<Integer> xmin_node_lid;
    SharedArray<Integer> xmax_node_lid;
    SharedArray<Integer> ymin_node_lid;
    SharedArray<Integer> ymax_node_lid;
    SharedArray<Integer> zmin_node_lid;
    SharedArray<Integer> zmax_node_lid;

    ENUMERATE_NODE(inode,mesh->allNodes()){
      const Node& node = *inode;
      Integer node_local_id = node.localId();
      bool is_xmin = true;
      bool is_xmax = true;
      bool is_ymin = true;
      bool is_ymax = true;
      bool is_zmin = true;
      bool is_zmax = true;
      Integer node_unique_id = node.uniqueId().asInteger();
      Integer node_z = node_unique_id / nb_node_xy;
      Integer node_y = (node_unique_id - node_z*nb_node_xy) / nb_node_x;
      Integer node_x = node_unique_id - node_z*nb_node_xy - node_y*nb_node_x;
      if (node_x!=0)
        is_xmin = false;
      if (node_x!=(nb_node_x-1))
        is_xmax = false;
      if (node_y!=0)
        is_ymin = false;
      if (node_y!=(nb_node_y-1))
        is_ymax = false;
      if (node_z!=0)
        is_zmin = false;
      if (node_z!=(nb_node_z-1))
        is_zmax = false;

      if (is_xmin)
        xmin_node_lid.add(node_local_id);
      if (is_xmax)
        xmax_node_lid.add(node_local_id);
      if (is_ymin)
        ymin_node_lid.add(node_local_id);
      if (is_ymax)
        ymax_node_lid.add(node_local_id);
      if (is_zmin)
        zmin_node_lid.add(node_local_id);
      if (is_zmax)
        zmax_node_lid.add(node_local_id);

    }
    _createNodeGroup(mesh,"NodeXMIN",xmin_node_lid);
    _createNodeGroup(mesh,"NodeXMAX",xmax_node_lid);
    _createNodeGroup(mesh,"NodeYMIN",ymin_node_lid);
    _createNodeGroup(mesh,"NodeYMAX",ymax_node_lid);
    _createNodeGroup(mesh,"NodeZMIN",zmin_node_lid);
    _createNodeGroup(mesh,"NodeZMAX",zmax_node_lid);

  }
  _createGraphLinkType(    mesh,
                           use_internal_partition,
                           cells_local_id,
                           links_type) ;

  // Maintenant, regarde s'il existe des donnees associees cell aux fichiers
  bool r = _readData(   mesh,
                        vtk_file,
                        use_internal_partition,
                        nodes_local_id,
                        cells_local_id);
  return r;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool IfpVtkMeshIOService::
_readStructuredDualGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition)
{
#if (ARCANE_VERSION<11602)
  mesh::DynamicGraph* graph = dynamic_cast<mesh::DynamicGraph*> (mesh) ;
  if(graph==NULL)
    {
      error() << "The mesh should be of type DynamicGraph";
      return true ;
    }
#else
#ifndef USE_ARCANE_V3
  IGraph* graph = mesh->graph();
#endif
#endif
  // IParallelMng* pm = subDomain()->parallelMng();

  // Lecture du nombre de points: DIMENSIONS nx ny nz
  const char* buf = 0;
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
      error() << "Syntax error in reading grid dimensions";
      return true;
    }

    vtk_file.checkString(dimension_str,"DIMENSIONS");
    if (nb_node_x<=1 || nb_node_y<=1 || nb_node_z<=1){
      error() << "Invalid dimensions: x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
      return true;
    }
  }

  info() << " Infos: " << nb_node_x << " " << nb_node_y << " " << nb_node_z;
  Integer nb_node = nb_node_x * nb_node_y * nb_node_z;

  Real Ox = 0. ;
  Real Oy = 0. ;
  Real Oz = 0. ;
  Real dx = 0. ;
  Real dy = 0. ;
  Real dz = 0. ;
  // Lecture du nombre de points: POINTS nb float
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string origin_str;
    iline >> ws >> origin_str >> ws >> Ox >> ws >> Oy >> ws >> Oz;
    if (!iline){
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(origin_str,"ORIGIN");
  }
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string step_str;
    iline >> ws >> step_str >> ws >> dx >> ws >> dy >> ws >> dz;
    if (!iline){
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(step_str,"STEP");
  }

  Integer sub_domain_id = subDomain()->subDomainId();

  Integer nb_cell_x = nb_node_x-1;
  Integer nb_cell_y = nb_node_y-1;
  Integer nb_cell_z = nb_node_z-1;

  Integer nb_dnode_x = nb_cell_x ;
  Integer nb_dnode_y = nb_cell_y ;
  Integer nb_dnode_z = nb_cell_z ;
  Integer nb_link_x = nb_dnode_x -1 ;
  Integer nb_link_y = nb_dnode_y -1 ;
  Integer nb_link_z = nb_dnode_z -1 ;

  if (use_internal_partition && sub_domain_id!=0){
    nb_node_x = 0;
    nb_node_y = 0;
    nb_node_z = 0;
    nb_cell_x = 0;
    nb_cell_y = 0;
    nb_cell_z = 0;
    nb_dnode_x = 0;
    nb_dnode_y = 0;
    nb_dnode_z = 0;
    nb_link_x = 0;
    nb_link_y = 0;
    nb_link_z = 0;
  }

  Integer nb_node_yz = nb_node_y*nb_node_z;
  Integer nb_node_xy = nb_node_x*nb_node_y;

  Integer nb_cell = nb_cell_x * nb_cell_y * nb_cell_z;

  SharedArray<Integer> cells_local_id(nb_cell);
  SharedArray<Integer> nodes_local_id(nb_node);

  Integer nb_dnode_xz = nb_dnode_x*nb_dnode_z;
  Integer nb_dnode_yz = nb_dnode_y*nb_dnode_z;
  Integer nb_dnode_xy = nb_dnode_x*nb_dnode_y;

  Integer nb_dnode = nb_dnode_x * nb_dnode_y * nb_dnode_z;
  Integer nb_link = nb_link_x * nb_dnode_yz +
    nb_link_y * nb_dnode_xz +
    nb_link_z * nb_dnode_xy ;

  SharedArray<Integer> links_local_id(nb_link);
  SharedArray<Integer> links_type(nb_link);
  SharedArray<Integer> dnodes_local_id(nb_dnode);

  // Creation du maillage et du graphe
  {
    SharedArray<Integer> nodes_unique_id(nb_node);

    info() << " NODE YZ = " << nb_node_yz;
    // Creation des noeuds
    //Integer nb_node_local_id = 0;
    {
      Integer node_local_id = 0;
      for( Integer z=0; z<nb_node_z; ++z ){
        for( Integer y=0; y<nb_node_y; ++y ){
          for( Integer x=0; x<nb_node_x; ++x ){

            Integer node_unique_id = x + (y)*nb_node_x + z*nb_node_x*nb_node_y;

            nodes_unique_id[node_local_id] = node_unique_id;
            //Integer owner = sub_domain_id;
            //nodes_local_id[node_local_id] = node_local_id;

            ++node_local_id;
          }
        }
      }
      //nb_node_local_id = node_local_id;
      warning() << " NB NODE LOCAL ID=" << node_local_id;
    }

    // Creation des mailles

    // Infos pour la creation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             8 pour chaque noeud
    SharedArray<Int64> cells_infos(nb_cell*10);

    {
      Integer cell_local_id = 0;
      Integer cells_infos_index = 0;

      for( Integer z=0; z<nb_cell_z; ++z ){
        for( Integer y=0; y<nb_cell_y; ++y ){
          for( Integer x=0; x<nb_cell_x; ++x ){
            Integer current_cell_nb_node = 8;

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Int64 cell_unique_id = x + y*nb_cell_x + z*nb_cell_x*nb_cell_y;

            cells_infos[cells_infos_index] = IT_Hexaedron8;
            ++cells_infos_index;

            cells_infos[cells_infos_index] = cell_unique_id;
            ++cells_infos_index;

            //Integer base_id = y + z*nb_node_y + x*nb_node_yz;
            Integer base_id = x + y*nb_node_x + z*nb_node_xy;
            cells_infos[cells_infos_index+0] = nodes_unique_id[base_id];
            cells_infos[cells_infos_index+1] = nodes_unique_id[base_id + 1];
            cells_infos[cells_infos_index+2] = nodes_unique_id[base_id + nb_node_x + 1];
            cells_infos[cells_infos_index+3] = nodes_unique_id[base_id + nb_node_x + 0];
            cells_infos[cells_infos_index+4] = nodes_unique_id[base_id + nb_node_xy];
            cells_infos[cells_infos_index+5] = nodes_unique_id[base_id + nb_node_xy + 1];
            cells_infos[cells_infos_index+6] = nodes_unique_id[base_id + nb_node_xy + nb_node_x + 1];
            cells_infos[cells_infos_index+7] = nodes_unique_id[base_id + nb_node_xy + nb_node_x + 0];
            cells_infos_index += current_cell_nb_node;
            cells_local_id[cell_local_id] = cell_local_id;
            ++cell_local_id;
          }
        }
      }
    }



    // Creation du graph
#if (ARCANE_VERSION<11602)
    CArrayInt64 dnodes_unique_id(nb_dnode);
#else
    Int64SharedArray dnodes_unique_id(nb_dnode);
    Int64SharedArray dnodes_infos(3*nb_dnode);
#endif

    info() << " NODE YZ = " << nb_dnode_yz;
    // Creation des noeuds
    //Integer nb_dnode_local_id = 0;
#if (ARCANE_VERSION>=11602)
    Integer dnode_info = 0;
#endif
    {
      Integer dnode_local_id = 0;
      for( Integer z=0; z<nb_dnode_z; ++z ){
        for( Integer y=0; y<nb_dnode_y; ++y ){
          for( Integer x=0; x<nb_dnode_x; ++x ){

            Integer dnode_unique_id = x + (y)*nb_dnode_x + z*nb_dnode_x*nb_dnode_y;

            dnodes_unique_id[dnode_local_id] = dnode_unique_id;

            dnodes_local_id[dnode_local_id] = dnode_local_id;

#if (ARCANE_VERSION>=11602)
            dnodes_infos[dnode_info++] = IT_DualCell;
            dnodes_infos[dnode_info++] = dnode_unique_id;
            dnodes_infos[dnode_info++] = dnode_unique_id;
#endif
            ++dnode_local_id;
          }
        }
      }
      //nb_dnode_local_id = dnode_local_id;
      warning() << " NB NODE LOCAL ID=" << dnode_local_id;
    }

    // Creation des liaisons

    // Infos pour la creation des liaisons
    // type de noeud dual : 1
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             2 pour chaque noeud
#if (ARCANE_VERSION<11602)
    CArrayInt64 links_infos(nb_link*4+1) ;
#else
	#if (ARCANE_VERSION >= 12201)
    	SharedArray<Int64> links_infos(nb_link*4) ;
	#else /* ARCANE_VERSION */
			CArrayInt64 links_infos(nb_link*4) ;
	#endif /* ARCANE_VERSION */
#endif
    {
      Integer link_local_id = 0;
      Integer links_infos_index = 0;
      Integer current_link_nb_dnode = 2;
#if (ARCANE_VERSION<11602)
      links_infos[links_infos_index] = IT_DualCell ;
      ++links_infos_index;
#endif
      for( Integer z=0; z<nb_dnode_z; ++z ){
        for( Integer y=0; y<nb_dnode_y; ++y ){
          for( Integer x=0; x<nb_link_x; ++x ){

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Integer link_unique_id = x + y*nb_link_x + z*nb_link_x*nb_dnode_y ;

#if (ARCANE_VERSION<11602)
            links_infos[links_infos_index] = IT_Line2;
#else 
            links_infos[links_infos_index] = 2;
#endif
            ++links_infos_index;

            links_infos[links_infos_index] = link_unique_id;
            ++links_infos_index;

            //Integer base_id = y + z*nb_dnode_y + x*nb_dnode_yz;
            Integer base_id = x + y*nb_dnode_x + z*nb_dnode_xy;
            links_infos[links_infos_index+0] = dnodes_unique_id[base_id];
            links_infos[links_infos_index+1] = dnodes_unique_id[base_id + 1];
            links_infos_index += current_link_nb_dnode;
            links_local_id[link_local_id] = link_local_id;
            links_type[link_local_id] = 0 ;
            ++link_local_id;
          }
        }
      }
      Integer ptr_j = nb_link_x * nb_dnode_yz ;
      for( Integer z=0; z<nb_dnode_z; ++z ){
        for( Integer y=0; y<nb_link_y; ++y ){
          for( Integer x=0; x<nb_dnode_x; ++x ){

            //Integer link_unique_id = y + (z)*nb_link_y + x*nb_link_y*nb_link_z;
            Integer link_unique_id = ptr_j + x + y*nb_dnode_x + z*nb_dnode_x*nb_link_y;

#if (ARCANE_VERSION<11602)
            links_infos[links_infos_index] = IT_Line2;
#else 
            links_infos[links_infos_index] = 2;
#endif
            ++links_infos_index;

            links_infos[links_infos_index] = link_unique_id;
            ++links_infos_index;

            //Integer base_id = y + z*nb_dnode_y + x*nb_dnode_yz;
            Integer base_id = x + y*nb_dnode_x + z*nb_dnode_xy;
            links_infos[links_infos_index+0] = dnodes_unique_id[base_id];
            links_infos[links_infos_index+1] = dnodes_unique_id[base_id + nb_dnode_x + 0];
            links_infos_index += current_link_nb_dnode;
            links_local_id[link_local_id] = link_local_id;
            links_type[link_local_id] = 1 ;
            ++link_local_id;
          }
        }
      }
      Integer ptr_k = nb_link_x * nb_dnode_yz + nb_link_y*nb_dnode_xz ;
      for( Integer z=0; z<nb_link_z; ++z ){
        for( Integer y=0; y<nb_dnode_y; ++y ){
          for( Integer x=0; x<nb_dnode_x; ++x ){

            //Integer link_unique_id = y + (z)*nb_link_y + x*nb_link_y*nb_link_z;
            Integer link_unique_id = ptr_k + x + y*nb_dnode_x + z*nb_dnode_x*nb_dnode_y;

#if (ARCANE_VERSION<11602)
            links_infos[links_infos_index] = IT_Line2;
#else 
            links_infos[links_infos_index] = 2;
#endif
            ++links_infos_index;

            links_infos[links_infos_index] = link_unique_id;
            ++links_infos_index;

            //Integer base_id = y + z*nb_dnode_y + x*nb_dnode_yz;
            Integer base_id = x + y*nb_dnode_x + z*nb_dnode_xy;
            links_infos[links_infos_index+0] = dnodes_unique_id[base_id];
            links_infos[links_infos_index+1] = dnodes_unique_id[base_id + nb_dnode_xy];
            links_infos_index += current_link_nb_dnode;
            links_local_id[link_local_id] = link_local_id;
            links_type[link_local_id] = 2 ;
            ++link_local_id;
          }
        }
      }
    }

#if (ARCANE_VERSION<11602)
    graph->setDimension(3) ;
    graph->allocateCells(nb_cell,cells_infos,false);
    // cells_unique_id = dnodes_unique_id dans ce cas particulier
    graph->allocateLinks(nb_link,links_infos,false);
    graph->addDualNodes(IT_DualCell,nb_dnode,dnodes_unique_id,dnodes_unique_id) ;
    graph->endAllocate();
#else 
    mesh->setDimension(3) ;
    mesh->allocateCells(nb_cell,cells_infos,false);
#ifndef USE_ARCANE_V3
    graph->modifier()->addLinks(nb_link,links_infos);
    graph->modifier()->addDualNodes(nb_dnode,dnodes_infos) ;
#endif
    mesh->endAllocate();
#endif

    // Positionne les coordonnees
    {
      SharedArray<Real3> coords(nb_node);
      Real nz = Oz ;
      for( Integer z=0; z<nb_node_z; ++z )
        {
          Real ny = Oy ;
          for( Integer y=0; y<nb_node_y; ++y )
            {
              Real nx = Ox ;
              for( Integer x=0; x<nb_node_x; ++x )
                {
                  Integer node_unique_id = x + y*nb_node_x + z*nb_node_xy;
                  coords[node_unique_id] = Real3(nx,ny,nz);
                  nx += dx ;
                }
              ny += dy ;
            }
          nz += dz ;
        }
      VariableNodeReal3& nodes_coord_var(mesh->nodesCoordinates());
      ENUMERATE_NODE(inode,mesh->allNodes()){
        const Node& node = *inode;
        nodes_coord_var[inode] = coords[node.uniqueId().asInteger()];
      }
    }
  }

  // Cree les groupes de faces des cetes du parallelepipede
  {
    SharedArray<Integer> xmin_surface_lid;

    SharedArray<Integer> xmax_surface_lid;
    SharedArray<Integer> ymin_surface_lid;
    SharedArray<Integer> ymax_surface_lid;
    SharedArray<Integer> zmin_surface_lid;
    SharedArray<Integer> zmax_surface_lid;

    ENUMERATE_FACE(iface,mesh->allFaces()){
      const Face& face = *iface;
      Integer face_local_id = face.localId();
      bool is_xmin = true;
      bool is_xmax = true;
      bool is_ymin = true;
      bool is_ymax = true;
      bool is_zmin = true;
      bool is_zmax = true;
      for( NodeEnumerator inode(face.nodes()); inode(); ++inode ){
        const Node& node = *inode;
        Int64 node_unique_id = node.uniqueId();
        Int64 node_z = node_unique_id / nb_node_xy;
        Int64 node_y = (node_unique_id - node_z*nb_node_xy) / nb_node_x;
        Int64 node_x = node_unique_id - node_z*nb_node_xy - node_y*nb_node_x;
        if (node_x!=0)
          is_xmin = false;
        if (node_x!=(nb_node_x-1))
          is_xmax = false;
        if (node_y!=0)
          is_ymin = false;
        if (node_y!=(nb_node_y-1))
          is_ymax = false;
        if (node_z!=0)
          is_zmin = false;
        if (node_z!=(nb_node_z-1))
          is_zmax = false;
      }
      if (is_xmin)
        xmin_surface_lid.add(face_local_id);
      if (is_xmax)
        xmax_surface_lid.add(face_local_id);
      if (is_ymin)
        ymin_surface_lid.add(face_local_id);
      if (is_ymax)
        ymax_surface_lid.add(face_local_id);
      if (is_zmin)
        zmin_surface_lid.add(face_local_id);
      if (is_zmax)
        zmax_surface_lid.add(face_local_id);

    }
    _createFaceGroup(mesh,"XMIN",xmin_surface_lid);
    _createFaceGroup(mesh,"XMAX",xmax_surface_lid);
    _createFaceGroup(mesh,"YMIN",ymin_surface_lid);
    _createFaceGroup(mesh,"YMAX",ymax_surface_lid);
    _createFaceGroup(mesh,"ZMIN",zmin_surface_lid);
    _createFaceGroup(mesh,"ZMAX",zmax_surface_lid);

  }

  // Maintenant, regarde s'il existe des donnees associees cell aux fichiers
  bool r = _readData(   mesh,
                        vtk_file,
                        use_internal_partition,
                        nodes_local_id,
                        cells_local_id);
  if (r)
    return r;
  return false;
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Lecture des noeuds et de leur coordonnees.
 */
void IfpVtkMeshIOService::
_readNodesUnstructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,Array<Real3>& node_coords)
{
  const char* func_name = "IfpVtkMeshIOService::_readNodesUnstructuredGrid()";
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);
  std::string points_str;
  std::string data_type_str;
  Integer nb_node = 0;
  iline >> ws >> points_str >> ws >> nb_node >> ws >> data_type_str;
  if (!iline)
    throw IOException(func_name,"Syntax error in reading number of points ");
  vtk_file.checkString(points_str,"POINTS");
  vtk_file.checkString(data_type_str,"float","double");
  if (nb_node<0){
    String msg = "Number of nodes invalid: n=" + nb_node;
    throw IOException(func_name,msg);
  }

  info() << " Infos: " << nb_node;

  // Lecture les coordonnees
  node_coords.resize(nb_node);
  {
    for( Integer i=0; i<nb_node; ++i ){
      Real nx = vtk_file.getReal();
      Real ny = vtk_file.getReal();
      Real nz = vtk_file.getReal();
      node_coords[i] = Real3(nx,ny,nz);
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Lecture des mailles et de leur connectivite.
 *
 * En retour, remplit \a cells_nb_node, \a cells_type et \a cells_connectivity.
 */
void IfpVtkMeshIOService::
_readCellsUnstructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,
                           Array<Integer>& cells_nb_node,
                           Array<Integer>& cells_type,
                           Array<Int64>& cells_connectivity)
{
  const char* func_name = "IfpVtkMeshIOService::_readCellsUnstructuredGrid()";
  const char* buf = vtk_file.getNextLine();
  istringstream iline(buf);
  std::string cells_str;
  Integer nb_cell = 0;
  Integer nb_cell_node = 0;
  iline >> ws >> cells_str >> ws >> nb_cell >> ws >> nb_cell_node;
  if (!iline)
    throw IOException(func_name,"Syntax error in reading cells");
  vtk_file.checkString(cells_str,"CELLS");
  if (nb_cell<0 || nb_cell_node<0){
    StringBuilder msg("Invalid dimensions: nb_cell=");
    msg += nb_cell;
    msg += " nb_cell_node=";
    msg += nb_cell_node;
    throw IOException(func_name,msg.toString());
  }

  cells_nb_node.resize(nb_cell);
  cells_type.resize(nb_cell);
  cells_connectivity.resize(nb_cell_node);
  {
    Integer connectivity_index = 0;
    for( Integer i=0; i<nb_cell; ++i ){
      Integer n = vtk_file.getInteger();
      cells_nb_node[i] = n;
      for( Integer j=0; j<n; ++j ){
        Integer id = vtk_file.getInteger();
        cells_connectivity[connectivity_index] = id;
        ++connectivity_index;
      }
    }
  }

  // Lecture du type des mailles
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string cell_types_str;
    Integer nb_cell_type;
    iline >> ws >> cell_types_str >> ws >> nb_cell_type;
    if (!iline){
      throw IOException(func_name,"Syntax error in reading type of cells");
    }
    vtk_file.checkString(cell_types_str,"CELL_TYPES");
    if (nb_cell_type!=nb_cell){
      StringBuilder msg = "Incoherent number CELL_TYPES: v=";
      msg += nb_cell_type;
      msg += " nb_cell=";
      msg += nb_cell;
      throw IOException(func_name,msg.toString());
    }
  }
  for( Integer i=0; i<nb_cell; ++i ){
    Integer vtk_ct = vtk_file.getInteger();
    Integer it = IT_NullType;
    // Le type est defini dans vtkCellType.h
    switch(vtk_ct){
    case 0: it = IT_NullType; break; // VTK_EMPTY_CELL
    case 1: it = IT_Vertex; break; // VTK_VERTEX
    case 3: it = IT_Line2; break; // VTK_LINE
    case 5: it = IT_Triangle3; break; // VTK_TRIANGLE
    case 9: it = IT_Quad4; break; // VTK_QUAD
    case 7: // VTK_POLYGON (a tester...)
      if (cells_nb_node[i]==5)
        it = IT_Pentagon5;
      if (cells_nb_node[i]==6)
        it = IT_Hexagon6;
#if ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0)
      if (cells_nb_node[i]==7)
        it = IT_Heptagon7;
      if (cells_nb_node[i]==8)
        it = IT_Octogon8;
#endif
      break;
    case 10: it = IT_Tetraedron4; break; // VTK_TETRA
    case 14: it = IT_Pyramid5; break; // VTK_PYRAMID
    case 13: it = IT_Pentaedron6; break; // VTK_WEDGE
    case 12: it = IT_Hexaedron8; break; // VTK_HEXAHEDRON
    case 15: it = IT_Heptaedron10; break; // VTK_PENTAGONAL_PRISM
    case 16: it = IT_Octaedron12; break; // VTK_HEXAGONAL_PRISM
#if ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0)
    case 27: it = IT_Enneedron14; break;
    case 28: it = IT_Decaedron16;break;
#endif
    default:
      {
        String msg = "Type of vtk cell unknown or invalid: type=" + vtk_ct;
        throw IOException(func_name,msg);
      }
      break;
    }
    cells_type[i] = it;
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool IfpVtkMeshIOService::
_readUnstructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition, const Integer mesh_dimension)
{
  // const char* func_name = "IfpVtkMeshIOService::_readUnstructuredGrid()";
  // IParallelMng* pm = subDomain()->parallelMng();
  // ITraceMng* msg = subDomain()->traceMng();

  Integer nb_node = 0;
  Integer nb_cell = 0;
  Integer nb_cell_node = 0;
  Integer sid = subDomain()->subDomainId();
  SharedArray<Real3> node_coords;
  SharedArray<Int64> cells_infos;
  SharedArray<Integer> cells_local_id;
  SharedArray<Integer> nodes_local_id;
  // Si on utilise le partitionneur interne, seul le sous-domaine lit le maillage
  bool need_read = true;
  if (use_internal_partition)
    need_read = (sid==0);

  if (need_read){
    _readNodesUnstructuredGrid(mesh,vtk_file,node_coords);
    nb_node = node_coords.size();
    nodes_local_id.resize(nb_node);
    for(int inode=0;inode<nb_node;inode++)
      nodes_local_id[inode] = inode ;
    info()<<" nb_node"<<nb_node;

    // Lecture des infos des mailles
    // Lecture de la connectivite
    SharedArray<Integer> cells_nb_node;
    SharedArray<Int64> cells_connectivity;
    SharedArray<Integer> cells_type;
    _readCellsUnstructuredGrid(mesh,vtk_file,cells_nb_node,cells_type,cells_connectivity);
    nb_cell = cells_nb_node.size();
    nb_cell_node = cells_connectivity.size();
    cells_local_id.resize(nb_cell);

    // Creation des mailles
    // Infos pour la creation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             1 pour chaque noeud
    cells_infos.resize(nb_cell*2 + nb_cell_node);
    {
      Integer cells_infos_index = 0;
      Integer connectivity_index = 0;
      for( Integer i=0; i<nb_cell; ++i ){
        Integer current_cell_nb_node = cells_nb_node[i];
        Integer cell_unique_id = i;

        cells_local_id[i] = i;

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

  mesh->setDimension(mesh_dimension);
  mesh->allocateCells(nb_cell,cells_infos,false);
  mesh->endAllocate();

  // Positionne les coordonnees
  {
    VariableNodeReal3& nodes_coord_var(mesh->nodesCoordinates());
    ENUMERATE_NODE(inode,mesh->allNodes()){
      const Node& node = *inode;
      nodes_coord_var[inode] = node_coords[node.uniqueId().asInt64()];
    }
  }

  // Maintenant, regarde s'il existe des donnees associees aux fichier
  bool r = _readData(   mesh,
                        vtk_file,
                        use_internal_partition,
                        nodes_local_id,
                        cells_local_id);
  return r;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_readFacesMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition)
{
  // const char* func_name = "IfpVtkMeshIOService::_readFacesMesh()";
  // ITraceMng* msg = subDomain()->traceMng();

  ifstream ifile(file_name.localstr());
  if (!ifile){
    info() << "No file to describe the faces '" << file_name << "'";
    return;
  }
  IfpVtkFile vtk_file(&ifile);
  const char* buf = 0;
  // Lecture de la description
  buf = vtk_file.getNextLine();
  String format = vtk_file.getNextLine();
  if (not IfpVtkFile::isEqualString(format,"ASCII")){
    error() << "Only the format 'ASCII' is recognized (format='" << format << "')";
    return;
  }
  eMeshType mesh_type = VTK_MT_Unknown;
  // Lecture du type de maillage
  // TODO: en parallele, avec use_internal_partition vrai, seul le processeur 0
  // lit les donnees. Dans ce cas, inutile que les autres ouvre le fichier.
  {
    buf = vtk_file.getNextLine();
    istringstream mesh_type_line(buf);
    std::string dataset_str;
    std::string mesh_type_str;
    mesh_type_line >> ws >> dataset_str >> ws >> mesh_type_str;
    vtk_file.checkString(dataset_str,"DATASET");
    if (IfpVtkFile::isEqualString(mesh_type_str,"UNSTRUCTURED_GRID")){
      mesh_type = VTK_MT_UnstructuredGrid;
    }
    if (mesh_type==VTK_MT_Unknown){
      error() << "The file to describe the faces should be of format 'UNSTRUCTURED_GRID' (format=" << mesh_type_str << "')";
      return;
    }
  }
  {
    IParallelMng* pm = subDomain()->parallelMng();

    //Integer nb_node = 0;
    Integer nb_face = 0;
    //Integer nb_face_node = 0;
    Integer sid = pm->commRank() ;

    SharedArray<Integer> faces_local_id;

    // Si on utilise le partitionneur interne, seul le sous-domaine lit le maillage
    bool need_read = true;
    if (use_internal_partition)
      need_read = (sid==0);

    if (need_read){
      {
        // Lit des noeuds, mais ne conserve pas leur coordonnees
        SharedArray<Real3> node_coords;
        _readNodesUnstructuredGrid(mesh,vtk_file,node_coords);
        //nb_node = node_coords.size();
      }

      // Lecture des infos des faces
      // Lecture de la connectivite
      SharedArray<Integer> faces_nb_node;
      SharedArray<Int64> faces_connectivity;
      SharedArray<Integer> faces_type;
      _readCellsUnstructuredGrid(mesh,vtk_file,faces_nb_node,faces_type,faces_connectivity);
      nb_face = faces_nb_node.size();
      //nb_face_node = faces_connectivity.size();

      // Il faut a partir de la connectivite retrouver les localId() des faces
      faces_local_id.resize(nb_face);
      {
        IMeshUtilities* mu = mesh->utilities();
        mu->localIdsFromConnectivity(IK_Face,faces_nb_node,faces_connectivity,faces_local_id);
      }
    }


    // Maintenant, regarde s'il existe des donnees associees aux fichier
    _readFaceData(mesh,vtk_file,use_internal_partition,IK_Face,faces_local_id);
  }
}

bool IfpVtkMeshIOService::
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

void IfpVtkMeshIOService::
_readStructuredFacesMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition)
{
  // const char* func_name = "IfpVtkMeshIOService::_readFacesMesh()";
  // ITraceMng* msg = subDomain()->traceMng();

  IParallelMng* pm = subDomain()->parallelMng();
  Integer sid = pm->commRank() ;
  // Si on utilise le partitionneur interne, seul le sous-domaine lit le maillage
  // bool need_read = true;
  // if (use_internal_partition)
  //   need_read = (sid==0);

  ifstream ifile(file_name.localstr());
  if (!ifile){
    info() << "No file to describe the faces '" << file_name << "'";
    return;
  }
  IfpVtkFile vtk_file(&ifile);
  const char* buf = 0;
  Integer nb_node_x = 0;
  Integer nb_node_y = 0;
  Integer nb_node_z = 0;
  Integer nb_face = 0 ;
  Integer nb_face_node = 0 ;
  if(sid==0)
  {
    {
      buf = vtk_file.getNextLine();
      istringstream iline(buf);
      std::string dimension_str;
      iline >> ws >> dimension_str >> ws >> nb_node_x
            >> ws >> nb_node_y >> ws >> nb_node_z;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return ;
      }

      vtk_file.checkString(dimension_str,"DIMENSIONS");
      if (nb_node_x<0 || nb_node_y<0 || nb_node_z<0 || nb_node_x+nb_node_y+nb_node_z<1){
        error() << "Invalid dimensions: x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
        return ;
      }
    }
    {
      buf = vtk_file.getNextLine();
      istringstream iline(buf);
      std::string faces_str;
      iline >> ws >> faces_str >> ws >> nb_face;
      vtk_file.checkString(faces_str,"FACES");
      if (nb_face<0){
        error() << "Invalid dimensions: number of faces=" << nb_face;
        return ;
      }
    }

    info() << " Infos: " << nb_node_x << " " << nb_node_y << " " << nb_node_z;
    info() << " Number of faces: " << nb_face;
    //nb_node = nb_node_x * nb_node_y * nb_node_z;

    // Integer sub_domain_id = subDomain()->subDomainId();

    // Integer nb_cell_x = nb_node_x-1;
    // Integer nb_cell_y = nb_node_y-1;
    // Integer nb_cell_z = nb_node_z-1;

    // Integer nb_node_yz = nb_node_y*nb_node_z;
    // Integer nb_node_xy = nb_node_x*nb_node_y;

    //nb_cell = nb_cell_x * nb_cell_y * nb_cell_z;
  }
  SharedArray<Integer> faces_local_id(nb_face);
  if(sid==0)
  {
    // Lecture des infos des faces
    // Lecture de la connectivite
    SharedArray<Integer> faces_nb_node;
    SharedArray<Int64> faces_connectivity;
    SharedArray<Integer> faces_type;
    nb_face_node=4*nb_face ;
    faces_nb_node.resize(nb_face);
    faces_type.resize(nb_face);
    faces_connectivity.resize(nb_face_node);
    {
      Integer connectivity_index = 0;
      for( Integer iface=0; iface<nb_face; ++iface )
      {
        Integer i = vtk_file.getInteger();
        Integer j = vtk_file.getInteger();
        Integer k = vtk_file.getInteger();
        Integer l = vtk_file.getInteger();
        if(l<0||l>2) error()<<"l:"<<l<<"out of range";
        faces_nb_node[iface] = 4;
        if(l==0)
          {
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            i++ ;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            j++ ;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            i--;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            ++connectivity_index;
          }
        if(l==1)
          {
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            i++ ;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            k++ ;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            i--;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            ++connectivity_index;
          }
        if(l==2)
          {
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            j++ ;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            k++ ;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            j--;
            ++connectivity_index;
            if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
            faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
            ++connectivity_index;
          }
        faces_type[iface] = IT_Quad4 ;
      }
    }

    // Il faut a partir de la connectivite retrouver les localId() des faces
    //faces_local_id.resize(nb_face);
    {
      IMeshUtilities* mu = mesh->utilities();
      mu->localIdsFromConnectivity(IK_Face,faces_nb_node,faces_connectivity,faces_local_id);
    }
  }

  // Maintenant, regarde s'il existe des donnees associees aux fichier
  _readFaceData(mesh,vtk_file,use_internal_partition,IK_Face,faces_local_id);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool IfpVtkMeshIOService::_readData( IPrimaryMesh* mesh,
                                     IfpVtkFile& vtk_file,
                                     bool use_internal_partition,
                                     Array<Integer>& node_local_ids,
                                     Array<Integer>& cell_local_ids,
                                     const char * buf)
{
  // Seul le sous-domain maitre lit les valeurs. Par contre, les autres
  // sous-domaines doivent connaitre la liste des variables et groupes creees.
  // Si une donnee porte le nom 'GROUP_*', on considere qu'il s'agit d'un
  // groupe
  info()<<"READATA :****************************";
  OStringStream created_infos_str;
  created_infos_str() << "<?xml version='1.0' ?>\n";
  created_infos_str() << "<infos>";
  Integer sid = subDomain()->subDomainId();
  IParallelMng* pm = subDomain()->parallelMng();
  Integer nb_node_kind = mesh->nbItem(IK_Node);
  Integer nb_cell_kind = mesh->nbItem(IK_Cell);

  bool reading_node = false;
  bool reading_cell = false;
  //eItemKind item_kind ;
  if (sid==0){
    bool continue_loop = ((buf!=NULL) or ( !vtk_file.isEnd() && (buf=vtk_file.getNextLine())!=0));
    while(continue_loop) {
      info() << "Read line";
      istringstream iline(buf);
      std::string data_str;
      iline >> data_str;
      if (IfpVtkFile::isEqualString(data_str,"POINT_DATA")){
        Integer nb_item =0;
        iline >> ws >> nb_item;
        reading_node = true;
        reading_cell = false;
        //item_kind = IK_Node ;
      }
      else if (IfpVtkFile::isEqualString(data_str,"CELL_DATA")){
        Integer nb_item =0;
        iline >> ws >> nb_item;
        reading_cell = true;
        reading_node = false;
        //item_kind = IK_Cell ;
      }
      else if (reading_node){
        if(_readNodeData(   mesh,
                            vtk_file,
                            nb_node_kind,
                            node_local_ids,
                            buf,
                            iline,
                            data_str,
                            created_infos_str))
          return true ;
      }
      else if(reading_cell){
        if(_readCellData(   mesh,
                            vtk_file,
                            nb_cell_kind,
                            cell_local_ids,
                            buf,
                            iline,
                            data_str,
                            created_infos_str))
          return true ;
      }
      else {
        error() << "Wait for value POINT_DATA or CELL_DATA, read ='" << data_str << "'";
        return true;
      }
      continue_loop = ( !vtk_file.isEnd() && (buf=vtk_file.getNextLine())!=0);
    }
  }

  created_infos_str() << "</infos>";
  if (use_internal_partition){
    ByteSharedArray bytes;
    if (sid==0){
      info()<<"READATA :**********PROC MASTER";
      const String str = created_infos_str.str();
      Integer len = str.length();
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
        XmlNodeList vars = doc_node.documentElement().children("node-variable");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){  
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Create variable: " << name;
          VariableNodeReal* var = new VariableNodeReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("node-variable3");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){  
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Create variable: " << name;
          VariableNodeReal3* var = new VariableNodeReal3(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal3>(var));
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
          info() << "Create group: " << name;
          node_family->createGroup(name);
        }
      }
      // Lecture des variables
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-variable");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){  
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Create variable: " << name;
          VariableCellReal* var = new VariableCellReal(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
          mesh->userDataList()->setData(name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(var));
        }
      }
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-variable3");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){  
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Create variable: " << name;
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
          info() << "Create group: " << name;
          cell_family->createGroup(name);
        }
      }
    }
  }
  return false;
}

bool IfpVtkMeshIOService::_readNodeData( IPrimaryMesh* mesh,
                                         IfpVtkFile& vtk_file,
                                         int nb_node_kind,
                                         Array<Integer>& local_ids,
                                         const char* buf,
                                         istringstream& iline,
                                         std::string& data_str,
                                         OStringStream& created_infos_str)
{
  std::string type_str;
  std::string name_str;
  bool is_group = false;
  int nb_component = 0;
  iline >> ws >> name_str >> ws >> type_str >> ws >> nb_component;
  info() << "** ** ** READNAME: name=" << name_str << " type=" << type_str;
  String cstr(name_str.c_str());
  if (cstr.startsWith("GROUP_")){
    is_group = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** GROUP ! name=" << new_name;
    name_str = new_name;
  }
  if (not IfpVtkFile::isEqualString(data_str,"SCALARS") and not IfpVtkFile::isEqualString(data_str,"VECTORS")){
    error() << "Only datas of type 'SCALAR' or 'VECTOR' are supported, lu=" << data_str;
    return true;
  }
  if (is_group)
    {
      if (not IfpVtkFile::isEqualString(type_str,"int"))
        {
          error() << "The type of data for groups should be 'int', read=" << type_str;
          return true;
        }
      // Pour lire LOOKUP_TABLE
      buf = vtk_file.getNextLine();
      created_infos_str() << "<node-group name='" << name_str << "'/>";
      _readItemGroup(mesh,vtk_file,name_str,nb_node_kind,IK_Node,local_ids);
    }
  else
    {
      if (not IfpVtkFile::isEqualString(type_str,"float") and not IfpVtkFile::isEqualString(type_str,"double"))
        {
          error() << "Only datas of type 'float' or 'double' are supported, read=" << type_str;
          return true;
        }
      // Pour lire LOOKUP_TABLE
			if (IfpVtkFile::isEqualString(data_str,"SCALARS")){
      	buf = vtk_file.getNextLine();
      	created_infos_str() << "<node-variable name='" << name_str << "'/>";
      	_readNodeVariable(mesh,vtk_file,name_str,nb_node_kind);
			}
			else{
      	created_infos_str() << "<node-variable3 name='" << name_str << "'/>";
      	_readNodeVectorVariable(mesh,vtk_file,name_str,nb_node_kind);
			}
    }
  return false ;
}




bool IfpVtkMeshIOService::_readCellData(IPrimaryMesh* mesh,
                                        IfpVtkFile& vtk_file,
                                        int nb_cell_kind,
                                        Array<Integer>& local_ids,
                                        const char* buf,
                                        istringstream& iline,
                                        std::string& data_str,
                                        OStringStream& created_infos_str)
{
  // Seul le sous-domain maitre lit les valeurs. Par contre, les autres
  // sous-domaines doivent connaitre la liste des variables et groupes creees.
  // Si une donnee porte le nom 'GROUP_*', on considere qu'il s'agit d'un
  // groupe


  std::string type_str;
  std::string name_str;
  bool is_group = false;
  int nb_component = 0;
  iline >> ws >> name_str >> ws >> type_str >> ws >> nb_component;
  info() << "** ** ** READNAME: name=" << name_str << " type=" << type_str;
  String cstr(name_str.c_str());
  if (cstr.startsWith("GROUP_")){
    is_group = true;
    std::string new_name(name_str.c_str()+6);
    info() << "** ** ** GROUP ! name=" << new_name;
    name_str = new_name;
  }
  if (not IfpVtkFile::isEqualString(data_str,"SCALARS") and not IfpVtkFile::isEqualString(data_str,"VECTORS")){
    error() << "Only datas of type 'SCALAR' or 'VECTOR' are supported, lu=" << data_str;
    return true;
  }
  if (is_group){
    if (not IfpVtkFile::isEqualString(type_str,"int")){
      error() << "The type of data for groups should be 'int', read=" << type_str;
      return true;
    }
    // Pour lire LOOKUP_TABLE
    buf = vtk_file.getNextLine();
    created_infos_str() << "<cell-group name='" << name_str << "'/>";
    _readItemGroup(mesh,vtk_file,name_str,nb_cell_kind,IK_Cell,local_ids);
  }
  else
    {
      if (not IfpVtkFile::isEqualString(type_str,"float") and not IfpVtkFile::isEqualString(type_str,"double")){
        error() << "Only datas of type 'float' or 'double' are supported, read=" << type_str;
        return true;
      }
      // Pour lire LOOKUP_TABLE
			if (IfpVtkFile::isEqualString(data_str,"SCALARS")){
      	buf = vtk_file.getNextLine();
				created_infos_str() << "<cell-variable name='" << name_str << "'/>";
				_readCellVariable(mesh,vtk_file,name_str,nb_cell_kind);
			}
			else{
      created_infos_str() << "<cell-variable3 name='" << name_str << "'/>";
      _readCellVectorVariable(mesh,vtk_file,name_str,nb_cell_kind);
			}
    }
  return false ;
}




bool IfpVtkMeshIOService::
_readFaceData(IPrimaryMesh* mesh,
              IfpVtkFile& vtk_file,
              bool use_internal_partition,
              eItemKind item_kind,
              ConstArrayView<Integer> local_ids)
{
  // Seul le sous-domain maitre lit les valeurs. Par contre, les autres
  // sous-domaines doivent connaitre la liste des variables et groupes creees.
  // Si une donnee porte le nom 'GROUP_*', on considere qu'il s'agit d'un
  // groupe

  OStringStream created_infos_str;
  created_infos_str() << "<?xml version='1.0' ?>\n";
  created_infos_str() << "<infos>";
  Integer sid = subDomain()->subDomainId();
  IParallelMng* pm = subDomain()->parallelMng();
  Integer nb_item_kind = mesh->nbItem(item_kind);
  Integer nb_faces = 0 ;

  const char* buf = 0;
  bool reading_cell = false;
  if (sid==0){
    while ( !vtk_file.isEnd() && ((buf = vtk_file.getNextLine()) != 0)){
      info() << "Read line";
      istringstream iline(buf);
      std::string data_str;
      iline >> data_str;
      if (IfpVtkFile::isEqualString(data_str,"CELL_DATA")){
        Integer nb_item =0;
        iline >> ws >> nb_item;
        reading_cell = true;
        nb_faces = nb_item ;
      }
      else{
        if (reading_cell){
          std::string type_str;
          std::string name_str;
          bool is_group = false;
          int nb_component = 0;
          iline >> ws >> name_str >> ws >> type_str >> ws >> nb_component;
          info() << "** ** ** READNAME: name=" << name_str << " type=" << type_str;
          String cstr(name_str.c_str());
          if (cstr.startsWith("GROUP_")){
            is_group = true;
            std::string new_name(name_str.c_str()+6);
            info() << "** ** ** GROUP ! name=" << new_name;
            name_str = new_name;
          }
          if (not IfpVtkFile::isEqualString(data_str,"SCALARS")){
            error() << "Only datas of type 'SCALAR' are supported, lu=" << data_str;
            return true;
          }
          if (is_group){
            if (not IfpVtkFile::isEqualString(type_str,"int")){
              error() << "The type of data for groups should be 'int', read=" << type_str;
              return true;
            }
            // Pour lire LOOKUP_TABLE
            buf = vtk_file.getNextLine();
            if (reading_cell){
              created_infos_str() << "<cell-group name='" << name_str << "'/>";
              _readItemGroup(mesh,vtk_file,name_str,nb_faces,item_kind,local_ids);
            }
          }
          else{
            if (not IfpVtkFile::isEqualString(type_str,"float") and not IfpVtkFile::isEqualString(type_str,"double")){
              error() << "Only datas of type 'float' or 'double' are supported, read=" << type_str;
              return true;
            }
            // Pour lire LOOKUP_TABLE
            buf = vtk_file.getNextLine();
            if (reading_cell){
              created_infos_str() << "<cell-variable name='" << name_str << "'/>";
              if (item_kind!=IK_Cell)
                throw IOException("Read of face variables in not supported");
              _readCellVariable(mesh,vtk_file,name_str,nb_item_kind);
            }
          }
        }
        else{
          error() << "Wait value CELL_DATA or POINT_DATA, read='" << data_str << "'";
          return true;
        }
      }
    }
  }
  created_infos_str() << "</infos>";
  if (use_internal_partition){
    ByteSharedArray bytes;
    if (sid==0){
      const String str = created_infos_str.str();
      Integer len = str.length();
      bytes.resize(len+1);
      ::memcpy(bytes.unguardedBasePointer(),str.localstr(),len+1);
      info() << "SEND STR=" << str;
    }
    pm->broadcastMemoryBuffer(bytes,0);
    if (sid!=0){
      String str((const char*)bytes.unguardedBasePointer(),bytes.size());
      info() << "FOUND STR=" << bytes.size() << " " << str;
      IIOMng* iomng = subDomain()->ioMng();
      IXmlDocumentHolder* doc = iomng->parseXmlBuffer(bytes,"InternalBuffer");
      XmlNode doc_node = doc->documentNode();
      // Lecture des variables
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-variable");
#if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){  
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Create variable: " << name;
          VariableCellReal var(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
        }
      }
      // Lecture des groupes
      {
        XmlNodeList vars = doc_node.documentElement().children("cell-group");
        IItemFamily* item_family = mesh->itemFamily(item_kind);
 #if (ARCANE_VERSION<11602)
        for( XmlNodeList::const_iter i(vars); i(); ++i ){
#else
        for( XmlNodeList::const_iterator i = vars.begin(), end = vars.end(); i != end; ++i ){  
#endif
          XmlNode xnode = *i;
          String name = xnode.attrValue("name");
          info() << "Creation group: " << name;
          item_family->createGroup(name);
        }
      }
    }
  }
  return false;
}

bool IfpVtkMeshIOService::_createGraphLinkType(    IPrimaryMesh* mesh,
                                                   bool use_internal_partition,
                                                   Array<Integer>& cell_local_ids,
                                                   Array<Integer>& links_type)
{
  // Seul le sous-domain maitre lit les valeurs. Par contre, les autres
  // sous-domaines doivent connaitre la liste des variables et groupes creees.

  //OStringStream created_infos_str;
  //created_infos_str() << "<?xml version='1.0' ?>\n";
  //created_infos_str() << "<infos>";
  Integer sid = subDomain()->subDomainId();
  // IParallelMng* pm = subDomain()->parallelMng();
  Integer nb_cell_kind = mesh->nbItem(IK_Cell);
  std::string name_str("CellVtkType");
  info() << "Read values of variable: " << name_str << " n=" << nb_cell_kind;
  VariableCellReal var(VariableBuildInfo(mesh,name_str,IVariable::PNoRestore));
  if (sid==0)
    {
      RealArrayView values = var.asArray();
      for( Integer i=0; i<nb_cell_kind; ++i )
        {
          values[i] = links_type[i];
        }
    }
  info() << "End create variable: " ;

  /*
    eItemKind item_kind ;
    if (sid==0){
    item_kind = IK_Cell ;

    std::string type_str;
    std::string name_str("CellVtkType");
    created_infos_str() << "<cell-variable name='" << name_str << "'/>";

    info() << "Read values of variable: " << name_str << " n=" << nb_cell_kind;
    VariableCellReal var(VariableBuildInfo(mesh,name_str,IVariable::PNoRestore));
    CArrayBaseReal values(var.variable()->accessor()->asArrayReal());
    for( Integer i=0; i<nb_cell_kind; ++i ){
    values[i] = links_type[i];
    }
    info() << "End create variable: " ;
    }
    created_infos_str() << "</infos>";
    if (use_internal_partition){
    CArrayByte bytes;
    if (sid==0){
    const String str = created_infos_str.localstr();
    Integer len = str.len();
    bytes.resize(len+1);
    ::memcpy(bytes.begin(),str.localstr(),len+1);
    info() << "SEND STR=" << str;
    }
    pm->broadcastMemoryBuffer(bytes,0);
    if (sid!=0){
    String str(bytes.begin(),bytes.size());
    info() << "FOUND STR=" << bytes.size() << " " << str;
    IIOMng* iomng = subDomain()->ioMng();
    IXmlDocumentHolder* doc = iomng->parseXmlBuffer(bytes,"InternalBuffer");
    XmlNode doc_node = doc->documentNode();
    // Lecture des variables
    {
    XmlNodeList vars = doc_node.documentElement().children("cell-variable");
    for( XmlNodeList::const_iter i(vars); i(); ++i ){
    XmlNode xnode = *i;
    String name = xnode.attrValue("name");
    info() << "Create variable: " << name;
    VariableNodeReal var(VariableBuildInfo(mesh,name,IVariable::PNoRestore));
    }
    }
    }
    }
  */
  return false;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_createFaceGroup(IMesh* mesh,const String& name,ConstArrayView<Integer> faces_lid)
{
  info() << "Create face group '" << name << "'"
         << " size = " << faces_lid.size();

  mesh->faceFamily()->createGroup(name,faces_lid);
}

void IfpVtkMeshIOService::
_createNodeGroup(IMesh* mesh,const String& name,ConstArrayView<Integer> nodes_lid)
{
  info() << "Create node group '" << name << "'"
         << " size = " << nodes_lid.size();

  mesh->nodeFamily()->createGroup(name,nodes_lid);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_readNodeVariable(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& var_name,Integer nb_node)
{
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Read values of variable: " << var_name << " n=" << nb_node;
  VariableNodeReal * var_node = new VariableNodeReal(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal>(var_node));

  RealArrayView  values = var_node->asArray();

  for( Integer i=0; i<nb_node; ++i ){
    Real v = vtk_file.getReal();
    values[i] = v;
  }
  info() << "End create variable: " << vtk_file.isEnd();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_readNodeVectorVariable(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& var_name,Integer nb_node)
{
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Read values of variable: " << var_name << " n=" << nb_node;
  VariableNodeReal3 * var_node = new VariableNodeReal3(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableNodeReal3>(var_node));

  Real3ArrayView  values = var_node->asArray();

  for( Integer i=0; i<nb_node; ++i ){
    Real vx = vtk_file.getReal();
    Real vy = vtk_file.getReal();
    Real vz = vtk_file.getReal();
    values[i] = Real3(vx,vy,vz);
  }
  info() << "End create variable: " << vtk_file.isEnd();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_readCellVariable(IPrimaryMesh* mesh,
                  IfpVtkFile& vtk_file,
                  const String& var_name,
                  Integer nb_cell)
{
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Read values of variable: " << var_name << " n=" << nb_cell;

  VariableCellReal * var_cell = new VariableCellReal(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal>(var_cell));

  RealArrayView  values = var_cell->asArray();


  for( Integer i=0; i<nb_cell; ++i ){
    Real v = vtk_file.getReal();
    values[i] = v;
  }
  info() << "End create variable: " << var_cell->variable()->fullName() << " " << vtk_file.isEnd();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_readCellVectorVariable(IPrimaryMesh* mesh,
                        IfpVtkFile& vtk_file,
                        const String& var_name,
                        Integer nb_cell)
{
  //TODO Faire la conversion uniqueId() vers localId() correcte
  info() << "Read values of variable: " << var_name << " n=" << nb_cell;

  VariableCellReal3 * var_cell = new VariableCellReal3(VariableBuildInfo(mesh,var_name,IVariable::PNoRestore));
  mesh->userDataList()->setData(var_name,new Arcane::AutoDestroyUserData<Arcane::VariableCellReal3>(var_cell));

  Real3ArrayView  values = var_cell->asArray();


  for( Integer i=0; i<nb_cell; ++i ){
    Real vx = vtk_file.getReal();
    Real vy = vtk_file.getReal();
    Real vz = vtk_file.getReal();
    values[i] = Real3(vx,vy,vz);
  }
  info() << "End create variable: " << var_cell->variable()->fullName() << " " << vtk_file.isEnd();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpVtkMeshIOService::
_readItemGroup(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,const String& name,Integer nb_item,
               eItemKind ik,ConstArrayView<Integer> local_ids)
{
  IItemFamily* item_family = mesh->itemFamily(ik);
  info() << "Reading infos for group: " << name;

  SharedArray<Integer> ids;
  for( Integer i=0; i<nb_item; ++i ){
    Integer v = vtk_file.getInteger();
    if (v!=0)
      ids.add(local_ids[i]);
  }
  info() << "Create group: " << name << " nb_element=" << ids.size();

  item_family->createGroup(name,ids);
}

void IfpVtkMeshIOService::
_createItemGroup(IMesh* mesh,
                 IfpVtkFile& vtk_file,
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
    // Integer nb_item_xy = nb_item_x*nb_item_y ;
    // Integer local_nb_item_xy = local_nb_item_x*local_nb_item_y ;
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
  info() << "Create group: " << group_name << " nb_element=" << ids.size();
  item_family->createGroup(group_name,ids);
}

void IfpVtkMeshIOService::
_computePartition(Integer nproc,Integer* np1, Integer* np2)
{
  String cut_dir = platform::getEnvironmentVariable("PARTITIONER_CUT_DIR");
  if(!cut_dir.null())
  {
    if(cut_dir.localstr()[0]=='X')
    {
      String npx_str = platform::getEnvironmentVariable("PARTITIONER_CUT_NPX");
      if(npx_str.null())
      {
        *np1 = nproc ;
        *np2 = 1 ;
      }
      else
      {
        Integer npx = nproc+1 ;
        builtInGetValue(npx,npx_str.localstr()) ;
        if(nproc%npx!=0)
          fatal()<<"PARTITIONER_CUT_NPX "<<npx_str<<" incompatible with number of procs : "<<nproc ;
        *np1 = npx ;
        *np2 = nproc/npx ;
      }
      info()<<"Partition dir X : NPX="<<*np1<<" NPY="<<*np2;
      return ;
    }
    else if(cut_dir.localstr()[0]=='Y')
    {
      String npy_str = platform::getEnvironmentVariable("PARTITIONER_CUT_NPY");
      if(npy_str.null())
      {
        *np2 = nproc ;
        *np1 = 1 ;
      }
      else
      {
        Integer npy = nproc+1 ;
        builtInGetValue(npy,npy_str.localstr()) ;
        if(nproc%npy!=0)
          fatal()<<"PARTITIONER_CUT_NPY "<<npy_str<<" incompatible with number of procs : "<<nproc ;
        *np2 = npy ;
        *np1 = nproc/npy ;
      }
      info()<<"Partition dir Y : NPX="<<*np1<<" NPY="<<*np2;
      return ;
    }
  }
  Integer npx = (Integer) math::sqrt((Real) nproc) ;
  while(nproc%npx!=0) npx--;
  *np1 = npx ;
  *np2 = nproc/npx ;
  info()<<"Partition dir XY : NPX="<<*np1<<" NPY="<<*np2;
}

bool IfpVtkMeshIOService::
_readDistStructuredGrid(IPrimaryMesh* mesh,IfpVtkFile& vtk_file,bool use_internal_partition)
{
  IParallelMng* pm = subDomain()->parallelMng();

  ////////////////////////////////////////////////////////////
  // Lecture des parametres obligatoires

  // Lecture du nombre de points: DIMENSIONS nx ny nz
  const char* buf = 0;
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
      error() << "Syntax error in reading grid dimensions";
      return true;
    }
    vtk_file.checkString(dimension_str,"DIMENSIONS");
    if (nb_node_x<0 || nb_node_y<0 || nb_node_z<0 || nb_node_x+nb_node_y+nb_node_z<1){
      error() << "Invalid dimensions: x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
      return true;
    }
  }

  info() << "Number of nodes (x, y, z) = ("
         << nb_node_x << ", "
         << nb_node_y << ", "
         << nb_node_z << ")";
  Integer nb_node = nb_node_x * nb_node_y * nb_node_z;

  Real Ox = 0. ;
  Real Oy = 0. ;
  Real Oz = 0. ;
  Real dx = 0. ;
  Real dy = 0. ;
  Real dz = 0. ;
  SharedArray<Real> dzf ;
  bool top_def = false ;
  bool top_file_read = false ;
  bool dz_def = false ;
  bool dz_file_read = false ;
  bool layer_top = false ;
  std::string top_file_str("Undefined") ;
  std::string dz_file_str("Undefined") ;
  // Lecture du nombre de points: POINTS nb float
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string origin_str;
    iline >> ws >> origin_str ;
    if(origin_str=="ORIGIN")
    {
      iline >> ws >> Ox >> ws >> Oy >> ws >> Oz;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
    }
    else
    {
      top_def = true ;
      if(origin_str=="TOP")
        {
          iline >> ws >> top_file_str ;
          if (!iline){
            error() << "Syntax error in reading top surface data";
            return true;
          }
        }
    }
    vtk_file.checkString(origin_str,"ORIGIN","TOP");
  }
  {
    buf = vtk_file.getNextLine();
    istringstream iline(buf);
    std::string step_str;
    iline >> ws >> step_str ;
    if(step_str=="DXYZ")
    {
      iline >> ws >> dx >> ws >> dy >> ws >> dz;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
      layer_top = (dz<0) ;
    }
    else if(step_str=="STEP")
    {
      warning() << "STEP keyword is deprecated; use DXYZ instead.";
      iline >> ws >> dx >> ws >> dy >> ws >> dz;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return true;
      }
      layer_top = (dz<0) ;
    }
    else if(step_str=="DXY")
      {
        iline >> ws >> dx >> ws >> dy  ;
        if (!iline){
          error() << "Syntax error in reading grid dimensions";
          return true;
        }
        buf = vtk_file.getNextLine();
        istringstream iline2(buf);
        std::string stepz_str;
        iline2 >> ws >> stepz_str ;
        if(stepz_str=="DZ")
          {
            iline2 >> ws >> dz;
            if (!iline2){
              error() << "Syntax error in reading grid dimensions";
              return true;
            }
            layer_top = (dz<0) ;
          }
        else
          {
            if(stepz_str=="DZFILE")
            {
              dz_def = true ;
              iline2 >> ws >> dz_file_str;
              if (!iline2){
                error() << "Syntax error in reading grid dimensions";
                return true;
              }
              dzf.resize(nb_node_z) ;
              readBufferFromFile(dz_file_str,dzf,nb_node_z) ;
              layer_top = (dzf[0]<0) ;
            }
          }
      }
    else
    {
      // STEP est volontairement non liste dans l'ensemble des mots clefs car deprecated
      vtk_file.checkString(step_str,"DXYZ","DXY");
    }
  }

  Integer sub_domain_id = subDomain()->subDomainId();
  Integer nproc = pm->commSize() ;
  Integer rank = pm->commRank() ;

  info()<<" Number of domains "<<nproc;
  info()<<" Rank of current domain "<<rank;
  info()<<" Use internal partitioner :"<<use_internal_partition;

  Integer nb_cell_x = nb_node_x-1;
  Integer nb_cell_y = nb_node_y-1;
  Integer nb_cell_z = nb_node_z-1;
  Integer first_x = 0 ;
  Integer first_y = 0 ;
  Integer local_nb_node_x = nb_node_x ;
  Integer local_nb_node_y = nb_node_y ;
  Integer local_nb_cell_x = nb_cell_x ;
  Integer local_nb_cell_y = nb_cell_y ;

  if (use_internal_partition)
  {
    if(sub_domain_id!=0)
    {
      nb_node_x = 0;
      nb_node_y = 0;
      nb_node_z = 0;
      nb_cell_x = 0;
      nb_cell_y = 0;
      nb_cell_z = 0;
      local_nb_node_x = 0;
      local_nb_node_y = 0;
      local_nb_cell_x = 0;
      local_nb_cell_y = 0;
    }
  }
  else
  {
     Integer np1 = 1 ;
     Integer np2 = nproc ;
     _computePartition(nproc,&np1,&np2) ;
    Integer npx = np1 ;
    Integer npy = np2 ;
    if(nb_cell_x>nb_cell_y)
    {
       npx = np2 ;
       npy = np1 ;
    }
    SharedArray<Integer> ix(npx+1) ;
    Integer ni = nb_cell_x/npx ;
    Integer ri = nb_cell_x%npx ;
    ix[0] = 0 ;
    for(Integer i=0;i<ri;i++)
       ix[i+1]=ix[i]+ni+1;
    for(Integer i=ri;i<npx;i++)
       ix[i+1]=ix[i]+ni;
    SharedArray<Integer> jy(npy+1) ;
    Integer nj = nb_cell_y/npy ;
    Integer rj = nb_cell_y%npy ;
    jy[0] = 0 ;
    for(Integer i=0;i<rj;i++)
       jy[i+1]=jy[i]+nj+1;
    for(Integer i=rj;i<npy;i++)
       jy[i+1]=jy[i]+nj;
    Integer rx = rank%npx ;
    Integer ry = rank/npx ;
    first_x = ix[rx] ;
    first_y = jy[ry] ;
    local_nb_node_x = ix[rx+1]-ix[rx]+1 ;
    local_nb_node_y = jy[ry+1]-jy[ry]+1 ;
    local_nb_cell_x = local_nb_node_x - 1 ;
    local_nb_cell_y = local_nb_node_y - 1 ;
    info()<<"Local nb node x"<<local_nb_node_x;
    info()<<"Local nb node y"<<local_nb_node_y;
    info()<<"Local nb cell x"<<local_nb_cell_x;
    info()<<"Local nb cell y"<<local_nb_cell_y;
    info()<<"NPX NPY "<<npx<<" "<<npy;
    for(Integer i=0;i<npx+1;i++)
       info()<<" ix["<<i<<"] "<<ix[i];
    for(Integer i=0;i<npy+1;i++)
       info()<<" jy["<<i<<"] "<<jy[i];
  }

  Integer nb_node_yz = nb_node_y*nb_node_z;
  Integer nb_node_xy = nb_node_x*nb_node_y;

  Integer nb_cell = nb_cell_x * nb_cell_y * nb_cell_z;

  Integer local_nb_cell = local_nb_cell_x * local_nb_cell_y * nb_cell_z;
  Integer local_nb_node = local_nb_node_x * local_nb_node_y * nb_node_z ;

  // Integer local_nb_node_yz = local_nb_node_y*nb_node_z;
  Integer local_nb_node_xy = local_nb_node_x*local_nb_node_y;

  SharedArray<Integer> cells_local_id(local_nb_cell);
  SharedArray<Integer> nodes_local_id(local_nb_node);

  info()<<"Total number of nodes : "<<nb_node;
  info()<<"Total number of cells : "<<nb_cell;
  info()<<"Local number of nodes : "<<local_nb_node;
  info()<<"Local number of cells : "<<local_nb_cell;
  info()<<"First X "<<first_x;
  info()<<"First Y "<<first_y;

  ////////////////////////////////////////////////////////////
  // Creation du maillage

  {
    SharedArray<Integer> nodes_unique_id(local_nb_node);

    info() << " NODE YZ = " << nb_node_yz;
    // Creation des noeuds
    //Integer nb_node_local_id = 0;
    {
      Integer node_local_id = 0;
      for( Integer z=0; z<nb_node_z; ++z ){
        for( Integer y=first_y; y<first_y+local_nb_node_y; ++y ){
          for( Integer x=first_x; x<first_x+local_nb_node_x; ++x ){
            Integer node_unique_id = x + y*nb_node_x + z*nb_node_xy;
            nodes_unique_id[node_local_id] = node_unique_id;
            //Integer owner = sub_domain_id;
            //nodes_local_id[node_local_id] = node_local_id;

            ++node_local_id;
          }
        }
      }
      //nb_node_local_id = node_local_id;
      warning() << " NB NODE LOCAL ID=" << node_local_id;
    }

    // Creation des mailles

    // Infos pour la creation des mailles
    // par maille: 1 pour son unique id,
    //             1 pour son type,
    //             8 pour chaque noeud
    SharedArray<Int64> cells_infos(local_nb_cell*10);
    Integer off_layer = 0 ;
    if(layer_top)
      off_layer = 4 ;
    {
      Integer cell_local_id = 0;
      Integer cells_infos_index = 0;

      for( Integer z=0; z<nb_cell_z; ++z ){
        for( Integer y=first_y; y<first_y+local_nb_cell_y; ++y ){
          for( Integer x=first_x; x<first_x+local_nb_cell_x; ++x ){
            Integer current_cell_nb_node = 8;

            //Integer cell_unique_id = y + (z)*nb_cell_y + x*nb_cell_y*nb_cell_z;
            Int64 cell_unique_id = x + y*nb_cell_x + z*nb_cell_x*nb_cell_y;
            cells_infos[cells_infos_index] = IT_Hexaedron8;
            ++cells_infos_index;

            cells_infos[cells_infos_index] = cell_unique_id;
            ++cells_infos_index;

            //Integer base_id = y + z*nb_node_y + x*nb_node_yz;
            Integer base_id = (x-first_x) + (y-first_y)*local_nb_node_x + z*local_nb_node_xy;
            cells_infos[cells_infos_index+0+off_layer] = nodes_unique_id[base_id];
            cells_infos[cells_infos_index+1+off_layer] = nodes_unique_id[base_id + 1];
            cells_infos[cells_infos_index+2+off_layer] = nodes_unique_id[base_id + local_nb_node_x + 1];
            cells_infos[cells_infos_index+3+off_layer] = nodes_unique_id[base_id + local_nb_node_x + 0];
            cells_infos[cells_infos_index+4-off_layer] = nodes_unique_id[base_id + local_nb_node_xy];
            cells_infos[cells_infos_index+5-off_layer] = nodes_unique_id[base_id + local_nb_node_xy + 1];
            cells_infos[cells_infos_index+6-off_layer] = nodes_unique_id[base_id + local_nb_node_xy + local_nb_node_x + 1];
            cells_infos[cells_infos_index+7-off_layer] = nodes_unique_id[base_id + local_nb_node_xy + local_nb_node_x + 0];
            cells_infos_index += current_cell_nb_node;
            cells_local_id[cell_local_id] = cell_local_id;
            ++cell_local_id;
          }
        }
      }
    }

    info()<<"Start building mesh"<<local_nb_cell<<" "<<cells_infos.size();
    mesh->setDimension(3) ;
    mesh->allocateCells(local_nb_cell,cells_infos,false);
    mesh->endAllocate();

    // Positionne les coordonnees
    info()<<"Start creating mesh coordinates";
    {
      SharedArray<Real3> coords(local_nb_node);
      SharedArray<Real> topz ;
      SharedArray<Real> dzf ;
      if ((sub_domain_id==0)||(!use_internal_partition))
      {
        if(top_def)
        {
          topz.resize(nb_node_y*nb_node_x) ;
          top_file_read = readBufferFromFile(top_file_str,topz,nb_node_y*nb_node_x) ;
	  if (!top_file_read) {
	    error() << "Error in reading file TOP : " << top_file_str;
	    return true;
	  }
	}
	if(dz_def)
        {
          dzf.resize(nb_node_z) ;
          dz_file_read = readBufferFromFile(dz_file_str,dzf,nb_node_z) ;
	  if (!dz_file_read) {
	    error() << "Error in reading file DZFILE : " << dz_file_str;
	    return true;
	  }
        }
        Real nz = Oz ;
        Real tz = 0. ;
        for( Integer z=0; z<nb_node_z; ++z )
        {
          Real ny = Oy+first_y*dy ;
          for( Integer y=first_y; y<first_y+local_nb_node_y; ++y )
            {
              Real nx = Ox+first_x*dx ;
              for( Integer x=first_x; x<first_x+local_nb_node_x; ++x )
                {
                  // Integer node_unique_id = x + y*nb_node_x + z*nb_node_xy;
                  Integer node_local_id = (x-first_x) +(y-first_y)*local_nb_node_x + z*local_nb_node_xy;
                  if(top_def) tz = topz[y*nb_node_x+x] ;
                  coords[node_local_id] = Real3(nx,ny,nz+tz);
                  nx += dx ;
                }
              ny += dy ;
            }
          nz += dz ;
          if(dz_def) nz += dzf[z] ;
        }
      }
      VariableNodeReal3& nodes_coord_var(mesh->nodesCoordinates());
      ENUMERATE_NODE(inode,mesh->ownNodes()){
        const Node& node = *inode;
        Int64 node_unique_id = node.uniqueId();
        Int64 node_z = node_unique_id / nb_node_xy;
        Int64 node_y = (node_unique_id - node_z*nb_node_xy) / nb_node_x;
        Int64 node_x = node_unique_id - node_z*nb_node_xy - node_y*nb_node_x;
        Integer x = node_x - first_x ;
        Integer y = node_y - first_y ;
        Integer z = node_z ;
        Integer node_local_id = x +y*local_nb_node_x + z*local_nb_node_xy;
        //info()<<" Node : lid uid : "<<node_local_id<<" "<<node_unique_id<<" x,y,z : "<<x<<" "<<y<<" "<<z;
        nodes_coord_var[inode] = coords[node_local_id];
      }
      if(!use_internal_partition)
        nodes_coord_var.synchronize() ;
    }
  }

  // Cree les groupes de faces des cotes du parallelepipede
  SharedArray<Integer> xmin_surface_lid;
  SharedArray<Integer> xmax_surface_lid;
  SharedArray<Integer> ymin_surface_lid;
  SharedArray<Integer> ymax_surface_lid;
  SharedArray<Integer> zmin_surface_lid;
  SharedArray<Integer> zmax_surface_lid;

  ENUMERATE_FACE(iface,mesh->allFaces()){
    const Face& face = *iface;
    Integer face_local_id = face.localId();
    bool is_xmin = true;
    bool is_xmax = true;
    bool is_ymin = true;
    bool is_ymax = true;
    bool is_zmin = true;
    bool is_zmax = true;
    for( NodeEnumerator inode(face.nodes()); inode(); ++inode ){
      const Node& node = *inode;
      Int64 node_unique_id = node.uniqueId();
      Int64 node_z = node_unique_id / nb_node_xy;
      Int64 node_y = (node_unique_id - node_z*nb_node_xy) / nb_node_x;
      Int64 node_x = node_unique_id - node_z*nb_node_xy - node_y*nb_node_x;
      if (node_x!=0)
        is_xmin = false;
      if (node_x!=(nb_node_x-1))
        is_xmax = false;
      if (node_y!=0)
        is_ymin = false;
      if (node_y!=(nb_node_y-1))
        is_ymax = false;
      if (node_z!=0)
        is_zmin = false;
      if (node_z!=(nb_node_z-1))
        is_zmax = false;
    }
    if (is_xmin)
      xmin_surface_lid.add(face_local_id);
    if (is_xmax)
      xmax_surface_lid.add(face_local_id);
    if (is_ymin)
      ymin_surface_lid.add(face_local_id);
    if (is_ymax)
      ymax_surface_lid.add(face_local_id);
    if (is_zmin)
      zmin_surface_lid.add(face_local_id);
    if (is_zmax)
      zmax_surface_lid.add(face_local_id);

  }
  _createFaceGroup(mesh,"XMIN",xmin_surface_lid);
  _createFaceGroup(mesh,"XMAX",xmax_surface_lid);
  _createFaceGroup(mesh,"YMIN",ymin_surface_lid);
  _createFaceGroup(mesh,"YMAX",ymax_surface_lid);
  _createFaceGroup(mesh,"ZMIN",zmin_surface_lid);
  _createFaceGroup(mesh,"ZMAX",zmax_surface_lid);

  ////////////////////////////////////////////////////////////
  // Lecture des parametres optionnels

  info() << "Reading optional parameters";

  std::string tag_zmin = "DEFAULT";
  std::string tag_zmax = "DEFAULT";
  std::string tag_ymin = "DEFAULT";
  std::string tag_ymax = "DEFAULT";
  std::string tag_xmin = "DEFAULT";
  std::string tag_xmax = "DEFAULT";

  bool ok = true ;
  do {
    buf = vtk_file.getNextLine(IfpVtkFile::OPTIONAL_LINE);
    if( buf == NULL ) break;
    istringstream iline(buf);

    // Lis le mot cle
    std::string keyword;
    iline >> ws >> keyword;

    if( keyword == "TAGS" ) {
      // Lecture des etiquettes associeees aux faces du parallelepipede
      // pour le conditions au bord dans l'ordre suivant:
      // z = zmin, z = zmax, y = ymin, y = ymax, x = xmin, x = xmax

      iline >> ws >> tag_zmin >> ws >> tag_zmax
            >> ws >> tag_ymin >> ws >> tag_ymax
            >> ws >> tag_xmin >> ws >> tag_xmax;
      if(!iline) {
        error() << "Syntax error in reading tags associated to boundary faces";
        return true;
      }
      info() << "Tags associated to boundary faces:";
      info() << "(z = zmin, z = zmax, y = ymin, y = ymax, x = xmin, x = xmax) = ("
             << tag_zmin << ", " << tag_zmax << ", "
             << tag_ymin << ", " << tag_ymax << ", "
             << tag_xmin << ", " << tag_xmax << ")";
    } else if( keyword == "CELLGROUPS" ) {
      Integer nb_groups = 0 ;
      iline >> ws >> nb_groups;
      if (!iline){
        error() << "Syntax error in reading cell groups";
        return true;
      }
      for(Integer igroup = 0; igroup < nb_groups; igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_cell_x, nb_cell_y, nb_cell_z,
                         first_x,local_nb_cell_x,first_y,local_nb_cell_y,IK_Cell,
                         use_internal_partition) ;
    } else if( keyword == "NODEGROUPS" ) {
      Integer nb_groups = 0 ;
      if (!iline){
        error() << "Syntax error in reading node groups";
        return true;
      }
      for(Integer igroup=0;igroup<nb_groups;igroup++)
        _createItemGroup(mesh, vtk_file, buf, nb_node_x, nb_node_y, nb_node_z,
                         first_x,local_nb_node_y,first_y,local_nb_node_y,IK_Node,
                         use_internal_partition);
    } else if(( keyword == "CELL_DATA")||( keyword == "POINT_DATA")) {
      // Maintenant, regarde s'il existe des donnees associees cell aux fichiers
        ok = _readData( mesh,
                            vtk_file,
                            use_internal_partition,
                            nodes_local_id,
                            cells_local_id,
                            buf );
        if(ok) return true ;
    } else {
      error() << "Keyword [" << keyword << "] unknown";
      return true;
    }
  } while( ok );

  // Create face groups for boundary conditions
  // The prefix "BC_" is added to the boundary condition name by default
  // in order to avoid group name collisions
  info() << "Create boundary conditions groups";

  if( mesh->findGroup( "BC_" + tag_zmin ).null() )
    _createFaceGroup(mesh, "BC_" + tag_zmin, zmin_surface_lid);
  else
    error() << "FATAL: Group with reserved name BC_" + tag_zmin
            << " already exists";

  if( mesh->findGroup( "BC_" + tag_zmax ).null() )
    _createFaceGroup(mesh, "BC_" + tag_zmax, zmax_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_zmax ).addItems( zmax_surface_lid );

  if( mesh->findGroup( "BC_" + tag_ymin ).null() )
    _createFaceGroup(mesh, "BC_" + tag_ymin, ymin_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_ymin ).addItems( ymin_surface_lid );

  if( mesh->findGroup( "BC_" + tag_ymax ).null() )
    _createFaceGroup(mesh, "BC_" + tag_ymax, ymax_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_ymax ).addItems( ymax_surface_lid );

  if( mesh->findGroup( "BC_" + tag_xmin ).null() )
    _createFaceGroup(mesh, "BC_" + tag_xmin, xmin_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_xmin ).addItems( xmin_surface_lid );

  if( mesh->findGroup( "BC_" + tag_xmax ).null() )
    _createFaceGroup(mesh, "BC_" + tag_xmax, xmax_surface_lid);
  else
    mesh->findGroup( "BC_" + tag_xmax ).addItems( xmax_surface_lid );

  return false ;
}


void IfpVtkMeshIOService::
_readDistStructuredFacesMesh(IPrimaryMesh* mesh,const String& file_name,const String& dir_name,bool use_internal_partition)
{
  // const char* func_name = "IfpVtkMeshIOService::_readFacesMesh()";
  // ITraceMng* msg = subDomain()->traceMng();

  IParallelMng* pm = subDomain()->parallelMng();
  Integer sid = pm->commRank() ;
  // Si on utilise le partitionneur interne, seul le sous-domaine lit le maillage
  // bool need_read = true;
  // if (use_internal_partition)
  //   need_read = (sid==0);

  ifstream ifile(file_name.localstr());
  if (!ifile){
    info() << "No file description for faces '" << file_name << "'";
    return;
  }
  IfpVtkFile vtk_file(&ifile);
  const char* buf = 0;
  Integer nb_node_x = 0;
  Integer nb_node_y = 0;
  Integer nb_node_z = 0;
  Integer nb_face = 0 ;
  Integer nb_face_node = 0 ;
  if((sid==0) ||(!use_internal_partition))
  {
    {
      buf = vtk_file.getNextLine();
      istringstream iline(buf);
      std::string dimension_str;
      iline >> ws >> dimension_str >> ws >> nb_node_x
            >> ws >> nb_node_y >> ws >> nb_node_z;
      if (!iline){
        error() << "Syntax error in reading grid dimensions";
        return ;
      }
      vtk_file.checkString(dimension_str,"DIMENSIONS");
      if (nb_node_x<0 || nb_node_y<0 || nb_node_z<0 || nb_node_x+nb_node_y+nb_node_z<1){
        error() << "Invalid dimensions: x=" << nb_node_x << " y=" << nb_node_y << " z=" << nb_node_z;
        return ;
      }
    }
    {
      buf = vtk_file.getNextLine();
      istringstream iline(buf);
      std::string faces_str;
      iline >> ws >> faces_str >> ws >> nb_face;
      vtk_file.checkString(faces_str,"FACES");
      if (nb_face<0){
        error() << "Invalid dimensions: number of faces=" << nb_face;
        return ;
      }
    }

    info() << " Infos: " << nb_node_x << " " << nb_node_y << " " << nb_node_z;
    info() << " Number of faces: " << nb_face;
  }

  Integer sub_domain_id = subDomain()->subDomainId();
  Integer nproc = pm->commSize() ;
  Integer rank = pm->commRank() ;

  info()<<" Number of domains "<<nproc;
  info()<<" Rank of current domain "<<rank;
  info()<<" Use internal partitioner :"<<use_internal_partition;

  Integer nb_cell_x = nb_node_x-1;
  Integer nb_cell_y = nb_node_y-1;
  //Integer nb_cell_z = nb_node_z-1;
  Integer first_x = 0 ;
  Integer first_y = 0 ;
  Integer local_nb_node_x = nb_node_x ;
  Integer local_nb_node_y = nb_node_y ;
  Integer local_nb_cell_x = nb_cell_x ;
  Integer local_nb_cell_y = nb_cell_y ;

  if (use_internal_partition)
  {
    if(sub_domain_id!=0)
    {
      nb_node_x = 0;
      nb_node_y = 0;
      nb_node_z = 0;
      nb_cell_x = 0;
      nb_cell_y = 0;
      //nb_cell_z = 0;
      local_nb_node_x = 0;
      local_nb_node_y = 0;
      local_nb_cell_x = 0;
      local_nb_cell_y = 0;
    }
  }
  else
  {
     Integer np1 = 1 ;
     Integer np2 = nproc ;
     _computePartition(nproc,&np1,&np2) ;
    Integer npx = np1 ;
    Integer npy = np2 ;
    if(nb_cell_x>nb_cell_y)
    {
       npx = np2 ;
       npy = np1 ;
    }
    SharedArray<Integer> ix(npx+1) ;
    Integer ni = nb_cell_x/npx ;
    Integer ri = nb_cell_x%npx ;
    ix[0] = 0 ;
    for(Integer i=0;i<ri;i++)
       ix[i+1]=ix[i]+ni+1;
    for(Integer i=ri;i<npx;i++)
       ix[i+1]=ix[i]+ni;
    SharedArray<Integer> jy(npy+1) ;
    Integer nj = nb_cell_y/npy ;
    // Integer rj = nb_cell_y%npy ;
    jy[0] = 0 ;
    for(Integer i=0;i<ri;i++)
       jy[i+1]=jy[i]+nj+1;
    for(Integer i=ri;i<npy;i++)
       jy[i+1]=jy[i]+nj;
    Integer rx = rank%npx ;
    Integer ry = rank/npx ;
    first_x = ix[rx] ;
    first_y = jy[ry] ;
    local_nb_node_x = ix[rx+1]-ix[rx]+1 ;
    local_nb_node_y = jy[ry+1]-jy[ry]+1 ;
    local_nb_cell_x = local_nb_node_x - 1 ;
    local_nb_cell_y = local_nb_node_y - 1 ;
    info()<<"Local nb node x"<<local_nb_node_x;
    info()<<"Local nb node y"<<local_nb_node_y;
    info()<<"Local nb cell x"<<local_nb_cell_x;
    info()<<"Local nb cell y"<<local_nb_cell_y;
    info()<<"NPX NPY "<<npx<<" "<<npy;
    for(Integer i=0;i<npx+1;i++)
       info()<<" ix["<<i<<"] "<<ix[i];
    for(Integer i=0;i<npy+1;i++)
       info()<<" jy["<<i<<"] "<<jy[i];
  }
  // Integer local_nb_node_yz = local_nb_node_y*nb_node_z;
  // Integer local_nb_node_xy = local_nb_node_x*local_nb_node_y;

  info()<<"First X "<<first_x;
  info()<<"First Y "<<first_y;




  SharedArray<Integer> faces_local_id(nb_face);
  if((sid==0)||(!use_internal_partition))
  {
    // Lecture des infos des faces
    // Lecture de la connectivite
    SharedArray<Integer> faces_nb_node;
    SharedArray<Int64> faces_connectivity;
    SharedArray<Integer> faces_type;
    nb_face_node=4*nb_face ;
    faces_nb_node.resize(nb_face);
    faces_type.resize(nb_face);
    faces_connectivity.resize(nb_face_node);
    {
      Integer connectivity_index = 0;
      for( Integer iface=0; iface<nb_face; ++iface )
      {
        Integer i = vtk_file.getInteger();
        Integer j = vtk_file.getInteger();
        Integer k = vtk_file.getInteger();
        Integer l = vtk_file.getInteger();
        if( (i>=first_x)&&(i<first_x+local_nb_cell_x)&&
            (j>=first_y)&&(j<first_y+local_nb_cell_y) )
        {
          if(l<0||l>2) error()<<"l:"<<l<<"out of range";
          faces_nb_node[iface] = 4;
          if(l==0)
            {
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              i++ ;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              j++ ;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              i--;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              ++connectivity_index;
            }
          if(l==1)
            {
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              i++ ;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              k++ ;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              i--;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              ++connectivity_index;
            }
          if(l==2)
            {
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              j++ ;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              k++ ;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              j--;
              ++connectivity_index;
              if(check(i,j,k,nb_node_x,nb_node_y,nb_node_z)) return ;
              faces_connectivity[connectivity_index] = k*(nb_node_x*nb_node_y)+j*nb_node_x+i;
              ++connectivity_index;
            }
        }
        faces_type[iface] = IT_Quad4 ;
      }
    }

    // Il faut a partir de la connectivite retrouver les localId() des faces
    //faces_local_id.resize(nb_face);
    {
      IMeshUtilities* mu = mesh->utilities();
      mu->localIdsFromConnectivity(IK_Face,faces_nb_node,faces_connectivity,faces_local_id);
    }
  }

  // Maintenant, regarde s'il existe des donnees associees aux fichier
  _readFaceData(mesh,vtk_file,use_internal_partition,IK_Face,faces_local_id);
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
