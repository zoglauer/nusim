#!/usr/local/bin/bash

# usage: makespec.sh dirname X Y

if [ $# != 0 ] ; then
    echo Syntax:  invert_pixpos_new
    exit 0
fi


export CONV_PATH=$NUSIM/resource/examples/AddOn/convert_nusim_to_fits

DATAHOME=/home/nustar1
# Init CALDB environmental variables:
. $DATAHOME/SOC/CALDB/software/tools/caldbinit.sh
HEADAS=/usr/local/heasoft

# Modify below if not running on sarasvarti for your normal IDL pointers...
# system IDL setup
/usr/local/rsi/idl/bin/idl_setup.bash

# IDL startup for batch jobs
export IDL_STARTUP=$CONV_PATH/startup/defb.pro

# defb.pro includes the builds of the astrolib, which is required for
# all of this to work.

idl -quiet invert_pixpos_new.bat


