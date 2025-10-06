/*
 * IxmPropertyBaseInfo.h
 *
 *  Created on: Jul 4, 2012
 *      Author: mesriy
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMPROPERTYINFO_H_
#define ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMPROPERTYINFO_H_

#include "IXM4FileInfo.h"

#include <limits> // Boundary behavior std tools (to generate NaN)

using namespace Arcane;

class IXMPropertyInfo {

public:
	IXMPropertyInfo();
	virtual ~IXMPropertyInfo();

	static IXMPropertyInfo* create(IMesh* mesh,const String& name,
	                                      const String& family);
	  //! Créé une instance pour la variable \a variable.
	static IXMPropertyInfo* create(IVariable* variable);

	virtual IVariable* variable() const =0;

	virtual void writeProperty(XmlNode current_node, const String& format, IXM4FileInfo & fileinfo, const bool write_only_active_cells = false, const Real& time=std::numeric_limits<Real>::quiet_NaN( )) =0;
	  //! Chemin dans le fichier Hdf5 contenant la valeur de la variable
	  const String& path() const { return m_hdf_path; }
	  //! Positionne le chemin dans le fichier Hdf5 contenant la valeur de la variable
	  void setPath(const String& path,const String& directory_name ) { m_hdf_path = path; m_directory_name = directory_name;}


protected:
	String getDataType(IVariable* var) const;

protected:
	String m_hdf_path;
	String m_directory_name;
	Real m_values_time;

};

#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMPROPERTYBASEINFO_H_ */
