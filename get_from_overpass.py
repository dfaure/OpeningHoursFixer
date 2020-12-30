#!/usr/bin/env python3
# https://github.com/mvexel/overpass-api-python-wrapper
import overpass
import re

api = overpass.API(timeout=3000)

regexp = re.compile("https://www.openstreetmap.org/([^/]+)/([0-9]+)")

nodes = []
ways = []
relations = []

with open('data/list.txt') as f:
    for line in f.readlines():
        match = re.match(regexp, line)
        if match is None:
            print('bad syntax: ' + line)
        else:
            type = match.group(1)
            id = match.group(2)
            if type == 'node':
                nodes.append(id)
            elif type == 'way':
                ways.append(id)
            elif type == 'relation':
                relations.append(id)
            else:
                print('unsupported type ' + type)

# https://wiki.openstreetmap.org/wiki/Overpass_API/Overpass_QL#By_element_id
nodeQuery = 'node(id:' + ','.join(nodes) + ')'
wayQuery = 'way(id:' + ','.join(ways) + ')'
relationQuery = 'relation(id:' + ','.join(relations) + ')'
query = '(' + nodeQuery + ';' + wayQuery + ';' + relationQuery + ';)'
response = api.get(query, responseformat="xml", verbosity='meta center qt')
xmlfile = open("data/osm.xml", "w+")
xmlfile.write(response)
