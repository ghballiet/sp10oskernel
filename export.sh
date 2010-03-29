#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: export.sh project_number"
    exit -1
fi

# export and tar current file
name='glhunt/ phwilcox/'
project=$1
file="$(date +%m%d)_project$project.tgz"

svn up
for n in `echo $name`; do
    svn export current/ $n
done
tar rvf $file $name/
for n in `echo $name`; do
    rm -rf $n
done
mv $file archive/
svn add archive/*
svn ci -m "archived: $file"