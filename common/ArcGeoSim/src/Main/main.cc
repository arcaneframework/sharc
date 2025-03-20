#include "ArcGeoSim/Appli/Architecture/ArcGeoSimMain.h"

#ifdef ARCGEOSIM_USE_EXPORT
//#include "ArcaneDemoDynamicLoading.h"
#endif

int main(int argc, char* argv[])
{ 
  Arcane::ApplicationInfo info(&argc,&argv,
                               "ArcGeoSim",
                               Arcane::VersionInfo(1,0,0));
  
  return ArcGeoSim::main(info);
}
