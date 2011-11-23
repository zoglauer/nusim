import nusim_cfg
import os, sys


argv = sys.argv
argc = len(argv)

if argc < 3:
    print "Usage: %s outconfigfile exptime (flux)" % argv[0]
    print "Generates configuration to a point source based on default config in "
    print "  $NUSIM/resource/configurations/AstrophysicsMode.cfg"
    print "Resets exposure time and flux (in photons/s/cm^2, if given) to inputted values"
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

# Make sure there is only one pointing here
if len(cfg.pointings_list) != 1:
    print "Expected only one pointing, got %d" % len(cfg.pointings_list)
    sys.exit()

# Set exposure time in pointing and overall obs. time
cfg.pointings_list[0].find("Time").text = exptime
cfg.find("ObservationTime").text = exptime

# Actually save events list
cfg.find("ModuleOptions/XmlTagEventSelector/SaveEventsAsFits").text = "true"

# "bug" fix
cfg.find("ModuleOptions/XmlTagModuleOrbitEngineTLE/TLEFileName").text = \
     nusim_root + "resource/data/Launch.tle"

cfg.write(outfile)

# Actually run nusim
com = nusim_root + "bin/nusim -c " + outfile + " -a"
import popen2
proc = popen2.Popen4(com)
inf = proc.fromchild
line = " "
while line != "":
    line = inf.readline()
    print line.strip()
