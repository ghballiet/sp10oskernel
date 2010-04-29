#!/bin/bash

OLDPWD=$PWD
COMPILE_ROOT="/home/glhunt/svn/sp10oskernel/current"
MKDIRS="usr/ vmlarix/"

cd $COMPILE_ROOT && \
echo "Updating source from repository..." && \
svn up && \

for f in $MKDIRS; do
    cd $f && \
    echo -n "Compiling $f..." && \
    make >/dev/null && echo "done." || (echo "ERROR!" && return 1)
    cd $COMPILE_ROOT
done

cd $OLDPWD