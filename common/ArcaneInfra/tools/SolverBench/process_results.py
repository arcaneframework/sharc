import xml.etree.ElementTree as ET
import sys
import os
import config
from io_utils import get_files_by_extension
from io_utils import ireplace
from io_utils import get_element_in_xml
from io_utils import get_all_elements_in_xml
from html_utils import create_main_page
from html_utils import write_a_html_page
from html_utils import create_result_page_header
from html_utils import create_a_title
from html_utils import create_a_link
from html_utils import create_a_table_header
from html_utils import copy_sortable_script

class TestCase :
    def __init__(self, filename, case, nb_proc, solver_name, package, max_it, tol, solver, precond, init_time, calls, success, error, iterations, solve_time, total_time):
        self.filename = filename
        self.case = case
        self.nb_proc = nb_proc
        self.solver_name = solver_name
        self.package = package
        self.max_it = max_it
        self.tol = tol
        self.solver = solver
        self.precond = precond
        self.init_time = init_time
        self.calls = calls
        self.success = success
        self.error = error
        self.iterations = iterations
        self.solve_time = solve_time
        self.total_time = total_time

    def __repr__(self):
        return 'Filename: %s, Case: %s, nb_proc: %s, solver_name: %s, package: %s, max_it: %s, tol: %s, solver: %s, precond: %s, init_time: %s, calls: %s, success: %s, error: %s, iterations: %s, solve_time: %s, total_time: %s' % (self.filename, self.case, self.nb_proc, self.solver_name, self.package, self.max_it, self.tol, self.solver, self.precond, self.init_time, self.calls, self.success, self.error, self.iterations, self.solve_time, self.total_time)

def table_header_for_a_test():
    return  ['package', 'solver', 'precond', 'nb_proc', 'success', 'error', 'iterations', 'init_time', 'solve_time', 'total_time']

def create_results_for_one_test(test_case, results_list):
    html, body = create_result_page_header()
    h1 = create_a_title('h1', os.path.splitext(test_case)[0])
    body.append(h1)
    solvers_for_that_case = set()
    for result in results_list:
        if result.case == test_case:
            solvers_for_that_case.add(result.solver_name)
    for solver_name in solvers_for_that_case:
        h2 = create_a_title('h2', solver_name)
        h1.append(h2)
        tbody = create_a_table_header(h2, table_header_for_a_test())
        for result in results_list:
            if result.case == test_case and result.solver_name == solver_name:
                if not 'original' in result.filename:
                    if result.success == '1':
                        tr = ET.Element('tr', attrib={ 'style': 'background-color:#00b300;color:#ffffff;' })
                    else: #result.success == 0
                        tr = ET.Element('tr', attrib={ 'style': 'background-color:#b30000;color:#ffffff;' })
                    tbody.append(tr)
                    for value in table_header_for_a_test():
                        td = ET.Element('td')
                        td.text = getattr(result, value)
                        tr.append(td)
                else: #original test case
                    tr = ET.Element('tr' , attrib={ 'style': 'background-color:#b3b3b3;color:#ffffff;' })
                    tbody.append(tr)
                    for value in table_header_for_a_test():
                        td = ET.Element('td')
                        td.text = getattr(result, value)
                        tr.append(td)
    return html

def table_header_for_a_solver():
    return  ['case', 'package', 'solver', 'precond', 'nb_proc', 'success', 'error', 'iterations', 'init_time', 'solve_time', 'total_time']

def create_results_for_one_solver(solver, results_list):
    html, body = create_result_page_header()
    h1 = create_a_title('h1', solver)
    body.append(h1)
    cases_for_that_solver = set()
    for result in results_list:
        if result.solver_name == solver:
            cases_for_that_solver.add(result.solver_name)
    for case in cases_for_that_solver:
        tbody = create_a_table_header(h1, table_header_for_a_solver())
        for result in results_list:
            if result.solver_name == solver:
                if not 'original' in result.filename:
                    if result.success == '1':
                        tr = ET.Element('tr', attrib={ 'style': 'background-color:#00b300;color:#ffffff;' })
                    else: #result.success == 0
                        tr = ET.Element('tr', attrib={ 'style': 'background-color:#b30000;color:#ffffff;' })
                    tbody.append(tr)
                    for value in table_header_for_a_solver():
                        td = ET.Element('td')
                        td.text = getattr(result, value)
                        tr.append(td)
    return html

def get_result_list(result_files):
    results_list = []
    for file in result_files:
        if os.path.getsize(file) > 0 and 'coderesult.xml' not in file and 'time_history.xml' not in file:
            tree = ET.parse(file)
            root = tree.getroot()
            test_case = str(get_element_in_xml(root,'input-file').text)
            test_case = os.path.basename(test_case)
            nb_proc = get_element_in_xml(root,'nb-proc').text
            solver = get_element_in_xml(root,'solver')
            solver_name = get_element_in_xml(solver, 'name').text
            package = get_element_in_xml(solver, 'package').text
            max_it = get_element_in_xml(solver, 'max-it').text
            tol = get_element_in_xml(solver, 'tol').text
            if get_element_in_xml(solver, 'solver-algo').text is not None:
                solver_algo = get_element_in_xml(solver, 'solver-algo').text.lower()
            else:
                solver_algo = ''
            if get_element_in_xml(solver, 'precond-algo').text is not None:
                precond_algo = get_element_in_xml(solver, 'precond-algo').text.lower()
            else:
                precond_algo = ''
            actual_precond_name = str()
