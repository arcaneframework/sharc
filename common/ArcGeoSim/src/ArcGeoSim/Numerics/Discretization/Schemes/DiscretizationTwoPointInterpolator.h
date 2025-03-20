// -*- C++ -*-
#ifndef DISCRETIZATIONTWOPOINTINTERPOLATOR
#define DISCRETIZATIONTWOPOINTINTERPOLATOR

#include <arcane/Item.h>

#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/DiscreteOperatorProperty.h"
#include "ArcGeoSim/Numerics/Discretization/Tools/DiscretizationCoefficientArray.h"
using namespace Arcane;

namespace DiscretizationTwoPointInterpolator
{
  template<typename VariableTypeT>
  Real twoPointInternalTransmissivity(const Discretization::Face & F,
      DiscretizationConnectivity* dc,
      const Arcane::MeshVariableScalarRefT<typename Discretization::ConvertD2DoF<Discretization::Cell>::Type, VariableTypeT>& kappa,
      Centers<Discretization::Face>  * f_centers,
      OrientedUnitNormals<Discretization::Face> * f_normals,
      Measures<Discretization::Face> * f_measures,
      Centers<Discretization::Cell>  *  c_centers)
  {
#ifdef ARCANE_DEBUG
    const bool iIsOnBoundary = (dc)->isCellGroupBoundary(F) || (dc)->isSubDomainBoundary(F) ;
    ARCANE_ASSERT( (!iIsOnBoundary), ("Internal face expected") );
#endif

    typedef VariableTypeT DiffusionType;

    const Real3 & nF =  (*f_normals)(F)*(*f_measures)(F);// pour les DoF les normals sont unitaires !
    const Real3 & xF =  (*f_centers)(F);

    const Discretization::Cell & K = dc->backCell(F);
    const Discretization::Cell & L = dc->frontCell(F);

    const DiffusionType & kappaK = kappa[K];
    const DiffusionType & kappaL = kappa[L];

    const Real3& xK = (*c_centers)(K);
    const Real3& xL = (*c_centers)(L);

    Real dKF = math::abs(math::scaMul(xF-xK,nF)); // dist(xK, F)
    Real dLF = math::abs(math::scaMul(xF-xL,nF)); // dist(xL, F)
    Real dKL = math::sqrt(math::scaMul(xL-xK,xL-xK));

    Real nornF = math::sqrt(math::scaMul(nF,nF));

    Real tK = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaK, nF))/(dKF * nornF);
    Real tL = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaL, nF))/(dLF * nornF);

    Real cosO = math::abs(math::scaMul(xL-xK,nF))/(dKL * nornF);
    cosO *= cosO ;

    return tK*tL*nornF*cosO/(tK + tL);
  }

  /*------------------------------------------------------------------------------*/

  template<typename VariableTypeT>
  Real twoPointBoundaryTransmissivity(const Face & F,
                                      const VariableTypeT & kappa,
                                      const IGeometryMng::Real3Variable * f_centers,
                                      const IGeometryMng::Real3Variable * f_normals,
                                      const IGeometryMng::RealVariable * f_measures,
                                      const IGeometryMng::Real3Variable * c_centers)
  {
    ARCANE_ASSERT( (F.isSubDomainBoundary()), ("Boundary face expected") );

    typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

    // Retrieve information
    const Cell& T  = F.boundaryCell();

    const Real3& nF = (*f_normals)[F];

    const Real3& CT = (*c_centers)[T];
    const Real3& CF = (*f_centers)[F];
    const Real3 sF = CF - CT;

    Real dF1 = math::scaMul(sF, sF);
    Real dF2 = math::abs(math::scaMul(sF, nF));

    const DiffusionType & kappaT = kappa[T];

    // Compute transmissivities
    Real nFnF =  math::scaMul(nF, nF);
    Real cos2 =  (dF2 * dF2) / (dF1 * nFnF);

    Real tau  =  math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaT, nF)) / dF2;
    return tau * cos2;
  }

  /*------------------------------------------------------------------------------*/

  template<typename VariableTypeT>
  Real standardTwoPointInternalTransmissivity(const Face & F,
                                              const VariableTypeT & kappa,
                                              const IGeometryMng::Real3Variable * f_centers,
                                              const IGeometryMng::Real3Variable * f_normals,
                                              const IGeometryMng::RealVariable * f_measures,
                                              const IGeometryMng::Real3Variable * c_centers)
  {
    ARCANE_ASSERT( (!F.isSubDomainBoundary()), ("Internal face expected") );

    typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

    const Real3 & nF =  (*f_normals)[F];
    const Real3 & xF =  (*f_centers)[F];

    const Cell & K = F.backCell();
    const Cell & L = F.frontCell();

    const DiffusionType & kappaK = kappa[K];
    const DiffusionType & kappaL = kappa[L];

    const Real3& xK = (*c_centers)[K];
    const Real3& xL = (*c_centers)[L];

    Real dKF = math::abs(math::scaMul(xF-xK,nF)); // dist(xK, F)
    Real dLF = math::abs(math::scaMul(xF-xL,nF)); // dist(xL, F)


    Real tK = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaK, nF))/dKF ;
    Real tL = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaL, nF))/dLF ;

    Real tau  = math::abs(tK + tL) ? (tK * tL / (tK + tL)) : 0;

    return tau;
  }

  /*------------------------------------------------------------------------------*/

  template<typename VariableTypeT>
  Real standardTwoPointBoundaryTransmissivity(const Face & F,
                                              const VariableTypeT & kappa,
                                              const IGeometryMng::Real3Variable * f_centers,
                                              const IGeometryMng::Real3Variable * f_normals,
                                              const IGeometryMng::RealVariable * f_measures,
                                              const IGeometryMng::Real3Variable * c_centers)
  {
    ARCANE_ASSERT( (F.isSubDomainBoundary()), ("Boundary face expected") );

    typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

    // Retrieve information
    const Cell& T  = F.boundaryCell();

    const Real3& nF = (*f_normals)[F];

    const Real3& CT = (*c_centers)[T];
    const Real3& CF = (*f_centers)[F];
    const Real3 sF = CF - CT;

    Real dTF2 = math::abs(math::scaMul(sF, nF));

    const DiffusionType & kappaT = kappa[T];

    // Compute transmissivities
    Real tau  =  math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaT, nF)) / dTF2;

    return  tau;
  }

} // namespace

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/

#endif
