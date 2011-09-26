/* H+
       Title     : Quat.cc
       Author    : Bryce A. Roberts
       Date      : Some time after the Mars Pathfinder landing
       Synopsis  : Quaternion math library
       SCCS      : %W% %G%
       Revisions :
       mm/dd/yy name   description
 H-
*/

#include "Quat.h"

typedef vecutil::Quat __V;
using namespace vecutil;


/************************************************************
** Constructors
************************************************************/ 
// default constructor
__V::Quat(void) { quat[0]=0.0; quat[1]=0.0; quat[2]=0.0; quat[3]=1.0; }

// copy constructor
__V::Quat(const Quat& old) {
  quat[0]=old.quat[0]; quat[1]=old.quat[1];
  quat[2]=old.quat[2]; quat[3]=old.quat[3];
  }

// assignment constructor - set each element of the quaternion
// first three arguments are the vector part, fourth is the scalar part
__V::Quat(double a, double b, double c, double d) {
  quat[0]=a; quat[1]=b; quat[2]=c; quat[3]=d;
  }

// assignment constructor - set the pure (vector) and real (scalar) part
__V::Quat(const Dirvector& purePart, double realPart) {
  quat[0]=purePart[0]; quat[1]=purePart[1]; quat[2]=purePart[2];
  quat[3]=realPart;
  }

// assignment constructor - build a quaternion that expresses a rotation 
// equivalent to those defined by three Euler angles;
// the result is guaranteed to be a unit quaternion
__V::Quat(Euler e) { set(e); }

/************************************************************
** Access functions
************************************************************/
// initialize to the identity quaternion ([0, 0, 0], 1)
Quat __V::identity(void) {
  quat[0]=0.0; quat[1]=0.0; quat[2]=0.0; quat[3]=1.0;
  return(*this);
  }

// set each element of the quaternion first three arguments are the 
// vector part, fourth is the scalar part
Quat __V::set(double a, double b, double c, double d) {
  quat[0]=a; quat[1]=b; quat[2]=c; quat[3]=d;    
  return(*this);
  }

// set the pure (vector) and real (scalar) part
Quat __V::set(const Dirvector& purePart, double realPart) {
  quat[0]=purePart[0]; quat[1]=purePart[1]; quat[2]=purePart[2];
  quat[3]=realPart;
  return(*this);
  }

// set the quaternion to express a rotation equivalent to those 
// defined by three Euler angles;
Quat __V::set(Euler e) {
  double phi, theta, psi;
  unsigned phiAxis, thetaAxis, psiAxis;
  e.get(phi, theta, psi, phiAxis, thetaAxis, psiAxis);
  
  // first rotation
  singleAxisRotation(phi, phiAxis);

  // second rotation
  Quat q2;
  q2.singleAxisRotation(theta, thetaAxis);
  
  // third rotation
  Quat q3;
  q3.singleAxisRotation(psi, psiAxis);

  // build a quaternion that represents all three rotations
  multiplyRight(q2); multiplyRight(q3);

  return(*this);
  }

// set just the scalar (real) part of the quaternion
Quat __V::setScalar(double d) { quat[3]=d; return(*this); }

// set just the vector (pure) part of the quaternion
Quat __V::setVector(Dirvector v) { 
  quat[0]=v[0]; quat[1]=v[1]; quat[2]=v[2]; 
  return(*this);
  }


// get the real part of the quaternion
double __V::getScalar(void) const { return(quat[3]); }

// get the pure part of the quaternion as a Dirvector
Dirvector __V::getVector(void) const {
  return(Dirvector(quat[0], quat[1], quat[2]));
  }

// allow access to the internal data more directly by overloading the
// [] operator
double __V::operator[] (const unsigned int index) const {
  return(quat[index]); 
  }
double& __V::operator[] (const unsigned int index) { return(quat[index]); }

/************************************************************
** Math functions
************************************************************/
// return the length of the quaternion
double __V::norm(void) const {
  return(sqrt(quat[0]*quat[0]+quat[1]*quat[1]+
	      quat[2]*quat[2]+quat[3]*quat[3]));
  }

// make the quaternion a unit quaternion
Quat __V::normalize(void) {
  register double norm=sqrt(quat[0]*quat[0]+quat[1]*quat[1]+
			    quat[2]*quat[2]+quat[3]*quat[3]);

  quat[0]/=norm; quat[1]/=norm; quat[2]/=norm; quat[3]/=norm;
  
  return(*this);
  }

