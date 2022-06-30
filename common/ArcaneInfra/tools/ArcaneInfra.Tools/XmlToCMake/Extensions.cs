using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public static class StringExtension
{
    public static string CamelCaseName(this string name)
    {
        char[] std_name = name.ToCharArray();
        bool next_is_upper = true;
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < std_name.Length; ++i)
        {
            char ch = std_name[i];
            if (ch == '-')
            {
                next_is_upper = true;
            }
            else if (next_is_upper)
            {
                sb.Append(Char.ToUpper(ch));
                next_is_upper = false;
            }
            else
            {
                sb.Append(ch);
            }
        }
        return sb.ToString();
    }
}

static class LanguageExtensions {

  public static XmlToCMake.Language Axl(this XmlToCMake.Language l) {
    return XmlToCMake.Language.axl;
  }

  public static XmlToCMake.Language Cpp(this XmlToCMake.Language l) {
    return XmlToCMake.Language.c;
  }
  
  public static XmlToCMake.Language CppHeader(this XmlToCMake.Language l) {
    return XmlToCMake.Language.cheader;
  }
  
  public static XmlToCMake.Language C(this XmlToCMake.Language l) {
    return XmlToCMake.Language.c1;
  }
  
  public static XmlToCMake.Language CHeader(this XmlToCMake.Language l) {
    return XmlToCMake.Language.cheader1;
  }
  
  public static XmlToCMake.Language CSharp(this XmlToCMake.Language l) {
    return XmlToCMake.Language.c2;
  }
  
  public static XmlToCMake.Language Module(this XmlToCMake.Language l) {
    return XmlToCMake.Language.module;
  }
  
  public static XmlToCMake.Language Service(this XmlToCMake.Language l) {
    return XmlToCMake.Language.service;
  }

  public static XmlToCMake.Language F90(this XmlToCMake.Language l) {
    return XmlToCMake.Language.f90;
  }

  public static bool isAxl(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.axl;
  }

  public static bool isCpp(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.c;
  }

  public static bool isCppHeader(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.cheader;
  }

  public static bool isC(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.c1;
  }

  public static bool isCHeader(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.cheader1;
  }

  public static bool isCSharp(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.c2;
  }

  public static bool isModule(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.module;
  }
  
  public static bool isService(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.service;
  }

  public static bool isF90(this XmlToCMake.Language l) {
    return l == XmlToCMake.Language.f90;
  }
}

static class FilesExtensions {
  
  public static IEnumerable<String> Files (this XmlToCMake.Files[] files, XmlToCMake.Language lang) 
  {
    if(files == null) {
      return new List<String> ();
    }
    return files.Where(f => f.file != null && f.language == lang)
                .SelectMany (f => f.file)
                .Where (f => f.Value != null)
                .Select (f => f.Value);
  }

  public static IEnumerable<String> Headers (this XmlToCMake.Files[] files, XmlToCMake.Language lang) 
  {
    if(files == null) {
      return new List<String> ();
    }
    return files.Where(f => f.file != null && f.language == lang)
                .SelectMany (f => f.file)
                .Where (f => f.Value != null)
                .Where (f => f.headerSpecified == true )
                .Where (f => f.header == true)
                .Select (f => f.Value);
  }
}
