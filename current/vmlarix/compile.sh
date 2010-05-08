#!/bin/bash

HOME_DIR='/home/glhunt/svn/sp10oskernel/current/'
BUILD_DIRS='usr/ vmlarix/'

cd $HOME_DIR
svn up
for d in $BUILD_DIRS; do
    echo "Building $d..."
    cd $d
    make >/dev/null
    echo "done."
    cd $HOME_DIR
done
cd $HOME_DIR
cd vmlarix
echo "Running simics..."
./startsimics.sh