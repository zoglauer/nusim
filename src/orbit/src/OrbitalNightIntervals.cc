/* H+
 *
 *  Title        : OrbitalNightIntervals.cc
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : class for calculating occultations of a target
 *
 *  $LastChangedDate: 2011-08-15 20:11:06 -0700 (Mon, 15 Aug 2011) $
 *  $LastChangedRevision: 6408 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include "OrbitalNightIntervals.h"
#include "OrbitModel.h"
#include "CelestialTarget.h"


typedef vecutil::OrbitalNightIntervals __V;

static const double haSun=DEG2RAD(0.53313)/2.0;


/// find occultation status at an instant in time
int __V::findStatus(const tmtc::TmTcTime& t) const {
  // get the orbit state vector
  const vecutil::StateVector sv=om->getStateVector(t);

  // get the antisun vector
  Dirvector dvAntisun=CelestialTarget::getSun(t).negate();

  // half angle of the solid earth
  const double ha=asin(KM_PER_EARTH/sv.getAltitude());

  // test if in region.  Penumbra: part of the sun is occulted
  // Umbra: all of the sub is occulted
  switch (eventType) {
  case PENUMBRA:
    return((sv.getPosition().angle(dvAntisun)<ha+haSun) ? +1 : -1);
  case UMBRA:
    return((sv.getPosition().angle(dvAntisun)<ha-haSun) ? +1 : -1);
    }

  // this will never be reached
  return(0);
  }


/// Constructor
__V::OrbitalNightIntervals(vecutil::OrbitModel *__om) :
  OrbitalIntervals(__om), eventType(PENUMBRA) { }

/// set the desired event type
void __V::setEventType(event_enum eventType) {
  OrbitalNightIntervals::eventType=eventType;
  }
