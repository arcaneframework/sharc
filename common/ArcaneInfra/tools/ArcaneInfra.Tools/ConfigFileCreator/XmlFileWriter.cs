using System;
using System.Collections.Generic;
using System.Xml;
using System.IO;

namespace ConfigFileCreator
{
  public class XmlFileWriter
  {
    public class InstructionFile
    {
      public InstructionFile(string RequestedFilename, bool RequestedHasHeader)
      {
        Filename = RequestedFilename;
        HasHeader = RequestedHasHeader;
      }
      public InstructionFile(string RequestedFilename)
      {
        Filename = RequestedFilename;
        HasHeader = true;
      }
      public string Filename { get; private set; }
      public bool HasHeader { get; private set; }
    }
    
    Dictionary<string, HashSet<InstructionFile>> m_files_per_label = new Dictionary<string, HashSet<InstructionFile>>();
    
    public XmlFileWriter() {}
    
    public void Add(string label, IList<string> files)
    {
      if(m_files_per_label.ContainsKey(label))
        throw new Exception(String.Format("label '{0}' already defined", label));

      if(files.Count == 0) return;
      
      m_files_per_label[label] = new HashSet<InstructionFile>();
      
      foreach(var file in files)
        m_files_per_label[label].Add(new InstructionFile(file));
    }

    public void Add(string label, IList<InstructionFile> files)
    {
      if(m_files_per_label.ContainsKey(label))
        throw new Exception(String.Format("label '{0}' already defined", label));

      if(files.Count == 0) return;
      
      m_files_per_label[label] = new HashSet<InstructionFile>();
      
      foreach(var file in files)
        m_files_per_label[label].Add(file);
    }
    
    public void Write(XmlWriter writer)
    { 
      if(writer == null)
        throw new NullReferenceException("writer");

      foreach(KeyValuePair<string, HashSet<InstructionFile>> vp in m_files_per_label)
      {
        if(vp.Value.Count > 0)
        {
          writer.WriteStartElement("files");
          writer.WriteStartAttribute("language");
          writer.WriteString(vp.Key);
          writer.WriteEndAttribute();
          foreach(var file in vp.Value)
          {
            writer.WriteStartElement("file");
            if (!file.HasHeader)
            {
              writer.WriteStartAttribute("header");
              writer.WriteString("false");
              writer.WriteEndAttribute();
            }
            writer.WriteString(file.Filename);
            writer.WriteEndElement();
          }
          writer.WriteEndElement();
        }
      }
    }
  }
}
