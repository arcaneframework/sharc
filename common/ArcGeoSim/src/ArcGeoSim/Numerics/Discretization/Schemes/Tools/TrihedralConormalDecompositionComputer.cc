//
// Created by encheryg on 20/06/24.
//

#include "TrihedralConormalDecompositionComputer.h"

TrihedralConormalDecompositionComputer::TrihedralConormalDecompositionComputer(DiscretizationGeometry& dg,
                                                                               TraceOperator& traceOp,
                                                                               const Arcane::VariableCellReal3x3& conductivity)
                                                             : ConormalDecompositionComputer(dg)  {
  constexpr auto epsilon(1.e-14) ;

  DiscretizationCellVariableRefTReaderWrapper<Arcane::Real3x3> varWrapper(*dg.discretizationConnectivity(), conductivity) ;

  // Geometric data
  OrientedUnitNormals<Discretization::Face> normals(&m_dg);
  Centers<Discretization::Cell> cellCenters(&m_dg);
  Centers<Discretization::Face> geoFaceCenters(&m_dg);

  const auto dc(m_dg.discretizationConnectivity()) ;

  struct _LocalFaceWithData {
    Arcane::Integer m_cellLocalIndex ;
    Arcane::Real m_distToCenter ;
  };

  Arcane::UniqueArray<_LocalFaceWithData> data ;
  data.reserve(m_maxNbFaces) ;

  Arcane::Real3UniqueArray t ;
  t.reserve(m_maxNbFaces) ;

  Arcane::RealUniqueArray tn ;
  tn.reserve(m_maxNbFaces) ;

  Arcane::Real3UniqueArray nt ;
  nt.reserve(m_maxNbFaces) ;

  Arcane::Real3UniqueArray cn ;
  cn.reserve(m_maxNbFaces) ;

  Arcane::RealUniqueArray ncn ;
  ncn.reserve(m_maxNbFaces) ;

  auto orthoNormalize = [](std::array<Arcane::Real3, 3>& orthoNormV) {
    for(auto n = 0 ; n < 2 ; ++n) {
      auto& v0 = orthoNormV[0] ;
#ifdef USE_ARCANE_V3
      v0 /= v0.normL2() ;
#else
      v0 /= v0.abs() ;
#endif
      for(auto i = 1 ; i < 3 ; ++i) {
        auto& vi = orthoNormV[i] ;
        for(auto j = 0 ; j < i ; ++j) {
          const auto& vj = orthoNormV[j] ;
          vi -= Arcane::math::dot(vi, vj) * vj ;
        }
      }
    }
  };

  auto compare =
      [](const _LocalFaceWithData &l1, const _LocalFaceWithData &l2) {
        return l1.m_distToCenter < l2.m_distToCenter;
      };

  auto diffRelNormInfMax(0.) ;

  ENUMERATE_DISCRETIZATION_CELL(iCell, dc -> allCells()) {
    const Arcane::Cell arcCell(dc -> cell(iCell)) ;

    Discretization::ConnectedItems faces(dc->faces(iCell));

    const auto &cellDiffusion = varWrapper[iCell];

    const auto &cellCenter = cellCenters(iCell);

    data.clear() ;
    t.clear() ;
    tn.clear() ;
    cn.clear() ;
    ncn.clear() ;
    ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
      const bool isBackBound = (dc->backCell(iFace).localId() == iCell->localId());

      const Discretization::FaceCellInd iFaceCellInd =
          (isBackBound ? Discretization::FaceCellInd::Back : Discretization::FaceCellInd::Front);

      const Arcane::Real3 iOutwardUnitNormal = normals(iFace, iFaceCellInd);

      auto normCoNormal(Arcane::math::prodTensVec(cellDiffusion, iOutwardUnitNormal));
#ifdef USE_ARCANE_V3
      cn.push_back(normCoNormal) ;

      const auto coNormalL2Norm(normCoNormal.normL2()) ;
      normCoNormal /= coNormalL2Norm ;

      ncn.push_back(coNormalL2Norm) ;

      const auto &faceCenter = traceOp.point(*iFace);
      auto v(faceCenter - cellCenter);

      t.push_back(v) ;
      const auto vNorm(v.normL2()) ;
      v /= vNorm;
      nt.push_back(v) ;
      tn.push_back(vNorm) ;
#else
      cn.add(normCoNormal) ;

      const auto coNormalL2Norm(normCoNormal.abs()) ;
      normCoNormal /= coNormalL2Norm ;

      ncn.add(coNormalL2Norm) ;

      const auto &faceCenter = traceOp.point(*iFace);
      auto v(faceCenter - cellCenter);

      t.add(v) ;
      const auto vNorm(v.abs()) ;
      v /= vNorm;
      nt.add(v) ;
      tn.add(vNorm) ;
#endif

      const int intFCInd(static_cast<int>(iFaceCellInd));

      ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
        auto &coeff((m_coeffs[iFace->localId()])[intFCInd][jFace.index()]);
        coeff = 0.0;
      }
    }

    bool decompositionFound(false) ;
    ENUMERATE_DISCRETIZATION_FACE(iFace, faces) {
      const auto iIndex(iFace.index());

      data.clear();
      const auto incn(cn[iIndex] / ncn[iIndex]);
      ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
        const auto jIndex(jFace.index());

        if (jIndex != iIndex) {
          const auto diff(nt[jIndex] - incn);
#ifdef USE_ARCANE_V3
          const auto dist(diff.normL2());
          data.push_back({jIndex, dist});
#else
          const auto dist(diff.abs());
          data.add({jIndex, dist});
#endif
        }
      }
      std::sort(data.begin(), data.end(), compare);

      //const Arcane::Face arcFace(dc -> face(iFace)) ;

      auto nrMaxFound(std::numeric_limits<Arcane::Real>::infinity());
      auto nrMinFound(std::numeric_limits<Arcane::Real>::lowest());

      std::array<Arcane::Integer, 3> iTriplet{0, 0, 0};
      std::array<Arcane::Real, 3> rTriplet{nrMaxFound, nrMaxFound, nrMaxFound};

      /*const auto onStop(arcCell.uniqueId().asInt64() == 9475 && arcFace.uniqueId().asInt64() == 29449) ;
      if(onStop) {
        dc->mesh()->traceMng()->info() << Arcane::String::format("On stop : cn={0}", cn[fi]);
        dc->mesh()->traceMng()->info() << Arcane::String::format("On stop : cc={0}", cellCenter);
        const auto nodeCoords(dc->mesh()->nodesCoordinates()) ;
        ENUMERATE_NODE(iNode, arcCell.nodes()) {
          dc->mesh()->traceMng()->info() << Arcane::String::format("On stop : node[{0}]={1}", iNode.index(),
                                                                   nodeCoords[iNode]);
        }
      }*/

      for (auto j = 0; j < data.size() - 1; ++j)
        for (auto k = j + 1; k < data.size(); ++k) {
          const std::array<Arcane::Real3, 3> tt = {t[iIndex], t[data[j].m_cellLocalIndex],
                                                   t[data[k].m_cellLocalIndex]};

          std::array<Arcane::Real3, 3> orthoNTT = tt;
          orthoNormalize(orthoNTT);

          const auto ndf(Arcane::math::mixteMul(orthoNTT[2], orthoNTT[0], orthoNTT[1]));

          if (std::abs(ndf) < epsilon)
            continue;

          const auto df(Arcane::math::mixteMul(tt[2], tt[0], tt[1]));

          const auto d1(Arcane::math::mixteMul(tt[2], cn[iIndex], tt[1]));
          const auto d2(Arcane::math::mixteMul(tt[2], tt[0], cn[iIndex]));
          const auto d3(Arcane::math::mixteMul(cn[iIndex], tt[0], tt[1]));
          const std::array<Arcane::Real, 3> r = {d1 / df, d2 / df, d3 / df};
          const std::array<Arcane::Real, 3> ttn = {tn[iIndex], tn[data[j].m_cellLocalIndex],
                                                   tn[data[k].m_cellLocalIndex]};
          const std::array<Arcane::Real, 3> nr = {r[0] / ncn[iIndex] * ttn[0],
                                                  r[1] / ncn[iIndex] * ttn[1],
                                                  r[2] / ncn[iIndex] * ttn[2]};

          const auto nrMin = std::min({nr[0], nr[1], nr[2]});
          const auto nrMax = std::max({nr[0], nr[1], nr[2]});

          const bool allNonNegative = nrMin >= 0.;
          const bool allLessOrEqOne = nrMax <= 1.;

          if (allNonNegative) {
            if (allLessOrEqOne) {
              iTriplet = {iIndex, j, k};
              rTriplet = {r[0], r[1], r[2]};

              /*if(onStop) {
                dc->mesh()->traceMng()->info() << Arcane::String::format("On stop : optimal triplet", cn[fi]);
              }*/
              decompositionFound = true ;
              goto endLoop;
            } else if (nrMax < nrMaxFound) {
              nrMaxFound = nrMax;
              nrMinFound = nrMin;
              iTriplet = {iIndex, j, k};
              rTriplet = {r[0], r[1], r[2]};
              decompositionFound = true ;
            }
          } else if (nrMinFound < 0 && nrMin > nrMinFound) {
            nrMaxFound = nrMax;
            nrMinFound = nrMin;
            iTriplet = {iIndex, j, k};
            rTriplet = {r[0], r[1], r[2]};
            decompositionFound = true ;
          }
        }

      if(!decompositionFound)
        dc -> mesh() -> traceMng() -> fatal() << Arcane::String::format("No decomposition found for cell-face pair ({0}, {1})",
                                                                        dc -> cell(iCell).uniqueId().asInt64(),
                                                                        dc -> face(iFace).uniqueId().asInt64()) ;

      endLoop:

      const bool isBackBound = (dc->backCell(iFace).localId() == iCell->localId());
      const Discretization::FaceCellInd iFaceCellInd =
          (isBackBound ? Discretization::FaceCellInd::Back : Discretization::FaceCellInd::Front);
      const int intFCInd(static_cast<int>(iFaceCellInd));

      for (int i = 0; i < 3; ++i) {
        const auto cli((i==0) ? iTriplet[i] : data[iTriplet[i]].m_cellLocalIndex);
        auto &c(m_coeffs[iFace->localId()][intFCInd][cli]);
        c = rTriplet[i];
      }

      /*if(arcFace.localId() == 3185) {
        dc->mesh()->traceMng()->info() << Arcane::String::format("##### problematic face {0} with back cell {1} and front cell {2}",
                                         arcFace.localId(), arcFace.backCell().localId(),
                                         arcFace.frontCell().localId()) ;
        dc->mesh()->traceMng()->info() << Arcane::String::format("Conormal = {0}", cn[fi]) ;
        dc->mesh()->traceMng()->info() << Arcane::String::format("Geo center vect= {0}", geoFaceCenters(*iFace)-cellCenters(iCell)) ;
        dc->mesh()->traceMng()->info() << Arcane::String::format("Trace center vect= {0}", traceOp.point(*iFace)-cellCenters(iCell)) ;
        ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
          const auto fj(jFace.index());

          const auto& c = m_coeffs[iFace->localId()][intFCInd][fj] ;
          const auto& v = t[fj] ;

          const auto jArcFaceLId(dc->face(jFace).localId()) ;
          dc->mesh()->traceMng()->info() << Arcane::String::format("problematic face c({2}) = {0}, v = {1}", c, v,
                                                                   jArcFaceLId) ;
        }

      }*/

      // Check decomposition
      auto diff = cn[iIndex];

      ENUMERATE_DISCRETIZATION_FACE(jFace, faces) {
        const auto jIndex(jFace.index());

        const auto &c = m_coeffs[iFace->localId()][intFCInd][jIndex];
        const auto &v = t[jIndex];
        diff -= c * v;

        //dc->mesh()->traceMng()->info() << Arcane::String::format("c = {0}, v = {1}", c, v) ;
      }

      auto diffNormInf(0.);
      auto coNormalNormInf(0.);

      for (auto i(0); i < 3; ++i) {
        diffNormInf = std::max({diffNormInf, std::fabs(diff[i])});
        coNormalNormInf = std::max({coNormalNormInf, std::fabs(cn[iIndex][i])});
      }

      const auto diffRelNormInf = (coNormalNormInf > std::numeric_limits<Arcane::Real>::epsilon()) ?
                                  diffNormInf / coNormalNormInf : diffNormInf ;

      if(iCell -> isOwn() && diffRelNormInf > diffRelNormInfMax) {
        diffRelNormInfMax = diffRelNormInf ;
      }
    }
  }
  
  diffRelNormInfMax = dc->mesh()->parallelMng()->reduce(Arcane::IParallelMng::eReduceType::ReduceMax,
                                                        diffRelNormInfMax) ;
  dc->mesh()->traceMng()->info() <<
  Arcane::String::format("Infinite relative norm of conormal-decomposition error = {0}",diffRelNormInfMax) ;
}
