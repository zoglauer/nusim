 /* H+
 *
 *  Title        : TmTcIntervalSet.cc
 *  Author       : broberts
 *  Created      : Mon May  1 11:01:33 PDT 2006
 *  Synopsis     : a class to hold non overlapping time intervals
 *
 *  $LastChangedDate: 2009-11-15 18:36:18 -0800 (Sun, 15 Nov 2009) $
 *  $LastChangedRevision: 4131 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include "TmTcIntervalSet.h"

#include <iterator>

/// find the intersection of this IntervalSet and an external Interval
tmtc::TmTcIntervalSet 
tmtc::TmTcIntervalSet::intersection(const tmtc::TmTcInterval& ivl) const {
  tmtc::TmTcIntervalSet ret;

  // find the bounds of intervals that might intersect with ivl
  const_iterator i1=setIvl.lower_bound(tmtc::TmTcInterval(ivl.getStart())),
    i2=setIvl.upper_bound(tmtc::TmTcInterval(ivl.getEnd()));

  if (i1!=setIvl.begin())
    i1--;

  if (i2!=setIvl.end())
    i2++;

  for (const_iterator i=i1; i!=i2; ++i) {
    tmtc::TmTcInterval _ivl=((*i) & ivl);

    if (_ivl)
      ret.setIvl.insert(_ivl);
    }

  return(ret);
  } 

/// find the intersection of this IntervalSet and another one
tmtc::TmTcIntervalSet 
tmtc::TmTcIntervalSet::intersection(const tmtc::TmTcIntervalSet& is) const {
  tmtc::TmTcIntervalSet ret;
  
  for (const_iterator i=begin(); i!=end(); i++) {
    tmtc::TmTcIntervalSet _r=is.intersection(*i);
    ret.setIvl.insert(_r.begin(), _r.end());
    }
  
  return(ret);
  } 

tmtc::TmTcIntervalSet tmtc::TmTcIntervalSet::negate(void) const {
  tmtc::TmTcIntervalSet ret;
  
  tmtc::TmTcInterval ivlLast;
  
  for (const_iterator i=begin(); i!=end(); i++) {
    if (ivlLast) {
      ivlLast.tEnd=(*i).tStart;
      ret.setIvl.insert(ivlLast);
      }

    ivlLast=(*i); ivlLast.tStart=(*i).tEnd;
    }
  
  return(ret);
  }

tmtc::TmTcIntervalSet tmtc::TmTcIntervalSet::negate(const TmTcTime& t1, 
						    const TmTcTime& t2) const {
  tmtc::TmTcIntervalSet ret;

  if (size()==0) {
    ret.insert(TmTcInterval(t1, t2));
    return(ret);
    }

  ret=negate();

  if (t1<front().getStart())
    ret.insert(TmTcInterval(t1, front().getStart()));

  if (t2>back().getEnd())
    ret.insert(TmTcInterval(back().getEnd(), t2));

  return(ret);
  }


tmtc::TmTcIntervalSet::const_iterator 
tmtc::TmTcIntervalSet::begin(void) const { 
  return(setIvl.begin()); 
  }

tmtc::TmTcIntervalSet::const_iterator 
tmtc::TmTcIntervalSet::end(void) const { 
  return(setIvl.end()); 
  }

size_t tmtc::TmTcIntervalSet::size(void) const { 
  return(setIvl.size()); 
  }

bool tmtc::TmTcIntervalSet::empty(void) const { 
  return(setIvl.empty()); 
  }

void tmtc::TmTcIntervalSet::erase(void) { 
  setIvl.erase(setIvl.begin(), setIvl.end()); 
  }

/// get the first interval in the interval set
tmtc::TmTcInterval tmtc::TmTcIntervalSet::front(void) const {
  return(*begin());
  }

/// get the last interval in the interval set
tmtc::TmTcInterval tmtc::TmTcIntervalSet::back(void) const {
  return(*(--end()));
  }

/// get the cumulative time of all the intervals
tmtc::TmTcDuration tmtc::TmTcIntervalSet::getCumulativeTime(void) const {
  double d=0;

  for (const_iterator i=begin(), iEnd=end(); i!=iEnd; i++) {
    d+=i->getSpan().getSeconds();
    }

  return(TmTcDuration().setSeconds(d));
  }

void tmtc::TmTcIntervalSet::insert(const TmTcInterval& ivl) {
  setIvl.insert(ivl); 
  } 

/// test if a value is spanned by any of the intervals in this set
bool tmtc::TmTcIntervalSet::operator() (const tmtc::TmTcTime& t) const {
  pair<const_iterator, const_iterator> p=
    setIvl.equal_range(tmtc::TmTcInterval(t));

  if (p.first!=begin())
    p.first--;

  for (const_iterator i=p.first; i!=p.second; i++) {
    if ((*i)(t))
      return(true);
    }
  
  return(false);
  }


tmtc::TmTcIntervalSet tmtc::operator & (const tmtc::TmTcIntervalSet& is,
					const tmtc::TmTcInterval& ivl) {
  return(is.intersection(ivl));
  }

tmtc::TmTcIntervalSet tmtc::operator & (const tmtc::TmTcInterval& ivl,
					const tmtc::TmTcIntervalSet& is) {
  return(is.intersection(ivl));
  }

tmtc::TmTcIntervalSet tmtc::operator & (const tmtc::TmTcIntervalSet& is1,
					const tmtc::TmTcIntervalSet& is2) {
  return(is1.intersection(is2));
  }

tmtc::TmTcIntervalSet tmtc::operator ! (const tmtc::TmTcIntervalSet& is) {
  return(is.negate());
  }

ostream& tmtc::operator << (ostream& os, const tmtc::TmTcIntervalSet& is) {
  copy(is.begin(), is.end(), ostream_iterator<tmtc::TmTcInterval>(os, "\n"));
  return(os);
  }

