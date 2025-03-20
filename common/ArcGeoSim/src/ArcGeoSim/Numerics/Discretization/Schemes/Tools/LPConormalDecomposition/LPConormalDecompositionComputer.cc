//
// Created by encheryg on 13/01/24.
//

#include <Highs.h>

#include "LPConormalDecompositionComputer.h"

LPConormalDecompositionComputer::LPConormalDecompositionComputer(DiscretizationGeometry& dg,
                                                                       TraceOperator& traceOp,
                                                                       const Arcane::VariableCellReal3x3& conductivity)
                                : ConormalDecompositionComputer(dg){
  DiscretizationCellVariableRefTReaderWrapper<Arcane::Real3x3> varWrapper(*dg.discretizationConnectivity(), conductivity) ;

  constexpr auto cAlpha = 1.0e+9 ;
  constexpr auto delta= std::numeric_limits<Arcane::Real>::epsilon() ;
  constexpr auto conormalThreshold = 0. ;

  constexpr auto minusInf(std::numeric_limits<double>::lowest()) ;
  constexpr auto inf(std::numeric_limits<double>::infinity()) ;

  const auto d(3) ;
  const auto dimShift((d==2) ? 1 : 0) ;

  // Geometric data
  OrientedUnitNormals<Discretization::Face> normals(&m_dg) ;
  Centers<Discretization::Cell> cellCenters(&m_dg) ;

  // Set common model data
  HighsModel coNormalModel ;

  coNormalModel.lp_.sense_ = ObjSense::kMinimize ;
  coNormalModel.lp_.offset_ = 0.0 ;

  // Column variables (tildeAlpha_i)_{i=0...nbFaces-1}, gamma
  const auto maxCol(m_maxNbFaces + 1) ;
  // Rows: conormal component-wise, constraints between tildeAlpha_i and gamma and on the weighted sum of tildeAlpha_i
  const auto maxRow(d + m_maxNbFaces + 1) ;
  const auto maxNNZ(d * m_maxNbFaces + 3 * m_maxNbFaces) ;

  coNormalModel.lp_.col_cost_.reserve(maxCol) ;
  coNormalModel.lp_.col_lower_.reserve(maxCol) ;
  coNormalModel.lp_.col_upper_.reserve(maxCol) ;
  coNormalModel.lp_.row_lower_.reserve(maxRow) ;
  coNormalModel.lp_.row_upper_.reserve(maxRow) ;
  coNormalModel.lp_.a_matrix_.start_.reserve(maxRow + 1) ;
  coNormalModel.lp_.a_matrix_.index_.reserve(maxNNZ) ;
  coNormalModel.lp_.a_matrix_.value_.reserve(maxNNZ) ;

  coNormalModel.lp_.a_matrix_.format_ = MatrixFormat::kRowwise ;

  // Create a Highs instance
  Highs highs ;
  highs.setOptionValue("log_to_console", false) ;
  auto smallMatrixValue(1.e-12) ;
  //highs.setOptionValue("small_matrix_value", smallMatrixValue) ;
  //highs.setOptionValue("solver", "simplex") ;
  //highs.setOptionValue("simplex_scale_strategy", 4) ;
  /*highs.setOptionValue("presolve", "on") ;
  highs.setOptionValue("simplex_strategy", 3) ;
  highs.setOptionValue("primal_feasibility_tolerance", 1e-10) ;
  highs.setOptionValue("dual_feasibility_tolerance", 1e-10) ;*/

  auto diffMax(0.) ;

  const auto dc(m_dg.discretizationConnectivity()) ;

  ENUMERATE_DISCRETIZATION_CELL(iCell, dc -> allCells()) {

    const Discretization::ConnectedItems faces(dc -> faces(iCell)) ;

    const auto nbFaces(faces.size()) ;

    const auto& cellConductivity = varWrapper[iCell] ;

    const auto& cellCenter = cellCenters(iCell) ;

    coNormalModel.lp_.num_col_ = nbFaces + 1 ;
    coNormalModel.lp_.num_row_ = d + nbFaces + 1 ;
    const std::size_t nnz((d + 3) * nbFaces) ;

    coNormalModel.lp_.col_cost_.resize(coNormalModel.lp_.num_col_) ;
    coNormalModel.lp_.col_lower_.resize(coNormalModel.lp_.num_col_) ;
    coNormalModel.lp_.col_upper_.resize(coNormalModel.lp_.num_col_) ;
    coNormalModel.lp_.row_lower_.resize(coNormalModel.lp_.num_row_) ;
    coNormalModel.lp_.row_upper_.resize(coNormalModel.lp_.num_row_) ;
    coNormalModel.lp_.a_matrix_.start_.resize(coNormalModel.lp_.num_row_+1) ;
    coNormalModel.lp_.a_matrix_.index_.resize(nnz) ;
    coNormalModel.lp_.a_matrix_.value_.resize(nnz) ;

    // Settings of the model that are common to all faces
    for(auto k(0) ; k < d ; ++k) {
      coNormalModel.lp_.row_lower_[k] = coNormalModel.lp_.row_upper_[k] = 0. ;
    }

    // Definition of the cost function + bounds on variables and constraints
    ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
      const auto i(iFace.index()) ;
      coNormalModel.lp_.col_cost_[i] = 1.0 ;
      coNormalModel.lp_.col_lower_[i] = minusInf ;
      coNormalModel.lp_.col_upper_[i] = cAlpha ;

      const auto k(d+i) ;
      coNormalModel.lp_.row_lower_[k] = 0.0 ;
      coNormalModel.lp_.row_upper_[k] = inf ;
    }

    coNormalModel.lp_.col_cost_[nbFaces] = nbFaces ;
    coNormalModel.lp_.col_lower_[nbFaces] = 0.0 ;
    coNormalModel.lp_.col_upper_[nbFaces] = cAlpha ;

    const auto lastRow(coNormalModel.lp_.num_row_ - 1) ;
    coNormalModel.lp_.row_lower_[lastRow] =  delta ;
    coNormalModel.lp_.row_upper_[lastRow] = inf ;

    // Definition of the constraint matrix
    std::size_t count(0) ;

    for(auto k(0) ; k < d ; ++k) {
      coNormalModel.lp_.a_matrix_.start_[k] = count ;
      ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
        const auto iFaceCenter = traceOp.point(iFace) ;

        const auto iFaceCellCenterVector(iFaceCenter - cellCenter) ;

#ifdef USE_ARCANE_V3
        const auto iFaceCenterDistance(iFaceCellCenterVector.normL2()) ;
#else
        const auto iFaceCenterDistance(iFaceCellCenterVector.abs()) ;
#endif

        const auto value(iFaceCellCenterVector[k + dimShift] / iFaceCenterDistance) ;
        if(fabs(value) > smallMatrixValue) {
          const auto i(iFace.index()) ;
          coNormalModel.lp_.a_matrix_.index_[count] = i ;
          coNormalModel.lp_.a_matrix_.value_[count] = value ;
          ++count ;
        }
      }
    }

    ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
      const auto i(iFace.index()) ;
      const auto k(d + i) ;
      coNormalModel.lp_.a_matrix_.start_[k] = count ;

      coNormalModel.lp_.a_matrix_.index_[count] = i ;
      coNormalModel.lp_.a_matrix_.value_[count] = 1.0 ;
      ++count ;

      coNormalModel.lp_.a_matrix_.index_[count] = nbFaces ;
      coNormalModel.lp_.a_matrix_.value_[count] = 1.0 ;
      ++count ;
    }

    coNormalModel.lp_.a_matrix_.start_[d + nbFaces] = count ;
    ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
      const auto iFaceCenter(traceOp.point(iFace)) ;

      const auto iFaceCellCenterVector(iFaceCenter - cellCenter) ;

