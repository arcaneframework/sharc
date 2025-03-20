// -*- C++ -*-
#ifndef DIVKGRADLSCHEMESTENCILS_H
#define DIVKGRADLSCHEMESTENCILS_H
#include "ArcGeoSim/Utils/Utils.h"
#include <arcane/ArcaneVersion.h>
#include "arcane/VariableTypedef.h"
#include "ArcGeoSim/Utils/ItemGroupMap.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/IDivKGradDiscreteOperator.h"

using namespace Arcane;


class LScheme_CenteredStencil
{
  typedef std::vector<Integer> FaceNum;
  typedef std::vector<FaceNum> FaceNumByCell;
  typedef VariableCellInteger CellNum;

public:

  // local cell numbers of the cells in the stencil
  SharedArray<Integer> cells;
  // local face numbers of the faces in the stencil
  SharedArray<Integer> faces;
  // the number of faces in the stencil
  Integer numfaces;
  // the number of cells in the stencil
  Integer numcells;
  // the number of edges in the stencil
  Integer numedges;

  // 	Container for transmissibilities
  SharedArray2<Real> transm;
  //	Cell-to-edge connectivity table  NumLocEdgebyCell;
  SharedArray2<Integer> NumLocEdgebyCell;

  //Storage for edge centers
  SharedArray<Real3> EdgeCenters;

  LScheme_CenteredStencil(const Node& node, Integer icell, CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell,VariableNodeReal3 coords);

  ~LScheme_CenteredStencil(){};
};

class L_Stencil{
private:
  // local cell numbers of the cells in the stencil
  SharedArray<Integer> cells;
  // local face numbers of the faces in the stencil
  SharedArray<Integer> faces;
public:
  typedef std::vector<Integer> FaceNum;
  typedef std::vector<FaceNum> FaceNumByCell;
  typedef VariableCellInteger CellNum;

  void setCell(Integer i, Integer numcell);
  Integer getCell(Integer i);
  void setFace(Integer i , Integer numface);
  Integer getFace(Integer i);
  L_Stencil(const Node& node, Integer icell, CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell);
  L_Stencil(const Node& node, Integer icell, Integer iface, CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell);
  L_Stencil(const Node& node, Integer icell,
            ItemVectorT<Cell> node_cells,
            CellNum& NumLocCell, FaceNumByCell& NumLocfacesbyCell);
  // 	Container for transmissibilities
  SharedArray2<Real> transm;
};

#endif
