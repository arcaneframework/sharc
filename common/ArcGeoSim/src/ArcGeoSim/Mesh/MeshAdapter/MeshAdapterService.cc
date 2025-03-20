// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Utils/ArrayUtils.h"
#include "ArcGeoSim/Mesh/Estimator/IEstimator.h"
#include "ArcGeoSim/Mesh/AMRDataTransfer/IAMRDataTransfer.h"
#include "ArcGeoSim/Mesh/MeshAdapter/IMeshAdapter.h"
#include <arcane/ArcaneVersion.h>
#include <arcane/utils/Math.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/ITimeLoopMng.h>
#include <arcane/Timer.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/MathUtils.h>
#include <arcane/IMeshModifier.h>
#include <arcane/IMesh.h>
#include <arcane/IParallelMng.h>
#include <arcane/ItemRefinementPattern.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/VariableTypedef.h>
#include <arcane/IMesh.h>
#include <arcane/ItemTypes.h>
#include <arcane/VariableBuildInfo.h>
#include <boost/shared_ptr.hpp>
#include <arcane/mesh/MeshRefinement.h>
#include <arcane/mesh/DynamicMesh.h>
#include <vector>

#include "MeshAdapter_axl.h"

// TODO ajouter un interpolateur variable
// TODO virer les nommages restrictions P0 ...
// FIXME risque d'utilisation de UserMark1 depuis ArcGeoSim
// TODO normaliser le prefixe des variables globales

class MeshAdapterService
        : public ArcaneMeshAdapterObject
        , public AbstractMeshAdapter
{

public:

  //! Constructeur de la classe
  MeshAdapterService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneMeshAdapterObject(sbi)
  , AbstractMeshAdapter()
  , m_mesh(NULL)
  , m_estimator(NULL)
  , m_amr_data_transfer(NULL)
  , m_refine_on_init(false)
  , m_is_adapt(false)
  , m_is_initialized(false)
  {
    ;
  }

  //! Destructeur
  virtual ~MeshAdapterService();

public:
  
  //====================================================================================================
  // Les m�thodes d'interface
  //====================================================================================================
  void init(IMesh * mesh);
  void update();


  Integer maxLevel() {
    return m_max_level ;
  }

  bool refineOnInit() {
    return m_refine_on_init ;
  }

  void registerUnrefinableCellGroup(CellGroup const& group) ;
  void registerUnrefinableFaceGroup(FaceGroup const& group) ;

  bool compute(VariableCellReal& sol, VariableCellReal& sol_tn, VariableCellReal& error, Real dt);
  bool compute(VariableCellReal& error);
  void adapt() ;
  bool flagRefinedCellGroup(CellGroup const& group) ;
  void refineAllMesh() ;

  IEstimator* getEstimator()
  {
    return m_estimator ;
  }

  void addDataToRegister(String name, VariableFaceReal & data);
  //!
  void addDataToRegister(String name, VariableFaceInteger & data);
  void addDataToRegister(String name, VariableFaceInt64 & data);
 //!
  void addDataToRegister(String name, VariableCellReal & data);
  //!
  void addDataToRegister(String name, VariableCellReal & data, VariableParticleReal & particle_data);
  //!
  void addDataToRegister(String name, VariableCellReal3 & data);
  void addDataToRegister(String name, VariableCellArrayReal3 & data);
  //!
  void addDataToRegister(String name, VariableCellReal3x3 & data);
  //!
  void addDataToRegister(String name, VariableCellInteger & data);
  //!
  void addDataToRegister(String name, VariableCellArrayReal & data);
  void addDataToRegister(String name, VariableFaceArrayReal & data);
  void addExtensiveDataToRegister(String name, VariableCellArrayReal & data);
  //!
  void registerTransportFunctor();

  bool contains(Arcane::String name) const
  {
    return m_amr_data_transfer->contains(name);
  }

  CellGroup const& getJustAddedCells() const {
    return m_added_cells ;
  }

  CellGroup const& getJustCoarsenCells() const {
    return m_coarsen_cells ;
  }

  CellGroup const& getJustRefinedCells() const {
    return m_refined_cells ;
  }

  CellGroup const& getNewActiveCells() const {
    return m_new_active_cells ;
  }



private:
  bool _checkType(Int32 type) ;
  //! convertisseur de l'erreur en flage de raffinement
  bool errorToFlagConverter(VariableCellReal& error);
  //! calcul la liste des particules par mailles
  void computeCellParticlesMap();
  //! update refinement restriction variable
  void updateRestriction();

  enum RefinementRestrictionState { eNoRestriction, eUserRestriction, eNeighborRestriction, eTypeRestriction, eLaterRestriction };

  //! Indique si la maille a subi une restriction
  inline bool isRestricted(const Cell & cell) const;

  /*! retourne true si l'�tat associ� ��la maille � changer */
  inline bool updateCellRestriction(const Cell & cell, const RefinementRestrictionState st);

  //! Retourne les cellules voisines d'une cellule de m�me niveau ou d'un niveau parent
  CellVector neighborCells(const Cell & cell) const;
#if (ARCANE_VERSION>=12201)
  template<int typeID>
  void _addNeighborCellsFromParent(const Cell & cell, Array<Int32>& neighbor_cells, const ItemRefinementPatternT<typeID>& rp) const;
#endif /* ARCANE_VERSION */

  //=====================================================================================================
  // Les variables
  //=====================================================================================================

private:
  //! maillage courant
  IMesh * m_mesh;
  // pas de temps a l'iteration courante
  Real m_dt;

  //! Pr�fixe d'instance des donn�es globales (groupes, variables...)
  String m_prefix;

  //! parametres de raffinement
  Integer m_max_level;
  Real m_refine_percentage;
  Real m_coarsen_percentage;

  // Interface des Estimateur d'erreur
  IEstimator * m_estimator;
  // Interface de l'AMRDataTransfer
  IAMRDataTransfer * m_amr_data_transfer;

  SharedArray<CellGroup> m_unrefinable_cells ;
  SharedArray<FaceGroup> m_unrefinable_faces ;

  CellGroup m_added_cells ;
  CellGroup m_refined_cells ;
  CellGroup m_coarsen_cells ;
  CellGroup m_new_active_cells ;

  boost::shared_ptr<VariableCellInt32> m_refinement_restriction;

  bool m_refine_on_init ;
  bool m_is_adapt ;

  bool m_is_initialized ;

};

