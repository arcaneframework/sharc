#include "RtnSpace2D.h"




void RtnSpace2D::compute(const RealArray & flux_values, CellsGroup& callCells)
{
  __compute_Map(flux_values,callCells);
}


void RtnSpace2D::__compute_Map(const RealArray & flux_values,CellsGroup& callCells )
{

  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
    Integer numCellFace = 4;

  Centers<Discretization::Cell> cellCenters(m_dg) ;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;

  Centers<Discretization::Face> faceCenters(m_dg) ;
  Measures<Discretization::Face> faceMeasures(m_dg) ;
  OrientedUnitNormals<Discretization::Face> faceNormals(m_dg) ;

  m_rtn_coef.resize(6);
  ENUMERATE_DISCRETIZATION_CELL(icell, callCells) {
    const Discretization::Cell& cell = *icell;

    // Coef per Flux
    //Eigen::VectorXd fluxLeftSide(numCellFaces);
    //Eigen::MatrixXd rtnCoefT(numCellFaces,numCellFaces);
    RealMatrix rtnCoefT(numCellFace,numCellFace);
    RealVector fluxLeftSide(numCellFace);
    Real hx,hy;
    hx=hy=std::sqrt(cellMeasures(icell));

    const Real3 xT = (cellCenters)(cell);
    const Discretization::ConnectedItems faces((m_dc)->faces(cell)) ;
    Integer i=0;
    ENUMERATE_DISCRETIZATION_FACE(iface,faces){
      const  Discretization::Face& face=*iface;


      const Real3 xF = (faceCenters)(face);
      const Real3 nTF = (faceNormals)(face);
      Real  mF = (faceMeasures)(face);
      Real coefX1=(xF[2]-xT[2])/hx;
      Real coefY1=(xF[1]-xT[1])/hy;
      Real coefX2=1.;
      Real coefY2=1.;
//mF =1.;
      rtnCoefT(i,0)=mF*nTF[2]*coefX1;
      rtnCoefT(i,1)=mF*nTF[2]*coefX2;
      rtnCoefT(i,2)=mF*nTF[1]*coefY1;
      rtnCoefT(i,3)=mF*nTF[1]*coefY2;

      // Face flux storage
      const Discretization::Cell & up = m_dc->backCell(iface);
      Integer sign = (cell.localId() == up.localId() ? 1 : -1);
      //Real epsilonTF = isEqual(cell, T1) ? 1. : -1.;
      sign = 1.;
      fluxLeftSide(i)= sign*flux_values[face.localId()];
      i++;
    }
    //std::cout<<"solve Quad"<<"\n";
    //Eigen::VectorXd fluxLeftSideNew=rtnCoefT.lu().solve(fluxLeftSide);
    PermutationMatrix PERM(numCellFace);
    Integer test=lu_factorize(rtnCoefT, PERM);

   // WARNING TEST == 0
    if(test != 0)
      std::cout << "      I ' M   OUT     " << std::endl;

    lu_substitute(rtnCoefT, PERM, fluxLeftSide);

     for(Integer i=0;i!=numCellFace;i++)
      m_rtn_coef[cell][i]=fluxLeftSide(i);


  }

}


Real3 RtnSpace2D::eval(const Real3 & P, const Discretization::Cell & cell)
{
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;

  Real3 result(0.,0.,0.);
  ConstArrayView<Real> Coef=m_rtn_coef[cell];

  Real hx,hy;
  hx=hy=std::sqrt(cellMeasures(cell));
  const Real3 xT = (cellCenters)(cell);
  Real coefX1=(P[2]-xT[2])/hx;
  Real coefY1=(P[1]-xT[1])/hy;
  Real coefX2=1.;
  Real coefY2=1.;

  result[0]=Coef[0]*coefX1+Coef[1]*coefX2;
  result[1]=Coef[2]*coefY1+Coef[3]*coefY2;
  result[2]=0.;

  return result;

}

Real RtnSpace2D::div(const RealArray & flux_values, const Discretization::Cell & cell)
{
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  Measures<Discretization::Face> faceMeasures(m_dg) ;

  Real result = 0.;

  const Real mT = (cellMeasures)(cell);
  const Discretization::ConnectedItems faces((m_dc)->faces(cell)) ;
  ENUMERATE_DISCRETIZATION_FACE(iface,faces){
    const Discretization::Face& face=*iface;
    //const Real mF = (faceMeasures)(face);

    const Discretization::Cell & up = m_dc->backCell(iface);
    Integer sign = (cell.localId() == up.localId() ? 1 : -1);

    result += sign * flux_values[face.localId()];///mF;
  }

  result /=mT;


  return result;

}

