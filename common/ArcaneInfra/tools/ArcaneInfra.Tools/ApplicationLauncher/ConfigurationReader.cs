using System;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.Reflection;

namespace Application
{
  public class ConfigurationReader
  {
    public static T Initialize<T> (Stream file)
      where T : class, new()
    {
      string xsd_resource_name = "ApplicationLauncher.ApplicationConfiguration.xsd"; // with namespace
      Assembly assembly = Assembly.GetExecutingAssembly ();

      XmlReaderSettings settings = new XmlReaderSettings ();
      using (Stream stream = assembly.GetManifestResourceStream(xsd_resource_name)) {
        if (stream == null)
          throw new ApplicationLauncher.InternalException (String.Format ("Cannot open resource '{0}'", xsd_resource_name));
        XmlSchema schema = XmlSchema.Read (stream, null);
        settings.Schemas.Add (schema);
      }
      settings.ValidationType = ValidationType.Schema;
      settings.ValidationFlags = XmlSchemaValidationFlags.ProcessIdentityConstraints | XmlSchemaValidationFlags.ReportValidationWarnings;

      XmlSerializer serializer = new XmlSerializer (typeof(T));
      using (XmlReader reader = XmlReader.Create(file, settings)) {
        return serializer.Deserialize (reader) as T;
      }
    }
  }
}

