#include "PUMADataWriter.h"
/* Author : pajon at July  2019
 */

#include <arcane/utils/ScopedPtr.h>
#include <arcane/ItemGroup.h>
#include <arcane/ISerializedData.h>
#include <arcane/IData.h>
#include <arcane/SerializeBuffer.h>
#include <arcane/IItemFamily.h>

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/Real3.h>
#include "arcane/utils/List.h"
#include <arcane/utils/OStringStream.h>
#include <arcane/ISubDomain.h>
#include <arcane/ItemGroup.h>
#include <arcane/Directory.h>
#include <arcane/IPostProcessorWriter.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/IVariableMng.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/utils/PlatformUtils.h>
#include <arcane/IRessourceMng.h>
#include <arcane/PostProcessorWriterBase.h>
#include <arcane/Directory.h>

#include "ArcGeoSim/Mesh/Utils/StringToItemUtils.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"

#include "ArcGeoSim/Utils/ArrayUtils.h"
#include "ArcGeoSim/Utils/DataTypeUtils.h"

#include <ctype.h>

#include <map>
//#include <boost/shared_ptr.hpp>

using namespace Arcane;

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

PUMADataWriter::
PUMADataWriter(IMesh* mesh,const String& directory_name,const Integer event_number)
: m_mesh(mesh)
, m_directory_name(directory_name)
, m_trace_mng(m_mesh->subDomain()->traceMng())
, m_ressource_mng(m_mesh->subDomain()->ressourceMng())
, m_io_mng(m_mesh->subDomain()->ioMng())
, m_event_number(event_number)
{
}

/*---------------------------------------------------------------------------*/

PUMADataWriter::
~PUMADataWriter()
{
}


/*---------------------------------------------------------------------------*/

/**********************************************************************
 * [beginWrite]
 **********************************************************************/
void
PUMADataWriter::
beginMeshWrite(const String& file_name){


	/****************************************
	 * Create puma .edo file
	 ****************************************/
	createPUMAFile(file_name);

	/****************************************
	// select PUMA variables
	****************************************/

	IVariableMng * mng = m_mesh->subDomain()->variableMng();

	Arcane::VariableCollection  variables = mng->variables();

	puma_variables.clear();

	for (VariableCollection::Enumerator ivar = variables.enumerator(); ++ivar;)
	    {
	        if ( (*ivar)->name() == "Domain_Permeability_Fluid" ||
	             (*ivar)->name() == "Domain_VolumeFraction_Fluid" ||
	             (*ivar)->name() == "Domain_KrPcRockType_System" )
	        {
	            puma_variables.add(*ivar);
	        }

	    }

}


/**********************************************************************
 * [endWrite]
 **********************************************************************/
void
PUMADataWriter::
endMeshWrite()
{

}


// Ecriture d'un maillage dynamique
/**********************************************************************
 * [meshIncrementElement]
 **********************************************************************/
