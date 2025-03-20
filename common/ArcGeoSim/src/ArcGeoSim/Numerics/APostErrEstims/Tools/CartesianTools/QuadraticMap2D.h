#ifndef SRC_ERRORESTIMATES_TWODIMENSIONTOOLS_QuadraticMap2D_H_
#define SRC_ERRORESTIMATES_TWODIMENSIONTOOLS_QuadraticMap2D_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"

#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/array.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Quadratic2D.h"
#include "types.h"



using namespace Arcane;


//const Integer numCellFaces = 6;
/*!
  \struct QuadraticMap
  \author Soleiman YOUSEF
  \brief Map for Quadratique functions on a mesh
*/
class QuadraticMap2D{
  typedef boost::numeric::ublas::permutation_matrix<std::size_t> PermutationMatrix;

public:

  QuadraticMap2D(DiscretizationGeometry* dg,
      VariableDoFArrayReal& quadratic_coef)
    :m_dg(dg),
     m_dc(m_dg->discretizationConnectivity()),
     m_quadratic_coef(quadratic_coef)
  {
    //DO NOTHING
  }

  virtual ~QuadraticMap2D(){};

  Real eval(const Real3 & P, const Discretization::Cell & K);
	       
  Real3 gradient(const Real3 & P, const Discretization::Cell & K);

  typedef Discretization::ItemGroup CellsGroup;
  void compute(const RealSharedArray & aprox_values,
	       const RealSharedArray & flux_values,
	       const VariableDoFReal3x3& perm,
	       CellsGroup& callCells);


private:
  DiscretizationGeometry* m_dg;
  DiscretizationConnectivity* m_dc ;
  VariableDoFArrayReal& m_quadratic_coef;


  void __compute_Map(const RealSharedArray & aprox_values,
		     const RealSharedArray & flux_values,
		     const VariableDoFReal3x3& perm,
		     CellsGroup& callCells);
};



#endif