#ifdef USE_ARCANE_V3
      const auto iFaceCenterDistance(iFaceCellCenterVector.normL2()) ;
#else
      const auto iFaceCenterDistance(iFaceCellCenterVector.abs()) ;
#endif

      const auto value(1. / iFaceCenterDistance) ;

      if(fabs(value) > smallMatrixValue) {
        const auto i(iFace.index()) ;
        coNormalModel.lp_.a_matrix_.index_[count] = i ;
        coNormalModel.lp_.a_matrix_.value_[count] = value ;
        ++count ;
      }
    }

    coNormalModel.lp_.a_matrix_.start_[lastRow+1] = count ;

    if(count > nnz) {
      dc -> mesh() -> traceMng() -> fatal() << "LP conormal decomposition : too large non zero entries" ;
    }

    HighsStatus returnStatus(highs.passModel(coNormalModel)) ;

    if(returnStatus != HighsStatus::kOk) {
      dc -> mesh() -> traceMng() -> fatal() << "LP conormal decomposition : invalid model" ;
    }

    // Calculations of the coefficients
    ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
      const auto isBackBound = (dc->backCell(iFace).localId() == iCell->localId()) ;

      const auto iFaceCellInd = (isBackBound ? Discretization::FaceCellInd::Back : Discretization::FaceCellInd::Front) ;

      const auto iOutwardUnitNormal = normals(iFace, iFaceCellInd) ;

      const auto iCoNormal = Arcane::math::prodTensVec(cellConductivity, iOutwardUnitNormal) ;

