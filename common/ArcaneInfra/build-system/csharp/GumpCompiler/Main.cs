using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace GumpCompiler
{
  public class Version
  {
    public static String Name = "Gump";
    public static String Number = "1.0.0";
    public static String Date = DateTime.Now.ToString (System.Globalization.CultureInfo.InvariantCulture);
  }

  class MainClass
  {
    public static void Main (string[] args)
    {
      bool help = false;
      string xml_file = null;
      string path = null;
      bool verbose = false;

      var p = new NDesk.Options.OptionSet () {
        { "h|?|help", "Show help message and exit", v => help = v != null },
        { "xml=", "Data Model XmL describing file", v => xml_file = v },
        { "path=", "Path where C++ data model files are generated", v => path = v },
        { "v|verbose", "Enable verbosity", v => verbose = v != null }
      };

      p.Parse (args);

      //verbose = true;

      //xml = "C:/Users/desrozis/Desktop/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/Gump/Xsd/test/test.xml";
      //path = "C:/Users/desrozis/Desktop/ArcaneDemo-Gump/common/ArcaneInfra/tools/ArcaneInfra.Tools/Gump/Xsd/test/gen";

      //xml_file = "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/src/Gump/Example.xml";
      //path = "/work/IRLIN276_1/desrozis/IFPEN/working/ArcaneDemo-Gump/src/Gump/gen";

      if (xml_file == null) {
        Console.WriteLine ("xml option is mandatory");
        help = true;
      }

      if (!File.Exists (xml_file)) {
        Console.WriteLine ("xml file doesn't exist...");
        help = true;
      }

      if (path == null) {
        Console.WriteLine ("path option is mandatory");
        help = true;
      }

      if (help) {
        Console.WriteLine ("usage: Gump.exe [OPTION]+\n");
        Console.WriteLine ("Valid options include:");
        p.WriteOptionDescriptions (Console.Out);
        Environment.Exit (-1);
      }

      if (verbose) {
        Console.WriteLine ("Data-model generator xml -> c++");
        Console.WriteLine ("Data-model file : {0}", xml_file);
        Console.WriteLine ("Data-model directory : {0}", path);
      }

      gump gump = null;
      var serializer = new XmlSerializer (typeof(gump));
      if (verbose) {
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine ("parse data-model from xml file");
        Console.ResetColor ();
      }
      try {
        using (StreamReader stream = File.OpenText(xml_file)) {
          try {
            using (XmlReader reader = XmlReader.Create(stream, Xml.CreateXmlSettings (xml_file))) {
              gump = serializer.Deserialize (reader) as gump;
            }
          } catch(Exception e) {
            Console.Error.WriteLine("XmlReader Unexpected Exception : {0}", e.Message);
            Console.Error.WriteLine("Exception : {0}", e.GetType().FullName);
            var inner = e.InnerException;
            if(inner != null) {
              Console.Error.WriteLine("XmlReader Unexpected Exception : {0}", inner.Message);
              Console.Error.WriteLine("Exception : {0}", inner.GetType().FullName);
            }
            Environment.Exit(-1);
          } 
        }
      } catch(Exception e) {
        Console.Error.WriteLine("StreamReader Unexpected Exception : {0}", e.Message);
        Environment.Exit(-1);
      } 

      Directory.CreateDirectory (path);
      
      path += "/";
      
      path += gump.model.@namespace;
      
      Directory.CreateDirectory (path);

      path += "/";

      if (verbose) {
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine ("save xml-data-model in xml file");
        Console.ResetColor ();
      }
      using (var writer = File.CreateText(path + "xml-data-model.xml")) {
        serializer.Serialize (writer, gump);
      }

      if (verbose) {
        Console.ForegroundColor = ConsoleColor.Green;
        Console.WriteLine ("start generation of data model");
        Console.ResetColor ();
      }

      Directory.CreateDirectory (path + "Entities");
      Directory.CreateDirectory (path + "Builders");
      Directory.CreateDirectory (path + "Properties");

      var session = new Dictionary<string, object> ();
      session ["gump"] = gump;
      session ["path"] = gump.model.@namespace;

      { // Type de propriétés
        var gen = new PropertyKindT4 ();
        gen.Session = session;
        var file = path + "Properties/PropertyKind.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Traits pour les propriétés
        var gen = new PropertyTraitsT4 ();
        gen.Session = session;
        var file = path + "Properties/PropertyTraits.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      foreach (var property in gump.Properties()) { // Propriétés
        var gen = new PropertyT4 ();
        session ["property"] = property;
        gen.Session = session;
        var file = path + "Properties/" + property.name + ".h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Type de Entités
        var gen = new EntityKind ();
        gen.Session = session;
        var file = path + "Entities/EntityKind.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Traits pour les entités
        var gen = new EntityTraitsT4 ();
        gen.Session = session;
        var file = path + "Entities/EntityTraits.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Tags pour les entités
        var gen = new EntityTagT4 ();
        gen.Session = session;
        var file = path + "Entities/EntityTag.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Toutes les entités
        var gen = new AllEntities ();
        gen.Session = session;
        var file = path + "Entities.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Toutes les propriétés
        var gen = new AllProperties ();
        gen.Session = session;
        var file = path + "Properties.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      foreach (var entity in gump.BaseEntities()) { // Entités de base
        var gen = new BaseEntityT4 ();
        gen.Session = session;
        session ["entity"] = entity;
        var file = path + "Entities/" + entity.name + ".h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      foreach (var entity in gump.BaseEntities()) { // Builder des entités de base
        var gen = new BaseEntityBuilderT4 ();
        gen.Session = session;
        session ["entity"] = entity;
        var file = path + "Builders/" + entity.name + "Builder.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      foreach (var entity in gump.DerivedEntities()) { // Entités dérivées
        var gen = new DerivedEntityT4 ();
        gen.Session = session;
        session ["entity"] = entity;
        var file = path + "Entities/" + entity.name + ".h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      foreach (var entity in gump.DerivedEntities()) { // Builder des entités dérivées
        var gen = new DerivedEntityBuilderT4 ();
        gen.Session = session;
        session ["entity"] = entity;
        var file = path + "Builders/" + entity.name + "Builder.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Fabrique d'entités
        var gen = new FactoryT4 ();
        gen.Session = session;
        var file = path + "Builders/Factory.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }

      { // Fabrique d'entités
        var gen = new XPathT4 ();
        gen.Session = session;
        var file = path + "XPath.h";
        if (verbose) {
          Console.ForegroundColor = ConsoleColor.Red;
          Console.WriteLine ("* generate {0}", file);
          Console.ResetColor ();
        }
        using (StreamWriter writer = new StreamWriter(file)) {
          writer.Write (gen.TransformText ());
        }
      }
    }
  }
}
