/* H+
 *
 *  Title        : OrbitalNightIntervals.h
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

#ifndef _OrbitalNightIntervals_H
#define _OrbitalNightIntervals_H


#include <GUtil.h>
#include <OrbitalIntervals.h>


namespace vecutil {


class OrbitalNightIntervals : public vecutil::OrbitalIntervals {
public:
  /// event type
  enum event_enum { UMBRA, PENUMBRA };

protected:
  /// the type of eclipse (umbra or penumbra)
  event_enum eventType;

public:
  /// find occultation status at an instant in time
  int findStatus(const tmtc::TmTcTime& t) const;

  /// Constructor
  OrbitalNightIntervals(vecutil::OrbitModel *__om);

  /// set the desired event type
  void setEventType(event_enum eventType);
  };

}

#endif
