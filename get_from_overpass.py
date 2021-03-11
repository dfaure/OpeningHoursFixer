#!/usr/bin/env python3
# https://github.com/mvexel/overpass-api-python-wrapper
import overpass
# https://docs.python.org/fr/3/library/json.html
import json
import re
import glob

api = overpass.API(timeout=3000)

regexp = re.compile("https://www.openstreetmap.org/([^/]+)/([0-9]+)")

nodes = []
ways = []
relations = []

jsonfilenames = glob.glob('data/*.json', recursive=False)
for filename in jsonfilenames:
    jsonfile = open(filename)
    jsondata = json.load(jsonfile)
    print("{0}: {1} issues".format(filename, len(jsondata['issues'])))
    for jsonissue in jsondata['issues']:
        json_osm_id = jsonissue['osm_ids']
        if 'nodes' in json_osm_id:
            for id in json_osm_id['nodes']:
                nodes.append(str(id))
        if 'ways' in json_osm_id:
            for id in json_osm_id['ways']:
                ways.append(str(id))
        if 'relations' in json_osm_id:
            for id in json_osm_id['relations']:
                relations.append(str(id))

# https://wiki.openstreetmap.org/wiki/Overpass_API/Overpass_QL#By_element_id
useArea = False
if nodes:
    nodeQuery = 'node(id:' + ','.join(nodes) + ')' + ('(area)' if useArea else '') + ';'
else:
    nodeQuery = ''
if ways:
    wayQuery = 'way(id:' + ','.join(ways) + ')' + ('(area)' if useArea else '') + ';'
else:
    wayQuery = ''
if relations:
    relationQuery = 'relation(id:' + ','.join(relations) + ')' + ('(area)' if useArea else '') + ';'
else:
    relationQuery = ''
query = '(' + nodeQuery + wayQuery + relationQuery + ')'
if useArea:
    query = 'area[name="France"]; ' + query
response = api.get(query, responseformat="xml", verbosity='meta center qt')
xmlfile = open("data/osm.xml", "w+")
xmlfile.write(response)