void
PUMADataWriter::
writeEventToFile() {


    // Retrieve nodes unique ids

    Int64SharedArray nodesUniqueIDs; // Unique nodes-IDs array

    ENUMERATE_NODE(iNode,m_mesh->allNodes()){
        nodesUniqueIDs.add(iNode->uniqueId().asInt64());
    }

    // Retrieve geometry

    SharedArray<Real> nodeValues ;

    VariableItemReal3& nodes_coords =m_mesh->nodesCoordinates();

    SharedArray<Real3> coords;
    coords.reserve(m_mesh->nbNode());

    ENUMERATE_NODE(inode,m_mesh->ownNodes()){
        coords.add(nodes_coords[inode]);
    }

    // PUMA output

    Integer meshNbNodes=m_mesh->nbNode();

    String fullFileName = m_mesh->subDomain()->caseFullFileName();

    cout << "fullFileName = " <<  fullFileName  << "\n";

    ICaseMng* caseMng = m_mesh->subDomain()->caseMng();

    ICaseDocument* caseDocument = caseMng->caseDocument();

    CaseNodeNames* cnn = caseDocument->caseNodeNames();

    XmlNodeList mesh_elems(caseDocument->meshElements());
    if (mesh_elems.empty()){
        m_trace_mng->error() << "No mesh in the input datas";
    }
    Integer nb_mesh = mesh_elems.size();

    if ( nb_mesh > 1){
        m_trace_mng->info() << "Too many meshes";
    }

    XmlNode meshfile_elem = mesh_elems[0].child(cnn->mesh_file);

    String mesh_file = meshfile_elem.value();
    if (mesh_file.null())
        m_trace_mng->error() << "No mesh in the input datas";
    else
        m_trace_mng->info() << "mesh_file = " << mesh_file << "\n";

    Integer fileLenght = mesh_file.len();

    String extension = mesh_file.substring(fileLenght-3 );

    if (extension != "vt2" || extension != "vt2")
        m_trace_mng->error() << "FATAL: wrong extension, only vt2 or vtk is accepted" + extension;

    Integer nb_node_x, nb_node_y, nb_node_z;

    Real3 minimum, maximum;

    //Real3 minimum(0.,0.,-4180);
    //Real3 maximum(2000.0,2022.3258,-4050.0);

    bool retour = _readMeshDimensions(mesh_file, nb_node_x, nb_node_y, nb_node_z, minimum, maximum);

    if (retour) m_trace_mng->error() << "FATAL: the mesh file is not correct";


    m_trace_mng->error() << "nx = " << nb_node_x << "\n";
    m_trace_mng->error() << "ny = " << nb_node_y << "\n";
    m_trace_mng->error() << "nz = " << nb_node_z << "\n";

    m_trace_mng->error() << "z min = " << minimum << "\n";
    m_trace_mng->error() << "z max = " << maximum << "\n";

    Integer nb_cell_x, nb_cell_y, nb_cell_z;

    nb_cell_x = nb_node_x - 1;
    nb_cell_y = nb_node_y - 1;
    nb_cell_z = nb_node_z - 1;

    recursiveWriteFirstLevel(minimum, maximum, nb_cell_x, nb_cell_y, nb_cell_z, meshNbNodes, nodesUniqueIDs, coords);


}

