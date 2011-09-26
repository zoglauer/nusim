/* H+
 *
 *  Title        : CalculateIntervalsIntersection.h
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

#ifndef _CalculateIntervalsIntersection_H
#define _CalculateIntervalsIntersection_H

#include <CalculateIntervals.h>


namespace vecutil {

/// wrapper class to calculate intersection between two intervals modules
class CalculateIntervalsIntersection {
private:
  /// first interval module
  const CalculateIntervals& i1;

  /// second interval module
  const CalculateIntervals& i2;

public:
  /// Constructor
  CalculateIntervalsIntersection(const CalculateIntervals& __i1,
				 const CalculateIntervals& __i2);

  /// get intervals in intersection
  tmtc::TmTcIntervalSet getIntervals(const tmtc::TmTcTime& tStart,
				     const tmtc::TmTcTime& tEnd) const;
  };

}

vecutil::CalculateIntervalsIntersection operator & 
(const vecutil::CalculateIntervals& i1,
 const vecutil::CalculateIntervals& i2);

#endif
