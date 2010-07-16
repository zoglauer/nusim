/*
 * NModuleInterfaceOrientations.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceOrientations__
#define __NModuleInterfaceOrientations__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NOrientation.h"
#include "NOrientations.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface provides the base functionality for retrieving orientation data for the satellite modules
//! This is a special decorator for satellite modules. It provides function to retrieve orientation data.

class NModuleInterfaceOrientations : public NOrientations
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceOrientations();
  //! Default destructor
  virtual ~NModuleInterfaceOrientations();

  //! Get all orientations at time t
  virtual NOrientations GetAllOrientations(const NTime& t) { DetermineOrientations(t); return *dynamic_cast<NOrientations*>(this); }
  
  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationSpaceCraftRelInertial(const NTime& t) { DetermineOrientations(t); return NOrientations::GetOrientationSpaceCraftRelInertial(); }

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationFocalPlaneRelSC(const NTime& t) { DetermineOrientations(t); return NOrientations::GetOrientationFocalPlaneRelSC(); }

  //! Get the optic bench orientation at time t
  virtual NOrientation GetOrientationOpticalBench(const NTime& t) { DetermineOrientations(t); return NOrientations::GetOrientationOpticalBench(); }

  //! Get the optics orientation at time t                      
  virtual NOrientation GetOrientationOptics(const NTime& t, int OpticsID) { DetermineOrientations(t); return NOrientations::GetOrientationOptics(OpticsID); }

  //! Get the optics orientation realtive to the optical bench at time t                      
  virtual NOrientation GetOrientationOpticsRelOpticalBench(const NTime& t, int OpticsID) { DetermineOrientations(t); return NOrientations::GetOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get the aperture orientation at time t
  virtual NOrientation GetOrientationAperture(const NTime& t, int ApertureID) { DetermineOrientations(t); return NOrientations::GetOrientationAperture(ApertureID); }

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyLaser(const NTime& t, int MetrologyID) { DetermineOrientations(t); return NOrientations::GetOrientationMetrologyLaser(MetrologyID); }

  //! Get the metrology orientation relative to the optical bench at time t
  virtual NOrientation GetOrientationMetrologyLaserRelOpticalBench(const NTime& t, int MetrologyID) { DetermineOrientations(t); return NOrientations::GetOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyDetector(const NTime& t, int MetrologyID) { DetermineOrientations(t); return NOrientations::GetOrientationMetrologyDetector(MetrologyID); }

  //! Get the star tracker orientation at time t
  virtual NOrientation GetOrientationStarTracker(const NTime& t, int StarTrackerID) { DetermineOrientations(t); return NOrientations::GetOrientationStarTracker(StarTrackerID); }

  //! Get the star tracker orientation relative to the optical bench at time t
  virtual NOrientation GetOrientationStarTrackerRelOpticalBench(const NTime& t, int StarTrackerID) { DetermineOrientations(t); return NOrientations::GetOrientationStarTrackerRelOpticalBench(StarTrackerID); }

  //! Get the focal plane detector module orientation at time t
  virtual NOrientation GetOrientationFocalPlaneModule(const NTime& t, int ModuleID) { DetermineOrientations(t); return NOrientations::GetOrientationFocalPlaneModule(ModuleID); }

  //! Get the focal plane detector module - detector orientation at time t
  virtual NOrientation GetOrientationDetectorRelFocalPlaneModule(const NTime& t, int ModuleID, int DetectorID) { DetermineOrientations(t); return NOrientations::GetOrientationDetectorRelFocalPlaneModule(ModuleID, DetectorID); }



	//! Get the calibrated optical axis of optics module (independent of time)
	virtual MVector GetCalibratedPointingOpticalAxisRelOM(int OpticsID);

	//! Get the calibrated pointing direction of the metrology laser (independent of time)
	virtual MVector GetCalibratedPointingMetrologyLaserRelML(int MetrologyID);



  //! Get the CALIBRATED focal plane detector module orientation
  virtual NOrientation GetCalibratedOrientationFocalPlaneModule(int ModuleID) { return m_Calibrations.GetOrientationFocalPlaneModule(ModuleID); }

  //! Get the CALIBRATED metrology detector orientation
  virtual NOrientation GetCalibratedOrientationMetrologyDetector(int MetrologyID) { return m_Calibrations.GetOrientationMetrologyDetector(MetrologyID); }

  //! Get the CALIBRATED optic bench orientation
  virtual NOrientation GetCalibratedOrientationOpticalBench() { return m_Calibrations.GetOrientationOpticalBench(); }

  //! Get the CALIBRATED optics orientation realtive to the optical bench                    
  virtual NOrientation GetCalibratedOrientationOpticsRelOpticalBench(int OpticsID) { return m_Calibrations.GetOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get the CALIBRATED metrology orientation relative to the optical bench
  virtual NOrientation GetCalibratedOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_Calibrations.GetOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get the CALIBRATED star tracker orientation relative to the optical bench
  virtual NOrientation GetCalibratedOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_Calibrations.GetOrientationStarTrackerRelOpticalBench(StarTrackerID); }



	//! Get the centroiding error of the metrology detector
	virtual double GetCentroidingErrorMetrologyDetector(int MetrologyID);

	//! Get the pointing error of the star tracker
	virtual double GetPointingErrorStarTracker(int StarTrackerID);



  //! Get the xyz uncertainty of determining the focal plane detector module orientation
  virtual MVector GetErrorXYZOrientationFocalPlaneModule(int ModuleID) { return m_ErrorsXYZ.GetOrientationFocalPlaneModule(ModuleID).GetTranslation(); }

  //! Get the xyz uncertainty of determining the metrology detector orientation
  virtual MVector GetErrorXYZOrientationMetrologyDetector(int MetrologyID) { return m_ErrorsXYZ.GetOrientationMetrologyDetector(MetrologyID).GetTranslation(); }

  //! Get xyz uncertainty of determining the aperture orientation
  virtual MVector GetErrorXYZOrientationAperture(int ApertureID) { return m_ErrorsXYZ.GetOrientationAperture(ApertureID).GetTranslation(); }

  //! Get xyz uncertainty of determining the optic bench orientation
  virtual MVector GetErrorXYZOrientationOpticalBench() { return m_ErrorsXYZ.GetOrientationOpticalBench().GetTranslation(); }

  //! Get xyz uncertainty of determining the optics orientation realtive to the optical bench                    
  virtual MVector GetErrorXYZOrientationOpticsRelOpticalBench(int OpticsID) { return m_ErrorsXYZ.GetOrientationOpticsRelOpticalBench(OpticsID).GetTranslation(); }

  //! Get xyz uncertainty of determining the metrology orientation relative to the optical bench
  virtual MVector GetErrorXYZOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_ErrorsXYZ.GetOrientationMetrologyLaserRelOpticalBench(MetrologyID).GetTranslation(); }

  //! Get xyz uncertainty of determining the star tracker orientation relative to the optical bench
  virtual MVector GetErrorXYZOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_ErrorsXYZ.GetOrientationStarTrackerRelOpticalBench(StarTrackerID).GetTranslation(); }



  //! Get the dcm uncertainty of determining the focal plane detector module orientation
  virtual MVector GetErrorDCMOrientationFocalPlaneModule(int ModuleID) { return m_ErrorsDCM.GetOrientationFocalPlaneModule(ModuleID).GetTranslation(); }

  //! Get the dcm uncertainty of determining the metrology detector orientation
  virtual MVector GetErrorDCMOrientationMetrologyDetector(int MetrologyID) { return m_ErrorsDCM.GetOrientationMetrologyDetector(MetrologyID).GetTranslation(); }

  //! Get dcm uncertainty of determining the aperture orientation
  virtual MVector GetErrorDCMOrientationAperture(int ApertureID) { return m_ErrorsDCM.GetOrientationAperture(ApertureID).GetTranslation(); }

  //! Get dcm uncertainty of determining the optic bench orientation
  virtual MVector GetErrorDCMOrientationOpticalBench() { return m_ErrorsDCM.GetOrientationOpticalBench().GetTranslation(); }

  //! Get dcm uncertainty of determining the optics orientation realtive to the optical bench                    
  virtual MVector GetErrorDCMOrientationOpticsRelOpticalBench(int OpticsID) { return m_ErrorsDCM.GetOrientationOpticsRelOpticalBench(OpticsID).GetTranslation(); }

  //! Get dcm uncertainty of determining the metrology orientation relative to the optical bench
  virtual MVector GetErrorDCMOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_ErrorsDCM.GetOrientationMetrologyLaserRelOpticalBench(MetrologyID).GetTranslation(); }

  //! Get dcm uncertainty of determining the star tracker orientation relative to the optical bench
  virtual MVector GetErrorDCMOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_ErrorsDCM.GetOrientationStarTrackerRelOpticalBench(StarTrackerID).GetTranslation(); }




  // protected methods:
 protected:
  //! Determine orientations if they are not yet present for the given time t
  //! This function needs to be derived and is empty!
  virtual void DetermineOrientations(const NTime& t) {}

	//! Read the data from file
	bool ReadOrientations(TString FileName);

	
  // private methods:
 private:



  // protected members:
 protected:
  //! The file name
  TString m_FileName;
  //! The data base in full...
  vector<NOrientations> m_Orientations;	 
	 
  //! All calibrated orientations
  NOrientations m_Calibrations;
	 
  //! All orientation errors XYZ - only the vector component is used
  NOrientations m_ErrorsXYZ;
  //! All orientation errors DCM - only the vector component is used to store the DCM errors
  NOrientations m_ErrorsDCM;

	//! The optical axis of optics module 1
	MVector m_CalibratedOpticalAxis1RelOM1;
	//! The optical axis of optics module 2
	MVector m_CalibratedOpticalAxis2RelOM2;
	
	//! The pointing of metrology laser 1
	MVector m_CalibratedPointingMetrologyLaser1RelML1;
	//! The pointing of metrology laser 2
	MVector m_CalibratedPointingMetrologyLaser2RelML2;
	//! The pointing of metrology laser 3
	MVector m_CalibratedPointingMetrologyLaser3RelML3;

	//! The pointing knowledge error of the star tracker
	double m_PointingKnowledgeErrorStarTracker4;
	
	//! The centroiding error of the metrology detector 1
	double m_CentroidingErrorMetrologyDetector1;
	//! The centroiding error of the metrology detector 2
	double m_CentroidingErrorMetrologyDetector2;
	//! The centroiding error of the metrology detector 3
	double m_CentroidingErrorMetrologyDetector3;
	   
	
  // private members:
 private:
	 
	
#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceOrientations, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
