#!/bin/bash

help() {
    echo ""
    echo "Replace script for NuSIM configuration file";
    echo "  based upon MEGAlib's replace script." 
    echo "";
    echo "Usage: replace 'original string' 'new string' filename1 filename2 ...";
    echo "";
    echo "Remark 1: The quotation marks are only necessary if the string contains space characters.";
    echo "";
}



if [ $# -lt 3 ] ; then
    help; 
    exit 0;
fi;


start=$1
shift
stop=$1
shift
files=" "
while [ $# -gt 0 ] ; do
    files="$files $1"
    shift
done

types="cfg"



# Do some sanity checks on the input:
substr() {
  echo "$1" | cut -c$2-`expr $2 + $3 - 1`
}

length=${#start}

last='-'
for inc in `seq 1 $length`; do
    char=`substr "${start}" ${inc} 1`
    if ([ "$char" == "/" ] && [ "$last" != "\\" ]); then 
        echo "Error: All \"/\" must be preceded by a \"\\\"!"
        exit 1;
    fi
    last=$char
done

length=${#stop}

last='-'
for inc in `seq 1 $length`; do
    char=`substr "${stop}" ${inc} 1`
    if ([ "$char" == "/" ] && [ "$last" != "\\" ]); then 
        echo "Error: All \"/\" must be preceded by a \"\\\"!"
        exit 1;
    fi
    last=$char
done

current=`pwd`

echo $start $stop $files

for file in $files; do
    echo "  Analysing file: $file";
    newfile="/tmp/"$file".new";
    sed "s/$start/$stop/g" $file > $newfile;
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
