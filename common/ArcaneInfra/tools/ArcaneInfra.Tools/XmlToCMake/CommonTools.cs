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
    public static bool isWindows()
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
    #endregion
  }
}

