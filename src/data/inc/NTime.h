/*
 * NTime.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NTime__
#define __NTime__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing time
//! This class represents a NuSTAR time - either ideal or measurement. 
//! Currently the internal data represnattion is a double. This might be changed in future versions.

class NTime
{
  // public interface:
 public:
  //! Standard constructor
  NTime();
  //! Copy constructor
  NTime(const NTime& Time);
  //! Default constructor with double
  NTime(double Seconds);
  //! Default constructor with two long's
  NTime(long Seconds, long NanoSeconds = 0);
  //! Default constructor with two int's
  NTime(int Seconds, int NanoSeconds = 0);
  //! Default constructor with two unsigned int's
  NTime(unsigned int Seconds, unsigned int NanoSeconds = 0);
  //! Set everything explictly
  NTime(unsigned int m_Year, unsigned int m_Month, unsigned int m_Day, unsigned int m_Hour = 0, 
        unsigned int m_Minute = 0, unsigned int m_Second = 0, unsigned int m_NanoSecond = 0);
  //! Default destructor
  virtual ~NTime();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the seconds
  bool Set(double Seconds);
  //! Set all values explictly
  bool Set(unsigned int m_Year, unsigned int m_Month, unsigned int m_Day, unsigned int m_Hour = 0, 
           unsigned int m_Minute = 0, unsigned int m_Second = 0, unsigned int m_NanoSecond = 0);
  //! Set as two longs
  bool Set(const long Seconds, const long NanoSeconds = 0);
  //! Set as two doubles
  bool Set(const double Seconds, const double NanoSeconds);
  //! Set as TI string: "TI 2376547.23487"
  bool Set(const TString& String);

  //! Return content as seconds
  double GetAsSeconds() const;
  //! Return the content as seconds
  //operator double() const { return GetAsSeconds(); }

  //! Return the nanoseconds (0..999999999)
  unsigned int GetNanoSeconds();
  //! Return the seconds (0..59)
  unsigned int GetSeconds();
  //! Return the minutes
  unsigned int GetMinutes();
  //! Return the hours
  unsigned int GetHours();
  //! Return the days
  unsigned int GetDays();
  //! Return the days
  unsigned int GetDaysInYear();
  //! Return the months
  unsigned int GetMonths();
  //! Return the years
  unsigned int GetYears();
  
  //! The assignment operator
  NTime& operator=(const NTime& Time);
  //! The assignment operator with a double
  NTime& operator=(const double& Time);

  //! The += operator
  NTime& operator+=(const NTime& Time);
  //! The += operator with a double
  NTime& operator+=(const double& Time);

  //! The -= operator
  NTime& operator-=(const NTime& Time);
  //! The -= operator with a double
  NTime& operator-=(const double& Time);

  //! The + operator 
  NTime operator+(const NTime& Time) const;
  //! The + operator with a double
  NTime operator+(const double& Time) const;
  //! The - operator 
  NTime operator-(const NTime& Time) const;
  //! The - operator with a double
  NTime operator-(const double& Time) const;

  //! The * operator with a double
  NTime operator*(const double& Scalar) const;

  //! The / operator with a time
  double operator/(const NTime& Time) const;
  //! The / operator with a double
  NTime operator/(const double& Scalar) const;

  
  //! The == operator
  bool operator==(const NTime& Time) const;
  //! The != operator
  bool operator!=(const NTime& Time) const;
  //! The > operator
  bool operator>(const NTime& Time) const;
  //! The < operator
  bool operator<(const NTime& Time) const;
  //! The >= operator
  bool operator>=(const NTime& Time) const;
  //! The <= operator
  bool operator<=(const NTime& Time) const;

  //! Get as string with the given precisision and without "sec"
  TString GetString(unsigned int Precision = 9) const;

  //! Return the date as a string
  TString GetDateInString();
  //! Return the date as a string
  TString GetOccultationString();
  //! Return the date as a string
  TString GetASCIIFileString();

  //! Dump the time as string in seconds in the form "123.1376765 sec"
  TString ToString() const;

  // protected methods:
 protected:
  //NTime() {};
  //NTime(const NTime& NCTHit) {};

  //! Make sure there are not more than 999999999 nanoseconds, and that they are positive.
  void Normalize();
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  // The internal time representation
  //! The seconds
  long m_Seconds;
  //! The nanoseconds
  long m_NanoSeconds; 
  
#ifdef ___CINT___
 public:
  ClassDef(NTime, 0) // no description
#endif

};

//! Global operator for Time IO:
std::ostream& operator<<(std::ostream& os, const NTime& Time);

#endif


////////////////////////////////////////////////////////////////////////////////
