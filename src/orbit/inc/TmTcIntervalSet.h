 /* H+
 *
 *  Title        : TmTcIntervalSet.h
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

#ifndef TmTcIntervalSet_H
#define TmTcIntervalSet_H

#include <TmTcInterval.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>


namespace tmtc {

class TmTcIntervalSet {
private:
  typedef set<TmTcInterval> __SI; __SI setIvl;

  /// serialization template function to write out instances of this class
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    (void)version;
    ar & setIvl;
    }

  /// find the intersection of this IntervalSet and an external Interval
  TmTcIntervalSet intersection(const TmTcInterval& ivl) const;

  /// find the intersection of this IntervalSet and another one
  TmTcIntervalSet intersection(const TmTcIntervalSet& is) const;

  TmTcIntervalSet negate(void) const;

public:
  TmTcIntervalSet negate(const TmTcTime& t1, 
			 const TmTcTime& t2) const;
  
  typedef set<TmTcInterval>::const_iterator const_iterator;

  const_iterator begin(void) const;

  const_iterator end(void) const;

  size_t size(void) const;

  bool empty(void) const;

  void erase(void);

  void insert(const TmTcInterval& ivl);

  /// get the first interval in the interval set
  TmTcInterval front(void) const;

  /// get the last interval in the interval set
  TmTcInterval back(void) const;

  /// get the cumulative time of all the intervals
  TmTcDuration getCumulativeTime(void) const;

  /// test if a value is spanned by any of the intervals in this set
  bool operator() (const TmTcTime& t) const;

  friend TmTcIntervalSet operator & (const TmTcIntervalSet&, 
				     const TmTcInterval&);

  friend TmTcIntervalSet operator & (const TmTcInterval&, 
				     const TmTcIntervalSet&);

  friend TmTcIntervalSet operator & (const TmTcIntervalSet&, 
				     const TmTcIntervalSet&);

  friend TmTcIntervalSet operator ! (const TmTcIntervalSet&);

  friend ostream& operator << (ostream&, const TmTcIntervalSet&);
  };

TmTcIntervalSet operator & (const TmTcIntervalSet& is,
			    const TmTcInterval& ivl);

TmTcIntervalSet operator & (const TmTcInterval& ivl,
			    const TmTcIntervalSet& is);

TmTcIntervalSet operator & (const TmTcIntervalSet& is1,
			    const TmTcIntervalSet& is2);

TmTcIntervalSet operator ! (const TmTcIntervalSet& is);

ostream& operator << (ostream& os, const TmTcIntervalSet& is);

}

#endif

