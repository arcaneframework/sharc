#ifndef IMESHGENERATOR_H_
#define IMESHGENERATOR_H_

class IMeshGenerator
{
public:
  IMeshGenerator() {}
  virtual ~IMeshGenerator() {}
  
  virtual void init() = 0 ;
  
  virtual void generate() = 0 ;
};

#endif /*IMESHGENERATOR_H_*/
