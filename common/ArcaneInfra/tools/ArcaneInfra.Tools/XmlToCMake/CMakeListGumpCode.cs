using System;
using System.Linq;
using System.Collections.Generic;

namespace XmlToCMake
{
  public partial class CMakeListGump
  {
    public Application Application { get; private set; }

    public CMakeListGump (Application application)
    {
    }
        
    public String Mono {
      get {
        if(ArcaneInfra.Common.CommonTools.isWindows()) {
          return "";
        } else {
          return "mono";
        }
      }
    }
  }
}

