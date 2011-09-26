/* H+
 *
 *  Title        : CalculateIntervalsIntersection.cc
 *  Author       : broberts
 *  Created      : Wed Apr 22 15:40:45 PDT 2009
 *  Synopsis     : wrapper to find intersection of two intervals
 *
 *  $LastChangedDate: 2009-04-22 16:43:05 -0700 (Wed, 22 Apr 2009) $
 *  $LastChangedRevision: 3268 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include "CalculateIntervalsIntersection.h"

typedef vecutil::CalculateIntervalsIntersection __V;


/// Constructor
__V::CalculateIntervalsIntersection(const CalculateIntervals& __i1,
				    const CalculateIntervals& __i2) : 
  i1(__i1), i2(__i2) { }

/// get intervals in intersection
tmtc::TmTcIntervalSet __V::getIntervals(const tmtc::TmTcTime& tStart,
					const tmtc::TmTcTime& tEnd) const {
  const tmtc::TmTcIntervalSet is1=i1.getIntervals(tStart, tEnd);
  const tmtc::TmTcIntervalSet is2=i2.getIntervals(tStart, tEnd);
  
  return(is1 & is2);
  }

vecutil::CalculateIntervalsIntersection operator & 
(const vecutil::CalculateIntervals& i1,
 const vecutil::CalculateIntervals& i2) {
  return(vecutil::CalculateIntervalsIntersection(i1, i2));
  }
