/* H+
 *
 *      Title:     TmTcTime.cc
 *      Author:    Bryce A. Roberts
 *      Date:      Sun Jul 24 14:39:22 PDT 2005
 *      Synopsis:  parses t/m
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#include "TmTcTime.h"

using namespace tmtc;

#include <sys/time.h>
#include <boost/lexical_cast.hpp>

using namespace boost;

/// is a given character a traditional time separation character?
bool TmTcTime::isSeparator(char c) {
  return(c==':' || c=='/' || c=='-' || c=='T');
  }

// test if the time in this instance been defined
bool TmTcTime::isDefined(void) const { return(spd!=-1); }

TmTcTime TmTcTime::add(const TmTcDuration& d) const {
  TmTcTime _t;

  if (d.sgn) {
    _t.dpz=dpz+d.d; _t.spd=spd+d.s;
    }
  else {
    _t.dpz=dpz-d.d; _t.spd=spd-d.s;
    }

  if (_t.spd>=86400) {
    _t.spd-=86400; _t.dpz++;
    }
  else if (_t.spd<0) {
    _t.spd+=86400; _t.dpz--;
    }

  return(_t);
  }

TmTcTime TmTcTime::subtract(const TmTcDuration& d) const {
  TmTcTime _t;

  if (!d.sgn) {
    _t.dpz=dpz+d.d; _t.spd=spd+d.s;
    }
  else {
    _t.dpz=dpz-d.d; _t.spd=spd-d.s;  
    }

  if (_t.spd>=86400) {
    _t.spd-=86400; _t.dpz++;
    }
  else if (_t.spd<0) {
    _t.spd+=86400; _t.dpz--;
    }

  return(_t);
  }

TmTcDuration TmTcTime::subtract(const TmTcTime& t) const {
  TmTcDuration d;

  d.setSeconds((dpz-t.dpz)*86400.0 + (spd-t.spd));

  return(d);
  }

// protected constructor -- only for internal use!
TmTcTime::TmTcTime(tmtc_ushort dpz, tmtc_double spd) :
  toStreamPtr(&TmTcTime::toStreamEngineering) {
  TmTcTime::dpz=dpz; TmTcTime::spd=spd;

  if (spd>=86400) {
    TmTcTime::dpz++;
    TmTcTime::spd-=86400;
    }

  }

//////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////
TmTcTime::TmTcTime(void) : toStreamPtr(&TmTcTime::toStreamEngineering) { 
  dpz=0; spd=-1.0; 
  }

//////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////
// is the given year a leap year
bool TmTcTime::isLeap(int y) { 
  return(((y%4==0 && y%100!=0) || y%400==0)); 
  }

// find the month/day for the given year/day-of-year
void TmTcTime::doy2cal(int year, int doy, int& month, int& day) {
  // number of days in leap and non-leap year months
  const static int daytab[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
				  {0,31,29,31,30,31,30,31,31,30,31,30,31}};
  int leap=isLeap(year) ? 1 : 0;
  day=doy;
  for (month=1; day>daytab[leap][month]; month++)
    day-=daytab[leap][month];
  }

// find the doy of a given month/day
void TmTcTime::cal2doy(int year, int month, int day, int& doy) {
  // number of days in leap and non-leap year months
  const static int daytab[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
				  {0,31,29,31,30,31,30,31,31,30,31,30,31}};
  int leap=isLeap(year) ? 1 : 0;
  
  doy=0;
  for (int m=0; m<month; m++) 
    doy+=daytab[leap][m];
    
  doy+=day;
  }

//////////////////////////////////////////////////////////////////////
// Mutators
//////////////////////////////////////////////////////////////////////
void TmTcTime::write(ostream& os) const {
  (void)os.write((const char *)(&dpz), sizeof(dpz));
  (void)os.write((const char *)(&spd), sizeof(spd));
  }

// mark this time as undefined/invalid
TmTcTime& TmTcTime::setInvalid(void) { 
  dpz=0; spd=-1.0; return(*this); 
  }

// round up to the nearest integral second (only affects TIME, not date)
TmTcTime TmTcTime::ceil(double sec) const {
  if (sec<0.001)
    return(*this);
  else
    return(TmTcTime(dpz, tmtc_float(::ceil(spd/sec)*sec)));
  }

// round down to the nearest integral second (only affects TIME, not date)
TmTcTime TmTcTime::floor(double sec) const {
  if (sec<0.001)
    return(*this);
  else
    return(TmTcTime(dpz, tmtc_float(::floor(spd/sec)*sec)));
  }

// set the time to be the current system time
// NOTE: this member function is platform dependent, and only works
//   well for POSIX
TmTcTime& TmTcTime::setCurrent(void) {
  struct timeval tv;
  (void)gettimeofday(&tv, NULL);
  return(setUnix(double(tv.tv_sec)+double(tv.tv_usec)/1.0E6));
  }

/// set the time assuming THEMIS epoch (zero point: 1 Jan 2001)
TmTcTime& TmTcTime::setThemis(double t) {
  return(setUnix(t+978307200));
  }

// set the time with a UNIX-format time
TmTcTime& TmTcTime::setUnix(double t) {
  dpz=25567+tmtc_ushort(::floor(t/86400.0));
  spd=tmtc_double(t-86400*::floor(t/86400.0));
  return(*this);
  }

// set the time with a CCSDS-format time (zero point: 24 May 1968)
TmTcTime& TmTcTime::setCcsds(tmtc_time42 t) {
  dpz=24980+tmtc_ushort(::floor(t/86400.0));
  spd=t-86400*::floor(t/86400.0);
  return(*this);
  }

// set the time with PB5-format time arguments
TmTcTime& TmTcTime::setPb5(tmtc_ushort tjd, tmtc_ulong sec, tmtc_ushort msec) {
  dpz=34980+tjd;
  spd=sec+tmtc_double(msec)/1000.0;

  return(*this);
  }

TmTcTime& TmTcTime::setCcsds(tmtc_time40 t) {
  dpz=24980+tmtc_ushort(::floor(t/86400.0));
  spd=t-86400*::floor(t/86400.0);
  return(*this);
  }

// set the time with a traditional JD
TmTcTime& TmTcTime::setJd(double jd) {
  // DPZ = JD - 1 Jan 1900: integer days since 00h 1 Jan 1900
  dpz=tmtc_ushort(::floor(jd-2415020.5));

  // seconds past 00h
  spd=tmtc::tmtc_double(((jd-2415020.5)-double(dpz))*86400.0);

  return(*this);
  }

// set the time with a modified julian date
TmTcTime& TmTcTime::setMjd(double mjd) { return(setJd(mjd+2400000.5)); }

// set date with Gregorian calendar args, using month and day
TmTcTime& TmTcTime::setGregorian(int y, int m, int d, 
				 int hr, int mn, double sc) {
  if (y<1900 || y>2100 || m<1 || m>12 || d<1 || d>31 ||
      hr<0 || hr>23 || mn<0 || mn>59 || sc<0 || sc>61.0) {
    GUException e("TmTcTime::setGregorian()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "gregorian calendar arguments are out of range");
    throw(e);
    }

  int m1=(m-14)/12, y1=y+4800;

  dpz=(1461*(y1+m1)/4+367*(m-2-12*m1)/12-(3*((y1+m1+100)/100))/4+
       d-32075)-2415021;
    
  spd=tmtc_double(hr)*3600.0+tmtc_double(mn)*60.0+sc;

  return(*this);
  }

// set date with Gregorian calendar args, using day of year
TmTcTime& TmTcTime::setGregorian(int y, int doy, int hr, int mn, double sc) {
  if (doy<1 || doy>366) {
    GUException e("TmTcTime::setGregorian()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "gregorian calendar arguments are out of range");
    throw(e);
    }

  // convert day-of-year to month and day
  int m, d; doy2cal(y, doy, m, d);

  // set date with traditional YMD arguments
  return(setGregorian(y, m, d, hr, mn, sc));
  }
  
// set time from "exact engineering format", i.e. YYYY:DOY:HH:MM:SS.SSS
TmTcTime& TmTcTime::setExactEngineering(const string& str) {
  // check for proper length
  if (str.length()<19) {
    GUException e("TmTcTime::setExactEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
    
  // check for proper delimiters
  if (!isSeparator(str[4]) || 
      !isSeparator(str[8]) || 
      !isSeparator(str[11]) || 
      !isSeparator(str[14]) ||
      str[17]!='.') {
    GUException e("TmTcTime::setExactEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  
  // parse data
  tmtc_ushort y, d, h, m; tmtc_double s;
  try {
    y=lexical_cast<tmtc_ushort>(str.substr(0, 4));
    d=lexical_cast<tmtc_ushort>(str.substr(5, 3));
    h=lexical_cast<tmtc_ushort>(str.substr(9, 2));
    m=lexical_cast<tmtc_ushort>(str.substr(12, 2));
    s=lexical_cast<tmtc_double>(str.substr(15));

    // convert year/day-of-year info to internal format
    return(setGregorian(y, d, h, m, s));
    }
  catch (...) {
    GUException e("TmTcTime::setExactEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  }

// set time from "gregorian format", i.e. YYYY:MM:DD:HH:MM:SS.SSS
TmTcTime& TmTcTime::setGregorian(const string& str) {
  // check for proper length
  if (str.length()<19) {
    GUException e("TmTcTime::setGregorian()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
    
  // check for proper delimiters
  if (!isSeparator(str[4]) || 
      !isSeparator(str[7]) || 
      !isSeparator(str[10]) || 
      !isSeparator(str[13]) ||
      !isSeparator(str[16])) {
    GUException e("TmTcTime::setGregorian()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  
  // parse data
  tmtc_ushort y, mon, d, h, m; tmtc_double s;
  try {
    y=lexical_cast<tmtc_ushort>(str.substr(0, 4));
    mon=lexical_cast<tmtc_ushort>(str.substr(5, 2));
    d=lexical_cast<tmtc_ushort>(str.substr(8, 2));
    h=lexical_cast<tmtc_ushort>(str.substr(11, 2));
    m=lexical_cast<tmtc_ushort>(str.substr(14, 2));
    s=lexical_cast<tmtc_double>(str.substr(17));

    // convert year/day-of-year info to internal format
    return(setGregorian(y, mon, d, h, m, s));
    }
  catch (...) {
    GUException e("TmTcTime::setGregorian()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  }


// set time from "engineering format", i.e. YYYY:DOY:HH:MM:SS
TmTcTime& TmTcTime::setEngineering(const string& str) {
  // check for proper length
  if (str.length()!=17) {
    GUException e("TmTcTime::setEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
    
  // check for proper delimiters
  if (!isSeparator(str[4]) || 
      !isSeparator(str[8]) || 
      !isSeparator(str[11]) || 
      !isSeparator(str[14])) {
    GUException e("TmTcTime::setEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  
  // parse data
  tmtc_ushort y, d, h, m; tmtc_double s;
  try {
    y=lexical_cast<tmtc_ushort>(str.substr(0, 4));
    d=lexical_cast<tmtc_ushort>(str.substr(5, 3));
    h=lexical_cast<tmtc_ushort>(str.substr(9, 2));
    m=lexical_cast<tmtc_ushort>(str.substr(12, 2));
    s=lexical_cast<tmtc_double>(str.substr(15, 2));

    // convert year/day-of-year info to internal format
    return(setGregorian(y, d, h, m, s));
    }
  catch (...) {
    GUException e("TmTcTime::setEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  }

/// set the time from short engineering format, ie YYYY:DOY or YYYY/DOY
TmTcTime& TmTcTime::setShortEngineering(const string& str) {
  // check for proper length
  if (str.length()!=8) {
    GUException e("TmTcTime::setShortEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
    
  // check for proper delimiters
  if (!(str[4]!=':' || str[4]!='/')) {
    GUException e("TmTcTime::setShortEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  
  // parse data
  tmtc_ushort y, d;
  try {
    y=lexical_cast<tmtc_ushort>(str.substr(0, 4));
    d=lexical_cast<tmtc_ushort>(str.substr(5, 3));

    // convert year/day-of-year info to internal format
    return(setGregorian(y, d, 0, 0, 0.0));
    }
  catch (...) {
    GUException e("TmTcTime::setShortEngineering()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "time string '"+str+"' is invalid");
    throw(e);
    }
  }

TmTcTime& TmTcTime::setItos(const string& str) {
  try {
    if (str.size()<15)  {
      throw(GUException("TmTcTime::setItos()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"time string size is invalid"));
      }
  
    if (str[2]!='-' || str[6]!='-' || str[9]!=':' || str[12]!=':') {
      throw(GUException("TmTcTime::setItos()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"time string format is invalid"));
      }

    // parse data
    tmtc_ushort year, doy, hour, min; double sec;
    try {
      year=lexical_cast<tmtc_ushort>(str.substr(0, 2))+2000;
      doy=lexical_cast<tmtc_ushort>(str.substr(3, 3));
      hour=lexical_cast<tmtc_ushort>(str.substr(7, 2));
      min=lexical_cast<tmtc_ushort>(str.substr(10, 2));
      sec=lexical_cast<tmtc_double>(str.substr(13, 2));
      }
    catch (...) {
      GU_THROW("TmTcTime::setItos()",
	       "time argument(s) are invalid", str);
      }
      
    // convert year/day-of-year info to internal format
    (void)setGregorian(year, doy, hour, min, sec);
    }
  catch (GUException e) {
    e.add(GUException("TmTcTime::setItos()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "failed to set ITOS-format time with string '"+str+"'"));
    throw(e);
    }

  return(*this);
  }

// set "SatTrack" time format
TmTcTime& TmTcTime::setSatTrack(const string& str) {
  try {
    // split into date and time substrings
    GUStringvec sv; sv.setInputTokenSeparator(" "); str >> sv;
    if (sv.size()!=2) 
      throw(GUException("TmTcTime::setSatTrack()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"date/time string is invalid"));

    // parse time
    GUStringvec sv3; sv3.setInputTokenSeparator(":"); sv[1] >> sv3;
    if (sv3.size()!=3)
      throw(GUException("TmTcTime::setSatTrack()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"time substring is invalid"));

    // parse date
    GUStringvec sv2; sv2.setInputTokenSeparator("/"); sv[0] >> sv2;
    if (sv2.size()==2) { // YYYY/DOY or DOY/YYYY
      if (sv2[0].size()==4)
	return(setGregorian(lexical_cast<tmtc_ushort>(sv2[0]),
			    lexical_cast<tmtc_ushort>(sv2[1]),
			    lexical_cast<tmtc_ushort>(sv3[0]), 
			    lexical_cast<tmtc_ushort>(sv3[1]), 
			    lexical_cast<tmtc_double>(sv3[2])));
      else
	return(setGregorian(lexical_cast<tmtc_ushort>(sv2[1]),
			    lexical_cast<tmtc_ushort>(sv2[0]),
			    lexical_cast<tmtc_ushort>(sv3[0]), 
			    lexical_cast<tmtc_ushort>(sv3[1]), 
			    lexical_cast<tmtc_double>(sv3[2])));      
    }
    else if (sv2.size()==3) { // YYYY/MM/DD
      return(setGregorian(lexical_cast<tmtc_ushort>(sv2[0]),
			  lexical_cast<tmtc_ushort>(sv2[1]),
			  lexical_cast<tmtc_ushort>(sv2[2]),
			  lexical_cast<tmtc_ushort>(sv3[0]), 
			  lexical_cast<tmtc_ushort>(sv3[1]), 
			  lexical_cast<tmtc_double>(sv3[2])));
      }
    else {
      GUStringvec sv2_2; sv2_2.setInputTokenSeparator("-"); sv[0] >> sv2_2;    
      if (sv2_2.size()==3) {
	return(setGregorian(lexical_cast<tmtc_ushort>(sv2_2[2]),
			    lexical_cast<tmtc_ushort>(sv2_2[1]),
			    lexical_cast<tmtc_ushort>(sv2_2[0]),
			    lexical_cast<tmtc_ushort>(sv3[0]), 
			    lexical_cast<tmtc_ushort>(sv3[1]), 
			    lexical_cast<tmtc_double>(sv3[2])));
        }
      else
	throw(GUException("TmTcTime::setSatTrack()",
			  GUException::ERROR,
			  __FILE__, __LINE__,
			  "date string size is invalid"));
      }
    }
  catch (...) {
    GU_THROW("TmTcTime::setSatTrack()",
	     "SatTrack formated date/time is invalid", str);
    }
  }

/// set "GMAN" time format
TmTcTime& TmTcTime::setGman(const string& str) {
  GUStringvec sv1; sv1.setInputTokenSeparator(" ");
  str >> sv1;

  // make sure there's a date and time field
  if (sv1.size()!=2) 
    throw(GUException("TmTcTime::setGman()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "time '"+str+"' is invalid"));

  /// date field
  int y, mon, d;
  {
    GUStringvec sv2; sv2.setInputTokenSeparator("-"); sv1[0] >> sv2;
    if (sv2.size()!=3) {
      throw(GUException("TmTcTime::setGman()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"date field '"+sv1[0]+"' is invalid"));
      }
    try {
      y=boost::lexical_cast<int>(sv2[0]);
      mon=getMonthNumberFromString(sv2[1]);
      d=boost::lexical_cast<int>(sv2[2]);
      }
    catch (...) {
      throw(GUException("TmTcTime::setGman()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"date field '"+sv1[0]+"' is invalid"));
      }
    }

  tmtc_ushort h, m; tmtc_double s;
  {
    GUStringvec sv2; sv2.setInputTokenSeparator(":"); sv1[1] >> sv2;
    if (sv2.size()!=3) {
      throw(GUException("TmTcTime::setGman()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"time field '"+sv1[1]+"' is invalid"));
      }
    try {
      h=lexical_cast<tmtc_ushort>(sv2[0]);
      m=lexical_cast<tmtc_ushort>(sv2[1]);
      s=lexical_cast<tmtc_double>(sv2[2]);
      }
    catch (...) {
      throw(GUException("TmTcTime::setGman()",
			GUException::ERROR,
			__FILE__, __LINE__,
			"time field '"+sv1[1]+"' is invalid"));
      }
    }    

  return(setGregorian(y, mon, d, h, m, s));
  }

/// set time with autodetected format
TmTcTime& TmTcTime::twister(const string& str) {
  if (stolower(str)=="now") {
    setCurrent();
    return(*this);
    }
  try {
    if (setExactEngineering(str))
      return(*this);
    }
  catch (...) { ; }
  try {
    if (setEngineering(str))
      return(*this);
    }
  catch (...) { ; }
  try {
    if (setGregorian(str))
      return(*this);
    }
  catch (...) { ; }
  try {
    if (setShortEngineering(str))
      return(*this);
    }
  catch (...) { ; }
  try {
    if (setItos(str)) 
      return(*this);
    }
  catch (...) { ; }
  try {
    if (setSatTrack(str)) 
      return(*this);
    }
  catch (...) { ; }

  // if we got this far, format wasn't recognized
  GUException e("TmTcTime::twister()",
		GUException::ERROR,
		__FILE__, __LINE__,
		"couldn't recognize timeformat '"+str+"'");
  throw(e);
  }

//////////////////////////////////////////////////////////////////////
// Accessors
//////////////////////////////////////////////////////////////////////
bool TmTcTime::read(istream& is) {
  // read date
  is.read((char *)(&dpz), sizeof(dpz));
  if (is.gcount()!=sizeof(dpz))
    return(false);

  // read time
  is.read((char *)(&spd), sizeof(spd));
  if (is.gcount()!=sizeof(spd))
    return(false);
  return(true);
  }

// get UNIX time format
double TmTcTime::getUnix(void) const { 
  return((dpz-25567)*86400.0 + spd); 
  }

/// get THEMIS time (seconds since 1 jan 2001)
double TmTcTime::getThemis(void) const {
  return(getUnix()-978307200);
  }


// get CCSDS time format
double TmTcTime::getCcsds(void) const { 
  return((dpz-24980)*86400.0 + spd);
  }

/// get the time in PB5-format time arguments
void TmTcTime::getPb5(tmtc_ushort& tjd, 
		      tmtc_ulong& sec, 
		      tmtc_ushort& msec) const {
  tjd=dpz-34980;
  sec=tmtc_ulong(::floor(spd));
  msec=tmtc_ushort(::floor((spd-::floor(spd))*1000.0));
  }

// get Julian date
double TmTcTime::getJd(void) const { 
  return(double(dpz)+2415020.5+(spd/86400.0)); 
  }

// get modified Julian date
double TmTcTime::getMjd(void) const { return(getJd()-2400000.5); }

/// get "YMD" Gregorian calendar arguments
string TmTcTime::getGregorian(void) const {
  int y, mon, d, h, m; double s;
  char buffer[MAXSTRING];
  getGregorian(y, mon, d, h, m, s);
  snprintf(buffer, 30, "%04d:%02d:%02d:%02d:%02d:%06.3f", y, mon, d, h, m, s);

  return(buffer);
  }

// get "YMD" Gregorian calendar arguments
void TmTcTime::getGregorian(int& year, int& month, int& day, 
			    int& hr, int& mn, double& sc) const {
  int p=int(dpz)+2415021+68569, q=4*p/146097, r=p-(146097*q+3)/4,
    s=4000*(r+1)/1461001, t=r-1461*s/4+31, u=80*t/2447, v=u/11;
  year=100*(q-49)+s+v; month=u+2-12*v; day=t-2447*u/80;

  // convert milliseconds past 00h into hr, mn, sc
  tmtc_double _spd=spd;
  hr=int(::floor(_spd/3600.0)); _spd-=hr*3600;
  mn=int(::floor(_spd/60.0)); _spd-=mn*60;
  sc=_spd;
  }

// get "Year/day-of-year" Gregorian calendar arguments
void TmTcTime::getGregorian(int& year, int& doy,
			    int& hr, int& mn, double& sc) const {
  int month, day; getGregorian(year, month, day, hr, mn, sc);
  cal2doy(year, month, day, doy);
  }

// get the time in an "engineering" format string, e.g. 2005:001:02:03:04
string TmTcTime::getEngineering(void) const {
  char buffer[20];
  
  if (!isDefined()) {
    snprintf(buffer, 20, "XXXX:XXX:XX:XX:XX");
    }
  else {
    int y, doy, h, m; double s;
    getGregorian(y, doy, h, m, s);
    snprintf(buffer, 20, "%04d:%03d:%02d:%02d:%02d", y, doy, h, m, 
	     int(::floor(s)));
    }

  return(buffer);
  }

// get the time in an "exact engineering" format string, 
// e.g. 2005:001:02:03:04.123
string TmTcTime::getExactEngineering(void) const {
  char buffer[24];
  
  if (!isDefined()) {
    snprintf(buffer, 24, "XXXX:XXX:XX:XX:XX.XXX");
    }
  else {
    int y, doy, h, m; double s;
    getGregorian(y, doy, h, m, s);
    snprintf(buffer, 24, "%04d:%03d:%02d:%02d:%06.3f", y, doy, h, m, s);
    }

  return(buffer);
  }
  
// get condensed format, which leaves out the year, e.g. "101/12:13:14"
string TmTcTime::getCondensed(void) const {
  int y, doy, h, m; double s;
  getGregorian(y, doy, h, m, s);
  char buffer[20];
  if (!isDefined())
    snprintf(buffer, 20, "XXX/XX:XX:XX");
  else
    snprintf(buffer, 20, "%03d/%02d:%02d:%02d", doy, h, m, 
	     int(::floor(s+0.5)));

  return(buffer);
  }

/// get "SatTrack" formnat, e.g. "2006/001 00:00:00.000"
string TmTcTime::getSatTrack(void) const {
  char buffer[24];
  
  if (!isDefined()) {
    snprintf(buffer, 24, "XXXX/XXX XX:XX:XX.XXX");
    }
  else {
    int y, doy, h, m; double s;
    getGregorian(y, doy, h, m, s);
    snprintf(buffer, 24, "%04d/%03d %02d:%02d:%06.3f", y, doy, h, m, s);
    }  

  return(buffer);
  }

/// get short "SatTrack" format, e.g. "2006/001 00:00:00"
string TmTcTime::getShortSatTrack(void) const {
  char buffer[21];
  
  if (!isDefined()) {
    snprintf(buffer, 21, "XXXX/XXX XX:XX:XX");
    }
  else {
    int y, doy, h, m; double s;
    getGregorian(y, doy, h, m, s);
    snprintf(buffer, 21, "%04d/%03d %02d:%02d:%02d", 
	     y, doy, h, m, int(::floor(s)));
    }  

  return(buffer);
  }

/// get time in filename format, eg "2006_001_000000"
string TmTcTime::getFilenameFormat(void) const {
  stringstream ss;

  int y, doy, h, m; double s;
  getGregorian(y, doy, h, m, s);
  ss << y << "_" << setw(3) << setfill('0') << doy << "_"
     << setw(2) << setfill('0') << h 
     << setw(2) << setfill('0') << m
     << setw(2) << setfill('0') << int(s);
  
  return(ss.str());
  }

/// get mail format
string TmTcTime::getMailFormat(void) const {
  stringstream ss;

  ss << getDayOfWeekString() << ", ";

  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);

  ss << fixed << setw(2) << setfill('0') << d 
     << " " << getMonthString().substr(0, 3) << " " << y << " ";
  ss << fixed << setw(2) << setfill('0') << hr << ":";
  ss << fixed << setw(2) << setfill('0') << mn << ":";
  ss << fixed << setw(2) << setfill('0') << int(::floor(sc));

  return(ss.str());
  }

// get just the year of the date
unsigned TmTcTime::getYear(void) const {
  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);
  return(y);
  }

// get just the month of the date
unsigned TmTcTime::getMonth(void) const {
  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);
  return(m);
  }

/// get the month name (in English only... desolet)
string TmTcTime::getMonthString(void) const {
  switch (getMonth()) {
  case 1:   return("January");
  case 2:   return("February");
  case 3:   return("March");
  case 4:   return("April");
  case 5:   return("May");
  case 6:   return("June");
  case 7:   return("July");
  case 8:   return("August");
  case 9:   return("September");
  case 10:  return("October");
  case 11:  return("November");
  case 12:  return("December");
  }

  return("");
  }

// get just the day of the date
unsigned TmTcTime::getDay(void) const {
  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);
  return(d);
  }

// get just the day-of-year of the date
unsigned TmTcTime::getDoy(void) const {
  int y, doy, hr, mn; double sc;
  getGregorian(y, doy, hr, mn, sc);
  return(doy);
  }

// get just the hour of the time
unsigned TmTcTime::getHour(void) const {
  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);
  return(hr);
  }

// get just the hour of the time
unsigned TmTcTime::getMinute(void) const {
  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);
  return(mn);
  }

// get just the hour of the time
double TmTcTime::getSecond(void) const {
  int y, m, d, hr, mn; double sc;
  getGregorian(y, m, d, hr, mn, sc);
  return(sc);
  }

/// get an hh:mm:ss string
string TmTcTime::getHms(void) const {
  stringstream ss;
  ss << setw(2) << setfill('0') << getHour() << ":" 
     << setw(2) << getMinute() << ":" << setw(2) << int(::floor(getSecond()));

  return(ss.str());
  }

/// get an hh:mm:ss.sss string
string TmTcTime::getExactHms(void) const {
  stringstream ss;
  ss << setw(2) << setfill('0') << getHour() << ":" 
     << setw(2) << getMinute() << ":" 
     << setw(6) << setfill('0') << fixed << setprecision(3) << getSecond();

  return(ss.str());
  }


// return the day of the week
int TmTcTime::getDayOfWeek(void) const {
  return((dpz+1)%7);
  }

int TmTcTime::getWeekOfMonth(void) const {
  TmTcTime _t; _t.setGregorian(getYear(), getMonth(), 1, 0, 0, 0.0);

  long lmonth=_t.dpz-((_t.dpz+1)%7);

  return(int(::floor(double(dpz-lmonth)/7.0)));
  }

// return the text string for the day of week
string TmTcTime::getDayOfWeekString(void) const {
  switch (dpz%7) {
  case 0:
    return("Mon");
  case 1:
    return("Tue");
  case 2:
    return("Wed");
  case 3:
    return("Thu");
  case 4:
    return("Fri");
  case 5:
    return("Sat");
  default:
    return("Sun");
    }
  }

// get the day fraction, where 00h=0.0 and 23:59:59.99999 1.0-tiny
double TmTcTime::getDayFrac(void) const { return(spd/86400.0); }

/// get the month number from a strong
int TmTcTime::getMonthNumberFromString(const string& mon) {
  string m=depad(stolower(mon));
  
  if (m=="jan")
    return(1);
  else if (m=="feb")
    return(2);
  else if (m=="mar")
    return(3);
  else if (m=="apr")
    return(4);
  else if (m=="may")
    return(5);
  else if (m=="jun")
    return(6);
  else if (m=="jul")
    return(7);
  else if (m=="aug")
    return(8);
  else if (m=="sep")
    return(9);
  else if (m=="oct")
    return(10);
  else if (m=="nov")
    return(11);
  else if (m=="dec")
    return(12);
  else {
    GUException e("TmTcTime::getMonthNumberFromString()",
		  GUException::ERROR,
		  __FILE__, __LINE__,
		  "month string '"+mon+"' is unrecognized");
    throw(e);
    }
  }

//////////////////////////////////////////////////////////////////////
// overloaded operators
//////////////////////////////////////////////////////////////////////
// boolean test - is this instance defined?
TmTcTime::operator bool() const { return(spd!=-1); }

/// asignment operator doesn't affect format pmf
TmTcTime& TmTcTime::operator = (const TmTcTime& old) {
  dpz=old.dpz; spd=old.spd;
  return(*this);
  }

bool tmtc::operator < (const TmTcTime& one, const TmTcTime& two) {
  if (one.dpz!=two.dpz)
    return(one.dpz<two.dpz);
  else
    return(one.spd<two.spd);
  }

bool tmtc::operator > (const TmTcTime& one, const TmTcTime& two) {
  if (one.dpz!=two.dpz)
    return(one.dpz>two.dpz);
  else
    return(one.spd>two.spd);
  }

bool tmtc::operator == (const TmTcTime& one, const TmTcTime& two) {
  return(one.dpz==two.dpz && one.spd==two.spd);
  }

bool tmtc::operator != (const TmTcTime& one, const TmTcTime& two) {
  return(!(one==two));
  }

bool tmtc::operator <= (const TmTcTime& one, const TmTcTime& two) {
  return(one<two || one==two);
  }

bool tmtc::operator >= (const TmTcTime& one, const TmTcTime& two) {
  return(one>two || one==two);
  }

TmTcTime tmtc::operator + (const TmTcTime& t, const TmTcDuration& d) {
  return(t.add(d));
  }

TmTcTime tmtc::operator + (const TmTcDuration& d, const TmTcTime& t) {
  return(t.add(d));
  }

TmTcTime tmtc::operator - (const TmTcTime& t, const TmTcDuration& d) {
  return(t.subtract(d));
  }

TmTcDuration tmtc::operator - (const TmTcTime& t1, const TmTcTime& t2) {
  return(t1.subtract(t2));
  }

TmTcTime& tmtc::operator += (TmTcTime& t, const TmTcDuration& d) {
  return(t=t.add(d));
  }

TmTcTime& tmtc::operator -= (TmTcTime& t, const TmTcDuration& d) {
  return(t=t.subtract(d));
  }

ostream& tmtc::operator << (ostream& os, const TmTcTime& t) {
  t.toStream(os);
  return(os);
  }

istream& tmtc::operator >> (istream& is, TmTcTime& t) {
  t.fromStream(is);
  return(is);
  }
