using System;
using System.IO;
using System.Collections.Generic;
using System.Reflection;

namespace ApplicationLauncher
{
  static class AssemblyResolver 
  {
    public static void Initialize() {
      Logger.log (Logger.Channel.DevDebug, "Initializing AssemblyResolver");
      Assembly thisAssembly = Assembly.GetExecutingAssembly();
      String thisPath = thisAssembly.Location;
      String directory = Path.GetDirectoryName(thisPath);

      AddDirectory (directory);

      AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler( ResolveEventHandler );
    }

    static public void AddDirectory(string directory) {
      if(!m_directories.Contains(directory))
        m_directories.Add(directory);
    }
    
    private static Assembly ResolveEventHandler( Object sender, ResolveEventArgs args ){
      Assembly newAssembly = null;
      String newAssemblyName = null;

      String[] onlyName = args.Name.Split(',');
      if (onlyName.Length == 0)
        return newAssembly;
      else
        newAssemblyName = onlyName [0];

      try {
        Assembly thisAssembly = Assembly.GetExecutingAssembly();
        String thisPath = thisAssembly.Location;        

        foreach(String directory in m_directories) 
        {
          String pathToManagedAssembly = Path.Combine(directory, onlyName[0] + ".dll");
          if(File.Exists(pathToManagedAssembly)){
            newAssembly = Assembly.LoadFile(pathToManagedAssembly);
            break; // or continue if try a another path in m_directories list
          }
        }

        if (newAssembly != null)
          Logger.log (Logger.Channel.DevDebug, "Resolving assembly '{0}' with '{1}'", newAssemblyName, newAssembly.Location);
        else
          Logger.log (Logger.Channel.DevDebug, "Resolving assembly '{0}' failed", newAssemblyName);
      } catch (Exception e) {
        Logger.log(Logger.Channel.DevDebug, "Error while resolving assembly '{0}' : {1}", newAssemblyName, e.Message);        
      }
      return newAssembly;
    }

    private static List<String> m_directories = new List<String>();
  }
}

