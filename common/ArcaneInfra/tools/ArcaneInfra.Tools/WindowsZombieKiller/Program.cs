using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace WindowsZombieKiller
{
    class Program
    {
        private static int RTCODE_OK = 0;
        private static int RTCODE_ERROR = 1;

        static void Main(string[] args)
        {
            try
            {
                WindowsZombieKiller c = new WindowsZombieKiller();
                c.analyze(args);
                Environment.Exit(RTCODE_OK);
            }
            catch (WinZombieKillerExit e)
            {
                Console.WriteLine(e.Message);
                Environment.Exit(RTCODE_OK);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(String.Format("WindowsZombieKiller exiting with an error: {0}", e.Message));
                Environment.Exit(RTCODE_ERROR);
            }
        }
    }

    #region EXCEPTION
    [Serializable]
    public class WinZombieKillerException : Exception
    {
        public WinZombieKillerException()
        {
        }

        public WinZombieKillerException(string message)
            : base(message)
        {
            ;
        }
    }

    [Serializable]
    public class WinZombieKillerExit : Exception
    {
        public WinZombieKillerExit()
        {
        }

        public WinZombieKillerExit(string message)
            : base(message)
        {
            ;
        }
    }
    #endregion

    public interface IDetectOpenFiles
    {
        List<Process> GetProcessesUsingFiles(List<String> files);
    }


    public class WindowsZombieKiller
    {
        #region CONFIGURATIONS
        string codename = "WindowsZombieKiller.exe";
        string default_handle_path = @"G:\Rt40_50_prj\D\D1653_Arcane\Public\Windows64\Utils\Handle\Handle.exe";
        #endregion

        #region MEMBERS
        private Mono.Options.OptionSet m_options = null;
        bool m_show_help = false;
        bool m_verbose = false;
        string m_handle_path = null;
        bool m_kill = false;
        int m_retry_count = 0;
        bool m_skip_invalid = false;
        #endregion

        public WindowsZombieKiller()
        {
            m_options = new Mono.Options.OptionSet();
            m_options.Add("k|kill", "Kill found processes which use resource", v => { m_kill = true; });
            m_options.Add("retry=", "Retry to kill several times", (int n) => { m_retry_count = n; });
            m_options.Add("skip|skip-invalid", "Skip invalid directory or files", s => { m_skip_invalid = true; });
            m_options.Add("with-handle:", String.Format("use handle sysinternal command\n(default path is {0})", default_handle_path), (string s) => { if (String.IsNullOrEmpty(s)) m_handle_path = default_handle_path; else m_handle_path = s; });
            m_options.Add("v|verbose", "Verbose mode", v => { m_verbose = true; });
            m_options.Add("h|help", "Show help page", v => { m_show_help = true; });
        }

        public void analyze(string[] args)
        {
            try
            {
                List<String> remaining_args = m_options.Parse(args);

                // Before directory/file checking
                if (m_show_help || remaining_args.Count < 1)
                    ShowHelpAndExit();
                if (m_retry_count < 0)
                    throw new Mono.Options.OptionException(String.Format("--retry argument cannot be negative"),"--retry");

                List<String> filepaths = new List<String>();
                
                // Check if remaining args looks like options
                foreach(String arg in remaining_args)
                {
                    if (arg.StartsWith("-"))
                        throw new Mono.Options.OptionException(String.Format("Invalid option {0}", arg), arg);
                    String full_path = Path.GetFullPath(arg);
                    if (Directory.Exists(full_path) || File.Exists(full_path))
                        filepaths.Add(full_path);
                    else
                    {
                        if (m_skip_invalid)
                            Console.WriteLine("Warning: skip invalid path '{0}'", arg);
                        else
                            throw new WinZombieKillerException(String.Format("Argument '{0}' is not a valid path", arg));
                    }
                }

                if (filepaths.Count < 1)
                {
                    Console.WriteLine("Nothing to clean; exiting");
                    return;
                }

                IDetectOpenFiles x = null;
                if (m_handle_path != null)
                {
                    if (m_verbose)
                        Console.WriteLine("Starting cleaning using External Handle command of {0}", String.Join(" ", filepaths.Select(s => "'" + s + "'")));
                    x = new ExternalDetectOpenFiles(m_handle_path);
                }
                else
                {
                    if (m_verbose)
                        Console.WriteLine("Starting cleaning using Internal command of {0}", String.Join(" ", filepaths.Select(s => "'" + s + "'")));
                    x = new InternalDetectOpenFiles();
                }

                List<Process> pids = x.GetProcessesUsingFiles(filepaths);

                if (m_kill)
                {   // retry only if kill is enabled
                    DoKill(pids);
                    for (int retry_index = 1; retry_index <= m_retry_count && pids.Count > 0; ++retry_index)
                    {
                        if (m_verbose)
                            Console.WriteLine("Retry #{0} to check busy resource", retry_index);
                        System.Threading.Thread.Sleep(1000);
                        pids = x.GetProcessesUsingFiles(filepaths);
                        DoKill(pids);
                    }

                    pids = x.GetProcessesUsingFiles(filepaths);
                    if (pids.Count > 0)
                        throw new WinZombieKillerException("Cannot clean processes using resource");
                }
                if (m_verbose)
                    Console.WriteLine("Cleaning done");
            }
            catch (Mono.Options.OptionException e)
            {
                ShowHelpAndExit(e.Message);
            }
        }

        // using external command Handle.exe : https://technet.microsoft.com/en-us/sysinternals/bb896655.aspx
        private class ExternalDetectOpenFiles : IDetectOpenFiles
        {
            #region CONFIGURATIONS
            private string m_handle_path;
            private string default_handle_options = @"/accepteula -u";
            #endregion

            public ExternalDetectOpenFiles(String handle_path)
            {
                m_handle_path = handle_path;
            }

            public List<Process> GetProcessesUsingFiles(List<string> files)
            {
                string matchPattern = @"^(?<cmd>.*)(\s+pid:\s+)\b(?<pid>\d+)\b(\s+type:\s+)(?<type>\w+)\s+(?<user>\S+)\s+(?<id>\S+):\s+(?<file>[^\r\n]*)";
                Regex regex = new Regex(matchPattern, RegexOptions.ExplicitCapture | RegexOptions.Multiline);

                List<Process> pids = new List<Process>();

                foreach (String file in files)
                {
                    if (m_handle_path != null)
                    {
                        Process tool = new Process();
                        tool.StartInfo.FileName = m_handle_path;
                        tool.StartInfo.Arguments = default_handle_options + " " + file;
                        tool.StartInfo.UseShellExecute = false;
                        tool.StartInfo.RedirectStandardOutput = true;
                        tool.Start();
                        tool.WaitForExit();
                        string outputTool = tool.StandardOutput.ReadToEnd();

                        foreach (Match match in regex.Matches(outputTool))
                        {
                            String cmd = match.Groups["cmd"].Value;
                            int pid = int.Parse(match.Groups["pid"].Value);
                            String type = match.Groups["type"].Value;
                            String user = match.Groups["user"].Value;
                            String id = match.Groups["id"].Value;
                            String filename = match.Groups["file"].Value;

                            Console.WriteLine("Busy resource found on {0} '{1}' used by process {2} of user {3}", type, filename, pid, user);

                            try
                            {
                                pids.Add(Process.GetProcessById(pid));
                            }
                            catch { } // some processes may fail
                        }
                    }
                }

                return pids.Distinct().ToList();
            }
        }

        private void DoKill(List<Process> pids)
        {
            foreach (Process p in pids)
            {
                try
                {
                    Console.WriteLine("Killing process {0} with command '{1}'", p.Id, p.ProcessName);
                    p.Kill();
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine(String.Format("Cannot kill process : {0}", e.Message));
                }
            }
        }

        private void ShowHelpAndExit(String message = null)
        {
            StringWriter writer = new StringWriter();
            if (message == null)
                writer.WriteLine("Requested Help page");
            else
                writer.WriteLine(message);

            writer.WriteLine("Usage: {0} [options] Directory-Or-File...", codename);
            writer.WriteLine("Options : ");
            m_options.WriteOptionDescriptions(writer);

            if (message == null)
                throw new WinZombieKillerExit(writer.ToString());
            else
                throw new WinZombieKillerException(writer.ToString());
        }

    }
}