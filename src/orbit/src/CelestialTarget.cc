/*
 * H+
 *       Title     : CelestialTarget.cc
 *       Author    : Bryce A. Roberts
 *       Date      : Tue Apr  5 16:35:36 PDT 2005
 *       Synopsis  : calculates the position of heavenly bodies
 *       Revisions :
 *       mm/dd/yy name   description
 * H-
 */

#include "CelestialTarget.h"

// solar mean longitude terms 
const static double L0= (4.881627973);	        // constant term 
const static double L1= (628.3319509);	        // linear term 
const static double L2= (5.279620987E-6);	// qudratic term 

// solar anomaly terms
const static double M0_SUN= (6.256583522);	 // constant term 
const static double M1= (628.3019457);	         // linear term 
const static double M2= (-2.617993E-6);	         // quadratic term 
const static double M3= (-5.759586532E-8);	 // cubic term 

// earth orbit's eccentricity
const static double ECC0= (0.01675104);	         // constant term 
const static double ECC1= (-0.0000418);	         // linear term 
const static double ECC2= (-0.000000126);	 // quadratic term 

const static double ACCURACY= (1.0E-9);      // accuracy for iterated solution 

// date conventions
const static double JD1900= (2415020.0);     // Julian day number of 1/0.5/1900
  const static double DAYSPERCENTURY= (36525.0);  // Julian Century = 36525 ephemeris days

typedef vecutil::CelestialTarget __V;

// utility function -- make sure an anglie is between [0, TWOPI)
double __V::reduceAngle(double ang) {
  double intpart=ang/TWOPI;
  intpart=(intpart>=0.0) ? floor(intpart) : ceil(intpart);
  ang-=TWOPI*intpart;
  if (ang<0) 
    ang+=TWOPI;
  return(ang);
  }

// get the J2000 ECI unit position vector for the sun
vecutil::Dirvector __V::getSun(const tmtc::TmTcTime& st) {
  // Julian centuries since January 0.5 1900
  double t=(st.getJd()-JD1900)/DAYSPERCENTURY;
    
  // geometric mean longitude of sun, referred to the mean equinox of date
  double meanLong=reduceAngle(L0 + t*(L1 + L2*t));

  // mean anomaly of the sun
  double meanAnomaly=reduceAngle(M0_SUN + t*(M1 + t*(M2 +M3*t)));

  // eccentricity of earth's orbit
  double eccentricity=reduceAngle(ECC0 + t*(ECC1 + ECC2*t));

  // get true anomaly
  double e0 = meanAnomaly;

  double delta;
  while (fabs(delta=(meanAnomaly+eccentricity*sin(e0)-e0)/
	      (1.0-eccentricity*cos(e0)))>=ACCURACY)
    e0 += delta;
  
  double trueAnomaly=2.0*atan(tan(e0/2.0)*sqrt((1+eccentricity)/
					       (1-eccentricity)));

  // get ecliptic-frame vector (lat=0.0)
  Dirvector dv=Dirvector().
    fromCelestial(reduceAngle(meanLong+trueAnomaly-meanAnomaly), 0.0);
  
  // convert to J2000 ECI
  dv.ecl2equ();

  return(dv);
  }

/// get the angle between the sun and a target
double __V::getSunAngle(const tmtc::TmTcTime& st,
			const Dirvector& dvTarg) {
  const Dirvector dvSun=getSun(st);

  return(dvSun.angle(dvTarg));
  }

