#ifndef ARCGEOSIM_TESTS_SERVICETESTERS_MESHPARTITIONERTESTER_IFPMESHPARTITIONERTESTERSERVICE_H
#define ARCGEOSIM_TESTS_SERVICETESTERS_MESHPARTITIONERTESTER_IFPMESHPARTITIONERTESTERSERVICE_H
/* Author : mesriy
 * 
 */

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"
//#include "Partitioner/IMeshPartitioner.h"

namespace Arcane { }
using namespace Arcane;

#include "IfpMeshPartitionerTester_axl.h"

//class IMeshPartitioner ;
class IfpMeshPartitionerTesterService :
  public ArcaneIfpMeshPartitionerTesterObject
{
public:
  //! Constructor
  IfpMeshPartitionerTesterService(const Arcane::ServiceBuildInfo & sbi) : 
    ArcaneIfpMeshPartitionerTesterObject(sbi) {}
  
  //! Destructor
  virtual ~IfpMeshPartitionerTesterService() {};
  
public:
  //! Initialization
  void init();
  //! Run the test
  int test();
private :
	
};

#endif /* ARCGEOSIM_TESTS_SERVICETESTERS_MESHPARTITIONERTESTER_IFPMESHPARTITIONERTESTERSERVICE_H */
