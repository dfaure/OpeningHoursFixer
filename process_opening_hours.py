#!/usr/bin/env python3
# https://docs.python.org/3/library/xml.etree.elementtree.html#module-xml.etree.ElementTree
import xml.etree.ElementTree as ET
import os
from PyKOpeningHours.PyKOpeningHours import OpeningHours, Error

xmlfile = open("data/osm.xml", "r")
response = xmlfile.read()

# parse XML
root = ET.fromstring(response)
tree = ET.ElementTree(root)
hours = {} # type/id -> [old, new]
for child in root:
    if child.tag == 'node' or child.tag == 'way' or child.tag == 'relation':
        key = child.tag + '/' + child.get('id')
        old_opening_hours_tag = child.find("./tag[@k='opening_hours']")
        if not old_opening_hours_tag is None:
            old_opening_hours = old_opening_hours_tag.get('v')
            parser = OpeningHours()
            parser.setExpression(old_opening_hours)
            if parser.error() == Error.SyntaxError or parser.error() == Error.IncompatibleMode:
                new_oh = ''
            else:
                new_oh = parser.normalizedExpression()
            if old_opening_hours != new_oh:
                hours[key] = [old_opening_hours, new_oh]
        else:
            print('ERROR: no opening_hours found in ' + key)

# save
class CurrentOutput:
    file_counter = 0 # C++ static
    def __init__(self):
        self.fileName = 'data/editme-' + str(CurrentOutput.file_counter) + '.txt'
        self.file = open(self.fileName, 'w')
        self.line_counter = 0
    def add(self, lines):
        self.file.write(lines)
        self.line_counter += 1
        return self.line_counter
    def close(self):
        self.file.close()
        CurrentOutput.file_counter += 1

out = CurrentOutput()
for key in sorted(hours.keys()):
    entry = hours[key]
    lines = '{}\n  OLD {}\n  NEW {}\n'.format(key, entry[0], entry[1])
    if out.add(lines) > 99:
        out.close()
        out = CurrentOutput()

out.close()
