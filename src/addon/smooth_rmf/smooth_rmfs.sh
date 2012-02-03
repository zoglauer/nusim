echo "Splitting rmfs out from original file..."
if test ! -f NuSTAR_FPMB_AveMuTau_2.rmf; then
    cp NuSTAR_FPMB_AveMuTau.rmf NuSTAR_FPMB_AveMuTau_2.rmf
    fdelhdu "NuSTAR_FPMB_AveMuTau_2.rmf[4]" y y
    fdelhdu "NuSTAR_FPMB_AveMuTau_2.rmf[3]" y y
    fdelhdu "NuSTAR_FPMB_AveMuTau_2.rmf[1]" y y
fi
if test ! -f NuSTAR_FPMB_AveMuTau_3.rmf; then
    cp NuSTAR_FPMB_AveMuTau.rmf NuSTAR_FPMB_AveMuTau_3.rmf
    fdelhdu "NuSTAR_FPMB_AveMuTau_3.rmf[4]" y y
    fdelhdu "NuSTAR_FPMB_AveMuTau_3.rmf[2]" y y
    fdelhdu "NuSTAR_FPMB_AveMuTau_3.rmf[1]" y y
fi
if test ! -f NuSTAR_FPMB_AveMuTau_4.rmf; then
    cp NuSTAR_FPMB_AveMuTau.rmf NuSTAR_FPMB_AveMuTau_4.rmf
    fdelhdu "NuSTAR_FPMB_AveMuTau_4.rmf[3]" y y
    fdelhdu "NuSTAR_FPMB_AveMuTau_4.rmf[2]" y y
    fdelhdu "NuSTAR_FPMB_AveMuTau_4.rmf[1]" y y
fi

echo "Smoothing 1st rmf"
echo "tail -f NuSTAR_FPMB_AveMuTau_1_wnoise.log"
smooth_rmf NuSTAR_FPMB_AveMuTau.rmf NuSTAR_FPMB_1_wnoise.rmf 400. >& NuSTAR_FPMB_AveMuTau_1_wnoise.log
mv row1000_conv.dat row1000_conv_1.dat
mv row1000_orig.dat row1000_orig_1.dat 
echo "Smoothing 2nd rmf"
echo "tail -f NuSTAR_FPMB_AveMuTau_2_wnoise.log"
smooth_rmf NuSTAR_FPMB_AveMuTau_2.rmf NuSTAR_FPMB_2_wnoise.rmf 400. >& NuSTAR_FPMB_AveMuTau_2_wnoise.log
mv row1000_conv.dat row1000_conv_2.dat
mv row1000_orig.dat row1000_orig_2.dat 
echo "Smoothing 3rd rmf"
echo "tail -f NuSTAR_FPMB_AveMuTau_3_wnoise.log"
smooth_rmf NuSTAR_FPMB_AveMuTau_3.rmf NuSTAR_FPMB_3_wnoise.rmf 400. >& NuSTAR_FPMB_AveMuTau_3_wnoise.log
mv row1000_conv.dat row1000_conv_3.dat
mv row1000_orig.dat row1000_orig_3.dat 
echo "Smoothing 4th rmf"
echo "tail -f NuSTAR_FPMB_AveMuTau_4_wnoise.log"
smooth_rmf NuSTAR_FPMB_AveMuTau_4.rmf NuSTAR_FPMB_4_wnoise.rmf 400. >& NuSTAR_FPMB_AveMuTau_4_wnoise.log
mv row1000_conv.dat row1000_conv_4.dat
mv row1000_orig.dat row1000_orig_4.dat 

