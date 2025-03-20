#ifndef ILINEARSYSTEMVISITOR_H
#define ILINEARSYSTEMVISITOR_H

#include <arcane/ItemTypes.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/TraceInfo.h>

//using namespace Arcane;

/**
 * Interface du service du modu?le de schma numeric.
 */

class ILinearSystem;
class HypreLinearSystem;
class PETScLinearSystem;
class IFPLinearSystem;
class LocalLinearSystem;
class MTLLinearSystem;
class SuperLULinearSystem;
class GPULinearSystem ;

class ILinearSystemVisitor
{

public:
  virtual ~ILinearSystemVisitor() { }

  /**
   *  Initialise
   */
  virtual void init() = 0;

  virtual bool visit(      ILinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
  virtual bool visit(  HypreLinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
  virtual bool visit(  PETScLinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
  virtual bool visit(    IFPLinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
  virtual bool visit(  LocalLinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
  virtual bool visit(    MTLLinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
  virtual bool visit(SuperLULinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemanted");  }
  virtual bool visit(    GPULinearSystem * system) { throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");  }
};
 //END_NAME_SPACE_PROJECT


#endif
