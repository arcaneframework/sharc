using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.IO;
using System.Linq;
using System.Globalization;
using System.Reflection;
using System.Text.RegularExpressions;

// TODO --ignore-current-environment ou --reset-env uniquement sous Linux
// TODO reformat files by replacing spaces <=> tabs 
// TODO les export successifs qui ajoutent des données sur une même variable d'environnement peuvent être conflictuels car exécutés sans prise en compte de la mise à jour de la valeur

namespace ApplicationLauncher
{
  #region EXCEPTION
  [Serializable]
  public class LauncherException : Exception
  {
    public LauncherException (string message)
      : base(message)
    {
      ;
    }
  }

  [Serializable]
  public class InternalException : Exception
  {
    public InternalException (string message)
      : base(message)
    {
      ;
    }
  }

  [Serializable]
  public class ExitException : Exception
  {
    public ExitException (string message)
      : base(message)
    {
      ;
    }
  }
  
  [Serializable]
  public class HelpException : Exception
  {
    public HelpException (string message)
      : base(message)
    {
      ;
    }
  }
  #endregion


  public class Launcher
  {
    #region CONFIGURATIONS
    private readonly string mpirun_linux_default = "mpirun";
    private readonly string[] mpirun_linux_default_options = { };
    private readonly string mpirun_windows_default = "mpiexec.exe";
    private readonly string[] mpirun_windows_default_options = { "--delegate" };
    private readonly Version mpirun_required_version = new Version ("4.0.2");
    #endregion
    
    public enum InternalDebugMode
    {
      None,
      Info,
      Test,
      Echo }
    ;

    public interface ILauncherOptions
    {
      bool dry_mode { get; }

      bool verbose_run { get; }

      bool check_mode { get; }

      string output_file { get; }

      InternalDebugMode internal_debug { get; }

      string install_path { get; }

      string rootDir { get; }

      string binaryDir { get; }

      string libDir { get; }

      int nb_proc { get; }

      string mpirun_path { get; }

      string mpirt_path { get; }

      List<string> mpirun_extra_options { get; }
    }

    private class LauncherOptions : ILauncherOptions
    {
      public LauncherOptions ()
      {
        dry_mode = false;
        verbose_run = false;
        check_mode = false;
        output_file = null;
        internal_debug = InternalDebugMode.None;
        install_path = null;
        rootDir = null;
        binaryDir = null;
        libDir = null;

        nb_proc = 1;
        mpirun_path = null;
        mpirt_path = null;
        mpirun_extra_options = new List<string> ();
      }

      public bool dry_mode { get; set; }

      public bool verbose_run { get; set; }

      public bool check_mode { get; set; }

      public string output_file { get; set; }

      public InternalDebugMode internal_debug { get; set; }

      public string install_path { get; set; }

      public int nb_proc { get; set; }

      public string mpirun_path { get; set; }

      public string mpirt_path { get; set; }

      public List<string> mpirun_extra_options { get; set; }

      public string rootDir { get; set; }

      public string binaryDir { get; set; }

      public string libDir { get; set; }
    }

    #region OPTIONS WITH DEFAULT VALUES
    private bool show_help = false;
    private string parameters; // file.arc
    #endregion

    #region MEMBERS
    private Application.ILoader loader = null;
    private Mono.Options.OptionSet opt_set = null;
    private static LauncherOptions options = null;
    private List<Application.IAction> actions = new List<Application.IAction> ();
    #endregion

    public static ILauncherOptions Options { get { return options; } }




