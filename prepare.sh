#!/bin/sh

rm -f data/list.txt

#grep france country_list.txt | while read line; do
#    val=`echo $line | sed -e 's/^[^"]*"//;s/".*//'`
#    echo $val
#
#    wget "http://osmose.openstreetmap.fr/en/errors/?country=$val&limit=10000&class=32501&item=3250" -O - | grep _blank | \
#        perl -e 'while (<>) { print "$1\n" if (/href=\"([^\"]*)\"/); }' >> data/list.txt
#done

grep _blank /tmp/Osmose.html | \
    perl -e 'while (<>) { print "$1\n" if (/href=\"([^\"]*)\"/); }' >> data/list.txt

# data/list.txt => data/osm.xml
echo "Querying overpass..."
./get_from_overpass.py

echo "Processing..."
rm -f data/editme* data/err-editme*
# data/osm.xml => data/editme-*.txt
./process_opening_hours.py

echo 'Now edit one of the data/editme-*.txt files, save it as data/done.txt, then run upload_changes.sh, repeat'
