using System;
using System.Collections.Generic;
using System.Xml;
using System.IO;

namespace XmlToCMake
{
  public sealed class PackageList
  {
    private readonly XmlDocument m_document = null;
    private readonly XmlDocument m_document2 = null;
    private List<String> m_packages_enabled = new List<String> ();
    private Dictionary<String,PackageFile> m_packages = new Dictionary<String,PackageFile> ();

    public bool Verbose { set; get; }

    public PackageList (XmlDocument packages_file_document, XmlDocument arcane_packages_file_document)
    {
      m_document = packages_file_document;
      m_document2 = arcane_packages_file_document;
    }
        
    private PackageFile EnablePackage (String package)
    {
      if (m_packages_enabled.Contains (package) == true) 
        return m_packages [package];

      PackageFile package_file = null;
      
      if (m_packages.ContainsKey (package) == true)
        package_file = m_packages [package];
      else
        package_file = RegisterPackage (package);

      if (package_file != null)
        m_packages_enabled.Add (package);
      
      return package_file;
    }
    
    public bool IsAvailablePackage (String package)
    {
      XmlNode node = m_document.SelectSingleNode (String.Format ("//packages/package[(@name='{0}' and @available='true')]", package));
      XmlNode node2 = m_document2.SelectSingleNode (String.Format ("//packages/package[(@name='{0}' and @available='true')]", package));
      return (node != null || node2 != null);
    }
      
    public PackageFile RegisterPackage (String package)
    {
      XmlNode node = m_document.SelectSingleNode (String.Format ("//packages/package[(@name='{0}' and @available='true')]", package));
      XmlNode node2 = m_document2.SelectSingleNode (String.Format ("//packages/package[(@name='{0}' and @available='true')]", package));

      XmlElement element = node as XmlElement;
      if (node == null) {
        if (node2 != null) 
          element = node2 as XmlElement;
        else
          return null;
      } else {
        if (node2 != null) {
          Console.Error.WriteLine ("WARNING: Conflicting configuration between Arcane and ArcGeoSim for '" + package + "'"); // ajouté l'emplacement de l'erreur
          Environment.Exit (-1);
        }
        // else // already done with default initialization from node
      }
      
      List<String> includes = new List<String> ();
      List<String> libraries = new List<String> ();
      List<String> library_paths = new List<String> ();
      List<String> flags = new List<String> ();

      foreach (XmlNode node_ in element) {
        if (node_.Name == "include") {
          includes.Add (node_.InnerText);
        }
        if (node_.Name == "lib-name") {
          libraries.Add (node_.InnerText);
        }
        if (node_.Name == "flags") {
          flags.Add (node_.InnerText);
        }
        if (node_.Name == "lib-path") {
          library_paths.Add (node_.InnerText);
        }
      }

      PackageFile package_file = new PackageFile (package, includes, libraries, library_paths, flags);
      m_packages [package] = package_file;
      return package_file;
    }

    public PackageFile GetPackageFile (String package, bool is_optional)
    {
      String name = convertTagToPkglistName (package);

      PackageFile package_file = EnablePackage (name);
      if (package_file == null) {
        if (!is_optional) {
          Console.Error.WriteLine ("ERROR: Unknown or not configured package '" + name + "' : check your configuration\n"); // ajouté l'emplacement de l'erreur
          Environment.Exit (-1);
        } else {
          if (Verbose)
            Console.WriteLine ("WARNING: optional package '" + name + "' disabled");
        } 
      }
      return package_file;
    }
    
    public ICollection<PackageFile> GetAllPackageFile ()
    {
      return m_packages.Values;
    }

    public IEnumerable<PackageFile> GetEnabledPackageFile() {
      foreach (var package_name in m_packages_enabled) {
        yield return m_packages [package_name];
      }
    }

    public static string convertTagToPkglistName (string tag)
    {
      string tmp = tag.ToLower();
      tmp = tmp.Replace ('-', '_');
      tmp = tmp.Replace ('+', 'x');
      return tmp;
    }
  }
}