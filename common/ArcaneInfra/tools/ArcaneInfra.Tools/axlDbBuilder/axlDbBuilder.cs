using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.IO;

namespace axlDbBuilder
{
    class axlDbBuilder
    {
        static void Main(string[] args)
        {
            if (args.Count() < 1 || args.Count() > 3 )
            {
                Console.WriteLine("Usage: axlDbBuilder.exe axldb.txt [exclude-file-list.txt] [application-name]");
                System.Environment.Exit(1);
            }
            XElement axldb_file = new XElement("root", new XElement("modules"), new XElement("services"));
            if (args.Count() == 3) axldb_file.Add(new XAttribute("application-name", args[2]));

            List<string> axldb_reduced = new List<string>();
            XDocument current_axl;
            if (!File.Exists(args[0])) throw new Exception("Unexisting entry file "+args[0]);
            bool check_exclusion = args.Count() >=2;
            foreach (string current_file in File.ReadLines(args[0]))
            {
                // Check if file is to exclude (if exclude list given)
                if (check_exclusion)
                {
                    if (File.ReadLines(args[1]).FirstOrDefault(file => Path.GetFileName(file) == Path.GetFileName(current_file)) != default(string))
                    {
                        Console.WriteLine("Excluding file " + current_file);
                        continue;
                    }
                }
                // Add kept line in axldb reduced
                axldb_reduced.Add(current_file);
                // Load axl file and proceed
                current_axl = XDocument.Load(current_file);
                if (current_axl.Element("module") != null) _addModule(axldb_file.Element("modules"),current_axl);
                else if (current_axl.Element("service") != null) _addService(axldb_file.Element("services"), current_axl,current_file);
                else throw new Exception("Ill-formed axl file: "+current_file+", no module or service root node found");
            }
            string axl_db_file_name = "axl_db_file";
            axldb_file.Save(axl_db_file_name);
            Console.WriteLine("-- Saving " + axl_db_file_name + "--");
            File.WriteAllLines("axldb_reduced.txt", axldb_reduced);
            Console.WriteLine("-- Writing axldb_reduced.txt (= axldb.txt without excluded files) --");
        }

        private static void _addService(XElement service_nodes, XDocument current_axl, string current_file)
        {
            string service_name = current_axl.Element("service").Attribute("name").Value;
            XElement new_service_node = new XElement("service", new XAttribute("name", service_name), new XAttribute("file-base-name",Path.GetFileNameWithoutExtension(current_file)));
            var new_service_interfaces = current_axl.Descendants("interface");
            new_service_node.Add(from interface_node in new_service_interfaces select new XElement("implement-class", new XAttribute("name", interface_node.Attribute("name").Value)));
            // Try to get service alias(es) ie registered in ARCANE_REGISTER_SERVICE.
            string cc_path = Path.GetDirectoryName(current_file) +"/" + Path.GetFileNameWithoutExtension(current_file) + "Service.cc";
            if (File.Exists(cc_path))
            {
                var service_aliases = from line in File.ReadLines(cc_path) where (line.Contains(("ARCANE_REGISTER_SERVICE"))) select line.Split('(',',')[1];
                foreach (var alias in service_aliases)
                {
                    if (alias != service_name) new_service_node.Add(new XElement("alias", new XAttribute("name", alias)));
                }
            }
            else Console.WriteLine("Info: no alias defined for service " + service_name +" since " + cc_path+ " cannot be opened");
            service_nodes.Add(new_service_node);
            // Find service instance info
            _addServiceInstanceNodes(current_axl, new_service_node);
            Console.WriteLine("Adding service " + service_name);

        }

        private static void _addModule(XElement modules_node, XDocument current_axl)
        {
            string module_name = current_axl.Element("module").Attribute("name").Value;
            Console.WriteLine("Adding module " + module_name);
            // Find service instance info
            XElement new_module_node = new XElement("module", new XAttribute("name", module_name));
            _addServiceInstanceNodes(current_axl, new_module_node);
            modules_node.Add(new_module_node);
        }

        private static void _addServiceInstanceNodes(XDocument current_axl, XElement new_service_or_module_node)
        {
            var service_instance_nodes = current_axl.Descendants("service-instance");
            new_service_or_module_node.Add(from service_instance_node in service_instance_nodes.ToList()  select new XElement("has-service-instance",
                new XAttribute("interface", service_instance_node.Attribute("type").Value), new XAttribute("option-name",_getFullOptionName(service_instance_node))));
        }
     
        private static string _getFullOptionName(XElement option_node)
        {
            string option_name = option_node.Attribute("name").Value;
            foreach (XElement parent in option_node.Ancestors())
            {
                if (parent != null && parent.Attribute("name") != null) option_name = String.Format("{0}_{1}",parent.Attribute("name").Value, option_name);
            }
            return option_name;
        }
        
    }
}




