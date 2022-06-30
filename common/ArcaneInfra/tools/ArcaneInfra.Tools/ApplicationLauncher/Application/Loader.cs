using System;
using System.Collections;
using System.Collections.Generic;
using ApplicationLauncher;
using System.IO;
using System.Reflection;

namespace Application
{
	public class Loader : ILoader
	{
    public Loader()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();
      string configuration_resource_name = "Application.Application.xml";
      Stream stream = assembly.GetManifestResourceStream(configuration_resource_name);
      if (stream == null) {
        throw new ApplicationLauncher.InternalException(String.Format("Cannot open resource '{0}'", configuration_resource_name));
      }
      configuration = ConfigurationReader.Initialize<ApplicationConfiguration> (stream);
      if (configuration == null)
        throw new ApplicationLauncher.InternalException ("Cannot parse application configuration");
    }

		public ICollection<IAction> Load ()
		{
			List<IAction> actions = new List<IAction> ();
      actions.Add (new Application(configuration));
			return actions;
		}

    public bool EnableParallelMode { get { return configuration.EnableParallelMode; } }

    private ApplicationConfiguration configuration = null;
	}
}