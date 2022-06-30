import os
import xml.etree.ElementTree as ET
from collections import defaultdict
import config
from solver_dictionnary import build_solver_dictionnary
from io_utils import get_files_by_extension
from io_utils import indent
from io_utils import create_tests_ressources
from io_utils import get_element_in_xml
from io_utils import get_all_elements_in_xml

def get_avail_solvers_preconds(alien_root, package_name, package_directory, solver_dict):
    input_directory = alien_root + "/" + package_directory
    available_solvers = []
    available_preconds = []
    if(package_name != 'PETScSolver'):
        for subdir, dirs, files in os.walk(input_directory):
            for file in files:
                if file.endswith(".axl"):
                    axl_file = subdir + os.sep + file
                    tree = ET.parse(axl_file)
                    root = tree.getroot()
                    solver_node = None
                    if(package_name in solver_dict and 'solver' in solver_dict[package_name]):
                        solver_node = get_element_in_xml(root, str('.//*[@name="' + solver_dict[package_name]['solver'] + '"][@type="' + solver_dict[package_name]['solver-type'] + '"]'))
                    if(solver_node is not None):
                        solvers = solver_node.iter('enumvalue')
                        for solver in solvers:
                            available_solvers.append(solver.get('name'))
                    precond_node = None
                    if(package_name in solver_dict and 'precond' in solver_dict[package_name]):
                        precond_node = get_element_in_xml(root, str('.//*[@name="' + solver_dict[package_name]['precond'] + '"][@type="' + solver_dict[package_name]['precond-type'] + '"]'))
                    if(precond_node is not None):
                        preconds = precond_node.iter('enumvalue')
                        for precond in preconds:
                            available_preconds.append(precond.get('name'))
    else:
        for subdir, dirs, files in os.walk(input_directory):
            for file in files:
                if file.endswith(".axl") and not 'FieldSplit' in file and not 'AdditiveSchwarz' in file and not 'SolverConfigSuperLU' in file and not 'SolverConfigLU' and not 'MUMPS' in file:
                    axl_file = subdir + os.sep + file
                    tree = ET.parse(axl_file)
                    root = tree.getroot()
                    service_name = root.get('name')
                    if('PETScSolverConfig' in service_name):
                        available_solvers.append(service_name)
                    if('PETScPrecConfig' in service_name):
                        available_preconds.append(service_name)
    if(config.verbose):
        print("Package: " + package_name)
        print("Available solvers: ")
        for solver in available_solvers:
            print(solver)
        print("Available preconds: ")
        for precond in available_preconds:
            print(precond)
    return available_solvers, available_preconds

def generate_new_file(in_file, output_dir, package, solvers_to_change, solvers, preconds, solver_options, solver_dict):
    if(package != 'IFPSolver'):
        for solver in solvers:
            for precond in preconds:
                new_file_name = os.path.basename(in_file)
                new_file_name = os.path.splitext(new_file_name)[0]
                new_file_name = new_file_name + "_" + package + "_" + solver + "_" + precond
                tree = ET.parse(in_file)
                root = tree.getroot()
                for solver_to_change in solvers_to_change:
                    physic_roots = get_all_elements_in_xml(root, './/'+ solver_to_change.tag)
                    linear_solver = None
                    for physic in physic_roots:
                        linear_solver = get_element_in_xml(physic, solver_to_change.text)
                        if(linear_solver is not None):
                            linear_solver.set('name', str(package))
                            solver_node = get_element_in_xml(linear_solver, 'solver')
                            if(solver_node is not None):
                                output_filename= new_file_name + "_" + solver_to_change.tag
                                linear_solver.remove(solver_node)
                                add_new_linear_solver_node(package, linear_solver, solver, precond, solver_options, output_filename, solver_dict)
                indent(root)
                tree.write(output_dir + "/" + new_file_name + ".arc")
    else:
        solver = "none"
        for precond in preconds:
            new_file_name = os.path.basename(in_file)
            new_file_name = os.path.splitext(new_file_name)[0]
            new_file_name = new_file_name + "_" + package + "_" + solver + "_" + precond
            tree = ET.parse(in_file)
            root = tree.getroot()
            for solver_to_change in solvers_to_change:
                physic_roots = get_all_elements_in_xml(root, './/'+solver_to_change.tag)
                linear_solver = None
                for physic in physic_roots:
                    linear_solver = get_element_in_xml(physic, solver_to_change.text)
                    if(linear_solver is not None):
                        linear_solver.set('name', str(package))
                        solver_node = get_element_in_xml(linear_solver, 'solver')
                        if(solver_node is not None):
                            output_filename= new_file_name + "_" + solver_to_change.tag
                            linear_solver.remove(solver_node)
                            add_new_linear_solver_node(package, linear_solver, solver, precond, solver_options, output_filename, solver_dict)
            indent(root)
            tree.write(output_dir + "/" + new_file_name + ".arc")
    if(config.verbose):
        print("in_file: " + in_file)
        print("output_dir: " + output_dir)
        print("package: " + package)

