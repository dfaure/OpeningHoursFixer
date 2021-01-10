#!/usr/bin/env python3
# https://docs.python.org/3/library/xml.etree.elementtree.html#module-xml.etree.ElementTree
import xml.etree.ElementTree as ET
import os
import sys
from PyKOpeningHours.PyKOpeningHours import OpeningHours, Error

xmlfile = open("data/osm.xml", "r")
response = xmlfile.read()

blacklist = {}
if os.path.exists('blacklist'):
    with open('blacklist') as f:
        for line in f.readlines():
            key = line.rstrip('\n')
            if key != '':
                blacklist[key] = 1

# parse XML
root = ET.fromstring(response)
tree = ET.ElementTree(root)
hours = {} # type/id -> [old, new]
for child in root:
    if child.tag == 'node' or child.tag == 'way' or child.tag == 'relation':
        key = child.tag + '/' + child.get('id')
        if not key in blacklist:
            old_opening_hours_tag = child.find("./tag[@k='opening_hours']")
            if not old_opening_hours_tag is None:
                old_opening_hours = old_opening_hours_tag.get('v')
                parser = OpeningHours()
                parser.setExpression(old_opening_hours)
                if parser.error() == Error.SyntaxError or parser.error() == Error.IncompatibleMode:
                    new_oh = ''
                else:
                    new_oh = parser.normalizedExpression()
                    if new_oh != '':
                        parser.setExpression(new_oh)
                        if parser.error() == Error.SyntaxError:
                            print('ERROR: cannot parse my own expression back:\n' + old_opening_hours + '\n' + new_oh)
                            sys.exit(1)
                simplify = lambda s: s.replace('24:00', '00:00')
                if simplify(old_opening_hours) != simplify(new_oh):
                    hours[key] = [old_opening_hours, new_oh]
            else:
                print('ERROR: no opening_hours found in ' + key)

# save
class CurrentOutput:
    file_counter = 0 # C++ static
    def __init__(self, prefix):
        self.fileName = 'data/' + prefix + 'editme-' + str(CurrentOutput.file_counter) + '.txt'
        self.file = open(self.fileName, 'w')
        self.line_counter = 0
    def add(self, lines):
        self.file.write(lines)
        self.line_counter += 1
        return self.line_counter
    def close(self):
        self.file.close()
        CurrentOutput.file_counter += 1

out = CurrentOutput('')
out_err = CurrentOutput('err-')
for key in sorted(hours.keys()):
    entry = hours[key]
    lines = '{}\n  OLD {}\n  NEW {}\n'.format(key, entry[0], entry[1])
    if entry[1] == '':
        if out_err.add(lines) > 99:
            out_err.close()
            out_err = CurrentOutput('err-')
    else:
        if out.add(lines) > 99:
            out.close()
            out = CurrentOutput('')

out.close()
