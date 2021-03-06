/*
 * NSatellite.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NSatellite__
#define __NSatellite__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TRotation.h"

// MEGAlib libs:
#include "NGlobal.h"
#include "MTime.h"

// NuSTAR libs:
#include "NTime.h"
#include "NPointing.h"
#include "NOrientation.h"
#include "NOrbit.h"
#include "NModuleInterfaceOrbit.h"
#include "NModuleInterfacePointing.h"
#include "NModuleInterfaceOrientations.h"
#include "NModuleInterfaceTime.h"
#include "NModuleInterfaceGeometryAndDetectorProperties.h"
         
// Forward declarations:

////////////////////////////////////////////////////////////////////////////////


//! \brief The satellite super module
//! This class represents the satellite super module.
//! It is the facade to all the data provided by the individual satellite modules, and provides their main data to the outside world, 
//! such as pointing, orbit, and orientations.
//! It also knows the current space craft time.
//! It implements the interfaces NModuleInterfaceOrbit, NModuleInterfacePointing, NModuleInterfaceOrientations, and NModuleInterfaceTime

class NSatellite : public NModuleInterfaceOrbit, public NModuleInterfacePointing, public NModuleInterfaceOrientations, public NModuleInterfaceTime, public NModuleInterfaceGeometryAndDetectorProperties
{
  // public interface:
 public:
  //! Standard constructor
  NSatellite();
  //! Default destructor
  virtual ~NSatellite();

  //! Resets all data to default values
  virtual void Clear() {};

  //! Initialize for simulating a new run
  virtual bool Initialize();

  //! Set the pointing module
  void SetPointingModule(NModuleInterfacePointing* Pointing) { m_Pointing = Pointing; }
  //! Get the pointing module
  NModuleInterfacePointing* GetPointingModule() { return m_Pointing; }
  //! Set the orbit module
  void SetOrbitModule(NModuleInterfaceOrbit* Orbit) { m_Orbit = Orbit; }
  //! Get the orbit module
  NModuleInterfaceOrbit* GetOrbitModule() { return m_Orbit; }
  //! Set the orientations module
  void SetOrientationsModule(NModuleInterfaceOrientations* Orientations) { m_Orientations = Orientations; }
  //! Get the orientations module
  NModuleInterfaceOrientations* GetOrientationsModule() { return m_Orientations; }
  //! Set the time module
  void SetTimeModule(NModuleInterfaceTime* Time) { m_Time = Time; }
  //! Get the time module
  NModuleInterfaceTime* GetTimeModule() { return m_Time; }
  //! Set the detector properties module
  void SetGeometryAndDetectorPropertiesModule(NModuleInterfaceGeometryAndDetectorProperties* GeometryAndDetectorProperties) { m_GeometryAndDetectorProperties = GeometryAndDetectorProperties; }
  //! Get the detector properties module
  NModuleInterfaceGeometryAndDetectorProperties* GetGeometryAndDetectorPropertiesModule() { return m_GeometryAndDetectorProperties; }


  //! Be the facade for the modules, i.e. implement all interfaces

  //! Get the orbit at time t
  virtual NOrbit GetOrbit(const NTime& t) { return m_Orbit->GetOrbit(t); }
  //! Get the orbit duration
  NTime GetOrbitDuration(NTime t) const { return m_Orbit->GetOrbitDuration(t); }
  //! Get the start time of the next black-out
  virtual NTime StartOfNextBlackout(NTime t) { return m_Orbit->StartOfNextBlackout(t); }
  //! Get the end time of the next black-out
  virtual NTime EndOfNextBlackout(NTime t) { return m_Orbit->EndOfNextBlackout(t); }
  //! Return true if we are within blackout
  virtual bool IsBlackout(NTime t) { return m_Orbit->IsBlackout(t); }
  //! Get the number of blackouts in between
  virtual NTime GetBlackoutDuration(NTime t1, NTime t2) { return m_Orbit->GetBlackoutDuration(t1, t2); }
  //! Get the effective observation time in between times
  virtual NTime GetEffectiveObservationTime(NTime t1, NTime t2) { return m_Orbit->GetEffectiveObservationTime(t1, t2); }
  //! Get the ideal time as a function of observation time
  virtual NTime FindIdealTime(NTime ObservationTime) { return m_Orbit->FindIdealTime(ObservationTime); }

  //! Get time of next switch to night
  virtual NTime GetNextEndNightTime(NTime t) { return m_Orbit->GetNextEndNightTime(t); }
  //! Get time of next switch to day
  virtual NTime GetNextBeginNightTime(NTime t) { return m_Orbit->GetNextBeginNightTime(t); }
  //! Get time of next switch to night
  virtual NTime GetLastEndNightTime(NTime t) { return m_Orbit->GetLastEndNightTime(t); }
  //! Get time of next switch to day
  virtual NTime GetLastBeginNightTime(NTime t) { return m_Orbit->GetLastBeginNightTime(t); }
  //! Return true if we are within night time
  virtual bool IsNight(NTime t) { return m_Orbit->IsNight(t); }

//! Return if we provide day and night cycles
  virtual bool OrbitProvidesDayAndNightCycles() const { return m_Orbit->OrbitProvidesDayAndNightCycles(); }



  //! Get all orientations
  virtual NAlignmentsDBEntry GetAllAlignments(const NTime& t) { return m_Orientations->GetAllAlignments(t); }

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationSpaceCraftRelInertial(const NTime& t) { return m_Orientations->GetOrientationSpaceCraftRelInertial(t); }

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationFocalPlaneRelSC(const NTime& t) { return m_Orientations->GetOrientationFocalPlaneRelSC(t); }

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationOpticalBench(const NTime& t) { return m_Orientations->GetOrientationOpticalBench(t); }

  //! Get the optics orientation at time t                      
  virtual NOrientation GetOrientationOptics(const NTime& t, int OpticsID) { return m_Orientations->GetOrientationOptics(t, OpticsID); }

  //! Get the optics orientation realtive to the optical bench at time t                      
  virtual NOrientation GetOrientationOpticsRelOpticalBench(const NTime& t, int OpticsID) { return m_Orientations->GetOrientationOpticsRelOpticalBench(t, OpticsID); }

  //! Get the aperture orientation at time t
  virtual NOrientation GetOrientationAperture(const NTime& t, int ApertureID) { return m_Orientations-> GetOrientationAperture(t, ApertureID); }

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyLaser(const NTime& t, int MetrologyID) { return m_Orientations->GetOrientationMetrologyLaser(t, MetrologyID); }

  //! Get the metrology orientation realtive to the optical bench at time t
  virtual NOrientation GetOrientationMetrologyLaserRelOpticalBench(const NTime& t, int MetrologyID) { return m_Orientations->GetOrientationMetrologyLaserRelOpticalBench(t, MetrologyID); }

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyDetector(const NTime& t, int MetrologyID) { return m_Orientations->GetOrientationMetrologyDetector(t, MetrologyID); }

  //! Get the star tracker orientation at time t
  virtual NOrientation GetOrientationStarTracker(const NTime& t, int StarTrackerID) { return m_Orientations->GetOrientationStarTracker(t, StarTrackerID); }

  //! Get the star tracker orientation realtive to the optical bench at time t
  virtual NOrientation GetOrientationStarTrackerRelOpticalBench(const NTime& t, int StarTrackerID) { return m_Orientations->GetOrientationStarTrackerRelOpticalBench(t, StarTrackerID); }

  //! Get the focal plane detector module orientation at time t
  virtual NOrientation GetOrientationFocalPlaneModule(const NTime& t, int ModuleID) { return m_Orientations->GetOrientationFocalPlaneModule(t, ModuleID); }

  //! Get the focal plane detector module - detector orientation at time t
  virtual NOrientation GetOrientationDetectorRelFocalPlaneModule(const NTime& t, int ModuleID, int DetectorID) { return m_Orientations->GetOrientationDetectorRelFocalPlaneModule(t, ModuleID, DetectorID); }

  //! Get the file name of the pertubations data base
  virtual TString GetPerturbedAlignmentsDBFileName() const { return m_Orientations->GetPerturbedAlignmentsDBFileName(); }



  //! Get the CALIBRATED focal plane detector module orientation
  virtual NOrientation GetCalibratedOrientationFocalPlaneModule(int ModuleID) { return m_Orientations->GetCalibratedOrientationFocalPlaneModule(ModuleID); }

  //! Get the CALIBRATED metrology detector orientation
  virtual NOrientation GetCalibratedOrientationMetrologyDetector(int MetrologyID) { return m_Orientations->GetCalibratedOrientationMetrologyDetector(MetrologyID); }

  //! Get the CALIBRATED optic bench orientation
  virtual NOrientation GetCalibratedOrientationOpticalBench() { return m_Orientations->GetCalibratedOrientationOpticalBench(); }

  //! Get the CALIBRATED optics orientation realtive to the optical bench                    
  virtual NOrientation GetCalibratedOrientationOpticsRelOpticalBench(int OpticsID) { return m_Orientations->GetCalibratedOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get the CALIBRATED metrology orientation relative to the optical bench
  virtual NOrientation GetCalibratedOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_Orientations->GetCalibratedOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get the CALIBRATED star tracker orientation relative to the optical bench
  virtual NOrientation GetCalibratedOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_Orientations->GetCalibratedOrientationStarTrackerRelOpticalBench(StarTrackerID); }
  
  //! Get the file name of the calibrated alignments data base
  virtual TString GetCalibratedAlignmentsDBFileName() const { return m_Orientations->GetCalibratedAlignmentsDBFileName(); }


  // Optics

  //! The bore sight of an optics module 
  MVector GetBoreSightRelOM(const NTime& t, int ModuleID) { return m_Orientations->GetBoreSightRelOM(t, ModuleID); }

  //! The CALIBRATED bore sight of an optics module 
  MVector GetCalibratedBoreSightRelOM(int ModuleID) { return m_Orientations->GetCalibratedBoreSightRelOM(ModuleID); }



  // Metrology/Star tracker

  //! Get the pointing error of the star tracker
  virtual double GetPointingErrorStarTracker(const NTime& t, int StarTrackerID) { return m_Orientations->GetPointingErrorStarTracker(t, StarTrackerID); }

  //! Get the centroiding error of the metrology detector
  virtual double GetCentroidingErrorMetrologyDetector(const NTime& t, int MetrologyID) { return m_Orientations->GetCentroidingErrorMetrologyDetector(t, MetrologyID); }
 
  //! Get the origin position of the metrology laser (independent of time)
  virtual MVector GetOriginMetrologyLaserRelML(const NTime& t, int MetrologyID) { return m_Orientations->GetOriginMetrologyLaserRelML(t, MetrologyID); }
 
  //! Get the calibrated origin position of the metrology laser (independent of time)
  virtual MVector GetCalibratedOriginMetrologyLaserRelML(int MetrologyID) { return m_Orientations->GetCalibratedOriginMetrologyLaserRelML(MetrologyID); }
 
  //! Get the pointing direction of the metrology laser (independent of time)
  virtual MVector GetPointingMetrologyLaserRelML(const NTime& t, int MetrologyID) { return m_Orientations->GetPointingMetrologyLaserRelML(t, MetrologyID); }
 
  //! Get the calibrated pointing direction of the metrology laser (independent of time)
  virtual MVector GetCalibratedPointingMetrologyLaserRelML(int MetrologyID) { return m_Orientations->GetCalibratedPointingMetrologyLaserRelML(MetrologyID); }


  /*
  //! Get the calibrated optical axis of optics module (independent of time)
  virtual MVector GetCalibratedPointingOpticalAxisRelOM(int OpticsID) { return m_Orientations->GetCalibratedPointingOpticalAxisRelOM(OpticsID); }

  //! Get the calibrated pointing direction of the metrology laser (independent of time)
  virtual MVector GetCalibratedPointingMetrologyLaserRelML(int MetrologyID) { return m_Orientations->GetCalibratedPointingMetrologyLaserRelML(MetrologyID); }


	//! Get the centroiding error of the metrology detector
	virtual double GetCentroidingErrorMetrologyDetector(int MetrologyID) { return m_Orientations->GetCentroidingErrorMetrologyDetector(MetrologyID); }

	//! Get the pointing error of the star tracker
	virtual double GetPointingErrorStarTracker(int StarTrackerID) { return m_Orientations->GetPointingErrorStarTracker(StarTrackerID); }
  */

  /*
  //! Get the xyz uncertainty of determining the focal plane detector module orientation
  virtual MVector GetErrorXYZOrientationFocalPlaneModule(int ModuleID) { return m_Orientations->GetErrorXYZOrientationFocalPlaneModule(ModuleID); }

  //! Get the xyz uncertainty of determining the metrology detector orientation
  virtual MVector GetErrorXYZOrientationMetrologyDetector(int MetrologyID) { return m_Orientations->GetErrorXYZOrientationMetrologyDetector(MetrologyID); }

  //! Get xyz uncertainty of determining the aperture orientation
  virtual MVector GetErrorXYZOrientationAperture(int ApertureID) { return m_Orientations->GetErrorXYZOrientationAperture(ApertureID); }

  //! Get xyz uncertainty of determining the optic bench orientation
  virtual MVector GetErrorXYZOrientationOpticalBench() { return m_Orientations->GetErrorXYZOrientationOpticalBench(); }

  //! Get xyz uncertainty of determining the optics orientation realtive to the optical bench                    
  virtual MVector GetErrorXYZOrientationOpticsRelOpticalBench(int OpticsID) { return m_Orientations->GetErrorXYZOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get xyz uncertainty of determining the metrology orientation relative to the optical bench
  virtual MVector GetErrorXYZOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_Orientations->GetErrorXYZOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get xyz uncertainty of determining the star tracker orientation relative to the optical bench
  virtual MVector GetErrorXYZOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_Orientations->GetErrorXYZOrientationStarTrackerRelOpticalBench(StarTrackerID); }



  //! Get the dcm uncertainty of determining the focal plane detector module orientation
  virtual MVector GetErrorDCMOrientationFocalPlaneModule(int ModuleID) { return m_Orientations->GetErrorDCMOrientationFocalPlaneModule(ModuleID); }

  //! Get the dcm uncertainty of determining the metrology detector orientation
  virtual MVector GetErrorDCMOrientationMetrologyDetector(int MetrologyID) { return m_Orientations->GetErrorDCMOrientationMetrologyDetector(MetrologyID); }

  //! Get dcm uncertainty of determining the aperture orientation
  virtual MVector GetErrorDCMOrientationAperture(int ApertureID) { return m_Orientations->GetErrorDCMOrientationAperture(ApertureID); }

  //! Get dcm uncertainty of determining the optic bench orientation
  virtual MVector GetErrorDCMOrientationOpticalBench() { return m_Orientations->GetErrorDCMOrientationOpticalBench(); }

  //! Get dcm uncertainty of determining the optics orientation realtive to the optical bench                    
  virtual MVector GetErrorDCMOrientationOpticsRelOpticalBench(int OpticsID) { return m_Orientations->GetErrorDCMOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get dcm uncertainty of determining the metrology orientation relative to the optical bench
  virtual MVector GetErrorDCMOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_Orientations->GetErrorDCMOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get dcm uncertainty of determining the star tracker orientation relative to the optical bench
  virtual MVector GetErrorDCMOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_Orientations->GetErrorDCMOrientationStarTrackerRelOpticalBench(StarTrackerID); }
  */



  //! Set the satellite pointing at time t
  virtual NPointing GetPointing(const NTime& t) { return m_Pointing->GetPointing(t); }


  // The time interface:

  //! Set the ideal time
  virtual void SetTime(const NTime& Ideal) { m_Time->SetTime(Ideal); }
  //! Advance the ideal time
  virtual void AdvanceTime(const NTime& Time) { m_Time->AdvanceTime(Time); }
  //! Get the ideal time
  virtual NTime GetTime() { return m_Time->GetTime(); }

  //! Get the NuSIM time as time since Epoch
  virtual NTime GetTimeSinceEpoch() { return m_Time->GetTimeSinceEpoch(); }  
  //! Get the absolute time
  virtual NTime GetAbsoluteTime() { return m_Time->GetAbsoluteTime(); }

  //! Set the effective observation time
  virtual void SetEffectiveObservationTime(const NTime& Time) { m_Time->SetEffectiveObservationTime(Time); }
  //! Get the effective observation time
  virtual NTime GetEffectiveObservationTime() { return m_Time->GetEffectiveObservationTime(); }


  //! Set the absolute observation start time
  virtual void SetAbsoluteObservationStartTime(const NTime& Time) { m_Time->SetAbsoluteObservationStartTime(Time); }
  //! Get the absolute observation start time
  virtual NTime GetAbsoluteObservationStartTime() { return m_Time->GetAbsoluteObservationStartTime(); }
  //! Get the epoch observation start time
  virtual NTime GetEpochObservationStartTime() { return m_Time->GetEpochObservationStartTime(); }
  
  //! Set the absolute observation end time
  virtual void SetAbsoluteObservationEndTime(const NTime& Time) { m_Time->SetAbsoluteObservationEndTime(Time); }
  //! Get the absolute observation end time
  virtual NTime GetAbsoluteObservationEndTime() { return m_Time->GetAbsoluteObservationEndTime(); }
  //! Get the epoch observation end time
  virtual NTime GetEpochObservationEndTime() { return m_Time->GetEpochObservationEndTime(); }

  //! Convert from NuSIM time to time since Epoch
  virtual NTime ConvertToTimeSinceEpoch(const NTime& Time) { return m_Time->ConvertToTimeSinceEpoch(Time); }
  //! Convert to NuSIM time from time since Epoch
  virtual NTime ConvertFromTimeSinceEpoch(const NTime& Time) { return m_Time->ConvertFromTimeSinceEpoch(Time); }
  //! Convert from NuSIM time to absolute time
  virtual NTime ConvertToAbsoluteTime(const NTime& Time) { return m_Time->ConvertToAbsoluteTime(Time); }
  //! Convert to NuSIM time from absolute time
  virtual NTime ConvertFromAbsoluteTime(const NTime& Time) { return m_Time->ConvertFromAbsoluteTime(Time); }
  
  

  // The detector and geometry properties interface:

  //! Get the half dimensions of the detectors (active material only)
  virtual MVector GetDetectorHalfDimension() { return m_GeometryAndDetectorProperties->GetDetectorHalfDimension(); }
  //! Get the detector pixels in x-direction (detector coordinate system)
  virtual int GetDetectorPixelsX() { return m_GeometryAndDetectorProperties->GetDetectorPixelsX(); }
  //! Get the detector pixels in y-direction (detector coordinate system)
  virtual int GetDetectorPixelsY() { return m_GeometryAndDetectorProperties->GetDetectorPixelsY(); }
  //! Get the detector pixel size in x-direction (detector coordinate system)
  virtual double GetDetectorAveragePixelSizeX() { return m_GeometryAndDetectorProperties->GetDetectorAveragePixelSizeX(); }
  //! Get the detector pixel size in y-direction (detector coordinate system)
  virtual double GetDetectorAveragePixelSizeY() { return m_GeometryAndDetectorProperties->GetDetectorAveragePixelSizeY(); }

  //! Get the distance to closest neighboring pixel
  virtual double GetDistanceToClostestNeighborPixel(int Telescope, int Detector, MVector Position) { 
    return m_GeometryAndDetectorProperties->GetDistanceToClostestNeighborPixel(Telescope, Detector, Position); }
  //! Create an interaction in the clostest neighboring pixel
  virtual NInteraction CreateInteractionClostestNeighborPixel(int Telescope, int Detector, MVector Position) {
    return m_GeometryAndDetectorProperties->CreateInteractionClostestNeighborPixel(Telescope, Detector, Position); }

  //! Get the radius of a sphere surriounding on optics module
  virtual double GetSurroundingSphereRadius() { return m_GeometryAndDetectorProperties->GetSurroundingSphereRadius(); }
  //! Get the z offset in optics module coordinates the top of the optics module
  virtual double GetSurroundingSphereZOffsetInOpticsModuleCoordinates() { return m_GeometryAndDetectorProperties->GetSurroundingSphereZOffsetInOpticsModuleCoordinates(); }


  //! Return the total CZT absorption coefficient (= macroscopic cross section)
  virtual double GetCZTAbsorptionCoefficient(double Energy) { return m_GeometryAndDetectorProperties->GetCZTAbsorptionCoefficient(Energy); }
  //! Return the photo CZT absorption coefficient (= macroscopic cross section)
  virtual double GetCZTPhotoAbsorptionCoefficient(double Energy) { return m_GeometryAndDetectorProperties->GetCZTPhotoAbsorptionCoefficient(Energy); }
  //! Return the Compton CZT absorption coefficient (= macroscopic cross section)
  virtual double GetCZTComptonAbsorptionCoefficient(double Energy) { return m_GeometryAndDetectorProperties->GetCZTComptonAbsorptionCoefficient(Energy); }
  //! Return the Rayleigh CZT absorption coefficient (= macroscopic cross section)
  virtual double GetCZTRayleighAbsorptionCoefficient(double Energy) { return m_GeometryAndDetectorProperties->GetCZTRayleighAbsorptionCoefficient(Energy); }

  //! Return the total CALIBRATED CZT absorption coefficient (= macroscopic cross section)
  virtual double GetCalibratedBerylliumAbsorptionCoefficient(double Energy) { return m_GeometryAndDetectorProperties->GetCalibratedBerylliumAbsorptionCoefficient(Energy); }
  //! Return the total CZT absorption coefficient (= macroscopic cross section)
  virtual double GetBerylliumAbsorptionCoefficient(double Energy) { return m_GeometryAndDetectorProperties->GetBerylliumAbsorptionCoefficient(Energy); }

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  // Everything here is preliminary
  
  //! The pointing module
  NModuleInterfacePointing* m_Pointing;
  //! The orbit module
  NModuleInterfaceOrbit* m_Orbit;
  //! The orientations module
  NModuleInterfaceOrientations* m_Orientations;
  //! The orientations module
  NModuleInterfaceTime* m_Time;
  //! The detector properties module
  NModuleInterfaceGeometryAndDetectorProperties* m_GeometryAndDetectorProperties;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NSatellite, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
