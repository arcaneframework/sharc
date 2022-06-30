using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

namespace XmlToCMake
{
  public partial class CMakeListLibrariesXml
  {
    public String BinaryDir { get; private set; } 

    public String GumpPath { get; private set; } 

    public IEnumerable<String> Includes { get; private set; }

    public IEnumerable<Tuple<Libraries,Library>> F90Libraries { get; private set; }

    public Application Application { get; private set; }

    public CMakeListLibrariesXml (String binary_dir, 
                                  List<String> includes,
                                  Application application,
                                  List<Tuple<Libraries,Library>> f90_libraries)
    {
      BinaryDir = binary_dir;

      var incls = new List<String> ();
      foreach(var i in includes) {
        incls.Add (i.Replace("\\", "/"));
      }
      Includes = incls;

      Application = application;

      F90Libraries = f90_libraries;

      GumpPath = Path.Combine(BinaryDir, Path.Combine("generated", "gump")).Replace("\\", "/");
    }

    public IEnumerable<Libraries> Libraries { 
      get {
        return new List<Libraries> (Application.libraries);
      }
    }

    public IEnumerable<Libraries> DependencyLibraries(Libraries libraries) { 
      if(libraries.librarydependency == null)
        return new List<Libraries> ();
      var libs = new List<Libraries> ();
      var all = new List<Libraries> (Application.libraries);
      foreach (var l in libraries.librarydependency) {
        if(l.name == libraries.name) {
          Console.WriteLine ("Library {0} self dependency skipped", libraries.name);
        }
        var lib = all.SingleOrDefault(p => p.name == l.name);
        if(lib == null) {
          Console.WriteLine ("Unknonw library {0} for dependency", l.name);
        } else {
          libs.Add (lib);
        }
      }
      return libs;
    }

    public String LibraryPath(Libraries libraries, Library library) 
    {
      var n = libraries.Name.CamelCaseName();
      var ln = n + "_" + library.name.Replace("-", "_") + ".dir";
      var path = Path.Combine(BinaryDir, Path.Combine("CMakeFiles", ln));
      return path.Replace("\\", "/");
    }

    public String LibraryF90Path(Libraries libraries, Library library) 
    {
      var n = libraries.Name.CamelCaseName();
      var ln = n + "_" + library.name.Replace("-", "_") + "_f90" + ".dir";
      var path = Path.Combine(BinaryDir, Path.Combine("CMakeFiles", ln));
      return path.Replace("\\", "/");
    }
  }
}

