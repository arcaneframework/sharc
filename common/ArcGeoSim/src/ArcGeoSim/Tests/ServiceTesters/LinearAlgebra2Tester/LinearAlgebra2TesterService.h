#ifndef ARCGEOSIM_TESTS_SERVICETESTERS_LALGEBRATESTERSERVICE_H
#define ARCGEOSIM_TESTS_SERVICETESTERS_LALGEBRATESTERSERVICE_H
/* Author :
 *
 */

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"
#ifdef USE_ALIEN_V1
#include <ALIEN/Alien-IFPEN.h>
#include <ArcGeoSim/Numerics/AlienTools/BasicIndexManager.h>
#endif

#ifdef USE_ALIEN_V2
#include <alien/ref/AlienRefSemantic.h>
#include <alien/arcane_tools/IIndexManager.h>
#include <alien/arcane_tools/indexManager/BasicIndexManager.h>
#include <alien/expression/solver/ILinearSolver.h>
#endif

namespace Arcane { }
using namespace Arcane;

#include "ArcGeoSim/Tests/ServiceTesters/LinearAlgebra2Tester/LinearAlgebra2TestOptionTypes.h"

#include "LinearAlgebra2Tester_axl.h"

class MemoryAllocationTracker;

class LinearAlgebra2TesterService :
  public ArcaneLinearAlgebra2TesterObject
{
public:

  //! Constructor
  LinearAlgebra2TesterService(const Arcane::ServiceBuildInfo & sbi) :
    ArcaneLinearAlgebra2TesterObject(sbi)
  {}
  
  //! Destructor
  virtual ~LinearAlgebra2TesterService() {};
  
public:
  //! Initialization
  void init();
  //! Run the test
  int test();

private:

  void buildAndFillInVector(Alien::Vector& vectorB, const double& value);

  template<typename Profiler>
  void profileMatrix(const CellCellGroup& cell_cell_connection,
                     const ItemGroup areaP,
                     const IntegerArray2& allPIndex,
                     const IntegerArray& allUIndex,
                     const IntegerArray& allXIndex,
                     Profiler & profiler);

  void streamProfileMatrix(const CellCellGroup& cell_cell_connection,
                           const ItemGroup areaU,
                           const ItemGroup areaP,
                           const IntegerArray2& allPIndex,
                           const IntegerArray& allUIndex,
                           const IntegerArray& allXIndex,
                           Alien::StreamMatrixBuilder & inserters);

  void streamFillInMatrix(const CellCellGroup& cell_cell_connection,
                          const ItemGroup areaP,
                          const IntegerArray2& allPIndex,
                          const IntegerArray& allUIndex,
                          const IntegerArray& allXIndex,
                          Alien::StreamMatrixBuilder & inserters);

  template<typename Builder>
  void fillInMatrix(const CellCellGroup& cell_cell_connection,
                    const ItemGroup areaP,
                    const IntegerArray2& allPIndex,
                    const IntegerArray& allUIndex,
                    const IntegerArray& allXIndex,
                    Builder & builder);

  Alien::ILinearSolver::Status solve(Alien::ILinearSolver * solver,
                                     Alien::Matrix& matrixA,
                                     Alien::Vector& vectorB,
                                     Alien::Vector& vectorX);
#if defined(USE_ALIEN_V2)
  void vectorVariableUpdate(Alien::Vector& vectorB, Alien::ArcaneTools::IIndexManager::Entry indexSetU);
#else
  void vectorVariableUpdate(Alien::Vector& vectorB, Alien::IIndexManager::Entry indexSetU);
#endif
  void checkVectorValues(Alien::Vector& VectorX, const double& value);

  void checkDotProductWithManyAlgebra(const Alien::Vector& vectorB,
                                      Alien::Vector& vectorX,
#if defined(USE_ALIEN_V1) || defined(USE_ALIEN_V2)
                                      Alien::ISpace& space);
#else
                                      Alien::Space& space);
#endif
private :
  eItemKind m_stencil_kind      = Arcane::IK_Face;
  Real m_diag_coeff             = 0.;
  IParallelMng * m_parallel_mng = nullptr;
  Integer m_n_extra_indices     = 0;

private:
  MemoryAllocationTracker * m_memory_tracker = nullptr;
  Real getAllocatedMemory(); 
  Real getMaxMemory();
  void resetMaxMemory();
  Int64 getAllocationCount();

  // Type de statistiques
  class BuildingStat {
  public:
    BuildingStat(LinearAlgebra2TesterService * owner, Arcane::Timer & timer) 
      : m_owner(owner), m_timer(timer), memory(0), memory_max(0), time(0), allocation_count(0) { }
    void start() 
    {
      memory = m_owner->getAllocatedMemory(); 
      m_owner->resetMaxMemory();
      m_timer.start();
      allocation_count = m_owner->getAllocationCount();
    }
    void stop() 
    {
      m_timer.stop();
      memory_max = m_owner->getMaxMemory() - memory;
      memory = m_owner->getAllocatedMemory() - memory;
      time = m_timer.lastActivationTime();
      allocation_count = m_owner->getAllocationCount() - allocation_count;
    }

  private:
    LinearAlgebra2TesterService * m_owner = nullptr;
    Arcane::Timer & m_timer;

  public:
    Real memory=0., memory_max=0., time=0.;
    Int64 allocation_count = 0;
  };

  Real fij(const Cell & ci, const Cell & cj);
};

#endif /* ARCGEOSIM_TESTS_SERVICETESTERS_LINEARSOLVERTESTER_LINEARALGEBRATESTERSERVICE_H */
