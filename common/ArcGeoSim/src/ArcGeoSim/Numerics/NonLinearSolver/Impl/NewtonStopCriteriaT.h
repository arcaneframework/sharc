#ifndef NEWTONSTOPCRITERIAT_H
#define NEWTONSTOPCRITERIAT_H

#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearStopCriteria.h"

#include "ArcGeoSim/Numerics/Utils/Norme.h"

#include "ArcGeoSim/Utils/VisitorT.h"

using namespace Arcane;

#define DEFAULTNEWTONSTOPCRITERIAVALUE 1e-6

template<
  typename NonLinearSystemType,
  ArcGeoSim::Numerics::eNormeType norme_type
  >
class NewtonStopCriteriaT 
  : public INonLinearStopCriteria
  , public VisitorT<NonLinearSystemType>
{
public:
  //! Constructeur de la classe
  NewtonStopCriteriaT(Real criteria=DEFAULTNEWTONSTOPCRITERIAVALUE) 
    : m_criteria(criteria)
    , m_residual(-1)
    , m_error(0)
  {
    ;
  } 
  
  /** Destructeur de la classe */
  virtual ~NewtonStopCriteriaT() {}
  
public:

  //! Getting operator name
  // Is it name or kind of operator ?
  String name() const { return "NewtonStopCriteriaT"; }
  
  //! Initialize
  void init() 
  { 
    m_residual = -1;
    m_error = 0;
  }
  
  //! Set Stop Criteria Value 
  void setCriteriaValue(const Real criteria)
  {
    m_criteria = criteria; 
  }
  
  //! Visiting non linear system
  Integer visit(NonLinearSystemType * system) 
  {
    m_residual = system->getNewtonResidual(m_norme);
    m_error = system->getNumericalError();
    if(m_error!=0) return 1;
    if(m_residual<m_criteria) return 0;
    if(m_residual>=m_criteria) return 2;
    return 1;
  }

  Real criteriaValue() const { return m_residual; }

private :
   
  Real m_criteria;
  Real m_residual;
  Integer m_error;
  ArcGeoSim::Numerics::Norme<norme_type> m_norme;
};


#endif
