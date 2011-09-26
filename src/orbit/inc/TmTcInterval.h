
 /* H+
 *
 *  Title        : TmTcInterval.h
 *  Author       : broberts
 *  Created      : Mon May  1 10:13:04 PDT 2006
 *  Synopsis     : time interval
 *
 *  $LastChangedDate: 2006-09-28 08:33:59 -0700 (Thu, 28 Sep 2006) $
 *  $LastChangedRevision: 1415 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#ifndef _TmTcInterval_H
#define _TmTcInterval_H


#include <TmTcTime.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>

namespace tmtc {

class TmTcIntervalSet;

class TmTcInterval {
private:
  /// start and end times of the interval
  TmTcTime tStart, tEnd;

  /// serialization template function to write out instances of this class
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    (void)version;
    ar & tStart & tEnd;
    }

  /// test for intersection
  bool isIntersection(const TmTcInterval& ivl) const;
    
  /// find the intersection of this interval and another one
  TmTcInterval intersection(const TmTcInterval& ivl) const;

  /// find the union of this interval and another one
  TmTcInterval join(const TmTcInterval& ivl) const;

  /// is this interval less than ivl? (ends before ivl starts)
  bool isLessThan(const TmTcInterval& ivl) const;

  /// is this interval greater than ivl? (starts after ivl ends)
  bool isGreaterThan(const TmTcInterval& ivl) const;

public:
  /// Constructor
  TmTcInterval(void);

  /// Constructor - set both times equal
  TmTcInterval(const TmTcTime& t);

  /// two-arg constructor
  TmTcInterval(const TmTcTime& _tStart,
	       const TmTcTime& _tEnd);
  
  /// get interval start time
  TmTcTime getStart(void) const;
  
  /// get interval end time
  TmTcTime getEnd(void) const;

  /// test if time t is included in this interval
  bool operator () (const TmTcTime& t) const;
  
  /// test if the interval is defined
  operator bool() const;

  /// get the duration spanned by this interval
  TmTcDuration getSpan(void) const;

  /// friends
  friend bool operator && (const TmTcInterval&, const TmTcInterval&);
  friend bool operator < (const TmTcInterval&, const TmTcInterval&);
  friend bool operator > (const TmTcInterval&, const TmTcInterval&);
  friend TmTcInterval operator & (const TmTcInterval&, const TmTcInterval&);
  friend TmTcInterval& operator &= (TmTcInterval&, const TmTcInterval&);
  friend TmTcInterval operator | (const TmTcInterval&, const TmTcInterval&);
  friend TmTcInterval& operator |= (TmTcInterval&, const TmTcInterval&);

  friend ostream& operator << (ostream&, const TmTcInterval&);

  friend class TmTcIntervalSet;
  };

bool operator && (const TmTcInterval& one, const TmTcInterval& two);

bool operator < (const TmTcInterval& one, const TmTcInterval& two);

bool operator > (const TmTcInterval& one, const TmTcInterval& two);

TmTcInterval operator & (const TmTcInterval& one, const TmTcInterval& two);

TmTcInterval& operator &= (TmTcInterval& one, const TmTcInterval& two);

TmTcInterval operator | (const TmTcInterval& one, const TmTcInterval& two);

TmTcInterval& operator |= (TmTcInterval& one, const TmTcInterval& two);

ostream& operator << (ostream& os, const TmTcInterval& ext);
}

#endif
