/*
 * NOrbit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NOrbit__
#define __NOrbit__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing the current orbit parameters
//! This class represents a the current orbit parameters, such as altitude inclination latitude, longitude

class NOrbit
{
  // public interface:
 public:
  //! Standard constructor
  NOrbit();
  //! Default destructor
  virtual ~NOrbit();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the altitude in mm
  void SetAltitude(double Altitude) { m_Empty = false; m_Altitude = Altitude; }
  //! Get the altitude in mm
  double GetAltitude() const { return m_Altitude; }

  //! Set the inclination in rad
  void SetInclination(double Inclination) { m_Empty = false; m_Inclination = Inclination; }
  //! Get the inclinations in rad
  double GetInclination() const { return m_Inclination; }

  //! Set the geographic latitude in rad
  void SetLatitude(double Latitude) { m_Empty = false; m_Latitude = Latitude; }
  //! Get the geographic latitude in rad
  double GetLatitude() const { return m_Latitude; }

  //! Set the geographic longitude in rad
  void SetLongitude(double Longitude) { m_Empty = false; m_Longitude = Longitude; }
  //! Get the geographic longitude in rad
  double GetLongitude() const { return m_Longitude; }


  // protected methods:
 protected:
  //NOrbit() {};
  //NOrbit(const NOrbit& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! The altitude in mm
  double m_Altitude;
  //! The inclination in rad
  double m_Inclination;
  //! The latitude in rad
  double m_Latitude;
  //! The longitude in rad
  double m_Longitude;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NOrbit, 0) // no description
#endif

};

//! Global operator for Orbit IO:
std::ostream& operator<<(std::ostream& os, const NOrbit& Orbit);

#endif


////////////////////////////////////////////////////////////////////////////////
