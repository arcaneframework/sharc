#ifndef GRADMLSTENCILS_H
#define GRADMLSTENCILS_H

#include <arcane/ArcaneVersion.h>
#include <arcane/VariableTypedef.h>
#include "ArcGeoSim/Numerics/DiscreteOperator/IDivKGradDiscreteOperator.h"

using namespace Arcane;

class L_Stencil{
  private:
// local cell numbers of the cells in the stencil
    SharedArray<Integer> cells;
// local face numbers of the faces in the stencil
    SharedArray<Integer> faces;
  public:
//  Container for transmissibilities


    SharedArray2<Real> LocBeta;
    void setCell(Integer i, Integer numcell);
    Integer getCell(Integer i);
    void setFace(Integer i , Integer numface);
    Integer getFace(Integer i);
    L_Stencil(const Cell & cell, std::vector<Byte> & StFaces);
    ~L_Stencil(){};
};


#endif
