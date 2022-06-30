using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace ApplicationLauncher
{
	public class Env
	{
		private Env() { }

		public class Tag { 
			public Tag(string name, string description)
			{
				this.name = name;
				this.description = description;
			}

			public override string ToString() {
				return String.Format ("{0} ({1})", name, description);
			}

			private string name;
			private string description;
		};

		#region MEMBERS
		private static Dictionary<Tag, Dictionary<string, string>> environments = new Dictionary<Tag, Dictionary<string, string>>();
		private static Dictionary<String, Tag> tag_dictionary = new Dictionary<string, Tag>();
		#endregion

		public static void CreateTag(string tag, string description) {
			if (tag_dictionary.ContainsKey (tag)) {
				throw new InternalException(String.Format("already existing export tag '{0}'", tag));
			} else {
				tag_dictionary.Add (tag, new Tag(tag, description));
			}
		}

		public static Tag GetTag(string tag) {
			Tag existingTag = null;
			if (tag_dictionary.TryGetValue (tag, out existingTag)) {
				return existingTag;	
			} else {
				throw new InternalException(String.Format("undefined export tag '{0}'",tag));
			}
		}

		public static void Export(Tag tag, string variableName, string strValue)
		{
			Dictionary<string,string> tagDictionary = null;
			if (!environments.TryGetValue (tag, out tagDictionary)) 
			{
				tagDictionary = new Dictionary<string, string>();
				environments.Add (tag, tagDictionary);
			}
			
			try
			{
				tagDictionary.Add(variableName, strValue);
			}			
			catch (System.ArgumentNullException)
			{
				Logger.log(Logger.Channel.Error,"export: key is null do nothing");
			}
			catch (System.ArgumentException)
			{
        Logger.log(Logger.Channel.Error,"export: duplicate '{0}' do nothing", variableName);
			}
			catch (Exception e)
			{
				Logger.log(Logger.Channel.Error,"export: do noting {0}",e.Message);
			}
		}

		public static void LoadEnvironment(ProcessStartInfo startInfo, Tag[] tags) 
		{
			if (tags == null)
				return;
			
			foreach (Tag tag in tags) {
				Logger.log (Logger.Channel.Debug, "Setting environment for tag {0}", tag);
				
				if (environments.ContainsKey(tag)) {
					foreach (System.Collections.Generic.KeyValuePair<string, string> pair in environments[tag]) {
  					Logger.log(Logger.Channel.Debug, "\t{0}={1}", (string)pair.Key, (string)pair.Value);
						try {
                            if (! startInfo.EnvironmentVariables.ContainsKey((string)pair.Key)) 
							    startInfo.EnvironmentVariables[(string)pair.Key] = (string)pair.Value;
                            else 
								startInfo.EnvironmentVariables[(string)pair.Key] = (string)pair.Value + System.IO.Path.PathSeparator + startInfo.EnvironmentVariables[(string)pair.Key];
						} catch (System.ArgumentNullException) {
							//key is null do nothing
						} catch (System.ArgumentException) {
							//duplicate variableName do nothing
						} catch (Exception) {
							//do noting
						}
					}
				}
			}
		}


	}
}