bool
PUMADataWriter::
_readMeshDimensions(const String& file_name, Integer & nb_node_x, Integer & nb_node_y, Integer & nb_node_z, Real3 & minimum, Real3 & maximum)
{
  ifstream ifile(file_name.localstr());
  if (!ifile){
    m_trace_mng->info() << "It is not possible to open file for reading '" << file_name << "'";
    return true;
  }
  IfpVtkFileCopy vtk_file(&ifile);
  const char* buf = 0;
  // Lecture de la description
  String description = vtk_file.getNextLine();
  // Teste si le maillage est 2D en cherchant le mot cl� 2d dans la description vtk. Tout les procs lisent, ne n�cessite
  // pas de synchronisation mais utilise la variable de classe suppl�mentaire mesh_dim. De plus le keyword doit
  // �tre ajout� dans les maillage non strucur�s vtk
  //Integer mesh_dimension = 3;
  //if (description.lower().contains("2d") || description.lower().contains("2 d")) mesh_dimension = 2;
  String format = vtk_file.getNextLine();
  if (not IfpVtkFileCopy::isEqualString(format,"ASCII")){
    m_trace_mng->info() << "Only the 'ASCII' format is recognized (format='" << format << "')";
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
    if (IfpVtkFileCopy::isEqualString(mesh_type_str,"STRUCTURED_GRID")){
      mesh_type = VTK_MT_StructuredGrid;
    }
    if (IfpVtkFileCopy::isEqualString(mesh_type_str,"DIST_STRUCTURED_GRID")){
      mesh_type = VTK_MT_DistStructuredGrid;
    }
    if (IfpVtkFileCopy::isEqualString(mesh_type_str,"STRUCTURED_GRAPH")){
      mesh_type = VTK_MT_StructuredGraph;
    }
    if (IfpVtkFileCopy::isEqualString(mesh_type_str,"STRUCTURED_DUALGRID")){
      mesh_type = VTK_MT_StructuredDualGrid;
    }
    if (IfpVtkFileCopy::isEqualString(mesh_type_str,"UNSTRUCTURED_GRID")){
      mesh_type = VTK_MT_UnstructuredGrid;
    }
    if (mesh_type==VTK_MT_Unknown){
      m_trace_mng->info() << "Only the formats 'STRUCTURED_GRID' and 'UNSTRUCTURED_GRID' are implemented (format=" << mesh_type_str << "')";
      return true;
    }
  }
  bool ret = true;

  if ( mesh_type != VTK_MT_StructuredGrid )
      m_trace_mng->info() << "La grille n'est pas structur�e" << "\n";

  // Lecture du nombre de points: DIMENSIONS nx ny nz
  nb_node_x = 0;
  nb_node_y = 0;
  nb_node_z = 0;

  buf = vtk_file.getNextLine();
  istringstream iline(buf);
  std::string dimension_str;
  iline >> ws >> dimension_str >> ws >> nb_node_x
  >> ws >> nb_node_y >> ws >> nb_node_z;
  if (!iline){
      m_trace_mng->info() << "Syntax error in reading grid dimensions";
      return ret;
  }

  // Lecture Origin et Pas en X, Y et Z

  Real Ox = 0. ;
  Real Oy = 0. ;
  Real Oz = 0. ;
  Real dx = 0. ;
  Real dy = 0. ;
  Real dz = 0. ;

  std::string dx_file_str("Undefined") ;
  std::string dy_file_str("Undefined") ;
  std::string dz_file_str("Undefined") ;

  buf = vtk_file.getNextLine();
  istringstream ilineOrigin(buf);
  std::string origin_str;
  ilineOrigin >> ws >> origin_str ;
  if(IfpVtkFileCopy::isEqualString(origin_str,"ORIGIN"))
  {
      ilineOrigin >> ws >> Ox >> ws >> Oy >> ws >> Oz;
      if (!ilineOrigin){
          m_trace_mng->info() << "Syntax error in reading grid dimensions";
          return true;
      }
      m_trace_mng->info() << "Ox = " << Ox << "\n";
      m_trace_mng->info() << "Oy = " << Oy << "\n";
      m_trace_mng->info() << "Oz = " << Oz << "\n";
  }

  vtk_file.checkString(origin_str,"ORIGIN","TOP");

  buf = vtk_file.getNextLine();
  istringstream ilineDXYZ(buf);
  std::string step_str;
  ilineDXYZ >> ws >> step_str ;
  if(IfpVtkFileCopy::isEqualString(step_str,"DXYZ"))
  {
      ilineDXYZ >> ws >> dx >> ws >> dy >> ws >> dz;
      if (!ilineDXYZ){
          m_trace_mng->info() << "Syntax error in reading grid dimensions";
          return true;
      }
      m_trace_mng->info() << "dx = " << dx << "\n";
      m_trace_mng->info() << "dy = " << dy << "\n";
      m_trace_mng->info() << "dz = " << dz << "\n";
  }
  else if(IfpVtkFileCopy::isEqualString(step_str,"DXY"))
        {
          ilineDXYZ >> ws >> dx >> ws >> dy  ;
          if (!ilineDXYZ){
            cout << "Syntax error in reading grid dimensions";
            return true;
          }
          m_trace_mng->info() << "dx = " << dx << "\n";
          m_trace_mng->info() << "dy = " << dy << "\n";
          buf = vtk_file.getNextLine();
          istringstream iline2(buf);
          std::string stepz_str;
          iline2 >> ws >> stepz_str ;
          if(IfpVtkFileCopy::isEqualString(stepz_str,"DZ"))
            {
              iline2 >> ws >> dz;
              if (!iline2){
                  m_trace_mng->info()  << "Syntax error in reading grid dimensions";
                return true;
              }
              m_trace_mng->info() << "dz = " << dz << "\n";
            }
        }


  minimum[0] = Ox;
  minimum[1] = Oy;
  minimum[2] = Oz;

  maximum[0] = Ox + (nb_node_x-1)*dx;
  maximum[1] = Oy + (nb_node_y-1)*dy;
  maximum[2] = Oz + (nb_node_z-1)*dz;

  return ret;

}

