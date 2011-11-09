/*
 * NObservatoryData.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NObservatoryData__
#define __NObservatoryData__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cmath>
#include <fstream>
using namespace std;

// ROOT libs:
#include "NQuaternion.h"

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NOrientation.h"
#include "NMetrologyData.h"
#include "NStarTrackerData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing all observatory data which is going to be merged into the event stream
//! This class represents a complete set of observatory data

class NObservatoryData
{
  // public interface:
 public:
  //! Standard constructor
  NObservatoryData();
  //! Default destructor
  virtual ~NObservatoryData();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the orientation is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the measurement time
  void SetTime(const NTime& Time) { m_Empty = false; m_Time = Time; }
  //! Get the measurement time
  NTime GetTime() const { return m_Time; }

  //! Set the direction of the optical axis in inertial space
  void SetDirectionOpticalAxisInIS(const MVector& Dir) { m_Empty = false; m_DirectionOpticalAxisInIS = Dir; }
  //! Get the direction of the event in inertial space
  MVector GetDirectionOpticalAxisInIS() { return m_DirectionOpticalAxisInIS; }

  //! Set the direction of the event in inertial space
  void SetDirectionEventInIS(const MVector& Dir) { m_Empty = false; m_DirectionEventInIS = Dir; }
  //! Get the direction of the event in inertial space
  MVector GetDirectionEventInIS() { return m_DirectionEventInIS; }

  //! Return the right ascension of event
  double GetRa();
  //! Return the right ascension of event
  double GetRaScaled();
  //! Return the declination of event
  double GetDec(); 
  //! Return the right ascension of Opical Axis (OA)
  double GetRaOA();
  //! Return the declination of Opical Axis (OA)
  double GetDecOA();

  //! Set the direction of the focal plane in optic bench plane
  void SetOrientationFocalPlaneToOB(const NOrientation& O) { m_Empty = false; m_OrientationFocalPlaneToOB = O; }
  //! Get the direction of the focal plane in optic bench plane
  NOrientation GetOrientationFocalPlaneToOB() { return m_OrientationFocalPlaneToOB; }

  //! Set the direction of the OB plane in inertial space
  void SetOrientationOBToIS(const NOrientation& O) { m_Empty = false; m_OrientationOBToIS = O; }
  //! Get the direction of the OB plane in inertial space
  NOrientation GetOrientationOBToIS() { return m_OrientationOBToIS; }


  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, int Version, bool Compact = false);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version, bool Compact = false);


  // private members:
 private:
  //! True if this data is still empty, which is set during Clear()
  bool m_Empty;

  //! The time
  NTime m_Time;

  //! The direction of the optical axis in inertial space  
  MVector m_DirectionOpticalAxisInIS;
  //! The direction of the event in inertial space
  MVector m_DirectionEventInIS;
  //! The direction of the focal plane in OB plane
  NOrientation m_OrientationFocalPlaneToOB;
  //! The direction of the OB plane in IS space
  NOrientation m_OrientationOBToIS;
  
  


#ifdef ___CINT___
 public:
  ClassDef(NObservatoryData, 0) // no description
#endif

};


////////////////////////////////////////////////////////////////////////////////


//! \brief Simple structure allowing to compare NObservatoryData objects by their measurement time.
//! This structure enables comparing (and ultimately sorting) NObservatoryData objects by
//! their measurement time.
struct NObservatoryDataTimeComparator
{
  bool operator() (const NObservatoryData& D, NTime Time) const
  {
    return D.GetTime() < Time;
  }

  bool operator() (NTime Time, const NObservatoryData& D) const
  {
    return Time < D.GetTime();
  }
};


#endif


////////////////////////////////////////////////////////////////////////////////
