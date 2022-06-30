using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.Xml.Linq;
using System.IO;
using System.ComponentModel;
using System.Text.RegularExpressions;
using ArcaneInfra.Common;

// TODO Ajouter log() mais avec un dictionnaire int -> string en clef
// ou bien un Generic pour indexer avec un enum + dictionnaire enum -> string
// TODO ajouter une completion contextuelle pour zsh
// TODO interdire le croisement Release / Debug entre la configuration et la compilation Windows
// Check if arcane-path of package paths exists before running CMake
/* Ajouter une norme d'info
    -X   specify a mandatory option
    [-X] specify a optional option
    <X>  must be replaced by a numerical or string value.
    X|Y  specify a choice beetween X and Y
*/


namespace ArcaneInfra.Configure
{
  public class Configurator
  {
    #region CONFIGURATIONS
    string codename = "Configurator.exe";
    string vendor_tag = "vendor";
    string windows_compile_command = "devenv %p.sln /build %m";
    string windows_test_command = "ctest -C %m";
    string linux_compile_command = "make -j 4";
    string linux_test_command = "ctest";
    string default_log_format = "%t.log";
    #endregion

    #region OPTIONS WITH DEFAULT VALUES
    bool show_help = false;
    bool new_build_system = true;
    bool release_mode = false;
    bool? static_mode = null;
    bool mpi_mode = true;
    bool dry_mode = false;
    bool sublime_generator = false;
    bool eclipse_generator = false;
    bool ninja_generator = false;
    string opt_arcane = null;
    string opt_arcane_path = null;
    string opt_arcgeosim_package_file = null;
    string opt_base_command = null;
    string opt_root_path = null;
    string opt_build_path = null;
    string opt_install_prefix = null;
    string opt_vstudio_name = null;
    string opt_toolset_name = null;
    string opt_graphviz = null;
    bool clean_cache = true;
    string opt_clean = null;
    string opt_cmake_extra_options = null;
    List<string> meta_tags = new List<string> ();
    Dictionary<string,bool> packages = new Dictionary<string,bool> ();
    string default_fortran_compiler = null;
    bool do_compile = false;
    bool do_test = false;
    string do_log_format = null;
    #endregion

    #region MEMBERS
    private DateTime m_date_time;
    private string m_project_name;
    private CommonTools m_common_tools = null;
    private Mono.Options.OptionSet m_options = null;
    private string m_current_compile_command;
    private string m_current_test_command;
    private string[] external_envs = { "CC", "CFLAFS", "CXX", "CXXFLAGS", "FC", "FFLAGS" };
    #endregion

    #region EXCEPTION
    [Serializable]
    public class ConfiguratorException : Exception
    {
      public ConfiguratorException ()
      {
      }
      
      public ConfiguratorException (string message)
        : base(message)
      {
        ;
      }
    }
    
    [Serializable]
    public class ConfiguratorExit : Exception
    {
      public ConfiguratorExit ()
      {
      }
      
      public ConfiguratorExit (string message)
        : base(message)
      {
        ;
      }
    }
    #endregion

    #region Logger
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

    #region CONSTRUCTOR
    public Configurator ()
    {
      m_common_tools = new CommonTools ();
      m_options = new Mono.Options.OptionSet ();
      m_date_time = DateTime.Now;
      if (m_common_tools.isWindows ()) { // Windows
        m_current_compile_command = windows_compile_command;
        m_current_test_command = windows_test_command;
      } else { // Linux
        m_current_compile_command = linux_compile_command;
        m_current_test_command = linux_test_command;
      }

      // Console.WriteLine ("Starting configure at {0}", m_date_time.ToString ("s"));

      String domain_name = System.Net.NetworkInformation.IPGlobalProperties.GetIPGlobalProperties().DomainName;
      if (Regex.IsMatch(domain_name, @"^ifp(.fr)?$"))
      {
          opt_vstudio_name = "Visual Studio 12 Win64";
          default_fortran_compiler = "ifort";
      }

      setupOptions ();
    }
    #endregion