void
PUMADataWriter::
recursiveWriteFirstLevel(const Real3 min, const Real3 max, const Integer ni, const Integer nj, const Integer nk, const Integer meshNbNodes, const Int64SharedArray nodesUniqueIDs, const SharedArray<Real3> coords)
{

    // first grid
    String parentGrid = "\"L0\"";

    Real3 parentSize = max - min;

    parentSize.x /= ni;
    parentSize.y /= nj;
    parentSize.z /= nk;

    Grille grid;
    grid.NXYZ = "NXYZ "+  parentGrid +" =  "+ni +" "+nj +" "+nk;

    grid.SUBSTITUE = "SUBSTITUE";
    grid.DX = "DX "+  parentGrid +  " = " +  parentSize.x;
    grid.DY = "DY "+  parentGrid +  " = " +  parentSize.y;
    grid.DZ = "DZ "+  parentGrid +  " = " +  parentSize.z;
    grid.ZTOP = "ZTOPRES "+  parentGrid +  " = " +  max.z;

    // Properties

    int nbCell = ni*nj*nk;

    RealUniqueArray poros(nbCell);
    RealUniqueArray perm(nbCell);
    Int32UniqueArray zonation(nbCell);

    poros.fill(0);
    perm.fill(0);
    zonation.fill(0);

    Arcane::IItemFamily* cell_family = m_mesh->cellFamily();

    ArcGeoSim::ArrayMng* real_array_mng = new ArcGeoSim::ArrayMngT<Real>(m_mesh->subDomain());
    ArcGeoSim::ArrayMng* int32_array_mng = new ArcGeoSim::ArrayMngT<Int32>(m_mesh->subDomain());

    Int32 countCell = 0;

    String cellName;

    ENUMERATE_CELL(iCell,m_mesh->allCells()){
        Cell cell = *iCell;

        Int32 niveau = cell.level();

        if ( niveau == 0  )
        {
            cellName = Arcane::String::format("{0}_{1}",m_event_number,cell.localId());

            Arcane::ItemGroup cellGroup = cell_family->createGroup(cellName);

            Arcane::Int64UniqueArray uids(1);
            uids[0] = cell.uniqueId();
            Arcane::IntegerUniqueArray lids(1);
            cell_family->itemsUniqueIdToLocalId(lids, uids);
            cellGroup.addItems(lids);

            cell_family->endUpdate();

            cout << "nombre de puma_variables =" << puma_variables.count() << "\n";

            for (VariableCollection::Enumerator ivar = puma_variables.enumerator(); ++ivar;)
            {
                //cout << "Variable =" << (*ivar)->name() << "\n";

                ArcGeoSim::VariableUtils::VariableView variable_view(*ivar);

                //ArcGeoSim::ArrayMng* array_mng;
                ArcGeoSim::VariableUtils::GenericArray generic_values;
                SharedArray<Real> values;
                SharedArray<Int32> intvalues;

                switch ((*ivar)->dataType()) {
                case DT_Real:
                    //array_mng = new ArcGeoSim::ArrayMngT<Real>(m_mesh->subDomain());
                    generic_values =ArcGeoSim::VariableUtils::GenericArray(real_array_mng);
                    generic_values = variable_view[cellGroup];
                    values = SharedArray<Real>(generic_values);
                    //for (int i=0;i<values.length();i++)
                    //    cout << "Array of Values " << values[i] << "\n";
                    if ( (*ivar)->name() == "Domain_Permeability_Fluid")
                    {
                        perm[countCell] = values[0];
                        //realproperties.at("PERM")[countCell] = values[0];
                    }
                    else
                        //if ( (*ivar)->name() == "Domain_VolumeFraction_Fluid")
                    {
                        poros[countCell] = values[0];
                        //realproperties.at("POROS")[countCell] = values[0];
                    }
                    break;
                case DT_Int32:
                    //array_mng = new ArcGeoSim::ArrayMngT<Int32>(m_mesh->subDomain());
                    generic_values =ArcGeoSim::VariableUtils::GenericArray(int32_array_mng);
                    generic_values = variable_view[cellGroup];
                    intvalues = SharedArray<Int32>(generic_values);
                    //for (int i=0;i<intvalues.length();i++)
                    //    cout << "Array of Values " << intvalues[i] << "\n";
                    zonation[countCell] = intvalues[0];
                    break;
                default:
                    cout << "Bad variable type" << "\n";
                    break;
                }
            }
            countCell++;
        }
    }

    delete real_array_mng;
    delete int32_array_mng;

    grid.poros = poros;
    grid.perm = perm;
    grid.zonation = zonation;

    map<String,RealUniqueArray> realproperties;
    realproperties.clear();

    realproperties.insert ( std::pair<String,RealUniqueArray>("POROS",poros) );
    realproperties.insert ( std::pair<String,RealUniqueArray>("PERM",perm) );

    grid.realproperties = realproperties;

    map_subgrids.clear();

    map_subgrids.insert ( std::pair<String,Grille>(parentGrid,grid) );

    m_dx[0] = parentSize.x;
    m_dy[0] = parentSize.y;
    m_dz[0] = parentSize.z;

    m_count_subgrid[0] =1 ;

    for (Integer i=1;i<8;i++)
    {
        m_count_subgrid[i] = 0;
        m_dx[i] = m_dx[i-1]/2.0;
        m_dy[i] = m_dy[i-1]/2.0;
        m_dz[i] = m_dz[i-1]/2.0;
    }

    ENUMERATE_CELL(iCell,m_mesh->allCells()){
        Cell cell = *iCell;

        Int32 niveau = cell.level();

        if ( niveau == 0 && cell.nbHChildren() > 1 && cell.isAncestor())
        {
            // compute mean

            Real3 mean(0.,0.,0.);

            Integer  nbNodes = cell.nbNode();

            for(Integer j=0; j<nbNodes;++j){
                Int64 uid=cell.node(j).uniqueId().asInt64();
                for(Integer i=0; i<meshNbNodes; ++i){
                    if (nodesUniqueIDs[i] != uid) continue;
                    //cout << " Node =  "  << i << " " << coords[i] << "\n";
                    mean += coords[i];
                    break;
                }

            }

            mean /= 8;

            // find position
            Integer i=0, j=0, k=0;

            findPosition(min, max, ni, nj, nk , mean, i, j, k);

            m_count_subgrid[1] +=1 ;
            Real zCellTop = min.z + (k+1)*(max.z - min.z)/nk;
            recursiveWrite(parentGrid, parentSize, zCellTop, cell, 1, i, j, k, meshNbNodes, nodesUniqueIDs, coords);
        }

    }

    ofstream myfile;

    String nameFile = String::format("{0}/{1}_{2}.edo",m_output_directory,m_output_file,m_event_number);

    const char* name = nameFile.localstr();
    myfile.open (name);

    // NXYZ
    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->second.NXYZ << endl;
    }

    //SUBSTITUE
    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->second.SUBSTITUE << endl;
    }

    // DX
    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->second.DX << endl;
    }

    // DY
    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->second.DY << endl;
    }

    // DZ
    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->second.DZ << endl;
    }


    // ZTOPRES
    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->second.ZTOP << endl;
    }

    myfile << "MATGRID    " << "\n";

    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << i->first << endl;
        }

    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << "PERMX     " << i->first << " = " << "\n";
        RealUniqueArray array = (RealUniqueArray) (i->second.perm);
        //RealUniqueArray array = (RealUniqueArray) (i->second.realproperties.at("PERM"));
        for (int j = 0; j < array.length(); j++)
            myfile << array[j] << endl;
    }

    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << "PERMY     " << i->first << " = " << "\n";
        RealUniqueArray array = (RealUniqueArray) (i->second.perm);
        //RealUniqueArray array = (RealUniqueArray) (i->second.realproperties.at("PERM"));
        for (int j = 0; j < array.length(); j++)
            myfile << array[j] << endl;
    }

    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << "POROS     " << i->first << " = " << "\n";
        RealUniqueArray array = (RealUniqueArray) (i->second.poros);
        //RealUniqueArray array = (RealUniqueArray) (i->second.realproperties.at("POROS"));
        for (int j = 0; j < array.length(); j++)
            myfile << array[j] << endl;
    }

    for(map<String,Grille>::iterator i=map_subgrids.begin(); i!=map_subgrids.end(); ++i) {
        myfile << "KRPC-CELL     " << i->first << " = "<< "\n";
        Int32SharedArray array = (Int32SharedArray) (i->second.zonation);
        for (int j = 0; j < array.length(); j++)
            myfile << array[j] << endl;
    }

    myfile.close();

    // statistiques

    cout << "nb Total Grids  =     " << map_subgrids.size() << "\n";

    Integer nbTotalGrids = 0;

    for (Integer i=0;i<8;i++)
    {
        cout << "Level " << i << " nb sous-grilles = " << m_count_subgrid[i] << "\n";
        nbTotalGrids += m_count_subgrid[i];
    }

    cout << "nbTotalGrids  =     " << nbTotalGrids << "\n";


}