// invert a quaternion under the assumption that it is a unit quaternion
// keep the scalar element positive
Quat __V::unitInverse(void) {
  if (quat[3]<0)
    quat[3]*=-1;
  else {
    quat[0]=-quat[0]; quat[1]=-quat[1]; quat[2]=-quat[2];
    }

  return(*this);
  }

// flip the sign of every quaternion element (represents same rotation)
Quat __V::negate(void) {
  quat[0]*=-1.0; quat[1]*=-1.0; quat[2]*=-1.0; quat[3]*=-1.0;

  return(*this);
  }

// add
Quat __V::add(Quat q) {
  quat[0]+=q[0]; quat[1]+=q[1]; quat[2]+=q[2]; quat[3]+=q[3];
  
  return(*this);
  }

// scalar multiplication
Quat __V::multiplyConstant(double c) {
  quat[0]*=c; quat[1]*=c; quat[2]*=c; quat[3]*=c;
  
  return(*this);
  }

// multiply the quaternion on the left
// (quaternion multiplication is NOT commutative)
Quat __V::multiplyLeft(const Quat& q1) {
  Quat q;
    
  q[3]=q1[3]*quat[3] - q1[0]*quat[0] - q1[1]*quat[1] - q1[2]*quat[2];
  q[0]=q1[3]*quat[0] + q1[0]*quat[3] + q1[1]*quat[2] - q1[2]*quat[1];
  q[1]=q1[3]*quat[1] - q1[0]*quat[2] + q1[1]*quat[3] + q1[2]*quat[0];
  q[2]=q1[3]*quat[2] + q1[0]*quat[1] - q1[1]*quat[0] + q1[2]*quat[3];
  
  memcpy((char *)(&quat), (char *)(&q), sizeof(quat));

  return(*this);
  }

// multiply the quaternion on the right
// (quaternion multiplication is NOT commutative)
Quat __V::multiplyRight(const Quat& q2) {
  Quat q;
  
  q[3]=quat[3]*q2[3] - quat[0]*q2[0] - quat[1]*q2[1] - quat[2]*q2[2];
  q[0]=quat[3]*q2[0] + quat[0]*q2[3] + quat[1]*q2[2] - quat[2]*q2[1];
  q[1]=quat[3]*q2[1] - quat[0]*q2[2] + quat[1]*q2[3] + quat[2]*q2[0];
  q[2]=quat[3]*q2[2] + quat[0]*q2[1] - quat[1]*q2[0] + quat[2]*q2[3];
  
  memcpy((char *)(&quat), (char *)(&q), sizeof(quat));
    
  return(*this);
  }

// multiply two external quaternions and store the result internally
Quat __V::multiply(const Quat& q1, const Quat& q2) {
  Quat quat;

  quat[3]=q1[3]*q2[3] - q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2];
  quat[0]=q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];
  quat[1]=q1[3]*q2[1] - q1[0]*q2[2] + q1[1]*q2[3] + q1[2]*q2[0];
  quat[2]=q1[3]*q2[2] + q1[0]*q2[1] - q1[1]*q2[0] + q1[2]*q2[3];    

  return(quat);
  }

/************************************************************
** Advanced operations
************************************************************/
// use the quaternion to find the projection of an arbitry vector
// onto a plane 
Dirvector __V::project(const Dirvector& planeNormal, const Dirvector& vec,
		    double angSep) const {
  // planeNormal is a vector that defines the plane onto which 
  // vec will be projected
  Dirvector norm=(*this).translate(planeNormal);
    
  return(Dirvector(vec[0]-angSep*norm[0], vec[1]-angSep*norm[1],
		   vec[2]-angSep*norm[2]));
  }

// use the quaternion to find the projection of an arbitry vector
// onto a plane 
Dirvector __V::project(const Dirvector& planeNormal, const Dirvector& vec)
  const {
  // planeNormal is a vector that defines the plane onto which 
  // vec will be projected
  Dirvector norm=(*this).translate(planeNormal);

  // angular separation (dot product between plane normal and vector)
  // is not provided, so we have to calculate it
  register double angSep=norm.safeDot(vec);
    
  return(Dirvector(vec[0]-angSep*norm[0], vec[1]-angSep*norm[1],
		   vec[2]-angSep*norm[2]));
  }

