/*
 * XdmfPropertyBaseInfo.h
 *
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFPROPERTYINFO_H_
#define ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFPROPERTYINFO_H_

using namespace Arcane;

class XDMFPropertyInfo {

public:
	XDMFPropertyInfo();
	virtual ~XDMFPropertyInfo();

	static XDMFPropertyInfo* create(IMesh* mesh,const String& name,
			const String& family);

	//! Créé une instance pour la variable \a variable.
	static XDMFPropertyInfo* create(IVariable* variable);

	virtual IVariable* variable() const =0;

	virtual void writeProperty(XmlNode current_node, const String& format,const Real& time=-1.0) =0;

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

#endif /* ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFPROPERTYBASEINFO_H_ */
