/* H+
 *      Title     : NoradTle.h
 *      Author    : Bryce A. Roberts
 *      Date      : 17 May 1998
 *      Synopsis  : propagates a satellite orbit based on a NORAD TLE
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#ifndef _NORADTLE_H
#define _NORADTLE_H


#include <TmTcTime.h>
#include <StateVector.h>

#include <OrbitModel.h>


namespace vecutil {

// define some absolute constants
const static double MIN_PER_DAY=  (1440.0);
const static double TWOBYTHREE=   (2.0/3.0);
const static double Q0=           (120.0);
const static double S0=           (78.0);
const static double THREEPIBYTWO= (PIBY2*3.0);
// NB: J2 is defined in StateVector.h
const static double J3=           (-0.253881E-5);
const static double J4=           (-1.65597E-6);
const static double KE=           (0.074366916);
const static double AE=           (1.0);
const static double S=            (AE*(1.0+S0/KM_PER_EARTH));
const static double Q0MS4=        pow((Q0-S0)*AE/KM_PER_EARTH, 4);
const static double CK2=          (0.5*J2*AE*AE);
const static double CK4=          (-0.375*J4*AE*AE*AE*AE);
const static double A30=          (-J3/AE*AE*AE);


class NoradTle : public OrbitModel {
protected:
  // the satellite name
  string satelliteName;

  // satellite catalog number (col 3-8)
  unsigned satNum;
  
  // international designator (col 10-17)
  string designator;
  
  // epoch time of the element set
  tmtc::TmTcTime epoch;
  
  // first and second derivatives of the mean motion (only used for SGP)
  double n0dt, n0dt2;

  // Bstar drag term (col 54-61)
  double bstar;

  // ephemeris type
  string ephemType;

  // element number 
  string elementNumber;

  // inclination (col 9-16)
  double i0;

  // right ascension of the ascending node (col 18-25)
  double raan;
  
  // double eccentricity (col 27-33)
  double e0;

  // argument of perigee (col 35-42)
  double w0;

  // mean anomaly (col 44-51)
  double M0;

  // double mean motion (col 53-63)
  double n0;

  // orbit number (col 64-68)
  int orbitNumber;

  /* derived variables (set by init) */
  double n0dp, a0dp, q0ms4, theta, theta_2, zeta_4, beta0_2, eta, C1, 
    C1_2, C1_3, C1_4, C3, C4, C5, D2, D3, D4, mdot, wdot, raandot; 

  bool checksum(const string& str, string& errorString);

public:
  bool read(const string& fileName, string& errorString);

  bool read(istream& is, string& errorString);

  /************************************************************
  ** Access functions
  ************************************************************/
  // set/get the satellite name
  string getSatelliteName(void) const;
  void setSatelliteName(const string& satellineName);

  // get the satellite catalog number
  unsigned getSatNumber(void) const;

  // get international designator
  string getDesignator(void) const;

  // get epoch time
  tmtc::TmTcTime getEpochTime(void) const;

  // get first derivative of mean motion
  double getMeanDot(void) const;

  // get second derivative of mean motion
  double getMeanDoubleDot(void) const;

  // get the Bstar drag term
  double getBstar(void) const;

  // get the ephemeris type
  string getEphemerisType(void) const;

  // get the element set number
  string getElementNumber(void) const;

  // get the inclination
  double getInclination(void) const;

  // get the RAAN
  double getRAAN(void) const;

  // get the eccentricity
  double getEccentricity(void) const;

  // get argument of perigee
  double getArgumentOfPerigee(void) const;

  // get mean anomaly
  double getMeanAnomaly(void) const;

  // get mean motion
  double getMeanMotion(void) const;

  // get the orbit number
  int getOrbitNumber(void) const;
  };

}

#endif


