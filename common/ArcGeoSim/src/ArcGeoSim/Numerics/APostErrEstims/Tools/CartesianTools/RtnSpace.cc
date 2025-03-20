#include "RtnSpace.h"
#include <arcane/IMesh.h>

#include <arcane/IItemFamily.h>
#include <arcane/mesh/NodeFamily.h>
#include <arcane/mesh/FaceFamily.h>


void RtnSpace::compute(const RealSharedArray & flux_values, CellsGroup& callCells)
{
  __compute_Map(flux_values,callCells);
}


void RtnSpace::__compute_Map(const RealSharedArray & flux_values,CellsGroup& callCells )
{

  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL
  //    ************* !!!!!!!!!!!!!!!!! ATTENTION UNIT NORMAL

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
    RealMatrix rtnCoefT(numCellFaces,numCellFaces);
    RealVector fluxLeftSide(numCellFaces);
    Real hx,hy,hz;
    hx = hy = hz = std::pow(cellMeasures(icell), 1./3.);
    const Real3 xT = (cellCenters)(cell);

    const Arcane::Cell& arcCell = m_dc->cell(cell);
    const FaceVectorView& faces = arcCell.faces();
    Integer i=0;
    ENUMERATE_FACE(iface,faces){
      fluxLeftSide(i) = 0;
      const Arcane::Face& arcFace = *iface;
      const Discretization::Face& face = m_dc->face(arcFace);
      const Real3 xF = (faceCenters)(face);
      const Real3 nTF = (faceNormals)(face);
      const Real  mF = (faceMeasures)(face);
      Real coefX1=(xF[0]-xT[0])/hx;
      Real coefY1=(xF[1]-xT[1])/hy;
      Real coefZ1=(xF[2]-xT[2])/hz;
      Real coefX2=1.;
      Real coefY2=1.;
      Real coefZ2=1.;

      rtnCoefT(i,0)=mF*nTF[0]*coefX1;
      rtnCoefT(i,1)=mF*nTF[0]*coefX2;
      rtnCoefT(i,2)=mF*nTF[1]*coefY1;
      rtnCoefT(i,3)=mF*nTF[1]*coefY2;
      rtnCoefT(i,4)=mF*nTF[2]*coefZ1;
      rtnCoefT(i,5)=mF*nTF[2]*coefZ2;
      if ((arcFace.isSubDomainBoundary() && arcFace.boundaryCell().isActive()) ||
            (not arcFace.isSubDomainBoundary() && arcFace.backCell().isActive() && arcFace.frontCell().isActive())){
        fluxLeftSide(i)= flux_values[face.localId()];
      }else{
        const Real3 iFaceOutwardUnitNormal = faceNormals(face);
        mesh::FaceFamily* face_family = dynamic_cast<mesh::FaceFamily *> (m_dc->mesh()->faceFamily());
        SharedArray<ItemInternal*> subfaces;
        face_family->subFaces(arcFace.internal(),subfaces);
        for(Integer j = 0; j < subfaces.size(); j++){
            const Arcane::Face& jArcFace = subfaces[j];
            const Discretization::Face& j_face = m_dc->face(jArcFace);
            if((jArcFace.isSubDomainBoundary() && jArcFace.boundaryCell().isActive()) ||
                    (not jArcFace.isSubDomainBoundary() && jArcFace.backCell().isActive() && jArcFace.frontCell().isActive()))
            {
            const Real3 jFaceOutwardUnitNormal = faceNormals(j_face);
            Real sign = math::scaMul(iFaceOutwardUnitNormal,jFaceOutwardUnitNormal);
            fluxLeftSide(i) += sign * flux_values[j_face.localId()];
            }
        }
      }
      i++;
    }
    /*
    const Discretization::ConnectedItems faces((m_dc)->faces(cell)) ;
    Integer i=0;
    ENUMERATE_DISCRETIZATION_FACE(iface,faces){
      const  Discretization::Face& face=*iface;


      const Real3 xF = (faceCenters)(face);
      const Real3 nTF = (faceNormals)(face);
      const Real  mF = (faceMeasures)(face);
      Real coefX1=(xF[0]-xT[0])/hx;
      Real coefY1=(xF[1]-xT[1])/hy;
      Real coefZ1=(xF[2]-xT[2])/hz;
      Real coefX2=1.;
      Real coefY2=1.;
      Real coefZ2=1.;

      rtnCoefT(i,0)=mF*nTF[0]*coefX1;
      rtnCoefT(i,1)=mF*nTF[0]*coefX2;
      rtnCoefT(i,2)=mF*nTF[1]*coefY1;
      rtnCoefT(i,3)=mF*nTF[1]*coefY2;
      rtnCoefT(i,4)=mF*nTF[2]*coefZ1;
      rtnCoefT(i,5)=mF*nTF[2]*coefZ2;

      // Face flux storage
      //const Discretization::Cell & up = m_dc->backCell(iface);
      //Integer sign = (cell.localId() == up.localId() ? 1 : -1);
      //Real epsilonTF = isEqual(cell, T1) ? 1. : -1.;
      fluxLeftSide(i)= flux_values[face.localId()];
      i++;
    }*/
    //std::cout<<"solve Quad"<<"\n";
    //Eigen::VectorXd fluxLeftSideNew=rtnCoefT.lu().solve(fluxLeftSide);
    PermutationMatrix PERM(numCellFaces);
    Integer test=lu_factorize(rtnCoefT, PERM);

   // WARNING TEST == 0
    if(test != 0)
      std::cout << "      I ' M   OUT     " << std::endl;

    lu_substitute(rtnCoefT, PERM, fluxLeftSide);

     for(Integer i=0;i!=numCellFaces;i++)
      m_rtn_coef[icell][i]=fluxLeftSide(i);


  }

}

Real3 RtnSpace::eval(const Real3 & P, const Discretization::Cell & cell)
{
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;

  Real3 result(0.,0.,0.);
  ConstArrayView<Real> Coef=m_rtn_coef[cell];

  Real hx,hy,hz;
  hx = hy = hz = std::pow(cellMeasures(cell), 1./3.);
  const Real3 xT = (cellCenters)(cell);
  Real coefX1=(P[0]-xT[0])/hx;
  Real coefY1=(P[1]-xT[1])/hy;
  Real coefZ1=(P[2]-xT[2])/hz;
  Real coefX2=1.;
  Real coefY2=1.;
  Real coefZ2=1.;

  result[0]=Coef[0]*coefX1+Coef[1]*coefX2;
  result[1]=Coef[2]*coefY1+Coef[3]*coefY2;
  result[2]=Coef[4]*coefZ1+Coef[5]*coefZ2;

  return result;

}

Real RtnSpace::div(const RealSharedArray & flux_values, const Discretization::Cell & cell)
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

