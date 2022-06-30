using System;
using System.Linq;
using System.Collections.Generic;

namespace XmlToCMake
{
  public partial class CMakeListLibrary
  {
    public Library Library { get; private set; }

    public PackageList Packages { get; private set; }

    public String LibraryName { get; private set; }
   
    private String OutputPath { get; set; }

    public CMakeListLibrary (Library library,
                             PackageList packages,
                             String library_name,
                             String output_path)
    {
      Library = library;
      Packages = packages;
      LibraryName = library_name;
      OutputPath = output_path.Replace ("\\", "/");
    }

    public IEnumerable<String> DirectoryPaths {
      get {
        return Library.directory.Select(p => OutputPath + "/" + p.Value);
      }
    }

    public IEnumerable<String> Libraries {
      get {
        var libs = new List<String> ();
        foreach (var package in Packages.GetAllPackageFile()) {
          libs.AddRange(package.Libraries);
        }
        return libs;
      }
    }
  }
}

