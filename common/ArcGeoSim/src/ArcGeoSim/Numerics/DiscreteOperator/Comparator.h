// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef COMPARATOR_H
#define COMPARATOR_H

#include <list>
#include <boost/shared_ptr.hpp>

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
struct TrueComparatorT
{
//   // For compatibilty only
//   struct Initializer
//   {
//     void resetPosition() 
//     {
//       // do nothing
//     }

//     void finalizeInsert()
//     {
//       // do nothing
//     }
//   };

//   // For compatibility only
//   TrueComparatorT(Initializer * a_initializer = NULL)
//   {
//     // do nothing
//   }

  bool compare(const T & a_value)
  {
    return true;
  }
};

/*----------------------------------------------------------------------------*/

template<typename T>
struct FalseComparatorT
{
//   // For compatibilty only
//   struct Initializer
//   {
//     void resetPosition() 
//     {
//       // do nothing
//     }

//     void finalizeInsert()
//     {
//       // do nothing
//     }
//   };

//   // For compatibility only
//   FalseComparatorT(Initializer * a_initializer = NULL)
//   {
//     // do nothing
//   }

  bool compare(const T & a_value)
  {
    return false;
  }
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
struct ComparatorT
{
  typedef std::list<T> ListT;

  class Initializer 
  {
  public:
    Initializer()
    {
      // do nothing
    }

    void resetPosition()
    {
      m_current_position = m_list.begin();
    }

    typename ListT::const_iterator currentPosition() const
    {
      return m_current_position;
    }

    void incrementPosition()
    {
      m_current_position++;
    }

    bool compare(const T & a_value)
    {
      m_list.push_back(a_value);
      return true;
    }

  private:
    ListT m_list;
    typename ListT::const_iterator m_current_position;
  };

  ComparatorT(Initializer * a_initializer)
    : m_initializer(a_initializer)
  {
    m_initializer->resetPosition();
  }

  bool compare(const T & a_value)
  {
    bool comparison = (*(m_initializer->currentPosition())==a_value);
    m_initializer->incrementPosition();
    return comparison;
  }
  
private:
  boost::shared_ptr<Initializer> m_initializer;
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
