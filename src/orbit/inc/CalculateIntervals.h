/* H+
 *
 *  Title        : CalculateIntervals.h
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : base class for calculating intervals
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

#ifndef _CalculateIntervals_H
#define _CalculateIntervals_H

#include <GUtil.h>
#include <TmTcIntervalSet.h>


namespace vecutil {

const static double DT_INC_DEFAULT  = 256.0;
const static double DT_STOP_DEFAULT = 2.0;


class CalculateIntervals {
protected:
  /// search quantum -- seconds between grid points
  tmtc::TmTcDuration dtInc;

  /// stopping quantum -- seconds between polish steps
  tmtc::TmTcDuration dtStop;

  /// find the interval status at time t
  /// this must be implemented by the child class
  /// it should return +1 at the start of the interval, -1 at the end
  /// the definition of the interval is thus left to the implementation
  /// of the child class
  virtual int findStatus(const tmtc::TmTcTime& t) const=0;

  /// iterate to a bracketed interval crossing (ingress or egress)
  tmtc::TmTcTime polish(tmtc::TmTcTime& t1, int o1, 
                        tmtc::TmTcTime& t2, int o2) const;

  /// Constructor
  CalculateIntervals(void) { 
    setIncrementSize(DT_INC_DEFAULT);
    dtStop.setSeconds(DT_STOP_DEFAULT);
  }
  virtual ~CalculateIntervals() {}

public:
  /// Calcalate intervals
  virtual tmtc::TmTcIntervalSet getIntervals(tmtc::TmTcTime tStart, 
					     tmtc::TmTcTime tEnd) const;

  /// set the grid search resolution in seconds, guaranteed to be a power of 2
  void setIncrementSize(double sec);

  /// set solution resolution, minimum of 2 seconds
  void setResolution(double sec);
  };

}

#include <CalculateIntervalsIntersection.h>


#endif
