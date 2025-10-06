#ifndef VISITORT_H_
#define VISITORT_H_

template<typename ObjectType>
class VisitorT
{
public:
  VisitorT(){}
  virtual ~VisitorT(){}
  virtual Integer visit(ObjectType* object) = 0 ;
};

#endif /*VISITORT_H_*/
