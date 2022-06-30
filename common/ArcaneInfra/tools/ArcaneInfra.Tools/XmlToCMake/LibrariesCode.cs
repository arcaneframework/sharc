using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace XmlToCMake
{
  public partial class Libraries
  {
    public Libraries ()
    {
    }

    public void check(bool verbose)
    {
      // La présence de la balise include permet de définir un chemin d'include explicite
      if (includepath == null) {
        var directories = path.Split ('/').ToList (); // on n'utilise pas Path.DirectorySeparatorChar car on reste dans le monde CMake
        if (directories.Contains ("src")) {
          while (directories.Last() != "src")
            directories.RemoveAt (directories.Count - 1);
          var p = String.Join ("/", directories.ToArray ());
          if (verbose)
            Console.WriteLine ("Using implicit include path '{0}' for library '{1}'", p, name);
        } else {
          if (verbose)
            Console.WriteLine ("Using implicit include path '{0}' for library '{1}'\nYou may specify an explicit include directory using include-path attribute", path, name);
        }
      }
    }

    public String Name { 
      get { 
        return name; 
      } 
    }

    public String IncludePath { 
      get { 
        if (includepath != null) {
          return includepath;
        }
        var directories = path.Split ('/').ToList (); 
        if (directories.Contains ("src")) {
          while (directories.Last() != "src") {
            directories.RemoveAt (directories.Count - 1);
          }
          return String.Join ("/", directories.ToArray ());
        } else {
          return path;
        }
      }
    }
  }
}
