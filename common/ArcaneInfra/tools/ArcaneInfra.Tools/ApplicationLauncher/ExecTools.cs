using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Linq;
using System.Text.RegularExpressions;

namespace ApplicationLauncher
{
	public class ExecTools
	{
		public delegate string ArgConverter(string args); //!< New delegate type for Argument Converter 

		public static bool isWindows()
		{
			switch (Environment.OSVersion.Platform) 
			{
			case PlatformID.Win32NT:
			case PlatformID.Win32S:
			case PlatformID.Win32Windows:
			case PlatformID.WinCE:
				return true;
			case PlatformID.MacOSX:
			case PlatformID.Unix:
				return false;
			default:
				Logger.log(Logger.Channel.Error, "Internal Error: Unknown plaform '{0}'\n", Environment.OSVersion.Platform);
				Environment.Exit(-1);
				return false;
			}
		}

		public static int ExecProcess(string cmd, string[] args, Env.Tag[] tags = null)
		{
			int r = 0;
			using (Process p = new Process()) {
				
				p.StartInfo.UseShellExecute = false;
				p.StartInfo.FileName = cmd;
				p.StartInfo.Arguments = argvToCommandLine(args);
				
				if (Launcher.Options.dry_mode)
					Logger.log(Logger.Channel.Dry,"Execute: {0} {1}", cmd, p.StartInfo.Arguments);
				else if (Launcher.Options.verbose_run)
					Logger.log(Logger.Channel.Verbose,"Execute: {0} {1}", cmd, p.StartInfo.Arguments);

				Env.LoadEnvironment(p.StartInfo, tags);
				
				if (Launcher.Options.output_file != null)
				{
					p.StartInfo.RedirectStandardOutput = true;
					p.StartInfo.RedirectStandardError = true;
					p.OutputDataReceived += (sender, out_args) => {
						if (out_args.Data != null) {
							Logger.log(Logger.Channel.Execute, out_args.Data);
						} };
					p.ErrorDataReceived += (sender, out_args) => {
						if (out_args.Data != null) {
							Logger.log(Logger.Channel.ExecuteError, out_args.Data);
						} };
				}
				
				if (Launcher.Options.internal_debug == Launcher.InternalDebugMode.Info)
				{
					System.Collections.Specialized.StringDictionary variables = p.StartInfo.EnvironmentVariables;
					List<KeyValuePair<string,string>> sorted_variables = new List<KeyValuePair<string, string>>();
					foreach(DictionaryEntry variable in variables) 
					{
						string key = variable.Key.ToString();
						string value = null;
						if (variable.Value != null) value = variable.Value.ToString();
						// else value = "(null)";
						sorted_variables.Add (new KeyValuePair<string, string>(key,value)); 
					}
					sorted_variables = sorted_variables.OrderBy(o => o.Key).ToList();
					Logger.log (Logger.Channel.Debug, "Defined Environment variable List");
					foreach(var variable in sorted_variables)
					{
						Logger.log(Logger.Channel.Debug, "\t{0}={1}", variable.Key, variable.Value);
					}
				}
				
				try {
					if (!Launcher.Options.dry_mode) {
						p.Start ();
						if (Launcher.Options.output_file != null)
						{
							p.BeginOutputReadLine ();
							p.BeginErrorReadLine ();
						}
						p.WaitForExit ();
						r = p.ExitCode;
					}
				}
				// catch (Win32Exception e)
				catch (Exception e) {
					Logger.log(Logger.Channel.Error,"Error '{0}' has been occurred while executing command:\n\t{1} {2}", e.Message, cmd, p.StartInfo.Arguments);
					r = -1;
				}
			}
			if (Launcher.Options.internal_debug == Launcher.InternalDebugMode.Info)
				Logger.log (Logger.Channel.Debug, "Completed execution : {0} {1}", cmd, argvToCommandLine(args));
			return r;
		}

		public static string argvToCommandLine(IEnumerable<string> args)
		{
			ArgConverter converter = null;
			
			if (isWindows ()) 
				converter = convertToWindowsCommandLine;
			else
				converter = convertToLinuxCommandLine;
			
			return String.Join (" ", args.Select(x => converter (x)).ToArray());
		}
		
