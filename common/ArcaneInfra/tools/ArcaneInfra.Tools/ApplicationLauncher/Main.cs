using System;

namespace ApplicationLauncher
{
  class MainClass
  {
    private static readonly int RTCODE_OK = 0;
    private static readonly int RTCODE_ERROR = 1;

    static int Main (string[] args)
    {
      // AssemblyResolver.Initialize ();
      // AssemblyResolver.AddDirectory ("bin");
	  DateTime start = DateTime.Now ;
      Launcher launcher = new Launcher ();
      try {
        launcher.parseArguments (args);
        launcher.Run ();
		double elapsedTIme = (double)((DateTime.Now - start).TotalMilliseconds) / 1000;
		Console.WriteLine ("Execution completed in {0} seconds", elapsedTIme);
        return (RTCODE_OK);
      } catch (ExitException e) {
        Console.WriteLine (e.Message);
        return (RTCODE_OK);
      } catch (Exception e) {
		double elapsedTIme = (double)((DateTime.Now - start).TotalMilliseconds) / 1000;
		Console.WriteLine (String.Format ("Execution exiting after {1} seconds with an error: {0}", e.Message, elapsedTIme));
        return (RTCODE_ERROR);
      }
    }
  }
}
