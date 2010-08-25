/*
 * NAlignments.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NAlignments__
#define __NAlignments__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NOrientation.h"
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This class contains all available orientations in NuSIM

class NAlignments
{
  // public interface:
 public:
  //! Default constructor
  NAlignments();
  //! Default destructor
  virtual ~NAlignments();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the orientation is empty
  bool IsEmpty() const { return m_Empty; }

  //! Parse the data form the default file format
  bool Parse(TString Positions, TString Rotations);

  //! Parse the calibration data form the default file format
  bool ParseCalibrations(TString Positions, TString Rotations);

  //! Parse the error data form the default file format
  bool ParseErrors(TString Positions);

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationSpaceCraftRelInertial();

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationFocalPlaneRelSC();

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationOpticalBench();

  //! Get the optics orientation at time t                      
  virtual NOrientation GetOrientationOptics(int OpticsID);

  //! Get the optics orientation realtive to the optical bench at time t                      
  virtual NOrientation GetOrientationOpticsRelOpticalBench(int OpticsID);

  //! Get the aperture orientation at time t
  virtual NOrientation GetOrientationAperture(int ApertureID);

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyLaser(int MetrologyID);

  //! Get the metrology orientation relative to the optical bench at time t
  virtual NOrientation GetOrientationMetrologyLaserRelOpticalBench(int MetrologyID);

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyDetector(int MetrologyID);

  //! Get the star tracker orientation at time t
  virtual NOrientation GetOrientationStarTracker(int StarTrackerID);

  //! Get the star tracker orientation relative to the optical bench at time t
  virtual NOrientation GetOrientationStarTrackerRelOpticalBench(int StarTrackerID);

  //! Get the focal plane detector module orientation at time t
  virtual NOrientation GetOrientationFocalPlaneModule(int ModuleID);

  //! Get the focal plane detector module - detector orientation at time t
  virtual NOrientation GetOrientationDetectorRelFocalPlaneModule(int ModuleID, int DetectorID);

  //! Get the time to which these orientations belong
  virtual NTime GetTime() const { return m_Time; }

  //! Set the time to which these orientations belong
  virtual void SetTime(NTime Time) { m_Time = Time; }

  //! Return the content as text
  virtual TString ToString() const;
  
  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);

  //! Parse the content from a line of an ASCII file  
  bool Parse(TString& Line);

  //! Parse the content from a line in the data base
  bool ParseDB(TString Positions, TString Rotations);

  
  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! The time of the orientations
  NTime m_Time;

  //! The optical bench relative to focal bench
  NOrientation m_OpticalBench;

  //! The space craft realtive the the inertial system
  NOrientation m_SpaceCraftRelInertial;

  //! The focal bench realive to the space craft
  NOrientation m_FocalPlaneRelSC;

  //! Optics 1 relative to optical bench
  NOrientation m_Optics1RelOB;
  //! Optics 2 relative to optical bench
  NOrientation m_Optics2RelOB;

  //! The aperture 1 relative to focal bench
  NOrientation m_Aperture1;
  //! The aperture 2 relative to focal bench
  NOrientation m_Aperture2;

  //! The metrology laser 1 relative to optical bench
  NOrientation m_MetrologyLaser1RelOB;
  //! The metrology detector 1 relative to focal bench
  NOrientation m_MetrologyDetector1;

  //! The metrology laser 2 relative to optical bench
  NOrientation m_MetrologyLaser2RelOB;
  //! The metrology detector 2 relative to focal bench
  NOrientation m_MetrologyDetector2;

  //! The metrology laser 3 relative to optical bench
  NOrientation m_MetrologyLaser3RelOB;
  //! The metrology detector 3 relative to focal bench
  NOrientation m_MetrologyDetector3;

  //! The star tracker 4 relative to optical bench
  NOrientation m_StarTracker4RelOB;

  //! The focal plane module 1 relative to focal bench
  NOrientation m_FocalPlaneModule1;
  //! The focal plane module 2 relative to focal bench
  NOrientation m_FocalPlaneModule2;

  //! The detector 1 of the focal plane module 1 relative to focal bench
  NOrientation m_FocalPlaneModule1Detector1RelFocalPlaneModule1;
  //! The detector 2 of the focal plane module 1 relative to focal bench
  NOrientation m_FocalPlaneModule1Detector2RelFocalPlaneModule1;
  //! The detector 3 of the focal plane module 1 relative to focal bench
  NOrientation m_FocalPlaneModule1Detector3RelFocalPlaneModule1;
  //! The detector 4 of the focal plane module 1 relative to focal bench
  NOrientation m_FocalPlaneModule1Detector4RelFocalPlaneModule1;

  //! The detector 1 of the focal plane module 2 relative to focal bench
  NOrientation m_FocalPlaneModule2Detector1RelFocalPlaneModule2;
  //! The detector 2 of the focal plane module 2 relative to focal bench
  NOrientation m_FocalPlaneModule2Detector2RelFocalPlaneModule2;
  //! The detector 3 of the focal plane module 2 relative to focal bench
  NOrientation m_FocalPlaneModule2Detector3RelFocalPlaneModule2;
  //! The detector 4 of the focal plane module 2 relative to focal bench
  NOrientation m_FocalPlaneModule2Detector4RelFocalPlaneModule2;


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NAlignments, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
