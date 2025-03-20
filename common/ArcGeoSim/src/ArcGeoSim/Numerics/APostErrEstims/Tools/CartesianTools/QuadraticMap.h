#ifndef SRC_ERRORESTIMATES_TWODIMENSIONTOOLS_QUADRATICMAP_H_
#define SRC_ERRORESTIMATES_TWODIMENSIONTOOLS_QUADRATICMAP_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"
#include "ArcGeoSim/Utils/ItemGroupBuilder.h"

#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/array.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Quadratic.h"
#include "types.h"



using namespace Arcane;


//const Integer numCellFaces = 6;
/*!
  \struct QuadraticMap
  \author Soleiman YOUSEF
  \brief Map for Quadratique functions on a mesh
*/
class QuadraticMap{
  typedef boost::numeric::ublas::permutation_matrix<std::size_t> PermutationMatrix;

public:

  QuadraticMap(DiscretizationGeometry* dg)
    :m_dg(dg),
     m_dc(m_dg->discretizationConnectivity()),
     m_quadratic_coef(nullptr)
  {
#ifdef USE_ARCANE_V3
    Arcane::IItemFamily* cellFamily_asIItemFamily = m_dc->cellFamily()->itemFamily();
#else
    Arcane::IItemFamily* cellFamily_asIItemFamily = m_dc->cellFamily();
#endif
    //DO NOTHING
    VariableBuildInfo quadratic_coef_vb(cellFamily_asIItemFamily,
                                        IMPLICIT_UNIQ_NAME,
                                        IVariable::PPrivate | IVariable::PTemporary);
    m_quadratic_coef.reset( new VariableDoFArrayReal(quadratic_coef_vb));

  }

  virtual ~QuadraticMap(){};

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
  std::unique_ptr<VariableDoFArrayReal> m_quadratic_coef;



  void __compute_Map(const RealSharedArray & aprox_values,
		     const RealSharedArray & flux_values,
		     const VariableDoFReal3x3& perm,
		     CellsGroup& callCells);
};



#endif
