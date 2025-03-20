#ifndef ARCGEOSIM_MESH_EXPORTERS_IXMVALUESXML_H
#define ARCGEOSIM_MESH_EXPORTERS_IXMVALUESXML_H
/*
 * IxmValuesXML.h
 *
 *  Created on: May 30, 2012
 *      Author: mesriy
 */

#include <arcane/utils/OStringStream.h>

#include "IXMValues.h"

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include "ArcGeoSim/Mesh/Exporters/IXM4/Formatter.h"

template<typename T>
class IXMValuesXML : public IXMValues<T> {

public :

  IXMValuesXML();
  virtual ~IXMValuesXML();


  void setDataNode(Arcane::XmlNode data_node);

  void setHeavyDataName(const String& hdata_name);

  //! Write the Array to the External Representation
  bool write(Array<T> & anArray){

	  String toReturn;

	    OStringStream   ss;
	    ss() << "\n              ";
	    for(Integer i=0,is=anArray.size();i<is;i++){
	        ss() << std::setprecision(10) << anArray[i] << " ";
	    }
	    ss() << "\n          ";
	    toReturn = ss.str();


	    this->m_data_node.setValue(toReturn);
	    return true;
	}


  //! Write the Array to the External Representation
    bool write(Array<T> & anArray, const Formatter<T>& formatter){

  	    String toReturn = formatter.apply(anArray);

  	    this->m_data_node.setValue(toReturn);
  	    return true;
  	}
};

/*---------------------------------------------------------------------------*/
template<typename T>
IXMValuesXML<T>::IXMValuesXML(){}

/*---------------------------------------------------------------------------*/
template<typename T>
IXMValuesXML<T>::~IXMValuesXML(){}

/*---------------------------------------------------------------------------*/
template<typename T>
void IXMValuesXML<T>::setDataNode(XmlNode data_node)
{
	this->m_data_node=data_node;
}

/*--------------------------------------------------------------*/
template<typename T>
void IXMValuesXML<T>::setHeavyDataName(const String& hdata_name) {
		this->m_heavy_data_name = hdata_name;
	};


//! Handle Real3 peculiarity

template<>
inline bool IXMValuesXML<Real3>::write(Array<Arcane::Real3> & anArray){
	OStringStream   ss;

  ss() << "\n";
  for(Integer i=0,is=anArray.size();i<is;i++){
      ss() << std::setprecision(10) << anArray[i].x << " " << std::setprecision(10) << anArray[i].y << " " << std::setprecision(10) << anArray[i].z << " " ;
  }
  ss() << "\n";
  String toReturn = ss.str();

  this->m_data_node.setValue(toReturn);
  return true;
}



#endif /* ARCGEOSIM_MESH_EXPORTERS_IXMVALUESXML_H */
