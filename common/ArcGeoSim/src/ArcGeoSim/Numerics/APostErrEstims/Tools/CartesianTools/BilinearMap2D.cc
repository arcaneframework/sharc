
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
 

#include "BilinearMap2D.h"





void BilinearMap2D::compute(const RealSharedArray & node_values,CellsGroup& callCells)
{
  this->__compute_Map(node_values, callCells);
}

void BilinearMap2D::__compute_Map(const RealSharedArray & node_values,
                                 CellsGroup& callCells)
{
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  Centers<Discretization::Node> nodeCenters(m_dg) ;

  const Integer numCellNodes = 4;
  m_bilinear_coef.resize(4);
  ENUMERATE_DISCRETIZATION_CELL(icell, callCells) {
    const Discretization::Cell& cell = *icell;
    // Coef per node
    RealMatrix bilinearCoefT(numCellNodes,numCellNodes);
    RealVector cellLeftSide(numCellNodes);
    Real hx,hy,hz;
    Measures<Discretization::Cell> cellMeasures(m_dg) ;
    hx=hy=hz=std::sqrt(cellMeasures(cell));
    const Real3 xT = (cellCenters)(cell);
    // Interpolation value at the nodes
    const Discretization::ConnectedItems nodes((m_dc)->nodes(cell)) ;
    Integer i=0;
    ENUMERATE_DISCRETIZATION_NODE(inode,nodes){
      const Discretization::Node& node=*inode;
      const Real3 xP = (nodeCenters)(node);

      Real coefX = (xP[2]-xT[2])/hx;
      Real coefY = (xP[1]-xT[1])/hy;
      Real coef0 = (coefX-0.5)*(coefY-0.5);
      Real coef1 = (coefX+0.5)*(coefY-0.5);
      Real coef2 = (coefX-0.5)*(coefY+0.5);
      Real coef3 = (coefX+0.5)*(coefY+0.5);
      bilinearCoefT(i,0)=coef0;
      bilinearCoefT(i,1)=coef1;
      bilinearCoefT(i,2)=coef2;
      bilinearCoefT(i,3)=coef3;
 
      cellLeftSide(i)= node_values[node.localId()];
      
      i++;
    }
    PermutationMatrix PERM(numCellNodes);
    Integer test=lu_factorize(bilinearCoefT, PERM);

    // WARNING TEST == 0
    if(test != 0)
      std::cout << "      I ' M   OUT     " << std::endl;
    lu_substitute(bilinearCoefT, PERM, cellLeftSide);

    for(Integer i=0;i!=numCellNodes;i++)
      m_bilinear_coef[cell][i]=cellLeftSide(i);
  }
}

 Real BilinearMap2D::eval(const Real3 & P, const Discretization::Cell & cell)
{

  Centers<Discretization::Cell> cellCenters(m_dg) ;
  ConstArrayView<Real> Coef=m_bilinear_coef[cell];

  Real hx,hy,hz;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  hx=hy=hz=std::sqrt(cellMeasures(cell));
 const Real3 xT = (cellCenters)(cell);
  Bilinear2D Pol(hx,hy,hz,xT,Coef);
  
  return Pol.eval(P);
}

 Real3 BilinearMap2D::gradient(const Real3 & P, const Discretization::Cell & cell)
{
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  ConstArrayView<Real> Coef=m_bilinear_coef[cell];

  Real hx,hy,hz;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  hx=hy=hz=std::sqrt(cellMeasures(cell));
  const Real3 xT = (cellCenters)(cell);

  Bilinear2D Pol(hx,hy,hz,xT,Coef);

  return Pol.gradient(P);
}
/*
 void BilinearMap2D::__compute_Map(const RealSharedArray & node_values,
                                  CellsGroup& callCells)
 {
   Centers<Discretization::Cell> cellCenters(m_dg) ;
   Centers<Discretization::Node> nodeCenters(m_dg) ;

   const Integer numCellNodes = 4;
   m_bilinear_coef.resize(4);
   ENUMERATE_DISCRETIZATION_CELL(icell, callCells) {
     const Discretization::Cell& cell = *icell;
     // Coef per node
     RealMatrix bilinearCoefT(numCellNodes,numCellNodes);
     RealVector cellLeftSide(numCellNodes);
     Real hx,hy,hz;
     Measures<Discretization::Cell> cellMeasures(m_dg) ;
     hx=hy=hz=std::sqrt(cellMeasures(cell));
     const Real3 xT = (cellCenters)(cell);
     // Interpolation value at the nodes
     const Discretization::ConnectedItems nodes((m_dc)->nodes(cell)) ;
     Integer i=0;
     ENUMERATE_DISCRETIZATION_NODE(inode,nodes){
       const Discretization::Node& node=*inode;
       const Real3 xP = (nodeCenters)(node);

       Real coefX=(xP[2]-xT[2])/hx;
       Real coefY=(xP[1]-xT[1])/hy;
       bilinearCoefT(i,0)=coefX*coefY;
       bilinearCoefT(i,1)=coefX;
       bilinearCoefT(i,2)=coefY;
       bilinearCoefT(i,3)=1;

       cellLeftSide(i)= node_values[node.localId()];

       i++;
     }
     PermutationMatrix PERM(numCellNodes);
     Integer test=lu_factorize(bilinearCoefT, PERM);

     // WARNING TEST == 0
     if(test != 0)
       std::cout << "      I ' M   OUT     " << std::endl;
     lu_substitute(bilinearCoefT, PERM, cellLeftSide);

     for(Integer i=0;i!=numCellNodes;i++)
       m_bilinear_coef[cell][i]=cellLeftSide(i);

   }
 }
*/
