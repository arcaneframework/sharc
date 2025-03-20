#include "QuadraticMap2D.h"


void QuadraticMap2D::compute(const RealSharedArray & aprox_values,
                           const RealSharedArray & flux_values,
                           const VariableDoFReal3x3& perm,
                           CellsGroup& callCells)
{
  __compute_Map(aprox_values, flux_values, perm,callCells);
}


void QuadraticMap2D::__compute_Map(const RealSharedArray & aprox_values,
                                 const RealSharedArray & flux_values,
                                 const VariableDoFReal3x3& perm,
                                 CellsGroup& callCells)
{
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;

  Centers<Discretization::Face> faceCenters(m_dg) ;
  Measures<Discretization::Face> faceMeasures(m_dg) ;
  OrientedUnitNormals<Discretization::Face> faceNormals(m_dg) ;

  const Integer numCellFaces = 4;
  m_quadratic_coef.resize(5);
  ENUMERATE_DISCRETIZATION_CELL(icell, callCells) {
    const Discretization::Cell& cell = *icell;

    Real3x3 Perm_T=perm[*icell];

    // Coef per Flux
    RealMatrix quadraticCoefT(numCellFaces,numCellFaces);
   RealVector fluxLeftSide(numCellFaces);
    Real hx,hy;
    hx=hy=std::sqrt(cellMeasures(icell));
    const Real3 xT = (cellCenters)(cell);
    const Discretization::ConnectedItems faces((m_dc)->faces(cell)) ;
    Integer i=0;
    ENUMERATE_DISCRETIZATION_FACE(iface,faces){
      const Discretization::Face& face=*iface;



      const Real3 xF = (faceCenters)(face);
      const Real3 nTF = (faceNormals)(face);
      Real  mF = (faceMeasures)(face);
      Real coefX1=2*(xF[2]-xT[2])/std::pow(hx,2);
      Real coefY1=2*(xF[1]-xT[1])/std::pow(hy,2);
      Real coefX2=1./hx;
      Real coefY2=1./hy;
//mF=1;
      quadraticCoefT(i,0)=mF*nTF[2]*(-Perm_T[2][2])*coefX1;
      quadraticCoefT(i,1)=mF*nTF[1]*(-Perm_T[1][1])*coefY1;
      quadraticCoefT(i,2)=mF*nTF[2]*(-Perm_T[2][2])*coefX2;
      quadraticCoefT(i,3)=mF*nTF[1]*(-Perm_T[1][1])*coefY2;
      // Face flux storage
      const Discretization::Cell & up = m_dc->backCell(iface);
      Integer sign = (cell.localId() == up.localId() ? 1 : -1);
      //Real epsilonTF = isEqual(cell, T1) ? 1. : -1.;
      sign = 1.;
      fluxLeftSide(i)= sign * flux_values[face.localId()];

      i++;
    }
    //std::cout<<"solve Quad"<<"\n";
    if(Perm_T[0][0]+Perm_T[1][1]!=0){
    PermutationMatrix PERM(numCellFaces);
    Integer test=lu_factorize(quadraticCoefT, PERM);

   // WARNING TEST == 0
    if(test != 0)
      std::cout << "      I ' M   OUT     " << std::endl;

    lu_substitute(quadraticCoefT, PERM, fluxLeftSide);

     for(Integer i=0;i!=numCellFaces;i++)
      m_quadratic_coef[cell][i]=fluxLeftSide(i);

     m_quadratic_coef[cell][4]=(aprox_values[cell.localId()] - (m_quadratic_coef[cell][0]+m_quadratic_coef[cell][1])/12.);
    }
    else{
     for(Integer i=0;i!=numCellFaces+1;i++)
      m_quadratic_coef[cell][i]=0;
    }
  }

}

Real QuadraticMap2D::eval(const Real3 & P, const Discretization::Cell & cell)
{
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  ConstArrayView<Real> Coef=m_quadratic_coef[cell];

  Real hx,hy,hz;
  hx=hy=hz=std::sqrt(cellMeasures(cell));

  const Real3 xT = (cellCenters)(cell) ;// Il faut changer par (cellCenters)(cell) si on veut evaler a un P qui n'est pas le centre de la maille;
  Quadratic2D Pol(hx,hy,hz,xT,Coef);
  return Pol.eval(P);

}


Real3 QuadraticMap2D::gradient(const Real3 & P, const Discretization::Cell & cell)
{
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  ConstArrayView<Real> Coef=m_quadratic_coef[cell];

  Real hx,hy,hz;
  hx=hy=hz=std::sqrt(cellMeasures(cell));
  const Real3 xT = (cellCenters)(cell);// Il faut changer par (cellCenters)(cell) si on veut evaler a un P qui n'est pas le centre de la maille;
  Quadratic2D Pol(hx,hy,hz,xT,Coef);
  return Pol.gradient(P);

}

