#ifndef VARIABLEUPDATERT_H
#define VARIABLEUPDATERT_H

#include "ArcGeoSim/Utils/VisitorT.h"

class INonLinearSystemVisitor;

template<typename NonLinearSystemType>
class VariableUpdaterT 
  : public INonLinearSystemVisitor
  , public VisitorT<NonLinearSystemType>
{
public:
  //! Constructeur de la classe
  VariableUpdaterT()
    : m_initialized(false) 
  {
    ;
  } 
  
  /** Destructeur de la classe */
  virtual ~VariableUpdaterT() {}
  
public:
  
  //! Initialisation
  void init() { m_initialized = true; }

  //! Getting operator name
  String name() const { return "VariableUpdaterT" ; }
  
  //! Visiting non linear system
  Integer visit(NonLinearSystemType * system) 
  {
    return system->updateVariable() ;
  }
  
private :
   
  bool m_initialized;
};


#endif
