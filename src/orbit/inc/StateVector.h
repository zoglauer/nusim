/* H+
 *      Title     : StateVector.h
 *      Author    : Bryce A. Roberts
 *      Date      : 19 October 1998
 *      Synopsis  : Orbit State Vector
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#ifndef _STATEVECTOR_H
#define _STATEVECTOR_H

#include <GUtil.h>
#include <TmTcTime.h>
#include <Dirvector.h>
#include <Quat.h>


namespace vecutil {


const static double K_EARTH=       (3.986005E14);    // G*Mearth
const static double K_SUN=         (1.37212438E20);  // G*Msun
const static double KM_PER_EARTH=  (6378.135);
const static double M_PER_EARTH=   (KM_PER_EARTH*1000);
const static double J2=            (1.082616E-3);


class StateVector {
protected:
  // time of state vector
  tmtc::TmTcTime st;

  // J2000 ECI position and velocity unit vectors
  Dirvector dvPos, dvVel;

  // position and velocity magnitudes
  double r, v;

  // appropriate gravitational constant
  double sv_K;

  int whichBody;

public:
  // which central body?
  enum { CENTRAL_EARTH, CENTRAL_SUN };

  // which direction in time to converge the vector
  enum { CONVERGE_BACKWARD, CONVERGE_FORWARD, CONVERGE_FREE };

  // which axis (position or velocity) to converge to
  enum { CONVERGE_POSITION, CONVERGE_VELOCITY };

  /************************************************************
  ** Constructors
  ************************************************************/
  StateVector(void) { ; }
  StateVector(const tmtc::TmTcTime& st, const Dirvector& dvPos, 
	      const Dirvector& dvVel, int centralBody);

  /************************************************************
  ** Access functions
  ************************************************************/
  void set(const tmtc::TmTcTime& st, const Dirvector& dvPos, const Dirvector& dvVel,
	   int centralBody);
  tmtc::TmTcTime getTime(void) const;
  Dirvector getPosition(void) const;
  Dirvector getVelocity(void) const;
  double getAltitude(void) const;
  double getLinearRate(void) const;
  double getAngularRate(void) const;
  
  // get the orbit plane normal vector (pole)
  Dirvector getPole(void) const;

  // get total energy (kinetic + potential)
  double getTotalEnergy(void) const; 

  // get the instantaneous moment of momentum
  double getMomentOfMomentum(void) const;

  // get the semi-major axis (a)
  double getSemiMajorAxis(void) const;

  // get eccentricity parameter 
  double getParameter(void) const;

  // get the eccentricity (e)
  double getEccentricity(void) const;

  // get the orbit plane inclination (i)
  double getInclination(void) const;

  // get the Keplerian period (assumes no J2 perturbation)
  tmtc::TmTcDuration getPeriod(void) const;

  // get the nodal period (Keplerian period with J2 perturbation 
  // causing regressing of apsides, should only be used if central
  // body is Earth)
  tmtc::TmTcDuration getNodalPeriod(void) const;

  // get the regression rate (this is not a true Keplerian element,
  // should only be used if central body is the earth)
  double getRegressionRate(void) const;

  // get classical mean motion 
  double getMeanMotion(void) const;

  // get mean motion corrected for regression of apsides, should only
  // be used if the central body is the earth
  double getNodalMeanMotion(void) const;

  // get true anomaly (f)
  double getTrueAnomaly(void) const;

  // convert a true anomaly into an eccentric anomaly
  static double trueToEccentricAnomaly(double f, double e);

  // convert an eccentric anomaly to true anomaly for a given eccentricity
  static double eccentricToTrueAnomaly(double E, double e);

  // get eccentric anomaly (E)
  double getEccentricAnomaly(void) const;

  // get the flight direction angle
  double getFlightDirectionAngle(void) const;
  
  // get position vector pointing to argument of perigee
  Dirvector getArgumentOfPerigee(void) const;

  // converge to a target vector
  tmtc::TmTcTime converge(const Dirvector& dvTarg, int whichAxis,
			  int whichDirection=CONVERGE_FREE) const;

  // propagate the given state vector to a new time using two-body model
  StateVector propagate(const tmtc::TmTcTime& st);

  // rotate the state vector about equatorial pole (simulates a launch
  // delay)
  StateVector rotate(const tmtc::TmTcDuration& dt, double dr);

  // get the perigee
  double getPerigee(void) const {
    return(getSemiMajorAxis()*(1-getEccentricity()));
    }

  // get the apogee
  double getApogee(void) const {
    return(getSemiMajorAxis()*(1+getEccentricity()));
    }

  void getErrorVector(const StateVector& sv2,
		      double& radial, double& inTrack, double& crossTrack,
		      double& range, double& radialRate, double& inTrackRate,
		      double& crossTrackRate, double& rangeRate) {
    Dirvector dvP1=getPosition(), dvV1=getVelocity(), dvC1=getPole(),
      dvP2=sv2.getPosition(), dvV2=sv2.getVelocity();

    Dirvector dvPerror=(dvP1*getAltitude()-dvP2*sv2.getAltitude());
    radial=dvPerror.dot(dvP1); inTrack=dvPerror.dot(dvV1);
    crossTrack=dvPerror.dot(dvC1);

    Dirvector dvVerror=(dvV1*getLinearRate()-dvV2*sv2.getLinearRate());
    radialRate=dvVerror.dot(dvP1); inTrackRate=dvVerror.dot(dvV1);
    crossTrackRate=dvVerror.dot(dvC1);

    range=dvPerror.length(); rangeRate=dvVerror.length();
    }
  };

ostream& operator << (ostream& os, const StateVector& sv);

}

#endif