def add_new_linear_solver_node(package, linear_solver, solver, precond, solver_options, logger_output_filename, solver_dict):
    logger_node = ET.SubElement(linear_solver,'logger')
    logger_node.set('name','SolverLogger')
    out_filename_node = ET.SubElement(logger_node,'out-filename')
    out_filename_node.text = str(logger_output_filename + "_" + linear_solver.tag + ".xml")
    if(package!='PETScSolver' and package != 'IFPSolver'):
        new_linear_solver_node = ET.SubElement(linear_solver, 'solver')
        new_linear_solver_node.text = str(solver)
        new_precond_node = ET.SubElement(linear_solver, 'preconditioner')
        new_precond_node.text = str(precond)
        verbose = get_element_in_xml(linear_solver, 'verbose')
        if(verbose is not None):
            verbose.tag = solver_dict[package]['verbose']
            if package.lower() != 'mcgsolver':
                verbose.text = 'true'
            else:
                verbose.text = '1'
        for option in solver_options:
            new_option = ET.SubElement(linear_solver, str(solver_dict[package][str(option.tag)]))
            new_option.text = option.text
    elif(package=='PETScSolver'):
        new_linear_solver_node = ET.SubElement(linear_solver, 'solver')
        solver = solver.replace('PETScSolverConfig', '')
        new_linear_solver_node.set('name', str(solver))
        for option in solver_options:
            new_option = ET.SubElement(new_linear_solver_node, str(solver_dict[package][str(option.tag)]))
            new_option.text = option.text
        new_precond_node = ET.SubElement(new_linear_solver_node, 'preconditioner')
        precond = precond.replace('PETScPrecConfig', '')
        new_precond_node.set('name', str(precond))
    else: #IFPSolver
        new_precond_node = ET.SubElement(linear_solver, 'precond-option')
        new_precond_node.text = str(precond)
        verbose = get_element_in_xml(linear_solver, 'verbose')
        if(verbose is not None):
            verbose.text = 'true'
        for option in solver_options:
            new_option = ET.SubElement(linear_solver, str(solver_dict[package][str(option.tag)]))
            new_option.text = option.text

def add_logger_to_original_arcfile(arcfiles, solvers_to_change):
    for file in arcfiles:
        tree = ET.parse(file)
        root = tree.getroot()
        base_filename = os.path.basename(file)
        name, ext = os.path.splitext(base_filename)
        new_file_name = name + "_original" + ext
        for solver in solvers_to_change:
            physic_roots = get_all_elements_in_xml(root, './/'+ solver.tag)
            linear_solver = None
            for physic in physic_roots:
                linear_solver = get_element_in_xml(physic, solver.text)
                if linear_solver is not None:
                    new_file_name = new_file_name + "_" + solver.tag
                    logger_node = ET.SubElement(linear_solver,'logger')
                    logger_node.set('name','SolverLogger')
                    out_filename_node = ET.SubElement(logger_node,'out-filename')
                    out_filename_node.text = str(name + '_' + solver.tag + '_' + linear_solver.tag + '.xml')
        indent(root)
        tree.write(file)

def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    tree = ET.parse(config.config_file)
    root = tree.getroot()

    solver_dict = defaultdict(dict)
    build_solver_dictionnary(solver_dict)

    alien_root = get_element_in_xml(root, 'alien-root')

    base_path = get_element_in_xml(root, 'base-path').text
    input_directories = get_all_elements_in_xml(root, 'input-directory')

    filelist = get_files_by_extension(input_directories,'.arc')
    output_directory = get_element_in_xml(root, 'output-directory').text

    app_name = root.find('app-name').text
    original_arc_files = create_tests_ressources(app_name, base_path, output_directory, filelist)
    solvers_to_change = get_element_in_xml(root, 'solvers')
    solver_options = get_element_in_xml(root, 'solver-global-options')

    add_logger_to_original_arcfile(original_arc_files, solvers_to_change)

    packages_node = get_element_in_xml(root, 'packages')

    for package in get_all_elements_in_xml(packages_node, 'package'):
        solvers, preconds = get_avail_solvers_preconds(alien_root.text, package.get('name'), package.text, solver_dict)

        for in_file in filelist:
            path = os.path.dirname(os.path.abspath(in_file))
            new_path = path.replace(base_path, output_directory)
            generate_new_file(in_file, new_path, package.get('name'), solvers_to_change, solvers, preconds, solver_options, solver_dict)

if __name__ == "__main__":
    main()