//=======================================================================================
// Destructeur
//=======================================================================================

MeshAdapterService::
~MeshAdapterService()
{

}

//==========================================================================================
// initialization
//==========================================================================================
void
MeshAdapterService ::init(IMesh * mesh)
{
  m_mesh = mesh;
  if(m_is_initialized) return ;
  if (options()->aMRDataTransfer.size())
  {
    m_amr_data_transfer = options()->aMRDataTransfer[0];
    m_amr_data_transfer->init();
  }
  if (options()->estimator.size() > 0)
  {
    // FIXME pas de relation en IEstimator et AMRDataModel, comment le dynamic_cast peut fonctionner ?
    m_estimator = options()->estimator[0];
    m_estimator->init(mesh) ;
    IAMRDataModel* ptr = dynamic_cast<IAMRDataModel*> (m_estimator) ;
    if(ptr)
      ptr->registerData(this) ;
  }
  else
  {
    IAppServiceMng* app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());

    m_estimator = app_service_mng->find<IEstimator>(false) ;
    if(m_estimator)
      m_estimator->init(mesh);
  }
  m_max_level = options()->maxLevel();
  m_refine_percentage = options()->refineRatio();
  m_coarsen_percentage = options()->coarsenRatio();

  if(options()->zone.size()>0)
  {
    for(Integer i=0;i<options()->zone.size();++i)
       m_unrefinable_cells.add(options()->zone[i]) ;
  }

  m_prefix = ""; // TODO D�finir une valeur non conflictuelle

  IItemFamily * cell_family = subDomain()->defaultMesh()->cellFamily();
  m_added_cells = cell_family->findGroup(m_prefix + "AMRAddedCells",true);
  m_coarsen_cells = cell_family->findGroup(m_prefix + "AMRCoarsenCells",true);
  m_refined_cells = cell_family->findGroup(m_prefix + "AMRRefinedCells",true);
  m_new_active_cells = cell_family->findGroup(m_prefix + "AMRNewActiveCells",true);

  m_refinement_restriction.reset(new VariableCellInt32(VariableBuildInfo(m_mesh, m_prefix + "RefinementRestriction", cell_family->name(), IVariable::PPrivate)));
  m_refinement_restriction->setUsed(false);

  m_refine_on_init = options()->refineOnInit() ;

  m_is_initialized = true ;
}

void
MeshAdapterService::update()
{
  if(m_estimator)
    m_estimator->update(m_mesh);
}