#ifdef USE_ARCANE_V3
      const auto iCoNormalNorm(iCoNormal.normL2()) ;
#else
      const auto iCoNormalNorm(iCoNormal.abs()) ;
#endif

      for(auto k(0) ; k < d ; ++k) {
        const auto value(iCoNormal[k + dimShift] / iCoNormalNorm) ;
        highs.changeRowBounds(k, value, value) ;
      }

      const auto i(iFace.index()) ;
      ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
        const auto j(jFace.index());
        if(i == j) {
          highs.changeColBounds(j, delta, cAlpha) ;
        } else {
          highs.changeColBounds(j, minusInf, cAlpha) ;
        }
      }


      // Debug
//      const auto& arcCell(dc -> cell(iCell)) ;
//      const auto& arcFace(dc -> face(iFace)) ;
//      std::cout << Arcane::String::format("*************** Constraints matrix for cell {0}, face {1}\n",
//                                          arcCell.localId(), arcFace.localId()) ;
//      if(arcFace.isSubDomainBoundary()) {
//        std::cout << Arcane::String::format("Boundary face\n") ;
//      }
//      auto value(0.) ;
//      for(auto k(0) ; k < d ; ++k) {
//        ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
//          const auto j(jFace.index());
//          highs.getCoeff(k,j,value) ;
//          std::cout << Arcane::String::format("A({0}, {1})={2}\n", k, j, value) ;
//        }
//        highs.getCoeff(k,faces.size(),value) ;
//        std::cout << Arcane::String::format("A({0}, {1})={2}\n", k, faces.size(), value) ;
//      }
//
//      for(auto k(d) ; k < d + faces.size() ; ++k) {
//        highs.getCoeff(k, k - d,value) ;
//        std::cout << Arcane::String::format("A({0}, {1})={2}\n", k, k - d, value) ;
//        highs.getCoeff(k, faces.size(),value) ;
//        std::cout << Arcane::String::format("A({0}, {1})={2}\n", k, faces.size(), value) ;
//      }
//
//      ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
//        const auto j(jFace.index());
//        highs.getCoeff(d+faces.size(),j,value) ;
//        std::cout << Arcane::String::format("A({0}, {1})={2}\n", d+faces.size(), j, value) ;
//      }
//      highs.getCoeff(d+faces.size(),faces.size(),value) ;
//      std::cout << Arcane::String::format("A({0}, {1})={2}\n", d+faces.size(), faces.size(), value) ;