    #region CONSTRUCTOR
    public Launcher ()
    {
      options = new LauncherOptions ();

      string assembly_name = "Application";
      // Si log : pb avec --internal-debug=test
      // Logger.log(Logger.Channel.Dev, "Loading {0} Assembly",assembly_name);
      Assembly ass = Assembly.Load (assembly_name);
      if (ass == null)
        throw new LauncherException (String.Format ("Cannot load {0} Assembly", assembly_name));
      Type ObjType = ass.GetType (assembly_name + ".Loader");
      if (ObjType == null)
        throw new LauncherException (String.Format ("Cannot load {0}.Loader Object", assembly_name));
      loader = (Application.ILoader)Activator.CreateInstance (ObjType);
      actions.AddRange (loader.Load ());
      // Logger.log(Logger.Channel.Dev, "{0} Assembly loaded", assembly_name);

      setupOptions ();
    }
    #endregion
    
    #region OPTIONS
    private void setupOptions ()
    {
      // Fix Invariant culture (US) for data representation and floating number parsing
      System.Threading.Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture; 

      opt_set = new Mono.Options.OptionSet ();

      foreach (Application.IAction action in actions) {
        Logger.log (Logger.Channel.Debug, "Starting Setup action for {0} component", action.Name);
        action.Setup (ref opt_set);
      }

      if (loader.EnableParallelMode) {
        opt_set.Add ("n|mpirun-nproc=", "define required processor number in simulation mode\n(only one used in forecast mode)", (int n) => {
          options.nb_proc = n; });
        if (ExecTools.isWindows ()) {
          opt_set.Add ("mpirun=", String.Format ("select Intel MPI launcher command\n(Windows default is {0}; requires version {1} or more)", mpirun_windows_default, mpirun_required_version), 
                     (string path) => {
            options.mpirun_path = path; });
        } else {
          opt_set.Add ("mpirun=", String.Format ("select Intel MPI launcher command\nLauncher command may contain options which will be prepended to mpirun-options\n(Linux default is {0}; requires version {1} or more)", mpirun_linux_default, mpirun_required_version),
                     (string path) => {
            options.mpirun_path = path; });
          opt_set.Add("mpirt-path=", "add a directory containing the mpi runtime and libraries (in mpirt-path/bin64) \n(default or if mpirun missing in mpirt-path/bin/64: mpirt is taken in the loaded env if available or searched in install_path/bin/mpirt)", (string s) => { options.mpirt_path = s; });
        }
        
        {
          string[] mpirun_default_options = (ExecTools.isWindows ()) ? mpirun_windows_default_options : mpirun_linux_default_options;
          string default_options_string = String.Join ("', '", mpirun_default_options);
          if (mpirun_default_options.Length == 0)
            default_options_string = "is empty";
          else if (mpirun_default_options.Length == 1)
            default_options_string = String.Format ("is '{0}'", default_options_string);
          else 
            default_options_string = String.Format ("are '{0}'", default_options_string);
          opt_set.Add ("x|mpirun-options=", "Add one extra option for mpirun command; this option may be repeated; default " + default_options_string,
                   (string options) => {
            Launcher.options.mpirun_extra_options.Add (options); });
        }
      }

      opt_set.Add ("o|output-file=", "filename to save all console output (default is disabled)", (string s) => {
        options.output_file = s;
        Logger.configureOutputFile (s); });
      opt_set.Add ("d|dry", "dry mode; simulate a run sequence", v => {
        options.dry_mode = true; });
      opt_set.Add ("v|verbose", "verbose run mode", v => {
        options.verbose_run = true; });
      opt_set.Add ("h|help|?", "This help page\nThen, parameters.arc is optional\n", v => {
        show_help = true; });

      opt_set.Add ("internal-debug:", "enable launcher internal debug infos\nChoose one between info, test and echo (default is 'info')", (string v) => {
        configureInternalDebug (v); });
      opt_set.Add ("install-path=", "select a root directory for installation\n(default is launcher directory)", (string s) => {
        options.install_path = s; });
      opt_set.Add ("check", "check installation", v => {
        options.check_mode = true; });
    }

