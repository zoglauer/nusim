#!/bin/bash

help() {
    echo ""
    echo "FixPath script for NuSIM configuration file";
    echo "This script looks for the \$NUSIM path and replaces it in the following manner:"
    echo "/home/andreas/nusim/Myfile.dat"
    echo "with: "
    echo "\$NUSIM/MyFile.dat"
    echo "";
    echo "Usage: FixPath filename1 filename2 ...";
    echo "";
}



if [ $# -lt 1 ] ; then
    help; 
    exit 0;
fi;

files=" "
while [ $# -gt 0 ] ; do
    files="$files $1"
    shift
done

start="$NUSIM"
stop="\$NUSIM"

echo "Replace $start with $stop"

for file in $files; do
    echo "  Analysing file: $file";
    newfile="/tmp/"$file".new";
    sed "s|$start|$stop|g" $file > $newfile;
    ret=`diff $file $newfile`;
    if (`test -z "$ret"`); then
        echo "    No modification necessary";
        rm -f $newfile
    else
        echo "    Updating file";
        mv $newfile $file;
    fi
done

exit
