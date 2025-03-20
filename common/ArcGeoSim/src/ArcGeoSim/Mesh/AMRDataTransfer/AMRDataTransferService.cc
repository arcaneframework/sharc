// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include <arcane/utils/Math.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/MathUtils.h>
#include <arcane/IMeshModifier.h>
#include <arcane/IParallelMng.h>
#include <arcane/mesh/ParticleFamily.h>
#include <arcane/ItemTypes.h>
#include <arcane/Item.h>
#include <arcane/VariableTypedef.h>
#include <arcane/VariableBuildInfo.h>
#include <arcane/utils/AMRComputeFunction.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/mesh/MeshRefinement.h>
#include <arcane/mesh/DynamicMesh.h>

#include "ArcGeoSim/Mesh/AccessorToolsMng/IMeshAccessorToolsMng.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshTools.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshCellAccessor.h"
#include "ArcGeoSim/Mesh/AMRDataTransfer/TypesInterpolator.h"
#include "ArcGeoSim/Mesh/AMRDataTransfer/DataToTransfer.h"
#include "ArcGeoSim/Mesh/Mapping/IParticleMapping.h"
#include "ArcGeoSim/Mesh/AMRDataTransfer/IAMRDataTransfer.h"

#include <map>
#include <boost/shared_ptr.hpp>

#include "AMRDataTransfer_axl.h"

class AMRDataTransferService:
public ArcaneAMRDataTransferObject
{

public:

        /** Constructeur de la classe */
        AMRDataTransferService(const ServiceBuildInfo & sbi)
        : ArcaneAMRDataTransferObject(sbi)
        , m_mesh_tools(mesh())
        {
                ;
        }


        virtual ~AMRDataTransferService();          // Destructeur

public:

        //====================================================================================================
        // Les m�thodes de l'interface
        //====================================================================================================
        //!
        virtual void init();
        //!
        virtual void addDataToRegister(String name, VariableFaceReal & data);
        virtual void addDataToRegister(String name, VariableFaceInteger & data);
        virtual void addDataToRegister(String name, VariableFaceInt64 & data);
        virtual void addDataToRegister(String name, VariableCellReal & data);
        virtual void addDataToRegister(String name, VariableCellReal & data, VariableParticleReal & particle_data);
        virtual void addDataToRegister(String name, VariableCellInteger & data);
        //!
        virtual void addDataToRegister(String name, VariableCellReal3 & data);
        virtual void addDataToRegister(String name, VariableCellArrayReal3 & data);
                //!
        virtual void addDataToRegister(String name, VariableCellReal3x3 & data);
        //!
        virtual void addDataToRegister(String name, VariableCellArrayReal & data);
        virtual void addDataToRegister(String name, VariableFaceArrayReal & data);
        virtual void addExtensiveDataToRegister(String name, VariableCellArrayReal & data);
        //!
        virtual void registerTransportFunctor();

private:
        //!
        void transportFunction(Array<ItemInternal*>& old_items, AMROperationType op);
        //!
        void  upscaleDataFunctionP0(Array<ItemInternal*>& old_items);
        //!
        void  downscaleDataFunctionP0(Array<ItemInternal*>& old_items);

        // Rm: les m�thodes suivantes pourront �tre utiles dans le cas d'une interpolation d'ordre �lev�
        //void transportScalarDataP0(Array<Cell>& old_items, AMROperationType op, VariableCellReal & variable);
        //void transportVectorDataP0(Array<Cell>& old_items, AMROperationType op, VariableCellReal3 & variable);
        //void transportTensorDataP0(Array<Cell>& old_items, AMROperationType op, VariableCellReal3x3 & variable);

        // Renvoie une donn�e � transf�rer
    DataToTransfer * data(String name) {
        DataToTransferMap::iterator it = m_data_to_transfer.find(name);

        if(it == m_data_to_transfer.end()) {
            throw FatalErrorException("Data doesn't exist");
        }
        return it->second.get();
    }

    bool contains(Arcane::String name) const
    {
      return m_data_to_transfer.find(name) != m_data_to_transfer.end();
    }


        // Renvoie une donn�e (scalaire, vecteur, tenseur ou tableau)
        template<typename SpecificData>
        typename SpecificData::Support& dataT(String name) {
                return dynamic_cast<SpecificData*>(data(name))->data();
        }

        //=====================================================================================================
        // Les variables
        //=====================================================================================================

        //! Etat d'initialisation du service
        bool m_initialized = false ;

        MeshTools m_mesh_tools ;

        // Type d'interpolation
        TypesInterpolator::eTypesInterpolator m_interplator_type;

        // Registre des donn�es
        typedef std::map<String, boost::shared_ptr<DataToTransfer> > DataToTransferMap;
        DataToTransferMap m_data_to_transfer;

        // liste des particules par mailles
        bool m_use_particles = false ;
        IItemFamily* m_particle_family = nullptr ;
        std::map<ItemInternal*,ParticleVector> m_cell_to_particles;
        Real m_epsilon = 0. ;

};

//=======================================================================================
// Destructeur
//=======================================================================================

