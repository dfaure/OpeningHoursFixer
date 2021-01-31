#!/usr/bin/env python3
import os
import subprocess
import shutil
import copy
import math
import xml.etree.ElementTree as ET

# Prepare output dir
if os.path.exists('changes'):
    shutil.rmtree('changes')
os.mkdir('changes')

class CurrentOutput:
    file_counter = 0           # C++ static
    object_counter = 0         # C++ static
    def __init__(self, root, prefix):
        self.hours_out = 'changes/' + prefix + str(CurrentOutput.file_counter) + '.hours'
        self.my_file_counter = CurrentOutput.file_counter
        self.xmlroot = ET.Element(root.tag)
        self.xmlroot.attrib = copy.deepcopy(root.attrib)
        self.count = 0

    def write_and_close(self, prefix):
        if self.count > 0:
            CurrentOutput.object_counter += self.count
            osm_out = 'changes/' + prefix + str(self.my_file_counter) + '.osm'
            ET.ElementTree(self.xmlroot).write(osm_out, 'unicode', True)
            # Create the corresponding ".osc" file
            subprocess.call(["python3", "../osm-bulk-upload/osm2change.py", osm_out])
            CurrentOutput.file_counter += 1

    def keep(self, child):
        return True

    def add(self, child):
        # https://stackoverflow.com/questions/15527399/python-elementtree-inserting-a-copy-of-an-element
        self.xmlroot.append(ET.ElementTree(child).getroot())
        self.count += 1

    def enlarge_area(self, child):
        lat_str = child.get('lat')
        lon_str = child.get('lon')
        if lat_str is None or lon_str is None:
            center = child.find("center")
            if center is None:
                print('No lat/lon nor center for id ' + child.get('id'))
            lat_str = center.get('lat')
            lon_str = center.get('lon')
        if lat_str is None or lon_str is None:
            print('Missing lat/lon for id ' + child.get('id'))
        lat = float(lat_str)
        lon = float(lon_str)
        if self.count == 0:
            self.min_lat = lat
            self.max_lat = lat
            self.min_lon = lon
            self.max_lon = lon
        else:
            self.min_lat = min(self.min_lat, lat)
            self.max_lat = max(self.max_lat, lat)
            self.min_lon = min(self.min_lon, lon)
            self.max_lon = max(self.max_lon, lon)

    # area = (2*pi)*R^2 |sin(lat1)-sin(lat2)| |lon1-lon2| / 360
    # http://mathforum.org/library/drmath/view/63767.html
    def area(self):
        size = math.pi * 2 * 6357 * 6357 * abs(math.sin(math.radians(self.max_lat)) - math.sin(math.radians(self.min_lat))) * abs(self.max_lon - self.min_lon) / 360
        #print('  lat {} - {}, lon {} - {} => area {}'.format(self.min_lat, self.max_lat, self.min_lon, self.max_lon, size))
        return size

tree = ET.parse('data/done.osm')
root = tree.getroot()

current_out_dict = {}

for child in root:
    if (child.tag == 'node' or child.tag == 'way') and child.get('action') == 'modify':
        reason = child.get('X-reason', '')
        if reason != '':
            child.attrib.pop('X-reason')
        if not reason in current_out_dict:
            current_out_dict[reason] = CurrentOutput(root, reason)
        current_out = current_out_dict[reason]
        if current_out.keep(child):

            current_out.enlarge_area(child)
            if current_out.area() > 500: # km^2
                current_out.write_and_close(reason)
                #print('starting new chunk')
                current_out_dict[reason] = CurrentOutput(root, reason)
                current_out = current_out_dict[reason]
                current_out.enlarge_area(child) # initialize with this one
            current_out.add(child)

for reason, current_out in current_out_dict.items():
    current_out.write_and_close(reason)

if CurrentOutput.file_counter == 0:
    print("nothing to upload")
else:
    print(str(CurrentOutput.file_counter) + " file(s) created, to upload changes to " + str(CurrentOutput.object_counter) + " object(s)")
