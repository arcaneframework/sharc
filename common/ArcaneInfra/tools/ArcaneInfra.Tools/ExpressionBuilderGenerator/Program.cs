using System;
using System.Collections.Generic;
using System.IO;

namespace ExpressionBuilderGenerator
{
  class Program
  {
    public class Interface : InterfaceGenerator
    {
      public Interface (int nn, int mm, string commandline)
      {
        this.Session = new Dictionary<string, object> ();
        this.Session ["n"] = nn;
        this.Session ["m"] = mm;
        this.Session ["commandline"] = commandline;
        this.Initialize ();
        m_n = nn;
        m_m = mm;
      }

      public void Generate (string path)
      {
        string file = path + "/IFunctionR" + m_n + "vR" + m_m + ".h";
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine ("* generate {0}", file);
        Console.ResetColor ();
        using (StreamWriter writer = new StreamWriter (file)) {
          writer.WriteLine (TransformText ());
        }
      }
        private int m_n, m_m;
    }

    public class Axl : AxlGenerator
    {
      public Axl (int nn, int mm)
		  {
        this.Session = new Dictionary<string, object> ();
			  this.Session ["n"] = nn;
				this.Session ["m"] = mm;
        this.Initialize ();
                m_n = nn;
                m_m = mm;
			}
			
			public void Generate (string path)
			{
				string file = path + "/ExpressionBuilderR" + m_n + "vR" + m_m + ".axl";
				Console.ForegroundColor = ConsoleColor.Blue;
				Console.WriteLine ("* generate {0}", file);
				Console.ResetColor ();
				using (StreamWriter writer = new StreamWriter (file)) {
          writer.WriteLine (TransformText ());
        }
			}
            private int m_n, m_m;
		}

    public class Service : ServiceGenerator
    {
      public Service (int nn, int mm, string commandline)
      {
        this.Session = new Dictionary<string, object> ();
        this.Session ["n"] = nn;
        this.Session ["m"] = mm;
        this.Session ["commandline"] = commandline;
        this.Initialize ();
        m_n = nn;
        m_m = mm;
      }
      
      public void Generate (string path)
      {
        string file = path + "/ExpressionBuilderR" + m_n + "vR" + m_m + "Service.cc";
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine ("* generate {0}", file);
        Console.ResetColor ();
        using (StreamWriter writer = new StreamWriter (file)) {
          writer.WriteLine (TransformText ());
        }
      }
        private int m_n, m_m;
    }

    public class Header : HeaderGenerator
    {
      public Header (int nn, int mm, string commandline)
      {
        this.Session = new Dictionary<string, object> ();
        this.Session ["n"] = nn;
        this.Session ["m"] = mm;
        this.Session ["commandline"] = commandline;
        this.Initialize ();
        m_n = nn;
        m_m = mm;
      }
      
      public void Generate (string path)
      {
        string file = path + "/ExpressionBuilderR" + m_n + "vR" + m_m + "Core.h";
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine ("* generate {0}", file);
        Console.ResetColor ();
        using (StreamWriter writer = new StreamWriter (file)) {
          writer.WriteLine (TransformText ());
        }
      }

      private int m_n, m_m;
    }

    public class Class : ClassGenerator
    {
      public Class (int nn, int mm, string commandline)
      {
        this.Session = new Dictionary<string, object> ();
        this.Session ["n"] = nn;
        this.Session ["m"] = mm;
        this.Session ["commandline"] = commandline;
        this.Initialize ();
        m_n = nn;
        m_m = mm;
      }
      
      public void Generate (string path)
      {
        string file = path + "/ExpressionBuilderR" + m_n + "vR" + m_m + "Core.cc";
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine ("* generate {0}", file);
        Console.ResetColor ();
        using (StreamWriter writer = new StreamWriter (file)) {
          writer.WriteLine (TransformText ());
        }
      }
        private int m_n, m_m;
    }

    static void Main(string[] args)
    {
      Mono.Options.OptionSet opt_set = new Mono.Options.OptionSet ();

      bool show_help = false;
      int n = -1;
      int m = 1;
      string path = null;
      bool generate_interface = false;
      bool generate_service = false;
      bool generate_class = false;
      bool accumulation = false;
      string commandline = String.Join (" ", args);

      opt_set.Add ("h|help|?", "This help page", v => {
        show_help = true; });
      opt_set.Add ("n=", "domain size", v => {
        n = Convert.ToInt32 (v); });
      opt_set.Add ("m=", "codomain size (must be one)", v => {
        m = Convert.ToInt32 (v); });
      opt_set.Add ("path=", "path where generate files", v => {
        path = v; });
      opt_set.Add ("interface", "generate interface IFunction", v => {
        generate_interface = true; });
      opt_set.Add ("service", "generate service ExpressionBuilder", v => {
        generate_service = true; });
      opt_set.Add ("class", "generate class ExpressionBuilderCore", v => {
        generate_class = true; });
      opt_set.Add ("all", "generate all", v => {
        generate_class = true;
        generate_service = true;
        generate_interface = true; });
      opt_set.Add ("accumulation", "generate domain from 1 to n", v => {
        accumulation = true; });

      opt_set.Parse (args);

      if (m != 1) {
        Console.WriteLine ("Error, m parameter must be one");
        show_help = true;
      }

      if (path == null) {
        Console.WriteLine ("Error, a path must be given");
        show_help = true;
      }

      if (n <= 0) {
        Console.WriteLine ("Error, n parameter must be greater than zero");
        show_help = true;
      }

      if (show_help) {
        Console.WriteLine ("Usage: ExpressionBuilderGenerator.exe [options]");
        Console.WriteLine ("Options:");
        opt_set.WriteOptionDescriptions (Console.Out);
        Environment.Exit (0);
      }

      int start = accumulation ? 1 : n;

      for (int i = start; i <= n; ++i) {

        if (generate_interface) {
          Interface ifunction = new Interface (i, m, commandline);
          ifunction.Generate (path);
        }

        if (generate_service) {
          Axl axl = new Axl (i, m);
          axl.Generate (path);
          Service service = new Service (i, m, commandline);
          service.Generate (path);
        }

        if (generate_class) {
          Header header = new Header (i, m, commandline);
          header.Generate (path);
          Class class_ = new Class (i, m, commandline);
          class_.Generate (path);
        }
      }
    }
  }
}
