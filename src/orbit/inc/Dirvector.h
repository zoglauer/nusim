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

#ifndef _Dirvector_H
#define _Dirvector_H

#include <GUtil.h>
#include <TmTcTime.h>


namespace vecutil {

class Dirvector {
private:
  double x[3];

public:
  /// Constructor
  Dirvector(void);

  /// Constructor
  Dirvector(double x0, double x1, double x2);

  /// normalize this vector 
  Dirvector& normalize(void);

  /// normalize this vector with a constant
  Dirvector& normalize(double c);
  
  /// normalize a copy of this vector
  Dirvector normalize_copy(void) const;

  /// take the negative of the vector
  Dirvector& negate(void);

  /// take the negative of the vector
  Dirvector negate_copy(void) const;
  
  /// find the length fo this vector
  double length(void) const;

  /// project this vector onto a plane defined by axisVector
  Dirvector project(const Dirvector& axisVector, 
		    double thedot) const;

  /// project this vector onto a plane defined by axisVector
  Dirvector project(const Dirvector& axisVector) const;

  /// convert from celestial coordinates (RA, Dec) to a direction vector
  /// argument must be in radians
  Dirvector fromCelestial(double ra, double dec);

  /// convert the direction vector to back to RA, Dec (in radians)
  void toCelestial(double& ra, double& dec) const;

  /// internally convert the direction vector from equatorial to ecliptic
  /// coordinate frame
  void equ2ecl(void);

  /// internally convert the direction vector from ecliptic to equatorial 
  /// coordinate frame
  void ecl2equ(void);

  /// precess the direction vector to epoch t2, assuming it is
  /// currently represented as epoch t1
  void precess(const tmtc::TmTcTime& t1, const tmtc::TmTcTime& t2);

  /// find the dot product of this vector with another
  double dot(const Dirvector& o) const;

  /// find the dot product of this vector, guaranteed to be in the range [-1,1]
  double safeDot(const Dirvector& y) const;

  /// find the angle between two vectors, in radians
  double angle(const Dirvector& y) const;

  static bool coneIntersect(const Dirvector&, const Dirvector&,
                            const Dirvector&, const Dirvector&,
                            Dirvector&, Dirvector&);

  static bool coneIntersect(const Dirvector&, double,
                            const Dirvector&, double, 
                            Dirvector&, Dirvector&);

  // the angle between the projection of two vectors onto a plane
  static double planeAngle(const Dirvector&, const Dirvector&, 
                           const Dirvector&);

  // get a vector that lies on the edge of the cone defined by this
  // vector and half-angle 'angle'
  Dirvector edgeOfCone(double angle) const;

  /// find the cross product between this vector and an external one
  /// (e.g. this x o)
  Dirvector cross(const Dirvector& o) const;

  /// convert this vector from ECI to ECEF for a given epoch
  Dirvector eci2ecef(const tmtc::TmTcTime& t) const;

  /// convert this vector from ECEF to ECI for a given epoch
  Dirvector ecef2eci(const tmtc::TmTcTime& t) const;

  double& operator [] (unsigned i);
  double operator [] (unsigned i) const;

  static Dirvector getRandomDirvector();
  };


Dirvector operator - (const Dirvector& one, const Dirvector& two);
Dirvector operator + (const Dirvector& one, const Dirvector& two);
Dirvector operator - (const Dirvector& one, double two);
Dirvector operator + (const Dirvector& one, double two);

Dirvector operator * (const Dirvector& one, double d);
Dirvector operator * (double d, const Dirvector& one);

Dirvector operator / (const Dirvector& one, double two);
Dirvector operator / (double two, const Dirvector& one);

ostream& operator << (ostream& os, const Dirvector& dv);

}

#endif
