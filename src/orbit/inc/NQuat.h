/* H+
 *      Title     : Quat.h
 *      Author    : Bryce A. Roberts
 *      Date      : Some time after the Mars Pathfinder landing
 *      Synopsis  : Quaternion math library
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#ifndef _QUAT_H
#define _QUAT_H

#include <GUtil.h>
#include <Dirvector.h>
#include <NEuler.h>


namespace vecutil {

class Quat {
protected:
  // stores quaternion as a 4-tuple of doubles. the axis of rotation
  // (aka the pure part of the quaternion) is stored in quat[0], 
  // quat[1], and quat[2]; the rotation about the axis (aka the 
  // real part) is stored in quat[3], following the convention 
  // that seems to be used in most code I've seen
  double quat[4];

public:
  /************************************************************
  ** Constructors
  ************************************************************/ 
  // default constructor
  Quat(void);

  // copy constructor
  Quat(const Quat& old);

  // assignment constructor - set each element of the quaternion
  // first three arguments are the vector part, fourth is the scalar part
  Quat(double a, double b, double c, double d);

  // assignment constructor - set the pure (vector) and real (scalar) part
  Quat(const Dirvector& purePart, double realPart);

  // equivalent to those defined by three Euler angles;
  Quat(Euler e);

  /************************************************************
  ** Access functions
  ************************************************************/
  // initialize to the identity quaternion ([0, 0, 0], 1)
  Quat identity(void);

  // set each element of the quaternion first three arguments are the 
  // vector part, fourth is the scalar part
  Quat set(double a, double b, double c, double d);

  // set the pure (vector) and real (scalar) part
  Quat set(const Dirvector& purePart, double realPart);

  // set the quaternion to express a rotation equivalent to those 
  // defined by three Euler angles;
  Quat set(Euler e);

  // set just the scalar (real) part of the quaternion
  Quat setScalar(double d);

  // set just the vector (pure) part of the quaternion
  Quat setVector(Dirvector v);

  // get the real part of the quaternion
  double getScalar(void) const;

  // get the pure part of the quaternion as a Dirvector
  Dirvector getVector(void) const;

  // allow access to the internal data more directly by overloading the
  // [] operator
  double operator[] (const unsigned int index) const;
  double& operator[] (const unsigned int index);

  /************************************************************
  ** Math functions
  ************************************************************/
  // return the length of the quaternion
  double norm(void) const;

  // make the quaternion a unit quaternion
  Quat normalize(void);

  // invert a quaternion under the assumption that it is a unit quaternion
  Quat unitInverse(void);

  // flip the sign of every quaternion element (represents same rotation)
  Quat negate(void);

  // add a quaternion to this one
  Quat add(Quat q);

  // scalar multiplication
  Quat multiplyConstant(double c);

  // multiply the quaternion on the left
  // (quaternion multiplication is NOT commutative)
  Quat multiplyLeft(const Quat& q1);

  // multiply the quaternion on the right
  // (quaternion multiplication is NOT commutative)
  Quat multiplyRight(const Quat& q2);

  // multiply two external quaternions and store the result internally
  static Quat multiply(const Quat& q1, const Quat& q2);

  /************************************************************
  ** Advanced operations
  ************************************************************/
  // use the quaternion to find the projection of an arbitry vector
  // onto a plane 
  Dirvector project(const Dirvector& planeNormal, const Dirvector& vec,
		    double angSep) const;

  // use the quaternion to find the projection of an arbitry vector
  // onto a plane 
  Dirvector project(const Dirvector& planeNormal, const Dirvector& vec) const;

  // use the quaternion to find the projection of an arbitrary vector
  // onto a plane; plane should not be in absolute coord system
  Dirvector projectAbs(const Dirvector& planeNormal, const Dirvector& vec);

  // set the quaterion to express a rotation about a _single_ body axis;
  Quat singleAxisRotation(double angle, unsigned axis);

  // set the quaternion to express a rotation about an arbitrary vector
  Quat arbitraryAxisRotation(const Dirvector& vec, double theta);

  // set the quaternion to express a rotation equivalent to the
  // rotation of one vector into another;
  // this assumes that v1 and v2 are already unit vectors;
  Quat fromVectors(const Dirvector& v1, const Dirvector& v2);

  // find the delta quaternion
  Quat delta(const Quat& arg) const;

  // find the magnitude of a rotation of one quaternion into another
  double deltaMagnitude(const Quat& arg) const;

  /*
  // convert quaternion to Euler angles
  Euler toEuler(unsigned first, unsigned second, unsigned third) const;
  */

  // use a rotation defined by a quaternion to translate a vector from
  // one reference frame to another
  // rotate the vector, using the formula Vrot=q*Vec*q^-1
  Dirvector translate(const Dirvector& d) const;

  // use a rotation defined by a quaternion to translate a vector from
  // one reference frame to another, but in the reverse direction
  // as translate
  // rotate the vector, using the formula Vrot=q*Vec*q^-1
  Dirvector inverseTranslate(const Dirvector& d) const;

  // rotate dv1 into dv2, using rotations first about dvAxis1, then about
  // dvAxis2
  Quat fromVectorsAroundVectors(const Dirvector& dv1, const Dirvector& dv2,
    const Dirvector& dvAxis1, const Dirvector& dvAxis2);

  // get a random quaternion
  static Quat getRandomQuat();

  // get a random quaternion that is angle radians "away" from q1
  static Quat getRandomQuat(const Quat& q1, double angle);
  };

ostream &operator << (ostream &out_file, const Quat& quat);

bool operator < (const Quat& q1, const Quat& q2);

}

#endif

