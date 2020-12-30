# OpeningHoursFixer
Fix `opening_hours` syntax in OSM data.

This uses the errors found by Osmose as a TODO list,
and uses https://invent.kde.org/libraries/kopeninghours for the parsing and reformatting,
and then lets the user manually review the changes, and fix by hand what the parser failed to understand.

The idea is that it's faster to edit a text file (with a diff-like layout), than to click-click-click
in Osmose, waiting for the server to reply at every step.

