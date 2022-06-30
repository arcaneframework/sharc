import os
import glob
import shutil
import config
import re

def indent(elem, level=0, more_sibs=False):
    i = "\n" + level*"  "
    if level:
        i += (level - 1) * '  '
        num_kids = len(elem)
        if num_kids:
            if not elem.text or not elem.text.strip():
                elem.text = i + "  "
                if level:
                    elem.text += '  '
                    count = 0
                    for kid in elem:
                        indent(kid, level + 1, count < num_kids - 1)
                        count += 1
                        if not elem.tail or not elem.tail.strip():
                            elem.tail = i
                            if more_sibs:
                                elem.tail += '  '
                            else:
                                if level and (not elem.tail or not elem.tail.strip()):
                                    elem.tail = i
                                    if more_sibs:
                                        elem.tail += '  '

def get_files_by_extension(input_directories, extension):
    filelist = []
    for input_directory in input_directories:
        for subdir, dirs, files in os.walk(input_directory.text):
            for filename in files:
                filepath = subdir + os.sep + filename
                if filepath.endswith(str(extension)):
                    filelist.append(filepath)
    return filelist

def create_tests_ressources(app_name, base_path, output_directory, filelist):
    original_arc_files = []
    for files in filelist:
        path = os.path.dirname(os.path.abspath(files))
        new_path = path.replace(base_path, output_directory)
        if not os.path.exists(new_path):
            os.makedirs(new_path)
        base_filename = os.path.basename(files)
        name, ext = os.path.splitext(base_filename)
        new_file_name = name + '_original' + ext
        if not os.path.exists(new_path + '/' + new_file_name):
            shutil.copy2(files, new_path + '/' + new_file_name)
            original_arc_files.append(new_path + '/' + new_file_name)
        if os.path.exists(path + '/input') and not os.path.exists(new_path + '/input') and not os.path.islink(new_path + '/input'):
            os.symlink(path + '/input', new_path + '/input')
        elif os.path.exists(path + '/Input') and not os.path.exists(new_path + '/Input') and not os.path.islink(new_path + '/Input'):
            os.symlink(path + '/Input', new_path + '/Input')
        else:
            files = glob.iglob(os.path.join(path, "*.vt2"))
            for file in files:
                if os.path.isfile(file):
                    shutil.copy2(file, new_path)

        config_files = glob.iglob(os.path.join(path, app_name + '.config'))
        for config_file in config_files:
            if os.path.isfile(config_file):
                shutil.copy2(config_file, new_path)
        if not os.path.exists(new_path + '/' + app_name + '.config'):
            found_config_file = 0
            for root, dir, files in os.walk(base_path + '/../src'):
                if found_config_file:
                    break
                for file in files:
                    if found_config_file:
                        break
                    if app_name + '.config' in file:
                        if not os.path.exists(new_path + '/' + app_name + '.config'):
                            shutil.copy2(root + '/' + file, new_path)
                            found_config_file = 1
    return original_arc_files

def get_element_in_xml(xml_root, tag):
    element = xml_root.find(str(tag))
    return element

def get_all_elements_in_xml(xml_root, tag):
    elements = xml_root.findall(str(tag))
    return elements

def ireplace(old, repl, text):
    return re.sub('(?i)'+re.escape(old), lambda m: repl, text)
