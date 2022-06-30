using System;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.Linq;
using ArcaneInfra.Common;
using System.Reflection;

namespace XmlToCMake
{
  public class XmlToCMakeLibraryParser
  {
#region CONFIGURATIONS
    private static string default_config_file = "config.xml";
    private static string default_axldb_file = "axldb.txt";
    private static string default_eclipse_config_file = "eclipse-config.xml";
#endregion

#region MEMBERS
    string m_axldb_file = default_axldb_file;
    string m_eclipse_config_file = default_eclipse_config_file;
    HashSet<string> m_axl_files = new HashSet<string> ();
    PackageList m_package_list;
    String m_cmake_source_dir;
    String m_cmake_binary_dir;
    String m_config_name;
#endregion

    public string AxlDbFile { 
      get {
        return m_axldb_file;
      }
      set {
        this.m_axldb_file = String.IsNullOrEmpty(value) ? default_axldb_file : value;
      }
    }

    public string EclipseConfigFile { 
      get {
        return m_eclipse_config_file;
      }
      set {
        this.m_eclipse_config_file = String.IsNullOrEmpty(value) ? default_eclipse_config_file : value;
      }
    }

    public bool Verbose { get; set; }

    private PackageList InitializePackageList (string packages_file, string arcane_packages_file)
    {
      // Parser pour virer les && (en mode windows)
      string content = null;
      using (StreamReader reader = new StreamReader(packages_file)) {
        content = reader.ReadToEnd ();
      }
      content = Regex.Replace (content, "&&", "&amp;&amp;");
      using (StreamWriter writer = new StreamWriter(packages_file)) {
        writer.Write (content);
      }

      var package_file_document = new XmlDocument ();
      package_file_document.Load (packages_file);
            
      var arcane_package_file_document = new XmlDocument ();
      arcane_package_file_document.Load (arcane_packages_file);
            
      var package_list = new PackageList (package_file_document, arcane_package_file_document);

      package_list.Verbose = Verbose;

      return package_list;
    }

    private static void prettyPrintException(Exception ex, XmlTextReader reader, String filename){
      Console.WriteLine ("Message: {0}", ex.Message);
      if(ex.InnerException != null){
        Console.WriteLine ("Inner Problem: {0}", ex.InnerException.Message);
        Console.WriteLine("Line Number: {0} Colone number: {1}", reader.LineNumber, reader.LinePosition);
      }
    }