AMRDataTransferService::~AMRDataTransferService()
{

}

//==========================================================================================
// init
//==========================================================================================
void AMRDataTransferService::init()
{
  if (m_initialized) return ;

  m_interplator_type = options()->interpolatorType();

  m_particle_family = mesh()->findItemFamily("Particle");
  if (m_particle_family)
  {
    m_use_particles = options()->useParticles();
    if (m_use_particles)
    {
      MeshCellAccessor* acc = new MeshCellAccessor(mesh());
      OrthonormalBoundingBox bbox = acc->getBoundingBox();
      m_epsilon = math::pow(10., -15.) * (bbox.getXMax() - bbox.getXMin()) * (bbox.getYMax() - bbox.getYMin())
          * (bbox.getZMax() - bbox.getZMin());
      delete acc ;
    }
  }
  m_initialized = true ;
}

//==========================================================================================
// addDataToRegister (scalaire)
//==========================================================================================
void AMRDataTransferService::addDataToRegister(String name, VariableFaceReal & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
#if (ARCANE_VERSION>=12201)
  mesh::DynamicMesh* dynamic_mesh = dynamic_cast<mesh::DynamicMesh*>(mesh());
  ARCANE_ASSERT((dynamic_mesh != NULL), ("mesh is not dynamic "));
  m_data_to_transfer[name].reset(new VariableFaceRealDataToTransfer(name, data, dynamic_mesh->meshRefinement()));
#else /* ARCANE_VERSION */
  m_data_to_transfer[name].reset(new VariableFaceRealDataToTransfer(name, data));
#endif /* ARCANE_VERSION */
}
void AMRDataTransferService::addDataToRegister(String name, VariableFaceInteger & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
#if (ARCANE_VERSION>=12201)
  mesh::DynamicMesh* dynamic_mesh = dynamic_cast<mesh::DynamicMesh*>(mesh());
  ARCANE_ASSERT((dynamic_mesh != NULL), ("mesh is not dynamic "));
  m_data_to_transfer[name].reset(new VariableFaceIntegerDataToTransfer(name, data, dynamic_mesh->meshRefinement()));
#else /* ARCANE_VERSION */
  m_data_to_transfer[name].reset(new VariableFaceIntegerDataToTransfer(name, data));
#endif /* ARCANE_VERSION */
}
void AMRDataTransferService::addDataToRegister(String name, VariableFaceInt64 & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
#if (ARCANE_VERSION>=12201)
  mesh::DynamicMesh* dynamic_mesh = dynamic_cast<mesh::DynamicMesh*>(mesh());
  ARCANE_ASSERT((dynamic_mesh != NULL), ("mesh is not dynamic "));
  m_data_to_transfer[name].reset(new VariableFaceInt64DataToTransfer(name, data, dynamic_mesh->meshRefinement()));
#else /* ARCANE_VERSION */
  m_data_to_transfer[name].reset(new VariableFaceInt64DataToTransfer(name, data));
#endif /* ARCANE_VERSION */
}
void AMRDataTransferService::addDataToRegister(String name, VariableCellReal & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellRealDataToTransfer(name, data));
}
void AMRDataTransferService::addDataToRegister(String name, VariableCellReal & data, VariableParticleReal & particle_data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  if (m_use_particles)
  {
    m_data_to_transfer[name].reset(new VariableParticleCellRealDataToTransfer(name, data, particle_data, m_cell_to_particles));
  }
  else
  {
    m_data_to_transfer[name].reset(new VariableCellRealDataToTransfer(name, data));
  }
}
void AMRDataTransferService::addDataToRegister(String name, VariableCellInteger & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellIntegerDataToTransfer(name, data));
}

//==========================================================================================
// addDataToRegister (vecteur)
//==========================================================================================
void AMRDataTransferService::addDataToRegister(String name, VariableCellReal3 & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellReal3DataToTransfer(name, data));
}
void AMRDataTransferService::addDataToRegister(String name, VariableCellArrayReal3 & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellArrayReal3DataToTransfer(name, data));
}

//==========================================================================================
// addDataToRegister (tenseur)
//==========================================================================================
void AMRDataTransferService::addDataToRegister(String name, VariableCellReal3x3 & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellReal3x3DataToTransfer(name, data));
}

//==========================================================================================
// addDataToRegister (tableau)
//==========================================================================================
void AMRDataTransferService::addDataToRegister(String name, VariableCellArrayReal & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellArrayRealDataToTransfer(name, data));
}
void AMRDataTransferService::addDataToRegister(String name, VariableFaceArrayReal & data)
{
  mesh::DynamicMesh* dynamic_mesh = dynamic_cast<mesh::DynamicMesh*>(mesh());
  ARCANE_ASSERT((dynamic_mesh != NULL), ("mesh is not dynamic "));
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableFaceArrayRealDataToTransfer(name, data,dynamic_mesh->meshRefinement()));
}
void AMRDataTransferService::addExtensiveDataToRegister(String name, VariableCellArrayReal & data)
{
  ARCANE_ASSERT((m_data_to_transfer.find(name) == m_data_to_transfer.end()),("Data already exists"));
  m_data_to_transfer[name].reset(new VariableCellArrayRealExtensiveDataToTransfer(name, data));
}

