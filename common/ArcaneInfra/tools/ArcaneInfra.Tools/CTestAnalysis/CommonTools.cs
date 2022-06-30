using System;
using System.Text;

namespace ArcaneInfra.Common
{
  public class CommonTools
  {
    private char m_variable_separator;

    public CommonTools ()
    {
      if (isWindows ())
        m_variable_separator = ';';
      else
        m_variable_separator = ':';
    }

    #region EXCEPTION
    [Serializable]
    public class CommonToolsException : Exception
    {
      public CommonToolsException()
      {
        ;
      }
      
      public CommonToolsException(string message)
        : base(message)
      {
        ;
      }
    }
    #endregion

    #region TOOLS
    public bool isWindows()
    {
      switch (Environment.OSVersion.Platform) 
      {
      case PlatformID.Win32NT:
      case PlatformID.Win32S:
      case PlatformID.Win32Windows:
      case PlatformID.WinCE:
        return true;
      case PlatformID.MacOSX:
      case PlatformID.Unix:
        return false;
      default:
        throw new CommonToolsException(String.Format("Internal Error: Unknown plaform '{0}'\n", Environment.OSVersion.Platform));
      }
    }

    public char VariableSeparator {
      get { return m_variable_separator; }
    }

    /**
       * Transforme la chaine de caractères "name" en un nom de fonction respectant
       * la typographie Arcane. Par exemple, "item-kind" devient "itemKind".
       * @param name le nom de la chaine d'origine.
       * @return la chaine transformée.
       */
    public string ToCamelCaseName (string name)
    {
      char[] std_name = name.ToCharArray ();
      bool next_is_upper = true;
      StringBuilder sb = new StringBuilder ();
      for (int i = 0; i < std_name.Length; ++i) {
        char ch = std_name [i];
        if (ch == '-') {
          next_is_upper = true;
        } else if (next_is_upper) {
          sb.Append (Char.ToUpper (ch));
          next_is_upper = false; 
        } else {
          sb.Append (ch);
        }
      }
      return sb.ToString ();
    }
    #endregion
  }
}

