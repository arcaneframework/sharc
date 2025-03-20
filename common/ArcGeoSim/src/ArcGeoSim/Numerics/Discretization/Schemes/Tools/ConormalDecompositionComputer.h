//
// Created by encheryg on 13/01/24.
//

#ifndef CONORMALDECOMPOSITIONCOMPUTER_H
#define CONORMALDECOMPOSITIONCOMPUTER_H

#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/TraceOperator.h"

class ConormalDecompositionComputer {
public:
  enum Type {LP, TRIHEDRAL} ;

  /**
   * @param[in] Discretization::Face : Arcane::Face or Contact
   * @param[in] ind the side of the face
   * @param[in] index refers to the index of the face around the discretization cell located on the ind side of face
   * @return coefficent in the conormal decomposition
   */
  Arcane::Real coeff(const Discretization::Face& face, const Discretization::FaceCellInd ind,
                     const Arcane::Integer index) const ;

  /**
   * @param[in] face
   * @param[in] ind the side of the face
   * @param[in] index refers to the index of the face around the discretization cell located on the ind side of face
   * @return coefficent in the conormal decomposition
   */
  Arcane::Real coeff(const Arcane::Face& face, const Discretization::FaceCellInd ind,
                     const Arcane::Integer index) const ;

  /**
   * @param[in] contact (or facet)
   * @param[in] ind the side of the face
   * @param[in] index refers to the index of the face around the discretization cell located on the ind side of face
   * @return coefficent in the conormal decomposition
   */
  Arcane::Real coeff(const Contact& c, const Discretization::FaceCellInd ind,
                     const Arcane::Integer index) const ;

protected:
  /**
   * @param dg : geometry
   */
  ConormalDecompositionComputer(DiscretizationGeometry& dg) ;

  DiscretizationGeometry& m_dg ;

  /// Coefficients of the decomposition
  Arcane::UniqueArray<std::array<Arcane::RealUniqueArray,2>> m_coeffs ;

  Arcane::Integer m_maxNbFaces ;
};

#endif //CONORMALDECOMPOSITIONCOMPUTER_H
