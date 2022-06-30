using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;
using System.Xml.Schema;
using System.IO;

namespace ArcFileXmlValidation
{
	#region EXCEPTION
	[Serializable]
	public class AxlToolException : Exception
	{
		public AxlToolException ()
		{
		}
		
		public AxlToolException (string message)
			: base(message)
		{
			;
		}
	}
	
	[Serializable]
	public class AxlToolExit : Exception
	{
		public AxlToolExit ()
		{
		}
		
		public AxlToolExit (string message)
			: base(message)
		{
			;
		}
	}
	#endregion

    class ArcFileXmlValidation
    {
		#region CONFIGURATIONS
		private static string codename = "axldoc";
		private static int RTCODE_OK = 0;
		private static int RTCODE_ERROR = 1;
		#endregion

        static void Main(string[] args)
        {
			try {
				ArcFileXmlValidation v = new ArcFileXmlValidation ();
				v.Execute (args);
				Environment.Exit (RTCODE_OK);
			} catch (AxlToolExit e) {
				Console.WriteLine (e.Message);
				Environment.Exit (RTCODE_OK);
			} catch (Exception e) {
				Console.Error.WriteLine (String.Format ("Exiting with an error: {0}", e.Message));
				Environment.Exit (RTCODE_ERROR);
			}
		}

		public ArcFileXmlValidation() 
		{

		}

