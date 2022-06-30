using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;

namespace XmlToCMake
{
  class Program
  {
    private static int RTCODE_OK = 0;
    private static int RTCODE_ERROR = 1;

    private static string codename = "XmlToCMake.exe";
       
    static void Main (string[] args)
    {
      bool show_help = false;
            
      var parser = new XmlToCMakeLibraryParser ();

      var options = new Mono.Options.OptionSet () {
                { "v|verbose", "Verbose mode", v => parser.Verbose = true },
                { "h|help", "Show help page", v => show_help = true },
                { "axldb:", "axldb file including all .axl file paths", v => parser.AxlDbFile = v },
                { "eclipse-config:", "Generate Eclipse configuration for includes and macro definitions", v => parser.EclipseConfigFile = v }
      };

      try {

        var extras = options.Parse (args);

        // Check if remaining args looks like options
        foreach (var arg in extras) {
          if (arg.StartsWith ("-")) {
            Console.Error.WriteLine ("Invalid option {0}", arg);

            showHelpAndExit (options);

            Environment.Exit (RTCODE_ERROR);
          }
        }

        if (show_help || args.Length < 6) {

          showHelpAndExit (options);

          Environment.Exit (RTCODE_OK);
        }

        parser.Parse (extras);

        Environment.Exit (RTCODE_OK);

      } catch (Mono.Options.OptionException e) {

        Console.Error.WriteLine (e.Message);

        showHelpAndExit (options);

        Environment.Exit (RTCODE_ERROR);

      } catch (Exception e) {
      
        Console.Error.WriteLine ("XmlToCMake exiting with an unexpected exception :");
        Console.Error.WriteLine ("  Message : {0}", e.Message);
        Console.Error.WriteLine ("  Message : {0}", e.InnerException.Message);
        Console.Error.WriteLine ("  Type    : {0}", e.GetType().FullName);

        Environment.Exit (RTCODE_ERROR);
      
      }
    }

    static private void showHelpAndExit (Mono.Options.OptionSet options)
    {
      Console.WriteLine ("Requested Help page");
     
      Console.WriteLine ("Usage: {1} [options] LIBRARIES_FILE PACKAGES_FILE ARCANE_PACKAGES_FILE OUTPUT_FILE CMAKE_SOURCE_DIR {0}", 
                             "CMAKE_BINARY_DIR PATHS_TO_INCLUDE [PATHS_TO_INCLUDE_IN_OLD_FORMAT...]", codename);
      Console.WriteLine ("Options : ");
      options.WriteOptionDescriptions (Console.Out);
    }
  }
}
