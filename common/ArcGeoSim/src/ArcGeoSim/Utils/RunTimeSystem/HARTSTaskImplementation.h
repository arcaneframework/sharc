// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* HARTSTaskImplementation.cc                                    (C) 2000-2010 */
/*                                                                           */
/* Impl�mentation des t�ches utilisant TBB (Intel Threads Building Blocks).  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifndef HARTSIMPL_H_
#define HARTSIMPL_H_

#include "arcane/utils/ArcanePrecomp.h"

#include "arcane/utils/IThreadImplementation.h"
#include "arcane/utils/NotImplementedException.h"
#include "arcane/utils/IFunctor.h"
#include "arcane/utils/Mutex.h"

#include "arcane/FactoryService.h"
#include "arcane/Concurrency.h"


#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/ExceptionUtils.h"

#ifdef USE_HARTS
#include "HARTS/HARTS.h"
#endif

#include "ArcGeoSim/Utils/RunTimeSystem/IRunTimeSystemMng.h"
#include "ArcGeoSim/Utils/RunTimeSystem/HARTSThreadEnv.h"

#include <arcane/utils/PlatformUtils.h>
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
class HARTSTaskImplementation ;
class HARTSTaskImplementation
: public ITaskImplementation
{
  typedef Arcane::ItemVectorView                               VectorType ;

  typedef RunTimeSystem::DataHandler                           DataHandlerType ;
  typedef RunTimeSystem::DataArgs<DataHandlerType>             DataArgsType ;

  static bool const                                            sequential = true ;
  typedef RunTimeSystem::TaskMng<>                             TaskMngType ;
  typedef RunTimeSystem::SeqEnv                                SeqEnvType ;
  typedef RunTimeSystem::DriverT<SeqEnvType>                   SeqDriverType ;
  typedef TaskMngType::ForkJoin<SeqDriverType>                 SeqForkJoinTaskType ;
  typedef RunTimeSystem::SchedulerT<SeqEnvType>                SeqSchedulerType ;
  typedef RunTimeSystem::DataMng                               DataMngType ;

  typedef Arcane::ITraceMng                                    ITraceMng ;

  class ExecException : public ArcGeoSim::Exception::NumericException
  {
  public :
    typedef ArcGeoSim::Exception::NumericException BaseType ;
    ExecException()
    : BaseType("Exec",__LINE__)
  {}
  } ;

  template<typename PartitionT>
  class BaseOperator
  {
  public :
    typedef PartitionT PartitionType ;
    BaseOperator(std::string const& name,PartitionType& partitioner,int partition_id,ITraceMng* trace_mng=NULL)
    : m_partition(partitioner)
    , m_partition_id(partition_id)
    , m_name(name)
    , m_trace_mng(trace_mng)
    {
#ifdef USE_PERF_COUNTER
      m_perf_timer.init("I") ;
      m_perf_timer.init("C") ;
#endif
    }

    std::string const& getName() const {
      return m_name ;
    }

    void startCounter(std::string const& phase) {
#ifdef USE_PERF_COUNTER
      m_perf_timer.start(phase) ;
#endif
    }
    void stopCounter(std::string const& phase) {
#ifdef USE_PERF_COUNTER
      m_perf_timer.stop(phase) ;
#endif
    }
    void printPerfInfo()
    {
#ifdef USE_PERF_COUNTER
      m_perf_timer.printInfo() ;
#endif
    }

    std::ostream& info() const {
      return m_trace_mng->info(m_partition_id).file() ;
    }

  protected :
    PartitionType& m_partition ;
    int m_partition_id ;
    std::string m_name ;
    ITraceMng* m_trace_mng ;
#ifdef USE_PERF_COUNTER
    RunTimeSystem::PerfCounterMng<std::string> m_perf_timer ;
#endif
  };

  template<typename PartitionT>
  class FunctorTask : public BaseOperator<PartitionT>
  {
  public :
      typedef BaseOperator<PartitionT>                                  BaseType ;
      typedef PartitionT                                                               PartitionType ;

      FunctorTask(IRangeFunctor* functor,PartitionType& partition,int partition_id,ITraceMng* trace_mng=NULL)
        : BaseType("FunctorTask",partition,partition_id,trace_mng)
        , m_functor(functor)
        {}

      virtual ~FunctorTask()
      {}

      void compute(DataArgsType& args)
      {
        typename PartitionType::Range range = this->m_partition.range(this->m_partition_id) ;
        m_functor->executeFunctor(range.begin(),range.size()) ;
      }

  private :
      IRangeFunctor* m_functor;
  };



  typedef TaskMngType::TaskListType                                                      TaskListType ;
  typedef TaskMngType::TaskPtrType                                                       TaskPtrType ;
  typedef TaskMngType::QueueType                                                         QueueType ;
  typedef TaskMngType::ITask::uid_type                                                   TaskUidType ;
  typedef TaskMngType::DAGType                                                           DAGType ;
  typedef std::vector<TaskUidType>                                                       DAGInfoType ;
  typedef std::size_t                                                                    SeqUidType ;

//#define TBB
#define PTH
#ifdef TBB
  typedef RunTimeSystem::TBBEnv                                     TBBEnvType ;
#endif
#ifdef PTH
  typedef RunTimeSystem::ThreadEnv                                  ThreadEnvType ;
#endif

  typedef RunTimeSystem::DriverT<ThreadEnvType>                     DriverType ;
  typedef DriverType                                                ForkJoinDriverType ;
  typedef TaskMngType::ForkJoin<DriverType>                         ForkJoinTaskType ;
  typedef RunTimeSystem::SchedulerT<ThreadEnvType>                  ParSchedulerType ;
  typedef RunTimeSystem::StdPartitioner                             PartitionerType ;

  class Sequence
  {
  public :
    Sequence() {}
    virtual ~Sequence() {
      HARTSTaskImplementation::free(m_partitioners) ;
    }
    void clear() {
      m_dag.clear() ;
      HARTSTaskImplementation::free(m_partitioners) ;
      m_partitioners.clear() ;
    }

    DAGType                                     m_dag ;
    TaskMngType                              m_task_mng ;
    std::vector<PartitionerType*>   m_partitioners ;
    std::vector<IRangeFunctor*>    m_functors ;
  };

 public:
  HARTSTaskImplementation(const ServiceBuildInfo& sbi)
  {}

  virtual ~HARTSTaskImplementation()
  {
    delete m_forkjoin_driver ;
    delete m_partitioner ;
    delete m_thread_env ;
    for(std::size_t i=0;i<m_sequences.size();++i)
      m_sequences[i].clear() ;
    m_sequences.clear() ;
  }

 public:
  virtual void build(){}

  virtual void initialize(Int32 nb_thread)
  {
    if (nb_thread<0)
      nb_thread = 0;
    m_is_active = (nb_thread!=1);

    ThreadEnvType* thread_env = new ThreadEnvType(nb_thread) ;
    m_thread_env = new ArcGeoSim::HARTSThreadEnv(thread_env) ;
    m_forkjoin_driver = new ForkJoinDriverType(thread_env) ;

    Integer nb_partition = nb_thread ;
    m_partitioner = new PartitionerType(nb_partition) ;

    createNewSequence() ;
    m_instance = this ;
  }

  virtual void terminate() {

    delete m_forkjoin_driver ;
    delete m_partitioner ;
    delete m_thread_env ;
    for(std::size_t i=0;i<m_sequences.size();++i)
      m_sequences[i].clear() ;
    m_sequences.clear() ;
    m_forkjoin_driver = nullptr ;
    m_partitioner = nullptr ;
    m_thread_env = nullptr ;
  }


  virtual ITask* createRootTask(ITaskFunctor* f)
  {
    return nullptr;
  }

  void printInfos(std::ostream& o) const {

  }

#if (ARCANE_VERSION >= 20103)
  virtual void executeParallelFor(Integer begin,Integer size,const ParallelLoopOptions& options,IRangeFunctor* f)
  {
    if (options.maxThread()==1)
    {
      executeParallelFor(begin,size,f) ;
    }
    else
    {
      executeParallelFor(begin,size,options.grainSize(),f) ;
    }
  }
#endif

#ifdef USE_ARCANE_V3
  void executeParallelFor(const ParallelFor1DLoopInfo& loop_info) {}
#endif
  
  virtual void executeParallelFor(Integer begin,Integer size,Integer block_size,IRangeFunctor* f)
  {
    executeParallelFor(begin,size,f) ;
  }

  virtual void postParallelFor(Integer begin,Integer size,Integer block_size,IRangeFunctor* f,SeqUidType sequence)
  {
    postParallelFor(begin,size,f,sequence) ;
  }

  virtual void executeParallelFor(Integer begin,Integer size,IRangeFunctor* f)
  {
    Sequence& seq = m_sequences[0] ;
    TaskMngType& task_mng = seq.m_task_mng ;
    DAGType& dag = seq.m_dag ;
    PartitionerType& partitioner = *m_partitioner ;
    std::size_t range = size ;
    partitioner.compute(range) ;
    std::size_t nb_partitions = partitioner.getSize() ;

    ForkJoinTaskType* compute_fk_task  = new ForkJoinTaskType(*m_forkjoin_driver,task_mng.getTasks(),task_mng.getTaskProperties()) ;
    dag.push_back(task_mng.addNew(compute_fk_task)) ;

    typedef FunctorTask<PartitionerType> FunctorTaskType ;
    typedef TaskMngType::Task<FunctorTaskType,true>  TaskType ;

    for(std::size_t ipart=0;ipart<nb_partitions;++ipart)
    {
          TaskType* task = new TaskType(new FunctorTaskType(f,partitioner,ipart)) ;
          TaskType::FuncType f = &FunctorTaskType::compute ;
          task->set("cpu",f) ;
          int uid = task_mng.addNew(task) ;
          compute_fk_task->add(uid) ;
    }
    process(task_mng,dag) ;
    task_mng.clear() ;
    dag.clear() ;
  }


  virtual void postParallelFor(Integer begin,Integer size,IRangeFunctor* f,SeqUidType sequence)
  {
    Sequence& seq = m_sequences[sequence] ;
    seq.m_functors.push_back(f) ;
    TaskMngType& task_mng = seq.m_task_mng ;
    DAGType& dag = seq.m_dag ;
    PartitionerType& partitioner = getPartitioner(sequence) ;
    std::size_t range = size ;
    partitioner.compute(range) ;
    std::size_t nb_partitions = partitioner.getSize() ;

    ForkJoinTaskType* compute_fk_task  = new ForkJoinTaskType(*m_forkjoin_driver,task_mng.getTasks(),task_mng.getTaskProperties()) ;
    dag.push_back(task_mng.addNew(compute_fk_task)) ;

    typedef FunctorTask<PartitionerType> FunctorTaskType ;
    typedef TaskMngType::Task<FunctorTaskType,true>  TaskType ;

    for(std::size_t ipart=0;ipart<nb_partitions;++ipart)
    {
          TaskType* task = new TaskType(new FunctorTaskType(f,partitioner,ipart)) ;
          TaskType::FuncType f = &FunctorTaskType::compute ;
          task->set("cpu",f) ;
          int uid = task_mng.addNew(task) ;
          compute_fk_task->add(uid) ;
    }
  }
#if (ARCANE_VERSION >= 30003)

  //! Exécute une boucle 1D en concurrence
  void executeParallelFor(const ComplexLoopRanges<1>& loop_ranges,
                                  const ParallelLoopOptions& options,
                                  IMDRangeFunctor<1>* functor) {}
  //! Exécute une boucle 2D en concurrence
  void executeParallelFor(const ComplexLoopRanges<2>& loop_ranges,
                                  const ParallelLoopOptions& options,
                                  IMDRangeFunctor<2>* functor) {}
  //! Exécute une boucle 3D en concurrence
  void executeParallelFor(const ComplexLoopRanges<3>& loop_ranges,
                                  const ParallelLoopOptions& options,
                                  IMDRangeFunctor<3>* functor) {}
  //! Exécute une boucle 4D en concurrence
  void executeParallelFor(const ComplexLoopRanges<4>& loop_ranges,
                                  const ParallelLoopOptions& options,
                                  IMDRangeFunctor<4>* functor) {}
#endif

  virtual bool isActive() const
  {
    return m_is_active;
  }
  virtual Int32 nbAllowedThread() const
  {
    return m_thread_env->getNbThreads();
  }

  virtual Int32 currentTaskThreadIndex() const {
	return 0 ;
  }

  virtual Int32 currentTaskIndex() const {
    return 0 ;
  }

  // not used with harts but require by interface
#if (ARCANE_VERSION >= 20405)
  virtual void setDefaultParallelLoopOptions(const ParallelLoopOptions& v)
  {
     m_parallel_loop_options = v ;
  }
  virtual const ParallelLoopOptions& defaultParallelLoopOptions(){ return m_parallel_loop_options; }
#endif

  static HARTSTaskImplementation* instance() {
    return m_instance ;
  }

  ArcGeoSim::IThreadEnv* getThreadEnv() {
    return m_thread_env  ;
  }


  ForkJoinDriverType*  getForkJoinDriver() {
    return m_forkjoin_driver ;
  }

  PartitionerType&  getPartitioner(SeqUidType sequence) {
    assert( ((sequence>=0)&&(sequence<m_sequences.size()))) ;
    if(sequence==0)
      return *m_partitioner ;
    else
    {
      PartitionerType* partitioner = new PartitionerType(m_partitioner->getSize()) ;
      m_sequences[sequence].m_partitioners.push_back(partitioner) ;
      return *partitioner ;
    }
  }

  void start() {
    m_sequences[0].m_task_mng.start() ;
  }


  void process(TaskMngType& task_mng,DAGType& dag)
  {
    task_mng.start() ;
    if(sequential)
    {
      SeqEnvType seq_env ;
      SeqSchedulerType scheduler(&seq_env) ;
      try
      {
        task_mng.run(scheduler,dag) ;
      }
      catch(TaskMngType::ExecTaskException& exc)
      {
        throw ExecException() ;
      }
    }
    else
    {
      ParSchedulerType scheduler(m_thread_env->getImpl()) ;
      try
      {
        task_mng.run(scheduler,dag) ;
      }
      catch(TaskMngType::ExecTaskException& exc)
      {
        throw ExecException() ;
      }
    }
  }

  void process(SeqUidType sequence)
  {
    assert( ((sequence>=0)&&(sequence<m_sequences.size()))) ;
    Sequence& seq = m_sequences[sequence] ;
    process(seq.m_task_mng,seq.m_dag) ;
  }

  void end() {
    end(0) ;
  }

  void end(SeqUidType sequence) {
    assert( ((sequence>=0)&&(sequence<m_sequences.size()))) ;
    m_sequences[sequence].clear() ;
  }

  SeqUidType createNewSequence()
  {
    SeqUidType uid = m_sequences.size() ;
    m_sequences.push_back(Sequence()) ;
    return uid ;
  }

  void initSequence(SeqUidType sequence)
  {
    assert( ((sequence>=0)&&(sequence<m_sequences.size()))) ;
    m_sequences[sequence].clear() ;
  }



  static HARTSTaskImplementation* m_instance ;
 private:
  template<typename T>
  static void free(std::vector<T>& v) {
    for(std::size_t  i=0;i<v.size();++i)
      delete v[i];
  }

  bool m_is_active = false;

  std::vector<Sequence> m_sequences ;

  ArcGeoSim::HARTSThreadEnv*                 m_thread_env      = nullptr;
  ForkJoinDriverType*                        m_forkjoin_driver = nullptr;
  PartitionerType*                           m_partitioner     = nullptr;

  ParallelLoopOptions                        m_parallel_loop_options ;

};


namespace Parallel
{
  /*!
   * \brief Applique en concurrence la m�thode \a function de l'instance
   * \a instance sur la vue \a items_view.
   * \ingroup Concurrency
   */
  template<typename InstanceType,typename ItemType> static void
  postForeach(const ItemVectorView& items_view,InstanceType* instance,void (InstanceType::*function)(ItemVectorViewT<ItemType> items),HARTSTaskImplementation::SeqUidType sequence)
  {
    typedef ItemRangeFunctorT<InstanceType,ItemType> FunctorType ;
    if(HARTSTaskImplementation::instance()==NULL)
    {
      FunctorType ipf(items_view,instance,function) ;
      TaskFactory::executeParallelFor(0,items_view.size(),500,&ipf);
    }
    else
    {
      FunctorType* ipf = new FunctorType(items_view,instance,function) ;
      HARTSTaskImplementation::instance()->postParallelFor(0,items_view.size(),ipf,sequence) ;
    }
  }


  template<typename LambdaType> static void
  postForeach(const ItemVectorView& items_view,HARTSTaskImplementation::SeqUidType sequence,LambdaType const& lambda_function)
  {
    typedef LambdaItemRangeFunctorT<LambdaType> FunctorType ;
    if(HARTSTaskImplementation::instance()==NULL)
    {
      FunctorType ipf(items_view,lambda_function);
      TaskFactory::executeParallelFor(0,items_view.size(),500,&ipf);
    }
    else
    {
      FunctorType* ipf = new FunctorType(items_view,lambda_function);
      HARTSTaskImplementation::instance()->postParallelFor(0,items_view.size(),ipf,sequence) ;
    }
  }

  /*!
   * \brief Applique en concurrence la m�thode \a function de l'instance
   * \a instance sur le groupe \a items.
   * \ingroup Concurrency
   */
  template<typename InstanceType,typename ItemType> static void
  postForeach(const ItemGroup& items,InstanceType* instance,void (InstanceType::*function)(ItemVectorViewT<ItemType> items),HARTSTaskImplementation::SeqUidType sequence)
  {
    postForeach(items.view(),instance,function,sequence);
  }


  template<typename LambdaType> static void
  postForeach(const ItemGroup& items, HARTSTaskImplementation::SeqUidType sequence,const LambdaType& lambda_function)
  {
    postForeach(items.view(),sequence,lambda_function);
  }
}


ARCANE_END_NAMESPACE

#endif