void
PUMADataWriter::
_getArraySize(Arcane::IVariable * var, Arcane::Integer& array_size)
{
  array_size = 1;
  if (var->itemGroup().null()) {// global variable
      array_size = var->nbElement();
    }
  else {// mesh variable
      if (var->dimension() == 2) {
          Integer nb_items;
          ArcGeoSim::VariableUtils::meshVariableArraySize(var->data(),nb_items,array_size);
      }
  }
}


void
PUMADataWriter::
recursiveWrite(const String parentGrid, Real3 & parentSize, const Real cellZTop, const Cell cell, const Integer level, const Integer i, const Integer j, const Integer k, const Integer meshNbNodes, const Int64SharedArray nodesUniqueIDs, const SharedArray<Real3> coords)
{
    // create name of the current grid

    Integer iplus1 = i+1;
    Integer jplus1 = j+1;
    Integer kplus1 = k+1;

   String currentGrid;

   if ( level >1 )
   m_count_subgrid[level] +=1 ;

    if ( level == 1)
       currentGrid = String::format("\"L{0}-{1}-{2}-{3}\"",level,iplus1,jplus1,kplus1);
    else
    {
        //Integer uidSubGrid =  4*k +  2*j  + i + 1;
        //currentGrid = String::format("\"L{0}-E{1}-A{2}\"",level,m_count_subgrid[level],uidSubGrid);
        currentGrid = String::format("\"L{0}-E{1}\"",level,m_count_subgrid[level]);
    }

    String position = String::format("{0} {1} {2}",iplus1,jplus1,kplus1);

    Grille grid;

    // store NXNYNZ
    grid.NXYZ = "; "+  currentGrid +  " = 2 2 2";

    // store SUBSTITUE
    grid.SUBSTITUE = "; "+  currentGrid +  " = " +  parentGrid + "  " + position;

    // store DX, DY,DZ;
    grid.DX = "; "+  currentGrid +  " = " +  m_dx[level];
    grid.DY = "; "+  currentGrid +  " = " +  m_dy[level];
    grid.DZ = "; "+  currentGrid +  " = " +  m_dz[level];

    // alternative way to do the same thing
    //grid.DX = "; "+  currentGrid +  " = " +  parentSize.x/2.0;  // it works also
    //grid.DY = "; "+  currentGrid +  " = " +  parentSize.y/2.0;
    //grid.DZ = "; "+  currentGrid +  " = " +  parentSize.z/2.0;

    grid.ZTOP = "; "+  currentGrid +  " = " +  cellZTop;

    // Properties

    RealUniqueArray poros(8);
    RealUniqueArray perm(8);
    Int32UniqueArray zonation(8);

    poros.fill(0);
    perm.fill(0);
    zonation.fill(0);

    map<String,RealUniqueArray> realproperties;
    realproperties.clear();

    realproperties.insert ( std::pair<String,RealUniqueArray>("POROS",poros) );
    realproperties.insert ( std::pair<String,RealUniqueArray>("PERM",perm) );

    Arcane::IItemFamily* cell_family = m_mesh->cellFamily();

    ArcGeoSim::ArrayMng* real_array_mng = new ArcGeoSim::ArrayMngT<Real>(m_mesh->subDomain());
    ArcGeoSim::ArrayMng* int32_array_mng = new ArcGeoSim::ArrayMngT<Int32>(m_mesh->subDomain());

    String cellName;

    for (Integer countChild=0;countChild<cell.nbHChildren();countChild++)
    {
        Cell cellChild = cell.hChild(countChild);

        cellName = String::format("{0}_{1}",m_event_number,cellChild.uniqueId());

        Arcane::ItemGroup cellGroup = cell_family->createGroup(cellName);

        Arcane::Int64UniqueArray uids(1);
        uids[0] = cellChild.uniqueId();
        Arcane::IntegerUniqueArray lids(1);
        cell_family->itemsUniqueIdToLocalId(lids, uids);
        cellGroup.addItems(lids);

        cell_family->endUpdate();


        for (VariableCollection::Enumerator ivar = puma_variables.enumerator(); ++ivar;)
        {
            //cout << "Variable =" << (*ivar)->name() << "\n";

            ArcGeoSim::VariableUtils::VariableView variable_view(*ivar);

            //ArcGeoSim::ArrayMng* array_mng;
            ArcGeoSim::VariableUtils::GenericArray generic_values;
            SharedArray<Real> values;
            SharedArray<Int32> intvalues;

            switch ((*ivar)->dataType()) {
            case DT_Real:
                //array_mng = new ArcGeoSim::ArrayMngT<Real>(m_mesh->subDomain());
                generic_values =ArcGeoSim::VariableUtils::GenericArray(real_array_mng);
                generic_values = variable_view[cellGroup];
                values = SharedArray<Real>(generic_values);
                //for (int i=0;i<values.length();i++)
                //    cout << "Array of Values " << values[i] << "\n";
                if ( (*ivar)->name() == "Domain_Permeability_Fluid" )
                {
                    perm[countChild] = values[0];
                }
                else
                    //if ( (*ivar)->name() == "Domain_VolumeFraction_Fluid")
                {
                    poros[countChild] = values[0];
                }
                break;
            case DT_Int32:
                //array_mng = new ArcGeoSim::ArrayMngT<Int32>(m_mesh->subDomain());
                generic_values =ArcGeoSim::VariableUtils::GenericArray(int32_array_mng);
                generic_values = variable_view[cellGroup];
                intvalues = SharedArray<Int32>(generic_values);
                //for (int i=0;i<intvalues.length();i++)
                //    cout << "Array of Values " << intvalues[i] << "\n";
                zonation[countChild] = intvalues[0];
                break;
            default:
                cout << "Bad variable type" << "\n";
                break;
            }
        }
    }

    delete real_array_mng;
    delete int32_array_mng;

    grid.poros = poros;
    grid.perm = perm;
    grid.zonation = zonation;

    realproperties.insert ( std::pair<String,RealUniqueArray>("POROS",poros) );
    realproperties.insert ( std::pair<String,RealUniqueArray>("PERM",perm) );
    grid.realproperties = realproperties;
    map_subgrids.insert ( std::pair<String,Grille>(currentGrid,grid) );

    //cout << "level i j k " << level << " " << i << " " << j << " " << k << "\n" ;

    // write info for children
    //if ( cell.nbHChildren() > 1 )
    //{

        for (Integer index=0;index<cell.nbHChildren();index++)
        {
            Cell newChild = cell.hChild(index);

            Integer i0=0, j0=0, k0=0;

            Real3 currentSize(0.,0.,0.);

            Real zTop;

            findIndices(cell, newChild, currentSize, zTop, i0, j0, k0, meshNbNodes, nodesUniqueIDs, coords);

            //cout << "currentGrid = " << currentGrid << "\n" ;
            //cout << "parentSize = " << parentSize/2. << "\n" ;
            //cout << "currentSize = " << currentSize << "\n";
            //cout << "Level = " << level << "  m_dx[level] = " << m_dx[level] << "\n";
            //cout << "Level = " << level << "  m_dy[level] = " << m_dy[level] << "\n";
            //cout << "Level = " << level << "  m_dz[level] = " << m_dz[level] << "\n\n";

            recursiveWrite(currentGrid, currentSize, zTop, newChild, level+1, i0, j0, k0, meshNbNodes, nodesUniqueIDs, coords);

        }

    //}

}

