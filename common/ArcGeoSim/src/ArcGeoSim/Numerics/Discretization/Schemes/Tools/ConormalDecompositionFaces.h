//
// Created by encheryg on 18/01/24.
//

#ifndef CONORMALDECOMPOSITIONFACES_H
#define CONORMALDECOMPOSITIONFACES_H

#include <vector>
#include <array>

#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ConormalDecompositionComputer.h"

class ConormalDecompositionFaces {
public:
  constexpr static Arcane::Real m_conormalThreshold = 0. ;

  /**
   * @details Builds the vector of the indices of the stencil faces for each pair
   * (Discretization::Face, Discretization::FaceCellInd). The indices correspond to the ones of the
   * vector returned by the method faces() of a DiscretizationConnectivity object.
   * @param[in] dc the used Discretization::Connectivity
   * @param[in] cdc the computed conormal decomposition
   */
  ConormalDecompositionFaces(const DiscretizationConnectivity& dc,
                             const ConormalDecompositionComputer& cdc,
                             const Arcane::Real maxRatioThreshold = m_conormalThreshold) ;

  /**
 * @param[in] face Discretization::Face
 * @param[in] ind Discretization::FaceCellInd
 * @return the indices of the stencil faces for the given pair
 */
  const Arcane::IntegerUniqueArray& indices(const Discretization::Face& face, const Discretization::FaceCellInd& ind) const ;

  /**
   * @details Returns the vector of the indices of the stencil faces for a pair
   * (Face, Discretization::FaceCellInd)
   * @param[in] face Arcane::Face
   * @param[in] ind Discretization::FaceCellInd
   * @return the indices of the stencil faces for the given pair
   */
  const Arcane::IntegerUniqueArray& indices(const Arcane::Face& face, const Discretization::FaceCellInd& ind) const ;

  /**
   * @details Returns the vector of the indices of the stencil faces for a pair
   * (Contact, Discretization::FaceCellInd)
   * @param[in] c Contact
   * @param[in] ind Discretization::FaceCellInd
   * @return the indices of the stencil faces for the given pair
   */
  const Arcane::IntegerUniqueArray& indices(const Contact& c, const Discretization::FaceCellInd& ind) const ;

private:
  const DiscretizationConnectivity& m_dc ;

  /// Indexes of the Discretization::Face s in the vector returned by the method faces of a Discretization::Connectivity
  Arcane::UniqueArray<std::array<Arcane::UniqueArray<Arcane::Integer>, 2>> m_faceIndices ;
};


#endif //CONORMALDECOMPOSITIONFACES_H
