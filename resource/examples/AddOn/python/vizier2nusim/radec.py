#! /usr/bin/env python
# Class to handle right ascention, declination coordinates
# 1/5/99 A. Ptak
# 4/9/99 A.P. fixed __repr__ bug (was taking int of second values)
# 01/03/03 A.P. changed string rep. to use 2 decimals for ra, 1 for dec
#          (was the other way around)

import string
#import regsub
import re
try:
    import numpy as N
    N.acos = N.arccos
except:
    import math as N
deg_to_rad = N.pi/180.

class radec:
    """ This class holds right assention, declination values and has functions
    for distance, to/from string,  and precession calculations """

    deg_per_h = 15.
    def __init__(self, s=' ', s2=None):
        self.ra = 0.
        self.dec = 0.
        if s != ' ':
            if s2 is None:
                self.StringAD(s)
            else:
                self.StringAD(s + ", " + s2)
            
        self.sep = " " # separator in sexidecimal rep.
        
    def __cmp__(self, ad2):
        "Sort by RA first, then dec"
        if type(ad2) != type(self):
            return(1)
        if self.ra < ad2.ra:
            return(-1)
        if self.ra > ad2.ra:
            return(1)
        if self.dec < ad2.dec:
            return(-1)
        if self.dec > ad2.dec:
            return(1)
        return(0)

    def __repr__(self):
        """ Returns a string (sexidecimal) representation """
        ra = self.ra/15. # Convert from deg. to hours
        h = int(ra)
        hm = int((ra - h)*60.)
        hs = ((ra - h - hm/60.) * 3600.)
        sgn = 1
        dec = self.dec
        if self.dec < 0:
            sgn = -1
            dec = -self.dec
        d = int(dec)
        dm = int((dec - d)*60.)
        ds = ((dec - d - dm/60.)*3600.)
        s = self.sep
        if sgn == 1:
            out = "%d%s%d%s%.2f %d%s%d%s%.1f" % (h, s, hm, s, hs, d, s,
                                                 dm, s, ds)
        else:
            out = "%d%s%d%s%.2f -%d%s%d%s%.1f" % (h, s, hm, s, hs, d, s,
                                                  dm, s, ds)

        return out

    def get_dec(self, sep=""):
        str = self.__repr__()
        if sep == " ":
            return(string.join(str.split()[3:]))
        else:
            return(str.split()[1])

    def get_ra(self, sep=""):
        str = self.__repr__()
        if sep == " ":
            return(string.join(str.split()[:3]))
        else:
            return(str.split()[0])

    def Dist(self, ad):
        "Returns distance between two coords. in degrees"
        #print "deg_to_rad = ", deg_to_rad
        #print "self.ra, self.dec = ", self.ra, self.dec
        #print "ad.ra, ad.dec = ", ad.ra, ad.dec
        ra_rad = self.ra*deg_to_rad
        dec_rad = self.dec*deg_to_rad
        ra2_rad = ad.ra*deg_to_rad
        dec2_rad = ad.dec*deg_to_rad
        #print ra_rad, dec_rad, ra2_rad, dec2_rad
        sin = N.sin
        cos = N.cos
        cosA = sin(dec_rad)*sin(dec2_rad) + \
               cos(dec_rad)*cos(dec2_rad)*cos(ra_rad - ra2_rad)
        #print "cosA = ", cosA
        if cosA > 0.99999999999999:
            return(0.)
        try:
            A = N.acos(cosA)/deg_to_rad
        except:
            if cosA < 0.99999:
                print "error computing distance between %s and %s, cosA=%f" \
                      % (self.__repr__(), ad, cosA)
            return(0.)
        
        return(A)
    
    def Precess(self, eq1, eq2):
        "Returns (ra, dec) precessed from eq1 to eq2, taken from precess.pro"
        ra_rad = self.ra*deg_to_rad
        dec_rad = self.dec*deg_to_rad
        a = N.cos(dec_rad)
        x = [a*N.cos(ra_rad), a*N.sin(ra_rad), N.sin(dec_rad)]
        sec_to_rad = deg_to_rad/3600.0
        r = self.PreMat(eq1, eq2)
        # Simulate idl # operator, x2 = r#x
        x2 = [0., 0., 0.]
        for i in [0, 1, 2]:
            for j in [0, 1, 2]:
                x2[i] = x2[i] + r[j][i] * x[j]
        ra = N.atan2(x2[1], x2[0])/deg_to_rad
        if (ra < 0.):
            ra = ra + 360.        
        dec = N.asin(x2[2])/deg_to_rad
	ad2 = radec()
	ad2.ra = ra
	ad2.dec = dec
        return (ad2)
    
    def PreMat(self, eq1, eq2):
        "From premat.pro"
        deg_to_rad = N.pi/180.
        sec_to_rad = deg_to_rad/3600.0
        T = 0.001*(eq2 - eq1)
        ST = 0.001*(eq1 - 2000.0)
        A = sec_to_rad*T*(23062.181 + ST*(139.656 +0.0139*ST)
                          + T*(30.188 - 0.344*ST+17.998*T))
        B = sec_to_rad * T * T * (79.280 + 0.410*ST + 0.205*T) + A
        C = sec_to_rad * T * (20043.109 - ST*(85.33 + 0.217*ST) 
                              + T*(-42.665 - 0.217*ST -41.833*T))
        sina = N.sin(A)
        sinb = N.sin(B)
        sinc = N.sin(C)
        cosa = N.cos(A)
        cosb = N.cos(B)
        cosc = N.cos(C)
        r = [[cosa*cosb*cosc-sina*sinb, sina*cosb+cosa*sinb*cosc,  cosa*sinc],
             [-cosa*sinb-sina*cosb*cosc, cosa*cosb-sina*sinb*cosc, -sina*sinc],
             [-cosb*sinc, -sinb*sinc, cosc]]
        return r
    
    def QD_Dist(self, ad):
        "Quick-and-dirty distance between two radec coords. in degrees"
        meandec = 0.5*(ad.dec + self.dec)
        deltara = (ad.ra - self.ra)*N.cos(meandec*N.pi/180.)
        deltadec = ad.dec - self.dec
        return N.sqrt(deltara*deltara + deltadec*deltadec)

    def StringAD(self, s):
        # Convert from NED format, 00:00:00 format to space-delimited
        #s = string.strip(regsub.gsub("[dhms:]", " ", s))
        s = string.strip(re.sub("[dhms:]", " ", s))
        s = s.replace(",", " ")
        # print "Parsing s = " + s
        vals = string.split(s)
        if len(vals) == 2:
            try:
                self.ra = float(vals[0])
                self.dec = float(vals[1])
            except:
                raise
            return
        if len(vals) != 6:
            # print "Warning, %s does not appear to consist of 6 values" % s
            raise ValueError, "%s does not appear to consist of 6 values" % s
        self.ra = self.deg_per_h*self.Ten(vals[0:3])
        self.dec = self.Ten(vals[3:6])

    def StringAD2(self, ra, dec):
        # As in StringAD, but with ra and dec strings passes separately
        vals = string.split(string.strip(ra))
        vals = vals + string.split(string.strip(dec))
        self.ra = self.deg_per_h*self.Ten(vals[0:3])
        self.dec = self.Ten(vals[3:6])

    def Ten(self, sex):
        # Convert from sexidecimal to degrees
        if len(sex) != 3:
            print "Error... expected 3 values but got %d" % len(sex)
            return
        d = sex[0]
        # If sign in from of decimal value is negative, we need to remember
        # this and strip the '-', otherwise values like -00 30 30 would
        # not convert properly
        sign = 1
        if d[0] == '-':
            sign = -1
            d = d[1:]
        d = string.atoi(d)
        m = string.atoi(sex[1])
        s = string.atof(sex[2])
        return sign*(d+m/60.+s/3600.)
    
    def ToURL(self):
        "Convert ra/dec to url parameter line format"
        ra = self.ra/15. # Convert from deg. to hours
        h = int(ra)
        hm = int((ra - h)*60.)
        hs = ((ra - h - hm/60.) * 3600.)
        sgn = 1
        dec = self.dec
        if self.dec < 0:
            sgn = -1
            dec = -self.dec
        d = int(dec)
        dm = int((dec - d)*60.)
        ds = ((dec - d - dm/60.)*3600.)
        if sgn == 1:
            out = "%d+%d+%.1f+%%2B%d+%d+%.2f" % (h, hm, hs, d, dm, ds)
        else:
            out = "%d+%d+%.1f+-%d+%d+%.2f" % (h, hm, hs, d, dm, ds)

        return out


if __name__ == '__main__':
    x = radec("0 53 53.6 -27 47 6")
    y = x.Precess(1950., 2000.0)
    print "%s" % (x)
    print y
    print x.ToURL()
    
    