void
PUMADataWriter::
findIndices(const Cell cell0, const Cell cell1,  Real3 & size, Real & zTop, Integer & i, Integer & j, Integer & k, const Integer meshNbNodes, const Int64SharedArray nodesUniqueIDs, const SharedArray<Real3> coords)
{
    // first cell minimum ( cell is supposed to have 8 corners)

    Real3 minimum;

    Int64 uid=cell0.node(0).uniqueId().asInt64();
    for(Integer index=0; index<meshNbNodes; ++index){
        if (nodesUniqueIDs[index] != uid) continue;
        minimum = coords[index];
        break;
    }

    // first cell maximum ( cell is supposed to have 8 corners)

    Real3 maximum;

    uid=cell0.node(6).uniqueId().asInt64();
    for(Integer index=0; index<meshNbNodes; ++index){
        if (nodesUniqueIDs[index] != uid) continue;
        maximum = coords[index];
        break;
    }

    // second cell
    Real3 mean(0.,0.,0.);

    Integer  nbNodes = cell1.nbNode();

    for(Integer iNode=0; iNode<nbNodes;++iNode){
        Int64 uid=cell1.node(iNode).uniqueId().asInt64();
        for(Integer index=0; index<meshNbNodes; ++index){
            if (nodesUniqueIDs[index] != uid) continue;
            mean += coords[index];
            break;
        }

    }

    mean /= 8;

    // find position

    findPosition(minimum, maximum, 2, 2, 2, mean, i, j, k);

    size = (maximum - minimum)/2.;

    zTop = minimum.z + (k+1)*size.z;

}


