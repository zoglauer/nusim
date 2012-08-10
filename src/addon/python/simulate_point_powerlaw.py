import nusim_cfg
import os, sys


argv = sys.argv
argc = len(argv)
simulate_bgd = True
flux = 0.
flux_Elow = 10.
flux_Ehi = 30.
gamma = 1.8
if argc < 3:
    #print "Usage: %s outconfigfile exptime (flux)" % argv[0]
    print "Usage: %s outconfigfile exptime (flux gamma flux_Elow flux_Ehi)" % argv[0]
    print "Generates configuration to a point source based on default config in "
    print "  $NUSIM/resource/configurations/AstrophysicsMode.cfg"
    print "Sets exposure time to inputted value"
    print "If given sets flux (input in ergs/s/cm^2) and photon index,"
    print "flux in flux_Elow to flux_Ehi range (def. = %.1f - %.1f keV)" % (flux_Elow, flux_Ehi)
    print "Default photon index = %f" % gamma
    #print "simulate_bgd = 1 (yes, default) or 0 (no)"
    
    sys.exit()

if not os.environ.has_key("NUSIM"):
    print "NUSIM not defined"
    sys.exit()

nusim_root = os.environ["NUSIM"]

incfgfile =nusim_root + "/resource/configurations/AstrophysicsMode.cfg"
if not os.path.exists(incfgfile):
    print "Hmm, %s not found" % incfgfile
    
cfg = nusim_cfg.nusim_cfg(incfgfile)
#print "Read " + incfgfile
outfile = argv[1]
exptime = argv[2] # leave as a string since we are setting xml fields

str = " not"
if simulate_bgd:
    str = ""
print "Background will%s be simulated" % str

flux = 0.
if len(argv) > 3:
    flux = float(argv[3])

if len(argv) > 4:
    gamma = float(argv[4])
if len(argv) > 5:
    flux_Elow = float(argv[5])
if len(argv) > 6:
    flux_Ehi = float(argv[6])

# Get power-law norm
norm = nusim_cfg.pl_norm(flux_Elow, flux_Ehi, gamma, flux)
photflux = nusim_cfg.pl_photflux(3., 82., gamma, norm)
print "Power-law norm to give F(%.1f-%.1f) = %g: %g" % (flux_Elow, flux_Ehi, flux, norm)
print "3-82 keV photon flux = %g" % photflux

# Make sure there is only one pointing here
if len(cfg.pointings_list) != 1:
    print "Expected only one pointing, got %d" % len(cfg.pointings_list)
    sys.exit()

# Set exposure time in pointing and overall obs. time
cfg.pointings_list[0].find("Time").text = exptime
cfg.find("ObservationTime").text = exptime

srcname = "pointsrc_flux%g" % flux
# Update source
cfg.src_list[0].find("Name").text = srcname
cfg.src_list[0].find("EnergyParam1").text = "%g" % 3.
cfg.src_list[0].find("EnergyParam2").text = "%g" % 82.
cfg.src_list[0].find("EnergyParam3").text = "%g" % gamma
cfg.src_list[0].find("Flux").text = "%g" % (photflux/100.)

cfg.find("TargetName").text = srcname
rootname = srcname
#if not simulate_bgd:
#    srcname += "_nobgd"
cfg.find("BaseFileName").text = srcname
cfg.find("SimulateOccultations").text = "false"

# Actually save events list
cfg.find("ModuleOptions/XmlTagEventSelector/SaveEventsAsFits").text = "true"

cfg.write(outfile)

# Actually run nusim
com = nusim_root + "/bin/nusim -c " + outfile + " -a"
import popen2
proc = popen2.Popen4(com)
inf = proc.fromchild
line = " "
while line != "":
    line = inf.readline()
    print line.strip()