#            if(package.lower() == 'petsc' and precond_algo == 'diag'):
#                actual_precond_name = precond_algo+'onal'
            if(package.lower() == 'petsc' and precond_algo == 'hypre-amg'):
                actual_precond_name = 'hypre'
#            elif(package.lower() == 'ifpsolver' and precond_algo == 'cpr-amg'):
#                actual_precond_name = 'cpramg'
            elif(package.lower() == 'trilinos' and precond_algo == 'riluk'):
                actual_precond_name = 'iluk'
            elif(package.lower() == 'trilinos' and precond_algo == 'fast_ilu'):
                actual_precond_name = 'filu'
            elif(package.lower() == 'htssolver' and precond_algo == 'chebyshev'):
                precond_name_position = file.lower().find(precond_algo)
                if precond_name_position == -1:
                    actual_precond_name = 'poly'
                else:
                    actual_precond_name = precond_algo
            else:
                actual_precond_name = precond_algo

            precond_name_position = file.lower().find(actual_precond_name)            

            extension_name_position_in_file = file.lower().find('.xml')
            if 'original' not in test_case:
                solver_name = file[ precond_name_position + len(actual_precond_name) + 1 : extension_name_position_in_file ]
                package_name_position = test_case.lower().find(package)
                extension_name_position = test_case.lower().find('.arc')
                test_case = test_case[ : package_name_position - 1] + test_case[ extension_name_position :]
            else:
                original_position = test_case.lower().find('original')
                extension_name_position = test_case.lower().find('.arc')
                test_case = test_case[ : original_position - 1] + test_case[ extension_name_position :]
                original_position_in_file = file.lower().find('original')
                solver_name = file[ original_position_in_file + len('original') + 1 : extension_name_position_in_file ]
            init_time = get_element_in_xml(solver, 'initialisation-time').text
            solve = get_element_in_xml(solver, 'solve')
            nb_calls = get_element_in_xml(solve, 'call').text
            success = get_element_in_xml(solve, 'success').text
            error = get_element_in_xml(solve, 'error').text
            iterations = get_element_in_xml(solve, 'iterations').text
            elapsed_time = get_element_in_xml(solve, 'elapsed-time').text
            total_time = get_element_in_xml(solver, 'total-time').text
            results_list.append(TestCase(file, test_case, nb_proc, solver_name, package, max_it, tol, solver_algo, precond_algo, init_time, nb_calls, success, error, iterations, elapsed_time, total_time))
    return results_list

def remove_invalid_results(results_list):
    new_results_list = [ result for result in results_list if result.iterations != '0' ]
    return new_results_list

def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    tree = ET.parse(config.config_file)
    root = tree.getroot()

    my_input_directory = get_all_elements_in_xml(root, 'output-directory')

    if not os.path.exists('html'):
        os.makedirs('html')

    result_files = get_files_by_extension(my_input_directory, '.xml')
    results_list = get_result_list(result_files)
    results_list = remove_invalid_results(results_list)
    
    results_list.sort(key=lambda test_case: test_case.case)
    test_cases = {result.case for result in results_list}

    if(config.verbose):
        print('I found ' + str(len(test_cases)) + ' test cases')
        for test_case in test_cases:
            print('Case: ' + test_case)

    test_cases_page = ET.Element('html')
    test_cases_body = ET.Element('body')
    test_cases_page.append(test_cases_body)
    test_cases_div = ET.Element('div', attrib={'class': 'div'})
    test_cases_body.append(test_cases_div)

    for test_case in test_cases:
        write_a_html_page('html/' + os.path.splitext(test_case)[0] + '.html', create_results_for_one_test(test_case, results_list))
        link = create_a_link(os.path.splitext(test_case)[0] + '.html', os.path.splitext(test_case)[0])
        p = ET.Element('p')
        test_cases_div.append(p)
        p.append(link)

    test_solvers = {result.solver_name for result in results_list}
    results_list.sort(key=lambda test_case: test_case.solver_name)
    solver_cases = {result.solver_name for result in results_list}

    if(config.verbose):
        print('I found ' + str(len(solver_cases)) + ' different solvers')
        for solver in solver_cases:
            print('Solver: ' + solver)

    solver_cases_page = ET.Element('html')
    solver_cases_body = ET.Element('body')
    solver_cases_page.append(solver_cases_body)
    solver_cases_div = ET.Element('div', attrib={'class': 'div'})
    solver_cases_body.append(solver_cases_div)

    for test_solver in test_solvers:
        write_a_html_page('html/' + os.path.splitext(test_solver)[0] + '.html', create_results_for_one_solver(test_solver, results_list))
        link = create_a_link(os.path.splitext(test_solver)[0] + '.html', os.path.splitext(test_solver)[0])
        p = ET.Element('p')
        solver_cases_div.append(p)
        p.append(link)

    write_a_html_page('html/test_cases.html', test_cases_body)
    write_a_html_page('html/test_solvers.html', solver_cases_body)
    write_a_html_page('html/index.html', create_main_page())

    copy_sortable_script()

if __name__ == "__main__":
    main()
