using System;
using System.IO;
using System.Collections.Generic;
using System.Xml;
using System.Text;

namespace ConfigFileCreator
{
  public class ConfigFileCreatorMain
  {
    public static int Main(string[] args)
    {
      // Document
      var document = new XmlDocument();
      var declaration = document.CreateXmlDeclaration("1.0", null, null);
      declaration.Encoding = "ISO-8859-1";
      
      document.AppendChild(declaration);

      var writer = new XmlTextWriter("config.xml", Encoding.GetEncoding("ISO-8859-1"));

      // Start
      writer.Formatting = Formatting.Indented;
      writer.WriteStartDocument();
      writer.WriteStartElement("makefile");
      writer.WriteStartElement("needed-packages");
      writer.WriteStartElement("package");
      writer.WriteStartAttribute("name");
      writer.WriteString("arcane");
      writer.WriteEndAttribute();
      writer.WriteEndElement();
      writer.WriteEndElement();

      // Arcane rules
      var analyzer = new ArcaneDirectoryAnalyzer(".");
      
      var file_writer = new XmlFileWriter();

      // Files
      file_writer.Add("c++",       analyzer.CppFiles);
      file_writer.Add("c++header", analyzer.HeaderFiles);
      file_writer.Add("axl",       analyzer.AxlFiles);
      file_writer.Add("module",    analyzer.ModuleFiles);
      file_writer.Add("service",   analyzer.ServiceFiles);
      file_writer.Write(writer);
      
      var directory_writer = new XmlDirectoryWriter();

      // Directories
      directory_writer.Add(analyzer.Directories);
      directory_writer.Write(writer);

      // End
      writer.WriteEndElement();
      writer.WriteEndDocument();
      writer.Close();

      return 0;
    }
  }
}
