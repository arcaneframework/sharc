using System;
using System.Collections.Generic;
using System.Xml;
using System.IO;

namespace ConfigFileCreator
{
  public class XmlDirectoryWriter
  {
    HashSet<string> m_dirs = new HashSet<string>();
    
    public XmlDirectoryWriter() {}

    public void Add(IList<string> dirs)
    {
      foreach(var dir in dirs)
        m_dirs.Add(dir);
    }

    public void Write(XmlWriter writer)
    {
      if(writer == null)
        throw new NullReferenceException("writer");
      
      if(m_dirs.Count > 0)
      {
        writer.WriteStartElement("directories");
        foreach(String directory in m_dirs) {
          writer.WriteStartElement("directory");
          writer.WriteString(directory);
          writer.WriteEndElement();
        }
        writer.WriteEndElement();
      }
    }
  }
}
