#!/bin/sh

# data/done-0.txt => data/done-0.osm
./create_changes.py

# data/done-0.osm => changes/*
./filter_changes.py

password=`pass show osmbot-openstreetmap.org | head -n 1`
date=`date +%Y-%m-%d`
version=`cat ./version`

for changeset in changes/*.osc; do

    comment="Fixing opening_hours syntax using KOpeningHours and manual review"

    ../osm-bulk-upload/upload.py -u davidfaure_bot -p $password -c yes -m "$comment" $changeset \
    -x "OpeningHoursFixer $version, via osm-bulk-upload/python.py" -z "https://github.com/dfaure/OpeningHoursFixer"
done
