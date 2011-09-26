/* H+
 *
 *      Title:     TmTcDuration.cc
 *      Author:    Bryce A. Roberts
 *      Date:      Sun Jul 24 14:30:48 PDT 2005
 *      Synopsis:  class for storing durations 
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#include "TmTcDuration.h"


using namespace tmtc;


//////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////
TmTcDuration::TmTcDuration(void) { sgn=1; d=0; s=0.0; }  

//////////////////////////////////////////////////////////////////////
// Mutators
//////////////////////////////////////////////////////////////////////
TmTcDuration& TmTcDuration::setDays(double _d) {
  // capture the sign of the number
  sgn=(_d>=0); _d=fabs(_d);

  // calculate the day portion
  d=tmtc_ushort(floor(_d)); 

  // set the seconds portion
  s=tmtc_double((_d-d)*86400.0);

  return(*this);
  }

TmTcDuration& TmTcDuration::setHours(double _h) { 
  // capture the sign of the number
  sgn=(_h>=0); _h=fabs(_h);

  // calculate the day portion
  d=tmtc_ushort(floor(_h/24.0));

  // set the seconds portion
  s=_h*3600.0-d*86400.0;

  return(*this);
  }

TmTcDuration& TmTcDuration::setMinutes(double _m) { 
  // capture the sign of the number
  sgn=(_m>=0); _m=fabs(_m);

  // calculate the day portion
  d=tmtc_ushort(floor(_m/1440.0));

  // set the seconds portion
  s=_m*60.0-d*86400.0;

  return(*this);
  }

TmTcDuration& TmTcDuration::setSeconds(double _s) { 
  // capture the sign of the number
  sgn=(_s>=0); _s=fabs(_s);

  // calculate the day portion
  d=tmtc_ushort(floor(_s/86400.0));

  // set the seconds portion
  s=_s-d*86400.0;

  return(*this);
  }

TmTcDuration& TmTcDuration::setHms(const string& str) {
  GUStringvec sv; sv.setInputTokenSeparator(":"); str >> sv;

  // must be at least three time fields (hours, minutes, seconds)
  if (sv.size()!=3) {
    GUException e("TmTcDuration::setHms()",
                  GUException::ERROR,
                  __FILE__, __LINE__,
                  "invalid duration argument '"+str+"'");
    throw(e);
    }

  // trim off leading and trailing whitespace
  sv=depad(sv);

  // time fields must be at least 2 characters wide (e.g. 00:00:00)
  if (sv[0].size()<2 || sv[1].size()<2 || sv[2].size()<2) {
    GUException e("TmTcDuration::setHms()",
                  GUException::ERROR,
                  __FILE__, __LINE__,
                  "invalid duration argument '"+str+"'");
    throw(e);
    }

  // keep track of the duration's sign
  double _sgn=1.0;
  if (sv[0][0]=='-')
    _sgn=-1.0;
  
  // convert time fields to variables
  int hh, mm; double ss;
  try {
    hh=abs(fromString<int>(sv[0]));
    mm=fromString<int>(sv[1]);
    ss=fromString<double>(sv[2]);
    }
  catch (GUException e) {
    e.add(GUException("TmTcDuration::setHms()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "invalid duration argument '"+str+"'"));
    throw(e);
    }

  // check time ranges
  if (mm<0 || mm>59 || ss<0 || ss>=60) {
    GUException e("TmTcDuration::setHms()",
                  GUException::ERROR,
                  __FILE__, __LINE__,
                  "duration argument(s) out of range");
    throw(e);
    }

  // set the duration
  return(setSeconds(_sgn*double(hh*3600.0+mm*60+ss)));
  }

//////////////////////////////////////////////////////////////////////
// Accessors
//////////////////////////////////////////////////////////////////////
double TmTcDuration::getDays(void) const { 
  double t=double(d)+double(s)/86400.0;
  if (!sgn)
    t*=-1;
  return(t);
  }
double TmTcDuration::getHours(void) const { return(getDays()*24.0); }
double TmTcDuration::getMinutes(void) const { return(getDays()*1440.0); }
double TmTcDuration::getSeconds(void) const { return(getDays()*86400.0); }

string TmTcDuration::getHms(void) const {
  stringstream ss;
  
  double _s=d*86400.0+s;

  // handle the sign 
  if (_s<0)
    ss << "-";
  _s=fabs(_s);

  ss.width(2); ss.fill('0');
  ss << int(floor(_s/3600.0)) << ":";
  _s-=floor(_s/3600.0)*3600;

  ss.width(2); ss.fill('0');
  ss << int(floor(_s/60.0)) << ":";
  _s-=floor(_s/60.0)*60;

  ss.width(2); ss.fill('0');
  ss << int(floor(_s));
  
  return(ss.str());
  }

TmTcDuration tmtc::operator + (const TmTcDuration& t1, const TmTcDuration& t2) {
  return(TmTcDuration().setSeconds(t1.getSeconds()+t2.getSeconds()));
  }

TmTcDuration tmtc::operator - (const TmTcDuration& t1, const TmTcDuration& t2) {
  return(TmTcDuration().setSeconds(t1.getSeconds()-t2.getSeconds()));
  }

bool tmtc::operator < (const TmTcDuration& one, const TmTcDuration& two) {
  if (one.d!=two.d)
    return(one.d<two.d);
  else
    return(one.s<two.s);
  }

bool tmtc::operator > (const TmTcDuration& one, const TmTcDuration& two) {
  if (one.d!=two.d)
    return(one.d>two.d);
  else
    return(one.s>two.s);
  }

bool tmtc::operator == (const TmTcDuration& one, const TmTcDuration& two) {
  return(one.d==two.d && one.s==two.s);
  }

bool tmtc::operator <= (const TmTcDuration& one, const TmTcDuration& two) {
  return(one<two || one==two);
  }

bool tmtc::operator >= (const TmTcDuration& one, const TmTcDuration& two) {
  return(one>two || one==two);
  }