void
MeshAdapterService::registerUnrefinableCellGroup(CellGroup const& group)
{
  m_unrefinable_cells.add(group) ;
}

void
MeshAdapterService::registerUnrefinableFaceGroup(FaceGroup const& group)
{
  m_unrefinable_faces.add(group) ;
}

//================================================================================================
// computation
//================================================================================================
void
MeshAdapterService::
refineAllMesh()
{
   flagRefinedCellGroup(m_mesh->allActiveCells());
   adapt();
}

bool
MeshAdapterService::
flagRefinedCellGroup(CellGroup const& group) {

  // Mise � jour des marqueurs de restriction de raffinement
  // Pourrait �tre factoris� s'il existait un prologue � la phase de raffinement
  m_refinement_restriction->setUsed(true);
  updateRestriction();

  bool mesh_must_change = false;

  std::map<Int32, Integer> types;
  ENUMERATE_CELL(icell, group.activeCellGroup()) {
    Cell cell = *icell;

    if ((*m_refinement_restriction)[cell] == eTypeRestriction)
      types[cell.type()]++;

    if (!isRestricted(*icell)) {
      if (cell.level() < m_max_level) {
        cell.internal()->setFlags(cell.internal()->flags() | ItemInternal::II_Refine);
        mesh_must_change = true;
      }
    }
  }

  if (types.size() != 0) {
    for (std::map<Int32, Integer>::iterator iter = types.begin(); iter != types.end(); ++iter)
      warning() << "TRY TO REFINE THE UNREFINABLE " << iter->second << " cells of TYPE : " << iter->first;
  }

  m_refinement_restriction->setUsed(false);

  m_mesh_need_adaptation |= mesh_must_change;
  return mesh_must_change;
}

bool MeshAdapterService::_checkType(Int32 type)
{
  switch(type)
  {
    case IT_Hexaedron8 :
#if (ARCANE_VERSION>=12201)
    case IT_Tetraedron4:
    case IT_Pyramid5:
    case IT_Pentaedron6:
    case IT_HemiHexa7:
    case IT_HemiHexa6:
    case IT_HemiHexa5:
    case IT_AntiWedgeLeft6:
    case IT_AntiWedgeRight6:
    case IT_DiTetra5:
#endif /* ARCANE_VERSION */
      return true ;
    default :
      return false ;
  }
}

bool MeshAdapterService::compute(VariableCellReal& sol,
                                 VariableCellReal& sol_tn,
                                 VariableCellReal& error,
                                 Real dt)
{
  m_dt = dt;

  // 1- Calcul du gradient en espace
  //m_estimator->firstOrderSpatialDerivative(sol,error,m_mesh);
  m_estimator->computeDerivative(sol, sol_tn, m_dt, error, m_mesh);

  // 2- On convertit en FLAG
  m_mesh_need_adaptation = errorToFlagConverter(error);

  return m_mesh_need_adaptation ;
}

bool
MeshAdapterService::compute(VariableCellReal& error)
{
  // 2- On convertit en FLAG
  m_mesh_need_adaptation = errorToFlagConverter(error);

  return m_mesh_need_adaptation ;
}

void
MeshAdapterService::adapt()
{
  bool mesh_has_changed = m_mesh->modifier()->adapt();

  if(mesh_has_changed)
  {
      SharedArray<Integer> added_lids ;
      SharedArray<Integer> coarsen_lids ;
      SharedArray<Integer> refined_lids ;
      Integer size = m_mesh->allActiveCells().size() ;
      added_lids.reserve(size) ;
      coarsen_lids.reserve(size) ;
      refined_lids.reserve(size) ;
      ENUMERATE_CELL(icell,m_mesh->allCells())
      {
        Integer lid = icell->localId() ;
        Integer flag = icell->internal()->flags() ;
        if(flag & ItemInternal::II_JustRefined)
        {
          refined_lids.add(lid) ;
        }
        if(flag & ItemInternal::II_JustCoarsened)
        {
          coarsen_lids.add(lid) ;
        }
        if(flag & ItemInternal::II_JustAdded)
        {
          added_lids.add(lid) ;
        }
      }
      m_added_cells.setItems(added_lids) ;
      m_coarsen_cells.setItems(coarsen_lids) ;
      m_refined_cells.setItems(refined_lids) ;
      m_new_active_cells.setItems(added_lids) ;
      m_new_active_cells.addItems(coarsen_lids) ;

      notifyAll() ;
  }
  else
  {
      m_added_cells.clear();
      m_coarsen_cells.clear() ;
      m_refined_cells.clear() ;
      m_new_active_cells.clear() ;
      m_new_active_cells.clear() ;
  }

  info()<<"NUMBER OF JUST ADDED CELLS   : "<<m_added_cells.size();
  info()<<"NUMBER OF JUST REFINED CELLS : "<<m_refined_cells.size();
  info()<<"NUMBER OF JUST COARSEN CELLS : "<<m_coarsen_cells.size();
  info()<<"NUMBER OF NEW ACTIVE CELLS   : "<<m_new_active_cells.size();

  reset() ;
}
//==========================================================================================
// addDataToRegister (scalaire)
//==========================================================================================

