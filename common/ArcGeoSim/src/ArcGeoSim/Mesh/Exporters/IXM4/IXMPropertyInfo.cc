/*
 * IxmPropertyInfo.cc
 *
 *  Created on: Jul 4, 2012
 *      Author: mesriy
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

#include "IXMDataNode.h"
#include "IXM4DataNode.h"
#include "IXMPropertyInfo.h"

#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMUidToLidMng.h"
#include "ArcGeoSim/Mesh/Utils/ArrayMng.h"
#include "ArcGeoSim/Mesh/Utils/ArrayMngT.h"
#include "ArcGeoSim/Mesh/Utils/VariableView.h"

#include <boost/math/special_functions/fpclassify.hpp>
#include <cmath>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IXMPropertyInfo::IXMPropertyInfo() {
	// TODO Auto-generated constructor stub

}

/*---------------------------------------------------------------------------*/

IXMPropertyInfo::~IXMPropertyInfo() {
	// TODO Auto-generated destructor stub
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType>
class IxmPropertyInfoT
: public IXMPropertyInfo
{
public:
  IxmPropertyInfoT(IVariable* var) : m_variable(var) {}

  IVariable* variable() const {return m_variable;}

  void writeProperty(XmlNode current_node, const String& format, IXM4FileInfo & fileinfo, const bool write_only_active_cells = false, const Real& time = std::numeric_limits<Real>::quiet_NaN( ));

private:

  void staticBuild(XmlNode current_node, const String& format,const Real& time, Int64SharedArray property_support, SharedArray<DataType> property_values, IXM4FileInfo &  fileinfo, const Integer array_size);
  void staticBuild(XmlNode current_node, const String& format,const Real& time,Array<DataType>& property_values, const Integer array_size);
  void _getArraySize(Arcane::Integer& array_size);
  void _gatherData(IParallelMng* pm, SharedArray<DataType>& values, Int64SharedArray& uids);

private:
  IVariable* m_variable;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IXMPropertyInfo* IXMPropertyInfo::
create(IMesh* mesh,const String& name,const String& family_name)
{
  IItemFamily* family = mesh->findItemFamily(family_name,true);
  IVariable* var = family->findVariable(name,true);
  return IXMPropertyInfo::create(var);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IXMPropertyInfo* IXMPropertyInfo::
create(IVariable* var)
{
  IXMPropertyInfo* prop_info = 0;
  switch (var->dataType()) {
  case DT_Real:
    prop_info = new IxmPropertyInfoT<Real>(var);
    break;
  case DT_Real2:
    prop_info = new IxmPropertyInfoT<Real2>(var);
    break;
  case DT_Real2x2:
    prop_info = new IxmPropertyInfoT<Real2x2>(var);
    break;
  case DT_Real3:
    prop_info = new IxmPropertyInfoT<Real3>(var);
    break;
  case DT_Real3x3:
    prop_info = new IxmPropertyInfoT<Real3x3>(var);
    break;
  case DT_Byte:
    prop_info = new IxmPropertyInfoT<Byte>(var);
    break;
  case DT_Int32:
    prop_info = new IxmPropertyInfoT<Int32>(var);
    break;
  case DT_Int64:
    prop_info = new IxmPropertyInfoT<Int64>(var);
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

String IXMPropertyInfo::
getDataType(IVariable* var) const {
	eDataType data_type= var->dataType();
	return ArcGeoSim::IXMTools::dataTypeIXMName(data_type);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
template<typename DataType> void
IxmPropertyInfoT<DataType>::
writeProperty(XmlNode current_node, const String& format, IXM4FileInfo & fileinfo, const bool write_only_active_cells, const Real& time)
{
  
  ItemGroup group;
  
  if (write_only_active_cells)
  {
     if ( m_variable->itemKind() ==  Arcane::IK_Cell )
      	group = m_variable->itemGroup().activeCellGroup();
     else
      	group = m_variable->itemGroup().own();
  }
  else
      	group = m_variable->itemGroup().own();
  
  //Arcane::eItemKind m_variable->itemKind;


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

  // Get uids array (or abstract_ids if they exist)
  Int64SharedArray uids(group.size());
  ArcGeoSim::IXMLidToUidMng lid2uid(m_variable->mesh());
  lid2uid.itemsUniqueId(uids,group.view(),m_variable->itemKind());

  Integer array_size;
  _getArraySize(array_size);

  // Write
  if (group.null()) { // global variable
      if (pm->isMasterIO()) staticBuild(current_node,format,time,values,array_size); // suppose variable was synchronized
  }
  else {
      _gatherData(pm,values,uids);
      if (pm->isMasterIO()) staticBuild(current_node,format,time,uids,values,fileinfo,array_size);
    }

  delete array_mng;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType> void
IxmPropertyInfoT<DataType>::
_getArraySize(Arcane::Integer& array_size)
{
  array_size = -1; // Dans ce cas on a une variable de type scalaire
  // Rm: une variable de type tableau peut ��tre de taille 0, 1 ou plus
  // array_size >= 0 ssi la variable est de type tableau
  if (m_variable->itemGroup().null()) {// global variable
      if (m_variable->dimension() == 1) {
    	  array_size = m_variable->nbElement();
      }
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
IxmPropertyInfoT<DataType>::
_gatherData(IParallelMng* pm, SharedArray<DataType>& values, Int64SharedArray& uids)
{
  Int64SharedArray all_uids;
  SharedArray<DataType> all_values;
  if(pm->isParallel()){
    pm->gatherVariable(uids.constView(),all_uids , pm->masterIORank());
    pm->gatherVariable(values.constView(),all_values , pm->masterIORank());
    uids = all_uids;
    values = all_values;
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType> void
IxmPropertyInfoT<DataType>::
staticBuild(XmlNode current_node, const String& format,const Real& time, Int64SharedArray property_support,
    SharedArray<DataType> property_values, IXM4FileInfo & fileinfo, const Integer array_size){

  IVariable* var= m_variable;
  String  _name =var->name();
  String  _kind =ArcGeoSim::IXMTools::itemKindIXMName(var->itemKind());
  _kind = _kind.lower();
  String  _group_name=var->itemGroupName();

  //ISubDomain* sd = var->subDomain();
  //ITraceMng* tm = sd->traceMng();
  String event_name;
  String support_name;
  String data_type= getDataType(var);
  XmlElement property_node(current_node,"property");
  property_node.setAttrValue("name",_name);
  property_node.setAttrValue("kind",_kind);

  XmlElement support_node(property_node,"support");
  support_node.setAttrValue("group-name",_group_name);

  {
	  if( format == "xml"  )
	  {
		  IXM4DataNode<Int64>::createIXM4DataNode(support_node,property_support);
	  }
	  else
	  {
		  XmlNode parent= current_node.parent();
		  if ( parent.name() == "mesh")
		     support_name = "mesh";
		  else    
		     support_name=parent.attrValue("name");
		  
		  //String hdata_support(String::format("{0}/{1}/{2}/{3}/support",m_hdf_path,support_name,current_node.name(),_name));
		  String hdata_support(String::format("{0}/{1}/groups/{2}/group-ids",m_hdf_path,support_name,_group_name));

		  bool writeHDFFile = false;

		  if (  fileinfo.supportSet.find(_group_name) == fileinfo.supportSet.end() )
		  {
			  fileinfo.supportSet.insert(_group_name);
			  writeHDFFile = true;
		  }

		  IXM4DataNode<Int64>::createIXM4DataNode(support_node,property_support, HDFFormatBuildInfo(hdata_support,m_directory_name,writeHDFFile));
	  }
  }

  XmlElement values_node(property_node,"values");
  if( !boost::math::isnan(time) ){
    String values_str_time(String::format("{0}",time));
    values_node.setAttrValue("time",values_str_time);
  }

  if( format == "xml"  )
  {
	  IXM4DataNode<DataType>::createIXM4DataNode(values_node,property_values,array_size);
  }
  else
  {
  		XmlNode parent= current_node.parent();
  		if(parent.name() == "mesh" )
			  event_name="mesh";
		  else
			  event_name=parent.attrValue("name");
	  	String hdata_values(String::format("{0}/{1}/{2}/{3}_{4}/values",m_hdf_path,event_name,current_node.name(),_kind,_name));
	  	IXM4DataNode<DataType>::createIXM4DataNode(values_node,property_values, HDFFormatBuildInfo(hdata_values,m_directory_name,true),array_size);
  }


}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType> void
IxmPropertyInfoT<DataType>::
staticBuild(XmlNode current_node, const String& format,const Real& time,Array<DataType>& property_values, const Integer array_size){
  IVariable* var= m_variable;
  String  _name=var->name();
  String  _kind=itemKindName(var->itemKind());
  String  _group_name=var->itemGroupName();

  String data_type= getDataType(var);
  XmlElement property_node(current_node,"property");
  property_node.setAttrValue("name",_name);
  property_node.setAttrValue("kind","global"); // used for variables mesh-independent


  XmlElement values_node(property_node,"values");
  if( !boost::math::isnan(time) ){
    String values_str_time(String::format("{0}",time));
    values_node.setAttrValue("time",values_str_time);
  }


  if( format == "xml"  )
  {
	  IXM4DataNode<DataType>::createIXM4DataNode(values_node,property_values,array_size);
  }
  else
  {
	  String event_name;
	  XmlNode parent= current_node.parent();
	  if(parent.name() == "mesh" )
		  event_name="mesh";
	  else
		  event_name=parent.attrValue("name");
	  String hdata_values(String::format("{0}/{1}/{2}/{3}/values",m_hdf_path,event_name,current_node.name(),_name));
	  IXM4DataNode<DataType>::createIXM4DataNode(values_node,property_values, HDFFormatBuildInfo(hdata_values,m_directory_name,true),array_size);
  }


}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
