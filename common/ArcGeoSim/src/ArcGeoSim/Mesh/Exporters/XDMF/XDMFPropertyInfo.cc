/*
 * XdmfPropertyInfo.cc
 *
 */
#include "arcane/utils/ArcanePrecomp.h"

#include "arcane/utils/ITraceMng.h"
#include "arcane/utils/StringBuilder.h"
#include "arcane/utils/HashTableMap.h"
#include "arcane/utils/NotImplementedException.h"
#include "arcane/utils/ArgumentException.h"

#include "arcane/MeshVariable.h"
#include "arcane/IItemFamily.h"
#include "arcane/IMesh.h"
#include "arcane/IMeshSubMeshTransition.h"
#include "arcane/IVariable.h"
#include "arcane/IParallelMng.h"
#include "arcane/ISubDomain.h"

#include "XDMFDataNode.h"
#include "XDMFPropertyInfo.h"

#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMUidToLidMng.h"
#include "ArcGeoSim/Mesh/Utils/ArrayMng.h"
#include "ArcGeoSim/Mesh/Utils/ArrayMngT.h"
#include "ArcGeoSim/Mesh/Utils/VariableView.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

XDMFPropertyInfo::XDMFPropertyInfo() {
	// TODO Auto-generated constructor stub

}

/*---------------------------------------------------------------------------*/

XDMFPropertyInfo::~XDMFPropertyInfo() {
	// TODO Auto-generated destructor stub
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType>
class XdmfPropertyInfoT
: public XDMFPropertyInfo
{
public:
  XdmfPropertyInfoT(IVariable* var) : m_variable(var) {}

  IVariable* variable() const {return m_variable;}

  void writeProperty(XmlNode current_node, const String& format,const Real& time=-1.0);

private:

  void staticBuild(XmlNode current_node, const String& format,const Real& time,Array<DataType>& property_values, const Integer array_size);
  void _getArraySize(Arcane::Integer& array_size);

private:
  IVariable* m_variable;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

XDMFPropertyInfo* XDMFPropertyInfo::
create(IMesh* mesh,const String& name,const String& family_name)
{
  IItemFamily* family = mesh->findItemFamily(family_name,true);
  IVariable* var = family->findVariable(name,true);
  return XDMFPropertyInfo::create(var);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

XDMFPropertyInfo* XDMFPropertyInfo::
create(IVariable* var)
{
  XDMFPropertyInfo* prop_info = 0;
  switch (var->dataType()) {
  case DT_Real:
    prop_info = new XdmfPropertyInfoT<Real>(var);
    break;
  case DT_Real2:
    prop_info = new XdmfPropertyInfoT<Real2>(var);
    break;
  case DT_Real2x2:
    prop_info = new XdmfPropertyInfoT<Real2x2>(var);
    break;
  case DT_Real3:
    prop_info = new XdmfPropertyInfoT<Real3>(var);
    break;
  case DT_Real3x3:
    prop_info = new XdmfPropertyInfoT<Real3x3>(var);
    break;
  case DT_Byte:
    prop_info = new XdmfPropertyInfoT<Byte>(var);
    break;
  case DT_Int32:
    prop_info = new XdmfPropertyInfoT<Int32>(var);
    break;
  case DT_Int64:
    prop_info = new XdmfPropertyInfoT<Int64>(var);
    break;
  case DT_String:
    throw FatalErrorException(A_FUNCINFO,"Bad variable type");
    break;
  case DT_Unknown:
    throw FatalErrorException(A_FUNCINFO,"Bad variable type");
    break;
  }
  return prop_info;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

String XDMFPropertyInfo::
getDataType(IVariable* var) const {
	eDataType data_type= var->dataType();
	return ArcGeoSim::IXMTools::dataTypeIXMName(data_type);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType> void
XdmfPropertyInfoT<DataType>::
writeProperty(XmlNode current_node, const String& format, const Real& time)
{
  ItemGroup group = m_variable->itemGroup().own();

  // Get values arrays
  ISubDomain* subdomain = m_variable->subDomain();
  IParallelMng* pm = subdomain->parallelMng();

  // Create a view on variable (to handle in the same way any VariableType)
  ArcGeoSim::VariableUtils::VariableView variable_view(m_variable);

  // Extract values on the variable item group (requires to create a GenericArray and an ArrayMng)
  ArcGeoSim::ArrayMng* array_mng = new ArcGeoSim::ArrayMngT<DataType>(subdomain);
  ArcGeoSim::VariableUtils::GenericArray generic_values(array_mng);

  generic_values = variable_view[group];

  // Copy extracted values in a standard Arcane::Array
  SharedArray<DataType> values(generic_values);

  Integer array_size;
  _getArraySize(array_size);

  // TO DO ...
  if ( array_size != 1 )
	  subdomain->traceMng()->fatal() << "Could not manage a property with more than one array";

  // Write
 if (pm->isMasterIO()) staticBuild(current_node,format,time,values,array_size); // suppose variable was synchronized
 delete array_mng;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType> void
XdmfPropertyInfoT<DataType>::
_getArraySize(Arcane::Integer& array_size)
{
  array_size = 1;
  if (m_variable->itemGroup().null()) {// global variable
      array_size = m_variable->nbElement();
    }
  else {// mesh variable
      if (m_variable->dimension() == 2) {
          Integer nb_items;
          ArcGeoSim::VariableUtils::meshVariableArraySize(m_variable->data(),nb_items,array_size);
      }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType> void
XdmfPropertyInfoT<DataType>::
staticBuild(XmlNode current_node, const String& format,const Real& time,Array<DataType>& property_values, const Integer array_size){
  IVariable* var= m_variable;
  String  _name=var->name();
  String  _kind=itemKindName(var->itemKind());
  String  _group_name=var->itemGroupName();

  String data_type= getDataType(var);
  XmlElement property_node(current_node,"Attribute");

  property_node.setAttrValue("Name",_name);
  property_node.setAttrValue("AttributeType","Scalar");
  property_node.setAttrValue("Center","Cell");

  if(format == "XML")
  {
	  XDMFDataNode<DataType>::createXDMFDataNode(property_node, property_values);
  }
  else
  {
	  String hdata_values(String::format("{0}/{1}/{2}/values",m_hdf_path,current_node.attrValue("Name"),_name));
	  XDMFDataNode<DataType>::createXDMFDataNode(property_node,property_values, HDFFormatBuildInfo(hdata_values ,m_directory_name));
  }

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
