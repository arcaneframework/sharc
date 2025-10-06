// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#ifndef MCKERNEL_H_
#define MCKERNEL_H_

//#define DEBUG


  namespace Parallel
  {
    template<bool stdscheduler=false>
    class MCKernel
    {
    public:

      typedef Arcane::ItemVectorView                                                              VectorType ;

      typedef RunTimeSystem::DataHandler                                             DataHandlerType ;
      typedef RunTimeSystem::DataArgs<DataHandlerType>                 DataArgsType ;

      static bool const                                                                sequential = stdscheduler ;
      typedef RunTimeSystem::StdDriver                                   STDDriverType ;
      typedef RunTimeSystem::TBBDriver                                  TBBDriverType ;
      typedef RunTimeSystem::PThreadDriver                           PTHDriverType ;
      typedef RunTimeSystem::PTHDriverN<>                          BTHDriverType ;

      typedef RunTimeSystem::TaskMng::ForkJoin<STDDriverType>        STDForkJoinTaskType ;
      typedef RunTimeSystem::TaskMng::ForkJoin<TBBDriverType>        TBBForkJoinTaskType ;
      typedef RunTimeSystem::TaskMng::ForkJoin<PTHDriverType>        PTHForkJoinTaskType ;
      typedef RunTimeSystem::TaskMng::ForkJoin<BTHDriverType>        BTHForkJoinTaskType ;

      typedef RunTimeSystem::DataMng                                     DataMngType ;

      typedef Arcane::ITraceMng                                                       ITraceMng ;

      class ExecException : public ArcGeoSim::Exception::NumericException
      {
      public :
        typedef ArcGeoSim::Exception::NumericException BaseType ;
        ExecException()
        : BaseType("Exec",__LINE__)
      {}
      } ;


      template<typename T>
      class Future
      {
      public :
        Future(T& value)
        : m_value(value)
        , m_wait_uid(RunTimeSystem::TaskMng::ITask::null_uid)
        , m_is_posted(false)
        {
        }

        void setWaitTaskUid(RunTimeSystem::TaskMng::ITask::uid_type uid)
        {
          m_wait_uid = uid ;
        }

        RunTimeSystem::TaskMng::ITask::uid_type getWaitTaskUid() const {
          return m_wait_uid ;
        }

        void setWaitTaskPostedFlag(bool value)
        {
          m_is_posted = value ;
        }

        bool getWaitTaskPostedFlag() {
          return m_is_posted ;
        }

        T& operator*()
        {
          return m_value ;
        }
      private :
        T& m_value ;
        RunTimeSystem::TaskMng::ITask::uid_type m_wait_uid ;
        bool m_is_posted ;
      };

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
          m_perf_timer.init("I") ;
          m_perf_timer.init("C") ;
        }

        std::string const& getName() const {
          return m_name ;
        }

        void startCounter(std::string const& phase) {
          m_perf_timer.start(phase) ;
        }
        void stopCounter(std::string const& phase) {
          m_perf_timer.stop(phase) ;
        }
        void printPerfInfo()
        {
          m_perf_timer.printInfo() ;
        }

        std::ostream& info() const {
          return m_trace_mng->info(m_partition_id) ;
        }
      protected :
        PartitionType& m_partition ;
        int m_partition_id ;
        std::string m_name ;
        ITraceMng* m_trace_mng ;
        RunTimeSystem::PerfCounterMng<std::string> m_perf_timer ;
      };

      template<typename PartitionT,typename InstanceT, typename ItemT>
      class ItemTask : public BaseOperator<PartitionT>
      {
      public :
          typedef BaseOperator<PartitionT>                                  BaseType ;
          typedef PartitionT                                                               PartitionType ;
          typedef boost::function2<void,
                                                      InstanceT*,
                                                      ItemVectorViewT<ItemT> >      FuncType ;

          ItemTask(InstanceT* instance, FuncType function, ItemGroupT<ItemT> const& group,PartitionType& partition,int partition_id,ITraceMng* trace_mng=NULL)
            : BaseType("ItemTask",partition,partition_id,trace_mng)
            , m_instance(instance)
            , m_function(function)
            , m_group(group)
            {}

          virtual ~ItemTask(){}

          void compute(DataArgsType& args)
          {
#ifdef DEBUG
            this->info()<<"ItemTask["<<this->m_partition_id<<"]"<<endl ;
#endif

            typedef RunTimeSystem::SplitVector<VectorType,PartitionT>           SplitVectorType ;
            typedef RunTimeSystem::SplitConstVector<VectorType,PartitionT> SplitConstVectorType ;
            typename PartitionType::Range range = this->m_partition.range(this->m_partition_id) ;
            ItemVectorView view = m_group.view().subView(range.begin(),range.size()) ;
            std::cout<<"ItemTask["<<this->m_partition_id<<"]"<<range.begin()<<" "<<range.size()<<" view size"<<view.size()<<std::endl;
            m_function(m_instance,view) ;
          }

      private :
          InstanceT* m_instance;
          FuncType m_function;
          ItemGroupT<ItemT> m_group;
      };


      template<typename ValueT>
      class AssertNullTask : public RunTimeSystem::TaskMng::BaseTask
      {
      public :
        typedef RunTimeSystem::TaskMng::BaseTask                  BaseType ;
        typedef  RunTimeSystem::TaskMng::ITask::TargetType     TargetType ;
        typedef RunTimeSystem::DataHandler                             DataHandlerType ;
        typedef RunTimeSystem::DataArgs<DataHandlerType> DataArgsType ;
        typedef  RunTimeSystem::TaskMng::QueueType               QueueType;

        AssertNullTask(ValueT& value,std::string const& type, TargetType const& target)
        : m_value(value)
        , m_type(type)
        , m_raise_exception(false)
        {
          m_targets.insert(target) ;
        }
        virtual ~AssertNullTask(){}

        void addTarget(TargetType const& target)
        {
          m_targets.insert(target) ;
        }

        void compute(DataArgsType& args)
        {
          m_raise_exception = m_value==0 ;
        }

        bool implement(TargetType const& type) const {
          return m_targets.find(type) != m_targets.end() ;
        }

        bool implement(TargetType& type) {
          return m_targets.find(type) != m_targets.end() ;
        }

        void compute(TargetType& type,QueueType& queue) {
#ifdef DEBUG
          std::cout<<"ASSERT NULL "<<m_type<<std::endl;
#endif
          m_raise_exception = false ;
          if(m_targets.find(type) != m_targets.end())
          {
            if(m_value==0)
              m_raise_exception = true ;

             finalyze(type,queue) ;
          }
        }

        void compute(TargetType& type)
        {
          m_raise_exception = false ;
          if(m_targets.find(type) != m_targets.end())
          {
            if(m_value==0) m_raise_exception = true ;
          }
        }


        void finalyze(TargetType& type,QueueType& queue)
        {
          if(m_raise_exception)
            queue.raiseException() ;
          else
            BaseTask::notifyChildren(queue) ;
          BaseTask::notify(queue) ;
        }

        void finalyze(TargetType& type,std::vector<int>& queue)
        {
          if(!m_raise_exception)
            BaseTask::notifyChildren(queue) ;
        }

        void finalyze(TargetType& type) {
        }
      private :
          ValueT& m_value ;
          std::string    m_type ;
          std::set<TargetType> m_targets ;
          bool m_raise_exception ;
      } ;


      typedef RunTimeSystem::TaskMng                                                       TaskMngType ;
      typedef TaskMngType::TaskListType                                                      TaskListType ;
      typedef TaskMngType::TaskPtrType                                                       TaskPtrType ;
      typedef TaskMngType::QueueType                                                        QueueType ;
      typedef RunTimeSystem::TaskMng::ITask::uid_type                             TaskUidType ;
      typedef TaskMngType::DAGType                                                            DAGType ;
      typedef std::vector<TaskUidType>                                                       DAGInfoType ;
      typedef std::size_t                                                                                  SeqUidType ;
      static int const                                                                                      nullSeqUid = -1 ;
      //static int const                                                                                      nullTaskUid = -1 ;
      static RunTimeSystem::TaskMng::ITask::uid_type                      nullTaskUid() {
        return RunTimeSystem::TaskMng::ITask::null_uid ;
      }