    private T Initialize<T> (string file, string xsd)
      where T : class, new()
    {
      T model = null;

      var serializer = new XmlSerializer (typeof(T));

      try {
        using (StreamReader stream = System.IO.File.OpenText(file)) {
          using (XmlReader reader = XmlReader.Create(stream, Xml.CreateXmlSettings (xsd,file))) {
            model = serializer.Deserialize (reader) as T;
          }
        }
      } catch(Exception e) {
        if(e.InnerException == null)
          Console.Error.WriteLine("Unexpected Exception : {0}", e.Message);
        else {
          var exception = e.InnerException as XmlException;
          Console.Error.WriteLine("\n* VALIDATION ERROR :");
          Console.Error.WriteLine("  File    : {0}", file);
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
      
      return model;
    }

    public void Parse (List<string> args)
    {
      m_config_name = default_config_file;

      m_package_list = InitializePackageList (args [1], args [2]);

      var output_file = args [3];

      m_cmake_source_dir = args [4];
      m_cmake_binary_dir = args [5];

      var includes = args.GetRange (5, args.Count - 5);

      var application = Initialize<Application> (args [0], "Libraries.xsd");

      var f90_libraries = new List<Tuple<Libraries,Library>> ();

      foreach (var libraries in application.libraries) {

        libraries.check(Verbose);

        foreach (Library library in libraries.library) {

          var languages = new Languages ();

          languages.Use (ScanLibrary<CMakeList> (library, libraries));
        
          if(languages.Contains (Language.f90)) {
          
            ScanLibrary<CMakeListf90> (library, libraries, "_f90");

            f90_libraries.Add (Tuple.Create(libraries,library));
          }
        }

      }

      using (StreamWriter writer = new StreamWriter(output_file)) {

        var cmake = new CMakeListLibrariesXml (m_cmake_binary_dir, 
                                               includes, 
                                               application, 
                                               f90_libraries);

        writer.Write (cmake.TransformText ());
      }

      if (m_axldb_file != null) {
        System.IO.File.WriteAllLines (m_axldb_file, m_axl_files.ToArray ());
      }

      if (m_eclipse_config_file != null) {
        if (Verbose) {
          Console.WriteLine("Exporting Eclipse configuration for packages: {0}", 
                            String.Join(", ", m_package_list.GetEnabledPackageFile().Select(p => p.Name)));
        }

        using (StreamWriter writer = new StreamWriter(m_eclipse_config_file)) {
          var ecf = new EclipseConfigFile(m_package_list.GetEnabledPackageFile());
          writer.Write (ecf.TransformText ());
        }
      }

    }

    private Languages ScanLibrary<V> (Library library, Libraries main_library, string suffix = "")
      where V : new() 
    {
      var main_library_name = main_library.name.CamelCaseName ();
      var main_library_path = main_library.path;

      var name = library.name;

      var library_name = main_library_name + "_" + library.name.Replace ("-", "_") + suffix;

      Console.WriteLine (">>>>> Create Library {0}{1}", name, suffix);
     
      var tmp = Path.Combine (m_cmake_binary_dir, Path.Combine ("CMakeFiles", library_name));
      var output_path = tmp + ".dir";

      var languages = new Languages ();

      foreach (var directory in library.directory) {
        String input_path = Path.Combine (m_cmake_source_dir, main_library_path);
                
        var full_output_path = Path.Combine (output_path, main_library_path);

        languages.Use(ParseDirectory<V> (directory, library_name, input_path, full_output_path, 0));
      }

      using (StreamWriter writer = new StreamWriter(String.Format("{0}/library.cmake", output_path))) {

        var full_output_path = Path.Combine (output_path, main_library_path);

        var cmake = new CMakeListLibrary (library, m_package_list, library_name, full_output_path);

        writer.Write (cmake.TransformText ());
      }

      return languages;
    }

    public Languages ParseDirectory<T> (Directory directory, String library_name, String input_path, String output_path, int current_depth)
      where T : new() 
    {
      var input  = Path.Combine (input_path,  directory.Value);
      var output = Path.Combine (output_path, directory.Value);

      System.IO.Directory.CreateDirectory (output);

      if (Verbose) {
        if (current_depth == 0)
          Console.WriteLine (">     directory {0}", directory.Value);
        else
          Console.WriteLine (">     {0}subdirectory {1}", new string (' ', current_depth * 4), directory.Value);
      }

      var failed_dependencies = Dependencies.Failed (directory, m_package_list);

      var languages = new Languages ();

      if (failed_dependencies.Count == 0) {

        languages.Use(ParseConfigFile<T> (library_name, input, output, current_depth + 1));

      } else {
        Console.WriteLine ("WARNING: Skip subdirectory {0} due to not satisfied dependency '{1}'", input, String.Join (",", failed_dependencies.ToArray ()));
        // On crée un fichier makefile vide 
        using (StreamWriter writer = new StreamWriter(output.Replace("\\", "/") + "/makefile.cmake")) {
          writer.Write ("#\n# WARNING: Skip subdirectory {0} due to not satisfied dependency '{1}'\n#\n", input, String.Join (",", failed_dependencies.ToArray ()));
        }
      }

      return languages;
    }
        
    public Languages ParseConfigFile<T> (String library_name, String input_path, String output_path, int current_depth)
      where T : new() 
    {
      var config_path = Path.Combine (input_path, m_config_name).ToString ();
            
      var makefile = Initialize<Makefile> (config_path, "Makefile.xsd");

      var failed_dependencies = Dependencies.Failed (makefile, m_package_list);
            
      var languages = new Languages ();

      if (failed_dependencies.Count != 0) {
        Console.WriteLine ("WARNING: Skip makefile of {0} due to not satisfied dependency '{1}'", input_path, String.Join (",", failed_dependencies.ToArray ()));
        // On crée un fichier makefile vide 
        using (StreamWriter writer = new StreamWriter(output_path.Replace("\\", "/") + "/makefile.cmake")) {
          writer.Write ("#\n# WARNING: Skip subdirectory {0} due to not satisfied dependency '{1}'\n#\n", input_path, String.Join (",", failed_dependencies.ToArray ()));
        }
        return languages;
      }

      var packages = makefile.neededpackages;

      List<string> all_compile_flags_buffer = new List<string> ();
      List<string> all_includes_buffer = new List<string> ();
      if (packages != null) {
        foreach (var package in packages) {
          bool is_optional = (package.optionalSpecified) ? package.optional : false;
          PackageFile package_file = m_package_list.GetPackageFile (package.name, is_optional);
          if (package_file != null) {
            CompilerOptions options = package_file.CompileFlags ();
            all_compile_flags_buffer.Add(options.Flags);
            foreach (String include in options.Includes) {
              all_includes_buffer.Add (String.Format ("\"{0}\"", include));
            }
          }
        }
      }
      String all_compile_flags = String.Join (" ", all_compile_flags_buffer.ToArray ());

      if (makefile.files != null) {
        foreach (var files in makefile.files) {
          var lang = files.language;
          languages.Use(lang);
          if(lang.isAxl() || lang.isModule() || lang.isService()) {
            if(files.file != null) {
              foreach (var file in files.file) {
                m_axl_files.Add (Path.Combine(input_path,file.Value + ".axl"));
              }
            }
          }
        }
      }

      if (makefile.directories != null) {
        foreach (var directory in makefile.directories) {
          languages.Use(ParseDirectory<T> (directory, library_name, input_path, output_path, current_depth));
        }
      }

      using (StreamWriter writer = new StreamWriter(output_path + "/makefile.cmake")) {

        var cmake = (T)Activator.CreateInstance(typeof(T), 
                                                makefile, 
                                                library_name,
                                                output_path, 
                                                input_path, all_compile_flags);

        var method = cmake.GetType().GetMethod("TransformText");

        writer.Write (method.Invoke(cmake, null));
      }

      return languages;
    }
  }
}
