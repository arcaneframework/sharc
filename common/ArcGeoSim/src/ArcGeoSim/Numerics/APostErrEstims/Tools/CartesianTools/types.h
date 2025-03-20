#ifndef TYPES_H
#define TYPES_H

#include <string>

#include <list>
#include <set>

#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>


// ! not parameter dimension
#define DIM 2

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef bool Boolean;
//typedef int Integer;
typedef unsigned int Unsigned;
//typedef double Real;
//typedef std::string String;


/*!
  \typedef DIndex
  \author Daniele Di Pietro <daniele-antonio.di-pietro@ifp.fr>
  \brief DIM-index to identify a cell in a DIM-dimensional mesh
*/
typedef boost::numeric::ublas::bounded_vector<Integer, DIM> DIndex;
typedef boost::numeric::ublas::bounded_vector<Real, DIM> Point;


// Tensor types

typedef boost::numeric::ublas::diagonal_matrix<Real, 
                                               boost::numeric::ublas::row_major, 
                                               boost::numeric::ublas::bounded_array<Real, DIM> > DiagonalTensor;

// Matrix and vector types
typedef boost::numeric::ublas::vector<Boolean> BooleanVector;
typedef boost::numeric::ublas::vector<Integer> IntegerVector;
typedef boost::numeric::ublas::vector<Unsigned> UnsignedVector;
typedef boost::numeric::ublas::vector<Real> RealVector;

typedef boost::numeric::ublas::matrix<Boolean> BooleanMatrix;
typedef boost::numeric::ublas::matrix<Integer> IntegerMatrix;
typedef boost::numeric::ublas::matrix<Unsigned> UnsignedMatrix;
typedef boost::numeric::ublas::matrix<Real> RealMatrix;

typedef boost::numeric::ublas::matrix_row<BooleanMatrix> BooleanMatrixRow;
typedef boost::numeric::ublas::matrix_row<IntegerMatrix> IntegerMatrixRow;
typedef boost::numeric::ublas::matrix_row<UnsignedMatrix> UnsignedMatrixRow;
typedef boost::numeric::ublas::matrix_row<RealMatrix> RealMatrixRow;

typedef boost::numeric::ublas::matrix_column<BooleanMatrix> BooleanMatrixColumn;
typedef boost::numeric::ublas::matrix_column<IntegerMatrix> IntegerMatrixColumn;
typedef boost::numeric::ublas::matrix_column<UnsignedMatrix> UnsignedMatrixColumn;
typedef boost::numeric::ublas::matrix_column<RealMatrix> RealMatrixColumn;

typedef const boost::numeric::ublas::matrix_row<const BooleanMatrix> ConstBooleanMatrixRow;
typedef const boost::numeric::ublas::matrix_row<const IntegerMatrix> ConstIntegerMatrixRow;
typedef const boost::numeric::ublas::matrix_row<const UnsignedMatrix> ConstUnsignedMatrixRow;
typedef const boost::numeric::ublas::matrix_row<const RealMatrix> ConstRealMatrixRow;

typedef boost::numeric::ublas::matrix_column<const BooleanMatrix> ConstBooleanMatrixColumn;
typedef boost::numeric::ublas::matrix_column<const IntegerMatrix> ConstIntegerMatrixColumn;
typedef boost::numeric::ublas::matrix_column<const UnsignedMatrix> ConstUnsignedMatrixColumn;
typedef boost::numeric::ublas::matrix_column<const RealMatrix> ConstRealMatrixColumn;

typedef boost::numeric::ublas::zero_vector<Real> ZeroRealVector;
typedef boost::numeric::ublas::zero_matrix<Real> ZeroRealMatrix;
typedef boost::numeric::ublas::zero_vector<Integer> ZeroIntegerVector;
typedef boost::numeric::ublas::zero_matrix<Integer> ZeroIntegerMatrix;

typedef boost::numeric::ublas::vector_range<BooleanVector> BooleanVectorRange;
typedef boost::numeric::ublas::vector_range<RealVector> RealVectorRange;
typedef boost::numeric::ublas::vector_range<IntegerVector> IntegerVectorRange;
typedef boost::numeric::ublas::vector_range<UnsignedVector> UnsignedVectorRange;
typedef boost::numeric::ublas::range Range;

// List and set types
typedef std::list<int> IntegerList;
typedef std::set<unsigned> UnsignedSet;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


#endif