    public void parseArguments (string[] args)
    {
      try {
        List<string> remaining_args = opt_set.Parse (args);

        // Check if remaining args looks like options
        for (int i=0; i<remaining_args.Count; ++i) {
          String arg = remaining_args [i];
          if (arg.Substring (0, 1) == "-") 
            throw new Mono.Options.OptionException (String.Format ("Invalid argument {0}", arg), arg);
        }

        if (show_help) {
          showHelpAndExit ();
        } else if (options.internal_debug == InternalDebugMode.Echo) {
          if (remaining_args.Count > 0)
            Console.Write (remaining_args [0]);
          Environment.Exit (0);
        } else if (options.internal_debug == InternalDebugMode.Test) {
          testInternalDebug ();
        } else if (remaining_args.Count == 0 && options.check_mode) {
          ; // check only : no run()
        } else if (remaining_args.Count == 1) {
          parameters = remaining_args [0];
        }

        bool has_error = false;
        has_error |= configureExecution ();
        if (loader.EnableParallelMode) {
          has_error |= configureParallel ();
        }

        foreach (Application.IAction action in actions) {
          Logger.log (Logger.Channel.Debug, "Starting Configure action for {0} component", action.Name);
          has_error |= action.Configure ();
        }

        if (has_error)
          throw new LauncherException("Application configuration failed");
      } catch (Mono.Options.OptionException e) {
        showHelpAndExit (e.Message);
      } catch (HelpException e) {
        showHelpAndExit (e.Message);
      }
    }

    #region EXECUTION SEQUENCES
    public void Run ()
    {
      try {
        List<Application.IAction>.Enumerator enumerator = actions.GetEnumerator ();
        Application.ActionSequence nextAction = Application.ActionSequence.Continue;
        while (nextAction == Application.ActionSequence.Continue && enumerator.MoveNext()) {
          Logger.log (Logger.Channel.Debug, "Starting Run on {0} component", enumerator.Current.Name);
          nextAction = enumerator.Current.Run (parameters);
          Logger.log (Logger.Channel.Debug, "Returned action after Run action on {0} component is {1}", enumerator.Current.Name, nextAction);
          switch (nextAction) {
          case Application.ActionSequence.Continue:
          // nothing to do
            break;
          case Application.ActionSequence.Stop:
          // stop and stop ok
            break;
          case Application.ActionSequence.Loop:
            enumerator = actions.GetEnumerator ();
            nextAction = Application.ActionSequence.Continue;
            break;
          case Application.ActionSequence.Abort:
            throw new LauncherException ("Run aborted");
          }
        }
      } catch (HelpException e) {
        showHelpAndExit (e.Message);
      }
    }
    #endregion

    private void showHelpAndExit (String message = null)
    {
      StringWriter writer = new StringWriter ();
      if (message == null)
        writer.WriteLine ("Requested Help page");
      else
        writer.WriteLine (message);

      writer.WriteLine ("Usage: {0} [options] [--] [parameter]", Path.GetFileName (System.Diagnostics.Process.GetCurrentProcess ().ProcessName));
      writer.WriteLine ("Options : ");
      opt_set.WriteOptionDescriptions (writer);

      if (message == null)
        throw new ExitException (writer.ToString ());
      else
        throw new LauncherException (writer.ToString ());
    }

    private void configureInternalDebug (string v)
    {
      if (v == null) {
        options.internal_debug = InternalDebugMode.Info;
      } else if (v == "info") {
        options.internal_debug = InternalDebugMode.Info;
      } else if (v == "test") {
        options.internal_debug = InternalDebugMode.Test;
      } else if (v == "echo") {
        options.internal_debug = InternalDebugMode.Echo;
      } else {
        showHelpAndExit (String.Format ("Invalid internal-debug option [{0}]", v));
      }

      if (options.internal_debug == InternalDebugMode.Info)
        Logger.configureEnableDebug (true);
    }

