#ifndef IMESHOPERATOR_H_
#define IMESHOPERATOR_H_

class IMeshOperator{
	public :
	
	  virtual ~IMeshOperator(){}
	  virtual void init() {} ;
	  virtual void ApplyOperator() = 0 ;
};


#endif /*IMESHOPERATOR_H_*/
