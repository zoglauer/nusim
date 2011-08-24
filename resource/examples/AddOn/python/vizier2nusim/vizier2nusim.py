"""
This is a script to read in a Vizier table and use ra, dec, and flux columns to
generate text-file source list input for NuSim
The conversion from the flux in the table and the NuStar band must be supplied
Right now the same spectral model is applied to all sources
A. Ptak 9 Aug 2011
"""

#try:
#    from pylab import *
#    _have_pylab = True
#except:
#    print "pylab not found or can't be loaded, will no make plots"
#    _have_pylab = False
    
from math import *
import sys
import read_vizier
import radec

argv = sys.argv
elow=6.
ehi=80.
model = "PowerLaw"
par1 = 1.8 # photon index
par2 = None
fluxconv = 0.2436 / 6.42e-9 # 6-80 keV photon flux / 0.3-10 keV flux for gamma=1.8
argc = len(argv)

if argc < 6:
    print "Usage: vizier2nusim.py nusimfile tblfile racol deccol fluxcol fluxconv (elow ehi modelname par1 par2)"
    print "fluxconv gives conversion of fluxes to NuSTAR band, in"
    print "energy range given by elow - ehi (def. = %.1f - %.1f)" % (elow, ehi)
    print "Model name = PowerLaw or BrokenPowerLaw (def. = %s)" % model
    print "Parameters = gamma (powerlaw) or break energy, gamma_low, gamma_hi (broken powerlaw)"
    print "Example: vizier2nusim.py M33_nusim_srcs.dat M33_paper.tsv RAJ2000 DEJ2000 CFlux %.3g %.1f %.1f %s %.2f"  % (fluxconv, elow, ehi, model, par1)
    sys.exit()

# Read in table
nusimfile = argv[1]
vfile = argv[2]
data = read_vizier.read_vizier_table(vfile)

racol = argv[3]
deccol = argv[4]
fluxcol = argv[5]
if racol not in data:
    print "Error, ra column %s not found in %s" % (racol, vfile)
if deccol not in data:
    print "Error, dec column %s not found in %s" % (deccol, vfile)
if fluxcol not in data:
    print "Error, flux column %s not found in %s" % (fluxcol, vfile)

fluxconv = float(argv[6])

if argc > 7:
    elow = float(argv[7])
if argc > 8:
    ehi = float(argv[8])
if argc > 9:
    model = argv[9]
if argc > 10:
    par1 = float(argv[10])
if argc > 11:
    par2 = float(argv[11])

modnums = {"PowerLaw":3, "BrokenPowerLaw":4}
if model not in modnums:
    print "Unsupported model:", model
    sys.exit()
    
maxflux = 0.
n = len(data[racol])
maxi = -1
for i in range(n):
    #print i, data[racol][i], data[deccol][i], data[fluxcol][i]
    if data[fluxcol][i] > maxflux:
        maxi = i
        maxflux = data[fluxcol][i]
print "Max. flux %.3g found at source %d: %s %s" % (maxflux, maxi+1, data[racol][maxi], data[deccol][i])

# write out nusim source format list
fil = open(nusimfile, 'w')
# Need to convert 2-10 kev flux to 6-80 keV photon flux
# for gamma=1.8, F(2-10) = 3.5e-11 has 6-80 kev photon flux = 0.0052
if par2 is None:
    str = "%s 1 %.3f %.4f %d %f %f %f %g\n"
else:
    str = "%s 1 %.3f %.4f %d %f %f %f %f %g\n"
    

for i in range(n):
    #if i != inucl:
    #    fil.write(str % (data[i].ra, data[i].dec, data[i].flux*0.293))
    #    f_20_50 = data[i].flux*0.545
    id = "src%d" % (i+1)
    try:
        posn = radec.radec(data[racol][i], data[deccol][i])
    except:
        print "Invalid position for source %d: %s %s" % (i+1, data[racol][i], data[deccol][i])
    if par2 is None:
        fil.write(str % (id, posn.ra, posn.dec, modnums[model], elow, ehi, par1, data[fluxcol][i]*fluxconv))
    else:
        fil.write(str % (id, posn.ra, posn.dec, modnums[model], elow, ehi, par1, par2, par3, data[fluxcol][i]*fluxscale))