//==========================================================================================
// compute
//==========================================================================================
void AMRDataTransferService::registerTransportFunctor()
{
  IAMRTransportFunctor* f = NULL;

  switch (m_interplator_type)
  {
    case TypesInterpolator::P0:
      f = new AMRComputeFunction(this, &AMRDataTransferService::transportFunction);
      break;
    default:
      info() << "case not yet implemented ";
      break ;
  }
  if(f) subDomain()->defaultMesh()->modifier()->registerCallBack(f);

}

//==========================================================================================================
// transport des donn�es
// Dans le cas d'une interpolation d'ordre 0, une seule m�thode suffit pour les variables
// scalaires, vectorielles et tensorielles.
//==========================================================================================================
void AMRDataTransferService::transportFunction(Array<ItemInternal*>& old_items, AMROperationType op)
{

  switch (op)
  {
    // Restriction
    case Restriction: // upscaleDataFunctionP0
      upscaleDataFunctionP0(old_items);
      break;

      // Prolongation
    case Prolongation: // downScaleFunctionP0
      downscaleDataFunctionP0(old_items);
      break;
    default:
      throw FatalErrorException(A_FUNCINFO,
          String::format("No callback function should be called with this operation {1}", op));
  }

}

//==========================================================================================================
// Upscaling des donn�es d'ordre 0
// Dans le cas d'une interpolation d'ordre 0, une seule m�thode suffit pour les variables
// scalaires, vectorielles et tensorielles. Pour une interpolation d'ordre plus �lev�, trois
// m�thodes distinctes pourraient s'av�rer n�cessaires.
//==========================================================================================================
void AMRDataTransferService::upscaleDataFunctionP0(Array<ItemInternal*>& old_items)
{
  if (m_use_particles)
  {
    std::map<Integer,ItemInternal*> child_to_parent;
    for (Integer i = 0; i < old_items.size(); i++)
    {
      ItemInternal* parent=old_items[i];
      UInt32 nb_children = parent->nbHChildren() ;
      for (UInt32 j = 0; j < nb_children; j++)
      {
        child_to_parent[Cell(parent).hChild(j).localId()] = parent;
      }
    }

    ENUMERATE_PARTICLE(iparticle,m_particle_family->view())
    {
      const Cell& cell = iparticle->cell();
      std::map<Integer,ItemInternal*>::iterator parent = child_to_parent.find(cell.localId());
      if (parent != child_to_parent.end())
      {
        m_particle_family->toParticleFamily()->setParticleCell(*iparticle, (Cell) (*parent).second);
        //cout << "Particle upscale:"<<iparticle->localId()<< " from cell :"<<cell.uniqueId()<<" to :"<<((Cell) (*parent).second).uniqueId()<< endl;
      }
    }
  }

  // Transport de la variable
  DataToTransferMap::iterator it;
  for (it = m_data_to_transfer.begin(); it != m_data_to_transfer.end(); ++it)
  {
    (*it).second->upscaleP0(old_items) ;
  }
}

//==========================================================================================================
// Downscaling des donn�es d'ordre 0
// Dans le cas d'une interpolation d'ordre 0, une seule m�thode suffit pour les variables
// scalaires, vectorielles et tensorielles. Pour une interpolation d'ordre plus �lev�, trois
// m�thodes distinctes pourraient s'av�rer n�cessaires.
//==========================================================================================================
void AMRDataTransferService::downscaleDataFunctionP0(Array<ItemInternal*>& old_items)
{
  if (m_use_particles)
  {

    VariableParticleReal3 particle_coords_variable(VariableBuildInfo(mesh(), "ParticleCoord", "Particle"));
    Real3 particle_coord;

    m_cell_to_particles.clear();
    ENUMERATE_PARTICLE(ipart,m_particle_family->view())
    {
      const Cell& cell = ipart->cell();
      particle_coord= particle_coords_variable[ipart];
      const Integer nb_children = cell.nbHChildren();
      if (nb_children > 0)
      {
        //cout << "Particle downscale:"<<ipart->localId()<< " from cell :"<<cell.uniqueId() ;
        for (Integer i = 0, is = nb_children; i < is ; i++)
        {
          const Cell& child = cell.hChild(i).toCell();
          if (m_mesh_tools.isInCell(child, particle_coord, m_epsilon))
          {
            m_particle_family->toParticleFamily()->setParticleCell(*ipart, child);
            m_cell_to_particles[child.internal()].addItem(*ipart);
            //cout <<" to :"<<child.uniqueId()<< endl;
            break;
          }
        }
      }
    }
  }

  DataToTransferMap::iterator it;
  for (it = m_data_to_transfer.begin(); it != m_data_to_transfer.end(); ++it)
  {
    (*it).second->interpolateP0(old_items) ;
  }
}

//==============================================
// L'enregistrement du service
//==============================================
ARCANE_REGISTER_SERVICE_AMRDATATRANSFER(AMRDataTransfer, AMRDataTransferService);