void
MeshAdapterService::addDataToRegister(String name, VariableFaceReal & data)
{
  m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableFaceInteger & data)
{
  m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableFaceInt64 & data)
{
  m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableCellReal & data)
{
	m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableCellReal & data, VariableParticleReal & particle_data)
{
  m_amr_data_transfer->addDataToRegister(name,data,particle_data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableCellInteger & data)
{
	m_amr_data_transfer->addDataToRegister(name,data);
}
//==========================================================================================
// addDataToRegister (vecteur)
//==========================================================================================
void
MeshAdapterService::addDataToRegister(String name, VariableCellReal3 & data)
{
	m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableCellArrayReal3 & data)
{
  m_amr_data_transfer->addDataToRegister(name,data);
}
//==========================================================================================
// addDataToRegister (tenseur)
//==========================================================================================
void
MeshAdapterService::addDataToRegister(String name, VariableCellReal3x3 & data)
{
	m_amr_data_transfer->addDataToRegister(name,data);
}
//==========================================================================================
// addDataToRegister (tableau)
//==========================================================================================
void
MeshAdapterService::addDataToRegister(String name, VariableCellArrayReal & data)
{
  m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addDataToRegister(String name, VariableFaceArrayReal & data)
{
  m_amr_data_transfer->addDataToRegister(name,data);
}
void
MeshAdapterService::addExtensiveDataToRegister(String name, VariableCellArrayReal & data)
{
	m_amr_data_transfer->addExtensiveDataToRegister(name,data);
}
//==========================================================================================
// Enregistrement de la fonction de transfer
//==========================================================================================
void
MeshAdapterService::registerTransportFunctor()
{
	m_amr_data_transfer->registerTransportFunctor();
}
//==========================================================================================================
// Flager les erreurs
//==========================================================================================================


bool MeshAdapterService::errorToFlagConverter(VariableCellReal& error)
{
  // Mise � jour des marqueurs de restriction de raffinement
  // Pourrait �tre factoris� s'il existait un prologue � la phase de raffinement
  m_refinement_restriction->setUsed(true);
  updateRestriction();

  ARCANE_ASSERT ((m_refine_percentage >= 0. && m_refine_percentage <= 1.),(""));
  ARCANE_ASSERT ((m_coarsen_percentage >= 0. && m_coarsen_percentage <= 1.),(""));

  error.synchronize() ;

  ENUMERATE_CELL(icell, m_mesh->allCells()) {
    if (isRestricted(*icell)) {
      error[icell] = 0;
    }
  }

  Real error_min = FloatInfo<Real>::maxValue();
  Real error_max = 0.;
  Real error_avg = 0.;

  bool mesh_must_change = false;

  IParallelMng* pm = m_mesh->parallelMng();

  ENUMERATE_CELL(icell,m_mesh->allActiveCells().own())
  {
    if(!isRestricted(*icell))
    {
      error_max = math::max (error_max, error[icell]);
      error_min = math::min (error_min, error[icell]);
      error_avg += error[icell] ;
      //info()<<" ERROR PS["<<icell->uniqueId()<<"]="<<error[icell];
      if(error[icell]<0)
        traceMng()->error()<<"NEGATIVE ERROR : "<<error[icell]<<" on CELL "<<icell->uniqueId();
    }
  }
  Integer nb_cells = m_mesh->allActiveCells().own().size() ;
  Integer total_nb_cells = pm->reduce(Parallel::ReduceSum,nb_cells) ;
  error_max= pm->reduce(Parallel::ReduceMax,error_max);
  error_min= pm->reduce(Parallel::ReduceMin,error_min);
  error_avg= pm->reduce(Parallel::ReduceSum,error_avg);
  error_avg /= total_nb_cells ;

  if(error_max==0) return false ;

  ARCANE_ASSERT ((error_max>0),("the maximum error should be definite positive"));

  //error_max= error_max/error_max;
  //error_min= error_min/error_max;
  // Compute the cutoff values for coarsening and refinement
  //Real error_delta = (1 - error_min);


  //const Real refine_cutoff  = (1.- m_refine_percentage*error_delta); //*error_max;
  //const Real coarsen_cutoff = m_coarsen_percentage*error_delta + error_min;
  //const Real refine_cutoff  = m_refine_percentage*(error_min-error_max) + error_max;

  Real refine_cutoff  = m_refine_percentage*error_avg ;
  Real coarsen_cutoff = m_coarsen_percentage*error_avg ;
  Real error_med = error_avg ;
  if(m_estimator && m_estimator->isNormalized())
  {
    class FormFunc
    {
    public :
      FormFunc(Real prec)
      {
        m_size = 1./prec ;
        m_dx = 1./m_size ;
        m_value.resize(m_size+2) ;
        m_value.assign(m_size+2,0) ;
      }
      void add(Real value) {
        int i = value*m_size ;
        ++m_value[i+1] ;
      }
      Real operator()(Real value) const {
        int n = value*m_value[m_size+1] ;
        for(int i=0;i<m_size+1;++i)
        {
          if(m_value[i]>n)
            return i*m_dx ;
        }
        return 1. ;
      }
      void compute() {
        int n = 0 ;
        for(int i=0;i<m_size+1;++i)
        {
          m_value[i] = n ;
          n += m_value[i+1] ;
        }
        m_value[m_size+1] = n ;
      }
      ArrayView<int> value() {
        return ArrayView<int>(m_size+2,m_value.data()) ;
      }
    private :
      int m_size ;
      Real m_dx ;
      std::vector<int> m_value ;
    };
    Real prec = error_avg/10 ;
    FormFunc func(prec) ;
    ENUMERATE_CELL(icell,m_mesh->allActiveCells().own())
    {
      if(!isRestricted(*icell))
      {
        func.add(error[icell]) ;
      }
    }
    pm->reduce(Parallel::ReduceSum,func.value()) ;
    func.compute() ;
    refine_cutoff = func(1-m_refine_percentage) ;
    coarsen_cutoff = func(m_coarsen_percentage) ;
    error_med = func(0.5) ;
  }
  else
  {
    refine_cutoff = m_refine_percentage *(error_avg-error_max) + error_max;
    coarsen_cutoff = m_coarsen_percentage *(error_avg-error_min) + error_min;
  }

  Integer nb_cells_to_refine = 0 ;
  Integer nb_cells_to_coarse = 0 ;

  Real error_var = 0 ;
  ENUMERATE_CELL(icell,m_mesh->allActiveCells().own()) {
    const Cell & cell = *icell;

    if (!isRestricted(cell)) {
      Real delta_error = error[icell] - error_avg;
      error_var += delta_error * delta_error;
      const Real cell_error = error[icell]; // error[icell]/error_max

      // 1) Deraffiner
      if (cell_error <= coarsen_cutoff) {
        if (cell.level() > 0) {
          cell.internal()->setFlags(cell.internal()->flags() | ItemInternal::II_Coarsen);
          mesh_must_change = true;
          ++nb_cells_to_coarse;
        }
      }

      // 2) Raffiner
      if (cell_error >= refine_cutoff) {
        if (cell.level() < m_max_level) {
          cell.internal()->setFlags(cell.internal()->flags() | ItemInternal::II_Refine);
          mesh_must_change = true;
          ++nb_cells_to_refine;
        }
      }
    }
  }
  SharedArray<Real> buffer(2) ;
  buffer[0] = error_var ;
  buffer[1] = mesh_must_change?1:0 ;
  SharedArray<Integer> ibuffer(2) ;
  ibuffer[0] = nb_cells_to_refine ;
  ibuffer[1] = nb_cells_to_coarse ;
  pm->reduce(Parallel::ReduceSum,buffer) ;
  pm->reduce(Parallel::ReduceSum,ibuffer) ;
  error_var = buffer[0]/total_nb_cells ;
  mesh_must_change = buffer[1]>0 ;

  info()<<"ERROR MAX          : "<<error_max;
  info()<<"ERROR AVG          : "<<error_avg;
  info()<<"ERROR MED          : "<<error_med;
  info()<<"ERROR MIN          : "<<error_min;
  info()<<"ERROR VAR          : "<<math::sqrt(error_var);
  info()<<"REFINE CUT OFF     : "<<refine_cutoff ;
  info()<<"CORSEN CUT OFF     : "<<coarsen_cutoff ;
  info()<<"NB CELLS TO REFINE : "<<ibuffer[0];
  info()<<"NB CELLS TO COARSE : "<<ibuffer[1];

  m_refinement_restriction->setUsed(false);

  return mesh_must_change;
}

void
MeshAdapterService::
updateRestriction()
{
  // D�finition des marqeurs aux cellules:
  // eNoRestriction : pas de restriction de raffinement ni maintenant ni plus tard
  // eLaterRestriction : cellule marqu�e comme � surveiller car son raffinement produira des cellules restreintes (de type eNeighborRestriction)
  // eUserRestriction : restriction par l'utilisateur : ne peut pas �tre raffin�e
  // eNeighborRestriction : restriction par l'algorithme de restriction par voisinage : ne peut plus �tre raffin�e
  // pour la suite : restriction == (eUserRestriction||eNeighborRestriction)

  // A it�rer si n�cessaire (ou bien trouver un ordre d'analyse)
  // 1- si une cellule a une restriction et a des enfants => fatal error
  // 2- Toute cellule ayant une cellule voisine de m�me niveau avec une restriction
  //    - soit a d�j� une restriction => rien � faire
  //    - soit passe sous observation => marqueur eLaterRestriction
  // 3- Toute cellule ayant un marqueur eLaterRestriction doit examiner ses enfants
  //    - si l'une des faces de l'enfant est voisine d'une cellule restreinte d'un niveau plus grossier alors l'enfant devient eNeighborRestriction
  //    - sinon est a son tour marqu�e eLaterRestriction


  // Par d�faut pas de restriction
  m_refinement_restriction->fill(eNoRestriction, m_mesh->allCells());
  // Marqueur eUserRestriction en partant des groupes de d�finition
  for(Integer i=0;i<m_unrefinable_cells.size();++i)
    m_refinement_restriction->fill(eUserRestriction, m_unrefinable_cells[i]);

  for(Integer i=0;i<m_unrefinable_faces.size();++i)
    ENUMERATE_FACE(iface,m_unrefinable_faces[i])
      ENUMERATE_CELL(icell, iface->cells())
        (*m_refinement_restriction)[*icell] = eUserRestriction;

    // Marqueur eTypeRestriction directement sur les feuilles
    ENUMERATE_CELL(icell, m_mesh->allActiveCells()) {
      const bool refinable = _checkType(icell->type());
      if (!refinable && !isRestricted(*icell))
        (*m_refinement_restriction)[*icell] = eTypeRestriction;
    }

  if (options()->enableRestrictionDiffusion()) {
    bool has_changed = false;
    do {
      has_changed = false;
      // On ne calcule que sur les mailles propres + propagation par synchronisation de la variable
      ENUMERATE_CELL(icell, m_mesh->ownCells()) {
        const Cell &cell = *icell;

        // 1- V�rification d'une �ventuelle restriction du parent
        // TODO nbHParent non interfac� depuis Item
        if (cell.internal()->nbHParent() > 0 && isRestricted(cell.hParent().toCell()))
          fatal() << "Restriction violation : cell " << ItemPrinter(cell.hParent().toCell()) << " is restricted and has children";

        if (not isRestricted(cell)) {
          // Recherche du voisinage (niveau courant ou niveau parent)
          CellVector neighbor_cells = neighborCells(cell);
          ENUMERATE_CELL(ineighbor_cell, neighbor_cells) {
            // On saute les voisins non trouv�s (principalement pour des raisons de bord)
            if (ineighbor_cell.localId() == NULL_ITEM_LOCAL_ID)
              continue;

            const Cell &neighbor_cell = *ineighbor_cell;
            if (isRestricted(neighbor_cell)) {
              if (neighbor_cell.level() == cell.level()) {
                // Etat jamais utilis� concr�tement
                // if (not isRestricted(cell)) // le statut a pu passer � eNeighborRestriction pendant l'it�ration des voisins, il faut donc v�rifier
                //   has_changed |= updateCellRestriction(cell, eLaterRestriction);
              } else {
                ARCANE_ASSERT((neighbor_cell.level() + 1 == cell.level()), ("Inconsistent level between neighbor cells"));
                has_changed |= updateCellRestriction(cell, eNeighborRestriction);
              }
            }
          }
        }
      }
      m_refinement_restriction->synchronize();
      has_changed = m_mesh->parallelMng()->reduce(Parallel::ReduceMax, has_changed);
    } while (has_changed);
  }

  // FIXME Do not use II_UserMark1
   ENUMERATE_CELL(icell, m_mesh->allCells()) {
     if (isRestricted(*icell)) {
       icell->internal()->setFlags(icell->internal()->flags() | ItemInternal::II_UserMark1);
     }
   }
}

bool
MeshAdapterService::
isRestricted(const Cell & cell) const {
  const RefinementRestrictionState state = static_cast<RefinementRestrictionState>((*m_refinement_restriction)[cell]);
  return state == eUserRestriction
          or state == eNeighborRestriction
          or state == eTypeRestriction;
}

bool
MeshAdapterService::
updateCellRestriction(const Cell & cell, const RefinementRestrictionState st) {
  Integer & state = (*m_refinement_restriction)[cell];
  if (state != st) {
    state = st;
    return true;
  } else {
    return false;
  }
}

CellVector
MeshAdapterService::
neighborCells(const Cell & cell) const
{
  // TODO Il est possible d'acc�l�rer ce calcul via des pr�calculs sur les types de mailles sous r�serve d'avoir d'information de d�composition (types des sous-mailles).
  FaceVectorView faces = cell.faces();

  SharedArray<Int32> neighbor_cells(faces.size(), NULL_ITEM_LOCAL_ID);

  bool not_found = false;
  ENUMERATE_FACE(iface, faces) {
    const Cell & frontCell = iface->frontCell();
    const Cell & backCell = iface->backCell();
    if (frontCell == cell)
      neighbor_cells[iface.index()] = backCell.localId();
    else if (backCell == cell)
      neighbor_cells[iface.index()] = frontCell.localId();
    else
      not_found = true;
  }

  if (not_found && (not (cell.internal()->nbHParent()>0)) ) {

#if (ARCANE_VERSION>=12201)
    mesh::DynamicMesh* dynamic_mesh = dynamic_cast<mesh::DynamicMesh*>(mesh());
    if (dynamic_mesh == NULL) fatal() << "mesh is not dynamic ";
    mesh::MeshRefinement* mesh_refinement = dynamic_mesh->meshRefinement();
    switch (cell.hParent().type())
    {
      case IT_Tetraedron4:
        _addNeighborCellsFromParent<IT_Tetraedron4>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_Tetraedron4>());
        break;
      case IT_Pyramid5:
        _addNeighborCellsFromParent<IT_Pyramid5>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_Pyramid5>());
        break;
      case IT_Pentaedron6:
        _addNeighborCellsFromParent<IT_Pentaedron6>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_Pentaedron6>());
        break;
      case IT_Hexaedron8:
        _addNeighborCellsFromParent<IT_Hexaedron8>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_Hexaedron8>());
        break;
      case IT_HemiHexa7:
        _addNeighborCellsFromParent<IT_HemiHexa7>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_HemiHexa7>());
        break;
      case IT_HemiHexa6:
        _addNeighborCellsFromParent<IT_HemiHexa6>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_HemiHexa6>());
        break;
      case IT_HemiHexa5:
        _addNeighborCellsFromParent<IT_HemiHexa5>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_HemiHexa5>());
        break;
      case IT_AntiWedgeLeft6:
        _addNeighborCellsFromParent<IT_AntiWedgeLeft6>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_AntiWedgeLeft6>());
        break;
      case IT_AntiWedgeRight6:
        _addNeighborCellsFromParent<IT_AntiWedgeRight6>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_AntiWedgeRight6>());
        break;
      case IT_DiTetra5:
        _addNeighborCellsFromParent<IT_DiTetra5>(cell, neighbor_cells, mesh_refinement->getRefinementPattern<IT_DiTetra5>());
        break;
      default:
        throw FatalErrorException(A_FUNCINFO,"Not supported refinement Item Type");
    }