		private void Execute(string[] args)
		{
			m_options = new Mono.Options.OptionSet ();

			bool do_sort = false;
			bool do_verbose = false;
			bool show_help = false;
			string temporary_directory = ".";

			List<String> exclusion_list = new List<String> ();		
			exclusion_list.Add ("//*[contains(local-name(),'-test')]");
			exclusion_list.Add ("/case/arca-d-e-s/post-processing[post-processor/format/@name='Ensight7PostProcessor']");


			// Configure options
			m_options.Add ("h|help", "Help page", v => { show_help = true; });
			m_options.Add ("v|verbose", "Display processing details", v => { do_verbose = true; });
			m_options.Add ("s|sorted", "Sort tags in .arc file", v => do_sort = true );
			m_options.Add ("x|exclude=", "Exclude XPath", (string s) => { exclusion_list.Add (s); });
			m_options.Add ("tmp=", "Set temporary directory", (string s) => { temporary_directory = s; });

			List<string> filenames = new List<string>(); // files to validate
			try {
				List<string> remaining_args = m_options.Parse (args);
				
				// Check if remaining args looks like options
				for (int i=0; i<remaining_args.Count; ++i) {
					String arg = remaining_args [i];
					if (arg.StartsWith("-"))
						throw new Mono.Options.OptionException (String.Format ("Invalid option {0}", arg), arg);
					else 
						filenames.Add (arg);
				}
				if (show_help) {
					_showHelpAndExit ();
				}
			} catch (Mono.Options.OptionException e) {
				_showHelpAndExit (e.Message);
			}

            if (filenames.Count < 2)
            {
				_showHelpAndExit("Missing files");
            }

			String xsd_filename = filenames [0];
			filenames.RemoveAt (0);

			XDocument xsd_file = XDocument.Load(xsd_filename);

			List<String> failed_validation = new List<String> ();
			foreach (string filename in filenames) {
				Console.WriteLine ("=== Processing file {0} ===", filename);

				string arcfile_with_validation_info_name = Path.Combine (temporary_directory,
				                                                         Path.GetFileNameWithoutExtension (filename) + "_for-xsd-validation.arc");
				{
					// Load .arc file in Xelement
					XDocument arc_file = XDocument.Load (filename);
					//Console.WriteLine(arc_file);
					//Console.WriteLine(arc_file.Elements().First());
					// Perform conversion: 1- add xsi namespace 
					XNamespace xml_namespace = "http://www.w3.org/2001/XMLSchema-instance";
					XNamespace xsd_namespace = "http://www.w3.org/2001/XMLSchema";

					arc_file.Element ("case").AddAttribute(XNamespace.Xmlns + "xsi", xml_namespace);
					arc_file.Element ("case").AddAttribute(XNamespace.Xmlns + "xsi", xsd_filename);

					foreach(string exclusion in exclusion_list) {
						if (do_verbose)
							Console.WriteLine("-- Processing exclusion XPath pattern {0}", exclusion);

						IEnumerable<XElement> r = null;
						try {
							r = arc_file.XPathSelectElements(exclusion);
							if (do_verbose)
								foreach(XElement x in r)
									Console.WriteLine("-- Excluding node {0} using XPath pattern {1}", x.GetAbsoluteXPath(), exclusion);
						} catch(Exception e) {
							Console.WriteLine("An error occurs while processing XPath pattern {0} ({1})", exclusion, e.Message);
						}
						if (r != null)
							r.Remove();
					}

					if (do_verbose) Console.WriteLine ("-- Add xsi:type info in service nodes");
					// Perform conversion: 2- add xsi:type attribute in all service node
					// 2.1- build service list
					var service_nodes = from element in arc_file.Element ("case").Descendants () where (element.Attribute ("name") != null) select element;
					// 2.2- build service names, using annotation info in xsd
					// 2.2- add xsi type attribute in arc file
					foreach (XElement service_node in service_nodes) {
						string service_type = (from element in xsd_file.Descendants (xsd_namespace + "annotation") 
	                                                where element.Element (xsd_namespace + "appinfo").Value == service_node.Name + "_" + service_node.Attribute ("name").Value 
	                                                select element.Element (xsd_namespace + "documentation").Value).FirstOrDefault ();
						if (service_type != null) {
							service_node.AddAttribute(xml_namespace + "type", service_type);
						} else {
							Console.WriteLine ("-- Type Info not generated for service " + service_node.Parent.Name + "/" + service_node.Name + "/" + service_node.Attribute ("name").Value);
						}
					}

					if (do_sort)
						arc_file = Sort (arc_file);

					if (do_verbose)
						Console.WriteLine ("-- Save arcfile with validation info in " + arcfile_with_validation_info_name);
					arc_file.Save (arcfile_with_validation_info_name);
				}

				{
					XDocument arc_file2 = XDocument.Load(arcfile_with_validation_info_name, 
					                                     /* LoadOptions.PreserveWhitespace | */LoadOptions.SetLineInfo/* | LoadOptions.SetBaseUri*/);

					// Validate xml file against Schema
					XmlSchemaSet schema = new XmlSchemaSet ();
					schema.Add ("", xsd_filename);
					if (do_verbose)
						Console.WriteLine ("-- Validating {0}", arcfile_with_validation_info_name);

					List<String> error_list = new List<String>();
					arc_file2.Validate (schema, (o,e) =>
					{
						error_list.Add (e.Message);
					});

					if (error_list.Count>0) {
						// has errors
						String position = "";
						Regex lineRegex = new Regex(@" Line (\d+),");
						Match lineMatch = lineRegex.Match(error_list[0]);
						if (lineMatch.Success)
							position = ":" + lineMatch.Groups[1].Captures[0];
						Console.WriteLine ("--- Following error are reported from file {0}{1}", arcfile_with_validation_info_name, position);

						foreach(var error in error_list)
							Console.WriteLine("\t{0}",error);
						Console.WriteLine ("=== [FAILED] {0} file did not validate", filename);

						failed_validation.Add (filename);
					} else {
						Console.WriteLine ("=== [SUCCESS] {0} file is validated", filename);
					}

				}
			}

			if (filenames.Count > 1) {
				if (failed_validation.Count > 0) {
					Console.WriteLine("== Summary of failed validationAll files validated"); 
					foreach(var filename in failed_validation) {
						Console.WriteLine("\t{0}", filename); 
					}
				} else {
					Console.WriteLine("== All files validated"); 
				}
			}
        }

