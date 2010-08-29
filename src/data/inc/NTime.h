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
  //! Default destructor
  virtual ~NTime();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the seconds
  void SetSeconds(double Seconds) { m_Empty = false; m_Seconds = Seconds; }
  //! Return content as seconds
  double GetSeconds() const { return m_Seconds; }

  //! The asignment operator
  NTime& operator=(const NTime& Time);
  //! The asignment operator with a double
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


  //! Dump the time as string ion seconds
  TString ToString() const;

  // protected methods:
 protected:
  //NTime() {};
  //NTime(const NTime& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! The internal time representation --- currently a double in seconds
  double m_Seconds;

#ifdef ___CINT___
 public:
  ClassDef(NTime, 0) // no description
#endif

};

//! Global operator for Time IO:
std::ostream& operator<<(std::ostream& os, const NTime& Time);

#endif


////////////////////////////////////////////////////////////////////////////////
