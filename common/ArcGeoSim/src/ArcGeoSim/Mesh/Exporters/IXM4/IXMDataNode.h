/*
 * IxmDataNode.h
 *
 *  Created on: Jul 31, 2012
 *      Author: mesriy
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMDATANODE_H_
#define ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMDATANODE_H_

#include <arcane/CommonVariables.h>
#include "arcane/XmlNode.h"
#include <arcane/utils/Array.h>
#include <arcane/utils/NotSupportedException.h>
#include <arcane/utils/StringBuilder.h>

#include "IXMValues.h"
#include "IXMValuesXML.h"
#include "IXMValuesHDF.h"

using namespace Arcane;


template<typename T>
class IXMDataNode {

	//! Types des codes de retour d'une lecture ou �criture
	enum eReturnType
	{
		RTOk, //!< Op�ration effectu�e avec succ�s
		RTError, //!< Erreur lors de l'op�ration
		/*! \brief Non concern� par l'op�ration.
		 * Cela signifie que le format de fichier ne correspond
		 * pas � ce lecteur ou que le service ne prend pas en compte
		 * cette op�ration.
		 */
		RTIrrelevant
	};
	enum eFormatType {
		IXM_FORMAT_XML,
		IXM_FORMAT_HDF
	};


public:
	IXMDataNode();

	IXMDataNode(XmlNode& parent_node, const String& format, const String& data_type, const String& directory_name, const Integer array_size=-1);

	virtual ~IXMDataNode();

	//! Get the format of the data. Usually XML | HDF
	eFormatType getFormat() const { return m_format;};

	//! Set the format of the data. Usually XML | HDF. Default is XML.
	void setFormat(const eFormatType& format) { m_format=format;};

	//! Get the data values access object
	// IXMValues * getValues(IXMValues *);

	//! Update Structure and Values potentially reading Heavy Data (INPUT)
	//Int32 update();

	//! Update the DOM (OUTPUT)
	Int32 build(Array<T>& array);

	//! Convenience Function
	Int32 getSize(){return m_size;};

	//! Convenience Function
	Int32 setSize(Int64 size){return m_size=size;};

	//!
	XmlNode getDataNode() {return m_data_node;};

	//!
	Int32 getFormat(){return m_format;}

	//! Set the name of the Heavy Data Set (if applicable)
	void setHeavyDataName(const String& hdata_name) {m_heavy_data_name=hdata_name;};

	//! Get the name of the Heavy Data Set (if applicable)
	String getHeavyDataName() const {return m_heavy_data_name;};

	//! Insert an Element
	XmlElement createFromParent( XmlNode& parent_node, const String& format, const String& type, const Integer array_size);


public:
	XmlNode  m_data_node;

protected:
	//! Make sure this->Values is correct
	Int32       checkValues(Int32 format);
	//!
	//Int32       updateInfos();

protected:
	eFormatType       m_format;
	Int32       m_array_is_mine;
//	Int32       m_type; // ? not used
	Int32		m_size;

	IXMValues<T>      *m_values;
	String      m_heavy_data_name;
	String      m_directory_name;
};

#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMDATANODE_H_ */


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
template<typename T>
IXMDataNode<T>::IXMDataNode() {
	// TODO Auto-generated constructor stub

}

/*---------------------------------------------------------------------------*/
template<typename T>
IXMDataNode<T>::IXMDataNode(XmlNode& parent_node, const String& format, const String& data_type, const String& directory_name, const Integer array_size)
  : m_data_node(this->createFromParent(parent_node, format, data_type,array_size))
	, m_format((format == "xml")?IXM_FORMAT_XML:IXM_FORMAT_HDF)
	, m_array_is_mine(1)
  , m_size(0)
     , m_values(NULL)
	//m_array = NULL;
	, m_heavy_data_name()
	, m_directory_name(directory_name)
{}

