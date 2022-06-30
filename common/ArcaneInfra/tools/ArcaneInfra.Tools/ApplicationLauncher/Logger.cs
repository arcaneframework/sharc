using System;
using System.Text;
using System.IO;
using System.Reflection;
using System.Resources;

namespace ApplicationLauncher
{
	public class Logger
	{
		static public void configureOutputFile(String output_file)
		{
			if (output_file != null)
			{
				bool overwritten = File.Exists(output_file);
				StreamWriter sw = new StreamWriter(output_file);
				sw.AutoFlush = true;
				Console.SetOut (new MirroringTextWriter(Console.Out, sw));
				
				if (overwritten) log(Channel.Warning, "output file '{0}' will be overwritten",output_file);
			}
		}

    static public void configureEnableDebug(bool flag) {
      enable_debug_mode = flag;
    }

		#region Logging facilities
		public enum Channel { Execute=0, ExecuteError, Debug, DevDebug, Verbose, Error, Warning, Dry, Info, MPIInfo, Banner };
    private static readonly string[] StringChannel = { "Execute", "Execute Error", "Debug", "Development", "Verbose", "Error", "Warning", "Dry", "Info", "MPI info", "" };
		
		static public void log(Channel c, string format) 
		{
			log(c, format, new object[]{});
		}
		
		static public void log(Channel c, string format, object arg0) 
		{
			log(c, format, new object[]{arg0});
		}
		
		static public void log(Channel c, string format, object arg0, object arg1) 
		{
			log(c, format, new object[]{arg0, arg1});
		}
		
		static public void log(Channel c, string format, object arg0, object arg1, object arg2) 
		{
			log(c, format, new object[]{arg0, arg1, arg2});
		}
		
		static public void log(Channel c, string format, object arg0, object arg1, object arg2, object arg3) 
		{
			log(c, format, new object[]{arg0, arg1, arg2, arg3});
		}
		
		static public void log(Channel c, string format, Object[] args) 
		{
      if (c == Channel.Debug && !enable_debug_mode)
        return;
#if !DEBUG
      if (c == Channel.DevDebug)
        return;
#endif

			String channel = StringChannel [(int)c];
			StringBuilder message = new StringBuilder();
			if (c != Channel.Banner)
				message.AppendFormat ("[{0}] ", channel);
			message.AppendFormat (format, args);
			if (c == Channel.Warning || c == Channel.Error)
				Console.ForegroundColor = ConsoleColor.Red;
			String s = message.ToString ();
			if (c == Channel.Banner)
				s = getBanner (s);
			Console.WriteLine( s );
			Console.ResetColor ();
		}

		static public void log(string channel, string format) 
		{
			log(channel, format, new object[]{});
		}
		
		static public void log(string channel, string format, object arg0) 
		{
			log(channel, format, new object[]{arg0});
		}
		
		static public void log(string channel, string format, object arg0, object arg1) 
		{
			log(channel, format, new object[]{arg0, arg1});
		}
		
		static public void log(string channel, string format, object arg0, object arg1, object arg2) 
		{
			log(channel, format, new object[]{arg0, arg1, arg2});
		}
		
		static public void log(string channel, string format, object arg0, object arg1, object arg2, object arg3) 
		{
			log(channel, format, new object[]{arg0, arg1, arg2, arg3});
		}
		
		static public void log(string channel, string format, Object[] args) 
		{
			StringBuilder message = new StringBuilder();
			message.AppendFormat ("[{0}] ", channel);
			message.AppendFormat (format, args);
			String s = message.ToString ();
			Console.WriteLine( s );
		}

		private class MirroringTextWriter : TextWriter {
			private TextWriter main, aux;
			
			public MirroringTextWriter(TextWriter Main, TextWriter Aux) {
				main = Main;
				aux = Aux;
			}
			public override Encoding Encoding {
				get {
					return main.Encoding;
				}
			}
			public override void Write(char value) {
				main.Write (value);
				aux.Write (value);
			}
			public override void Write(string value) {
				main.Write (value);
				aux.Write (value);
			}
		};
		#endregion

		#region BANNERS
		static private string getBanner(string title)
		{
			Assembly assembly = Assembly.GetExecutingAssembly ();
      ResourceReader reader = new ResourceReader(assembly.GetManifestResourceStream ("ApplicationLauncher.Resources.Banners.resources"));
			ResourceSet resSet = new ResourceSet (reader);
			
			// Build banner char by char
			string[] lines = new string[13];
			foreach(char c in title) 
			{
				string char_name = null;
				if      (c >= 'A' && c <= 'Z') { char_name = "big-char-" + c; }
				else if (c >= 'a' && c <= 'z') { char_name = "small-char-" + c; }
				else if (c >= '0' && c <= '9') { char_name = "num-char-" + c; }
				else                           { char_name = "char-" + c; }
				String banner = resSet.GetString(char_name);
				if (banner != null) {
					string[] char_lines = banner.Split('\n');
					for(int i=0;i<char_lines.Length;++i)
						lines[i] += char_lines[i];
				} else if (Launcher.Options.internal_debug == Launcher.InternalDebugMode.Info) {
					Logger.log(Channel.Error, "Unknown char '{0}'",c);
				}
			}
			
			// Underline chars
			char[] line = lines [10].ToCharArray ();
			bool prev_white = true;
			for(int j=0;j<line.Length;++j) {
				bool current_white = (line[j] == ' ');
				bool next_white = (j < line.Length-1)?(line[j+1] == ' '):true;
				if (prev_white && current_white && next_white) {
					line[j] = 'o';
				}
				prev_white = current_white;
			}
			lines[10]= new string(line);
			
			string full_banner = "";
			for (int i=0; i<lines.Length-1; ++i)
				full_banner += lines [i] + '\n';
			return full_banner;
		}
		#endregion

    #region MEMBERS
    private static bool enable_debug_mode = false;
    #endregion
	}
}