// use the quaternion to find the projection of an arbitrary vector
// onto a plane; plane should not be in absolute coord system
Dirvector __V::projectAbs(const Dirvector& planeNormal, 
			   const Dirvector& vec) {
  register double angSep=planeNormal.safeDot(vec);
    
  return(Dirvector(vec[0]-angSep*planeNormal[0], 
		   vec[1]-angSep*planeNormal[1],
		   vec[2]-angSep*planeNormal[2]));
  }


// set the quaterion to express a rotation about a _single_ body axis;
Quat __V::singleAxisRotation(double angle, unsigned axis) {
  // make sure the vector part of the quaternion is zero'd
  quat[0]=0.0; quat[1]=0.0; quat[2]=0.0;

  quat[axis-1]=sin(angle/2.0); quat[3]=cos(angle/2.0);
    
  return(*this);
  }

// set the quaternion to express a rotation about an arbitrary vector
Quat __V::arbitraryAxisRotation(const Dirvector& vec, double theta) { 
  double sin_theta=sin(theta/2.0);

  quat[0]=vec[0]*sin_theta;
  quat[1]=vec[1]*sin_theta;
  quat[2]=vec[2]*sin_theta;
  quat[3]=cos(theta/2.0);
    
  return(*this);
  }

// set the quaternion to express a rotation equivalent to the
// rotation of one vector into another;
// this assumes that v1 and v2 are already unit vectors;
Quat __V::fromVectors(const Dirvector& v1, const Dirvector& v2) {
  // make a vector perpendicular to both v1 and v2
  // the resultant vector will be the pole of the rotation
  Dirvector pole=v1.cross(v2).normalize();

  // the magnitude of the rotation comes from the inner product 
  // of v1 and v2
  double theta=acos(v1.safeDot(v2))/2.0;

  // form the quaternion
  quat[0]=pole[0]*sin(theta);
  quat[1]=pole[1]*sin(theta);
  quat[2]=pole[2]*sin(theta);
  quat[3]=cos(theta);

  normalize();

  return(*this);
  }

// find the delta quaternion
Quat __V::delta(const Quat& arg) const {
  // find the delta quaternion
  Quat temp=multiply(Quat(*this).unitInverse(), arg).normalize();
  
  // make sure scalar part is positive, to guarantee positive rotation
  // about the eigenaxis
  if (temp.getScalar()<0) 
    temp.negate();

  return(temp);
  }

// find the magnitude of a rotation of one quaternion into another
double __V::deltaMagnitude(const Quat& arg) const {
  Quat qDelta=delta(arg);
  return(2*fabs(atan(sqrt(qDelta.getVector().safeDot(qDelta.getVector()))/
		     qDelta.getScalar())));
  }


/*
// convert quaternion to Euler angles
Euler __V::toEuler(unsigned first, unsigned second, unsigned third) const {
  unsigned firstOld=first, secondOld=second, thirdOld=third;
  
  static int signArr[4][4]={{0, 0, 0, 0}, {0, 0, 1, -1}, {0, -1, 0, 1},
			    {0, 1, -1, 0} };

  int sgn=signArr[first][second];
  double alpha, beta, phi, theta, psi;

  // case of repeating Euler angle sequences
  if (first==third) {
    // modified rule for finding the "third" Euler axis
    third=6-first-second;
    
    alpha=atan2(quat[first-1], quat[3]);
    beta=atan2(double(sgn)*quat[third-1],quat[second-1]);
    theta=2*asin(sqrt(quat[second-1]*quat[second-1]+
		      quat[third-1]*quat[third-1]));
    phi=alpha+beta;
    psi=alpha-beta;
    }
  // case of non-repeating Euler angle sequences
  else {
    alpha=atan2(quat[first-1]+quat[third-1], 
		quat[3]+double(sgn)*quat[second-1]);
    beta=atan2(quat[first-1]-quat[third-1], 
	       quat[3]-double(sgn)*quat[second-1]);
    theta=asin(2*((quat[second-1]*quat[3]) + 
		  double(sgn)*quat[first-1]*quat[third-1]));
    
    // make sure the angles are in the customary ranges:
    // phi=[0, 2PI), psi=(-PI,PI]
    phi=alpha+beta; 
    }

  if (phi<0) 
    phi+=TWOPI;
  psi=alpha-beta;
  if (psi<PI)
    psi+=TWOPI;
  if (psi>PI)
    psi-=TWOPI;
  
  return(Euler(phi, theta, psi, firstOld, secondOld, thirdOld));    
  }
*/

