import xml.etree.ElementTree as ET
import sys
import os
import shutil

def write_a_html_page(filename, page):
    with open(filename, 'w') as file:
        if sys.version_info < (3, 0, 0):
            # python 2
            ET.ElementTree(page).write(file, encoding='utf-8', method='html')
        else:
            # python 3
            ET.ElementTree(page).write(file, encoding='unicode', method='html')

def create_main_page():
    html = ET.Element('html')
    body = ET.Element('body')
    html.append(body)
    div = ET.Element('div', attrib={'class': 'div'})
    body.append(div)
    para1 = ET.Element('p')
    div.append(para1)
    link_test_case = ET.Element('a', attrib={'href' : 'test_cases.html'})
    link_test_case.text = 'Results by cases'
    para1.append(link_test_case)
    para2 = ET.Element('p')
    div.append(para2)
    link_solvers = ET.Element('a', attrib={'href' : 'test_solvers.html'})
    link_solvers.text = 'Results by solvers'
    para2.append(link_solvers)
    return html

def create_a_title(heading, text):
    h = ET.Element(heading)
    h.text = text
    return h

def create_a_link(dst, text):
    link = ET.Element('a', attrib={'href' : dst})
    link.text = text
    return link

def create_result_page_header():
    html = ET.Element('html')
    head = ET.Element('head')
    html.append(head)
    script = ET.Element('script', attrib={'type' : 'text/javascript', 'src' : 'sorttable.js' })
    head.append(script)
    body = ET.Element('body')
    html.append(body)
    return html, body

def create_a_table_header(h2, header):
    table = ET.Element('table', attrib= { 'class' : 'sortable' })
    h2.append(table)
    thead = ET.Element('thead')
    table.append(thead)
    tbody = ET.Element('tbody')
    for head in header:
        td = ET.Element('td')
        td.text = head
        thead.append(td)
    table.append(tbody)
    return tbody

def copy_sortable_script():
    if not os.path.exists('html/sorttable.js'):
        shutil.copy2('sorttable.js', 'html/sorttable.js')
