using System;
using System.Linq;
using System.Collections.Generic;

namespace XmlToCMake
{
  public partial class CMakeList
  {
    public Makefile Makefile { get; private set; }
    
    private String OutputPath { get; set; }
    
    public String InputPath { get; private set; }
    
    public String CompilationFlags { get; private set; }

    public String LibraryName { get; private set; }

    public CMakeList ()
    {
    }

    public CMakeList (Makefile makefile,
                      String library_name,
                      String output,
                      String input,
                      String flags)
    {
      Makefile = makefile;
      OutputPath = output.Replace ("\\", "/");
      InputPath = input.Replace ("\\", "/");
      CompilationFlags = flags;
      LibraryName = library_name;
    }
 
    public String Mono {
      get {
        if(ArcaneInfra.Common.CommonTools.isWindows()) {
          return "";
        } else {
          return "mono";
        }
      }
    }

    public IEnumerable<String> DirectoryPaths {
      get {
        if(Makefile.directories == null) {
          return new List<String> ();
        }
        return Makefile.directories.Select(f => OutputPath + "/" + f.Value);
      }
    }

    public IEnumerable<String> ModuleFiles {
      get {
        return Makefile.files.Files (default(Language).Module ());
      }
    }

    public IEnumerable<String> ModuleHeaders {
      get {
        return Makefile.files.Headers (default(Language).Module ());
      }
    }

    public IEnumerable<String> ServiceFiles {
      get {
        return Makefile.files.Files (default(Language).Service ());
      }
    }

    public IEnumerable<String> ServiceHeaders {
      get {
        var headers = Makefile.files.Headers (default(Language).Service ());
        foreach(var h in headers) {
          Console.WriteLine("service header = {0}", h);
        }
        return Makefile.files.Headers (default(Language).Service ());
      }
    }

    public IEnumerable<String> CppFiles {
      get {
        return Makefile.files.Files (default(Language).Cpp ());
      }
    }

    public IEnumerable<String> CppHeaders {
      get {
        var headers = new List<String> ();
        headers.AddRange(Makefile.files.Files (default(Language).CppHeader ()));
        headers.AddRange(Makefile.files.Headers (default(Language).Cpp ()));
        return headers;
      }
    }

    public IEnumerable<String> CFiles {
      get {
        return Makefile.files.Files (default(Language).C ());
      }
    }

    public IEnumerable<String> CHeaders {
      get {
        if(Makefile.files == null) {
          return new List<String> ();
        }
        var headers = new List<String> ();
        headers.AddRange(Makefile.files.Files (default(Language).CHeader ()));
        headers.AddRange(Makefile.files.Headers (default(Language).C ()));
        return headers;
      }
    }

    public IEnumerable<String> CSharpFiles {
      get {
        return Makefile.files.Files (default(Language).CSharp ());
      }
    }

    public IEnumerable<String> AxlFiles {
      get {
        var axls = new List<string> ();
        axls.AddRange (Makefile.files.Files (default(Language).Axl ()));
        axls.AddRange (ModuleFiles);
        axls.AddRange (ServiceFiles);
        return axls;
      }
    }
  }
}

