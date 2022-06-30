using System;
using System.Collections.Generic;
using System.Text;

namespace XmlToCMake
{
    public class CompilerOptions
    {
        public CompilerOptions(List<String> includes, List<String> defines, String option_mark)
        {
            this.Includes = includes;
            this.Defines = defines;

            StringBuilder flags_buffer = new StringBuilder();
            Includes.ForEach(delegate(string s) { flags_buffer.AppendFormat("{2}I{0}\"{1}{0}\" ", @"\", s, option_mark); });
            Defines.ForEach(delegate(string s) { flags_buffer.AppendFormat("{2}D{0}\"{1}{0}\" ", @"\", s, option_mark); });
            this.Flags = flags_buffer.ToString();
        }
        public List<string> Includes { get; private set; }
        public List<string> Defines { get; private set; }
        public String Flags { get; private set; }
    };

  public sealed class PackageFile
  {
    public PackageFile(string name, List<String> includes, List<String> libraries, List<String> library_paths, List<String> flags)
    {
      this.Name = name;
      this.Includes = includes;

      // new version which skips optimized - debug sequence (which produces warning in CMake)
      this.Libraries = new List<String>();
      String last = null;
      foreach(String s in libraries)
      {
        if (last == null) { last = s; }
        else if (last == "optimized" && s == "debug") { last = null; } 
        else {
          this.Libraries.Add(last);
          last = s;
        }
      }
      if (last != null) this.Libraries.Add(last);
      
      this.LibraryPaths = library_paths;
      this.Flags = flags;
    }

    public string Name { get; private set; }
    public List<string> Libraries { get; private set; }
    public List<string> LibraryPaths { get; private set; }
    private List<string> Flags;
    private List<string> Includes;

    public CompilerOptions CompileFlags() 
    {
        List<String> define_list = new List<string>();
        List<String> include_list = new List<string>();
      
        this.Includes.ForEach(delegate(string s) {
        if (s.StartsWith("-I") || s.StartsWith("/I"))
        {
          include_list.Add(s.Substring(2, s.Length - 2));
          // Console.WriteLine("Include : {0}", s.Substring(2, s.Length - 2));
        }
        else
        {
          include_list.Add(s);
          // Console.WriteLine("Include : {0}", s);
        }
      } );
      
      this.Flags.ForEach(delegate(string s) {
        if (s.StartsWith("-D") || s.StartsWith("/D"))
        {
          define_list.Add(s.Substring(2, s.Length - 2));
          // Console.WriteLine("Compile flags : {0}", s.Substring(2, s.Length - 2));
        }
        else if (s.StartsWith("-I") || s.StartsWith("/I"))
        {
            include_list.Add(s.Substring(2, s.Length - 2));
            // Console.WriteLine("Include : {0}", s.Substring(2, s.Length - 2));
        }
        else if (s.StartsWith("-") || s.StartsWith("/"))
        {
            Console.Error.WriteLine("ERROR: Unknown flag '{0}' found in package '{1}'\n", s, Name);
            Environment.Exit(-1);
        }
        else 
        {
            define_list.Add(s);
        }
      });

      switch(Environment.OSVersion.Platform)
      {
        case System.PlatformID.Unix:
            return new CompilerOptions(include_list, define_list, "-");
        case System.PlatformID.Win32NT:
        case System.PlatformID.Win32Windows:
            return new CompilerOptions(include_list, define_list, "/");
        default:
            Console.Error.WriteLine("ERROR: Unknown plaform '{0}' : cannot generate platform options\n", Environment.OSVersion.Platform);
            Environment.Exit(-1);
            return null;
      }
    }
  }
}
