#ifndef BILINEARMAP2D_HPP
#define BILINEARMAP2D_HPP
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Bilinear2D.h"
#include "types.h"
#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"

//#include "defs.hpp"

//#include "QuadratureRules/gauss.hpp"

/*!
  \struct BilinearMap
  \author Soleiman YOUSEF
  \brief Map for bilinear functions on a mesh
*/
class BilinearMap2D{
  typedef boost::numeric::ublas::permutation_matrix<std::size_t> PermutationMatrix;

public:
  
  BilinearMap2D(DiscretizationGeometry* dg ,
      VariableDoFArrayReal& bilinear_coef)
    :m_dg(dg),
     m_dc(m_dg->discretizationConnectivity()),
     m_bilinear_coef(bilinear_coef)
{
    //DO NOTHING
  }

  virtual ~BilinearMap2D(){};

  Real eval(const Real3 & P, const Discretization::Cell & K) ;

  typedef Discretization::ItemGroup CellsGroup;
  void compute(const RealSharedArray & node_values, CellsGroup& callCells);

   Real3 gradient(const Real3 & P, const Discretization::Cell & K) ;

private:

  DiscretizationGeometry* m_dg;
  DiscretizationConnectivity* m_dc ;
  VariableDoFArrayReal& m_bilinear_coef;
  
  void __compute_Map(const RealSharedArray & node_values, CellsGroup& callCells);
 

};

#endif
