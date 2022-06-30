using System;
using System.Xml.Xsl;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.Text;
using System.Reflection;

using System.Collections.Generic;


using Mono.Options;

namespace LawCompiler
{
  class MainClass
  {
    private static int RTCODE_OK = 0;
    private static int RTCODE_ERROR = 1;

    public static void Main (string[] args)
    {
      bool verbose = false;
      bool help = false;

      string law = null;
      string output = null;

      var options = new OptionSet () {
        { "v|verbose", "Verbose mode", v => { verbose = true; } },
        { "h|help", "Show help page", v => { help = true; } },
        { "law:", "XML law file to transform", v => { law = v; } },
        { "output:", "Output directory", v => { output = v; } },
      };
      
      options.Parse (args);

      //verbose = true;
      //law = "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/src/Law/Example1.law";
      //output = "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/src/Law/gen";

      var root = Path.GetFileNameWithoutExtension (law);

      if (help) {
        Console.WriteLine ("Help : XsltProcessor.exe [OPTIONS]");
        options.WriteOptionDescriptions (Console.Out);
        Environment.Exit (RTCODE_OK);
      }
      
      if (String.IsNullOrEmpty (law)) {
        Console.WriteLine ("Error, XML law file null or empty\nHelp :");
        options.WriteOptionDescriptions (Console.Out);
        Environment.Exit (RTCODE_ERROR);
      }
      
      if (String.IsNullOrEmpty (output)) {
        Console.WriteLine ("Error, output null or empty\nHelp :");
        options.WriteOptionDescriptions (Console.Out);
        Environment.Exit (RTCODE_ERROR);
      }

      var assembly = Assembly.GetExecutingAssembly ();
      var stream = assembly.GetManifestResourceStream ("LawCompiler.Law2Axl.xsl");
      var xsl = XmlReader.Create (stream);

      var trans = new XslCompiledTransform (); 
      
      if (verbose) {
        Console.WriteLine ("* Load XSLT file");
      }
      try {
        trans.Load (xsl); 
      } catch (Exception e) {
        Console.WriteLine ("Error in loading XSLT : {0}", e.Message);
        Environment.Exit (-1);
      }

      if (verbose) {
        Console.WriteLine ("* Apply XSLT to generate {0}.axl", root);
      }

      trans.Transform (law, Path.Combine (output, root + ".axl"));

      if (verbose) {
        Console.WriteLine ("* Load XML model", root);
      }

      Law model = null;
     
      var serializer = new XmlSerializer (typeof(Law));
      
      try {
        using (var str = System.IO.File.OpenText(law)) {
          using (var reader = XmlReader.Create(str, Xml.CreateXmlSettings ())) {
            model = serializer.Deserialize (reader) as Law;
          }
        }
      } catch(Exception e) {
        if(e.InnerException == null)
          Console.Error.WriteLine("Unexpected Exception : {0}", e.Message);
        else {
          var exception = e.InnerException as XmlException;
          Console.Error.WriteLine("\n* VALIDATION ERROR :");
          Console.Error.WriteLine("  File    : {0}", law);
          Console.Error.WriteLine("  Line    : {0}", exception.LineNumber);
          Console.Error.WriteLine("  Column  : {0}", exception.LinePosition);
          if(exception.Message.Contains("Expected >")) {
            Console.Error.WriteLine("  Message : Invalid closing xml node <\\>");
          } else {
            Console.Error.WriteLine("  Unexpected Exception");
          }
          Console.Error.WriteLine(" [Internal exception catched : {0}]", exception.Message);
          Console.Error.WriteLine();
          Console.Error.WriteLine("Please, check your xml files and reconfigure!");
        }
      }

      if (model.name != root) {
        Console.WriteLine("file should have name of law {0} != {1}", root, model.name);
        Environment.Exit (RTCODE_ERROR);
      }

      using (StreamWriter writer = new StreamWriter(Path.Combine (output, root + "_law.h"))) {
        if (verbose) {
          Console.WriteLine ("* Generate service {0}_law.h", root);
        }
        var service = new LawServiceHeaderT4 ();
        service.Law = model;

        writer.Write (service.TransformText ());
      }

//      using (StreamWriter writer = new StreamWriter(Path.Combine (output, root + "_law.cc"))) {
//        if (verbose) {
//          Console.WriteLine ("* Generate service {0}_law.cc", root);
//        }
//        var service = new LawServiceCppT4 ();
//        service.Law = model;
//        
//        writer.Write (service.TransformText ());
//      }
    }
  }
}
