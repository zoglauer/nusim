/*
 * NTime.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NTime
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NTime.h"

// Standard libs:
#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;

// ROOT libs:
#include "TMath.h"

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NTime)
#endif


////////////////////////////////////////////////////////////////////////////////


const NTime g_TimeNotDefined = NTime(-numeric_limits<long>::max()+11, 0l);


////////////////////////////////////////////////////////////////////////////////


NTime::NTime()
{
  // Construct an instance of NTime

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NTime::NTime(const NTime& Time)
{
  // Construct an instance of NTime

  *this = Time;
}


////////////////////////////////////////////////////////////////////////////////


NTime::NTime(double Seconds)
{
  // Construct an instance of NTime

  Set(Seconds);
}


////////////////////////////////////////////////////////////////////////////////


NTime::NTime(long int Seconds, long int NanoSeconds)
{
  // Construct an instance of NTime

  Set(Seconds, NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


NTime::NTime(int Seconds, int NanoSeconds)
{
  // Construct an instance of NTime

  Set((long) Seconds, (long) NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


NTime::NTime(unsigned int Seconds, unsigned int NanoSeconds)
{
  // Construct an instance of NTime

  Set((long) Seconds, (long) NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


NTime::NTime(unsigned int Year, unsigned int Month, unsigned int Day, unsigned int Hour, 
             unsigned int Minute, unsigned int Second, unsigned int NanoSecond)
{
  // Construct an instance of NTime
  //
  // Format:
  // Year: 1970...2030 or 70 ... 130 or 70..99, 0, 1, 2, ... 30
  // Month: 1..12
  // Day: 1..31
  // Hour: 0..23
  // Min: 0..59
  // Sec: 0..61 (leap seconds!)
  // NanoSecond: 0..999999999 

  Set(Year, Month, Day, Hour, Minute, Second, NanoSecond);
}


////////////////////////////////////////////////////////////////////////////////


NTime::~NTime()
{
  // Delete this instance of NTime
}


////////////////////////////////////////////////////////////////////////////////


void NTime::Clear() 
{
  // Reset the time data to "NotDefined" values

  m_Seconds = -g_IntNotDefined;
  m_NanoSeconds = 0;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::Set(double Time)
{
  // Set the time using a double

  m_Seconds = (long) Time;
  m_NanoSeconds = (long) TMath::FloorNint((Time - m_Seconds) * 1000000000);
  m_Empty = false;
  
  Normalize();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::Set(const long Seconds, const long NanoSeconds)
{
  // Set the time using two longs

  m_Seconds = Seconds;
  m_NanoSeconds = NanoSeconds;
  m_Empty = false;
  
  Normalize();

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::Set(const TString& String, const TString& Prefix)
{
  if (String.BeginsWith(Prefix) == true) {
    long Seconds = 0;
    long NanoSeconds = 0;
    TString NewString = String;
    NewString.ReplaceAll(Prefix, "");
    NewString.ReplaceAll("sec", "");
    NewString.ReplaceAll(" ", "");
    MTokenizer T('.', true);
    T.Analyze(NewString);
    if (T.GetNTokens() == 1) {
      Seconds = T.GetTokenAtAsLong(0);
    } else if (T.GetNTokens() == 2) {
      Seconds = T.GetTokenAtAsLong(0);
      NanoSeconds = T.GetTokenAtAsLong(1);
      // Find digits:
      int Digits = T.GetTokenAt(1).Length();
      if (Digits < 9) {
        NanoSeconds *= int(pow(10.0, 9.0 - Digits));
      } else if (Digits > 9) {
        NanoSeconds /= int(pow(10.0, Digits - 9.0));   
      }
    } else {
      mout<<"NTime: Time not correct ("<<T.GetNTokens()<<" tokens): "<<String<<endl;
      return false;       
    }
    Set(Seconds, NanoSeconds);
  } else {
    mout<<"NTime: String does not start with "<<Prefix<<endl;
    return false;
  }

  m_Empty = false;
  Normalize();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::Set(const double Seconds, const double NanoSeconds)
{
  //

  Set(Seconds);
  NTime Temp(NanoSeconds/1000000000);
  *this += Temp;
  m_Empty = false;

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::Set(unsigned int Year, unsigned int Month, unsigned int Day, 
                unsigned int Hour, unsigned int Minute, unsigned int Second, 
                unsigned int NanoSecond)
{
  // Format:
  // Year: 1970...2030 or 70 ... 130 or 70..99, 0, 1, 2, ... 30
  // Month: 1..12
  // Day: 1..31
  // Hour: 0..23
  // Min: 0..59
  // Sec: 0..61 (leap seconds!)
  // NanoSecond: 0..999999999 

  struct tm tp;

  if (Year < 70) Year += 100;
  if (Year > 1970) Year -= 1900;
  tp.tm_year = Year;
  if (Month <= 0) {
    tp.tm_yday = Day;
  } else {
    tp.tm_mon = Month-1;
    tp.tm_mday = Day;
  }
  tp.tm_hour = Hour;
  tp.tm_min = Minute;
  tp.tm_sec = Second;
  tp.tm_isdst = -1; // ignore daylight saving stuff

  m_Seconds = mktime(&tp);
  
  if (m_Seconds == -1) {
    merr<<"No valid time format given!"<<endl;
    Clear();
  }

  m_NanoSeconds = NanoSecond;
  
  Normalize();

  m_Empty = false;
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NTime::Normalize()
{
  // Normalize
  
  while (m_NanoSeconds >= 1000000000) {
    m_Seconds += m_NanoSeconds/1000000000;
    m_NanoSeconds %= 1000000000;
  }
  while (m_NanoSeconds <= -1000000000) {
    m_Seconds += m_NanoSeconds/1000000000;
    m_NanoSeconds %= 1000000000;
  }

  while (m_Seconds > 0 && m_NanoSeconds < 0) {
    m_NanoSeconds += 1000000000;
    m_Seconds -= 1;
  }
  while (m_Seconds < 0 && m_NanoSeconds > 0) {
    m_NanoSeconds -= 1000000000;
    m_Seconds += 1;
  }
}


////////////////////////////////////////////////////////////////////////////////


double NTime::GetAsSeconds() const
{
  // Return the seconds

  return (double) m_Seconds + m_NanoSeconds/1000000000.0;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetNanoSeconds()
{
  // Return the seconds

  return m_NanoSeconds;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetSeconds()
{
  // Return the seconds

  struct tm tp;
  tp = *localtime(&m_Seconds);

  return tp.tm_sec;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetMinutes()
{
  // Return the minutes

  struct tm tp;
  tp = *localtime(&m_Seconds);

  return tp.tm_min;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetHours()
{
  // Return the hours

  struct tm tp;
  tp = *localtime(&m_Seconds);

  return tp.tm_hour;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetDays()
{
  // Return the day

  struct tm tp;
  tp = *localtime(&m_Seconds);

  return tp.tm_mday;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetDaysInYear()
{
  // Return the day

  struct tm tp;
  tp = *localtime(&m_Seconds);
  
  return tp.tm_yday + 1;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetMonths()
{
  // Return the month

  struct tm tp;
  tp = *localtime(&m_Seconds);

  return tp.tm_mon + 1;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NTime::GetYears()
{
  // Return the Year

  struct tm tp;
  tp = *localtime(&m_Seconds);

  return tp.tm_year + 1900;
}


////////////////////////////////////////////////////////////////////////////////


TString NTime::GetDateInString()
{
  ostringstream out;

  out<<GetYears()<<"-";
  
  if (GetMonths() < 10){ 
    out<<"0"<<GetMonths()<<"-";
  }	
  else {
    out<<GetMonths()<<"-";
  }
  
  if (GetDays() < 10) {
    out<<"0"<<GetDays()<<"T";
  }
  else {
    out<<GetDays()<<"T";
  }
    
  if (GetHours() < 10) {
    out<<"0"<<GetHours()<<":"; 
  }
  else {
    out<<GetHours()<<":";
  }
  	 
  if (GetMinutes() < 10) {
    out<<"0"<<GetMinutes()<<":";
  }
  else {
    out<<GetMinutes()<<":";
  }
  	
  if (GetSeconds() < 10) {
    out<<"0"<<GetSeconds();
  }
  else {
    out<<GetSeconds();
  }
   
  return TString(out.str().c_str());
}


////////////////////////////////////////////////////////////////////////////////


NTime& NTime::operator=(const NTime& T)
{
  // Assignment operator

  m_Seconds = T.m_Seconds;
  m_NanoSeconds = T.m_NanoSeconds;
  m_Empty = T.m_Empty;

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


NTime& NTime::operator=(const double& T)
{
  // Assignment operator utilizing a double

  Set(T);

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


NTime& NTime::operator+=(const NTime& T)
{
  // += operator

  m_Seconds += T.m_Seconds;
  m_NanoSeconds += T.m_NanoSeconds;
  m_Empty = false;
  Normalize();

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


NTime& NTime::operator+=(const double& T)
{
  // += operator

  NTime New(T);
  (*this) += New;

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


NTime& NTime::operator-=(const NTime& T)
{
  // -= operator

  m_Seconds -= T.m_Seconds;
  m_NanoSeconds -= T.m_NanoSeconds;
  m_Empty = false;
  Normalize();
  
  return *this;
}


////////////////////////////////////////////////////////////////////////////////


NTime& NTime::operator-=(const double& T)
{
  // -= operator

  NTime New(T);
  (*this) -= New;

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::operator==(const NTime& T) const
{
  // == operator

  return (m_Seconds == T.m_Seconds && m_NanoSeconds == T.m_NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::operator!=(const NTime& T) const
{
  // != operator

  return (m_Seconds != T.m_Seconds || m_NanoSeconds != T.m_NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::operator>(const NTime& T) const
{
  // > operator

  return (m_Seconds > T.m_Seconds || (m_Seconds == T.m_Seconds && m_NanoSeconds > T.m_NanoSeconds));
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::operator<(const NTime& T) const
{
  // < operator

  return (m_Seconds < T.m_Seconds || (m_Seconds == T.m_Seconds && m_NanoSeconds < T.m_NanoSeconds));
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::operator>=(const NTime& T) const
{
  // >= operator

  return (m_Seconds > T.m_Seconds || (m_Seconds == T.m_Seconds && m_NanoSeconds >= T.m_NanoSeconds));
}


////////////////////////////////////////////////////////////////////////////////


bool NTime::operator<=(const NTime& T) const
{
  // < operator

  return (m_Seconds < T.m_Seconds || (m_Seconds == T.m_Seconds && m_NanoSeconds <= T.m_NanoSeconds));
}


////////////////////////////////////////////////////////////////////////////////


NTime NTime::operator+(const NTime& Time) const
{
  //! The + operator 

  return NTime(m_Seconds + Time.m_Seconds, m_NanoSeconds + Time.m_NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


NTime NTime::operator+(const double& Time) const
{
  //! The + operator with a double

  return (*this) + NTime(Time);
}


////////////////////////////////////////////////////////////////////////////////


NTime NTime::operator-(const NTime& Time) const
{
  //! The - operator 

  return NTime(m_Seconds - Time.m_Seconds, m_NanoSeconds - Time.m_NanoSeconds);
}


////////////////////////////////////////////////////////////////////////////////


NTime NTime::operator-(const double& Time) const
{
  //! The - operator with a double

  return (*this) - NTime(Time);
}


////////////////////////////////////////////////////////////////////////////////


NTime NTime::operator*(const double& Scale) const
{
  //! The * operator with a double

  // Overflow detection --- possible loss of precision...
  if (numeric_limits<long>::max() < double(m_NanoSeconds)*Scale) {
    if (log10(1E9*GetAsSeconds()*Scale) > 53*log10(2)) {
      mout<<"Overflow in NTime: Precision loss of < "<<ceil(pow(10, log10(1E9*GetAsSeconds()*Scale)-53*log10(2)))<<" nanoseconds"<<endl;
      if (sizeof(long) < 8) {
        mout<<"   I suggest you switch to a 64-bit system..."<<endl;
      } 
    }
    return NTime(GetAsSeconds()*Scale);
  } else {
    NTime T;
    T.Set(m_Seconds*Scale, m_NanoSeconds*Scale);
    return T;
  }
}


////////////////////////////////////////////////////////////////////////////////


double NTime::operator/(const NTime& Time) const
{
  //! The / operator with a time

  if (Time.GetAsSeconds() == 0) return 0;

  return (*this).GetAsSeconds()/Time.GetAsSeconds();
}


////////////////////////////////////////////////////////////////////////////////


NTime NTime::operator/(const double& Scale) const
{
  //! The / operator with a double

  NTime T;
  T.Set(m_Seconds/Scale, m_NanoSeconds/Scale);
  return T;
}


////////////////////////////////////////////////////////////////////////////////


TString NTime::ToString() const
{
  // Dump the time as string ion seconds
  
  return GetString(9);
}


////////////////////////////////////////////////////////////////////////////////


TString NTime::GetString(unsigned int Precision) const
{
  // Dump the time as string in seconds
  
  if (Precision > 9) Precision = 9;
  
  long Nanos = m_NanoSeconds;
  Nanos /= pow(10.0, 9.0-Precision);
  
  ostringstream out;
  out<<((Nanos < 0 && m_Seconds == 0) ? "-" : "")<<m_Seconds;
  if (Precision > 0) {
    out<<"."<<setprecision(Precision)<<setw(Precision)<<setfill('0')<<fabs(Nanos);
  }
  out<<" sec";

  return TString(out.str().c_str());
}


////////////////////////////////////////////////////////////////////////////////


TString NTime::GetOccultationString()
{
  // Format: 2012/045 00:04:49
  
  ostringstream out;
  out<<GetYears()<<"/"
      <<setw(3)<<setfill('0')<<GetDaysInYear()<<" "
      <<setw(2)<<setfill('0')<<GetHours()<<":"
      <<setw(2)<<setfill('0')<<GetMinutes()<<":"
      <<setw(2)<<setfill('0')<<GetSeconds();
  
  return out.str().c_str();
}


////////////////////////////////////////////////////////////////////////////////


TString NTime::GetASCIIFileString()
{
  // Format: 2012/045 00:04:49
  
  ostringstream out;
  out<<GetYears()<<"-"
      <<setw(2)<<setfill('0')<<GetMonths()<<"-"
      <<setw(2)<<setfill('0')<<GetDays()<<" "
      <<setw(2)<<setfill('0')<<GetHours()<<"-"
      <<setw(2)<<setfill('0')<<GetMinutes()<<"-"
      <<setw(2)<<setfill('0')<<GetSeconds();
  
  return out.str().c_str();
}


////////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream& os, const NTime& Time)
{
  os<<Time.ToString();
  return os;
}


// NTime.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
