using System;
using System.Collections.Generic;
using System.IO;
using ApplicationLauncher;

namespace Application
{
  public interface ILoader {
    ICollection<IAction> Load ();
    bool EnableParallelMode { get; }
  }

  public enum ActionSequence { Continue, Stop, Loop, Abort };
  
  public interface IAction
  {
    void Setup(ref Mono.Options.OptionSet opt_set);
    bool Configure(); // return true on error while configuring
    ActionSequence Run(string parameters);
    string Name { get; }
  }
  
	public class Application : IAction 
	{
		#region CONFIGURATIONS
    private string APPLICATION_BINARY = null;
    private ApplicationConfiguration configuration;
		#endregion

    public Application(ApplicationConfiguration configuration) 
    {
      this.configuration = configuration;
      // Disables that default variable (may be confusing for sequential run)
      Environment.SetEnvironmentVariable ("ARCANE_PARALLEL_SERVICE", null);
    }

 		public virtual void Setup(ref Mono.Options.OptionSet opt_set) 
		{
      Env.CreateTag ("Run", "Default execution environment");
      Env.CreateTag ("Simulation", "Simulation execution environment");
    }

    public virtual bool Configure() 
		{ 
			bool has_error = false;
			APPLICATION_BINARY = Path.Combine(Launcher.Options.binaryDir, configuration.Name + ".exe");
			if (!File.Exists (APPLICATION_BINARY)) {
				Logger.log(Logger.Channel.Error, "Cannot access Application executable in '{0}'", APPLICATION_BINARY);
        has_error = true;
			} else if (Launcher.Options.check_mode) {
				Logger.log(Logger.Channel.Info,"Application found at location : " + APPLICATION_BINARY);
			}

			if (!ExecTools.isWindows () && Launcher.Options.libDir != null) {
				string LD_LIBRARY_PATH = Launcher.Options.libDir;
				Env.Export (Env.GetTag("Run"), "LD_LIBRARY_PATH", LD_LIBRARY_PATH);
			}

      if (configuration.ConfigLocation == ConfigLocation.ApplicationSide) {
        string STDENV_PATH_SHR = Launcher.Options.binaryDir;
        string STDENV_PATH_SHR_FILE = Path.Combine (STDENV_PATH_SHR, String.Format ("{0}.config", configuration.Name));
        if (File.Exists (STDENV_PATH_SHR_FILE)) {
          Env.Export (Env.GetTag ("Run"), "STDENV_PATH_SHR", STDENV_PATH_SHR);
        } else if (Launcher.Options.check_mode) {
          Logger.log (Logger.Channel.Error, "Standard config file '{0}' cannot be found", STDENV_PATH_SHR_FILE);
          has_error = true;
        } else {
          Logger.log (Logger.Channel.Warning, "Standard config file '{0}' cannot be found", STDENV_PATH_SHR_FILE);
        }
      }
			
			has_error |= configureVerboseRun ();
      return has_error;
		}

    public virtual ActionSequence Run(string parameters) 
		{ 
      Logger.log (Logger.Channel.Banner, "Simulation mode");
      int? nproc = null;
      if (configuration.EnableParallelMode) {
        nproc = Launcher.Options.nb_proc;
      } else {
        if (Launcher.Options.nb_proc > 1)
          throw new ApplicationLauncher.InternalException ("Illegal #proc on non-parallel application");
      }
      int res = runApplication (nproc, parameters, Env.GetTag ("Simulation"));

      if (res != 0) {
        Logger.log ("Simulation", "-------------------------------------------------------------");
        Logger.log ("Simulation", "An errors occurs in simulation mode with : see log for details [{0}]", res);
        throw new LauncherException ("An error occurs while running simulation");
      }
			return ActionSequence.Continue;
		}

    public string Name { get { return configuration.Name; } }

		private bool configureVerboseRun()
		{
			if (Launcher.Options.verbose_run) {
				Env.Export (Env.GetTag("Run"), "ARCANE_TRACE_TIMER", "TRUE");
			} else {
				Env.Export (Env.GetTag("Run"), "ARCANE_TRACE_TIMER", null);
			}
      return false;
		}

		public int runApplication(int? n, string arg, Env.Tag mainTag)
		{
			string cmd;
			List<string> cmd_args = new List<string> ();

			List<Env.Tag> tags = new List<Env.Tag> ();
			tags.Add (mainTag);
			tags.Add (Env.GetTag("Run"));

			if (n != null) {
				cmd = Launcher.Options.mpirun_path;
				cmd_args.AddRange(Launcher.Options.mpirun_extra_options);
				cmd_args.AddRange(ExecTools.toArgs( "-np", n ));
				cmd_args.AddRange(ExecTools.toArgs( "-genv", "ARCANE_PARALLEL_SERVICE", "Mpi" ));
				if (ExecTools.isWindows()) cmd_args.Add("-genvall");
				cmd_args.Add(APPLICATION_BINARY);
				cmd_args.Add (arg);
				tags.Add (Env.GetTag("Mpi"));
			} else {
				cmd = APPLICATION_BINARY;
				cmd_args.Add (arg);
				tags.Add (Env.GetTag("Mpi"));
			}

			// Cleanup fatal file before running
			foreach (FileInfo f in GetFatalFiles()) {
				f.Delete ();
			}

			int res = ExecTools.ExecProcess (cmd, cmd_args.ToArray(), tags.ToArray());
			/*
			if (res == 0 && GetFatalFiles().Length > 0) {
				// TODO: voir problème de fatal_6 en exécution parallèle
				// fatal_6 en cas de STOP_FAILED (ex: erreur de validateurs même non fatal; voir src/ArcTem/ArcTemModule.cc)
				logger.log(Logger.Channel.eError, "Fatal file detected : throws an error");
				return -1;
			} else {
				return res;
			}
			*/
			return res;
		}

		FileInfo[] GetFatalFiles() {
			FileInfo[] fatal_files = new DirectoryInfo(Directory.GetCurrentDirectory()).GetFiles("fatal_*");
			return fatal_files;
		}
  }
}

