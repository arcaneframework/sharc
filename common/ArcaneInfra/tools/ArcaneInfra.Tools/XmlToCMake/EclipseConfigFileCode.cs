using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace XmlToCMake
{
  public partial class EclipseConfigFile
  {
    // Regex for macro decomposition
    private static Regex macro_regex = new Regex (@"^(?<name>\w+)(=(?<value>.*))?$");

    public IEnumerable<String> IncludePathList { 
      get { 
        foreach (var package in m_package_files) {
          CompilerOptions options = package.CompileFlags ();
          foreach (String include in options.Includes) {
            yield return include;
          }
        }
      }
    }
    public IEnumerable<KeyValuePair<String, String>> MacroList { 
      get { 
        foreach (var package in m_package_files) {
          CompilerOptions options = package.CompileFlags ();
          foreach (String macro in options.Defines) {
            Match m = macro_regex.Match (macro);
            if (m.Success) {
              yield return new KeyValuePair<String, String> (m.Groups ["name"].Value, m.Groups ["value"].Value);
            } else {
              Console.Error.WriteLine ("Inconsistent macro definition '{0}' from package '{1}' will not be exported", macro, package.Name);
            }
          }
        }
      }
    }

    public EclipseConfigFile(IEnumerable<PackageFile> package_files)
    {
      m_package_files = package_files;
    }

    private IEnumerable<PackageFile> m_package_files;
  }
}