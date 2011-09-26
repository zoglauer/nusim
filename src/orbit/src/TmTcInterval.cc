
 /* H+
 *
 *  Title        : TmTcInterval.cc
 *  Author       : broberts
 *  Created      : Mon May  1 10:13:04 PDT 2006
 *  Synopsis     : time interval
 *
 *  $LastChangedDate: 2006-07-21 14:18:29 -0700 (Fri, 21 Jul 2006) $
 *  $LastChangedRevision: 1158 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include "TmTcInterval.h"


/// test for intersection
bool tmtc::TmTcInterval::isIntersection(const TmTcInterval& ivl) const {
  // both intervals are not defined
  if (!ivl || !(*this))
    return(false);
  // intervals defined but do not intersect
  else if (tEnd<ivl.tStart || tStart>ivl.tEnd)
    return(false);
  // intersection
  else
    return(true);
  }
    
/// find the intersection of this interval and another one
tmtc::TmTcInterval 
tmtc::TmTcInterval::intersection(const TmTcInterval& ivl) const {
  // return an empty interval if either of the intervals is not defined
  if (!ivl || !(*this))
    return(TmTcInterval());
  // no intersection, return empty interval
  else if (tEnd<ivl.tStart || tStart>ivl.tEnd)
    return(TmTcInterval());
  // valid intersection
  else
    return(TmTcInterval(max(tStart, ivl.tStart), min(tEnd, ivl.tEnd)));
  }

/// find the union of this interval and another one
tmtc::TmTcInterval 
tmtc::TmTcInterval::join(const TmTcInterval& ivl) const {
  // return an empty interval if either of the intervals is not defined
  if (!ivl && !(*this))
    return(TmTcInterval());
  // this interval is not defined, so return the external one
  else if (!(*this))
    return(ivl);
  // external interval not defined, so return this one
  else if (!ivl)
    return(*this);
    
  // valid join
  return(TmTcInterval(min(tStart, ivl.tStart), max(tEnd, ivl.tEnd)));
  }

/// is this interval less than ivl? (ends before ivl starts)
bool tmtc::TmTcInterval::isLessThan(const TmTcInterval& ivl) const {
  return(tEnd<ivl.tStart);
  }

/// is this interval greater than ivl? (starts after ivl ends)
bool tmtc::TmTcInterval::isGreaterThan(const TmTcInterval& ivl) const {
  return(tStart>ivl.tEnd);
  }

/// Constructor
tmtc::TmTcInterval::TmTcInterval(void) {}

/// Constructor - set both times equal
tmtc::TmTcInterval::TmTcInterval(const tmtc::TmTcTime& t) : 
  tStart(t), tEnd(t) {}

/// two-arg constructor
tmtc::TmTcInterval::TmTcInterval(const tmtc::TmTcTime& _tStart,
				 const tmtc::TmTcTime& _tEnd) : 
  tStart(_tStart), tEnd(_tEnd) {
  /// flip the order if the interval is reverse defined
  if (_tStart>_tEnd) {
    tStart=_tEnd; tEnd=_tStart;
    }
  }
  
/// get interval start time
tmtc::TmTcTime tmtc::TmTcInterval::getStart(void) const { return(tStart); }
  
/// get interval end time
tmtc::TmTcTime tmtc::TmTcInterval::getEnd(void) const { return(tEnd); }

/// test if time t is included in this interval
bool tmtc::TmTcInterval::operator () (const tmtc::TmTcTime& t) const {
  return(t>=tStart && t<=tEnd);
  }
  
/// test if the interval is defined
tmtc::TmTcInterval::operator bool() const { 
  return(bool(tStart) && bool(tEnd)); }
  

/// get the duration spanned by this interval
tmtc::TmTcDuration tmtc::TmTcInterval::getSpan(void) const {
  if (!tStart || !tEnd)
    return(tmtc::TmTcDuration());
  else
    return(tEnd-tStart);
  }

bool tmtc::operator && (const TmTcInterval& one, const TmTcInterval& two) {
  return(one.isIntersection(two));
  }

bool tmtc::operator < (const TmTcInterval& one, const TmTcInterval& two) {
  return(one.isLessThan(two));
  }

bool tmtc::operator > (const TmTcInterval& one, const TmTcInterval& two) {
  return(one.isGreaterThan(two));
  }

tmtc::TmTcInterval tmtc::operator & (const TmTcInterval& one, 
				     const TmTcInterval& two) {
  return(one.intersection(two));
  }

tmtc::TmTcInterval& tmtc::operator &= (TmTcInterval& one, 
				       const TmTcInterval& two) {
  return(one=one.intersection(two));
  }

tmtc::TmTcInterval tmtc::operator | (const TmTcInterval& one, 
				     const TmTcInterval& two) {
  return(one.join(two));
  }

tmtc::TmTcInterval& tmtc::operator |= (TmTcInterval& one, 
				       const TmTcInterval& two) {
  return(one=one.join(two));
  }

ostream& tmtc::operator << (ostream& os, const TmTcInterval& ext) {
  os << ext.tStart << " " << ext.tEnd;
  return(os);
  }
