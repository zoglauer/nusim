/* H+
 *
 *      Title:     TmTcDuration.h
 *      Author:    Bryce A. Roberts
 *      Date:      Sun Jul 24 14:30:48 PDT 2005
 *      Synopsis:  class for storing durations 
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#ifndef _TmTcDuration_H
#define _TmTcDuration_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>

#include <TmTcUtil.h>

namespace tmtc {

// forward declaration to make friend declarations work
class TmTcTime;

class TmTcDuration {
private:
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    (void)version;
    ar & sgn;
    ar & d;
    ar & s;
    }

  /// the sign of the duration
  int sgn;

  /// integer days of duration
  tmtc_ushort d;

  /// seconds of duration
  tmtc_double s;

 public:
  //////////////////////////////////////////////////////////////////////
  // Constructors
  //////////////////////////////////////////////////////////////////////
  TmTcDuration(void);
   
  //////////////////////////////////////////////////////////////////////
  // Mutators
  //////////////////////////////////////////////////////////////////////
  TmTcDuration& setDays(double _d);
  TmTcDuration& setHours(double _h);
  TmTcDuration& setMinutes(double _m);
  TmTcDuration& setSeconds(double _s);
  TmTcDuration& setHms(const string& str);

  //////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////
  double getDays(void) const;
  double getHours(void) const;
  double getMinutes(void) const;
  double getSeconds(void) const;
  string getHms(void) const;

  //////////////////////////////////////////////////////////////////////
  // Friends
  //////////////////////////////////////////////////////////////////////
  friend class TmTcTime;

  friend bool operator < (const TmTcDuration& one, const TmTcDuration& two);
  friend bool operator > (const TmTcDuration& one, const TmTcDuration& two);
  friend bool operator == (const TmTcDuration& one, const TmTcDuration& two);
  friend bool operator <= (const TmTcDuration& one, const TmTcDuration& two);
  friend bool operator >= (const TmTcDuration& one, const TmTcDuration& two);
  };

TmTcDuration operator + (const TmTcDuration& t1, const TmTcDuration& t2);
TmTcDuration operator - (const TmTcDuration& t1, const TmTcDuration& t2);

bool operator < (const TmTcDuration& one, const TmTcDuration& two);
bool operator > (const TmTcDuration& one, const TmTcDuration& two);
bool operator == (const TmTcDuration& one, const TmTcDuration& two);
bool operator != (const TmTcDuration& one, const TmTcDuration& two);
bool operator <= (const TmTcDuration& one, const TmTcDuration& two);
bool operator >= (const TmTcDuration& one, const TmTcDuration& two);

}

#endif
