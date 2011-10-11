#!/bin/sh
# Perform a test run of the NuSIM attitude file

export OBS_ID='10002002001'
export TARG_ID='0'
export OBJECT='3C273'

# These were not defined in the example startracker file.
# setenv RA_OBJ
# setenv DEC_OBJ
# setenv RA_NOM
# setenv DEC_NOM
# setenv RA_PNT
# setenv DEC_PNT
# setenv PA_PNT

SIMPATH=/users/nusim/soft/NuSim/svn/KKMSims
INFILE=${SIMPATH}/Yaw0test.startracker.fits
TLEFILE=${SIMPATH}/tle_nustar_2012_034.dat
OUTFILE=${SIMPATH}/Yaw0test.attitude.fits

L1_att_write.pl $INFILE $TLEFILE $OUTFILE
INFILE=${SIMPATH}/Yaw45test.startracker.fits
OUTFILE=${SIMPATH}/Yaw45test.attitude.fits
L1_att_write.pl $INFILE $TLEFILE $OUTFILE
