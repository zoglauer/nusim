/* H+
 *
 *  Title        : Dirvector.h
 *  Author       : broberts
 *  Created      : Wed Jul 19 08:38:00 PDT 2006
 *  Synopsis     : a direction vector class
 *
 *  $LastChangedDate: 2011-09-26 22:45:51 -0700 (Mon, 26 Sep 2011) $
 *  $LastChangedRevision: 6618 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include "Dirvector.h"
#include "Quat.h"

const static double OBLIQ2000=  (0.4090928);
const static double JD2000=     (2451544.5);

// JD of 1968 JAN 1 00h
static const double JD1968=2439855.5;
static const tmtc::TmTcTime t2000=
  tmtc::TmTcTime().setJd(2451545.0);

typedef vecutil::Dirvector __D;


/// Constructor
__D::Dirvector(void) { x[0]=x[1]=x[2]=0.0; }

/// Constructor
__D::Dirvector(double x0, double x1, double x2) { x[0]=x0; x[1]=x1; x[2]=x2; }

/// normalize this vector 
vecutil::Dirvector& __D::normalize(void) {
  register double temp=sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);

  // set norm to be 1.0 if it is zero, to avoid divide by zero problems
  if (temp==0)
    temp=1.0;
  
  x[0]/=temp; x[1]/=temp; x[2]/=temp; 
  
  return(*this);
  }

/// normalize this vector with a constant
vecutil::Dirvector& __D::normalize(double c) {
  x[0]/=c; x[1]/=c; x[2]/=c;
  return(*this);
  }

/// normalize a copy of this vector
vecutil::Dirvector __D::normalize_copy(void) const {
  register double temp=sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);

  // set norm to be 1.0 if it is zero, to avoid divide by zero problems
  if (temp==0)
    temp=1.0;
    
  return(Dirvector(x[0]/temp, x[1]/temp, x[2]/temp));
  }

/// take the negative of the vector
vecutil::Dirvector& __D::negate(void) {
  x[0]*=-1; x[1]*=-1; x[2]*=-1;
  return(*this);
  }

/// take the negative of the vector
vecutil::Dirvector __D::negate_copy(void) const {
  return(Dirvector(-x[0], -x[1], -x[2]));
  }
  
/// find the length fo this vector
double __D::length(void) const {
  return(sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
  }

// project this vector onto a plane defined by axisVector
vecutil::Dirvector __D::project(const Dirvector& axisVector, 
				double thedot) const {
  return(Dirvector(x[0]-axisVector.x[0]*thedot, 
		   x[1]-axisVector.x[1]*thedot, 
		   x[2]-axisVector.x[2]*thedot));
  }

// project this vector onto a plane defined by axisVector
vecutil::Dirvector __D::project(const Dirvector& axisVector) const {
  double thedot=dot(axisVector);
  return(Dirvector(x[0]-axisVector.x[0]*thedot, 
		   x[1]-axisVector.x[1]*thedot, 
		   x[2]-axisVector.x[2]*thedot));
  } 

vecutil::Dirvector __D::fromCelestial(double ra, double dec) {
  double cosDec=cos(dec); 
  x[0]=cos(ra)*cosDec; x[1]=sin(ra)*cosDec; x[2]=sin(dec);
    
  return(*this);
  }

// convert the direction vector to back to RA, Dec (in radians)
void __D::toCelestial(double& ra, double& dec) const {
  dec=(fabs(x[2])<1.0) ? asin(x[2]) : TWOPI*fabs(x[2])/(x[2]*4.0);
  
  if (x[0]!=0.0)
    ra=atan2(x[1], x[0]);
  else 
    ra=(x[1]!=0) ? TWOPI * fabs(x[1])/(x[1]*4.0) : 0.0;
  
  ra=(ra<0.0) ? ra+TWOPI : ra;
  }

void __D::equ2ecl(void) {
  double tmp1, tmp2;
  tmp1=cos(OBLIQ2000)*x[1] + sin(OBLIQ2000)*x[2];
  tmp2=-sin(OBLIQ2000)*x[1] + cos(OBLIQ2000)*x[2];
  x[1]=tmp1; x[2]=tmp2;
  }

// internally convert the direction vector from ecliptic to equatorial 
// coordinate frame
void __D::ecl2equ(void) {
  double tmp1, tmp2;
  tmp1=cos(OBLIQ2000)*x[1] - sin(OBLIQ2000)*x[2];
  tmp2=sin(OBLIQ2000)*x[1] + cos(OBLIQ2000)*x[2];
  x[1]=tmp1; x[2]=tmp2;
  }

// precess the direction vector to epoch t2, assuming it is
// currently represented as epoch t1
void __D::precess(const tmtc::TmTcTime& t1, 
		  const tmtc::TmTcTime& t2) {
  double t=0.001*(t2.getJd()-t1.getJd())/365.25;
  double st=0.001*(t1.getJd()-JD2000)/365.25;

  // find the three rotation angles
  double a=DEG2RAD(t*(23062.181+st*(139.656+0.0139*st)+
                      +t*(30.188-0.344*st+17.998*t)))/3600.0;
  double b=DEG2RAD(t*t*(79.280+0.410*st+0.205*t)/3600.0)+a;
  double c=DEG2RAD(t*(20043.109-st*(85.33+0.217*st)+
                      +t*(-42.665-0.217*st-41.833*t)))/3600.0;
  
  // do the precession rotation
  double temp[3], sina=sin(a), sinb=sin(b), sinc=sin(c),
    cosa=cos(a), cosb=cos(b), cosc=cos(c);
  temp[0]=(cosa*cosb*cosc-sina*sinb)*x[0]+(-cosa*sinb-sina*cosb*cosc)*x[1]+
    -cosb*sinc*x[2];
  temp[1]=(sina*cosb+cosa*sinb*cosc)*x[0]+(cosa*cosb-sina*sinb*cosc)*x[1]+
    -sinb*sinc*x[2];
  temp[2]=cosa*sinc*x[0]-sina*sinc*x[1]+cosc*x[2];
  
  // replace existing direction vector
  x[0]=temp[0]; x[1]=temp[1]; x[2]=temp[2];
  }


/// find the dot product of this vector with another
double __D::dot(const Dirvector& o) const {
  return(x[0]*o.x[0]+x[1]*o.x[1]+x[2]*o.x[2]);
  }

double __D::safeDot(const Dirvector& y) const {
  double dotdot=dot(y);
  
  if (fabs(dotdot)>1)
    dotdot=SIGN(dotdot);
    
  return(dotdot);
  }

// find the intersection of two cones.  Cone 1 is defined by the
// vertex vector dv1 and the position vector dvV1; Cone 2 is 
// defined by the vertex vector dv2 and the position dvV2
// returns intersection vectors dvI1, dvI2
bool __D::coneIntersect(const Dirvector& dv1, 
  const Dirvector& dvV1, const Dirvector& dv2, const Dirvector& dvV2,
  Dirvector& dvI1, Dirvector& dvI2) {
  // find a vector perpendicular to both dv1 and dv2
  // do NOT normalize dv3!
  Dirvector dv3=dv1.cross(dv2); 
  
  // find dot products between cone vertex vectors (v1, v2) and the
  // respective position vectors (dvV1, dvV2)
  double dot1V1=dv1.safeDot(dvV1), dot2V2=dv2.safeDot(dvV2);

  // find the dot product of the two cone vertex vectors
  double dotdot=dv1.safeDot(dv2);
  
  // check that dv1 and dv2 are not colinear
  if (dotdot==1)
    return(false);

  double denom=1-dotdot*dotdot,
    x=(dot1V1-dotdot*dot2V2)/denom,
    y=(dot2V2-dotdot*dot1V1)/denom;

  // protect against sqrt(-1) (case where cones don't intersect)
  double product=(x*dot1V1+y*dot2V2);
  if (product>1)
    return(false);

  double z=sqrt((1-product)/denom);

  // build the two intersection vectors
  dvI1[0]=x*dv1[0]+y*dv2[0]+z*dv3[0];
  dvI1[1]=x*dv1[1]+y*dv2[1]+z*dv3[1];
  dvI1[2]=x*dv1[2]+y*dv2[2]+z*dv3[2];
  dvI1.normalize();

  dvI2[0]=x*dv1[0]+y*dv2[0]-z*dv3[0];
  dvI2[1]=x*dv1[1]+y*dv2[1]-z*dv3[1];
  dvI2[2]=x*dv1[2]+y*dv2[2]-z*dv3[2];
  dvI2.normalize();

  return(true);
  }

// find the intersection of two cones.  Cone 1 is defined by the
// vertex vector dv1 and the dot product dp1; Cone 2 is 
// defined by the vertex vector dv2 and the dot product dp2
// returns intersection vectors dvI1, dvI2
bool __D::coneIntersect(const Dirvector& dv1, double dp1,
			const Dirvector& dv2, double dp2, 
			Dirvector& dvI1, Dirvector& dvI2) {
  // find a vector perpendicular to both dv1 and dv2
  // do NOT normalize dv3!  
  Dirvector dv3=dv1.cross(dv2);

  // find the dot product of the two cone vertex vectors
  double dotdot=dv1.safeDot(dv2);
  
  // check that dv1 and dv2 are not colinear
  if (dotdot==1)
    return(false);

  double denom=1-dotdot*dotdot,
    x=(dp1-dotdot*dp2)/denom,
    y=(dp2-dotdot*dp1)/denom;

  // protect against sqrt(-1) (case where cones don't intersect)
  double product=(x*dp1+y*dp2);
  if (product>1)
    return(false);

  double z=sqrt((1-product)/denom);

  // build the two intersection vectors
  dvI1[0]=x*dv1[0]+y*dv2[0]+z*dv3[0];
  dvI1[1]=x*dv1[1]+y*dv2[1]+z*dv3[1];
  dvI1[2]=x*dv1[2]+y*dv2[2]+z*dv3[2];
  dvI1.normalize();

  dvI2[0]=x*dv1[0]+y*dv2[0]-z*dv3[0];
  dvI2[1]=x*dv1[1]+y*dv2[1]-z*dv3[1];
  dvI2[2]=x*dv1[2]+y*dv2[2]-z*dv3[2];
  dvI2.normalize();

  return(true);
  }

double __D::planeAngle(const Dirvector& dvPole, 
		       const Dirvector& dv11, 
		       const Dirvector& dv22) {
  Dirvector dv1=dv11, dv2=dv22;
  dv1=dv1.project(dvPole); dv2=dv2.project(dvPole);
  Dirvector dvCross=dv1.cross(dv2);
  return(atan2(dvCross.length()*SIGN(dvCross.dot(dvPole)), dv1.dot(dv2)));
  }

// get a vector that lies on the edge of the cone defined by this
// vector and half-angle 'angle'
vecutil::Dirvector __D::edgeOfCone(double angle) const {
  // find a vector perpendicular to this one
  Dirvector d(*this);
  if (x[0]!=0) 
    d.x[0]*=-1;
  else if (x[1]!=0) 
    d.x[1]*=-1;
  else if (x[2]!=0) 
    d.x[2]*=-1;

  if (d.x[0]==0 && d.x[1]!=0)
    swap(d.x[0], d.x[1]);
  else if (d.x[1]==0 && d.x[2]!=0)
    swap(d.x[1], d.x[2]);
  else if (d.x[2]==0 && d.x[0]!=0)
    swap(d.x[2], d.x[0]);

  Dirvector dvPerp=cross(d); dvPerp.normalize();

  // rotate this vector about the perpendicular vector by 'angle'
  vecutil::Quat q; q.arbitraryAxisRotation(dvPerp, angle);
  return(q.translate(*this));
  }


/// find the cross product between this vector and an external one
/// (e.g. this x o)
vecutil::Dirvector __D::cross(const Dirvector& o) const {
  return(Dirvector(x[1]*o.x[2]-o.x[1]*x[2], 
		   x[2]*o.x[0]-o.x[2]*x[0],
		   x[0]*o.x[1]-o.x[0]*x[1]));
  }

/// convert this vector from ECI to ECEF for a given epoch t
vecutil::Dirvector __D::eci2ecef(const tmtc::TmTcTime& t) const {
  // find days since 1968 JAN 1 00h 
  const double jd=t.getJd()-JD1968;
  const double zmt=::floor(jd);

  // get sidereal time day fraction
  double gst=23696.535 + 236.55536*zmt + 86636.55536*(jd-zmt);
  gst=fmod(gst/3600.0, 24.0);
   
  // equatorial pole at epoch (Z)
  vecutil::Dirvector z(0, 0, 1);

  // X
  vecutil::Dirvector x; x.fromCelestial(TWOPI*(gst)/24.0, 0.0);

  // Y
  vecutil::Dirvector y=z.cross(x);

  // precess to J2000 ECI coordinate frame
  z.precess(t, t2000); x.precess(t, t2000); y.precess(t, t2000);

  // make a quaternion from the direction cosine matrix
  vecutil::Quat q;
  const double dcm[3][3]={{x[0], x[1], x[2]},
                    {y[0], y[1], y[2]},
                    {z[0], z[1], z[2]}};
  const double w=sqrt(1+x[0]+y[1]+z[2])/2.0;
  q.set((dcm[1][2]-dcm[2][1])/(4*w),
        (dcm[2][0]-dcm[0][2])/(4*w),
        (dcm[0][1]-dcm[1][0])/(4*w), w);
  q.normalize();

  return(q.inverseTranslate(*this));
  }

/// convert this vector from ECEF to ECI for a given epoch t
vecutil::Dirvector __D::ecef2eci(const tmtc::TmTcTime& t) const {
  // find days since 1968 JAN 1 00h 
  const double jd=t.getJd()-JD1968;
  const double zmt=::floor(jd);

  // get sidereal time day fraction
  double gst=23696.535 + 236.55536*zmt + 86636.55536*(jd-zmt);
  gst=fmod(gst/3600.0, 24.0);
   
  // equatorial pole at epoch (Z)
  vecutil::Dirvector z(0, 0, 1);

  // X
  vecutil::Dirvector x; x.fromCelestial(TWOPI*(gst)/24.0, 0.0);

  // Y
  vecutil::Dirvector y=z.cross(x);

  // precess to J2000 ECI coordinate frame
  z.precess(t, t2000); x.precess(t, t2000); y.precess(t, t2000);

  // make a quaternion from the direction cosine matrix
  vecutil::Quat q;
  const double dcm[3][3]={{x[0], x[1], x[2]},
                    {y[0], y[1], y[2]},
                    {z[0], z[1], z[2]}};
  const double w=sqrt(1+x[0]+y[1]+z[2])/2.0;
  q.set((dcm[1][2]-dcm[2][1])/(4*w),
        (dcm[2][0]-dcm[0][2])/(4*w),
        (dcm[0][1]-dcm[1][0])/(4*w), w);
  q.normalize();

  return(q.translate(*this));
  }

double __D::angle(const Dirvector& y) const {
  return(acos((*this).safeDot(y)));
  }

vecutil::Dirvector __D::getRandomDirvector() {
  double ra=360.0*getRandomNumber(), dec=90-180*getRandomNumber();
  return(Dirvector().fromCelestial(DEG2RAD(ra), DEG2RAD(dec)));
  }

double& __D::operator [] (unsigned i) { return(x[i]); }
double __D::operator [] (unsigned i) const { return(x[i]); }


vecutil::Dirvector vecutil::operator - (const vecutil::Dirvector& one, 
					const vecutil::Dirvector& two) {
  return(Dirvector(one[0]-two[0], one[1]-two[1], one[2]-two[2]));
  }

vecutil::Dirvector vecutil::operator + (const vecutil::Dirvector& one, 
					const vecutil::Dirvector& two) {
  return(Dirvector(one[0]+two[0], one[1]+two[1], one[2]+two[2]));
  }

vecutil::Dirvector vecutil::operator - (const vecutil::Dirvector& one, 
					double two) {
  return(Dirvector(one[0]-two, one[1]-two, one[2]-two));
  }

vecutil::Dirvector vecutil::operator + (const vecutil::Dirvector& one, 
					double two) {
  return(Dirvector(one[0]+two, one[1]+two, one[2]+two));
  }

vecutil::Dirvector vecutil::operator * (const vecutil::Dirvector& one, 
				      double d) {
  return(Dirvector(one[0]*d, one[1]*d, one[2]*d));
  }

vecutil::Dirvector vecutil::operator * (double d, 
				      const vecutil::Dirvector& one) {
  return(Dirvector(one[0]*d, one[1]*d, one[2]*d));
  }

ostream& vecutil::operator << (ostream& os, 
			      const vecutil::Dirvector& dv) {
  os << fixed << setprecision(6) << dv[0] << " " << dv[1] << " " << dv[2];
  return(os);
  }

vecutil::Dirvector vecutil::operator / (const vecutil::Dirvector& one, 
					double two) {
  return(Dirvector(one[0]/two, one[1]/two, one[2]/two));
  }

vecutil::Dirvector vecutil::operator / (double two, 
					const vecutil::Dirvector& one) {
  return(Dirvector(one[0]/two, one[1]/two, one[2]/two));
  }
