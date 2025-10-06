#ifndef ARCGEOSIM_NUMERICS_EXPRESSIONS_UNSTEADYREGULARGRIDDATA_UNSTEADYREGULARGRIDDATA_H
#error "Template implementation has to be used with template declaration"
#endif

#ifdef WIN32
#include <ciso646>
#endif

#include <arcane/ArcaneVersion.h>
#include <arcane/IXmlDocumentHolder.h>
#include <arcane/utils/ValueConvert.h>
#include <arcane/DomUtils.h>
#include <arcane/XmlNode.h>
#include <arcane/XmlNodeList.h>
#include "ArcGeoSim/Numerics/Expressions/UnsteadyRegularGridData/regular_non_uniform_grid_data_xsd.h"
#include "ArcGeoSim/Numerics/Expressions/UnsteadyRegularGridData/regular_uniform_grid_data_xsd.h"
#include "ArcGeoSim/Numerics/Expressions/UnsteadyRegularGridData/XDMFReaderTools.h"

//#define BAD_ORDERING

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::init()
{
  // After init() the table is ready for use

  //! Check Table is filled : table must be filled before init()
  //!         done either by the Service (points filename in the service options) :
  //!               MyUnsteadyGridDataService->init()
  //!       either manually :
  //!               UnsteadyGridDataT<MyInterpolator> my_unsteady_grid_data(traceMng(),ioMng());
  //!               my_unsteady_grid_data.setPoints(filename)
  //!               my_unsteady_grid_data.init()
  if (m_initialized)
    return;
  if (!m_filled)
    m_trace_mng->fatal()
        << "Cannot initialize function before table filled (function.setPoints(filename))";
  m_initialized = true;
}

/*---------------------------------------------------------------------------*/

template<typename T>
UnsteadyRegularGridDataT<T>::
UnsteadyRegularGridDataT(ITraceMng* trace_mng, IIOMng* io_mng)
  : m_trace_mng(trace_mng)
  , m_io_mng(io_mng)
  , m_initialized(false)
  , m_filled(false)
  , m_variable_initialized(false)
  , m_variable_empty(true)
  , m_variable_component_index(0)
  , m_variable_name(4)
  , m_schema_name("non_uniform_grid_xsd")
  , m_grid_type(RegularGridType::NonUniformGrid)
  , m_grid_size(4)
{
  m_variable.resize(4);
  // default constructor code here
}

/*---------------------------------------------------------------------------*/

template<typename T>
RegularGridType::GridDimension
UnsteadyRegularGridDataT<T>::
setPoints(const String& file_name)
{

  if (m_filled)
    return m_grid_dimension;

  m_trace_mng->debug(Trace::High) << "Setting points from file " << file_name;
  m_filename = file_name;

  //
  if ( m_filename.endsWith(".xml"))
	  _readInterpolationPoints();
  else if ( m_filename.endsWith(".xmf"))
	  m_grid_dimension = _readInterpolationPoints_XDMF();
  else
	  m_trace_mng->fatal() << "Undefined format for file '" << m_filename << "'";

  m_filled = true;

  return m_grid_dimension;
}

/*---------------------------------------------------------------------------*/

template<typename T>
String
UnsteadyRegularGridDataT<T>::
variableName(Integer index) const
{
  if (index > 3 or !m_variable_initialized)
    m_trace_mng->fatal()
        << "Incoherent index or variable not initialized. In variableName(Integer)";
  return m_variable_name[index];
}