//      std::cout << Arcane::String::format("cellCenter={0}\n", cellCenter) ;

//      ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
//        const auto &iFaceCenter(traceOp.point(iFace));
//
//        std::cout << Arcane::String::format("traceCenter={0}\n", iFaceCenter) ;
//
//        const auto iFaceCellCenterVector(iFaceCenter - cellCenter) ;
//
//        std::cout << Arcane::String::format("diffVector={0}\n", iFaceCellCenterVector) ;
//
//        const auto iFaceCenterDistance(iFaceCellCenterVector.abs()) ;
//
//        std::cout << Arcane::String::format("diffVectorNorm={0}\n", iFaceCenterDistance) ;
//      }


      returnStatus = highs.run() ;

      if(returnStatus != HighsStatus::kOk || highs.getModelStatus() != HighsModelStatus::kOptimal) {
        dc -> mesh() -> traceMng() -> pinfo() << "LP conormal decomposition : resolution failed. Now trying to relax the positivity of other face coefficients" ;

        /*const auto i(iFace.index()) ;
        ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
          const auto j(jFace.index());
          if(i == j) {
            highs.changeColBounds(j, delta, cAlpha) ;
          } else {
            highs.changeColBounds(j, minusInf, cAlpha) ;
          }
        }

        returnStatus = highs.run() ;

        if(returnStatus != HighsStatus::kOk || highs.getModelStatus() != HighsModelStatus::kOptimal)
          dc -> mesh() -> traceMng() -> fatal() << "LP conormal decomposition : resolution failed" ;*/
      }

      const auto& solution = highs.getSolution() ;

      // Sanity check of the solution
      auto iCoDecCoNorm(Arcane::Real3::zero()) ;
      /*auto info = dc -> mesh() -> traceMng() -> info() ;

      info << Arcane::String::format("***  LP Conorm. dec. cell {0}, face {1}\n",
      dc -> cell(iCell).localId(),
      dc -> face(iFace).localId()) ;*/

      ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
        const auto j(jFace.index()) ;

        const auto res(solution.col_value[j]) ;
        const auto truncRes((std::fabs(res) < conormalThreshold) ? 0. : res) ;

        const auto jFaceCenter(traceOp.point(jFace)) ;

        const auto jFaceCellCenterVector(jFaceCenter - cellCenter) ;

#ifdef USE_ARCANE_V3
        const auto jFaceCenterDistance(jFaceCellCenterVector.normL2()) ;
#else
        const auto jFaceCenterDistance(jFaceCellCenterVector.abs()) ;
#endif

        const auto ratio = iCoNormalNorm / jFaceCenterDistance ;

        auto& coeff(m_coeffs[iFace->localId()][static_cast<int>(iFaceCellInd)][j]) ;
        coeff = ratio * truncRes ;

        /*if(std::fabs(coeff) > std::numeric_limits<double>::epsilon())
          info << Arcane::String::format("alpha={0} cc={1} fac={2}\n", coeff, cellCenter, jFaceCenter) ;*/

        iCoDecCoNorm += coeff * jFaceCellCenterVector ;
      }
      const auto diff((iCoNormal-iCoDecCoNorm).abs()) ;

      diffMax = std::max(diff, diffMax) ;

      /*if(diff > 5.e-14) {
        dc -> mesh() -> traceMng() -> info() << Arcane::String::format("Diff on conormals for cell {0} and face {1}: {2}\n",
                                            dc -> cell(iCell).localId(), dc -> face(iFace).localId(),
                                            diff) ;
      }*/
    }
  }
  dc -> mesh() -> traceMng() -> info() << Arcane::String::format("LPCoDecCom max. diff.: {0}\n", diffMax) ;
}