#else /* ARCANE_VERSION */

    const Cell & hParent = cell.hParent().toCell();
    ItemTypeMng * itemTypeMng = ItemTypeMng::singleton();

    if (cell.type() != IT_Hexaedron8)
      throw NotImplementedException(A_FUNCINFO, "Refinement on non-hexaedron8");
    HexRefinementPattern hrp = itemTypeMng->refinementPattern(hParent.type());

    ItemTypeInfo * parent_type_info = itemTypeMng->typeFromId(hParent.type());
    ItemTypeInfo * child_type_info = itemTypeMng->typeFromId(cell.type());

    const Int32 ichild = hParent.whichChildAmI(cell.internal());
    ARCANE_ASSERT((ichild >= 0),("Invalid child"));

    ENUMERATE_FACE(iface, faces) {
      // D�j� trouv� au niveau fin, cela suffit
      if (neighbor_cells[iface.index()] != NULL_ITEM_LOCAL_ID)
        continue;

      // Un face est incluse dans une face parente ssi tous ces noeuds y sont inclus

      // Marque tous les noeuds parents utilis�s par cette d�finition de face
      ItemTypeInfo::LocalFace local_face = child_type_info->localFace(iface.index());
      ARCANE_ASSERT((local_face.typeId() == iface->type()), ("Inconsistent type with ItemTypeInfo"));
      UniqueArray<bool> current_nodes_of_face(parent_type_info->nbLocalNode(), false);
      Integer nnz_count = 0;
      for (Integer i_parent_node = 0; i_parent_node < parent_type_info->nbLocalNode(); ++i_parent_node)
        for (Integer i_child_node = 0; i_child_node < local_face.nbNode(); ++i_child_node) {
          const bool active_parent_node = (hrp.refine_matrix(ichild, local_face.node(i_child_node), i_parent_node) != 0.0);
          if (active_parent_node && !current_nodes_of_face[i_parent_node]) {
            ++nnz_count;
            current_nodes_of_face[i_parent_node] = active_parent_node;
          }
        }

      // Quelle est la face repr�sent�e dans current_nodes_of_face ?
      // Parcours toutes les faces parentes ayant nnz_count noeuds et v�rifie que tous les noeuds sont dans current_nodes_of_face
      Integer parent_face_found = -1;
      for (Integer i_parent_face = 0; i_parent_face < parent_type_info->nbLocalFace(); ++i_parent_face) {
        ItemTypeInfo::LocalFace local_parent_face = parent_type_info->localFace(i_parent_face);
        if (local_parent_face.nbNode() >= nnz_count) {
          Integer active_count = 0; // compte le nombre de sommets parents actifs dans la definition de la sous-face
          for (Integer i_parent_node=0; i_parent_node < local_parent_face.nbNode(); ++i_parent_node)
            if (current_nodes_of_face[local_parent_face.node(i_parent_node)])
              ++active_count;
          if (active_count == nnz_count) {
            parent_face_found = i_parent_face;
            break;
          }
        }
      }
      if (parent_face_found != -1) {
        Face parent_face = hParent.face(parent_face_found);
        const Cell &frontCell = parent_face.frontCell();
        const Cell &backCell = parent_face.backCell();
        if (frontCell == hParent)
          neighbor_cells[iface.index()] = backCell.localId();
        else if (backCell == hParent)
          neighbor_cells[iface.index()] = frontCell.localId();
      }
    }
