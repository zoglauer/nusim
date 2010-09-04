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
#include "NAlignmentsDBEntry.h"
#include "NMetrologyDBEntry.h"
#include "NOpticsDBEntry.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface provides the base functionality for retrieving orientation data for the satellite modules
//! This is a special decorator for satellite modules. It provides function to retrieve orientation data.

class NModuleInterfaceOrientations : public NAlignmentsDBEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceOrientations();
  //! Default destructor
  virtual ~NModuleInterfaceOrientations();

  
  // Pertubed 
  
  //! Get all orientations at time t
  virtual NAlignmentsDBEntry GetAllAlignments(const NTime& t) { AdvanceTime(t); return *dynamic_cast<NAlignmentsDBEntry*>(this); }
  
  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationSpaceCraftRelInertial(const NTime& t) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationSpaceCraftRelInertial(); }

  //! Get the optic bench orientation at time t                      
  virtual NOrientation GetOrientationFocalPlaneRelSC(const NTime& t) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationFocalPlaneRelSC(); }

  //! Get the optic bench orientation at time t
  virtual NOrientation GetOrientationOpticalBench(const NTime& t) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationOpticalBench(); }

  //! Get the optics orientation at time t                      
  virtual NOrientation GetOrientationOptics(const NTime& t, int OpticsID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationOptics(OpticsID); }

  //! Get the optics orientation realtive to the optical bench at time t                      
  virtual NOrientation GetOrientationOpticsRelOpticalBench(const NTime& t, int OpticsID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get the aperture orientation at time t
  virtual NOrientation GetOrientationAperture(const NTime& t, int ApertureID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationAperture(ApertureID); }

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyLaser(const NTime& t, int MetrologyID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationMetrologyLaser(MetrologyID); }

  //! Get the metrology orientation relative to the optical bench at time t
  virtual NOrientation GetOrientationMetrologyLaserRelOpticalBench(const NTime& t, int MetrologyID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get the metrology orientation at time t
  virtual NOrientation GetOrientationMetrologyDetector(const NTime& t, int MetrologyID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationMetrologyDetector(MetrologyID); }

  //! Get the star tracker orientation at time t
  virtual NOrientation GetOrientationStarTracker(const NTime& t, int StarTrackerID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationStarTracker(StarTrackerID); }

  //! Get the star tracker orientation relative to the optical bench at time t
  virtual NOrientation GetOrientationStarTrackerRelOpticalBench(const NTime& t, int StarTrackerID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationStarTrackerRelOpticalBench(StarTrackerID); }

  //! Get the focal plane detector module orientation at time t
  virtual NOrientation GetOrientationFocalPlaneModule(const NTime& t, int ModuleID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationFocalPlaneModule(ModuleID); }

  //! Get the focal plane detector module - detector orientation at time t
  virtual NOrientation GetOrientationDetectorRelFocalPlaneModule(const NTime& t, int ModuleID, int DetectorID) { AdvanceTime(t); return m_LatestPerturbedAlignments.GetOrientationDetectorRelFocalPlaneModule(ModuleID, DetectorID); }


  // Calibrated:

  //! Get the CALIBRATED focal plane detector module orientation
  virtual NOrientation GetCalibratedOrientationFocalPlaneModule(int ModuleID) { return m_CalibratedAlignments.GetOrientationFocalPlaneModule(ModuleID); }

  //! Get the CALIBRATED metrology detector orientation
  virtual NOrientation GetCalibratedOrientationMetrologyDetector(int MetrologyID) { return m_CalibratedAlignments.GetOrientationMetrologyDetector(MetrologyID); }

  //! Get the CALIBRATED optic bench orientation
  virtual NOrientation GetCalibratedOrientationOpticalBench() { return m_CalibratedAlignments.GetOrientationOpticalBench(); }

  //! Get the CALIBRATED optics orientation realtive to the optical bench                    
  virtual NOrientation GetCalibratedOrientationOpticsRelOpticalBench(int OpticsID) { return m_CalibratedAlignments.GetOrientationOpticsRelOpticalBench(OpticsID); }

  //! Get the CALIBRATED metrology orientation relative to the optical bench
  virtual NOrientation GetCalibratedOrientationMetrologyLaserRelOpticalBench(int MetrologyID) { return m_CalibratedAlignments.GetOrientationMetrologyLaserRelOpticalBench(MetrologyID); }

  //! Get the CALIBRATED star tracker orientation relative to the optical bench
  virtual NOrientation GetCalibratedOrientationStarTrackerRelOpticalBench(int StarTrackerID) { return m_CalibratedAlignments.GetOrientationStarTrackerRelOpticalBench(StarTrackerID); }


  // Optics:

  //! Get the bore sight of an optics module 
  MVector GetBoreSightRelOM(const NTime& t, int ModuleID) { AdvanceTime(t); return m_LatestOpticsUncertainties.GetBoreSightRelOM(ModuleID); }

  //! Get the CALIBRATED bore sight of an optics module 
  MVector GetCalibratedBoreSightRelOM(int ModuleID) { return m_CalibratedOpticsUncertainties.GetBoreSightRelOM(ModuleID); }



  // Metrology/Star tracker

  //! Get the pointing error of the star tracker
  virtual double GetPointingErrorStarTracker(const NTime& t, int StarTrackerID) { AdvanceTime(t); return m_LatestMetrologyUncertainties.GetPointingErrorStarTracker(StarTrackerID); }
 
  //! Get the origin of the metrology laser (independent of time)
  virtual MVector GetOriginMetrologyLaserRelML(const NTime& t, int MetrologyID) { AdvanceTime(t); return m_LatestMetrologyUncertainties.GetOriginMetrologyLaserRelML(MetrologyID); }
 
  //! Get the pointing direction of the metrology laser (independent of time)
  virtual MVector GetPointingMetrologyLaserRelML(const NTime& t, int MetrologyID) { AdvanceTime(t); return m_LatestMetrologyUncertainties.GetPointingMetrologyLaserRelML(MetrologyID); }

  //! Get the centroiding error of the metrology detector
  virtual double GetCentroidingErrorMetrologyDetector(const NTime& t, int MetrologyID) { AdvanceTime(t); return m_LatestMetrologyUncertainties.GetCentroidingErrorMetrologyDetector(MetrologyID); }

  //! Get the calibrated origin of the metrology laser (independent of time)
  virtual MVector GetCalibratedOriginMetrologyLaserRelML(int MetrologyID) { return m_CalibratedMetrologyUncertainties.GetOriginMetrologyLaserRelML(MetrologyID); }

  //! Get the calibrated pointing direction of the metrology laser (independent of time)
  virtual MVector GetCalibratedPointingMetrologyLaserRelML(int MetrologyID) { return m_CalibratedMetrologyUncertainties.GetPointingMetrologyLaserRelML(MetrologyID); }
  



  // protected methods:
 protected:
  //! Determine orientations if they are not yet present for the given time t
  //! This function needs to be derived and is empty!
  virtual void AdvanceTime(const NTime& t) {}


  // private methods:
 private:



  // protected members:
 protected:
  //! The current (= last calculated) pertubed aligments
  NAlignmentsDBEntry m_LatestPerturbedAlignments;
  //! The current (= last calculated) metrology uncertainties
  NMetrologyDBEntry m_LatestMetrologyUncertainties;
  //! The current (= last calculated) optics uncertainties
  NOpticsDBEntry m_LatestOpticsUncertainties;
  
  //! The current (= last calculated) metrology uncertainties
  NMetrologyDBEntry m_CalibratedMetrologyUncertainties;
  //! The current (= last calculated) optics uncertainties
  NOpticsDBEntry m_CalibratedOpticsUncertainties;
  //! The ground calibrated alignments
  NAlignmentsDBEntry m_CalibratedAlignments;
  
  // private members:
 private:
	 
	
#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceOrientations, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
