using System;
using System.Collections.Generic;
using System.Xml.Xsl;
using System.IO;
using System.Xml;
using System.Xml.Linq;

using Mono.Options;

namespace XsltProcessor
{
    class MainClass
    {
        public static void Main(string[] args)
        {
            bool verbose = false;
            bool help = false;
            bool dry = false;
            bool inplace = false;

            string xsl = null;
            string input = null;
            string output = null;
            bool recursive = false;
            string select = "*.arc";
            bool xml_format = true;

            var options = new OptionSet() {
        { "v|verbose", "Verbose mode", v => { verbose = true; } },
        { "h|help", "Show help page", v => { help = true; } },
        { "dry", "Dry mode; do not apply transformations", v => { dry = true; } },
        { "xsl=", "XSLT transformation file", v => { xsl = v; } }, 
        { "i|intput=", "Input XML file to transform", v => { input = v; } },
        { "o|output=", "Output for transformed file; used input file base name if output is a directory [OPTIONAL: default is undefined]\nmutually exclusive with --inplace", v => { output = v; } },
        { "inplace", "Transformation do in-place and a backup file .bak is created [OPTIONAL default: false]\nmutually exclusive with --output", v => { inplace = true; } },
        { "r|recursive", "Input and output are considered as directories [OPTIONAL default: false]\nFiles are selected using --select option value", v => { recursive = true; } },
        { "xml-format=", "Indentation step on output file [OPTIONAL default: true]\nMust be used if transformed output is not a valid XML file", (bool v) => { xml_format = v; } },
        { "select=", "Pattern to select XML files in directory [OPTIONAL default: *.arc]", v => { select = v; } }
      };

            List<string> remaining_args = options.Parse(args);
            if (remaining_args.Count > 0)
            {
                Console.WriteLine("Invalid command line");
                Console.WriteLine("Help : XsltProcessor.exe [OPTIONS]");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }

            if (help)
            {
                Console.WriteLine("Help : XsltProcessor.exe [OPTIONS]");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }

            if (String.IsNullOrEmpty(xsl))
            {
                Console.WriteLine("Error, undefined XSLT file\nHelp :");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }

            if (String.IsNullOrEmpty(input))
            {
                Console.WriteLine("Error, undefined XML input\nHelp :");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }

            if (File.Exists(input))
            {
                if (recursive)
                {
                    Console.WriteLine("Error, --recursive option requieres XML directory input\nHelp :");
                    options.WriteOptionDescriptions(Console.Out);
                    return;
                }
            }
            else if (Directory.Exists(input))
            {
                if (!recursive)
                {
                    Console.WriteLine("Error, directory input requires --recursive option\nHelp :");
                    options.WriteOptionDescriptions(Console.Out);
                    return;
                }
            }
            else
            {
                Console.WriteLine("Error, cannot acces to XML input\nHelp :");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }

            if (output == null)
            {
                if (!inplace)
                {
                    Console.WriteLine("Error, missing option --output or --inplace\nHelp :");
                    options.WriteOptionDescriptions(Console.Out);
                    return;
                }
            }
            else
            {
                if (File.Exists(output))
                {
                    if (recursive)
                    {
                        Console.WriteLine("Error, --recursive option requieres XML directory output\nHelp :");
                        options.WriteOptionDescriptions(Console.Out);
                        return;
                    }
                    else
                    {
                        Console.WriteLine("Warning, output file already exists and will be overwritten");
                    }
                }
                else if (Directory.Exists(output))
                {
                    if (!recursive)
                    {
                        Console.WriteLine("Error, directory input requires --recursive option\nHelp :");
                        options.WriteOptionDescriptions(Console.Out);
                        return;
                    }
                    else
                    {
                        Console.WriteLine("Warning, output directory already exists and included files may be overwritten");
                    }
                }
                else
                {
                    if (recursive)
                    {
                        try
                        {
                            if (verbose)
                            {
                                Console.WriteLine("* {0}Creating output directory {1}", (dry)?"[DRY] ":"", output);
                            }
                            if (!dry)
                            {
                                Directory.CreateDirectory(output);
                            }
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine("Error while creating directory '{0}' : {1}", output, e.Message);
                            Environment.Exit(-1);
                        }
                    }
                }
            }

            if (output != null && inplace)
            {
                Console.WriteLine("Error, conflincting options --output and --inplace\nHelp :");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }


            if (output == input)
            {
                Console.WriteLine("Error, output is input; remove --output option and use explicit --inplace option\nHelp :");
                options.WriteOptionDescriptions(Console.Out);
                return;
            }


            var trans = new XslCompiledTransform();

            if (verbose)
            {
                Console.WriteLine("* Load XSLT file {0}", xsl);
            }
            try
            {
                trans.Load(xsl);
            }
            catch (Exception e)
            {
                Console.WriteLine("Error in loading XSLT : {0}", e.Message);
                Environment.Exit(-1);
            }

            if (recursive)
            {
                if (verbose)
                {
                    Console.WriteLine("* Scanning directory {0} with selection pattern {1}", input, select);
                }

                var files = Directory.GetFiles(input,
                                               select,
                                               SearchOption.AllDirectories);
                foreach (var file in files)
                {
                    if (inplace)
                    {
                        Process(file, null, trans, verbose, dry, xml_format);
                    }
                    else
                    {
                        Process(file, output, trans, verbose, dry, xml_format);
                    }
                }
            }
            else
            {
                if (inplace)
                {
                    Process(input, null, trans, verbose, dry, xml_format);
                }
                else
                {
                    Process(input, output, trans, verbose, dry, xml_format);
                }
            }
        }

        private static void Process(string input, string output, XslCompiledTransform xsl, bool verbose, bool dry, bool xml_format)
        {
            string dry_str = (dry) ? "[DRY] " : "";

            if (verbose)
            {
                Console.WriteLine("* {1}Apply XSLT to XML File {0}", input, dry_str);
            }

            var stm = new MemoryStream();
            try
            {
                if (!dry)
                {
                    xsl.Transform(input, null, stm);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Error in transformation with XSLT : {0}", e.Message);
                return;
            }
            stm.Position = 0;

            try
            {
                if (output == null)
                {
                    if (verbose)
                    {
                        Console.WriteLine("* {1}Move input file {0} to backup {0}.bak", input, dry_str);
                    }
                    if (!dry)
                    {
                        File.Move(input, input + ".bak");
                    }
                    if (verbose)
                    {
                        Console.WriteLine("* Write output in file {0}", input);
                    }
                    if (!dry)
                    {
                        Write(stm, input, xml_format);
                    }
                }
                else
                {
                    if (Directory.Exists(output))
                    {
                        var file = Path.GetFileName(input);
                        output = Path.Combine(output, file);
                    }
                    if (verbose)
                    {
                        Console.WriteLine("* {1}Write output file {0}", output, dry_str);
                    }
                    if (!dry)
                    {
                        Write(stm, output, xml_format);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Error while writing output : {0}", e.Message);
                return;
            }
        }

        private static void Write(MemoryStream stm, string output, bool xml_format)
        {
            if (xml_format)
            {
                using (var outfile = new StreamWriter(output))
                {
                    var doc = new XDocument();
                    doc = XDocument.Load(stm);
                    outfile.Write(doc.ToString());
                }
            }
            else
            {
                using (var outfile = new FileStream(output, FileMode.Create, System.IO.FileAccess.Write))
                {
                    stm.WriteTo(outfile);
                }
            }
        }
    }
}