    private bool configureExecution ()
    {
      bool has_error = false;

      if (options.install_path != null) {
        if (!ExecTools.isWindows ()) { // Sous Linux, interprétation de ~ en tant que $HOME
          if (options.install_path.StartsWith ("~")) {
            string home_path = Environment.GetEnvironmentVariable ("HOME");
            if (home_path == null) {
              throw new LauncherException ("Cannot interpret ~ since $HOME is not defined");
            }
            options.install_path = home_path + options.install_path.Substring (1);
          }
        }
        options.rootDir = Path.GetFullPath (options.install_path);
      } else {
        String execFileName = Process.GetCurrentProcess ().MainModule.FileName;
        options.rootDir = Path.GetDirectoryName (execFileName);
      }

      options.binaryDir = Path.Combine (options.rootDir, "bin");

      if (!ExecTools.isWindows ()) {
        string LD_LIBRARY_PATH = Path.Combine (Launcher.Options.rootDir, "lib");
        if (Directory.Exists (LD_LIBRARY_PATH)) {
          options.libDir = LD_LIBRARY_PATH;
        } else if (Launcher.Options.check_mode) {
          Logger.log (Logger.Channel.Error, "Standard library directory '{0}' cannot be found", LD_LIBRARY_PATH);
          has_error = true;
        } else {
          Logger.log (Logger.Channel.Warning, "Standard library directory '{0}' cannot be found", LD_LIBRARY_PATH);
        }
      }

      if (!has_error)
        options.install_path = options.rootDir;
      return has_error;
    }