    #region OPTIONS
    private void setupOptions ()
    {
      m_options.Add ("d|debug", "Debug mode" + ((!release_mode) ? " (default)" : ""), 
                     v => { release_mode = false; });
      m_options.Add ("r|release", "Release mode" + ((release_mode) ? " (default)" : ""), 
                     v => { release_mode = true; });
      m_options.Add ("vendor", "Vendor mode\nThis option will enable vendor meta tag in configuration and checks some restrictions for software diffusion", 
                     v => { meta_tags.Add (vendor_tag); });

      if (m_common_tools.isWindows ()) { // Windows
        m_options.Add ("visual=", "Visual Studio name " + ((opt_vstudio_name != null)?String.Format("(default is '{0}')", opt_vstudio_name):("required")),
                       (string s) => { opt_vstudio_name = s; });
        m_options.Add ("toolset=", "Toolset for compiling (default value is provided by Visual Studio)", 
                       (string s) => { opt_toolset_name = s; });
      } else {
        m_options.Add ("sublime", "Sublime text + Makefile generator ",
                       v => { sublime_generator = true; });
        m_options.Add ("eclipse", "Eclipse + Makefile generator ",
                       v => { eclipse_generator = true; });
        m_options.Add ("ninja", "Ninja generator ",
                       v => { ninja_generator = true; });
      }

      // TODO
      // m_options.Add ("option=TYPE", "description", v => { /* ACTION */});
      // m_options.AddComment("Comment");
      m_options.Add ("new|new-build-system", "Enable new build system (default)", 
                     v => { new_build_system = true; });     
      m_options.Add ("old|old-build-system", "Disable new build system", 
                     v => { new_build_system = false; });     
      m_options.Add ("enable-shared", "Shared libraries compilation mode", 
                     v => { static_mode = false; });
      m_options.Add ("enable-static", "Static libraries compilation mode (opposite option of --enable-shared)", 
                     v => { static_mode = true; });
      m_options.Add ("prefix=", "Define install path", 
                     (string s) => { opt_install_prefix = normalizePath (s); });
      m_options.Add ("package=", "Define package absolute path (override ArcGeoSim default)", 
                     (string s) => { opt_arcgeosim_package_file = normalizePath (s); });
      m_options.Add ("arcane=", "Define local arcane version (override ArcGeoSim default)", 
                     (string s) => { opt_arcane = s; });
      m_options.Add ("arcane-path=", "Define local arcane absolute path (override ArcGeoSim default)", 
                     (string s) => { opt_arcane_path = normalizePath (s); });
      m_options.Add ("disable-mpi", "MPI configuration (default is enabled)",  
                     v => { mpi_mode = false; });
      m_options.Add ("v|verbose", "Enable verbose output", 
                     v => { GlobalContext.Instance.Verbose = true; });
      m_options.Add ("h|help|?", "Help page\n\n** Experimental features **", 
                     v => { show_help = true; });

      m_options.Add ("compile:", String.Format("compile after configuration (default command is '{0}')", m_current_compile_command),
                     (string s) => { do_compile = true; if (s != null) m_current_compile_command = s; });
      m_options.Add ("test:", String.Format("test after configuration and compilation (default command is '{0}')", m_current_test_command),
                     (string s) => { do_compile = do_test = true; if (s != null) m_current_test_command = s; });
      m_options.Add ("log:", "define log format (no value means default as '%t.log')\n%p is project name; %t is task name (compile, test...); %d is date; %m is mode\n\n** Advanced features **",
                     (string s) => { if (String.IsNullOrEmpty(s)) do_log_format = default_log_format; else do_log_format = s; });

      m_options.Add ("dry", "Dry mode; do not execute cmake commands and print them", 
                     v => { dry_mode = true; });
      m_options.Add ("meta=", "Enable meta tag (tag will appear lowercase in configuration files)", 
                     (string s) => { meta_tags.Add (s); });
      m_options.Add ("enable=", "Enable package (package name will appear lowercase in configuration files)", 
                     (string s) => { packages[s.ToLower()] = true; });
      m_options.Add ("disable=", "Disable package (package name will appear lowercase in configuration files)\nEnabled/disabled package force configuration ", 
                     (string s) => { packages[s.ToLower()] = false; });
      m_options.Add ("keep-cache", "Do not clean cache", 
                     v => { clean_cache = false; });
      m_options.Add ("clean-cache", "Clean cmake cache (default)", 
                     v => { clean_cache = true; });
      m_options.Add ("clean:", "Clean everything before running configure. Valid values are 'check', 'once' and 'force'\n(default is 'check' which checks if current directory looks like a configured build directory)", 
                     (string s) => { opt_clean = (s == null) ? "check" : s; });
      m_options.Add ("force-regenerate:", "deprecated old clean option name; you have to use --clean",
                     (string s) => { opt_clean = (s == null) ? "check" : s; Console.WriteLine("WARNING: --force-regenerate is deprecated; use --clean instead"); });
      m_options.Add ("cmake-extra-options=", "Add extra options to CMake\n\n** Internal features **",
                     (string s) => { opt_cmake_extra_options = s; });

      m_options.Add ("base-command=", "Define user command used to run that configure (required but automatically set by script wrapper)", 
                     (string s) => { opt_base_command = s; });
      m_options.Add ("distrib-path=", "Define project root directory (required if base-command if not set)\nDEPRECATED: will be replaced by --base-command", 
                     (string s) => { opt_root_path = normalizePath (s); });

      m_options.Add ("graphviz=", "Define dot file containing dependencies of cmake target", 
                     (string s) => { opt_graphviz = s; });

    }
    #endregion 

