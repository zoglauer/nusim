/* H+
 *      Title     : Euler.cc
 *      Author    : Bryce A. Roberts
 *      Date      : 3 September 1998
 *      Synopsis  : Euler angle container class
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */


#include "NEuler.h"

using namespace vecutil;

/************************************************************
** Constructors
************************************************************/
// default constructor
Euler::Euler(void) { 
  phi=0.0; theta=0.0; psi=0.0; 
  eulerOrder[0]=3; eulerOrder[1]=2; eulerOrder[2]=1;
  }

// assignment constructor - must specify the angles as well as the
// order, otherwise the Euler angles are ambiguous
Euler::Euler(double phiArg, double thetaArg, double psiArg, 
      unsigned phiAxis, unsigned thetaAxis, unsigned psiAxis) {
  phi=phiArg; theta=thetaArg; psi=psiArg;
  eulerOrder[0]=phiAxis; eulerOrder[1]=thetaAxis; eulerOrder[2]=psiAxis;
  }

/************************************************************
** Initialization
************************************************************/
// set the Euler angles - must specify the angles as well as the
// order, otherwise the Euler angles are ambiguous
void Euler::set(double phiArg, double thetaArg, double psiArg, 
		unsigned phiAxis, unsigned thetaAxis, unsigned psiAxis) {
  phi=phiArg; theta=thetaArg; psi=psiArg;
  eulerOrder[0]=phiAxis; eulerOrder[1]=thetaAxis; eulerOrder[2]=psiAxis;
  } 

/************************************************************
** Access functions
************************************************************/
// get the Euler angles and the axes they are associated with
void Euler::get(double& phiArg, double& thetaArg, double& psiArg,
		unsigned& phiAxis, unsigned& thetaAxis, 
		unsigned& psiAxis) const {
  phiArg=phi; thetaArg=theta; psiArg=psi; 
  phiAxis=eulerOrder[0]; thetaAxis=eulerOrder[1]; psiAxis=eulerOrder[2];
  }
  