    private bool configureParallel ()
    {
      Env.CreateTag ("Mpi", "Parallel environment");

      // Prépare mpd.conf (uniquement activé sous Linux)
      if (!ExecTools.isWindows ()) {
        try {
          //.NET 3.5 does not have System.Environment.SpecialFolder.UserProfile
          String home_path = System.Environment.GetFolderPath (System.Environment.SpecialFolder.Personal);
          String path = Path.Combine (home_path, @".mpd.conf"); // TODO: vérifier l'écriture du fichier

          // Delete the file if it exists.
          if (File.Exists (path)) {
            if (options.internal_debug == InternalDebugMode.Info)
              Logger.log (Logger.Channel.Warning, "mpd will use existing " + path + " file");
          } else {
            string secret = "secretword=visco";
            if (options.internal_debug == InternalDebugMode.Info)
              Logger.log (Logger.Channel.Debug, "Setting \"{1}\" into {0}", path, secret);

            //Create the file.
            using (FileStream fs = File.Create(path)) {
              byte[] info = new UTF8Encoding (true).GetBytes (secret);
              fs.Write (info, 0, info.Length);
            }
          }

          int ret = ExecTools.ExecProcess ("/bin/chmod", ExecTools.toArgs ("600", path));
          if (ret != 0) {
            throw new Exception (String.Format ("Cannot update permissions for file {0}", path));
          }

        } catch (Exception ex) {
          throw new LauncherException (String.Format ("MPI configuration failure : {0}", ex.Message));
        }
      }

      // Prépare et teste MPI runtime
      using (Process p = new Process()) {
        p.StartInfo.UseShellExecute = false;

        bool empty_extra_options = (options.mpirun_extra_options.Count == 0);

        string mpirt_exe_dir = null;
        string mpirt_lib_dir = null;
	string libfab_dir = null;
	string fi_prov_dir = null;

        // Handle mpi runtime user location if given
        if (options.mpirt_path != null) {
            mpirt_exe_dir = Path.Combine(options.mpirt_path, "bin");
            mpirt_lib_dir = Path.Combine(options.mpirt_path, "lib;");
            mpirt_lib_dir += Path.Combine(options.mpirt_path, "lib/release;");
            mpirt_lib_dir += Path.Combine(options.mpirt_path, "libfabric/lib");
            Env.Export(Env.GetTag("Mpi"), "LD_LIBRARY_PATH", mpirt_lib_dir);
	    libfab_dir = Path.Combine(options.mpirt_path, "libfabric/lib");
	    Env.Export(Env.GetTag("Mpi"), "LIBRARY_PATH", libfab_dir);
	    fi_prov_dir = Path.Combine(options.mpirt_path, "libfabric/lib/prov");
	    Env.Export(Env.GetTag("Mpi"), "FI_PROVIDER_PATH", fi_prov_dir);
        }

        if (options.mpirun_path != null) {
          if (!ExecTools.isWindows ()) {
            Regex regex = new Regex (@"^(?<cmd>'[^']*'|""[^""]*""|[^""'\s]+)\s*(?<arg>.*)$", RegexOptions.None);
            Match m = regex.Match (options.mpirun_path);
            if (m.Success) {
              options.mpirun_path = m.Groups ["cmd"].Value;
              string arg = m.Groups ["arg"].Value;
              options.mpirun_extra_options.Insert (0, arg);
            } else {
              throw new LauncherException (String.Format ("Cannot parse mpirun command '{0}'", options.mpirun_path));
            }
          }
		  else { // windows
			string mpirt_dir=Path.GetDirectoryName(options.mpirun_path);
			Env.Export(Env.GetTag ("Mpi"), "PATH",mpirt_dir + Path.PathSeparator + Environment.GetEnvironmentVariable ("PATH"));		  
		  }          
          if (File.Exists (options.mpirun_path)) {
            p.StartInfo.FileName = options.mpirun_path;
          } else {
            p.StartInfo.FileName = ExecTools.FindExec (options.mpirun_path);
          }
        } else if (ExecTools.isWindows ()) {
          options.mpirun_path = mpirun_windows_default;
          p.StartInfo.FileName = ExecTools.FindExec (options.mpirun_path);
        } else { // Linux
          string mpirt_dir = (mpirt_exe_dir != null) ? mpirt_exe_dir : Path.Combine (options.install_path, "bin", "mpirt");
          string mpirt_run = Path.Combine (mpirt_dir, mpirun_linux_default);
          if (File.Exists (mpirt_run)) {
            options.mpirun_path = String.Format ("embedded {0}", mpirun_linux_default);
            p.StartInfo.FileName = mpirt_run;
            Env.Export (Env.GetTag ("Mpi"), "I_MPI_ROOT", mpirt_dir);
            Env.Export (Env.GetTag ("Mpi"), "PATH", mpirt_dir + Path.PathSeparator + Environment.GetEnvironmentVariable ("PATH"));
          } else {
            options.mpirun_path = mpirun_linux_default;
            p.StartInfo.FileName = ExecTools.FindExec (options.mpirun_path);
          }
        }

        if (empty_extra_options) {
          // Add default extra options (these options are appended to options provided by mpirun_path command)
          if (ExecTools.isWindows ()) {
            options.mpirun_extra_options.AddRange (mpirun_windows_default_options);
          } else {
            options.mpirun_extra_options.AddRange (mpirun_linux_default_options);
          }
        }

        if (p.StartInfo.FileName == null || !File.Exists (p.StartInfo.FileName)) {
          throw new LauncherException (String.Format ("MPI configuration failure : cannot find MPI launcher [{0}]", options.mpirun_path));
        }

        Env.LoadEnvironment (p.StartInfo, new Env.Tag[] { Env.GetTag ("Mpi") });

        List<string> args = new List<string> ();
        args.Add ("--version");
        args.AddRange (options.mpirun_extra_options);
        p.StartInfo.Arguments = ExecTools.argvToCommandLine (args);

        try {
          if (!options.dry_mode || options.check_mode) {
            if (options.verbose_run)
              Logger.log (Logger.Channel.Verbose, "Execute: {0} {1}", p.StartInfo.FileName, p.StartInfo.Arguments);

            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.RedirectStandardError = true;

            List<string> mpirun_version_output = null;

            if (options.check_mode) {
              mpirun_version_output = new List<string> ();
              p.OutputDataReceived += (sender, arg) => {
                if (arg.Data != null) {
                  Logger.log (Logger.Channel.MPIInfo, arg.Data);
                  mpirun_version_output.Add (arg.Data);
                } };
              p.ErrorDataReceived += (sender, arg) => {
                if (arg.Data != null) {
                  Logger.log (Logger.Channel.MPIInfo, arg.Data);
                } };
            } else {
              p.OutputDataReceived += (sender, arg) => { };
              p.ErrorDataReceived += (sender, arg) => { };
            }

            p.Start ();
            p.BeginOutputReadLine ();
            p.BeginErrorReadLine ();
            p.WaitForExit ();
            int r = p.ExitCode;
            if (r != 0)
              throw new Exception (String.Format ("abnormal return code [{0}]", r));

            // Check intel mpi required
            if (mpirun_version_output != null) {
              Regex regex = new Regex (@"Version (?<major>\d+)\.(?<minor>\d+) Update (?<update>\d+) ", RegexOptions.None);
              foreach (string line in mpirun_version_output) {
                Match m = regex.Match (line);
                if (m.Success) {
                  Version current_version = new Version (String.Format ("{0}.{1}.{2}", 
                                                                      m.Groups ["major"].Value,
                                                                      m.Groups ["minor"].Value,
                                                                      m.Groups ["update"].Value));
                  int comparaison = current_version.CompareTo (mpirun_required_version);
                  if (comparaison < 0) 
                    throw new LauncherException ("MPI version requirement failed : found " + current_version.ToString () + "; required " + mpirun_required_version.ToString ());
                  break;
                }
              }
            }
          } else {
            Logger.log (Logger.Channel.Dry, "Execute; {0} {1}", p.StartInfo.FileName, p.StartInfo.Arguments);
          }
        } catch (Exception e) {
          throw new LauncherException ("MPI configuration failure : " + e.Message);
        }
        options.mpirun_path = p.StartInfo.FileName;
        if (options.check_mode) {
          Logger.log (Logger.Channel.MPIInfo, "Runtime found at location '{0}'", options.mpirun_path);
        }
      }

      if (options.nb_proc <= 0) {
        showHelpAndExit ("Error: nb proc should be >= 1");
      }

      return false; // no configuration error
    }
    #endregion

