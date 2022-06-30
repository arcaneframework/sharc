using System;
using System.Linq;
using System.Collections.Generic;

namespace XmlToCMake
{
  public partial class CMakeListf90
  {
    public Makefile Makefile { get; private set; }

    public String OutputPath { get; private set; }

    public String InputPath { get; private set; }

    public String CompilationFlags { get; private set; }

    public String LibraryName { get; private set; }

    public CMakeListf90 ()
    {
    }

    public CMakeListf90 (Makefile makefile, 
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

    public IEnumerable<String> F90Files {
      get {
        if(Makefile.files == null) {
          return new List<String> ();
        } else {
          return Makefile.files.Where(f => f.language.isF90())
                               .SelectMany(f => f.file)
                               .Select(f => f.Value);
        }
      }
    }

    public IEnumerable<String> DirectoryPaths {
      get {
        if(Makefile.directories == null) {
          return new List<String> ();
        } else {
          return Makefile.directories.Select(f => f.Value);
        }
      }
    }
  }
}

