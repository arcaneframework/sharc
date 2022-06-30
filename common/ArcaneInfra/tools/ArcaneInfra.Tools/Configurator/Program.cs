using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.Xml.Linq;
using System.IO;
using System.ComponentModel;

namespace ArcaneInfra.Configure
{
  class Program
  {
    private static int RTCODE_OK = 0;
    private static int RTCODE_ERROR = 1;

    static void Main (string[] args)
    {
      // foreach (string s in args) { Console.WriteLine (">> {0}", s); }

      try {
        Configurator c = new Configurator ();
        c.configure (args);
        Trace.WriteLine ("Configuration completed");
        Environment.Exit (RTCODE_OK);
      } catch (ArcaneInfra.Configure.Configurator.ConfiguratorExit e) {
        Console.WriteLine (e.Message);
        Environment.Exit (RTCODE_OK);
      } catch (Exception e) {
        Console.Error.WriteLine (String.Format ("Configuration exiting with an error: {0}", e.Message));
        Environment.Exit (RTCODE_ERROR);
      }
    }
  }
}
