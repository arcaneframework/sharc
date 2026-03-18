#pragma once

namespace ShArc
{
class IDynamicMeshMng
{
public:
  IDynamicMeshMng(){}
  virtual ~IDynamicMeshMng(){}

  virtual void init() = 0 ;
  virtual void updateNewEvent() = 0 ;
};
}
