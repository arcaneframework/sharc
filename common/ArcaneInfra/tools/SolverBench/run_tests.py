import xml.etree.ElementTree as ET
import os
import glob
import shutil
import config
from io_utils import get_files_by_extension
from io_utils import get_element_in_xml
from io_utils import get_all_elements_in_xml

def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    tree = ET.parse(config.config_file)
    root = tree.getroot()

    my_input_directory = get_all_elements_in_xml(root, 'output-directory')
    filelist = get_files_by_extension(my_input_directory, '.arc')
    appname = get_element_in_xml(root, 'app-name').text

    nb_procs_list = get_element_in_xml(root, 'nb-procs-list')

    for files in filelist:
        path = os.path.dirname(os.path.abspath(files))
        for nb_procs in nb_procs_list:
            new_path = os.path.join(path,"np" + nb_procs.text)
            filename = os.path.basename(files)

            if not os.path.exists(new_path):
                os.makedirs(new_path)

            if os.path.exists(path + '/input') and not os.path.exists(new_path + '/input') and not os.path.islink(new_path + '/input'):
                os.symlink(path + '/input', new_path + '/input')
            elif os.path.exists(path + '/Input') and not os.path.exists(new_path + '/Input') and not os.path.islink(new_path + '/Input'):
                os.symlink(path + '/Input', new_path + '/Input')
            else:
                resource_files = glob.iglob(os.path.join(path, "*.vt2"))
                for file in resource_files:
                    resource_filename = os.path.basename(file)
                    if os.path.isfile(file) and not os.path.exists(new_path + '/' + resource_filename) and not os.path.islink(new_path + '/' + resource_filename):
                        os.symlink(path + '/' + resource_filename, new_path + '/' + resource_filename)

            if not os.path.exists(new_path + '/' + filename ) and not os.path.islink(new_path + '/' + filename):
                os.symlink(path + '/' + filename, new_path + '/' + filename)

            if not os.path.exists(new_path + '/' + appname + '.config') and not os.path.islink(new_path + '/' + appname + '.config'):
                os.symlink(path + '/' + appname + '.config', new_path + '/' + appname + '.config')

            run_command = "mpirun -np "+ nb_procs.text + " "
            binary = get_element_in_xml(root, 'binary').text
            os.environ["ARCANE_PARALLEL_SERVICE"]="Mpi"
            os.chdir(new_path)
            if(config.verbose):
                print("Run command: " + run_command + binary + " " + new_path + '/' + filename + ">& output_" + os.path.splitext(filename)[0])
            os.system(run_command + binary + " " + new_path + '/' + filename + ">& output_" + os.path.splitext(filename)[0])

            if os.path.exists(new_path + '/' + appname + '.config'):
                os.unlink(new_path + '/' + appname + '.config')

            if os.path.exists(new_path + '/' + filename ):
                os.unlink(new_path + '/' + filename)

            if os.path.exists(new_path + '/input'):
                os.unlink(new_path + '/input')

            if os.path.exists(new_path + '/Input'):
                os.unlink(new_path + '/Input')

            resource_files = glob.iglob(os.path.join(new_path, "*.vt2"))
            for file in resource_files:
                if os.path.exists(file):
                    os.unlink(file)

            if (os.path.exists(new_path + '/output')):
                shutil.rmtree(new_path + '/output')

if __name__ == "__main__":
    main()
