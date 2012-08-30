"""
Class to manage nusim config files
"""
import sys, os
from xml.etree.ElementTree import ElementTree
from radec import radec
import numpy
# This approach will break if nusim changes these options of course!
specoptions = ["Monoenergetic", "Linear", "PowerLaw", "BrokenPowerLaw", "FileDifferentialFlux", "BlackBody", "NormalizedFunctionInPhPerCm2PerSPerKeV", "NormalizedEnergyPositionFluxFunction"]

beamoptions = ["FarFieldPoint", "FarFieldDisk", "NearFieldPoint", "NearFieldBeam", "FarFieldFitsFile", "FarFieldNormalizedEnergyPositionFluxFunction"]

# Two obvious design choices here, having
# nusim_cfg inheret from ElementTree or have it have an
# ElementTree as member data
# Difference in practice is likely to be mostly how often
# low-level access to the XML is likely to be needed
# probably would be handy to have that for automating
# the running of a large number of sims
class nusim_cfg(ElementTree):

    def __init__(self, cfgfile):
        """
        cfgfile required right now since may never be able to make
        nusim cfg files from scratch, don't see obvious need for that
        i.e., we'd minimally want to start from a cfg file in the nusim
        distribution
        Example:
        c  = nusim_cfg("nusim/resource/configurations/AstrophysicsMode.cfg")
        """

        ElementTree.__init__(self)
        self.parse(cfgfile)

        print "Read " + cfgfile
        v = self.find("NuSIMRevision")
        self.version = int(v.text)
        t = self.find("ObservationTime")
        self.tottime = float(t.text)
       
        self.pointings = self.find("ModuleOptions/XmlTagModulePointingPredefined/Pointings")
        
        try: # first try python 2.7 version of xml.etree
            self.pointings_iter = list(self.pointings.iter("Pointing"))
        except:
            self.pointings_iter = list(self.pointings.getiterator("Pointing"))
        self.pointings_list = list(self.pointings_iter)
        self.sources = self.find("ModuleOptions/XmlTagSourceDistribution/Sources")
        try: # first try python 2.7 version of xml.etree
            self.src_list = list(self.sources.iter("Source"))
        except:
            self.src_list = list(self.sources.getiterator("Source"))
            
    def reset_srclist(self):
        try: # first try python 2.7 version of xml.etree
            self.src_list = list(self.sources.iter("Source"))
        except:
            self.src_list = list(self.sources.getiterator("Source"))
            
    def report(self):
        """ Dump some supposedly relevant info """
        print "NuSIM version that saved this configuration file = %d" % self.version
        print "Total observation time = %.1f ks" % (self.tottime/1000.)
        point_centers = []
        print "%d pointing(s) defined" % len(self.pointings_list)
        i = 1
        for pointing in self.pointings_list:
            RA = float(pointing.find("RA").text)/60.
            DEC = float(pointing.find("DEC").text)/60.
            yawflag = True
            try:
                ang = float(pointing.find("Yaw").text)
            except:
                yawflag = False
                ang = float(pointing.find("Roll").text)
            exptime = float(pointing.find("Time").text)
            p = radec()
            p.ra, p.dec = RA, DEC
            point_centers.append(p)
            s = "Pointing %d:\n(%f, %f) %s roll=%.1f deg., exposure time = %.1f ks\n"
            if yawflag:
                s = s.replace("roll", "yaw")
            print s % (i, RA, DEC, p, ang/60., exptime/1000.)
            i = i + 1
            
        #print p

        print "%d source(s)" % len(self.src_list)
        srci = 1
        totflux = 0.
        totbatflux = 0.
        for src in self.src_list:
            name = src.find("Name").text
            print "Source %d: " % srci,  name
            srci = srci +1
            stype = int(src.find("SpectralType").text)-1
            spectype = specoptions[stype]
            print "  spectral option = %s" % (spectype)
            btype = int(src.find("BeamType").text)-1
            beamtype = beamoptions[btype]
            print "  spatial option = %s" % beamtype
            photflux = float(src.find("Flux").text)*100.
            ra = float(src.find("PositionParam2").text)/60.
            dec = float(src.find("PositionParam1").text)/60.
            p = radec()
            p.ra, p.dec = ra,dec
            print "  photon flux = %g ph/s/cm^2" % (photflux)
            if spectype == "PowerLaw":
                E1 = float(src.find("EnergyParam1").text)
                E2 = float(src.find("EnergyParam2").text)
                gamma = float(src.find("EnergyParam3").text)
                print "  E1, E2, Gamma = %.2f, %.2f, %.2f" % (E1, E2, gamma)
                norm = photflux/pl_photflux(E1, E2, gamma, 1.)
                print " PL norm = ", norm
                flux = pl_flux(E1, E2, gamma, norm)
                print "  flux = %.2g" % flux
                totflux += flux
                totbatflux += pl_flux(15, 195., gamma, norm)
            print "  Position = (%f, %f) %s" % (ra, dec, p)
            print "  offsets from pointing centers:"
            i = 1
            for pc in point_centers:
                d = p.Dist(pc)*60.
                print "    Pointing %d: offset = %.3f'" % (i, d)
                i = i + 1
        print "Total flux of power-law components: %g (%.1f-%.1f keV), %g (15-195 keV)" % (totflux, E1, E2, totbatflux)


# functions for helping with power laws
def pl_photflux(E1, E2, gamma, norm):
    """ Returns photon flux between E1 and E2 """
    return(norm*(E2**(-gamma+1) - E1**(-gamma+1))/(-gamma+1))


def pl_flux(E1, E2, gamma, norm):
    """ Returns photon flux between E1 and E2, converted to ergs/cm^2/s """
    if gamma==2.0:
        gamma = 2.001
    return(1.6e-9*norm*(E2**(-gamma+2) - E1**(-gamma+2))/(-gamma+2))


def pl_norm(E1, E2, gamma, flux):
    if gamma==2.0:
        gamma = 2.001
    return(flux/(1.6e-9*(E2**(-gamma+2) - E1**(-gamma+2))/(-gamma+2)))
 
def cutoffpl(E, gamma, Ecut, norm):
    "cut-off powerlaw fn"
    return(norm*E**(-gamma)*numpy.exp(-E/Ecut))

def cutoffpl_photflux(E1, E2, gamma, Ecut, norm, n=10000):
    "integrate cut-off power-law from E1 to E2 in n steps"
    dE = (E2-E1)/(n+1)
    Es = numpy.arange(n)*dE + E1
    fn = cutoffpl(Es, gamma, Ecut, norm)
    tot = sum(fn)*dE
    return(tot)

def cutoffpl_flux(E1, E2, gamma, Ecut, norm, n=10000):
    "integrate cut-off power-law from E1 to E2 in n steps"
    dE = (E2-E1)/n
    Es = numpy.arange(n)*dE + E1 + 0.5*dE
    #print Es[0], Es[-1]
    fn = cutoffpl(Es, gamma, Ecut, norm)*Es*1.6e-9
    tot = sum(fn)*dE
    return(tot)

def cutoffpl_norm(E1, E2, gamma, Ecut, flux, n=10000):
    return(flux/cutoffpl_flux(E1, E2, gamma, Ecut, 1., n))
 

if __name__ == "__main__":
    
    argv = sys.argv
    argc = len(argv)

    if argc < 2:
        print "Usage: %s nusim_config_file" % argv[0]
        sys.exit()

    c = nusim_cfg(argv[1])
    c.report()
        


    