    #region EXECUTION SEQUENCES
    public void configure (string[] args)
    {
      string[] saved_args = args;

      try {
        List<string> remaining_args = m_options.Parse (args);
        
        // Check if remaining args looks like options
        for (int i=0; i<remaining_args.Count; ++i) {
          String arg = remaining_args [i];
          if (arg.StartsWith ("-")) 
            throw new Mono.Options.OptionException (String.Format ("Invalid option {0}", arg), arg);
          else if (arg.Length != 0)
            throw new Mono.Options.OptionException (String.Format ("Invalid argument {0}", arg), arg);
        }
        
        if (show_help) {
          showHelpAndExit ();
        }
      } catch (Mono.Options.OptionException e) {
        showHelpAndExit (e.Message);
      }

      // Force tower and remove duplicated tags
      meta_tags = meta_tags.GroupBy (tag => tag.ToLower ()).Select (grp => grp.First ()).ToList ();

      // vendor mode may be enabled using vendor tag (default or in lowercase).
      bool vendor_mode = meta_tags.Contains (vendor_tag);

      if (opt_base_command != null) {
        if (opt_root_path != null)
          Console.WriteLine ("WARNING : overriding --distrib-path option using --base-command option");
        opt_root_path = Path.GetDirectoryName (opt_base_command);
      }
      if (opt_root_path == null) {
        showHelpAndExit ("Undefined required option --distrib-path");
      }

      opt_build_path = Environment.CurrentDirectory;

      if (String.IsNullOrEmpty (Environment.GetEnvironmentVariable ("FC")) && default_fortran_compiler != null) {
        Environment.SetEnvironmentVariable ("FC", default_fortran_compiler);
        if (GlobalContext.Instance.Verbose)
          Console.WriteLine ("Default FC compiler set to '{0}'", default_fortran_compiler);
      }
      
      if (m_common_tools.isWindows ()) { // Windows
        if (opt_vstudio_name == null) {
          showHelpAndExit ("Undefined required option --vstudio-name");
        }
      }

      // Check before cmake ?
      if (opt_arcane_path != null && !Directory.Exists (opt_arcane_path))
        throw new ConfiguratorException (String.Format ("Cannot access to Arcane directory '{0}'", opt_arcane_path));
      if (opt_arcgeosim_package_file != null && !File.Exists (opt_arcgeosim_package_file))
        throw new ConfiguratorException (String.Format ("Cannot access to ArcGeoSim package file '{0}'", opt_arcgeosim_package_file));

      if(new_build_system) {
        string vendor_tag = (vendor_mode) ? "+VENDOR" : "";
        if (opt_arcgeosim_package_file == null)
        {
          if (m_common_tools.isWindows())
          {
            opt_arcgeosim_package_file = "../common/ArcaneInfra/BuildSystem/configs/IFP-Windows_6.1-AMD64" + vendor_tag + ".cmake";
          }
          else
          {
            opt_arcgeosim_package_file = "../common/ArcaneInfra/BuildSystem/configs/IFP-Linux-x86_64" + vendor_tag + ".cmake";
          }
        }
      }

      // Configure default static_mode if not set
      if (static_mode == null) {
        if (new_build_system) {
          if (m_common_tools.isWindows ()) {
            static_mode = false;
          } else {
            static_mode = false;
          }
        } else {
          if (m_common_tools.isWindows ()) {
            static_mode = true;
          } else {
            static_mode = false;
          }
        }
        if (GlobalContext.Instance.Verbose)
          Console.WriteLine ("{0} library mode enabled as default value", ((bool)static_mode ? "Static" : "Shared"));
      }

      // Nettoyage préalable si requis par clean
      if (opt_clean != null) {
        if (opt_clean != "check" && opt_clean != "force" && opt_clean != "once")
          showHelpAndExit (String.Format ("Unknown --clean option '{0}'", opt_clean));

        DirectoryInfo build_dir = new DirectoryInfo (opt_build_path);
        if (opt_clean == "check" || opt_clean == "once") {
          if (!File.Exists ("cmake.configure"))
            throw new ConfiguratorException ("Cannot destroy directory since it does not look likde a configured directory with 'cmake.configure' file");
        }

        Console.WriteLine ("Cleaning build directory");
        foreach (FileInfo file in build_dir.GetFiles()) {
          file.Delete ();
        }
        foreach (DirectoryInfo directory in build_dir.GetDirectories()) {
          directory.Delete (true);
        }
      }

      // Ecrit des fichiers pour rejouer la configuration
      if (opt_base_command != null)
        writeReplay (opt_base_command, saved_args);
      else
        Console.WriteLine ("WARNING: replay files will not created due to missing --base-command option");

      if (clean_cache) {
        Console.WriteLine ("Cleaning cache");
        string cache_filename = "CMakeCache.txt";
        if (File.Exists (cache_filename))
          File.Delete (cache_filename);
      }

      // Le fichier contenant les infos de packages est passé par variable d'environnement
      // TODO : pourquoi ?
      if (opt_arcgeosim_package_file != null) {
        Environment.SetEnvironmentVariable ("ARCGEOSIM_PACKAGE_FILE", opt_arcgeosim_package_file);
      }
      
      // Configuration cache file
      string config_file_str = "# WARNING Auto generated configuration file. Do not edit unless you know what you are doing\n";
      config_file_str += "# configuration data\n";
      if (new_build_system) {
        config_file_str += String.Format ("set(CMAKE_INSTALL_PREFIX \"{0}\" CACHE PATH \"Installation directory\" FORCE)\n", opt_install_prefix);
      } else {
        config_file_str += String.Format ("set(CMAKE_CONFIGURATION_TYPES \"{0}\" CACHE TYPE INTERNAL FORCE)\n", releaseMode ()); // restrict allowed configuration to selected mode
        config_file_str += String.Format ("set(BUILD_SHARED_LIBS \"{0}\" CACHE BOOL \"Link configuration\" FORCE)\n", (bool)static_mode ? "OFF" : "ON");
        config_file_str += String.Format ("set(ARCGEOSIM_MPI \"{0}\" CACHE PATH \"Enable MPI configuration\" FORCE)\n", mpi_mode ? "TRUE" : "FALSE");
        config_file_str += String.Format ("set(VERBOSE \"{0}\" CACHE BOOL \"Verbosity\" FORCE)\n", GlobalContext.Instance.Verbose ? "TRUE" : "FALSE");
        config_file_str += String.Format ("set(CMAKE_BUILD_TYPE \"{0}\" CACHE BOOL \"Build configuration\" FORCE)\n", releaseMode ());
        config_file_str += String.Format ("set(CMAKE_INSTALL_PREFIX \"{0}\" CACHE PATH \"Installation directory\" FORCE)\n", opt_install_prefix);
        config_file_str += String.Format ("set(ARCGEOSIM_ARCANE_VERSION \"{0}\" CACHE PATH \"User Arcane version\" FORCE)\n", opt_arcane);
        config_file_str += String.Format ("set(ARCGEOSIM_ARCANE_PATH \"{0}\" CACHE PATH \"User Arcane path\" FORCE)\n", opt_arcane_path);
        config_file_str += String.Format ("set(ARCGEOSIM_PACKAGE_FILE \"{0}\" CACHE PATH \"User package file\" FORCE)\n", opt_arcgeosim_package_file);
        /* Not used since environment variable seems to be ok with Windows   
         * foreach (string compiler_env in compiler_envs)
              {
                  string s = Environment.GetEnvironmentVariable(compiler_env);
                  if (!String.IsNullOrEmpty(s))
                  {
                      config_file_str += String.Format ("set({0} \"{1}\")\n",compiler_env,s);
                  }
              }
         */
        foreach (string tag in meta_tags) {
          string cmake_tag = convertTagToCMake (tag);
          config_file_str += String.Format ("set({0}_FOUND \"TRUE\" CACHE BOOL \"{1} tag enabled\" FORCE)\n", cmake_tag, tag);
          config_file_str += String.Format ("set({0}_NAME \"{1}\" CACHE STRING \"{1} Mode enabled\" FORCE)\n", cmake_tag, tag);
        }

        foreach (KeyValuePair<string,bool> kvp in packages) {
          string tag = kvp.Key;
          string cmake_tag = convertTagToCMake (tag);
          config_file_str += String.Format ("set({0}_DISABLED \"{2}\" CACHE BOOL \"{1} package disabled\" FORCE)\n", cmake_tag, tag, (!kvp.Value) ? "TRUE" : "FALSE");
        }

        config_file_str += String.Format ("set(REQUIRED_DEPENDENCIES \"{0}\" CACHE LIST \"Initial required dependencies\")\n", 
                                       String.Join (";", meta_tags.ConvertAll (x => convertTagToCMake (x)).ToArray ()));

        if (m_common_tools.isWindows ()) { // Windows
          config_file_str += String.Format ("set(WIN32_PATCH_FILE \"patch_windows_for_vcproj.bat\" CACHE PATH \"User package file\" FORCE)\n");
          config_file_str += String.Format ("set(VISUAL_STUDIO_NAME \"{0}\" CACHE PATH \"Visual studio name\" FORCE)\n", opt_vstudio_name);
        }
      }
      config_file_str += "# end of configuration data\n";
      System.IO.File.WriteAllText ("cmake.configure", config_file_str);

      string cmake_args = "";
      cmake_args += " -Wno-dev";
      if (m_common_tools.isWindows ()) { // Windows
        cmake_args += " -G \"" + opt_vstudio_name + "\"";
        if (opt_toolset_name != null)
          cmake_args += " -T \"" + opt_toolset_name + "\"";
      } else {
        // cmake_args += " -DCMAKE_EXPORT_COMPILE_COMMANDS=ON";
        if (sublime_generator && eclipse_generator) {
          showHelpAndExit ("Both options --sublime and --eclipse enabled");
        }
        if (sublime_generator) {
          cmake_args += " -G \"Sublime Text 2 - Unix Makefiles\"";
        }
        if (eclipse_generator) {
          cmake_args += " -G \"Eclipse CDT4 - Unix Makefiles\"";
        }
        if (ninja_generator) {
          cmake_args += " -G \"Ninja\"";
        }
      }

      if (opt_graphviz != null) {
        cmake_args += " --graphviz=" + opt_graphviz;
      }
    
      if (new_build_system) {

        cmake_args += " -DNewBuildSystem=ON";

        if (GlobalContext.Instance.Verbose)
          cmake_args += " -DVerbose=ON";

        cmake_args += " -DDebug=" + (release_mode?"OFF":"ON");
        cmake_args += " -DShared=" + ((bool)static_mode?"OFF":"ON");

        if (meta_tags.Count > 0)
          cmake_args += " -DEnableMetas=\"" + String.Join(";",meta_tags.ToArray()) + "\"";
        IEnumerable<String> enabled_packages = packages.Where( v => v.Value == true).Select(v => v.Key);
        if (enabled_packages.Count() > 0)
          cmake_args += " -DEnablePackages=" + String.Join(";",enabled_packages.ToArray());
        IEnumerable<String> disabled_packages = packages.Where( v => v.Value == false).Select(v => v.Key);
        if (disabled_packages.Count() > 0)
          cmake_args += " -DDisablePackages=" + String.Join(";",disabled_packages.ToArray());

        if(opt_arcane != null)
          cmake_args += " -DArcaneVersion=" + opt_arcane;
        if(opt_arcane_path != null)
          cmake_args += " -DArcanePath=" + opt_arcane_path;
        if(opt_arcgeosim_package_file != null)
          cmake_args += " -DPackageFile=" + opt_arcgeosim_package_file;
      }
      cmake_args += " -Ccmake.configure";

      if (opt_cmake_extra_options != null)
        cmake_args += " " + opt_cmake_extra_options;
      cmake_args += " -- " + opt_root_path;
      //TODO proteger les arguments qui contiennent des espaces
      int r = ExecProcess ("cmake", cmake_args);
      if (r != 0) {
        throw new ConfiguratorException ("cmake command has failed");
      }

      if (m_common_tools.isWindows ()) { // Windows
          if (!new_build_system && (bool)static_mode) {
          Console.WriteLine ("Parsing vcproj file");
          r = ExecProcess ("patch_windows_for_vcproj.bat", null);
          if (r != 0) {
            throw new ConfiguratorException ("patch_windows_for_vcproj has failed");
          }
        }
      
        Console.WriteLine ("Executing post cmake commands");

        if (new_build_system) {
          Console.WriteLine ("** TODO ** Needs to rebuild bin directory **");
        } else {
          string pkg_path = Path.Combine (opt_build_path, "pkglist.xml");
          PkglistParser pkg_parser = new PkglistParser ();
          pkg_parser.Parse (pkg_path);
          string sys_copy_path = Path.Combine (opt_build_path, "bin");
          Console.WriteLine ("Rebuild bin directory '{0}'", sys_copy_path);
          //if (Directory.Exists(sys_copy_path)) {
          //  // Supprime les dll du repertoire de destination
          //  foreach (string f in Directory.GetFiles(sys_copy_path, "*.dll")) {
          //    if (GlobalContext.Instance.Verbose)
          //      Console.WriteLine("Removing dll '{0}'", f);
          //    File.Delete(f);
          //  }
          //}
          Directory.CreateDirectory (sys_copy_path);
          foreach (string dir in pkg_parser.Directories) {
            if (!Directory.Exists (dir)) {
              if (GlobalContext.Instance.Verbose)
                Console.WriteLine ("Skip non-existing directory '{0}'", dir);
              continue;
            } else {
              if (GlobalContext.Instance.Verbose)
                Console.WriteLine ("Copy DLLs from directory '{0}'", dir);
            }
            string[] files = Directory.GetFiles (dir, "*.dll");
            foreach (string f in files) {
              string copy_path = Path.Combine (sys_copy_path, Path.GetFileName (f));
              if (GlobalContext.Instance.Verbose)
                Console.WriteLine ("\tCopy '{0}' to '{1}'", f, copy_path);
              File.Copy (f, copy_path, true);
            }
          }
        }
      }

      // Since CMake is done, we can get project name from CMakeCache.txt
      setProjectName ();

      if (do_compile) {
        string log_file = parseFormat(do_log_format,"compile");
        string command = parseFormat(m_current_compile_command,"compile");
        doThis (command, log_file, "Compiling");
      }

      if (do_test) {
        string log_file = parseFormat (do_log_format, "test");
        string command = parseFormat(m_current_test_command,"test");
        doThis (command, log_file, "Testing");
      }
   
      if (vendor_mode && !new_build_system) {
        if (release_mode == false)
          Console.WriteLine ("WARNING: Vendor configuration without --release option");
        VendorChecker vendorChecker = new VendorChecker ();
        string pkg_path = Path.Combine (opt_build_path, "pkglist.xml");
        // TODO: regarder aussi le pkglist.xml de arcane
        vendorChecker.Check (pkg_path);
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
      
      writer.WriteLine ("Usage: {0} [options]", codename);
      writer.WriteLine ("Options : ");
      m_options.WriteOptionDescriptions (writer);
      writer.WriteLine ("\nNOTA: --base-command, --clean=once, --force-regenerate=once options are not replicated in replay configuration scripts");
      if (default_fortran_compiler != null)
        writer.WriteLine ("    : Default Fortran compiler is '{0}'", default_fortran_compiler);
  	  writer.WriteLine ("    : Configure catch following environment variables : {0}", String.Join (" ", external_envs));

      if (message == null)
        throw new ConfiguratorExit (writer.ToString ());
      else
        throw new ConfiguratorException (writer.ToString ());
    }

    int ExecProcess (string cmd, string args)
    {
      if (GlobalContext.Instance.Verbose)
        Console.WriteLine ("Launching process cmd={0} args={1}", cmd, args);

      if (dry_mode) {
        Console.WriteLine ("> {0} {1}", cmd, args);
        return 0;
      }

      int r = 0;
      using (Process p = new Process()) {
        p.StartInfo.UseShellExecute = false;
        p.StartInfo.FileName = cmd;
        p.StartInfo.Arguments = args;
        try {
          p.Start ();
          p.WaitForExit ();
          r = p.ExitCode;
          if (GlobalContext.Instance.Verbose)
            Console.WriteLine ("Process {0} exit code={1}", cmd, r);
        } catch (Win32Exception e) {
          Console.Error.WriteLine ("Error '{0}' has been occurred while executing command:\n\t{1} {2}", e.Message, cmd, args);
          r = -1;
        }
      }
      return r;
    }

    private void writeReplay (string this_command, string[] args)
    {
      Regex ValueOption = new Regex (@"^(?<flag>--|-|/)(?<name>[^:=]+)((?<sep>[:=])(?<value>.*))?$"); // Identique à Mono_Options.cs

      StringBuilder cmd = new StringBuilder();
      cmd.AppendFormat("{0}", this_command);
      List<string> arg_list = new List<string> (args);
      foreach (string loop_arg in arg_list.Distinct()) {
        string arg = loop_arg;
        Match m = ValueOption.Match (arg);
        if (m.Success) {
          string flag = m.Groups ["flag"].Value;
          string name = m.Groups ["name"].Value;
          if (name == "base-command")
            continue; // skip --base-command option
          if (m.Groups ["sep"].Success && m.Groups ["value"].Success) {
            string sep = m.Groups ["sep"].Value;
            string value = m.Groups ["value"].Value;
            if (name == "force-regenerate" && value == "once")
              continue; // skip --force-regenerate=once
            if (name == "clean" && value == "once")
              continue; // skip --clean=once
            arg = flag + name + sep + '"' + value + '"';
          } else {
            arg = flag + name;
          }
        } else {
          arg = '"' + arg + "'";
        }
        cmd.AppendFormat (" {0}", arg);
      }

      string force_regenerate_script_option = " --clean=once";
      if (opt_clean != null && opt_clean != "once") {
        force_regenerate_script_option = null;
      }

      if (m_common_tools.isWindows ()) { // Windows
        // More infos in special variables: http://www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/percent.mspx?mfr=true
        cmd.Append (" %*"); // add extra args
        List<string> envs = new List<string>();
        foreach (string comp in external_envs)
        {
            string comp_cmd = Environment.GetEnvironmentVariable(comp);
            if (!String.IsNullOrEmpty(comp_cmd))
                envs.Add(String.Format("set {0}={1}", comp, comp_cmd));
        }
        {
          string path = "reconfigure.bat";
          using (TextWriter writer = new StreamWriter(path)) {
            writer.WriteLine ("@echo off");
            foreach (string s in envs)
                writer.WriteLine("echo \"Environment: {0}\"", s);
            writer.WriteLine("echo \"Execute: {0}\"",cmd.ToString());
            foreach (string s in envs)
                writer.WriteLine(s);
            writer.WriteLine(cmd.ToString());
          }
        }
        
        {
          string path = "force-regenerate.bat";
          using (TextWriter writer = new StreamWriter(path)) {
            writer.WriteLine ("@echo off");
            foreach (string s in envs)
                writer.WriteLine("echo \"Environment: {0}\"", s);
            writer.WriteLine("echo \"Destroy all and execute: {0}\"{1}", cmd.ToString(), force_regenerate_script_option);
            foreach (string s in envs)
                writer.WriteLine(s);
            writer.WriteLine("{0}{1}", cmd.ToString(), force_regenerate_script_option);
          }
        }
      } else { // Linux
        cmd.Append (" $@"); // add extra args
        string env = "";
        foreach (string comp in external_envs)
        {
            string comp_cmd = Environment.GetEnvironmentVariable(comp);
            if (!String.IsNullOrEmpty(comp_cmd))
                env += String.Format("{0}={1} ", comp, comp_cmd);
        }
        {
          string path = "reconfigure";
          using (TextWriter writer = new StreamWriter(path)) {
            writer.WriteLine ("#!/bin/sh");
            writer.WriteLine ("echo 'Execute: {0}{1}'", env, cmd.ToString ());
            writer.WriteLine ("{0}{1}", env, cmd.ToString ());
          }
          if (ExecProcess ("/bin/chmod", String.Format ("+x {0}", path)) != 0) {
            throw new Exception (String.Format ("Cannot update permissions for file {0}", path));
          }
        }

        {
          string path = "force-regenerate";
          using (TextWriter writer = new StreamWriter(path)) {
            writer.WriteLine ("#!/bin/sh");
            writer.WriteLine ("echo 'Destroy all and execute: {0}{1}{2}'", env, cmd.ToString (), force_regenerate_script_option);
            writer.WriteLine ("{0}{1}{2}", env, cmd.ToString (),force_regenerate_script_option);
          }
          if (ExecProcess ("/bin/chmod", String.Format ("+x {0}", path)) != 0) {
            throw new Exception (String.Format ("Cannot update permissions for file {0}", path));
          }
        }
      }
    }

    private static string convertTagToCMake (string tag)
    {
      string tmp = tag.ToUpper();
      tmp = tmp.Replace ('-', '_');
      tmp = tmp.Replace ('+', 'X');
      return tmp;
    }

    private string normalizePath (string s)
    {
      if (m_common_tools.isWindows ()) {
        s = s.Replace ("\\", "/");
      }
      // s = s.Replace (" ", "\\ ");
      return s;

    }

    private void setProjectName() 
    {
      string filePath = "CMakeCache.txt";
      Regex regex = new Regex (@"^CMAKE_PROJECT_NAME:STATIC=(?<name>.*)?$");
      using (var f = File.OpenRead(filePath))
      {
        var s = new StreamReader(f);
        while (!s.EndOfStream)
        {
          string line = s.ReadLine();
          Match m = regex.Match (line);
          if (m.Success) {
            f.Close ();
            m_project_name = m.Groups ["name"].Value;
            return;
          }
        }
        f.Close();
      }
      throw new ConfiguratorException ("Cannot identify project name");
    }

    private string parseFormat(string format, string task)
    {
      if (String.IsNullOrEmpty (format))
        return null;

      string release_mode = releaseMode ();
      if (!m_common_tools.isWindows ())
        release_mode = release_mode.ToLower ();

      return format
        .Replace ("%p", m_project_name)
        .Replace ("%t", task)
        .Replace ("%d", m_date_time.ToString("s"))
        .Replace ("%m", release_mode);
    }

    private string releaseMode() {
      if (release_mode)
        return "Release";
      else
        return "Debug";
    }

    void doThis(string command, string log_file, string what) 
    {
      if (String.IsNullOrEmpty(command)) {
        Console.WriteLine ("Skip {0} due to null command", what.ToLower ());
        return;
      }

      TextWriter saved_cout = Console.Out;

      int split_position = command.IndexOf (" ");
      string command_base, command_args;
      if (split_position <= 0) {
        command_base = command;
        command_args = "";
      } else {
        command_base = command.Substring(0,split_position);
        command_args = command.Substring(split_position+1);
      }

      int r = 0;
      using (Process p = new Process()) {
        
        p.StartInfo.UseShellExecute = false;
        p.StartInfo.FileName = command_base;
        p.StartInfo.Arguments = command_args;
        
        if (dry_mode || GlobalContext.Instance.Verbose)
          Console.WriteLine("Execute: {0} {1}", p.StartInfo.FileName, p.StartInfo.Arguments);

        if (log_file != null)
        {
          // bool overwritten = File.Exists(log_file);
          Console.WriteLine ("{2} project {0} with logs in {1}", m_project_name, log_file, what);
          StreamWriter sw = new StreamWriter(log_file);
          sw.AutoFlush = true;
          Console.SetOut (new MirroringTextWriter(Console.Out, sw));     
          p.StartInfo.RedirectStandardOutput = true;
          p.StartInfo.RedirectStandardError = true;
          p.OutputDataReceived += (sender, out_args) => {
            if (out_args.Data != null) {
              Console.WriteLine(out_args.Data);
            } };
          p.ErrorDataReceived += (sender, out_args) => {
            if (out_args.Data != null) {
              Console.WriteLine(out_args.Data);
            } };
        } else {
          Console.WriteLine ("{1} project {0} (without logging)", m_project_name, what);
        }

        try {
          if (!dry_mode) {
            p.Start ();
            if (log_file != null)
            {
              p.BeginOutputReadLine ();
              p.BeginErrorReadLine ();
            }
            p.WaitForExit ();
            r = p.ExitCode;
          }
        }
        catch (Exception e) {
          throw new ConfiguratorException(String.Format ("Error '{0}' has been occurred while {3}:\n\t{1} {2}", e.Message, p.StartInfo.FileName, p.StartInfo.Arguments, what.ToLower()));
        }
      }

      // Restore Console out
      if (log_file != null) {
        Console.SetOut(saved_cout);
      }
    }
  }


  public sealed class GlobalContext
  {
    private static readonly GlobalContext _instance = new GlobalContext ();
    
    private GlobalContext ()
    {
    }
    
    public static GlobalContext Instance {
      get {
        return _instance;
      }
    }
    
    private bool verbose = false;
    
    public bool Verbose {
      get { return verbose; }
      set { verbose = value; }
    }
  }
}

