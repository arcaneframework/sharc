using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.IO;

namespace ArcaneInfra.Configure
{
  class PkglistParser
  {
    List<string> m_dll_dirs;
    public IList<string> Directories { get { return m_dll_dirs; } }

    public PkglistParser()
    {
      m_dll_dirs = new List<string>();
    }

    // Sous Windows, parser le fichier 'lib/pkglist.xml' et pour chaque bibliotheque, changer les
    // chemins qui contiennent 'lib' en 'bin' et si le chemin resultat existe
    // l'ajouter automatiquement au PATH lors de l'execution.
    // Ce mecanisme permet d'ajouter automatiquement les DLL dans le path pour l'execution des tests.
    // Pour certains packages (par exemple xerces), les DLL sont dans 'lib'. Il faut donc aussi
    // ajouter ce repertoire.
    public void Parse(string path)
    {
      XDocument doc = XDocument.Load(path);
      XElement root = doc.Root;
	  if (GlobalContext.Instance.Verbose)
        Console.WriteLine("DOC={0}", root);
      foreach (XElement pkg in root.Elements("package")) {
        bool is_available = (bool)pkg.Attribute("available");
        string name = (string)pkg.Attribute("name");
        // Ne traite pas les packages indisponibles
        if (!is_available)
          continue;
		if (GlobalContext.Instance.Verbose)
          Console.WriteLine("NAME={0} {1}", name, is_available);
        foreach (XElement lib_name in pkg.Elements("lib-name")) {
  		  if (GlobalContext.Instance.Verbose)
            Console.WriteLine("LIB={0}", lib_name.Value);
          _AddDir(Path.GetDirectoryName(lib_name.Value));
          _ParseLib(lib_name.Value);
        }
      }

	  if (GlobalContext.Instance.Verbose)
        foreach (string s in m_dll_dirs)
          Console.WriteLine("DIR={0}", s);
    }
    void _AddDir(string name)
    {
      if (m_dll_dirs.Contains(name))
        return;
      m_dll_dirs.Add(name);
    }
    void _ParseLib(string lib_full_name)
    {
      string name = Path.GetFileName(lib_full_name);
      string dll_name = Path.GetFileNameWithoutExtension(name) + ".dll";
      if (GlobalContext.Instance.Verbose)
        Console.WriteLine("DLL_NAME={0}", dll_name);
      string base_name = lib_full_name;
      string rel_name = String.Empty;
      while (!String.IsNullOrEmpty(base_name = Path.GetDirectoryName(base_name))) {
        string that_dir = Path.GetFileName(base_name);
        if (that_dir.ToLower().Equals("lib")) {
          string dll_dir = Path.Combine(Path.GetDirectoryName(base_name), Path.Combine("bin", rel_name));
		  if (GlobalContext.Instance.Verbose)
            Console.WriteLine("IS LIB dll={0}!!!",dll_dir);
          _AddDir(dll_dir);
          break;
        }
        // Console.WriteLine("T={0} {1} {2}", name, base_name, rel_name);
        rel_name = Path.Combine(that_dir,rel_name);
      }
    }
  }
}