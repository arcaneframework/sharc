using System;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml.XPath;

namespace ArcaneInfra.Configure
{
  class VendorChecker
  {
    public VendorChecker ()
    {
      ;
    }

    public void Check (string path)
    {
      //* 1 : vérifier dans le fichier pkglist.xml le présence de FlexNet et l'absence de parmetis en direct ou indirect
      bool FlexNetFound = false;
      bool ParMetisFound = false;

      Regex flexNetRegex = new Regex (@"(?<path>(.*/)?)FlexlmAPI(?<suffix>(\.lib)?)$");
      Regex metisRegex = new Regex (@"^(?<path>(.*/))(?<name>(lib?)(parmetis|metis).(dll|lib|so))$", RegexOptions.IgnoreCase);

      XDocument doc = XDocument.Load (path);
      XElement root = doc.Root;

      // Vérification que FlexNet est bien porté par Arcane
      XElement arcanePackageRoot = root.XPathSelectElement ("/packages/package[@name='arcane' and @available='true']");
      if (arcanePackageRoot == null) {
        Console.WriteLine ("WARNING: arcane package not detected; cannot check FlexNet protection");
      } else {
        foreach (XElement pkg in arcanePackageRoot.Elements("lib-name")) {
          string lib_name = (string)pkg.Value;
          Match m = flexNetRegex.Match (lib_name);
          if (m.Success) {
            FlexNetFound = true;
            // Console.WriteLine("FlexNet found !!");
          }
        }
      }

      /* may be moved into XmlToCMake generator ? */
      // Vérification que ParMetis n'est dans aucune dépendance
      foreach (XElement pkg in root.Elements("package")) {
        bool is_available = (bool)pkg.Attribute ("available");
        string package_name = (string)pkg.Attribute ("name");
        // Ne traite pas les packages indisponibles
        if (!is_available)
          continue;

        if (GlobalContext.Instance.Verbose)
          Console.WriteLine ("Check library {0} {1}", package_name, is_available);
        foreach (XElement lib in pkg.Elements("lib-name")) {
          string lib_name = lib.Value;
          Match m = metisRegex.Match (lib_name);
          if (m.Success) {
            ParMetisFound = true;
            Console.Error.WriteLine ("Prohibited Metis dependendy found in {0} : {1} !!", package_name, lib_name);
          }
        }
      }

      String error_message = "";

      if (!FlexNetFound) {
        error_message += "\n\tVendor compilation required to use FlexNet protected Arcane library";
      }

      if (ParMetisFound) {
        error_message += "\n\tVendor compilation cannot embed Metis library";
      }

      if (error_message.Length > 0)
        throw new Exception (error_message);
    }
  }
}
