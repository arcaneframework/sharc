// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* LoadBalanceModule.cc                                  (C) 2000-2008 */
/*                                                                           */
/* Module d'�quilibrage de charge.                                           */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/ArcanePrecomp.h"

#include "arcane/EntryPoint.h"
#include "arcane/ISubDomain.h"
#include "arcane/IParallelMng.h"
#include "arcane/ModuleFactory.h"
#include "arcane/IMeshPartitioner.h"
#include "arcane/ServiceUtils.h"
#include "arcane/CommonVariables.h"
#include "arcane/ITimeStats.h"
#include "arcane/ITimeLoopMng.h"
#include "arcane/Factory.h"
#include "arcane/ITimeHistoryMng.h"
#include "arcane/IMeshSubMeshTransition.h"
#include "arcane/IMeshModifier.h"
#include "arcane/ItemPrinter.h"
#include "arcane/IItemFamily.h"

#include "LoadBalance_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Module d'�quilibrage de charge
 */
class LoadBalanceModule
: public ArcaneLoadBalanceObject
{
 public:

  LoadBalanceModule(const ModuleBuildInfo& mb);
  ~LoadBalanceModule();

 public:

  virtual VersionInfo versionInfo() const { return VersionInfo(1,0,0); }

 public:

  void checkLoadBalance();
  void loadBalanceInit();

 private:

  VariableScalarReal m_elapsed_computation_time;
  /*! \brief Temps de calcul depuis le dernier �quilibrage
   * Note: cette valeur doit �tre synchronis�e.
   */
  Real m_computation_time;
  SharedArray<float> m_cells_weight;

 private:

  void _checkInit();
  Real _computeImbalance();
  void _computeWeights(RealConstArrayView compute_times,Real max_compute_time);
  void _computeWeights2(RealConstArrayView compute_times,Real max_compute_time);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_MODULE_LOADBALANCE(LoadBalanceModule);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LoadBalanceModule::
LoadBalanceModule(const ModuleBuildInfo& mb)
: ArcaneLoadBalanceObject(mb)
, m_elapsed_computation_time(VariableBuildInfo(this,"LoadBalanceElapsedComputationTime",
                                               IVariable::PNoDump|IVariable::PNoRestore))
, m_computation_time(0.0)
{
  ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LoadBalanceModule::
~LoadBalanceModule()
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void LoadBalanceModule::
loadBalanceInit()
{
  m_elapsed_computation_time = 0;
  _checkInit();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void LoadBalanceModule::
_checkInit()
{
  if (options()->period()==0 || !options()->active()){
    info() << "Equilibrage de charge d�sactiv�.";
    return;
  }

  if (!subDomain()->parallelMng()->isParallel()){
    info() << "Equilibrage de charge demand� mais inactif lors d'une "
           << "ex�cution s�quentielle";
    return;
  }
  
  info() << "Equilibrage de charge actif avec d�s�quilibre maximal: "
         << options()->maxImbalance();
  // Indique au maillage qu'il peut �voluer
  defaultMesh()->modifier()->setDynamic(true);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void LoadBalanceModule::
checkLoadBalance()
{
  ISubDomain* sd = subDomain();
  Integer global_iteration = sd->commonVariables().globalIteration();
  int period = options()->period();
  if (period==0)
    return;
  if (global_iteration==0)
    return;
  if ((global_iteration % period) != 0)
    return;
  
  Real imbalance = _computeImbalance();

  if (!options()->active())
    return;
  if (imbalance<options()->maxImbalance())
    return;
  if (m_computation_time<((Real)options()->minCpuTime()))
    return;

  m_computation_time = 0;
  info() << "Programme un repartitionnement du maillage";
  IMeshPartitioner* p = options()->partitioner();
  _computeWeights2(p->computationTimes(),p->maximumComputationTime());
  p->setCellsWeight(m_cells_weight,1);
  subDomain()->timeLoopMng()->registerActionMeshPartition(options()->partitioner());
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Calcule le poids de chaque maille et le range dans m_cells_weight
 */
void LoadBalanceModule::
_computeWeights(RealConstArrayView compute_times,Real max_compute_time)
{
  ISubDomain* sd = subDomain();
  IMesh* mesh = this->mesh();
  IParallelMng* pm = sd->parallelMng();
  Integer nb_sub_domain = pm->nbSubDomain();
  CellGroup own_cells = mesh->ownCells();
  IntegerSharedArray global_nb_own_cell(nb_sub_domain);
  Integer nb_own_cell = own_cells.size();
  pm->allGather(IntegerConstArrayView(1,&nb_own_cell),global_nb_own_cell);

  // compute_times[0] contient le temps global (sans tracking)
  // compute_times[1] contient le temps de tracking
  //Real max_compute_time = maximumComputationTime();
  //RealConstArrayView compute_times = computationTimes();
  bool has_compute_time = compute_times.size()!=0;
  bool has_cell_time = compute_times.size()==2;
  if (math::isZero(max_compute_time))
    max_compute_time = 1.0;
  Real compute_times0 = 1.0;
  Real compute_times1 = 0.0;
  if (has_compute_time){
    compute_times0 = compute_times[0];
    if (has_cell_time)
      compute_times1 = compute_times[1];
  }

  bool dump_info = true;

  Real time_ratio = compute_times0 / max_compute_time;
  Real time_ratio2 = compute_times1 / max_compute_time;

  if (dump_info){
    info() << " MAX_COMPUTE=" << max_compute_time;
    info() << " COMPUTE 0=" << compute_times0;
    info() << " COMPUTE 1=" << compute_times1;
    info() << " TIME RATIO 0=" << time_ratio;
    info() << " TIME RATIO 2=" << time_ratio2;
  }
  //Real mult_ratio = (1000.0 * total_nb_cell) / (max_compute_time * nb_sub_domain);
  //Real mult_ratio = (1000.0) / (max_compute_time);

  //Real sum_cells_time = 1.0;
  //Real max_cell_time = 0.0;
  //Real cell_ratio = compute_times1 / sum_cells_time;
  //ENUMERATE_CELL(iitem,own_cells){
    //Real cell_time = m_cells_times[iitem];
    //cell_time *= cell_ratio;
    //if (cell_time>max_cell_time)
      // max_cell_time = cell_time;
    //}
  //Real proportional_time = compute_times[0] / (max_compute_time * nb_own_cell);
  Real proportional_time = compute_times0 / (nb_own_cell+1);
  //Real tracking_max_cell_time = max_cell_time;
  //max_cell_time = proportional_time;
  //Real total_max_cell_time = pm->reduce(Parallel::ReduceMax,max_cell_time);

  if (dump_info){
    info() << " PROPORTIONAL TIME=" << proportional_time;
  }

  Real max_weight = 0.0;
  IItemFamily* cell_family = mesh->cellFamily();
  m_cells_weight.resize(cell_family->maxLocalId());
  ENUMERATE_CELL(iitem,own_cells){
    Integer index = iitem.index();
    const Cell& cell = *iitem;
    Real v0 = proportional_time;
    Real w = (v0);
    if (dump_info && index<10){
      info() << "Weight " << ItemPrinter(cell)
             << " v0=" << v0
             << " w=" << w;
    }
    m_cells_weight[index] = w;
    if (w>max_weight)
      max_weight = w;
  }

  Real total_max_weight = pm->reduce(Parallel::ReduceMax,max_weight);
  if (math::isZero(total_max_weight))
    total_max_weight = 1.0;

  if (dump_info){
    info() << " TOTAL MAX WEIGHT=" << total_max_weight;
  }

  ENUMERATE_CELL(iitem,own_cells){
    m_cells_weight[iitem.index()] /= total_max_weight;
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Calcule le poids de chaque maille et le range dans m_cells_weight
 */
void LoadBalanceModule::
_computeWeights2(RealConstArrayView compute_times,Real max_compute_time)
{
  ISubDomain* sd = subDomain();
  IMesh* mesh = this->mesh();
  IParallelMng* pm = sd->parallelMng();
  Integer nb_sub_domain = pm->nbSubDomain();
  CellGroup own_cells = mesh->ownCells();
  IntegerSharedArray global_nb_own_cell(nb_sub_domain);
  Integer nb_own_cell = own_cells.size();
  pm->allGather(IntegerConstArrayView(1,&nb_own_cell),global_nb_own_cell);

  // compute_times[0] contient le temps global (sans tracking)
  // compute_times[1] contient le temps de tracking
  bool has_compute_time = compute_times.size()!=0;
  bool has_cell_time = compute_times.size()==2;
  if (math::isZero(max_compute_time))
    max_compute_time = 1.0;
  Real compute_times0 = 1.0;
  Real compute_times1 = 0.0;
  if (has_compute_time){
    compute_times0 = compute_times[0];
    if (has_cell_time)
      compute_times1 = compute_times[1];
  }

  //Real time_ratio = compute_times0 / max_compute_time;
  //Real time_ratio2 = compute_times1 / max_compute_time;

  const Real eps = 0.2;
  const Real cost_factor = 10;
  Integer count = 0;

  IItemFamily* cell_family = mesh->cellFamily();
  m_cells_weight.resize(cell_family->maxLocalId());

  ENUMERATE_CELL(iitem,own_cells){
    if (m_gas_saturation[iitem] > eps and m_gas_saturation[iitem] < 1-eps) 
      {
        m_cells_weight[iitem.index()] = 1.;
        ++count;
      }
    else
      {
        m_cells_weight[iitem.index()] = 1. / cost_factor;
      }
  }

  info() << "Fake diphasic flash count : " << count << " / " << own_cells.size();

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Real LoadBalanceModule::
_computeImbalance()
{
  //TODO: rendre la m�thode compatible avec le retour-arri�re
  ITimeStats* time_stats = subDomain()->timeStats();
  IParallelMng* pm = subDomain()->parallelMng();

  // Temps �coul� depuis le d�but de l'ex�cution
  Real elapsed_computation_time = time_stats->elapsedTime(TP_Computation);
  Real computation_time = elapsed_computation_time - m_elapsed_computation_time();

  m_elapsed_computation_time = elapsed_computation_time;

  if (options()->statistics()){
    // Optionnel:
    // R�cup�re le temps de calcul de chaque sous-domaine pour en sortir
    // les historiques.
    // TODO: dans ce cas, le reduce standard pour le min et le max est
    // inutilise -> le supprimer
    Integer nb_sub_domain = pm->commSize();
    RealSharedArray compute_times(nb_sub_domain);
    Real my_time = computation_time;
    RealConstArrayView my_time_a(1,&my_time);
    pm->allGather(my_time_a,compute_times);
    ITimeHistoryMng* thm = subDomain()->timeHistoryMng();
    thm->addValue("SubDomainComputeTime",compute_times);
  }

  Real reduce_times[2];
  reduce_times[0] = computation_time;
  reduce_times[1] = -computation_time;
  pm->reduce(Parallel::ReduceMin,RealArrayView(2,reduce_times));
  Real min_computation_time = reduce_times[0];
  Real max_computation_time = -reduce_times[1];
  if (math::isZero(max_computation_time))
    max_computation_time = 1.;
  if (math::isZero(min_computation_time))
    min_computation_time = 1.;
  RealSharedArray computation_times(1);
  computation_times[0] = computation_time;

  m_computation_time += max_computation_time;
  
  Real ratio = computation_time / max_computation_time;
  Real imbalance = (max_computation_time - min_computation_time) / min_computation_time;
  info() << "Temps de calcul consomm� (" << pm->commRank() << ") :"
         << " nb_owncell=" << ownCells().size()
         << " current=" << computation_time
         << " min=" << min_computation_time
         << " max=" << max_computation_time
         << " ratio=" << ratio
         << " imbalance=" << imbalance
         << " full-compute=" << m_computation_time;

  IMeshPartitioner* p = options()->partitioner();
  p->setMaximumComputationTime(max_computation_time);
  p->setComputationTimes(computation_times);
  p->setImbalance(imbalance);
  p->setMaxImbalance(options()->maxImbalance());
  return imbalance;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
