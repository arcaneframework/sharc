/*
 * DiscretizationCoefficientArrayBuilder.h
 *
 *  Created on: Mar 11, 2016
 *      Author: yousefs
 */

#ifndef SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONCOEFFICIENTARRAYBUILDER_H_
#define SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONCOEFFICIENTARRAYBUILDER_H_


#include <algorithm>
#include <list>

#include "DiscretizationCoefficientArray.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/BinarySearch.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/StencilBuilder.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

enum DiscretizationCoefficientBasicOperationType {
  O_Sum            = 0,
  O_Subtraction    = 1,
  O_Multiplication = 2,
  O_Division       = 3
};

template<DiscretizationCoefficientBasicOperationType Op>
struct DiscretizationCoefficientBasicOperation
{
};

template<>
struct DiscretizationCoefficientBasicOperation<O_Sum>
{
  static void apply(Real & a_l, const Real a_r)
  {
    a_l += a_r;
  }
};

template<>
struct DiscretizationCoefficientBasicOperation<O_Subtraction>
{
  static void apply(Real & a_l, const Real a_r)
  {
    a_l -= a_r;
  }
};

template<>
struct DiscretizationCoefficientBasicOperation<O_Multiplication>
{
  static void apply(Real & a_l, const Real a_r)
  {
    a_l *= a_r;
  }
};

template<>
struct DiscretizationCoefficientBasicOperation<O_Division>
{
  static void apply(Real & a_l, const Real a_r)
  {
    a_l /= a_r;
  }
};

/*----------------------------------------------------------------------------*/

template<typename T, 
         typename StencilBuilder = OrderedStencilBuilderT<T>,
         typename P = Integer>
struct DiscretizationCoefficientArrayBuilderWithInserterT {
  typedef StencilBuilder StencilBuilderType;
  typedef typename StencilBuilder::SearchAlgorithm SearchAlgorithm;
  typedef P PositionType;

  // Inserter initializer
  class Initializer
  {
    typedef typename std::list<PositionType> InsertionListType;

  public:
    Initializer(DiscretizationCoefficientArrayT<T> * a_coefficient_array)
      : m_coefficient_array(a_coefficient_array)
    {
      // do nothing
    }

    DiscretizationCoefficientArrayT<T> * coefficientArray()
    {
      return m_coefficient_array;
    }

    void sum(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Sum>(a_face, a_item, a_value);
    }

    void subtract(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Subtraction>(a_face, a_item, a_value);
    }

    void multiply(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Multiplication>(a_face, a_item, a_value);
    }

    void multiply(const Discretization::Face & a_face, const Real & a_value = 0)
    {
      ArrayView<Real> coefficients_face = m_coefficient_array->coefficients(a_face);
      for(ArrayView<Real>::iterator i=coefficients_face.begin(); i!=coefficients_face.end(); i++) {
        *i *= a_value;
      }
    }

    void divide(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Division>(a_face, a_item, a_value);
    }

    Real coefficient(const Discretization::Face & a_face, const T & a_item)
    {
      // Find and store position
      ArrayView<Integer> stencil_face = m_coefficient_array->stencilLocalId(a_face);
      PositionType position = SearchAlgorithm::apply(stencil_face, a_item.localId());
      ARCANE_ASSERT(position!=-1, ("Item not present in stencil"));
      // Insert coefficient
      return m_coefficient_array->coefficients(a_face)[position];
    }

    void reset(const Discretization::Face & a_face)
    {
      m_coefficient_array->coefficients(a_face).fill(0.);
    }

    void finalizeInsert()
    {
      m_current_position = m_insertion_list.begin();
    }

    void resetPosition()
    {
      m_current_position = m_insertion_list.begin();
    }

    Integer currentPosition() const
    {
      return *m_current_position;
    }

    void incrementPosition()
    {
      m_current_position++;
    }

  private:
    template<DiscretizationCoefficientBasicOperationType Op>
    void _insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      // Find and store position
      ArrayView<Integer> stencil_face = m_coefficient_array->stencilLocalId(a_face);
      PositionType position = SearchAlgorithm::apply(stencil_face, a_item.localId());
      ARCANE_ASSERT(position!=-1, ("Item not present in stencil"));
      m_insertion_list.push_back(position);
      // Insert coefficient
      ArrayView<Real> coefficients_face = m_coefficient_array->coefficients(a_face);
      DiscretizationCoefficientBasicOperation<Op>::apply(coefficients_face[position], a_value);
    }