/*---------------------------------------------------------------------------*/
template<typename T>
IXMDataNode<T>::~IXMDataNode() {
	// TODO Auto-generated destructor stub
	if(this->m_values) delete this->m_values;
}


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
template<typename T>
Int32
IXMDataNode<T>::checkValues(Int32 format){
    if(this->m_values){
        // Exists
        if(this->m_format != format){
            // Wrong Format
            //debug() << "CheckValues Changing Format";
            delete this->m_values;
            this->m_values = NULL;
        }
    }
    if(!this->m_values){
        // Create One of the Proper Format
        switch (this->m_format) {
            case IXM_FORMAT_HDF :
                this->m_values = new IXMValuesHDF<T>(m_directory_name);
                break;
            case IXM_FORMAT_XML :
                this->m_values = new IXMValuesXML<T>();
                break;
            default :
            	throw FatalErrorException(A_FUNCINFO, "Unsupported Data Format");
                return(RTError);
        }
    }
    if(!this->m_values){
    	throw FatalErrorException(A_FUNCINFO, "Error Creating new XdmfValues");
        return(RTError);
    }

    return(RTOk);
}

/*---------------------------------------------------------------------------*/
/*template<typename T>
Int32 IXMDataNode<T>::updateInfos(){

    String value = m_data_node.attrValue("format");
    // Currently XML or HDF5
    if(value == "hdf"){
        this->setFormat(IXM_FORMAT_HDF);
    } else if(value == "hdf5"){
        this->setFormat(IXM_FORMAT_HDF);
    } else if(value == "h5"){
        this->setFormat(IXM_FORMAT_HDF);
    } else if(value == "xml"){
        this->setFormat(IXM_FORMAT_XML);
    }else if(value.null()){
    	throw NotSupportedException(A_FUNCINFO, "Unsupported data format");
        return(RTError);
    }
    return(RTOk);
}*/

/*---------------------------------------------------------------------------*/
template<typename T>
XmlElement
IXMDataNode<T>::createFromParent( XmlNode& parent_node, const String& format, const String& type, const Integer array_size){
	XmlElement data_node(parent_node,"data");
	data_node.setAttrValue("format",format);
	data_node.setAttrValue("type", type);
	if (array_size != -1) { // la variable est de type tableau
	    StringBuilder str_builder;
	    str_builder += array_size;
	    data_node.setAttrValue("array-size",str_builder.toString());
	}
	return(data_node);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Supported ixm Formats
#include "IXMValuesXML.h"
#include "IXMValuesHDF.h"


/*---------------------------------------------------------------------------*/
template<typename T>
Int32
IXMDataNode<T>::build(Array<T>& array){

    if(this->checkValues(this->m_format) != RTOk){
    	throw FatalErrorException(A_FUNCINFO, "Error Accessing Internal IXMValues");
        return(RTError);
    }
    // don't need switch case here, because m_values is already built
    // just useful in debug mode to print appropriate messages according to formt type
    m_values->setDataNode(this->m_data_node);

    switch (this->m_format) {
        case IXM_FORMAT_HDF :
            //debug() << "Writing Values in HDF Format";
            m_values->setHeavyDataName(this->getHeavyDataName());
            //if(!((IXMValuesHDF *)m_values)->write(array)){
            if(!(m_values)->write(array)){
            	throw FatalErrorException(A_FUNCINFO,"Writing Values Failed");
                return(RTError);
            }
            break;
        case IXM_FORMAT_XML :
            //debug() << "Writing Values in XML Format";
            //if(!((IXMValuesXML *)m_values)->write(array)){
            if(!(m_values)->write(array)){
            	throw FatalErrorException(A_FUNCINFO, "Writing Values Failed");
                return(RTError);
            }
            break;
        default :
        	throw FatalErrorException(A_FUNCINFO, "Unsupported Data Format");
            return(RTError);
    }
    return(RTOk);
}
/*---------------------------------------------------------------------------*/
