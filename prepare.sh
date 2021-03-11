#!/bin/sh

rm -f data/list.txt data/*.json

for val in france italy germany; do
  # the max limit is 500, says https://wiki.openstreetmap.org/wiki/Osmose/api/0.3
  wget "https://osmose.openstreetmap.fr/api/0.3/issues?item=3093&item=3250&class=32501&country=${val}*&full=true&limit=500" -O data/$val.json
done

# data/ errors.json => data/osm.xml
echo "Querying overpass..."
./get_from_overpass.py

echo "Processing..."
rm -f data/editme* data/err-editme*
# data/osm.xml => data/editme-*.txt
./process_opening_hours.py

echo 'Now edit one of the data/editme-*.txt files, save it as data/done.txt, then run upload_changes.sh, repeat'