//#define TBB
#define PTH
#ifdef TBB
      typedef  RunTimeSystem::TBBEnv                                      ThreadEnvType ;
      typedef TBBDriverType                                                        ForkJoinDriverType ;
      typedef TBBForkJoinTaskType                                              ForkJoinTaskType ;
      typedef RunTimeSystem::TBBScheduler                               ParSchedulerType ;
#endif
#ifdef PTH
      typedef  RunTimeSystem::ThreadEnv                                 ThreadEnvType ;
      typedef PTHDriverType                                                        ForkJoinDriverType ;
      typedef PTHForkJoinTaskType                                              ForkJoinTaskType ;
      typedef RunTimeSystem::PTHScheduler                                ParSchedulerType ;
#endif

    MCKernel(ThreadEnvType* thread_env)
      : m_thread_env(thread_env)
      , m_trace_mng(NULL)
      {
          m_dag.reserve(1) ;
//          m_dag_info.resize(m_nb_partitions+1) ;
//          m_dag_info.assign(m_nb_partitions+1,nullTaskUid()) ;
          m_nb_threads = m_thread_env->getNbThreads() ;
#ifdef PTH
          m_thread_env->SetAffinity();
#endif
          m_forkjoin_driver = new ForkJoinDriverType(m_thread_env) ;
      }

    virtual ~MCKernel()
    {
      delete m_forkjoin_driver ;
    }

    template<typename T>
    void free(std::vector<T>& v) {
      for(std::size_t  i=0;i<v.size();++i)
        delete v[i];
    }

    TaskMngType& getTaskMng() {
      return m_task_mng ;
    }

    DataMngType& getDataMng() {
      return m_data_mng ;
    }


    ForkJoinDriverType*  getForkJoinDriver() {
      return m_forkjoin_driver ;
    }

    void start() {
      m_task_mng.start() ;
    }

    void start(DAGType& dag)
    {
      m_task_mng.start(dag) ;
    }


    void process()
    {
      //start(m_dag) ;
      start() ;
      if(sequential)
      {
        RunTimeSystem::StdScheduler scheduler ;
        try
        {
          m_task_mng.run(scheduler,m_dag) ;
        }
        catch(TaskMngType::ExecTaskException& exc)
        {
          throw ExecException() ;
        }
      }
      else
      {
        ParSchedulerType scheduler(m_thread_env) ;
        try
        {
          m_task_mng.run(scheduler,m_dag) ;
        }
        catch(TaskMngType::ExecTaskException& exc)
        {
          throw ExecException() ;
        }
      }
    }



    template<typename SchedulerT>
    void process(SchedulerT& scheduler)
    {
      start() ;
      try
      {
        m_task_mng.run(scheduler,m_dag) ;
      }
      catch(TaskMngType::ExecTaskException& exc)
      {
        throw ExecException() ;
      }
    }



    /*!
     * \brief Applique en concurrence la m�thode \a function de l'instance
     * \a instance sur le groupe \a items.
     * \ingroup Concurrency
     */
    template<typename PartitionT,typename InstanceT,typename ItemT>
    void Foreach(const ItemGroupT<ItemT>& group,PartitionT* partition,InstanceT* instance,void (InstanceT::*function)(ItemVectorViewT<ItemT> items))
    {

      //TaskMngType::TaskListType& tasks = m_task_mng.getTasks() ;
      ForkJoinTaskType* compute_fk_task  = new ForkJoinTaskType(*m_forkjoin_driver,m_task_mng.getTasks()) ;
      m_dag.push_back(m_task_mng.addNew(compute_fk_task)) ;

      typedef ItemTask<PartitionT,InstanceT,ItemT> ItemTaskType ;
      typedef RunTimeSystem::TaskMng::Task<ItemTaskType,true>  TaskType ;

      Integer nb_partitions = partition->getSize() ;
      partition->split(group.view()) ;
      std::cout<<"NB PARTITION : "<<nb_partitions<<std::endl ;
      for(std::size_t ipart=0;ipart<nb_partitions;++ipart)
      {
            std::cout<<" PART["<<ipart<<"] : "<<partition->range(ipart).begin()<<" "<<partition->range(ipart).size()<<std::endl ;
            TaskType* task = new TaskType(new ItemTaskType(instance,function,group,*partition,ipart,m_trace_mng)) ;
            typename TaskType::FuncType f = &ItemTaskType::compute ;
            task->set("cpu",f) ;
            int uid = m_task_mng.addNew(task) ;
            compute_fk_task->add(uid) ;
      }
    }


    void end() {
      m_task_mng.clear() ;
      m_dag.clear() ;
      //m_dag_info.assign(m_dag_info.size(),nullTaskUid()) ;
    }



        void printPerfInfo() {
        }

        template<typename OpT>
        void _printPerfInfo(std::vector<OpT*> const& ops) {
          std::cout<<ops[0]->getName()<<" PERF INFO"<<std::endl ;
          for(int i=0;i<m_nb_threads;++i)
          {
            std::cout<<"THREAD ID : "<<i<<std::endl ;
            ops[i]->printPerfInfo() ;
          }
        }


        ITraceMng const*       getTraceMng() const {
          return m_trace_mng ;
        }

      private :

          TaskMngType m_task_mng ;
          DataMngType m_data_mng ;
          DAGType         m_dag ;
          int                                     m_nb_threads ;
          ThreadEnvType*                m_thread_env  ;
          ForkJoinDriverType*          m_forkjoin_driver ;

          ITraceMng*       m_trace_mng ;

    };
  }
#undef DEBUG
} /* namespace MCGSolver */

#endif /* MCKERNEL_H_ */
