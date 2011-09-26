/* H+
 *
 *      Title:     TmTcTime.h
 *      Author:    Bryce A. Roberts
 *      Date:      Sun Jul 24 14:39:22 PDT 2005
 *      Synopsis:  time class
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#ifndef _TmTcTime_H
#define _TmTcTime_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>

#include <TmTcUtil.h>
#include <TmTcDuration.h>

namespace tmtc {

class TmTcTime {
private:
  friend class boost::serialization::access;
  
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    (void)version;
    ar & dpz;
    ar & spd;
    }

  /// integer number of Julian days past my arbitrary zero point,
  /// which I define to be 00h, 1 Jan 1900
  tmtc_ushort dpz;

  /// seconds past 00h of the day specified above in dpz
  tmtc_double spd;

  /// typedef to pointer to member function
  typedef void (TmTcTime::*pmf)(ostream&) const;
  pmf toStreamPtr;

  /// is a given character a traditional time separation character?
  static bool isSeparator(char c);

  /// test if the time in this instance been defined
  bool isDefined(void) const;

  /// internal add/subtract functions
  TmTcTime add(const TmTcDuration& d) const;
  TmTcTime subtract(const TmTcDuration& d) const;
  TmTcDuration subtract(const TmTcTime& t) const;

  /// protected constructor -- only for internal use!
  TmTcTime(tmtc_ushort dpz, tmtc_double spd);

  /// member function to write time to stream in UNIX seconds
  void toStreamUnix(ostream& os) const {
    ios_base::fmtflags old=os.flags();
    os << fixed << setprecision(4) << getUnix();
    os.flags(old);
    }

  /// member function to write time to stream as "THEMIS seconds"
  void toStreamThemis(ostream& os) const {
    ios_base::fmtflags old=os.flags();
    os << fixed << setprecision(4) << getThemis();
    os.flags(old);
    }

  /// member function to write time to stream as an engineering string
  void toStreamEngineering(ostream& os) const {
    os << getEngineering();
    }

  void toStreamYmd(ostream& os) const {
    os << getGregorian();
    }

  /// member function to write time to stream as an exact engineering string
  void toStreamExactEngineering(ostream& os) const {
    os << getExactEngineering();
    }

  /// member function to write time to stream as a condensed string
  void toStreamCondensed(ostream& os) const {
    os << getCondensed();
    }

  void toStreamSatTrack(ostream& os) const {
    os << getSatTrack();
    }

  /// write to stream -- calls registered pointer to member function
  void toStream(ostream& os) const {
    ((*this).*(toStreamPtr))(os);
    }

  void fromStream(istream& is) {
    string str; is >> str;
    twister(str);
    }

public:
  //////////////////////////////////////////////////////////////////////
  // Constructors
  //////////////////////////////////////////////////////////////////////
  TmTcTime(void);

  /// set the output format, cached as a pointer to the appropriate 
  /// member function
  void setOutputFormat(string str) {
    str=depad(stoupper(str));
    
    if (str=="UNIX")
      toStreamPtr=&TmTcTime::toStreamUnix;
    else if (str=="THEMIS")
      toStreamPtr=&TmTcTime::toStreamThemis;
    else if (str=="DOY" || str=="ENG" || str=="ENGINEERING")
      toStreamPtr=&TmTcTime::toStreamEngineering;
    else if (str=="YMD")
      toStreamPtr=&TmTcTime::toStreamYmd;
    else if (str=="XCT" || str=="EXACT")
      toStreamPtr=&TmTcTime::toStreamExactEngineering;
    else if (str=="CON" || str=="CONDENSED")
      toStreamPtr=&TmTcTime::toStreamCondensed;
    else if (str=="SATTRACK")
      toStreamPtr=&TmTcTime::toStreamSatTrack;
    else
      toStreamPtr=&TmTcTime::toStreamEngineering;
    }

  /// is the given year a leap year
  static bool isLeap(int y);

  /// find the month/day for the given year/day-of-year
  static void doy2cal(int year, int doy, int& month, int& day);

  /// find the doy of a given month/day
  static void cal2doy(int year, int month, int day, int& doy);

  //////////////////////////////////////////////////////////////////////
  // Mutators
  //////////////////////////////////////////////////////////////////////
  void write(ostream& os) const;
  
  /// mark this time as undefined/invalid
  TmTcTime& setInvalid(void);

  /// round up to the nearest integral second (only affects TIME, not date)
  TmTcTime ceil(double sec) const;

  /// round down to the nearest integral second (only affects TIME, not date)
  TmTcTime floor(double sec) const;

  /// set the time to be the current system time
  /// NOTE: this member function is platform dependent, and only works
  ///   well for POSIX
  TmTcTime& setCurrent(void);

  /// set the time with a UNIX-format time
  TmTcTime& setUnix(double t);

  /// set the time assuming THEMIS epoch (zero point: 1 Jan 2001)
  TmTcTime& setThemis(double t);

  /// set the time with a CCSDS-format time (zero point: 24 May 1968)
  TmTcTime& setCcsds(tmtc_time42 t);
  TmTcTime& setCcsds(tmtc_time40 t);

  /// set the time with PB5-format time arguments
  TmTcTime& setPb5(tmtc_ushort tjd, tmtc_ulong sec, tmtc_ushort msec);

  /// set the time with a traditional JD
  TmTcTime& setJd(double jd);

  /// set the time with a modified julian date
  TmTcTime& setMjd(double mjd);

  /// set date with Gregorian calendar args, using month and day
  TmTcTime& setGregorian(int y, int m, int d, int hr, int mn, double sc);

  // set time from "gregorian format", i.e. YYYY:MM:DD:HH:MM:SS.SSS
  TmTcTime& setGregorian(const string& str);

  /// set date with Gregorian calendar args, using day of year
  TmTcTime& setGregorian(int y, int doy, int hr, int mn, double sc);
  
  /// set time from "exact engineering format", i.e. YYYY:DOY:HH:MM:SS.SSS
  TmTcTime& setExactEngineering(const string& str);

  /// set time from "engineering format", i.e. YYYY:DOY:HH:MM:SS
  TmTcTime& setEngineering(const string& str);
  
  /// set the time from short engineering format, ie YYYY:DOY or YYYY/DOY
  TmTcTime& setShortEngineering(const string& str);

  /// set ITOS time format (e.g. 05-350-01:02:03)
  TmTcTime& setItos(const string& str);

  /// set "SatTrack" time format
  TmTcTime& setSatTrack(const string& str);

  /// set "GMAN" time format
  TmTcTime& setGman(const string& str);

  /// set time with autodetected format
  TmTcTime& twister(const string& str);

  //////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////
  bool read(istream& is);

  /// get UNIX time format
  double getUnix(void) const;

  /// get THEMIS time (seconds since 1 jan 2001)
  double getThemis(void) const;

  /// get CCSDS time format
  double getCcsds(void) const;

  /// get the time in PB5-format time arguments
  void getPb5(tmtc_ushort& tjd, tmtc_ulong& sec, tmtc_ushort& msec) const;

  /// get Julian date
  double getJd(void) const;

  /// get modified Julian date
  double getMjd(void) const;

  /// get "YMD" Gregorian calendar arguments
  string getGregorian(void) const;

  /// get "YMD" Gregorian calendar arguments
  void getGregorian(int& year, int& month, int& day, 
		    int& hr, int& mn, double& sc) const;

  /// get "Year/day-of-year" Gregorian calendar arguments
  void getGregorian(int& year, int& doy,
		    int& hr, int& mn, double& sc) const;

  /// get the time in an "engineering" format string, e.g. 2005:001:02:03:04
  string getEngineering(void) const;

  /// get the time in an "exact engineering" format string, 
  /// e.g. 2005:001:02:03:04.123
  string getExactEngineering(void) const;
  
  /// get condensed format, which leaves out the year, e.g. "101/12:13:14"
  string getCondensed(void) const;

  /// get "SatTrack" format, e.g. "2006/001 00:00:00.000"
  string getSatTrack(void) const;

  /// get short "SatTrack" format, e.g. "2006/001 00:00:00"
  string getShortSatTrack(void) const;

  /// get time in filename format, eg "2006_001_000000"
  string getFilenameFormat(void) const;

  /// get mail format
  string getMailFormat(void) const;

  /// get just the year of the date
  unsigned getYear(void) const;

  /// get just the month of the date
  unsigned getMonth(void) const;

  /// get the month name (in English only... je suis desolet)
  string getMonthString(void) const;

  /// get just the day of the date
  unsigned getDay(void) const;

  /// get just the day-of-year of the date
  unsigned getDoy(void) const;

  /// get just the hour of the time
  unsigned getHour(void) const;

  /// get just the hour of the time
  unsigned getMinute(void) const;

  /// get just the hour of the time
  double getSecond(void) const;

  /// get an hh:mm:ss string
  string getHms(void) const;

  /// get an hh:mm:ss.sss string
  string getExactHms(void) const;

  /// return the text string for the day of week
  string getDayOfWeekString(void) const;

  /// return the day of the week (0=Sunday, 6=Saturday)
  int getDayOfWeek(void) const;

  /// get the week of the month (0 is the week where the 1st of the
  /// month is placed)
  int getWeekOfMonth(void) const;

  /// get the month number from a strong
  static int getMonthNumberFromString(const string& mon);

  /// get the day fraction, where 00h=0.0 and 23:59:59.99999999
  double getDayFrac(void) const;

  //////////////////////////////////////////////////////////////////////
  // overloaded operators
  //////////////////////////////////////////////////////////////////////
  // boolean test - is this instance defined?
  operator bool() const;

  /// asignment operator doesn't affect format pmf
  TmTcTime& operator = (const TmTcTime& old);

  //////////////////////////////////////////////////////////////////////
  // Friends
  //////////////////////////////////////////////////////////////////////
  friend bool operator < (const TmTcTime& one, const TmTcTime& two);
  friend bool operator > (const TmTcTime& one, const TmTcTime& two);
  friend bool operator == (const TmTcTime& one, const TmTcTime& two);
  friend bool operator != (const TmTcTime& one, const TmTcTime& two);
  friend bool operator <= (const TmTcTime& one, const TmTcTime& two);
  friend bool operator >= (const TmTcTime& one, const TmTcTime& two);
  friend TmTcTime operator + (const TmTcTime& t, const TmTcDuration& d);
  friend TmTcTime operator + (const TmTcDuration& d, const TmTcTime& t);
  friend TmTcTime operator - (const TmTcTime& t, const TmTcDuration& d);
  friend TmTcDuration operator - (const TmTcTime& t1, const TmTcTime& t2);
  friend TmTcTime& operator += (TmTcTime& t, const TmTcDuration& d);
  friend TmTcTime& operator -= (TmTcTime& t, const TmTcDuration& d);
  friend ostream& operator << (ostream& os, const TmTcTime& t);
  friend istream& operator >> (istream& is, TmTcTime& t);
  };

bool operator < (const TmTcTime& one, const TmTcTime& two);
bool operator > (const TmTcTime& one, const TmTcTime& two);
bool operator == (const TmTcTime& one, const TmTcTime& two);
bool operator != (const TmTcTime& one, const TmTcTime& two);
bool operator <= (const TmTcTime& one, const TmTcTime& two);
bool operator >= (const TmTcTime& one, const TmTcTime& two);
TmTcTime operator + (const TmTcTime& t, const TmTcDuration& d);
TmTcTime operator + (const TmTcDuration& d, const TmTcTime& t);
TmTcTime operator - (const TmTcTime& t, const TmTcDuration& d);
TmTcDuration operator - (const TmTcTime& t1, const TmTcTime& t2);
TmTcTime& operator += (TmTcTime& t, const TmTcDuration& d);
TmTcTime& operator -= (TmTcTime& t, const TmTcDuration& d);
ostream& operator << (ostream& os, const TmTcTime& t);
istream& operator >> (istream& is, TmTcTime& t);
}

#endif