  private:
    DiscretizationCoefficientArrayT<T> * m_coefficient_array;
    InsertionListType m_insertion_list;
    typename InsertionListType::const_iterator m_current_position;
  };

  // Inserter
  class Inserter
  {
  public:
    Inserter(Initializer * a_initializer)
      : m_initializer(a_initializer),
        m_coefficient_array(a_initializer->coefficientArray())
    {
    }

    void sum(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Sum>(a_face, a_item, a_value);
    }

    void subtract(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Subtraction>(a_face, a_item, a_value);
    }

    void multiply(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Multiplication>(a_face, a_item, a_value);
    }

    void multiply(const Discretization::Face & a_face, const Real & a_value = 0)
    {
      ArrayView<Real> coefficients_face = m_coefficient_array->coefficients(a_face);
      for(ArrayView<Real>::iterator i=coefficients_face.begin(); i!=coefficients_face.end(); i++)
        *i *= a_value;
    }

    void divide(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Division>(a_face, a_item, a_value);
    }

    Real coefficient(const Discretization::Face & a_face, const T & a_item)
    {
      // Find and store position
      ArrayView<Integer> stencil_face = m_coefficient_array->stencilLocalId(a_face);
      PositionType position = SearchAlgorithm::apply(stencil_face, a_item.localId());
      ARCANE_ASSERT(position!=-1, ("Item not present in stencil"));
      // Insert coefficient
      return m_coefficient_array->coefficients(a_face)[position];
    }

    void reset(const Discretization::Face & a_face)
    {
      m_coefficient_array->coefficients(a_face).fill(0.);
    }

  private:
    template<DiscretizationCoefficientBasicOperationType Op>
    void _insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      ArrayView<Real> coefficients_face = m_coefficient_array->coefficients(a_face);
      DiscretizationCoefficientBasicOperation<Op>::apply(coefficients_face[m_initializer->currentPosition()],
                                           a_value);
      m_initializer->incrementPosition();
    }

  private:
    Initializer * m_initializer;
    DiscretizationCoefficientArrayT<T> * m_coefficient_array;
  };
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T, 
         typename StencilBuilder = OrderedStencilBuilderT<T>,
         typename P = Integer>
struct DirectDiscretizationCoefficientArrayBuilderT
{
  typedef typename StencilBuilder::SearchAlgorithm SearchAlgorithm;
  typedef StencilBuilder StencilBuilderType;
  typedef P PositionType;

  class Inserter;
  typedef Inserter Initializer;

  class Inserter
  {
  public:
    Inserter(DiscretizationCoefficientArrayT<T> * a_coefficient_array)
      : m_coefficient_array(a_coefficient_array)
    {
      // do nothing
    }

    Inserter(Initializer * a_initializer)
      : m_coefficient_array(a_initializer->coefficientArray())
    {
      // do nothing (for compatibility only)
    }

        DiscretizationCoefficientArrayT<T> * coefficientArray()
    {
      return m_coefficient_array;
    }

    void sum(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Sum>(a_face, a_item, a_value);
    }

    void subtract(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Subtraction>(a_face, a_item, a_value);
    }

    void multiply(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Multiplication>(a_face, a_item, a_value);
    }

    void multiply(const Discretization::Face & a_face, const Real & a_value = 0)
    {
      ArrayView<Real> coefficients_face = m_coefficient_array->coefficients(a_face);
      for(ArrayView<Real>::iterator i=coefficients_face.begin(); i!=coefficients_face.end(); i++)
        *i *= a_value;
    }

    void divide(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      _insert<O_Division>(a_face, a_item, a_value);
    }

    Real coefficient(const Discretization::Face & a_face, const T & a_item)
    {
      // Find and store position
      ArrayView<Integer> stencil_face = m_coefficient_array->stencilLocalId(a_face);
      PositionType position = SearchAlgorithm::apply(stencil_face, a_item.localId());
      ARCANE_ASSERT(position!=-1, ("Item not present in stencil"));
      // Insert coefficient
      return m_coefficient_array->coefficients(a_face)[position];
    }

    void reset(const Discretization::Face & a_face)
    {
      m_coefficient_array->coefficients(a_face).fill(0.);
    }

    void finalizeInsert()
    {
      // do nothing (for compatibility only)
    }

    void resetPosition()
    {
      // do nothing (for compatibility only)
    }

    Integer currentPosition() const
    {
      // return meaningless value (for compatibility only)
      return -1;
    }

    void incrementPosition()
    {
      // do nothing (for compatibility only)
    }
  private:
    DiscretizationCoefficientArrayT<T> * m_coefficient_array;
    
    template<DiscretizationCoefficientBasicOperationType Op>
    void _insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0)
    {
      // Find and store position
      ArrayView<Integer> stencil_face = m_coefficient_array->stencilLocalId(a_face);
      PositionType position = SearchAlgorithm::apply(stencil_face, a_item.localId());
      ARCANE_ASSERT(position!=-1, ("Item not present in stencil"));
      // Insert coefficient
      ArrayView<Real> coefficients_face = m_coefficient_array->coefficients(a_face);
      DiscretizationCoefficientBasicOperation<Op>::apply(coefficients_face[position], a_value);
    }
  };
};



#endif /* SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONCOEFFICIENTARRAYBUILDER_H_ */

/*----------------------------------------------------------------------------*/
