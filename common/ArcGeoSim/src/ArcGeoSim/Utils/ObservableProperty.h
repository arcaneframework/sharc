// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef OBSERVABLE_PROPERTY_H
#define OBSERVABLE_PROPERTY_H



#include <arcane/VariableTypes.h>
#include <arcane/VariableTypedef.h>

#include <string>
#ifdef USE_BOOST_SIGNAL
#include <boost/signals.hpp>
#endif

/*!
  \class ObservableProperty
  \author D. Di Pietro <daniele-antonio.di-pietro@ifp.fr>
  \date 08-07-2008
  \brief A simple observable property.

  An observable property is a property which calls a set
  of functions (observers) on change.
  The candidate functions have the signature

  void <fct>()

  Class members may also be called using the following syntax:

  property.connectObserver(ObserverType<boost::bind<void>(&Class::member), &class_instance);

  Observers can be connected using the connectObserver members.
  Temporary blockage of connected observers may be obtained via the blockConnection
  member.
  Blocked connected observers are not called on change notifications.
  Connected observers can be unblocked via the unblockConnection member.
  Observers can be disconnected via the removeConnection member.

  ObservableProperty should be used to automatically account for 
  dependences.
 */
class ObservableProperty {
 public:
  //! Name type
  typedef Arcane::String NameType;
#ifdef USE_BOOST_SIGNAL
  //! Signal type
  typedef boost::signal<void()> SignalType;
  //! Observer type
  typedef SignalType::slot_function_type ObserverType;
  //! Connection type
  typedef boost::signals::connection ConnectionType;
#endif

 public:
  //! Constructor
  ObservableProperty() : m_name("UnknownProperty") {}
  ObservableProperty(const NameType& name) : m_name(name) {}

 public:
  //! Return property name
  const NameType& getName();

 public:
#ifdef USE_BOOST_SIGNAL
  //! Add observer
  ConnectionType connectObserver(ObserverType obs);
  //! Remove observer
  void removeConnection(ConnectionType& obs);
  //! Block observer
  void blockConnection(ConnectionType& obs);
  //! Unblock observer
  void unblockConnection(ConnectionType& obs);
#endif
  //! Notify change
  void notifyChange();
private:
#ifdef USE_BOOST_SIGNAL
  SignalType m_signal;
#endif
  NameType m_name;
};

template<typename ModelProperty,typename SupportType, typename ValueType>
class ObservablePropertyMng
{
public :
  typedef typename ModelProperty::eProperty PropertyType ;
  typedef typename ModelProperty::template PropertyTrait<SupportType,ValueType>::ElementType ElementType ;
#ifdef USE_BOOST_SIGNAL
  typedef ObservableProperty::ConnectionType ConnectionType ;
  typedef ObservableProperty::ObserverType ObserverType ;
  static int const NbProperties = ModelProperty::template PropertyTrait<SupportType,ValueType>::nb_properties ;
#endif
  ObservablePropertyMng() 
  {
#ifdef USE_BOOST_SIGNAL
    m_properties.resize(NbProperties) ;
    for(Integer i=0;i<NbProperties;i++)
      m_properties[i] = new ObservableProperty() ;
#endif
  }
  virtual ~ObservablePropertyMng()
  {
#ifdef USE_BOOST_SIGNAL
    for(Integer i=0;i<NbProperties;i++)
      delete m_properties[i] ;
    m_properties.resize(0); 
#endif
  }
#ifdef USE_BOOST_SIGNAL
  ConnectionType connectObserver(ElementType property,ObserverType obs)
  {
    return m_properties[property]->connectObserver(obs) ;
  }
  //! Remove observer
  void removeConnection(ElementType property,ConnectionType& obs)
  {
    return m_properties[property]->removeConnection(obs) ;
  }
  //! Block observer
  void blockConnection(ElementType property,ConnectionType& obs)
  {
    return m_properties[property]->blockConnection(obs) ;
  }
  //! Unblock observer
  void unblockConnection(ElementType property,ConnectionType& obs)
  {
    return m_properties[property]->unblockConnection(obs) ;
  }
#endif
  //! Notify change
  void notifyChange(ElementType property)
  {
    return m_properties[property]->notifyChange() ;
  }
private :
  SharedArray<ObservableProperty*> m_properties ;
};
#define PROPERTY_OBSERVER(class_name,func_name,instance_pointer) \
ObservableProperty::ObserverType(boost::bind<void>(&class_name::func_name,instance_pointer))

#endif
