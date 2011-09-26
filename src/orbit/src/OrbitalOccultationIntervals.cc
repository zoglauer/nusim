/* H+
 *
 *  Title        : OrbitalOccultationIntervals.cc
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : class for calculating occultations of a target
 *
 *  $LastChangedDate: 2009-04-22 08:52:46 -0700 (Wed, 22 Apr 2009) $
 *  $LastChangedRevision: 3262 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include "OrbitalOccultationIntervals.h"
#include "OrbitModel.h"


typedef vecutil::OrbitalOccultationIntervals __V;


/// find occultation status at an instant in time
int __V::findStatus(const tmtc::TmTcTime& t) const {
  // get the orbit state vector
  const vecutil::StateVector sv=om->getStateVector(t);
  
  // find earth half angle at time
  const double ha=asin(vecutil::KM_PER_EARTH/sv.getAltitude())+limbAngle;
    
  // get the nadir vector
  const vecutil::Dirvector dvNadir=sv.getPosition().negate();
    
  // calculate te occultation status
  const int occ=(dvNadir.angle(dvTarg)<ha) ? 1 : -1;
    
  return(occ);
  }


/// Constructor
__V::OrbitalOccultationIntervals(vecutil::OrbitModel *__om) :
  OrbitalIntervals(__om), limbAngle(0) { }

/// set the target direction vector
void __V::setTarget(const vecutil::Dirvector& dvTarg) {
  OrbitalOccultationIntervals::dvTarg=dvTarg;
  }

/// set the limb angle for occultation
void __V::setLimbAngle(double limbAngle) {
  OrbitalOccultationIntervals::limbAngle=limbAngle;
  }
