#ifndef BILINEARMAP_HPP
#define BILINEARMAP_HPP
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Bilinear.h"
#include "types.h"
#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"
#include "ArcGeoSim/Utils/ItemGroupBuilder.h"

//#include "defs.hpp"

//#include "QuadratureRules/gauss.hpp"

/*!
  \struct BilinearMap
  \author Soleiman YOUSEF
  \brief Map for bilinear functions on a mesh
*/
class BilinearMap{
  typedef boost::numeric::ublas::permutation_matrix<std::size_t> PermutationMatrix;

public:
  
  BilinearMap(DiscretizationGeometry* dg)
    :m_dg(dg),
     m_dc(m_dg->discretizationConnectivity()),
     m_bilinear_coef(nullptr)
{
    //DO NOTHING
#ifdef USE_ARCANE_V3
    Arcane::IItemFamily* cellFamily_asIItemFamily = m_dc->cellFamily()->itemFamily();
#else
    Arcane::IItemFamily* cellFamily_asIItemFamily = m_dc->cellFamily();
#endif
  VariableBuildInfo bilinear_coef_vb(cellFamily_asIItemFamily,
                                     IMPLICIT_UNIQ_NAME,
                                     IVariable::PPrivate | IVariable::PTemporary);
  m_bilinear_coef.reset( new VariableDoFArrayReal(bilinear_coef_vb));
}

  virtual ~BilinearMap(){};

  Real eval(const Real3 & P, const Discretization::Cell & K) ;

  typedef Discretization::ItemGroup CellsGroup;
  void compute(const RealArray & node_values, CellsGroup& callCells);

   Real3 gradient(const Real3 & P, const Discretization::Cell & K) ;


private:

  DiscretizationGeometry* m_dg;
  DiscretizationConnectivity* m_dc ;
  std::unique_ptr<VariableDoFArrayReal> m_bilinear_coef;
  
  void __compute_Map(const RealArray & node_values, CellsGroup& callCells);
 

};

#endif
