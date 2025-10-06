


/*
 * IxmMeshWriter.h
 *
 *  Created on: May 30, 2012
 *      Author: mesriy
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_IXMMESHWRITER_H
#define ARCGEOSIM_MESH_EXPORTERS_IXMMESHWRITER_H

#include <arcane/VariableCollection.h>
#include <arcane/utils/Collection.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/IDataWriter.h>
#include <arcane/IVariable.h>
#include <arcane/IData.h>
#include <arcane/XmlNode.h>
#include <arcane/IIOMng.h>
#include "IXM4FileInfo.h"

#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMItemConnections.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMDataReceiver.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMDataExchanger.h"

#include <map>

class IXMPropertyInfo;

using namespace Arcane;



class IXMMeshWriter
: public Arcane::IDataWriter
, public  ArcGeoSim::IXMDataReceiver
{
 public:

  //! Ecrivain du maillage  => appel par IPostProcessorWriter (notifyBeginWrite() et notifyEndWrite())
  //!
#if (ARCANE_VERSION >= 30003)
  IXMMeshWriter(IMesh* mesh, IGraph2* graph,
#else
  IXMMeshWriter(IMesh* mesh,
#endif
                IXmlDocumentHolder* ixm_doc, const String& directory_name,const eIXMMeshWriteInfo & write_mesh, const bool write_faces_infos, const Integer event_number, bool event_initialized, bool
  write_only_active_cells);
  //!
  virtual ~IXMMeshWriter();
  //!
  void beginMeshWrite(const String& file_name, const String& format, ItemGroupCollection& groups, VariableCollection& vars);
  //!
  void writeMeshToFile(const String& format);
  //!
  void writeCurrentMesh(const String& format, bool first_event);
  //!
  void writeEventToFile(const String& file_name,const Real& begin_time, const Real& end_time, const Integer& event_number);
  //!
  void writeVoidEventToFile(const String& file_name,const Real& begin_time, const Real& end_time, const Integer& event_number);
  //!
  void endMeshWrite();
  //!
  void writeMeshIncrement(const String& format);
  //!
  void writeVoidMesh(const String& format);

  void updateIxmDoc(const Integer& event_number);

  //! Ecrivain des variables : appel par IVariableMng (beginWrite(), write(ivar,idata), endWrite())
  //!
  void beginWrite(const VariableCollection& vars);
  //!
  void endWrite(){}
  //! Positionne les infos des m�ta-donn�es
  //!
  void setMetaData(const String& meta_data){}
  //!
  void setWriteFaceInfos(const bool& write_faces_infos){m_write_faces_infos = write_faces_infos;}
  //!
  //! Ecrit les donn�es \a data de la variable \a var
  //!
  void write(Arcane::IVariable* var,Arcane::IData* data);

  void getMeshType(const Arcane::String& mesh_type);


 protected:
  //!
  const String getGrandParentName(XmlNode currentNode);
 private:

  //!
  void nodeIdsElement(const String& format, XmlNode current_node, String node_ids="node-ids");
  //!
  void clonedNodeIdsElement(const String& format, XmlNode current_node, String node_ids="cloned_node-ids");
  //!
  void nodeCoordsElement(const String format, XmlNode currentNode);
  //!
  void nodeElement(const String& format, XmlNode currentNode);
  //!
  void updateCoordElement(const String& format, XmlNode currentNode);
  //!
  void cloneNodesElement(const String& format, XmlNode currentNode);
  //!
  void cloneNodeElement(const String& format, XmlNode currentNode);
  //!
  void cellElement(const String format, XmlNode currentNode);
  //!
  void deleteCellElement(const String format, XmlNode currentNode);
  //!
  void faceElement(const String format, XmlNode currentNode);
  //!
  void particleCoordsElement(const String format, XmlNode currentNode);
  //!
  void particleElement(const String format, XmlNode currentNode);
  //!
  void connectionElement(const String format, XmlNode currentNode);
  //!
  void writeGroups(XmlNode current_node , const String format);
  //!
  void groupElement(const ItemGroup& group, const String group_name, const String format, XmlNode currentNode); // group_name necessary to handle .own() operation
  //!
  void createIxmFile(const String& file_name, const String& format);
  //! create Ixm doc
  void createIxmDoc();
  //! Load Ixm doc if already created
  void loadIxmDoc();


  //! Update Ixm doc by incrementing events and properties
  void updateIxmDoc(const Real& begin_time, const Real& end_time, const Integer& event_number);


  //! Get base|ixm|hdf5 file names
  const String baseFileName() const;
  const String ixmFileName() const;
  String ixmFileName(const Integer& event_number);
  const String hdfFileName() const;

  ArcGeoSim::IXMDataExchanger* ixmDataExchanger() { return ArcGeoSim::IXMDataExchanger::instance();}
 private:
  IMesh* m_mesh;
#if (ARCANE_VERSION >= 30003)
  IGraph2* const m_graph = nullptr ;
#endif
  String m_directory_name;
  eIXMMeshWriteInfo m_write_mesh;
  Arcane::ITraceMng* m_trace_mng;
  IRessourceMng* m_ressource_mng;
  Arcane::IIOMng* m_io_mng;

  XmlNode* m_ixm_mesh;
  XmlNode* m_ixm_event;
  String m_file_name;
  String m_hdf_path; // relative to ixm file path
  String m_format;

  IXmlDocumentHolder* m_ixm_doc;
  XmlNode m_grid_node;
  XmlNode m_event_node;
  XmlNode m_mesh_node;
  XmlNode m_event_properties_node;
  XmlNode m_mesh_properties_node;

  ItemGroupCollection m_groups;

  const Integer m_event_number; // Given by Post processor. Cannot be modified in writer
  const bool m_events_initialized; // Given by Post processor. Cannot be modified in writer
  bool m_write_only_active_cells;
  bool m_static_mesh;
  String m_default_event_filename;
  String m_supported_version;
  bool m_write_mesh_properties;
  
  Real m_begin_time;

  bool m_write_faces_infos;

  IXM4FileInfo fileinfo;

  ArcGeoSim::ItemConnectionMng m_connection_mng;

  Arcane::String m_mesh_type;
};

#endif /* ARCGEOSIM_MESH_EXPORTERS_IXMMESHWRITER_H_ */
