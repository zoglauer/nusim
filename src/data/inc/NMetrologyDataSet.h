/*
 * NMetrologyDataSet.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NMetrologyDataSet__
#define __NMetrologyDataSet__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing the measured and calibrated metrology data
//! This class represents a single metrology measurement: the original laser hit position in the metrology coordinate system,
//! the measured currents of the PSD electrodes, the calibrated position in the metrology coordinate system as well as the measurement time

class NMetrologyDataSet
{
  // public interface:
 public:
  //! Standard constructor
  NMetrologyDataSet();
  //! Default destructor
  virtual ~NMetrologyDataSet();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the orientation is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the measurement time
  void SetTime(const NTime& Time) { m_Empty = false; m_Time = Time; }
  //! Get the measurement time
  NTime GetTime() const { return m_Time; }

  //! Set the original interaction position of the laser on the PSD in metrology coordinates in mm
  void SetOriginalLaserHit(const MVector& OriginalLaserHit) { m_Empty = false; m_OriginalLaserHit = OriginalLaserHit; }
  //! Get the original interaction position of the laser on the PSD in metrology coordinates in mm
  MVector GetOriginalLaserHit() const { return m_OriginalLaserHit; }

  //! Set the measured currents of the PSD electrodes
  void SetCurrents(const double North, const double South, const double East, const double West) { 
    m_Empty = false; 
    m_CurrentNorth = North; 
    m_CurrentSouth = South; 
    m_CurrentEast = East; 
    m_CurrentWest = West; 
  }
  //! Get the current of the North PSD Electrode
  double GetCurrentNorth() const { return m_CurrentNorth; }
  //! Get the current of the South PSD Electrode
  double GetCurrentSouth() const { return m_CurrentSouth; }
  //! Get the current of the East PSD Electrode
  double GetCurrentEast() const { return m_CurrentEast; }
  //! Get the current of the West PSD Electrode
  double GetCurrentWest() const { return m_CurrentWest; }

  //! Set the calibrated interaction position of the laser on the PSD in metrology coordinates in mm
  void SetCalibratedLaserHit(const MVector& CalibratedLaserHit) { m_Empty = false; m_CalibratedLaserHit = CalibratedLaserHit; }
  //! Get the calibrated interaction position of the laser on the PSD in metrology coordinates in mm
  MVector GetCalibratedLaserHit() const { return m_CalibratedLaserHit; }

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, int Version, TString Keyword);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);

  //! Interpolate from previous data point
  void Interpolate(NMetrologyDataSet A, NMetrologyDataSet B, NTime t);

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! Time the pointing was measured
  NTime m_Time;

  //! The original laser hit position of the laser on the PSD in metrology coordinates in mm
  MVector m_OriginalLaserHit;

  //! The measured current of the North PSD electrode
  double m_CurrentNorth;
  //! The measured current of the South PSD electrode
  double m_CurrentSouth;
  //! The measured current of the East PSD electrode
  double m_CurrentEast;
  //! The measured current of the West PSD electrode
  double m_CurrentWest;

  //! The calibrated laser hit position of the laser on the PSD in metrology coordinates in mm
  MVector m_CalibratedLaserHit;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NMetrologyDataSet, 0) // no description
#endif

};


////////////////////////////////////////////////////////////////////////////////


//! \brief Simple structure allowing to compare NMetrologyDataSet objects by their measurement time.
//! This structure enables comparing (and ultimately sorting) NMetrologyDataSet objects by
//! their measurement time.
struct NMetrologyDataSetTimeComparator
{
  bool operator() (const NMetrologyDataSet& D, NTime Time) const
  {
    return D.GetTime() < Time;
  }

  bool operator() (NTime Time, const NMetrologyDataSet& D) const
  {
    return Time < D.GetTime();
  }
};


#endif


////////////////////////////////////////////////////////////////////////////////