// get the J2000 ECI unit position vector for the moon
vecutil::Dirvector __V::getMoon(const tmtc::TmTcTime& st) {
  double t=(st.getJd()-2415020.0)/36525.0;
  
  double lp=4.719966572+t*(8399.709144-t*(1.977458e-5-t*3.316126e-8));
  reduceAngle(lp);
    
  double m=6.256583575+t*(628.3019466-t*(2.617994e-6+t*5.759587e-8));
  reduceAngle(m);

  double mp=5.168000340+t*(8328.691104+t*(1.604307e-4+t*2.513274e-7));
  reduceAngle(mp);
  
  double d=6.121523941+t*(7771.377194-t*(2.506293e-5-t*3.316126e-8));
  reduceAngle(d);
  
  double f=0.196365057+t*(8433.466290-t*(5.604252e-5+t*5.235988e-9));
  reduceAngle(f);

  double omega=4.523601515-t*(33.75714610-t*(3.626794e-5+t*3.839724e-8));
  reduceAngle(omega);

  // add in periodic terms
  double s=sin(8.93608577e-1+t*3.52556509e-1);

  double s1=6.918485e-5*sin(6.048613056+t*(2.319018977-t*1.601008e-4));
  double e=1.0-t*(2.495e-3+t*7.52e-6), e2=e*e;
  double s2=sin(omega);

  lp+=4.066617e-6*s+3.427827e-5*s2+s1;
  m-=3.103195e-5*s;
  mp+=1.425934e-5*s+s1+4.434882e-5*s2;
  d+=3.509857e-5*s+s1+3.427827e-5*s2;
  f+=s1-4.309392e-4*s2-7.553785e-5*sin(omega+4.800528108-4.0142573e-2*t);
    
  double lam=lp + 1.097593933e-1*sin(mp) + 2.223580883e-2*sin(2.0*d-mp) +
    1.148965955e-2*sin(2.0*d) + 3.728302535e-3*sin(2.0*mp) -
    e*sin(m)*3.239261e-3 - 1.995540e-3*sin(2.0*f) + 1.026131e-3*
    sin(2.0*(d-mp)) + e*9.985378e-4*sin(2.0*d-m-mp) +
    9.306096e-4*sin(2.0*d+mp) + e*8.006523e-4*sin(2.0*d-m);
  
  lam+=e*7.160039e-4*sin(mp-m) - 6.059434e-4*sin(d) -
    e*5.317146e-4*sin(m+mp) + 2.674892e-4*sin(2.0*(d-f)) -
    2.186548e-4*sin(2.0*f+mp) - 1.916372e-4*sin(2.0*f-mp) +
    1.862964e-4*sin(4.0*d-mp) + 1.751263e-4*sin(3.0*mp) +
    1.491907e-4*sin(4.0*d-2.0*mp) - e*1.380555e-4*sin(m-mp+2.0*d) -
    e*1.183857e-4*sin(2.0*d+m);
    
  lam+=9.00939e-5*sin(mp-d) + e*8.726646e-5*sin(m+d) +
    e*7.066838e-5*sin(mp-m+2.0*d) + 6.974336e-5*sin(2.0*(mp+d)) +
    6.740462e-5*sin(4.0*d) + 6.396632e-5*sin(2.0*d-3.0*mp) +
    e*4.703662e-5*sin(2.0*mp-m) + 4.541347e-5*sin(mp-2.0*(f+d)) +
    e*4.181809e-5*sin(2.0*(d-mp)-m) - 4.099778e-5*sin(mp+d);
  
  lam+=e2*3.925245e-5*sin(2.0*(d-m)) - e*3.708825e-5*sin(2.0*mp+m) -
    e2*3.62854e-5*sin(2.0*m) + e2*3.593633e-5*sin(2.0*(d-m)-mp) -
    3.094469e-5*sin(mp+2.0*(d-f)) - 2.7838e-5*sin(2.0*(f+d)) +
    e*2.129302e-5*sin(4.0*d-m-mp) - 1.937315e-5*sin(2.0*(mp+f)) +
    1.556834e-5*sin(mp-3.0*d);

  lam+=e*1.415462e-5*sin(m+mp+2.0*d) + 
    e*1.328196e-5*sin(4.0*d-m-2.0*mp) + e2*1.251401e-5*sin(mp-2.0*m) +
    e2*1.228712e-5*sin(mp-2.0*(m+d)) + e*1.209513e-5*sin(m-2.0*(mp-d)) +
    e*1.043707e-5*sin(2.0*(d-f)-m) + 9.599311e-6*sin(mp+4.0*d) +
    9.389871e-6*sin(4.0*mp);
  
  lam+=e*9.093165e-6*sin(4.0*d-m) + 8.4823e-6*sin(2.0*mp-d);

  double beta=8.950378271e-2*sin(f) + 4.897498601e-3*sin(mp+f) +
    4.84665716e-3*sin(mp-f) + 3.02357349e-3*sin(2.0*d-f) + 
    9.671393e-4*sin(2.0*d+f-mp) + 8.075988e-4*sin(2.0*d-f-mp) +
    5.685061e-4*sin(2.0*d+f) + 3.001617e-4*sin(2.0*mp+f) +
    1.617397e-4*sin(2.0*d+mp-f) + 1.539904e-4*sin(2.0*mp-f) +
    e*1.439373e-4*sin(2.*d-m-f);
  
  beta+=7.545058e-5*sin(2.0*(d-mp)-f) + 7.330383e-5*sin(2.0*d+f+mp) +
    e*5.88525e-5*sin(f-m-2.0*d) + e*4.314454e-5*sin(2.*d+f-m-mp) +
    e*3.878122e-5*sin(2.0*d+f-m) + e*3.616322e-5*sin(2.0*d-f-m-mp) +
    e*3.275983e-5*sin(f-m+mp) + 3.190462e-5*sin(4.0*d-f-mp) -
    e*3.146829e-5*sin(f+m) - 3.054326e-5*sin(3.0*f);

  beta+=e*2.740167e-5*sin(mp-m-f) - 2.595305e-5*sin(f+d) -
    e*2.584833e-5*sin(f+m+mp) + e*2.473132e-5*sin(f-m-mp) +
    e*2.356194e-5*sin(f-m) + 2.321288e-5*sin(f-d) +
    1.930334e-5*sin(f+3.0*mp) + 1.780236e-5*sin(4.0*d-f) +
    1.453859e-5*sin(f+4.0*d-mp) + 1.363102e-5*sin(mp-3.0*f) +
    1.169371e-5*sin(f+4.0*d-2.0*mp);

  beta+=1.05767e-5*sin(2.0*d-3.0*f) + 1.041962e-5*sin(2.0*(d+mp)-f) +
    e*8.587020e-6*sin(2.0*d+mp-m-f) + 7.853982e-6*sin(2.0*(mp-d)-f) +
    7.661995e-6*sin(3.0*mp-f) + 7.382743e-6*sin(f+2.0*(d+mp)) +
    7.365289e-6*sin(2.0*d-f-3.0*mp) - e*6.405358e-6*sin(m+f-mp+2.0*d) -
    e*6.161012e-6*sin(m+f+2.0*d) + 5.77704e-6*sin(f+4.*d);
  
  beta+=e*5.532694e-6*sin(2.0*d+f-m+mp)+ 
    e2*5.340708e-6*sin(2.0*(d-m)-f) - 4.939282e-6*sin(mp+3.0*f);

  double omega1=0.0004664*cos(omega);
  double omega2=0.0000754*cos(omega+4.800528107-t*4.01425728e-2);
  beta = (beta)*(1.0-omega1-omega2);

  /*
  // calculate paralax
  double paralax = 0.950724 + 0.051818*cos(mp) +0.009531*cos(2.0*d - mp) + 
    0.007843*cos(2.0*d) + 0.002824*cos(2.0*mp) +
    0.000857*cos(2.0*d+mp) + e*0.000533*cos(2.0*d - m) +
    0.000401*e*cos(2.0*d - m - mp) + 0.000320*e*cos(mp - m) -
    0.000271*cos(d) - 0.000264*e*cos(m + mp) -
    0.000198*cos(2.0*f - mp) + 0.000173*cos(3.0*mp) +
    0.000167*cos(4.0*d - mp) - 0.000167*cos(4.0*d - mp) -
    0.000111*e*cos(m) + 0.000103*cos(4.0*d - 2.0*mp) -
    0.000084*cos(2.0*mp - 2.0*d) - 0.000083*e*cos(2.0*d + m) +
    0.000079*cos(2.0*d + 2.0*mp) + 0.000072*cos(4.0*d) + 
    0.000064*e*cos(2.0*d - m + mp) - 
    0.000063*e*cos(2.0*d + m - mp) +
    0.000041*e*cos(m + d) + 0.000035*e*cos(2.0*mp - m) -
    0.000033*cos(3.0*mp - 2.0*d) - 0.000030*cos(mp + d) - 
    0.000029*cos(2.0*f - 2.0*d) - 0.000029*e*cos(2.0*mp + m) +
    0.000026*e2*cos(2.0*d - 2.0*m) - 
    0.000023*cos(2.0*f - 2.0*d + mp) + 
    0.000019*e*cos(4.0*d - m - mp);
  
  double dist=6378.14/sin(DEG2RAD(paralax));
  */
  
  Dirvector dv=Dirvector().fromCelestial(lam,beta);
  dv.ecl2equ();

  return(dv);
  }

/// get the angle between the moon and a target
double __V::getMoonAngle(const tmtc::TmTcTime& st,
			 const Dirvector& dvTarg) {
  const Dirvector dvMoon=getMoon(st);

  return(dvMoon.angle(dvTarg));
  }
