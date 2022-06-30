using System;
using System.Collections.Generic;
using System.IO;

namespace ConfigFileCreator
{
  public class ArcaneDirectoryAnalyzer
  {
    public ArcaneDirectoryAnalyzer(string path)
    {
      if(path == null)
        throw new NullReferenceException("path");
      
      var infos = new DirectoryInfo(path);
     
      this.Directories = new List<string>();
      this.HeaderFiles = new List<string>();
      this.AxlFiles = new List<string>();
      this.CppFiles = new List<XmlFileWriter.InstructionFile>();
      this.ModuleFiles = new List<XmlFileWriter.InstructionFile>();
      this.ServiceFiles = new List<XmlFileWriter.InstructionFile>();

      foreach(var info in infos.GetDirectories())
        if (!info.Name.StartsWith(".")) 
          this.Directories.Add(info.Name);

      List<string> AllCppFiles = new List<string>();
      foreach(var info in infos.GetFiles("*.cc")) 
        AllCppFiles.Add(info.Name.Replace(info.Extension,""));
      foreach(var info in infos.GetFiles("*.h")) 
        this.HeaderFiles.Add(info.Name.Replace(info.Extension,""));
      foreach(var info in infos.GetFiles("*.axl")) 
        this.AxlFiles.Add(info.Name.Replace(info.Extension,""));

      foreach(var s in AllCppFiles)
      {
        if (EndsWithModule(s)) 
        {
          bool HasHeader = (this.HeaderFiles.Contains(s));
          string ModuleName = s.Replace("Module","");
          this.ModuleFiles.Add(new XmlFileWriter.InstructionFile(ModuleName, HasHeader));
          if (HasHeader) this.HeaderFiles.Remove(s);
          this.AxlFiles.Remove(ModuleName);
        } 
        else if (EndsWithService(s))
        {
          bool HasHeader = (this.HeaderFiles.Contains(s));
          string ServiceName = s.Replace("Service","");
          this.ServiceFiles.Add(new XmlFileWriter.InstructionFile(ServiceName, HasHeader));
          if (HasHeader) this.HeaderFiles.Remove(s);
          this.AxlFiles.Remove(ServiceName);
        } 
        else
        {
          bool HasHeader = (this.HeaderFiles.Contains(s));
          this.CppFiles.Add(new XmlFileWriter.InstructionFile(s, HasHeader));
          if (HasHeader) this.HeaderFiles.Remove(s);
        }
      }
    }
    
    private static bool EndsWithModule(string s)
    {
      return ((s.Length > 5) && (s.Substring(s.Length - 6) == "Module"));
    }
    
    private static bool EndsWithService(string s)
    {
      return ((s.Length > 6) && (s.Substring(s.Length - 7) == "Service"));
    }

    public List<string> Directories { get; private set; }
    public List<string> HeaderFiles { get; private set; }
    public List<string> AxlFiles { get; private set; }
    public List<XmlFileWriter.InstructionFile> CppFiles { get; private set; }
    public List<XmlFileWriter.InstructionFile> ModuleFiles { get; private set; }
    public List<XmlFileWriter.InstructionFile> ServiceFiles { get; private set; }
  }
}
