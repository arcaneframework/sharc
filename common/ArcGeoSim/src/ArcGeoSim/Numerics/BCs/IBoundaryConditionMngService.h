#ifndef IBOUNDARYCONDITIONMNGSERVICE_H
#define IBOUNDARYCONDITIONMNGSERVICE_H



class IBoundaryCondition;

/*!
  \class IBoundaryConditionMngService
  \author Daniele Di Pietro <daniele-antonio.di-pietro@ifp.fr>
  \date 2007-27-7
  \brief Base class for boundary condition manager service
*/

class IBoundaryConditionMngService {
public:
  virtual ~IBoundaryConditionMngService() { }
  virtual void init() = 0;
  virtual IBoundaryCondition* boundaryCondition(const String& tag) = 0;
};

#endif