void
PUMADataWriter::
findPosition(const Real3 min, const Real3 max, const Integer ni , const Integer nj ,const Integer nk ,const Real3 coord, Integer & i, Integer & j, Integer & k)
{
    //Real3 totalSize =  max - min;

    Real3 cellSize;

    cellSize.x = (max.x - min.x)/ni;
    cellSize.y = (max.y - min.y)/nj;
    cellSize.z = (max.z - min.z)/nk;

    Real3 position = (coord - min)/cellSize;

    i = (Integer) position.x;
    j = (Integer) position.y;
    k = (Integer) position.z;


}

void
PUMADataWriter::
beginWrite(const VariableCollection &vars)
{
	//if (m_write_initial_grid_properties) m_initial_grid_properties_node = XmlElement(m_initial_grid,"Attribute");

	//m_current_grid_properties_node = XmlElement(m_current_grid,"Attribute");
}

/*---------------------------------------------------------------------------*/

void
PUMADataWriter::
write(Arcane::IVariable* ivar, Arcane::IData* data)
{

}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/**********************************************************************
 * [createPUMAFile]
 **********************************************************************/
void
PUMADataWriter::
createPUMAFile(const String& file_name)
{
	// output file name

	Arcane::SharedArray<String> temp_array;

	Arcane::String outputfilename;

	// use m_directory_name instead of ArcGeoSim::getOuputRootPath()
    // but problem in ArcGeoSimPostProcessorBase init method
	m_output_directory = ArcGeoSim::getOuputRootPath() + "/" +platform::getFileDirName(file_name);

	Arcane::SharedArray<String> small_array;

	file_name.split(small_array,'/');

	small_array.back().split(temp_array,'.');

	m_output_base_file = temp_array[0];

	outputfilename = temp_array[0]+".edo";

	m_trace_mng->info() << " output directory = " <<  m_output_directory ;

	m_trace_mng->info() << " output file = " <<  outputfilename ;

	// create directory if necessary

	platform::recursiveCreateDirectory(m_output_directory);

	// init m_output_file

	Arcane::StringSharedArray array;
	//file_name.split(array,'.');
	outputfilename.split(array,'.');

	m_output_file=array[0];

	m_trace_mng->info() << " output file = " <<  m_output_file ;

}