#endif /* ARCANE_VERSION */

  }
  return CellVector(m_mesh->cellFamily(), neighbor_cells);
}

#if (ARCANE_VERSION>=12201)
template<int typeID>
void
MeshAdapterService::
_addNeighborCellsFromParent(const Cell & cell, Array<Int32>& neighbor_cells, const ItemRefinementPatternT<typeID>& rp) const
{
  FaceVectorView faces = cell.faces();
  const Cell & hParent = cell.hParent().toCell();

  const Int32 ichild = hParent.whichChildAmI(cell.internal());
  ARCANE_ASSERT((ichild >= 0),("Invalid child"));

  ENUMERATE_FACE(iface, faces) {
    // D�j� trouv� au niveau fin, cela suffit
    if (neighbor_cells[iface.index()] != NULL_ITEM_LOCAL_ID)
      continue;

    if (rp.face_mapping_topo(ichild,iface.index()))
    {
      Integer parent_face_found = rp.face_mapping(ichild,iface.index());
      Face parent_face = hParent.face(parent_face_found);
      const Cell &frontCell = parent_face.frontCell();
      const Cell &backCell = parent_face.backCell();
      if (frontCell == hParent)
        neighbor_cells[iface.index()] = backCell.localId();
      else if (backCell == hParent)
        neighbor_cells[iface.index()] = frontCell.localId();
    }
  }
}
#endif /* ARCANE_VERSION */

//==============================================
// L'enregistrement du service
//==============================================
ARCANE_REGISTER_SERVICE_MESHADAPTER(MeshAdapter,MeshAdapterService);