/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
setVariable(const String & name, const Array<Real> & variable)
{
	if (m_variable_initialized)
		m_trace_mng->fatal() << "Variable already initialized";
	// Check size coherence
	if (!m_variable_empty and variable.size() != m_variable_size)
		m_trace_mng->fatal() << "Size error in variable initialization";
	m_variable_size = variable.size();
	// set variable component name
	_setVariableName(m_variable_component_index, name);
	// Add variable view in m_variable
	m_variable[m_variable_component_index].setArray(variable);
	// Update m_variable tools
	m_variable_empty = false;
	++m_variable_component_index;
	if (m_variable_component_index == 4)
		m_variable_initialized = true;
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
setVariable(Integer index, const Array<Real> & variable)
{
	if (m_variable_initialized)
		m_trace_mng->fatal() << "Variable already initialized";
	// Check size coherence
	if (!m_variable_empty and variable.size() != m_variable_size)
		m_trace_mng->fatal() << "Size error in variable initialization";
	m_variable_size = variable.size();
	// Add variable view in m_variable
	m_variable[index].setArray(variable);
	// Update m_variable tools
	m_variable_empty = false;
	++m_variable_component_index;
	if (m_variable_component_index == 4)
		m_variable_initialized = true;
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
setEvaluationResult(Array<Real> & result)
{
	m_result.setArray(result);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
eval()
{
  //! Check if variable defined (all components filled with the same size)
  if (!m_variable_initialized)
    m_trace_mng->fatal() << "variable not fully initialized";

  const Integer size = m_variable_size;
  if (size != m_result.size())
    m_trace_mng->fatal() << "Inconsistent size in eval()";

  for (Integer i = 0; i < size; ++i)
    {
      m_result[i] = eval(m_variable[0][i], m_variable[1][i], m_variable[2][i], m_variable[3][i]);
    }
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
cleanup()
{
  m_variable[0] = RealConstArrayView();
  m_variable[1] = RealConstArrayView();
  m_variable[2] = RealConstArrayView();
  m_variable[3] = RealConstArrayView();
  m_variable_empty = true;
  m_variable_initialized = false;
  m_variable_component_index = 0;
  m_result = RealArrayView();
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
eval(const Array<Real> & var0, const Array<Real> & var1,
		const Array<Real> & var2, const Array<Real> & var3,
		Array<Real> & res0)
		{
	// Set variable
	setVariable(0, var0);
	setVariable(1, var1);
	setVariable(2, var2);
	setVariable(3, var3);
	// Set Result
	setEvaluationResult(res0);
	// Eval
	eval();
	cleanup();
		}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
eval(const Real & x, const Real & y,
     const Real & z, const Real & t, Real & res0)
{
  // eval res0
  res0 = eval(x, y, z, t);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
eval(const ConstArrayView<Real> var0, const ConstArrayView<Real> var1,
     const ConstArrayView<Real> var2, const ConstArrayView<Real> var3,
     ArrayView<Real> res0)
{
  // Set Variable
  if (var0.size() != var1.size() or var1.size() != var2.size() or var2.size()
      != var3.size())
    {
      m_trace_mng->fatal() << "Size error in variable initialization";
    }
  m_variable[0] = var0;
  m_variable[1] = var1;
  m_variable[2] = var2;
  m_variable[3] = var3;
  m_variable_size = var0.size();
  m_variable_initialized = true;
  // Set result
  m_result = res0;
  // eval and clean
  eval();
  cleanup();
}

/*---------------------------------------------------------------------------*/

template<typename T>
inline
Real
UnsteadyRegularGridDataT<T>::
eval(const Real & x, const Real & y,
     const Real & z, const Real & t)
{
  ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

  // create structure storing closer point index
  PointIndex closer_point_index(4);
  // Store (x,y,z,t) in Point structure
  UnsteadyGridPoint
  point_value(x, y, z, t);
  // find closer point index
  _findCloserPoint(point_value(), closer_point_index);
  // interpolate and return interpolation
  //
 // Real value = m_property[PropertyIndexManager::computePropertyIndex(0,0,0,0,m_grid_size)];
 // cout << " Value (0, 0, 0, 0) = " << value << "/n";
  
 // value = m_property[PropertyIndexManager::computePropertyIndex(m_grid_size[0]-1,0,0,0,m_grid_size)];
 // cout << " Value (ni-1, 0, 0, 0) = " << value << "/n";
  
 //  value = m_property[PropertyIndexManager::computePropertyIndex(0,m_grid_size[1]-1,0,0,m_grid_size)];
 // cout << " Value (0, nj-1, 0, 0) = " << value << "/n";

 //   value = m_property[PropertyIndexManager::computePropertyIndex(m_grid_size[0]-1,m_grid_size[1]-1,0,0,m_grid_size)];
 //   cout << " Value (ni-1, nj-1, 0, 0) = " << value << "/n";
   
 //   value = m_property[PropertyIndexManager::computePropertyIndex(0,0,m_grid_size[2]-1,0,m_grid_size)];
 //   cout << " Value (0, 0, nk-1, 0) = " << value << "/n";
  
 //   value = m_property[PropertyIndexManager::computePropertyIndex(m_grid_size[0]-1,0,m_grid_size[2]-1,0,m_grid_size)];
 //   cout << " Value (ni-1, 0, nk-1, 0) = " << value << "/n";
  
 //   value = m_property[PropertyIndexManager::computePropertyIndex(0,m_grid_size[1]-1,m_grid_size[2]-1,0,m_grid_size)];
 //   cout << " Value (0, nj-1, nk-1, 0) = " << value << "/n";
  
 //   value = m_property[PropertyIndexManager::computePropertyIndex(m_grid_size[0]-1,m_grid_size[1]-1,m_grid_size[2]-1,0,m_grid_size)];
 //   cout << " Value (ni-1, nj-1, nk-1, 0) = " << value << "/n";
  

  
  return T::interpolate(point_value(), closer_point_index, m_x, m_y, m_z,
      m_t, m_property, m_grid_size);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
_findCloserPoint(const Array<Real>& point_value,
                 PointIndex& closer_point_index)
{
  // Scan in each direction
  closer_point_index[0] = _scan(point_value[0], m_x.size(), &m_x[0]);
  closer_point_index[1] = _scan(point_value[1], m_y.size(), &m_y[0]);
  closer_point_index[2] = _scan(point_value[2], m_z.size(), &m_z[0]);
  closer_point_index[3] = _scan(point_value[3], m_t.size(), &m_t[0]);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
_readInterpolationPoints()
{
  // Read grid file
  switch (m_grid_type)
  {
  case RegularGridType::UniformGrid:
    _readUniformGrid();
    break;
  case RegularGridType::NonUniformGrid:
    _readNonUniformGrid();
    break;
  default:
    m_trace_mng->fatal() << "Failure in reading regular grid data file "
    << m_filename;
    break;
  }
  // Store sizes
  m_grid_size[0] = m_x.size();
  m_grid_size[1] = m_y.size();
  m_grid_size[2] = m_z.size();
  m_grid_size[3] = m_t.size();
}


/*---------------------------------------------------------------------------*/

template<typename T>
RegularGridType::GridDimension
UnsteadyRegularGridDataT<T>::
_readInterpolationPoints_XDMF()
{

  RegularGridType::GridDimension gridDimension = RegularGridType::GridDimension::eUndefined;

  m_mesh_file_directory_name = Arcane::platform::getFileDirName(m_filename);

  m_trace_mng->debug() << "m_mesh_file_directory_name " << m_mesh_file_directory_name;

  // Read grid file
  switch (m_grid_type)
  {
  case RegularGridType::UniformGrid:
    _readGrid_XDMF();
    break;
  case RegularGridType::NonUniformGrid:
	  gridDimension = _readGrid_XDMF();
    break;
  default:
    m_trace_mng->fatal() << "Failure in reading regular grid data file "
    << m_filename;
    break;
  }
  // Store sizes
  m_grid_size[0] = m_x.size();
  m_grid_size[1] = m_y.size();
  m_grid_size[2] = m_z.size();
  m_grid_size[3] = m_t.size();

  return gridDimension;
}

/*---------------------------------------------------------------------------*/

template<typename T>
SharedArray<Real>
UnsteadyRegularGridDataT<T>::
transposeData(const SharedArray<Real> & values, int dimX, int dimY, int dimZ)
{
 if (dimX*dimY*dimZ != values.size())
   m_trace_mng->fatal() << "Incompatible size for data transposition";

#ifndef BAD_ORDERING
  SharedArray<Real> transposed_values(dimX*dimY*dimZ);

  auto col_major_mapper = [dimX,dimY,dimZ](int i, int j, int k) -> int { return k*(dimX*dimY)+j*(dimX)+i; };
  auto row_major_mapper = [dimX,dimY,dimZ](int i, int j, int k) -> int { return i*(dimY*dimZ)+j*(dimZ)+k; };

  for(int i=0;i<dimX;++i)
    for(int j=0;j<dimY;++j)
      for(int k=0;k<dimZ;++k)
        transposed_values[col_major_mapper(i,j,k)] = values[row_major_mapper(i,j,k)];

  return transposed_values;
#else
  return values;
#endif
}

/*---------------------------------------------------------------------------*/

template<typename T>
Integer
UnsteadyRegularGridDataT<T>::
_scan(const Real& v, const Integer& n,
      const Real * const vptr) const
{
  // Suppose que
  // * n > 0 (nombre d'element de vptr
  // * les valeurs de vptr sont ordonn�es croissantes
  // * v et la valeur maximale ne sont pas numeric_limits<Integer_type>::max()

  // Additional treatment to allow degenerate direction : only one point
  if ((n == 1))
    return 0;

  //! Prepare
  Integer ileft = 0;
  Integer iright = n; //
  Integer n_linear = 20; // threshold to switch from dichotomic to linear scan

  // check out of range value
  //if (v > vptr[n - 1] or v < vptr[0])
  //  m_trace_mng->fatal() << "Interpolated point out of table";
  
  if ( v < vptr[0] ) 
  	return - 1;
      
  if ( v > vptr[n - 1] ) 
  	return  n;

  // deal with last point
  if (vptr[n - 1] == v)
    {
      return n - 1;
    }

  //! Start dichotomy
  while (iright - ileft > n_linear)
    {
      const Integer imid = (iright + ileft) / 2;
      const Real vmid = vptr[imid];
      if (v < vmid)
        {
          iright = imid;
        }
      else
        {
          ileft = imid;
        }
    }

  //! Switch to linear search
  while (vptr[ileft + 1] <= v)
    {
      ++ileft;
    }

  return ileft;
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
printGrid() const
{
  if (!m_initialized) m_trace_mng->fatal() << "Unsteady Regular Grid not initialized. Cannot print.";
  else
    {
      m_trace_mng->debug(Trace::Highest) << "UnsteadyRegularGridData X direction points: ";
      m_trace_mng->debug(Trace::Highest) << m_x;
      m_trace_mng->debug(Trace::Highest) << "UnsteadyRegularGridData Y direction points: ";
      m_trace_mng->debug(Trace::Highest) << m_y;
      m_trace_mng->debug(Trace::Highest) << "UnsteadyRegularGridData Z direction points: ";
      m_trace_mng->debug(Trace::Highest) << m_z;
      m_trace_mng->debug(Trace::Highest) << "UnsteadyRegularGridData T direction points: ";
      m_trace_mng->debug(Trace::Highest) << m_t;
      m_trace_mng->debug(Trace::Highest) << "UnsteadyRegularGridData Property: ";
      m_trace_mng->debug(Trace::Highest) << m_property;
    }
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
_setVariableName(Integer n, const String & name)
{
  m_variable_name[n] = name;
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
setRegularGridType(const RegularGridType::eRegularGridType& grid_type)
{
  m_grid_type = grid_type;
  switch (m_grid_type)
    {
  case RegularGridType::UniformGrid:
    m_schema_name = "uniform_grid.xsd";
    break;
  case RegularGridType::NonUniformGrid:
    m_schema_name = "non_uniform_grid.xsd";
    break;
    }
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
_readNonUniformGrid()
{
  // file format:
  // <x-value>list_of_x_values</x-value>
  // <y-value>list_of_y_values</y-value>
  // <z-value>list_of_z_values</z-value>
  // <t-value>list_of_time_value</t_value>
  // <property-value>list_of_property_value</property_value>

  /* Exemple of sequential read using xsd schema
   * IXmlDocumentHolder * xmlDoc = m_io_mng->parseXmlFile(m_filename,
   *       m_schema_name, ByteConstArrayView(strlen((const char*)non_uniform_grid_xsd), non_uniform_grid_xsd));
   *
   */

  /* Parallel read : parse XML buffer :
   * use of xsd scheme not yet available in parseXmlBuffer
   * use xsd file regular_uniform_grid_data.xsd
   * To update when available (cf warning)
   */
#ifdef NO_USER_WARNINGS
#warning "TODO : use xsd scheme in parallel as soon as available in arcane IIOMng::parseXmlBuffer"
#endif
  // Collective read
  ByteSharedArray xmlBuffer;
  bool error_collective_read =
      m_io_mng->collectiveRead(m_filename, xmlBuffer);
  // Check error
  if (error_collective_read)
    m_trace_mng->fatal() << "Error in collective read of file " << m_filename;
  // Parse Buffer
  IXmlDocumentHolder * xmlDoc = m_io_mng->parseXmlBuffer(xmlBuffer,
      m_filename);
  // Check
  if (xmlDoc == NULL)
    m_trace_mng->fatal() << "Cannot open file: " << m_filename;

  //! Read XML file
  XmlNode rootNode = xmlDoc->documentNode().child("RegularGrid");
  // Check type of grid (no longer necessary when using xsd format)
  if (rootNode.attr("type", true).value() != "non-uniform")
    {
      m_trace_mng->fatal() << "Error file read is not a non-uniform grid";
    }
  XmlNode x_value_node = rootNode.child("x-value");
  XmlNode y_value_node = rootNode.child("y-value");
  XmlNode z_value_node = rootNode.child("z-value");
  XmlNode t_value_node = rootNode.child("t-value");
  XmlNode property_value_node = rootNode.child("property-value");
  bool hasErrorX = builtInGetValue(m_x, x_value_node.value());
  bool hasErrorY = builtInGetValue(m_y, y_value_node.value());
  bool hasErrorZ = builtInGetValue(m_z, z_value_node.value());
  bool hasErrorT = builtInGetValue(m_t, t_value_node.value());
  bool hasErrorProperty = builtInGetValue(m_property,
      property_value_node.value());
  if (hasErrorX or hasErrorY or hasErrorZ or hasErrorT or hasErrorProperty
      or m_property.size() != m_x.size() * m_y.size() * m_z.size()
          * m_t.size())
    {
      m_trace_mng->fatal() << "Failure in reading regular grid data file "
          << m_filename;
    }
  // Values must be sorted: expensive to check therefore sort yourself
  if (!_isSortedGrid())
    m_trace_mng->fatal() << "Non uniform grid point coordinates are not sorted in increasing order. Cannot continue";
}

/*---------------------------------------------------------------------------*/

template<typename T>
RegularGridType::GridDimension
UnsteadyRegularGridDataT<T>::
_readGrid_XDMF()
{

	bool isThereTime = false;

	int dimZ,dimY,dimX;

	// Collective read
	ByteSharedArray xmlBuffer;
	bool error_collective_read =
			m_io_mng->collectiveRead(m_filename, xmlBuffer);
	// Check error
	if (error_collective_read)
		m_trace_mng->fatal() << "Error in collective read of file " << m_filename;
	// Parse Buffer
	IXmlDocumentHolder * xmlDoc = m_io_mng->parseXmlBuffer(xmlBuffer,
			m_filename);
	// Check
	if (xmlDoc == NULL)
		m_trace_mng->fatal() << "Cannot open file: " << m_filename;

	//! Read XML file
	XmlNode rootNode = xmlDoc->documentNode().child("Xdmf");

	XmlNode domainNode = rootNode.child("Domain");

    // Main grid

	XmlNode mainGridNode = domainNode.child("Grid");

	// Topology and Geometry

	XmlNode topologyNode = domainNode.child("Topology");

	if ( topologyNode.null() )
	{
		topologyNode = mainGridNode.child("Topology");
	}

	Arcane::String topologyType = topologyNode.attr("TopologyType", true).value();

	m_trace_mng->debug(Trace::High) << "Topology Type ="  <<  topologyType;

	// checking if TopologyType is compatible with m_grid_type

	if (  m_grid_type == RegularGridType::UniformGrid  && ( topologyType != "2DCoRectMesh" && topologyType != "3DCoRectMesh" && topologyType != "1DCoMesh" ) )
		m_trace_mng->fatal() << "An uniform grid must be of topology type 2DCoRectMesh or 3DCoRectMesh";

	if (  m_grid_type == RegularGridType::NonUniformGrid && ( topologyType != "2DRectMesh" && topologyType != "3DRectMesh" && topologyType != "1DMesh") )
		m_trace_mng->fatal() << "An non uniform grid must be of of topology type 2DRectMesh or 3DRectMesh";

	m_trace_mng->debug(Trace::High) << "Topology Dimensions = " << topologyNode.attr("Dimensions", true).value();

	XmlNode geometryNode = domainNode.child("Geometry");

	if ( geometryNode.null() )
		{
			geometryNode = mainGridNode.child("Geometry");
		}

	Arcane::String geometryType = geometryNode.attr("Type", true).value();
	m_trace_mng->debug(Trace::High) << "GeometryType ="  <<  geometryType;

	m_trace_mng->debug(Trace::High) << "m_grid_type ="  <<  m_grid_type;

	// checking if GeometryType is compatible with m_grid_type

	if (  m_grid_type == RegularGridType::UniformGrid  && ( geometryType != "ORIGIN_DXDY" && geometryType != "ORIGIN_DXDYDZ" && geometryType != "ORIGIN_DZ") )
			m_trace_mng->fatal() << "An uniform grid must be of geometry type ORIGIN_DXDY or ORIGIN_DXDYDZ";

	if (  m_grid_type == RegularGridType::NonUniformGrid && ( geometryType != "VXVY" && geometryType != "VXVYVZ" && geometryType != "VZ" ) )
				m_trace_mng->fatal() << "An non uniform grid must be of geometry type VXVY or VXVYVZ";


	XmlNodeList dataItemList = geometryNode.children("DataItem");

	m_trace_mng->debug(Trace::Highest) << "DataItem List size ="  <<  dataItemList.size();

	for (XmlNodeList::const_iterator iDataItemList = dataItemList.begin();
			iDataItemList != dataItemList.end(); ++iDataItemList)
	{
		m_trace_mng->debug(Trace::High) << "DataItem Dimensions = " << iDataItemList->attr("Dimensions", true).value();
	}


	if ( geometryType == "VXVY" || geometryType == "VXVYVZ" || geometryType == "VZ" ) // non uniform
	{
		Arcane::SharedArray<int> dim;

		bool hasError = builtInGetValue(dim, topologyNode.attr("Dimensions", true).value());
		if ( hasError )
			m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (topology dimensions)";


		if ( geometryType == "VZ" )
		{

			if ( dim.size() != 1  )
				m_trace_mng->fatal() << String::format("The number of dimensions of topology node must be 1 [found {0}]", dim.size());


			dimZ = dim[0];
			dimY = 1;
			dimX = 1;

			XmlNode vzNode = dataItemList.node(0);

			m_z = ArcGeoSim::XDMFRealDataNodeReader(vzNode,
					"Error while reading VZ",
					m_mesh_file_directory_name).read();

			if ( m_z.size() != dimZ  )
							m_trace_mng->fatal() << String::format("The array of z coordinates has not the good size [expected {0}, found {1}]", dimZ, m_z.size());

			m_y.resize(1);
			m_y[0] = 0.0;

			m_x.resize(1);
			m_x[0] = 0.0;

		}
		else
		{
			if ( geometryType == "VXVY" )
			{
				if ( dim.size() != 2  )
					m_trace_mng->fatal() << String::format("The number of dimensions of topology node must be 2 [found {0}]", dim.size());

#ifndef BAD_ORDERING
				dimZ = 1;
				dimY = dim[1];
				dimX = dim[0];
#else
        dimZ = 1;
        dimY = dim[0];
        dimX = dim[1];
#endif

			}
			else
			{
				if ( dim.size() != 3  )
					m_trace_mng->fatal() << String::format("The number of dimensions of topology node must be 3 [found {0}]", dim.size());

#ifndef BAD_ORDERING
				dimZ = dim[2];
				dimY = dim[1];
				dimX = dim[0];
#else
        dimZ = dim[0];
        dimY = dim[1];
        dimX = dim[2];
#endif
			}

			XmlNode vxNode = dataItemList.node(0);

			m_x = ArcGeoSim::XDMFRealDataNodeReader(vxNode,
					"Error while reading VX",
					m_mesh_file_directory_name).read();

			m_trace_mng->debug(Trace::Highest) << "m_x = " << m_x;

			if ( m_x.size() != dimX  )
				m_trace_mng->fatal() << String::format("The array of x coordinates has not the good size [expected {0}, found {1}]", dimX, m_x.size());

			XmlNode vyNode = dataItemList.node(1);

			m_y = ArcGeoSim::XDMFRealDataNodeReader(vyNode,
					"Error while reading VY",
					m_mesh_file_directory_name).read();

			m_trace_mng->debug(Trace::Highest) << "m_y = " << m_y;

			if ( m_y.size() != dimY  )
				m_trace_mng->fatal() << String::format("The array of y coordinates has not the good size [expected {0}, found {1}]", dimY, m_y.size());

			//
			if ( geometryType == "VXVYVZ")
			{
				XmlNode vzNode = dataItemList.node(2);

				m_z = ArcGeoSim::XDMFRealDataNodeReader(vzNode,
						"Error while reading VZ",
						m_mesh_file_directory_name).read();
			}
			else
			{
				m_z.resize(1);
				m_z[0] = 0.0;
			}

			m_trace_mng->debug(Trace::Highest) << "m_z = " << m_z;

			if ( m_z.size() != dimZ  )
				m_trace_mng->fatal() << String::format("The array of z coordinates has not the good size [expected {0}, found {1}]", dimZ, m_z.size());
		}
	}
	else // uniform
	{
		if ( geometryType == "ORIGIN_DZ" )
		{
			m_trace_mng->debug(Trace::High) << "geometryType == ORIGIN_DZ" ;

			Arcane::SharedArray<int> dim;

			bool hasError = builtInGetValue(dim, topologyNode.attr("Dimensions", true).value());
			if ( hasError )
				m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (topology dimensions)";

			if ( dim.size() != 1  )
				m_trace_mng->fatal() << String::format("The number of dimensions of topology node must be 1 [found {0}]", dim.size());

      dimZ = dim[0];
      dimY = 1;
      dimX = 1;

			XmlNode originNode = dataItemList.node(0);

			Arcane::SharedArray<int> dimOrigin;
			hasError = builtInGetValue(dimOrigin, originNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (origin dimensions)";


      if ( dimOrigin.size() != 1  )
				m_trace_mng->fatal() << String::format("The number of dimensions of array Origin must be 1 [found {0}]", dimOrigin.size());

			int dimOriginNode = dimOrigin[0];

			if ( dimOriginNode != 1  )
							m_trace_mng->fatal() << String::format("The dimension of array Origin must be 1 [found {0}]", dimOriginNode);

			PointCollection origin = ArcGeoSim::XDMFRealDataNodeReader(originNode,
							"Error while reading ORIGIN",
							m_mesh_file_directory_name).read();

			if ( origin.size() != 1  )
				m_trace_mng->fatal() << String::format("The array Origin has not the good size [expected 1, found {0}]", origin.size());

			XmlNode deltaNode = dataItemList.node(1);

			Arcane::SharedArray<int> dimDelta;
			hasError = builtInGetValue(dimDelta, deltaNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (delta dimensions)";

			if ( dimDelta.size() != 1  )
				m_trace_mng->fatal() << String::format("The number of dimensions of array Delta must be 1 [found {0}]", dimDelta.size());

			int dimDeltaNode = dimDelta[0];

			if ( dimDeltaNode != 1  )
				m_trace_mng->fatal() << String::format("The dimension of array Delta must be 1 [found {0}]", dimDeltaNode);

			PointCollection delta = ArcGeoSim::XDMFRealDataNodeReader(deltaNode,
					"Error while reading Delta",
					m_mesh_file_directory_name).read();

			if ( delta.size() != 1  )
				m_trace_mng->fatal() << String::format("The array Delta has not the good size 1 [found {0}]", delta.size());

			m_z.resize(dimZ);

			for (Integer i = 0; i < dimZ; ++i)
			{
				m_z[i] = origin[0] + i * delta[0];
			}

			m_y.resize(1);
			m_y[0] = 0.0;

			m_x.resize(1);
			m_x[0] = 0.0;

		}
    else if ( geometryType == "ORIGIN_DXDY" )   //  2D uniform
		{
			m_trace_mng->debug(Trace::High) << "geometryType == ORIGIN_DXDY" ;

			Arcane::SharedArray<int> dim;

			bool hasError = builtInGetValue(dim, topologyNode.attr("Dimensions", true).value());
			if ( hasError )
				m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (topology dimensions)";

			if ( dim.size() != 2  )
							m_trace_mng->fatal() << String::format("The number of dimensions of topology node must be 2 [found {0}]", dim.size());

#ifndef BAD_ORDERING
      dimZ = 1;
			dimY = dim[1];
			dimX = dim[0];
#else
      dimZ = 1;
      dimY = dim[0];
      dimX = dim[1];
#endif

			m_trace_mng->debug(Trace::High) << "dimY = " << dimY ;
			m_trace_mng->debug(Trace::High) << "dimX = " << dimX ;


			XmlNode originNode = dataItemList.node(0);

			Arcane::SharedArray<int> dimOrigin;
			hasError = builtInGetValue(dimOrigin, originNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (origin dimensions)";

      if ( dimOrigin.size() != 1  )
				m_trace_mng->fatal() << String::format("The number of dimensions of array Origin must be 1 [found {0}]", dimOrigin.size());

			int dimOriginNode = dimOrigin[0];

			if ( dimOriginNode != 2  )
							m_trace_mng->fatal() << String::format("The dimension of array Origin must be 2 [found {0}]", dimOriginNode);

			PointCollection origin = ArcGeoSim::XDMFRealDataNodeReader(originNode,
							"Error while reading Origin",
							m_mesh_file_directory_name).read();

			if ( origin.size() != 2  )
				m_trace_mng->fatal() << String::format("The array Origin has not the good size [expected 2, found {0}]", origin.size());

			XmlNode deltaNode = dataItemList.node(1);

			Arcane::SharedArray<int> dimDelta;
			hasError = builtInGetValue(dimDelta, deltaNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (delta dimensions)";


      if ( dimDelta.size() != 1  )
							m_trace_mng->fatal() << String::format("The number of dimensions of array Delta must be 1 [found {0}]", dimDelta.size());

			int dimDeltaNode = dimDelta[0];

			if ( dimDeltaNode != 2  )
				m_trace_mng->fatal() << String::format("The dimension of array Delta must be 2 [found {0}]", dimDeltaNode);

			PointCollection delta = ArcGeoSim::XDMFRealDataNodeReader(deltaNode,
										"Error while reading Delta",
										m_mesh_file_directory_name).read();

			if ( delta.size() != 2  )
							m_trace_mng->fatal() << String::format("The array Delta has not the good size 2 [found {0}]", delta.size());

			m_x.resize(dimX);

			m_y.resize(dimY);

			for (Integer i = 0; i < dimX; ++i)
			{
				m_x[i] = origin[0] + i * delta[0];
			}
			for (Integer i = 0; i < dimY; ++i)
			{
				m_y[i] = origin[1] + i * delta[1];
			}

			m_trace_mng->debug(Trace::Highest) << "m_x = " << m_x;

			m_trace_mng->debug(Trace::Highest) << "m_y = " << m_y;

			//
			m_z.resize(1);
			m_z[0] = 0.0;

			m_trace_mng->debug(Trace::Highest) << "m_z = " << m_z;
		}
		else if (geometryType == "ORIGIN_DXDYDZ")   //  3D uniform
    {
      m_trace_mng->debug(Trace::High) << "geometryType == ORIGIN_DXDYDZ" ;

      Arcane::SharedArray<int> dim;

      bool hasError = builtInGetValue(dim, topologyNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (topology dimensions)";

      if ( dim.size() != 3  )
        m_trace_mng->fatal() << String::format("The number of dimensions of topology node must be 3 [found {0}]", dim.size());

#ifndef BAD_ORDERING
      dimZ = dim[2];
      dimY = dim[1];
      dimX = dim[0];
#else
      dimZ = dim[0];
      dimY = dim[1];
      dimX = dim[2];
#endif

      m_trace_mng->debug(Trace::High) << "dimZ = " << dimZ ;
      m_trace_mng->debug(Trace::High) << "dimY = " << dimY ;
      m_trace_mng->debug(Trace::High) << "dimX = " << dimX ;

      XmlNode originNode = dataItemList.node(0);

      Arcane::SharedArray<int> dimOrigin;
      hasError = builtInGetValue(dimOrigin, originNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (origin dimensions)";

      if ( dimOrigin.size() != 1  )
        m_trace_mng->fatal() << String::format("The number of dimensions of array Origin must be 1 [found {0}]", dimOrigin.size());

      int dimOriginNode = dimOrigin[0];

      if ( dimOriginNode != 3  )
        m_trace_mng->fatal() << String::format("The dimension of array Origin must be 3 [found {0}]", dimOriginNode);

      PointCollection origin = ArcGeoSim::XDMFRealDataNodeReader(originNode,
          "Error while reading Origin",
          m_mesh_file_directory_name).read();

      if ( origin.size() != 3  )
              m_trace_mng->fatal() << String::format("The array Origin has not the good size 3 [found {0}]", origin.size());

      XmlNode deltaNode = dataItemList.node(1);

      Arcane::SharedArray<int> dimDelta;
      hasError = builtInGetValue(dimDelta, deltaNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (delta dimensions)";

      if ( dimDelta.size() != 1  )
        m_trace_mng->fatal() << String::format("The number of dimensions of array Delta must be 1 [found {0}]", dimDelta.size());

      int dimDeltaNode = dimDelta[0];

      if ( dimDeltaNode != 3 )
        m_trace_mng->fatal() << String::format("The dimension of array Delta must be 3 [found {0}]", dimDeltaNode);

      PointCollection delta = ArcGeoSim::XDMFRealDataNodeReader(deltaNode,
          "Error while reading Delta",
          m_mesh_file_directory_name).read();

      if ( delta.size() != 3  )
         m_trace_mng->fatal() << String::format("The array Delta has not the good size 3 [found {0}]", delta.size());

      m_x.resize(dimX);

      m_y.resize(dimY);

      m_z.resize(dimZ);

      for (Integer i = 0; i < dimX; ++i)
      {
        m_x[i] = origin[0] + i * delta[0];
      }
      for (Integer i = 0; i < dimY; ++i)
      {
        m_y[i] = origin[1] + i * delta[1];
      }
      for (Integer i = 0; i < dimZ; ++i)
      {
        m_z[i] = origin[2] + i * delta[2];
      }

      m_trace_mng->debug(Trace::Highest) << "m_x = " << m_x;

      m_trace_mng->debug(Trace::Highest) << "m_y = " << m_y;

      m_trace_mng->debug(Trace::Highest) << "m_z = " << m_z;
		}
    else
    {
      dimX = dimY = dimZ = -1; // to avoid warning about non-initialized variable usage
      m_trace_mng->fatal() << String::format("Unrecognized geometry type {0}", geometryType);
    }
	}


	XmlNodeList gridList = mainGridNode.children("Grid");

	if ( gridList.size() == 0 )
	{
		m_t.resize(1);
		m_t[0] = 0.0;
		XmlNode attributeNode = mainGridNode.child("Attribute");
    String supportStr = attributeNode.attr("Center", true).value();
    m_trace_mng->debug(Trace::High) << "Center = " << supportStr;
    if (supportStr != "Node")
      m_trace_mng->fatal() << String::format("Failure in reading regular grid data file {0} (not supported data support {1})", m_filename, supportStr);

		// TODO: controler les dimensions de propri�t�s lues
		XmlNode dataItemNode = attributeNode.child("DataItem");

		Arcane::SharedArray<int> dimValues;
		bool hasError = builtInGetValue(dimValues, dataItemNode.attr("Dimensions", true).value());
    if ( hasError )
      m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (data dimensions)";

		if ( dimValues.size() != 1 && dimValues.size() != 2 && dimValues.size() != 3 )
			m_trace_mng->fatal() << "The number of dimensions of array Attribute must be 2 or 3";

		int nbValues = 1;
		for (int i=0;i<dimValues.size();i++)
			nbValues *=  dimValues[i];

		int sizeGrid = dimZ*dimY*dimX;

		if ( nbValues != sizeGrid  )
			m_trace_mng->fatal() << String::format("The number of attribute values must be {1} [declared {0}]", nbValues, sizeGrid);

		PointCollection values = ArcGeoSim::XDMFRealDataNodeReader(dataItemNode,
				"Error while reading Values",
				m_mesh_file_directory_name).read();

		if ( values.size() != nbValues  )
			m_trace_mng->fatal() << String::format("The number of attribute values must be {1} [found {0}]", values.size(), nbValues);

		m_property.addRange(transposeData(values, dimX, dimY, dimZ));
	}
	else
	{
		isThereTime = true;
		for (XmlNodeList::const_iterator iGridList = gridList.begin();
				iGridList != gridList.end(); ++iGridList)
		{
			XmlNode timeNode = iGridList->child("Time");
			m_trace_mng->debug(Trace::High) << "Grid Time Value = " << timeNode.attr("Value", true).value();

			String timestr = timeNode.attr("Value", true).value();

			Real time;

			bool hasError = builtInGetValue(time, timestr);
			if ( hasError )
				m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (time value)";

			m_t.add(time);

			XmlNode attributeNode = iGridList->child("Attribute");
      String supportStr = attributeNode.attr("Center", true).value();
			m_trace_mng->debug(Trace::High) << "Center = " << supportStr;
      if (supportStr != "Node")
        m_trace_mng->fatal() << String::format("Failure in reading regular grid data file {0} (not supported data support {1})", m_filename, supportStr);

			// TODO: controler les dimensions de propri�t�s lues
			XmlNode dataItemNode = attributeNode.child("DataItem");

			Arcane::SharedArray<int> dimValues;
			hasError = builtInGetValue(dimValues, dataItemNode.attr("Dimensions", true).value());
      if ( hasError )
        m_trace_mng->fatal() << "Failure in reading regular grid data file " << m_filename << " (data dimensions)";

      if ( dimValues.size() != 1 && dimValues.size() != 2 && dimValues.size() != 3 )
				m_trace_mng->fatal() << "The number of dimensions of array Attribute must be 2 or 3";

			int nbValues = 1;
			for (int i=0;i<dimValues.size();i++)
				nbValues *=  dimValues[i];

			int sizeGrid = dimZ*dimY*dimX;

			if ( nbValues != sizeGrid  )
				m_trace_mng->fatal() << String::format("The number of attribute values must be {1} [declared {0}]", nbValues, sizeGrid);

			PointCollection values = ArcGeoSim::XDMFRealDataNodeReader(dataItemNode,
					"Error while reading Values",
					m_mesh_file_directory_name).read();

			if ( values.size() != nbValues  )
				m_trace_mng->fatal() << String::format("The number of attribute values must be {1} [found {0}]", values.size(), nbValues);

      m_property.addRange(transposeData(values, dimX, dimY, dimZ));
		}
	}

	m_trace_mng->debug(Trace::Highest) << "m_t = " << m_t;

	m_trace_mng->debug(Trace::Highest) << "values = " << m_property;

	RegularGridType::GridDimension gridDimension = RegularGridType::GridDimension::eUndefined;

	if ( geometryType == "VXVY" || geometryType == "ORIGIN_DXDY" )
	{
		if ( isThereTime )
			gridDimension = RegularGridType::GridDimension::e2DT;
		else
			gridDimension = RegularGridType::GridDimension::e2D;
	}
	else
		if ( geometryType == "VXVYVZ" || geometryType == "ORIGIN_DXDYDZ" )
		{
			if ( isThereTime )
				gridDimension = RegularGridType::GridDimension::e3DT;
			else
				gridDimension = RegularGridType::GridDimension::e3D;
		}

	return gridDimension;

}

/*---------------------------------------------------------------------------*/

template<typename T>
void
UnsteadyRegularGridDataT<T>::
_readUniformGrid()
{
  // file format:
  // <x-dimension>Nx</x_dimension>
  // <x-extremal>Xmin Xmax</x-extremal>
  // <y-dimension>Ny</y_dimension>
  // <y-extremal>Ymin Ymax</y-extremal>
  // <z-dimension>Nz</z_dimension>
  // <z-extremal>Nz Zmin Zmax</z-extremal>
  // <t-value>list_of_time_value</t_value>
  // <property-value>list_of_property_value</property_value>

  /* Exemple of sequential read using xsd schema
   * IXmlDocumentHolder * xmlDoc = m_io_mng->parseXmlFile(m_filename,
   *       m_schema_name, ByteConstArrayView(strlen((const char*)uniform_grid_xsd), uniform_grid_xsd));
   */

  /* Parallel read : parse XML buffer :
   * use of xsd scheme not yet available in parseXmlBuffer
   * use xsd file regular_uniform_grid_data.xsd
   * To update when available (cf warning)
   */
#ifndef NO_USER_WARNING
#ifndef WIN32
//#warning "TODO : use xsd scheme in parallel as soon as available in arcane IIOMng::parseXmlBuffer"
#endif
#endif
  // Collective read
  ByteSharedArray xmlBuffer;
  bool error_collective_read =
      m_io_mng->collectiveRead(m_filename, xmlBuffer);
  // Check error
  if (error_collective_read)
    m_trace_mng->fatal() << "Error in collective read of file " << m_filename;
  // Parse Buffer
  IXmlDocumentHolder * xmlDoc = m_io_mng->parseXmlBuffer(xmlBuffer,
      m_filename);
  // Check
  if (xmlDoc == NULL)
    m_trace_mng->fatal() << "Cannot open file: " << m_filename;

  //! Read XML file
  XmlNode rootNode = xmlDoc->documentNode().child("RegularGrid");
  // Check type of grid (no longer necessary when using xsd format)
  if (rootNode.attr("type", true).value() != "uniform")
    {
      m_trace_mng->fatal() << "Error file read is not a uniform grid";
    }
  XmlNode x_dimension_node = rootNode.child("x-dimension");
  XmlNode x_extremal_node = rootNode.child("x-extremal");
  XmlNode y_dimension_node = rootNode.child("y-dimension");
  XmlNode y_extremal_node = rootNode.child("y-extremal");
  XmlNode z_dimension_node = rootNode.child("z-dimension");
  XmlNode z_extremal_node = rootNode.child("z-extremal");
  XmlNode t_value_node = rootNode.child("t-value");
  XmlNode property_value_node = rootNode.child("property-value");
  SharedArray<Real> x_extremal;
  SharedArray<Real> y_extremal;
  SharedArray<Real> z_extremal;
  bool hasErrorX = builtInGetValue(x_extremal, x_extremal_node.value());
  bool hasErrorY = builtInGetValue(y_extremal, y_extremal_node.value());
  bool hasErrorZ = builtInGetValue(z_extremal, z_extremal_node.value());
  bool hasErrorT = builtInGetValue(m_t, t_value_node.value());
  bool hasErrorProperty = builtInGetValue(m_property,
      property_value_node.value());
  if (hasErrorX or hasErrorY or hasErrorZ or hasErrorT or hasErrorProperty
      or x_extremal.size() != 2 or y_extremal.size() != 2
      or z_extremal.size() != 2)
    {
      m_trace_mng->fatal() << "Failure in reading regular grid data file "
          << m_filename;
    }
  Integer Nx, Ny, Nz;
  Real Xmin, Xmax, deltaX, Ymin, Ymax, deltaY, Zmin, Zmax, deltaZ;
  Nx = x_dimension_node.valueAsInteger();
  Xmin = x_extremal[0];
  Xmax = x_extremal[1];
  Ny = y_dimension_node.valueAsInteger();
  Ymin = y_extremal[0];
  Ymax = y_extremal[1];
  Nz = z_dimension_node.valueAsInteger();
  Zmin = z_extremal[0];
  Zmax = z_extremal[1];
  if (m_property.size() != Nx * Ny * Nz * m_t.size() or Nx < 1 or Ny < 1
      or Nz < 1)
    {
      m_trace_mng->pfatal() << "Failure in reading regular grid data file "
          << m_filename;
    }
  Nx == 1 ? deltaX = 0 : deltaX = (Xmax - Xmin) / (Nx - 1);
  Ny == 1 ? deltaY = 0 : deltaY = (Ymax - Ymin) / (Ny - 1);
  Nz == 1 ? deltaZ = 0 : deltaZ = (Zmax - Zmin) / (Nz - 1);
  // Generate grid
  m_x.resize(Nx);
  m_y.resize(Ny);
  m_z.resize(Nz);
  for (Integer i = 0; i < Nx; ++i)
    {
      m_x[i] = Xmin + i * deltaX;
    }
  for (Integer i = 0; i < Ny; ++i)
    {
      m_y[i] = Ymin + i * deltaY;
    }
  for (Integer i = 0; i < Nz; ++i)
    {
      m_z[i] = Zmin + i * deltaZ;
    }
}

/*---------------------------------------------------------------------------*/

template<typename T>
bool
UnsteadyRegularGridDataT<T>::
_isSortedGrid()
{
  // Check if grid point coordinates are sorted
  bool is_x_sorted = _isSortedArray(m_x);
  bool is_y_sorted = _isSortedArray(m_y);
  bool is_z_sorted = _isSortedArray(m_z);
  bool is_t_sorted = _isSortedArray(m_t);
  return is_x_sorted && is_y_sorted && is_z_sorted && is_t_sorted;

}

template<typename T>
bool
UnsteadyRegularGridDataT<T>::
_isSortedArray(const RealArray& array)
{
  bool is_sorted = true;
  Integer size = array.size();
  for (Integer i = 1 ; i < size && is_sorted; ++i)
    {
      is_sorted = (array[i] > array[i-1]);
    }
  return is_sorted;
}


//! Getting file name
template<typename T>
String
UnsteadyRegularGridDataT<T>::
getFileName() const
{
  return m_filename;
}

//! Getting grid type
template<typename T>
RegularGridType::eRegularGridType
UnsteadyRegularGridDataT<T>::
getGridType() const
{
  return m_grid_type;
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
