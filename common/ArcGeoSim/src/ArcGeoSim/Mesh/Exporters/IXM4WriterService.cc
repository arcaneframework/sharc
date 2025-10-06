// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * Ixm4WriterService.cc
 *
 *  Created on: Nov 12, 2012
 *      Author: mesriy
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Interface du service
#include <arcane/IPostProcessorWriter.h>

#include "arcane/BasicService.h"
#include <arcane/VariableCollection.h>
#include <arcane/utils/Collection.h>
#include <arcane/utils/List.h>
#include <arcane/ItemGroup.h>
#include <arcane/utils/Enumerator.h>
#include <arcane/PostProcessorWriterBase.h>
#include <arcane/IIOMng.h>
#include <arcane/IParallelMng.h>
#include <arcane/ArcaneVersion.h>
#include <arcane/IRessourceMng.h>
#include "arcane/IXmlDocumentHolder.h"

#include "ArcGeoSim/Mesh/Utils/MeshUtils.h"
#include "ArcGeoSim/Mesh/Utils/VariableUtils.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/IXMMeshWriter.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/IXM4FileInfo.h"
#include "ArcGeoSim/Mesh/Exporters/IHeavyDataFormat.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMMeshBuilderBase.h"

#include "IXM4Writer_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


class IXM4WriterService
: public ArcaneIXM4WriterObject
  {
public:


	/** Constructeur de la classe */
	IXM4WriterService(const Arcane::ServiceBuildInfo & sbi)
	: ArcaneIXM4WriterObject(sbi) {}

	/** Destructeur de la classe */
	~IXM4WriterService() {}

	virtual void build()
	{
		m_ixm_created = false;
		m_event_number = 0;
		IRessourceMng* ressource_mng = mesh()->subDomain()->ressourceMng();
		m_ixm_doc = ressource_mng->createXmlDocument();
	}

	//! Aucun dataWriter est associe a cet ecrivain
	virtual IDataWriter* dataWriter() { return m_writer; }

	//! Notifie qu'une sortie va �tre effectu�e avec les param�tres courants.
	virtual void notifyBeginWrite()
	{
		Trace::Setter trace(traceMng(),"IXM4Writer");
		debug() << "NotifyBeginWrite()";

		_setFileName();
		ItemGroupCollection groups = PostProcessorWriterBase::groups();
		VariableCollection variables = PostProcessorWriterBase::variables();
		Integer index = PostProcessorWriterBase::times().size();
		info() << "NB_VARS=" << variables.count() << " NB_TIMES=" << index;

		m_heavy_data_format = options()->heavyDataFormat();
		eIXM4DataFormat dataformat = m_heavy_data_format->getFormat();

		String format;

		switch (dataformat)
		{
		case FI_Hdf:
			format = "hdf";
			break;
		case FI_Split_Hdf:
			format = "split_hdf";
			break;
		case FI_Xml:
			format = "xml";
			break;
		}

		if ( options()->writeMesh() == WI_MeshByMesh)
		format = "split_hdf";


		//m_create_initial_mesh_element = options()->createInitialMeshElement();
		m_create_initial_mesh_element = false;
		
		//if ( options()->writeMesh() == WI_Once)
		//    write_mesh_once = true;
		    
		bool write_only_active_cells = false; //options()->writeOnlyActiveCells();

#if (ARCANE_VERSION >= 30003)
		m_writer = new IXMMeshWriter(subDomain()->defaultMesh(),ArcGeoSim::Mesh::GraphMng::graph(mesh(),true),
#else
		m_writer = new IXMMeshWriter(subDomain()->defaultMesh(),
#endif
		                             m_ixm_doc,PostProcessorWriterBase::baseDirectoryName(),options()->writeMesh(),
				options()->writeFacesInfos(),m_event_number, m_ixm_created, write_only_active_cells);

		info() << "FORMAT: " << format;

		// Prepare l'ecriture les fichiers XML/HDF contenant les meta-donnees et les donnees (mesh/properties)
		m_writer->beginMeshWrite(m_base_file_name,format,groups,variables);

		// Ecrit le maillage. L'ecriture des proprietes (IXMMeshWriter::write(IVariable*, IData*) est declenchee par Arcane::IVariableMng*
		// (ce qui lui permet eventuellement de filtrer les proprietes ecrites).
		RealConstArrayView my_times = times();
		
		bool first_event;
		if ( m_event_number == 0 )
		   first_event = true;
		else
		    first_event = false;
		
		switch(options()->writeMesh())
		{
		case WI_Never:
			if ( m_event_number >= 0)
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
			}
			break;
		case WI_Once:
			m_writer->setWriteFaceInfos(false);
			m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);				
			if ( m_event_number == 0 )	
			   m_writer->writeCurrentMesh(format,true);
			break;
		case WI_MeshByMesh:
		    //m_writer->setWriteFaceInfos(false);
		    //m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
		    //if ( m_event_number == 0 )
		    if ( m_event_number > 0 )
		        m_writer->updateIxmDoc(m_event_number);
		    m_writer->writeMeshToFile(format);
		    break;
		case WI_MeshByEvent:
			m_writer->setWriteFaceInfos(false);
			m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
			m_writer->writeCurrentMesh(format,first_event);
			break;
		case WI_IncrementMeshByEvent:
			m_writer->setWriteFaceInfos(false);
			if ( m_event_number == 0)
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
				m_writer->writeCurrentMesh(format,true);
			}
			if ( m_event_number >= 1)
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
				m_writer->writeMeshIncrement(format);
			}
			break;
		}


		/*switch(options()->writeMesh())
		{
		case WI_Never:
			if ( m_event_number >= 0)
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
			}
			break;
		case WI_Once:
			if ( m_event_number == 0 )
			{
				m_writer->writeMeshToFile(m_base_file_name);

				m_writer->writeVoidEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
			}
			else
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
			break;
		case WI_MeshByEvent:
			m_writer->setWriteFaceInfos(false);
			//if ( m_event_number >= 0)
			if ( m_event_number == 0 && m_create_initial_mesh_element)
			{
				m_writer->writeMeshToFile(m_base_file_name);
				m_writer->writeVoidEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);				
			}
			else
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
				m_writer->writeCurrentMesh(format);
			}
			break;
		case WI_IncrementMeshByEvent:
			m_writer->setWriteFaceInfos(false);
			if ( m_event_number == 0)
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
				m_writer->writeCurrentMesh(format);
			}
			if ( m_event_number >= 1)
			{
				m_writer->writeEventToFile(m_base_file_name,my_times[m_event_number], my_times[m_event_number], m_event_number);
				m_writer->writeMeshIncrement(format);
			}
			break;
		}*/


	}

	//! Notifie qu'une sortie vient d'�tre effectu�e.
	virtual void notifyEndWrite()
	{
	  Trace::Setter trace(traceMng(),"IXM4Writer");
	  debug() << "NotifyEndWrite()";

		// Finalize write (dump to file)
		m_writer->endMeshWrite();

		// Increment event_number
		++m_event_number;

		// Event global file is created
		m_ixm_created = true;

		delete m_writer;
		m_writer = 0;
	}

	//! Ferme l'�crivain. Apr�s fermeture, il ne peut plus �tre utilis�
	virtual void close() {
		delete m_ixm_doc;
	}

private:
	IXMMeshWriter* m_writer;
	String m_base_file_name;
	Integer m_event_number;
	bool m_is_master;
	bool m_ixm_created;
	bool m_create_initial_mesh_element;
	IHeavyDataFormat * m_heavy_data_format;
	IXmlDocumentHolder* m_ixm_doc;

private:

	void _setFileName()
	{
		const String& base_fname=  options()->baseFileName();
		PostProcessorWriterBase::setBaseFileName(base_fname);
		m_base_file_name = baseFileName();
		if (m_base_file_name.null())
			m_base_file_name = "data";
	}
  };
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_IXM4WRITER(IXM4Writer,IXM4WriterService);
ARCANE_REGISTER_SERVICE_IXM4WRITER(IXM4PostProcessor,IXM4WriterService);
