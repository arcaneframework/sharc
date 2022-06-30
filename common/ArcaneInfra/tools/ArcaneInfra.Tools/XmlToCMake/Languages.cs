using System;
using System.Collections.Generic;

namespace XmlToCMake
{
  public class Languages
  {
    private HashSet<Language> used = new HashSet<Language> ();

    public void Use(Language lang)
    {
      used.Add (lang);
    }

    public void Use(Languages langs)
    {
      used.UnionWith (langs.used);
    }

    public bool Contains(Language lang) {
      return used.Contains (lang);
    }
  }
}
