#ifndef IINFOMODEL_H_
#define IINFOMODEL_H_

#include <fstream>

class IInfoModel
{
public :
  virtual ~IInfoModel() {}
  virtual void printInfo() = 0 ;
  virtual void printCurrentTimeInfo() = 0 ;
  virtual void initHistoryCurve(std::ofstream& fout) {}
};

#endif /*IINFOMODEL_H_*/
