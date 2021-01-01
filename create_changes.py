#!/usr/bin/env python3
# https://docs.python.org/3/library/xml.etree.elementtree.html#module-xml.etree.ElementTree
import xml.etree.ElementTree as ET
import os
from PyKOpeningHours.PyKOpeningHours import OpeningHours, Error

# Parse edited data
fileName = 'data/done-0.txt'
hours = {}
with open(fileName) as f:
    while True:
        key = f.readline().rstrip('\n')
        if key == '':
            break
        old = f.readline()[6:].rstrip('\n')
        new = f.readline()[6:].rstrip('\n')
        if old != new and new != '':
            hours[key] = [old, new]
            #print('IN: {} {} -> {}'.format(key, old, new))

# Read XML
xmlfile = open("data/osm.xml", "r")
response = xmlfile.read()

# parse and modify XML
root = ET.fromstring(response)
tree = ET.ElementTree(root)
for child in root:
    if child.tag == 'node' or child.tag == 'way' or child.tag == 'relation':
        key = child.tag + '/' + child.get('id')
        if key in hours:
            [old_oh, new_oh] = hours[key]
            #print('OUT: {} {} -> {}'.format(key, old_oh, new_oh))
            old_opening_hours_tag = child.find("./tag[@k='opening_hours']")
            if not old_opening_hours_tag is None:
                old_opening_hours = old_opening_hours_tag.get('v')
                if False and old_opening_hours != old_oh: # just my own mistakes, ignore
                    print('opening_hours changed in OSM meanwhile! {}\n{}\n{}'.format(key, old_opening_hours, old_oh))
                else:
                    old_opening_hours_tag.set('v', new_oh)
                    child.set('X-reason', 'update_') # for filter_changes.py
                    child.set('action', 'modify')

tree.write('data/done-0.osm', 'unicode', True)
