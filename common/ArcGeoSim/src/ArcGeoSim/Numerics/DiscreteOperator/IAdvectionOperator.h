#ifndef IADVECTIONOPERATOR_H
#define IADVECTIONOPERATOR_H

#include "IDiscreteOperator.h"

class IAdvectionOperator : public IDiscreteOperator {
 public:
  //! Velocity flux type
  typedef VariableFaceReal VelocityFluxType;

 public:
  virtual ~IAdvectionOperator() {}

 public:
  virtual void setCells(const CellGroup& cells) = 0 ;
  virtual void setFaces(const FaceGroup& faces) = 0 ;
  //! Form the discrete operator
  virtual void formDiscreteOperator(const VelocityFluxType& v) = 0;
};

#endif