    private void testInternalDebug ()
    {
      Logger.log (Logger.Channel.Debug, "Testing internal procedures");
      
      Logger.log (Logger.Channel.Debug, "Argument conversion...");
      System.Diagnostics.Process proc = new System.Diagnostics.Process ();
      
      // List<string> names = new List<string>();
      // names.Add (System.Diagnostics.Process.GetCurrentProcess().ProcessName); // native (Assembly); assembly (mono)
      // names.Add (System.Reflection.Assembly.GetExecutingAssembly ().GetName().Name); // ko native (Assembly); ko assembly (Assembly)
      // names.Add (System.Reflection.Assembly.GetExecutingAssembly ().Location); // ko native (Assembly.exe) : ok assembly (Path/Assembly.exe)
      // names.Add (System.Reflection.Assembly.GetEntryAssembly().Location); // similar to GetExecutingAssembly
      // names.Add (System.Reflection.Assembly.GetCallingAssembly().Location); // similar to GetExecutingAssembly
      // names.Add (System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName); // ok pour native (Path/Assembly); ko assembly
      // names.Add (Environment.GetCommandLineArgs()[0]); // ko native (BadPath/Assembly.exe) ; ok assembly
      // names.Add (Path.GetFileName(System.Windows.Forms.Application.ExecutablePath));
      // names.Add (System.AppDomain.CurrentDomain.FriendlyName); // ko native (Assembly.exe) ; ok assembly
      // names.Add (System.Reflection.Assembly.GetExecutingAssembly ().CodeBase); // ko (file://Path/Assembly.exe)
      // names.Add (System.Reflection.Assembly.GetExecutingAssembly ().GetName().FullName); // ko (ArcTemLauncher, Version=1.0.5350.25248 ...)
      // foreach(string s in names) { log (Logger.Channel.Debug, "Name = {0}",s); }
      
      string processPath = System.Diagnostics.Process.GetCurrentProcess ().MainModule.FileName;
      string assemblyLocation = System.Reflection.Assembly.GetExecutingAssembly ().Location;
      bool is_rooted = Path.IsPathRooted (assemblyLocation); // is assemblyLocation defined as a full path
      if (is_rooted)
        proc.StartInfo.FileName = assemblyLocation;
      else
        proc.StartInfo.FileName = processPath;
      
      proc.StartInfo.UseShellExecute = false;
      proc.StartInfo.RedirectStandardOutput = true;
      proc.StartInfo.RedirectStandardError = true;
      string argumentFormat = "--internal-debug=echo {0} MARK";
      
      proc.ErrorDataReceived += (sender, arg) => {
        if (arg.Data != null) {
          Logger.log (Logger.Channel.MPIInfo, arg.Data);
        } };
      
      List<string> tests = new List<string> ();
      if (options.install_path != null)
        tests.Add (options.install_path);
      if (Options.mpirun_path != null)
        tests.Add (Options.mpirun_path);
      
      // Select converter for current OS
      ExecTools.ArgConverter converter = ExecTools.converter ();
      
      
      // http://msdn.microsoft.com/en-us/library/system.environment.getcommandlineargs.aspx
      // http://blogs.msdn.com/b/twistylittlepassagesallalike/archive/2011/04/23/everyone-quotes-arguments-the-wrong-way.aspx
      // http://blogs.msdn.com/b/oldnewthing/archive/2010/09/17/10063629.aspx
      tests.Add ("foobar");
      tests.Add ("alpha with spaces");
      tests.Add ("foo\"bar");
      tests.Add ("foo'bar");
      tests.Add (@"'alpha");
      tests.Add (@"beta'");
      tests.Add (@"\alpha");
      tests.Add (@"\\alpha");
      tests.Add (@"\\\alpha");
      tests.Add (@"\\beta");
      tests.Add (@"\\""alpha");
      tests.Add (@"""beta");
      tests.Add ("a\"b");
      tests.Add ("a\"b\"");
      tests.Add ("a\"b\" ");
      tests.Add ("a\"b\" c");
      tests.Add ("a;b&c");
      tests.Add (" @a!b<c|\t&d&\t|>c!b@a ");
      tests.Add (@"malicious-argument^&whoami");
      tests.Add (@"malicious-argument^""&whoami");
      tests.Add (@"malicious-argument\^&whoami");
      tests.Add (@"^""malicious-argument\^""&whoami^""");
      tests.Add (@"C:\Program Files\Test");
      tests.Add ("/path/with spaces");
      
      Logger.log (Logger.Channel.Debug, "\tTest   : [data] => [converted] => [check data]");
      bool has_failed = false;
      foreach (string key  in tests) {
        proc.StartInfo.Arguments = String.Format (argumentFormat, converter (key));
        try {
          proc.Start ();
          string output = proc.StandardOutput.ReadToEnd ();
          string diagnostic = "Passed";
          if (key != output) {
            diagnostic = "FAILED";
            has_failed = true;
          }
          Logger.log (Logger.Channel.Debug, "\t{3} : [{0}] => [{1}] => [{2}]", key, converter (key), output, diagnostic);
        } catch (Exception e) {
          has_failed = true;
          Logger.log (Logger.Channel.Debug, "\tFAILED => [{0}] => [{1}] : {2}", key, converter (key), e.Message);
        }
      }
      if (has_failed)
        throw new LauncherException ("Internal Debug Test has failed");
      else
        throw new ExitException ("Internal Debug Test finished successfully");
    }

  }
}
