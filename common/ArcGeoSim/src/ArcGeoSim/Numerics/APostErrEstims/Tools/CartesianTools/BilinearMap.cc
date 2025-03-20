
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
 

#include "BilinearMap.h"





void BilinearMap::compute(const RealArray & node_values,CellsGroup& callCells)
{
  this->__compute_Map(node_values, callCells);
}

void BilinearMap::__compute_Map(const RealArray & node_values,
                                 CellsGroup& callCells)
{
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  Centers<Discretization::Node> nodeCenters(m_dg) ;
  Measures<Discretization::Cell> cellMeasures(m_dg) ;

  const Integer numCellNodes = 8;
  (*m_bilinear_coef).resize(8);
  ENUMERATE_DISCRETIZATION_CELL(icell, callCells) {
    const Discretization::Cell& cell = *icell;
    // Coef per node
    RealMatrix bilinearCoefT(numCellNodes,numCellNodes);
    RealVector cellLeftSide(numCellNodes);
    Real hx,hy,hz;
    hx = hy = hz = std::pow(cellMeasures(cell), 1./3.);
    const Real3 xT = (cellCenters)(cell);
    const Arcane::Cell& arcCell = m_dc->cell(cell);
    const NodeVectorView& nodes = arcCell.nodes();
    Integer i=0;
    ENUMERATE_NODE(inode,nodes){
      const Arcane::Node& arcNode = *inode;
      const Discretization::Node& node = m_dc->node(arcNode);

      const Real3 xP = (nodeCenters)(node);

      Real coefX=(xP[0]-xT[0])/hx;
      Real coefY=(xP[1]-xT[1])/hy;
      Real coefZ=(xP[2]-xT[2])/hz;
      bilinearCoefT(i,0)=coefX*coefY*coefZ;
      bilinearCoefT(i,1)=coefX*coefY;
      bilinearCoefT(i,2)=coefY*coefZ;
      bilinearCoefT(i,3)=coefZ*coefX;
      bilinearCoefT(i,4)=coefX;
      bilinearCoefT(i,5)=coefY;
      bilinearCoefT(i,6)=coefZ;
      bilinearCoefT(i,7)=1;

      cellLeftSide(i)= node_values[node.localId()];

      i++;
    }
/*
    // Interpolation value at the nodes
    const Discretization::ConnectedItems nodes((m_dc)->nodes(cell)) ;
    Integer i=0;
    ENUMERATE_DISCRETIZATION_NODE(inode,nodes){
      const Discretization::Node& node=*inode;
      const Real3 xP = (nodeCenters)(node);

      Real coefX=(xP[0]-xT[0])/hx;
      Real coefY=(xP[1]-xT[1])/hy;
      Real coefZ=(xP[2]-xT[2])/hz;
      bilinearCoefT(i,0)=coefX*coefY*coefZ;
      bilinearCoefT(i,1)=coefX*coefY;
      bilinearCoefT(i,2)=coefY*coefZ;
      bilinearCoefT(i,3)=coefZ*coefX;
      bilinearCoefT(i,4)=coefX;
      bilinearCoefT(i,5)=coefY;
      bilinearCoefT(i,6)=coefZ;
      bilinearCoefT(i,7)=1;
 
      cellLeftSide(i)= node_values[node.localId()];
      
      i++;
    }
*/
    PermutationMatrix PERM(numCellNodes);
    Integer test=lu_factorize(bilinearCoefT, PERM);

    // WARNING TEST == 0
    if(test != 0)
      std::cout << "      I ' M   OUT     " << std::endl;
    lu_substitute(bilinearCoefT, PERM, cellLeftSide);

    for(Integer i=0;i!=numCellNodes;i++)
      (*m_bilinear_coef)[cell][i]=cellLeftSide(i);
    /*
    ENUMERATE_DISCRETIZATION_NODE(inode,nodes){
      const Discretization::Node& node=*inode;
      const Real3 xP = (nodeCenters)(node);
      //std::cout <<"eval : "<<eval(xP,cell)<<" ; "<<node_values[node.localId()]<<std::endl;
      }*/
  }
}

 Real BilinearMap::eval(const Real3 & P, const Discretization::Cell & cell)
{
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  ConstArrayView<Real> Coef=(*m_bilinear_coef)[cell];

  Real hx,hy,hz;
  hx = hy = hz = std::pow(cellMeasures(cell), 1./3.);
  const Real3 xT = (cellCenters)(cell);
  Bilinear Pol(hx,hy,hz,xT,Coef);
  
  return Pol.eval(P);
}

 Real3 BilinearMap::gradient(const Real3 & P, const Discretization::Cell & cell)
{
  Measures<Discretization::Cell> cellMeasures(m_dg) ;
  Centers<Discretization::Cell> cellCenters(m_dg) ;
  ConstArrayView<Real> Coef=(*m_bilinear_coef)[cell];

  Real hx,hy,hz;
  hx = hy = hz = std::pow(cellMeasures(cell), 1./3.);
  const Real3 xT = (cellCenters)(cell);

  Bilinear Pol(hx,hy,hz,xT,Coef);

  return Pol.gradient(P);
}
