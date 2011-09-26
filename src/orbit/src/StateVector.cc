/* H+
 *      Title     : StateVector.cc
 *      Author    : Bryce A. Roberts
 *      Date      : 19 October 1998
 *      Synopsis  : Orbit State Vector
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#include <StateVector.h>
#include <Quat.h>

// added this meaningless comment

using namespace vecutil;

/************************************************************
** Constructors
************************************************************/
StateVector::StateVector(const tmtc::TmTcTime& st, const Dirvector& dvPos, 
			 const Dirvector& dvVel, int centralBody) {
  this->st=st; this->dvPos=dvPos; this->dvVel=dvVel;
  r=this->dvPos.length(); this->dvPos.normalize(r);
  v=this->dvVel.length(); this->dvVel.normalize(v);
  switch (centralBody) {
  case CENTRAL_EARTH:
    sv_K=K_EARTH;
    whichBody=CENTRAL_EARTH;
    break;
  case CENTRAL_SUN:
    sv_K=K_SUN;
    whichBody=CENTRAL_SUN;
    break;
    }
  }


/************************************************************
** Access functions
************************************************************/
void StateVector::set(const tmtc::TmTcTime& st, const Dirvector& dvPos, 
		      const Dirvector& dvVel, int centralBody) {
  this->st=st; this->dvPos=dvPos; this->dvVel=dvVel;
  r=this->dvPos.length(); this->dvPos.normalize(r);
  v=this->dvVel.length(); this->dvVel.normalize(v);
  switch (centralBody) {
  case CENTRAL_EARTH:
    sv_K=K_EARTH;
    whichBody=CENTRAL_EARTH;
    break;
  case CENTRAL_SUN:
    sv_K=K_SUN;
    whichBody=CENTRAL_SUN;
    break;
    }
  }

tmtc::TmTcTime StateVector::getTime(void) const { return(st); }

Dirvector StateVector::getPosition(void) const { return(dvPos); }

Dirvector StateVector::getVelocity(void) const { return(dvVel); }

double StateVector::getAltitude(void) const { return(r); }

double StateVector::getLinearRate(void) const { return(v); }

double StateVector::getAngularRate(void) const { return(v/r); }


/************************************************************
** Computation functions
************************************************************/
// get the orbit plane normal vector (pole)
Dirvector StateVector::getPole(void) const {
  return(dvPos.cross(dvVel).normalize());
  }

// get total energy (kinetic + potential)
double StateVector::getTotalEnergy(void) const {
  return(1E6*v*v/2.0 - sv_K/(r*1E3));
  }

// get the instantaneous moment of momentum
double StateVector::getMomentOfMomentum(void) const { 
  return(v*r*1E6*sin(getFlightDirectionAngle())); 
  }

// get the semi-major axis (a)
double StateVector::getSemiMajorAxis(void) const {
  double h=getMomentOfMomentum(), e=getEccentricity();
  
  return(h*h/(sv_K*(1-e*e)));
  }

// get eccentricity parameter
double StateVector::getParameter(void) const {
  double h=getMomentOfMomentum();
  return(h*h/sv_K);
  }

// get the eccentricity
double StateVector::getEccentricity(void) const {
  double sinG=sin(getFlightDirectionAngle()), rvvk=1.0E9*r*v*v/sv_K;
  return(sqrt(1+rvvk*(rvvk-2)*sinG*sinG));
  }

// get the orbit plane inclination (i)
double StateVector::getInclination(void) const {
  return(acos(getPole().safeDot(Dirvector(0,0,1))));
  }

// get the Keplerian period (assumes no J2 perturbation)
tmtc::TmTcDuration StateVector::getPeriod(void) const {
  double a=getSemiMajorAxis();
  return(tmtc::TmTcDuration().setSeconds(TWOPI*sqrt((a*a*a)/sv_K)));
  }

// get the nodal period (includes J2 perturbation causing regressing apsides)
tmtc::TmTcDuration StateVector::getNodalPeriod(void) const {
  double sinIncl=sin(getInclination()), ar=M_PER_EARTH/getSemiMajorAxis(), 
    e=getEccentricity(), ome=(1-e*e), p=getPeriod().getSeconds();

  return(tmtc::TmTcDuration().setSeconds(p/(1+1.5*J2*ar*ar*(2-
    2.5*sinIncl*sinIncl)/ome*ome)));
  }

// get the regression rate (this is not a true Keplerian element)
double StateVector::getRegressionRate(void) const {
  double e=getEccentricity(), ome=(1-e*e), ar=M_PER_EARTH/getSemiMajorAxis();
  
  return(-(1.5*J2*(TWOPI/getPeriod().getSeconds())*ar*ar*
	   cos(getInclination()))/(ome*ome));
  }

// get classical mean motion
double StateVector::getMeanMotion(void) const { 
  return(TWOPI/getPeriod().getSeconds()); 
  }

// get mean motion corrected for regression of apsides
double StateVector::getNodalMeanMotion(void) const { 
  return(TWOPI/getNodalPeriod().getSeconds()); 
  }

// get true anomaly
double StateVector::getTrueAnomaly(void) const {  
  double sigma=dvPos.safeDot(dvVel)*1E6*r*v/sqrt(sv_K), 
    p=getParameter(), e=getEccentricity(), sinf=sqrt(p)*sigma/(r*e*1000), 
    cosf=(p/(r*1000) -1)/e;
  
  return(atan2(sinf, cosf));
  }