		private XElement Cascade(XElement element, bool baseCompare = false)
		{
			IComparer<string> comparer = null; 
			if (baseCompare)
				comparer = new ArcSortComparer ();
			else
				comparer = StringComparer.InvariantCulture;

			return new XElement(element.Name,
			                    element.Attributes(),
			                    from child in element.Nodes()
			                    where child.NodeType != XmlNodeType.Element && child.NodeType != XmlNodeType.Comment
			                    select child,
			                    element.Elements()
			                    // .Where (child => child.NodeType != XmlNodeType.Comment),
			                    .OrderBy(child => child.Name.ToString(), comparer)
			                    .Select(child => Cascade(child))
			                    );
		}


		private XDocument Sort(XDocument file)
		{
			return new XDocument(
				file.Declaration,
				from child in file.Nodes()
				where child.NodeType != XmlNodeType.Element && child.NodeType != XmlNodeType.Comment
				select child,
				Cascade(file.Root, true));
		}

		public class ArcSortComparer : IComparer<string>, IDisposable
		{
			public ArcSortComparer() {}
			
			#region IComparer<string> Members			
			int IComparer<string>.Compare(string x, string y)
			{
				int xindex;
				bool xfound = table.TryGetValue (x, out xindex);
				if (!xfound)
					xindex = table.Count;

				int yindex;
				bool yfound = table.TryGetValue (y, out yindex);
				if (!yfound)
					yindex = table.Count;

				if (!xfound && !yfound)
					return String.Compare (x, y);
				else 
					return Math.Sign (xindex - yindex);
			}
			#endregion
			
			private Dictionary<string, int> table = new Dictionary<string, int>()
			{
				{ "arcane", 0 },
				{ "mesh", 1 }
			};
			
			public void Dispose()
			{
				table.Clear();
				table = null;
			}
		}

		private void _showHelpAndExit (String message = null)
		{
			StringWriter writer = new StringWriter ();
			if (message == null)
				writer.WriteLine ("Requested Help page");
			else
				writer.WriteLine (message);
			
			writer.WriteLine ("Usage: {0} [options] scheme.xsd arcfiles...", codename);
			writer.WriteLine ("Options : ");
			m_options.WriteOptionDescriptions (writer);

			if (message == null)
				throw new AxlToolExit (writer.ToString ());
			else
				throw new AxlToolException (writer.ToString ());
		}


		#region MEMBERS
		private Mono.Options.OptionSet m_options = null;
		#endregion
    }

	public static class XExtensions // méthode d'extension de XElement
	{
		public static XElement AddAttribute(this XElement x, XName attrname, object attrvalue)
		{
			if (x.Attribute(attrname) == null)
				x.Add (new XAttribute (attrname, attrvalue));
			return x;
		}

		/// <summary>
		/// Get the absolute XPath to a given XElement
		/// (e.g. "/people/person[6]/name[1]/last[1]").
		/// </summary>
		public static string GetAbsoluteXPath(this XElement element)
		{
			if (element == null)
			{
				throw new ArgumentNullException("element");
			}
			
			Func<XElement, string> relativeXPath = e =>
			{
				int index = e.IndexPosition();
				string name = e.Name.LocalName;
				
				// If the element is the root, no index is required
				
				return (index == -1) ? "/" + name : string.Format
				(
					"/{0}[{1}]",
					name, 
					index.ToString()
					);
			};
			
			var ancestors = from e in element.Ancestors()
				select relativeXPath(e);
			
			return string.Concat(ancestors.Reverse().ToArray()) + 
				relativeXPath(element);
		}
		
		/// <summary>
		/// Get the index of the given XElement relative to its
		/// siblings with identical names. If the given element is
		/// the root, -1 is returned.
		/// </summary>
		/// <param name="element">
		/// The element to get the index of.
		/// </param>
		public static int IndexPosition(this XElement element)
		{
			if (element == null)
			{
				throw new ArgumentNullException("element");
			}
			
			if (element.Parent == null)
			{
				return -1;
			}
			
			int i = 1; // Indexes for nodes start at 1, not 0
			
			foreach (var sibling in element.Parent.Elements(element.Name))
			{
				if (sibling == element)
				{
					return i;
				}
				
				i++;
			}
			
			throw new InvalidOperationException
				("element has been removed from its parent.");
		}
	}
}
