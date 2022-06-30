using System;
using System.Collections.Generic;

namespace XmlToCMake
{
  public class Dependencies
  {
    public Dependencies ()
    {
    }

    public static List<string> Failed (Directory directory, PackageList package_list)
    {
      string[] dependencies = { };
      
      var failed_dependencies = new List<string> ();
      
      if (directory.depend == null) {
        return failed_dependencies;
      }
      
      if (directory.depend != null) {
        String depend_attribute = directory.depend;
        dependencies = depend_attribute.Split (new char[] { ' ', ',' }, StringSplitOptions.RemoveEmptyEntries);
      }
      
      foreach (string dep in dependencies) {
        string true_dep = PackageList.convertTagToPkglistName(dep);
        bool test_dep = true;
        while (true_dep[0] == '!') {
          test_dep = !test_dep;
          true_dep = true_dep.Substring (1);
        }
        if (package_list.IsAvailablePackage (true_dep) != test_dep) {
          failed_dependencies.Add (dep);
        }
      }
      
      return failed_dependencies;
    }
    
    public static List<string> Failed (Makefile makefile, PackageList package_list)
    {
      var failed_dependencies = new List<string> ();
      
      string[] dependencies = { };
      
      if (makefile.depend != null) {
        String depend_attribute = makefile.depend;
        dependencies = depend_attribute.Split (new char[] { ' ', ',' }, StringSplitOptions.RemoveEmptyEntries);
      }
      
      foreach (string dep in dependencies) {
        string true_dep = dep;
        bool test_dep = true;
        while (true_dep[0] == '!') {
          test_dep = !test_dep;
          true_dep = true_dep.Substring (1);
        }
        if (package_list.IsAvailablePackage (true_dep) != test_dep) {
          failed_dependencies.Add (dep);
        }
      }
      
      return failed_dependencies;
    }
  }
}

