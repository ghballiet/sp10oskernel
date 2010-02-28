#!/bin/bash

date=`date '+%y%m%d%H%M'`
hrdate=`date`
filename=vmlarix$date.tgz
webpath=/home/faculty2/pyeatt/public_html/cs5352
cd ..
gtar cfz $filename vmlarix
mv $filename $webpath
chmod a+r $webpath/$filename
test='s/<!-- Insert Update Here --!>/LINK_TARGET\
<!-- Insert Update Here --!>/g'
sed -e "$test" $webpath/index.html > tmp
test="s/LINK_TARGET/<li><a href=$filename>$filename<\/a>/g"
sed -e "$test" tmp > $webpath/index.html 
rm -f tmp
