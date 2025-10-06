#ifndef COMPUTE_ERROR_H
#define COMPUTE_ERROR_H

#include <arcane/VariableTypes.h>
#include <arcane/VariableTypedef.h>

#include "ArcGeoSim/Numerics/Expressions/IFunctionR4vR1.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

//Dependance à Discrete Operator???
//Il faut deplacer le fichier ou supprimer la dependance
#include <boost/shared_ptr.hpp>
#include "ArcGeoSim/Numerics/DiscreteOperator/CoefficientArray.h"

#include "ArcGeoSim/Numerics/BCs/IBoundaryCondition.h"
#include "ArcGeoSim/Numerics/BCs/IBoundaryConditionMng.h"

using namespace Arcane;

/*!
  \fn computeLInfErrorNorm
  \author dipietrd
  \brief Compute the \f$L^{\infty}\f$ error norm on a group of cells
 */
template<typename REAL3MAP, typename REALMAP>
Real computeLInfErrorNorm(const VariableCellReal & uh,
                          const CellGroup & cells,
                          const REAL3MAP & c_centers,
                          const REALMAP & c_measures,
                          IFunctionR4vR1 * u,
                          Real t = 0)
{
  
  Real err = 0.;
  ENUMERATE_CELL(iT, cells) {
    const Cell& T = *iT;
    const Real3& C_T = c_centers[T];
    Real uh_T = uh[T];
    Real u_T = u->eval(C_T.x,C_T.y,C_T.z, t);

    err = math::max(err, math::abs(u_T - uh_T));
  }
  return err;
}


/*!
  \fn computeL2ErrorNorm
  \author dipietrd
  \brief Compute the \f$L^{2}\f$ error norm on a group of cells
 */
template<typename REAL3MAP, typename REALMAP>
Real computeL2ErrorNorm(const VariableCellReal& uh,
                        const CellGroup&        cells,
                        const IGeometryMng::Real3GroupMap& c_centers,
                        const IGeometryMng::RealGroupMap& c_measures,
                        IFunctionR4vR1*              u,
                        Real                    t = 0)
{
  
  Real err = 0.;
  ENUMERATE_CELL(iT, cells) {
    const Cell& T = *iT;
    const Real3& C_T = c_centers[T];
    Real meas_T = c_measures[T];
    Real uh_T = uh[T];
    Real u_T = u->eval(C_T.x,C_T.y,C_T.z, t);

    err += meas_T * pow(u_T - uh_T, 2);
  }
  
  err = sqrt(err);

  return err;
}

/*!
  \fn computeL2ErrorNormPower2
  \author dipietrd
  \brief Compute the second power of the \f$L^{2}\f$ error norm
  on a group of cells (useful for parallel computations)
 */
template<typename REAL3MAP, typename REALMAP>
Real computeL2ErrorNormPower2(const VariableCellReal& uh,
			      const CellGroup&        cells,
			      const REAL3MAP& c_centers,
			      const REALMAP& c_measures,
			      IFunctionR4vR1*              u,
                              Real                    t = 0)
{
  
  Real err = 0.;
  ENUMERATE_CELL(iT, cells) {
    const Cell& T = *iT;
    const Real3& C_T = c_centers[T];
    Real meas_T = c_measures[T];
    Real uh_T = uh[T];
    Real u_T = u->eval(C_T.x,C_T.y,C_T.z, t);

    err += meas_T * pow(u_T - uh_T, 2);
  }

  return err;
}

/*!
  \fn computeLpErrorNorm
  \author dipietrd
  \brief Compute the \f$L^{p}\f$ error norm on a group of cells
*/
Real computeLpErrorNorm(const VariableCellReal& uh,
                        const CellGroup&        cells,
                        const IGeometryMng::Real3GroupMap& c_centers,
                        const IGeometryMng::RealGroupMap& c_measures,
                        IFunctionR4vR1*              u,
                        Real                    p = 2,  
                        Real                    t = 0);

/*!
  \fn computeLpErrorNorm
  \author dipietrd
  \brief Compute the \f$p\f$th power of the \f$L^{p}\f$ error norm
  on a group of cells
 */
Real computeLpErrorNormPowerp(const VariableCellReal& uh,
                        const CellGroup&        cells,
                        const IGeometryMng::Real3GroupMap& c_centers,
                        const IGeometryMng::RealGroupMap& c_measures,
                        IFunctionR4vR1*              u,
                        Real                    p = 2,  
                        Real                    t = 0);


/*!
  \fn computeFluxErrors
  \author kapyrini
  \brief Compute the   \f$L^{\infty}\f$ and \f$L^{2}\f$ normalized(!!!) error norms for the fluxes
  in the computational domain.
  The first component of output is \f$L^{\infty}\f$ norm, the second is \f$L^{2}\f$ norm.
 */

Real2 computeFluxErrors(const FaceGroup&        faces,
                          const CellGroup& cells,
                          const FaceGroup& boundary_faces,
                          const VariableCellReal& uh,
                          const  PartialVariableFaceReal& uhb,
                          const IGeometryMng::RealGroupMap& f_measures,
                          const IGeometryMng::Real3GroupMap& f_centers,
                          const IGeometryMng::Real3GroupMap& f_normals,
                          const IGeometryMng::RealGroupMap& c_measures,
                          const VariableCellReal3x3& k,
                          IMesh* mesh,
                          IBoundaryConditionMng* m_bc_mng,
                          boost::shared_ptr<CoefficientArrayT<Cell> > m_cell_coefficients,
                          boost::shared_ptr<CoefficientArrayT<Face> >  m_face_coefficients,
                          IFunctionR4vR1*  grad_x,
                          IFunctionR4vR1*  grad_y,
                          IFunctionR4vR1*  grad_z,
                          Real                    t);

/*!
  \fn FaceAverage
  \author kapyrini
  \brief Compute the   average of the function u on a face..
 */

Real FaceAverage(IMesh* mesh, const Face& face,
                              IFunctionR4vR1* u, Real t = 0);
                          
#endif
