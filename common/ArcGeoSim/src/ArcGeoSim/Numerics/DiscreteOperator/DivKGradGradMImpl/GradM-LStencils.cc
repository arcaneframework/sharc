#include <vector>
#include "GradM-LStencils.h"

using namespace Arcane;

L_Stencil::L_Stencil(const Cell & cell, std::vector<Byte> & StFaces){
  cells.resize(4);
  faces.resize(3);
  LocBeta.resize(3,4);
  cells[0] = 0;
  for (Integer i = 0; i < 3; i++) {
    // find cells and faces
    faces[i] = StFaces[i];
    const Face & face = cell.face(StFaces[i]);
    if (!face.isSubDomainBoundary())
      cells[i+1] = StFaces[i]+1;
    else
      cells[i+1] = -1;
  }
}