		private static string convertToLinuxCommandLine(string s) 
		{
			if (string.IsNullOrEmpty (s))
				return s;
			s = Regex.Replace (s, @"\\", @"\\");
			s = Regex.Replace (s, @"("")", @"\$1");
			return "\"" + s + "\"";
		}
		
		private static string convertToWindowsCommandLine(string s) 
		{
			// Inspiré de la discussion stackoverflow : http://stackoverflow.com/questions/5510343/escape-command-line-arguments-in-c-sharp/6040946#6040946
			if (string.IsNullOrEmpty (s))
				return s;

			bool Force = false;
			if (Force)
			{
				// Force surrounding quotes
				s = Regex.Replace (s, @"(\\*)" + "\"", @"$1$1\" + "\"");
				s = "\"" + Regex.Replace (s, @"(\\+)$", @"$1$1") + "\"";
			}
			else
			{
				// Do not force surrounding quotes
				s = Regex.Replace (s, @"(\\*)" + "\"", @"$1\$0");
				s = Regex.Replace (s, @"^(.*\s.*?)(\\*)$", "\"$1$2$2\"");
			}
			return s;
		}

		private static string convertToWindowsCommandLine2(string Argument)
		{
			// Inspiré de http://blogs.msdn.com/b/twistylittlepassagesallalike/archive/2011/04/23/everyone-quotes-arguments-the-wrong-way.aspx
			
			bool Force = false; // force surrounding "
			// Unless we're told otherwise, don't quote unless we actually
			// need to do so --- hopefully avoid problems if programs won't
			// parse quotes properly
			if (Force == false 
			    && String.IsNullOrEmpty(Argument) == false 
			    && Argument.IndexOfAny(new char[] { ' ', '\t', '\n', '\v', '\"' }) == -1) 
			{
				return Argument;
			}
			else 
			{
				StringBuilder CommandLine = new StringBuilder();
				IEnumerator<char> It = Argument.GetEnumerator();
				while(It.MoveNext()) {
					int NumberBackslashes = 0;
					bool do_continue = true;
					while (It.Current == '\\' && (do_continue = It.MoveNext())) {
						++NumberBackslashes;
					}
					
					if (!do_continue) {
						// Escape all backslashes, but let the terminating
						// double quotation mark we add below be interpreted
						// as a metacharacter.
						CommandLine.Append (new string ('\\', NumberBackslashes * 2));
						break;
					} else if (It.Current == '\"') {
						// Escape all backslashes and the following double quotation mark.
						CommandLine.Append (new string('\\', NumberBackslashes * 2 + 1));
						CommandLine.Append(It.Current);
					}
					else {
						// Backslashes aren't special here.
						CommandLine.Append (new string('\\', NumberBackslashes));
						CommandLine.Append(It.Current);
					}
				}
				return "\"" + CommandLine.ToString () + "\"";
			}
		}

		public static ArgConverter converter()
		{
			if (isWindows ())
				return convertToWindowsCommandLine;
			else
				return convertToLinuxCommandLine;
		}

		public static string[] toArgs(Object o1)
		{
			return new string[] { o1.ToString () };
		}
		
		public static string[] toArgs(Object o1, Object o2)
		{
			return new string[] { o1.ToString (), o2.ToString() };
		}
		
		public static string[] toArgs(Object o1, Object o2, Object o3)
		{
			return new string[] { o1.ToString (), o2.ToString(), o3.ToString() };
		}
		
		public static string[] toArgs(Object o1, Object o2, Object o3, Object o4)
		{
			return new string[] { o1.ToString (), o2.ToString(), o3.ToString(), o4.ToString() };
		}
		
		public static String FindExec(String exeName)
		{
			string environmentPath = System.Environment.GetEnvironmentVariable("PATH");
			string[] paths = environmentPath.Split(Path.PathSeparator);
			string exePath = paths.Select(x => Path.Combine(x, exeName))
				.Where(x => File.Exists(x))
					.FirstOrDefault();
			if (exePath != null)
				return exePath;
			else 
				return null;
		}
	}
}

