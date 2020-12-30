#!/bin/sh

wget 'http://osmose.openstreetmap.fr/en/errors/?limit=50000&class=32501&item=3250' -O - | grep _blank | \
    perl -e 'while (<>) { print "$1\n" if (/href=\"([^\"]*)\"/); }' > data/list.txt

# data/list.txt => data/osm.xml
./get_from_overpass.py

rm -f data/editme*
# data/osm.xml => data/editme-*.txt
./process_opening_hours.py

echo 'Now edit data/editme-*.txt, save as data/done-*.txt, then run upload_changes.sh'
