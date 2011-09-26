/* H+
 *      Title     : Euler.h
 *      Author    : Bryce A. Roberts
 *      Date      : 3 September 1998
 *      Synopsis  : Euler angle container class
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#ifndef _Euler_H
#define _Euler_H

#include <GUtil.h>

namespace vecutil {

struct Euler {
protected:
  // phi, theta, and psi are the first, second, and third Euler angle
  // rotations (expressed in radians)
  double phi, theta, psi;

  // stores about which axes the phi, theta, and psi rotations are;
  // this is necessary because yaw, pitch, roll is NOT the same rotation 
  // as (for instance) roll, pitch, yaw
  unsigned eulerOrder[3];

public:
  /************************************************************
  ** Constructors
  ************************************************************/
  // default constructor
  Euler(void);

  // assignment constructor - must specify the angles as well as the
  // order, otherwise the Euler angles are ambiguous
  Euler(double phiArg, double thetaArg, double psiArg, 
	unsigned phiAxis, unsigned thetaAxis, unsigned psiAxis);

  /************************************************************
  ** Initialization
  ************************************************************/
  // set the Euler angles - must specify the angles as well as the
  // order, otherwise the Euler angles are ambiguous
  void set(double phiArg, double thetaArg, double psiArg, 
	unsigned phiAxis, unsigned thetaAxis, unsigned psiAxis);

  /************************************************************
  ** Access functions
  ************************************************************/
  // get the Euler angles and the axes they are associated with
  void get(double& phiArg, double& thetaArg, double& psiArg,
	   unsigned& phiAxis, unsigned& thetaAxis, unsigned& psiAxis) const;
  };

}

#endif

