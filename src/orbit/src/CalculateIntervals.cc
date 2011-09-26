/* H+
 *
 *  Title        : CalculateIntervals.cc
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : base class for calculating orbital intervals
 *
 *  $LastChangedDate: 2009-04-22 14:47:43 -0700 (Wed, 22 Apr 2009) $
 *  $LastChangedRevision: 3265 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include <CalculateIntervals.h>


typedef vecutil::CalculateIntervals __V;


/// iterate to a bracketed crossing
tmtc::TmTcTime __V::polish(tmtc::TmTcTime& t1, int o1, 
                           tmtc::TmTcTime& t2, int o2) const {
  // calculate the midpoint time
  tmtc::TmTcTime t;
  t.setUnix((t1.getUnix()+t2.getUnix())/2.0);
  
  // time delta has reached stopping criterion
  if ((t2-t1)<dtStop)
    return(t);

  // find status at midpoint time
  const int __o=findStatus(t);

  // adjust bracket times based on status
  if (__o==o1) 
    t1=t;
  else if (__o==o2)
    t2=t;

  // recursive descent
  return(polish(t1, o1, t2, o2));
  }

/// calcalate intervals
tmtc::TmTcIntervalSet __V::getIntervals(tmtc::TmTcTime tStart,
                                        tmtc::TmTcTime tEnd) const {
  int oLast=0; 

  /// make sure start and end times are even multiples of increment timestep
  tStart=tStart.floor(dtInc.getSeconds());
  tEnd=tEnd.ceil(dtInc.getSeconds());

  tmtc::TmTcTime t, tIngress;
  tmtc::TmTcIntervalSet is; tmtc::TmTcTime t1, t2;
  for (t=tStart.floor(dtInc.getSeconds()); t<=tEnd; t+=dtInc) {
    t2=t;

    // find status at this time
    const int o=findStatus(t);
    
    // already in interval at start of search
    if (o==1 && oLast==0) {
      tIngress=t;
      }
    // found an state transition
    else if (o*oLast<0) {
      // find the exact time of the bracketed transition
      t=polish(t1, oLast, t2, o);

      // ingress found
      if (o==1) 
        tIngress=t;
      // egress found
      else {
        if (tIngress)
          is.insert(tmtc::TmTcInterval(tIngress, t));

        tIngress.setInvalid();
        }
      }

    t1=t;

    oLast=o; 
    }

  if (bool(tIngress) && bool(t))
    is.insert(tmtc::TmTcInterval(tIngress, t));

  return(is);
  }

/// set the grid search resolution in seconds, guaranteed to be a power of 2
void __V::setIncrementSize(double sec) {
  // make sure search will be >= 2.0 seconds
  sec=max(sec, 2.0);

  // make sure it's an even power of 2
  sec=pow(2, floor(log(sec)/log(2.0)));

  // set the increment
  dtInc.setSeconds(sec);
  }

/// set solution resolution, minimum of 2 seconds
void __V::setResolution(double sec) {
  // make sure stop time will be >= 2.0 seconds
  sec=max(sec, 2.0);

  // make sure it's an even power of 2
  sec=pow(2, floor(log(sec)/log(2.0)));

  // set the increment
  dtStop.setSeconds(sec);
  }
