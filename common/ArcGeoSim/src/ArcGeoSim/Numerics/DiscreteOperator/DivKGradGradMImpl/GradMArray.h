#ifndef GRADMARRAY_H
#define GRADMARRAY_H

#include <arcane/utils/Array.h>
using namespace Arcane;


//  The class for storing coefficients Beta and of similar type. For every cell we need to store the
//  interpolation coefficients for each of its faces.
class GradMArray {
  private:
    Integer maxelem_per_cell;
    Integer maxlocalId;
    Integer ncells;

  public:
    SharedArray<Real> coeff; // coefficients
    SharedArray<Integer> index; // corresponding face numbers to recover the local L-stencil
    SharedArray<Integer> posF; // pointers to face coefficients and indexes
    SharedArray<Integer> posC; // pointers to lines in posF, corresponding to cells

    GradMArray(Integer size1,Integer size2, Integer maxlocalId);
    ~GradMArray(){};
  //! Return the size of the stencil
    Integer stencilSize(const Cell& cell, Integer iface);
  //! Return the local id of the elements in the stencil of face F
    ArrayView<Integer> stencil(const Cell& cell, Integer iface);
  //! Return the coefficients for face F
    ArrayView<Real> coefficients(const Cell& cell, Integer iface);
};

GradMArray::GradMArray(Integer size1,Integer size2, Integer maxlocalId){
  coeff.resize(size1);
  index.resize(size1);
  posF.resize(size2);
  posC.resize(maxlocalId);
}

Integer GradMArray::stencilSize(const Cell& cell, Integer iface){
  Integer pF = posF[posC[cell.localId()]+iface];
  return index[pF];
}

ArrayView<Integer> GradMArray::stencil(const Cell& cell, Integer iface){
  Integer pF = posF[posC[cell.localId()]+iface];
  Integer length = index[pF] - 1; //the cell itself is not included in the stencil, but the first coefficient corresponds to it
  return ArrayView<Integer>(length, &index[pF+1]);
}

ArrayView<Real> GradMArray::coefficients(const Cell& cell, Integer iface){
 Integer pF = posF[posC[cell.localId()]+iface];
 Integer length = index[pF] ;
 return ArrayView<Real>(length, &coeff[pF]);
}

#endif
