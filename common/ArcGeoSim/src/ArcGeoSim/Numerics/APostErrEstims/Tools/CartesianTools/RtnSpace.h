#ifndef SRC_ERRORESTIMATES_TWODIMENSIONTOOLS_RTNSPACE_H_
#define SRC_ERRORESTIMATES_TWODIMENSIONTOOLS_RTNSPACE_H_
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

#include "types.h"


using namespace Arcane;


const Integer numCellFaces = 6;
/*!
  \struct RtnSpace
  \author Soleiman YOUSEF
  \brief Map for Quadratique functions on a mesh
*/

class RtnSpace{
  typedef boost::numeric::ublas::permutation_matrix<std::size_t> PermutationMatrix;

public:

  RtnSpace(DiscretizationGeometry* dg ,
 	   VariableDoFArrayReal& rtn_coef)
    :m_dg(dg),
     m_dc(m_dg->discretizationConnectivity()),
     m_rtn_coef(rtn_coef)
  {
    //DO NOTHING
  }

  //virtual ~RtnSpace(){};

  Real3 eval(const Real3 & P, const Discretization::Cell & K);
  Real   div(const RealSharedArray & flux_values, const Discretization::Cell & cell);

  typedef Discretization::ItemGroup CellsGroup;
  void compute(const RealSharedArray & flux_values,CellsGroup& callCells);


  DiscretizationGeometry* m_dg ;
  DiscretizationConnectivity* m_dc ;

  VariableDoFArrayReal& m_rtn_coef;


private:
  void __compute_Map(const RealSharedArray & flux_values, CellsGroup& callCells);
  void init();
};





#endif