/**********************************************************************
 * [baseFileName]
 **********************************************************************/
const String
PUMADataWriter::
baseFileName() const
{
	// File name
	return String::format("{0}/{1}",m_output_directory,m_output_file);
}

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

const char* IfpVtkFileCopy::
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
    throw IOException("IfpVtkFileCopy::getNexLine()","Unexpected EndOfFile");
  return NULL;
}


const char* IfpVtkFileCopy::
getNextLine()
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
  return NULL;
}

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

Real IfpVtkFileCopy::
getReal()
{
  Real v = 0.;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("IfpVtkFileCopy::getReal()","Bad Real");
}

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

Integer IfpVtkFileCopy::
getInteger()
{
  Integer v = 0;
  (*m_stream) >> ws >> v;
  if (m_stream->good())
    return v;
  throw IOException("IfpVtkFileCopy::getInteger()","Bad Integer");
}

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

void IfpVtkFileCopy::
checkString(const String& current_value,const String& expected_value)
{
  StringBuilder current_value_low(current_value);
  current_value_low.toLower();
  StringBuilder expected_value_low(expected_value);
  expected_value_low.toLower();

  if (current_value_low.toString()!=expected_value_low.toString()){
    String s = "Wait for string '" + expected_value + "', found '" + current_value + "'";
    throw IOException("IfpVtkFileCopy::checkString()",s);
  }
}

void IfpVtkFileCopy::
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
    throw IOException("IfpVtkFileCopy::checkString()",s);
  }
}

void IfpVtkFileCopy::
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
    throw IOException("IfpVtkFileCopy::checkString()",s);
  }
}

bool IfpVtkFileCopy::
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
/*---------------------------------------------------------------------------*/