// use a rotation defined by a quaternion to translate a vector from
// one reference frame to another
// rotate the vector, using the formula Vrot=q*Vec*q^-1
Dirvector __V::translate(const Dirvector& d) const{
  double q0, q1, q2, q3;

  q3=-quat[0]*d[0] - quat[1]*d[1] - quat[2]*d[2];
  q0= quat[3]*d[0] + quat[1]*d[2] - quat[2]*d[1];
  q1= quat[3]*d[1] - quat[0]*d[2] + quat[2]*d[0];
  q2= quat[3]*d[2] + quat[0]*d[1] - quat[1]*d[0];

  Dirvector dThis;
    
  dThis[0]=-q3*quat[0] + q0*quat[3] - q1*quat[2] + q2*quat[1];
  dThis[1]=-q3*quat[1] + q0*quat[2] + q1*quat[3] - q2*quat[0];
  dThis[2]=-q3*quat[2] - q0*quat[1] + q1*quat[0] + q2*quat[3];

  return(dThis);
  }

// use a rotation defined by a quaternion to translate a vector from
// one reference frame to another, but in the reverse direction
// as translate
// rotate the vector, using the formula Vrot=q*Vec*q^-1
Dirvector __V::inverseTranslate(const Dirvector& d) const {
  double q0, q1, q2, q3;
  
  q3=-quat[0]*d[0] - quat[1]*d[1] - quat[2]*d[2];
  q0=-quat[3]*d[0] + quat[1]*d[2] - quat[2]*d[1];
  q1=-quat[3]*d[1] - quat[0]*d[2] + quat[2]*d[0];
  q2=-quat[3]*d[2] + quat[0]*d[1] - quat[1]*d[0];

  Dirvector dThis;
    
  dThis[0]=-q3*quat[0] - q0*quat[3] - q1*quat[2] + q2*quat[1];
  dThis[1]=-q3*quat[1] + q0*quat[2] - q1*quat[3] - q2*quat[0];
  dThis[2]=-q3*quat[2] - q0*quat[1] + q1*quat[0] - q2*quat[3];

  return(dThis);
  }

// rotate dv1 into dv2, using rotations first about dvAxis1, then about
// dvAxis2
Quat __V::fromVectorsAroundVectors(const Dirvector& dv1, const Dirvector& dv2,
				    const Dirvector& dvAxis1, 
				    const Dirvector& dvAxis2) {
  Dirvector dvI, dvI1, dvI2;
  double twoRot, oneRot;
  Quat qTwo;
    
  // find intersection between Y- and Z-cones 
  Dirvector::coneIntersect(dvAxis1, dv2, dvAxis2, dv1, dvI1, dvI2);

  // find intersection vector to use to give accute twoRot angle
  dvI=(dvI1.safeDot(dv1)>=0) ? dvI1 : dvI2;
  
  // rotate dv1 about dvY to become intersection vector
  twoRot=Dirvector::planeAngle(dv1, dvI, dvAxis2);
  arbitraryAxisRotation(dvAxis2, twoRot);

  // rotate dvI about Z to become final vector
  oneRot=Dirvector::planeAngle(dvI, dv2, dvAxis1);
  qTwo.arbitraryAxisRotation(dvAxis1, oneRot);
  
  // combine the two rotations
  multiplyLeft(qTwo);
  
  return(*this);
  }

// get a random quaternion
Quat __V::getRandomQuat() {
  return(Quat(getRandomNumber(), getRandomNumber(), getRandomNumber(),
	      fabs(getRandomNumber())).normalize());
  }

// get a random quaternion that is angle radians "away" from q1
Quat __V::getRandomQuat(const Quat& q1, double angle) {
  return(multiply(q1, 
    Quat().arbitraryAxisRotation(Dirvector::getRandomDirvector(), angle)));
  }


// overloaded output operator for Quat class
ostream &operator << (ostream &out_file, const Quat& quat) {
  out_file << quat[0] << " " << quat[1] << " " 
	   << quat[2] << " " << quat[3];
  return(out_file);
  }

bool operator < (const Quat& q1, const Quat& q2) {
  return(q1[3]<q2[3]);
  }