// convert a true anomaly to eccentric anomaly for a given eccentricity
double StateVector::trueToEccentricAnomaly(double f, double e) {
  return(atan2((sqrt(1-e*e)*sin(f))/(1+e*cos(f)), (e+cos(f))/(1+e*cos(f))));
  }

// convert an eccentric anomaly to true anomaly for a given eccentricity
double StateVector::eccentricToTrueAnomaly(double E, double e) {
  return(atan2((sqrt(1-e*e)*sin(E))/(1-e*cos(E)), (cos(E)-e)/(1-e*cos(E))));
  }

// get eccentric anomaly (E)
double StateVector::getEccentricAnomaly(void) const {
  double a=getSemiMajorAxis(),
    sinE=1.0E6*r*v*dvPos.safeDot(dvVel)/(sqrt(sv_K)*sqrt(a)),
    cosE=1-(1000*r/a);
  return(atan2(sinE, cosE));
  }

// get the flight direction angle
double StateVector::getFlightDirectionAngle(void) const {
  return(dvPos.angle(dvVel));
  }

// get position vector pointing to perigee
Dirvector StateVector::getArgumentOfPerigee(void) const {
  double f=getTrueAnomaly();
  Dirvector dvPole=getPole();
  return(Quat().arbitraryAxisRotation(dvPole, f).translate(getPosition()));
  }

// converge to a position vector
tmtc::TmTcTime StateVector::converge(const Dirvector& dvTarg, int whichAxis,
			      int whichDirection) const {
  // find true anomaly of current state vector 
  double f1=getTrueAnomaly(), f2=f1;
  
  // estimate true anomaly of target state vector
  if (whichAxis==CONVERGE_POSITION)
    f2=f1+Dirvector::planeAngle(getPole(), dvPos, dvTarg);
  else if (whichAxis==CONVERGE_VELOCITY)
    f2=f1+Dirvector::planeAngle(getPole(), dvVel, dvTarg);

  // convert from true to eccentric anomalies for use with Kepler's equation
  double e=getEccentricity(), E1=StateVector::trueToEccentricAnomaly(f1, e),
    E2=StateVector::trueToEccentricAnomaly(f2, e);

  // make sure a quadrant transition doesn't cause a scalar magnitude
  // error in the eccentric anomaly values
  if (f2<f1 && E2>E1)
    E2-=TWOPI;
  else if (f1<f2 && E1>E2)
    E1-=TWOPI;

  switch (whichDirection) {
  case CONVERGE_BACKWARD:
    if (E2>E1)
      E2-=TWOPI;
    break;
  case CONVERGE_FORWARD:
    if (E1>E2)
      E1-=TWOPI;
    break;
  case CONVERGE_FREE:
    if (E1-E2>0.5*TWOPI)
      E2+=TWOPI;
    break;
  default:
    return(tmtc::TmTcTime());
    }

  // calculate the time between current vector and interpolating vector
  tmtc::TmTcDuration dt;
  dt.setSeconds((E2-E1-e*(sin(E2)-sin(E1)))/getMeanMotion());
  
  // correct the original estimate
  return(getTime()+dt);
  }


StateVector StateVector::propagate(const tmtc::TmTcTime& st) {
  tmtc::TmTcDuration dt=st-getTime();

  double n=getMeanMotion(), e=getEccentricity(),  
    E1=getEccentricAnomaly(), E2=E1;

  for (int i=0; i<3; i++) 
    E2=n*dt.getSeconds()+E1+e*(sin(E2)-sin(E1));
    
  double fNew=StateVector::eccentricToTrueAnomaly(E2, e),
    a=getSemiMajorAxis(), rNew=(a*(1-e*e))/(1+e*cos(fNew)),
    h=getMomentOfMomentum(), h2=h*h, f0=getTrueAnomaly(), 
    r0=r*1000.0, v0=v*1000.0;

  // find orbital basis vectors
  Dirvector ie=(sv_K/h2)*(e+cos(f0))*dvPos*r0 - (r0/h)*sin(f0)*dvVel*v0,
    ip=(sv_K/h2)*sin(f0)*dvPos*r0 + (r0/h)*cos(f0)*dvVel*v0;

  // find orbital position, velocity vectors (in km)
  Dirvector dvR=(rNew*cos(fNew)*ie + rNew*sin(fNew)*ip)/1000.0,
    dvV=((-sv_K/h)*sin(fNew)*ie + (sv_K/h)*(e+cos(fNew))*ip)/1000.0;

  // return a complete state vector
  return(StateVector(st, dvR, dvV, whichBody));
  }

// rotate the state vector about equatorial pole (simulates a launch
// delay)
StateVector StateVector::rotate(const tmtc::TmTcDuration& dt, double rotAngle) {
  // add the time delay
  st=st+dt;

  // do the rotation
  Quat q=Quat().arbitraryAxisRotation(Dirvector(0,0,1), rotAngle);  
  dvPos=q.translate(dvPos); dvVel=q.translate(dvVel);

  return(*this);
  }

// override output operator
ostream& operator << (ostream& os, const StateVector& sv) {
  double r=sv.getAltitude(), v=sv.getLinearRate();
  Dirvector dvPos=sv.getPosition(), dvVel=sv.getVelocity();

  os << sv.getTime().getExactEngineering() << ",";

  os.setf(ios::fixed, ios::floatfield);

  os << setw(8) << sv.getAltitude()-KM_PER_EARTH << "," << v << "," <<
    dvPos[0]*r << "," << dvPos[1]*r << "," << dvPos[2]*r << "," <<
    dvVel[0]*v << "," << dvVel[1]*v << "," << dvVel[2]*v;

  return(os);
  }
