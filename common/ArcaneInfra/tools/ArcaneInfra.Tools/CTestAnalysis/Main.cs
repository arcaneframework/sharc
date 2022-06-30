using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;

namespace CTestAnalysis
{
  class Program
  {
    private static int RTCODE_OK = 0;
    private static int RTCODE_ERROR = 1;

    static void Main (string[] args)
    {
      try {
        CTestAnalysis c = new CTestAnalysis ();
        c.analyze(args);
        Environment.Exit (RTCODE_OK);
      } catch (CTestAnalysisExit e) {
        Console.WriteLine (e.Message);
        Environment.Exit (RTCODE_OK);
      } catch (Exception e) {
        Console.Error.WriteLine (String.Format ("CTestAnalysis exiting with an error: {0}", e.Message));
        Environment.Exit (RTCODE_ERROR);
      }
    }
  }

  #region EXCEPTION
  [Serializable]
  public class CTestAnalysisException : Exception
  {
    public CTestAnalysisException ()
    {
    }
    
    public CTestAnalysisException (string message)
      : base(message)
    {
      ;
    }
  }
  
  [Serializable]
  public class CTestAnalysisExit : Exception
  {
    public CTestAnalysisExit ()
    {
    }
    
    public CTestAnalysisExit (string message)
      : base(message)
    {
      ;
    }
  }
  #endregion

  public class CTestAnalysis 
  {
    #region CONFIGURATIONS
    string codename = "CTestAnalysis.exe";
    string default_test_file = "Test.xml";
    #endregion
    
    #region MEMBERS
    private Mono.Options.OptionSet m_options = null;
    bool m_show_help = false;
    bool m_verbose = false;
    bool m_do_backup = false;
    #endregion

    public CTestAnalysis()
    {
      m_options = new Mono.Options.OptionSet ();

      m_options.Add ("v|verbose", "Verbose mode", v => { m_verbose = true; });
      m_options.Add ("h|help", "Show help page", v => { m_show_help = true; });
      m_options.Add ("backup", "Do backup of original file", v => { m_do_backup = true; });
    }

    public void analyze(string[] original_args)
    {
      String[] args = m_options.Parse (original_args).ToArray();
      try {
        // Check if remaining args looks like options
        for (int i=0; i<args.Length; ++i) {
          String arg = args [i];
          if (arg.StartsWith("-")) 
            throw new Mono.Options.OptionException (String.Format ("Invalid option {0}", arg), arg);
        }
        
        if (m_show_help || args.Length < 1) {
          showHelpAndExit ();
        }

        String directory = args[0];
        if (!Directory.Exists (directory)) throw new CTestAnalysisException(String.Format("Test-Directory '{0}' not found",directory));
        String[] lines = File.ReadAllLines(Path.Combine(directory,"TAG"));
        String tag_directory = Path.Combine(directory,lines[0]);
        String xml_test_file = Path.Combine(tag_directory,default_test_file);

        if (m_verbose)
          Console.WriteLine("Starting analysis of '{0}'", xml_test_file);    

        XmlDocument document = new XmlDocument ();
        document.Load (xml_test_file);
        if (m_do_backup) {
          String backup_file = xml_test_file + ".bak";
          if (m_verbose)
            Console.WriteLine("Backup done into '{0}'",backup_file);
          File.Copy(xml_test_file, backup_file);
        }

        XmlNode testing_element = document.DocumentElement.SelectSingleNode("Testing");

        Dictionary<String,String> converted_test_names = new Dictionary<string, string>();
       
        String regex_pattern = @"(_\d+proc)$";
        String regex_replacement = @"";
        Regex regex = new Regex(regex_pattern);
        {
          XmlNodeList test_list_nodes = testing_element.SelectNodes("Test");
          foreach(XmlNode node in test_list_nodes)
          {
            String test_name = (node.SelectSingleNode("Name") as XmlElement).InnerText;
            String full_test_name = (node.SelectSingleNode("FullName") as XmlElement).InnerText;
            String converted_test_name = test_name;
            String converted_test_path = regex.Replace(test_name, regex_replacement);
            converted_test_path = "/" + String.Join ("/", converted_test_path.Split(new Char[] { '_', '-' }));
            converted_test_names.Add (full_test_name, converted_test_name);

            node.SelectSingleNode("Name").InnerText = converted_test_name;
            node.SelectSingleNode("FullName").InnerText = converted_test_name;
            node.SelectSingleNode("Path").InnerText = converted_test_path;
            if (m_verbose) {
              Console.WriteLine("[VERBOSE] test '{0}' will be renamed '{1}'",full_test_name,converted_test_name);
            }
          }
        }

        {
          XmlNodeList test_list_nodes = testing_element.SelectSingleNode("TestList").SelectNodes("Test");
          foreach(XmlNode node in test_list_nodes)
          {
            String test_name = (node as XmlElement).InnerText;
            String converted_test_name;
            if (!converted_test_names.TryGetValue(test_name, out converted_test_name))
            {
              throw new CTestAnalysisException(String.Format("Inconsistent test name '{0}'",test_name));
            }
            node.InnerText = converted_test_name;
          }
        }
            
        document.Save (xml_test_file);

        if (m_verbose)
          Console.WriteLine("Analysis done");    

      } catch (Mono.Options.OptionException e) {
        showHelpAndExit (e.Message);
      } catch (XmlException e) {
        throw new CTestAnalysisExit (String.Format("Error while loading {0}\nMessage: {1}", e.SourceUri, e.Message));
      }
    }

    private void showHelpAndExit (String message = null)
    {
      StringWriter writer = new StringWriter ();
      if (message == null)
        writer.WriteLine ("Requested Help page");
      else
        writer.WriteLine (message);
      
      writer.WriteLine ("Usage: {0} [options] Test-Directory", codename);
      writer.WriteLine ("Options : ");
      m_options.WriteOptionDescriptions (writer);
      
      if (message == null)
        throw new CTestAnalysisExit (writer.ToString ());
      else
        throw new CTestAnalysisException (writer.ToString ());
    }

  }
}
