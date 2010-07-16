#! /bin/bash

EXCLUDED="MGlobal.h MGlobal.cxx MFileResponse.cxx"

cd $NUSIM/src/megalib
MEGALIB_H=`find . -name "*.h" -printf "%f "`
MEGALIB_C=`find . -name "*.cxx" -printf "%f "`

cd $NUSIM
if ( test ! -d tmp_megalib); then 
    echo "Checking out MEGAlib..."
    mkdir tmp_megalib
    cd tmp_megalib
    cvs -d :pserver:anonymous@cvs.mpe.mpg.de:/home/zoglauer/Repository -z3 co -P MEGAlib
    cd MEGAlib/src
else 
    echo "Updating MEGAlib..."
    cd tmp_megalib
    cvs update
    cd MEGAlib/src
fi

for i in $MEGALIB_H; do
    echo "Testing: $i"

    # Check if the file is in the exclude list:
    ISEXCLUDED="NO"
    for e in $EXCLUDED; do
        if [ "$i" == "$e" ]; then
            ISEXCLUDED="YES"
        fi
    done
    if [ "${ISEXCLUDED}" == "YES" ]; then
        echo "Excluded!"
        continue
    fi

    # Find the file in the MEGAlib directory:
    F=`find . -name "$i"`

    if [ "$F" != "" ]; then
        # and replace it
        echo "Replacing ${NUSIM}/src/megalib/inc/$i with $F..."
        cp ${F} ${NUSIM}/src/megalib/inc/${i}
     fi
done

for i in $MEGALIB_C; do
    echo "Testing: $i"

    # Check if the file is in the exclude list:
    ISEXCLUDED="NO"
    for e in $EXCLUDED; do
        if [ "$i" == "$e" ]; then
            ISEXCLUDED="YES"
        fi
    done
    if [ "${ISEXCLUDED}" == "YES" ]; then
        echo "Excluded!"
        continue
    fi

    # Find the file in the MEGAlib directory:
    F=`find . -name "$i"`

    if [ "$F" != "" ]; then
        # and replace it
        echo "Replacing ${NUSIM}/src/megalib/src/$i with $F..."
        cp ${F} ${NUSIM}/src/megalib/src/${i}
    fi
done

cd $NUSIM
#rm -t tmp_megalib