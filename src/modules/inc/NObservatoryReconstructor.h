/*
 * NObservatoryReconstructor.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NObservatoryReconstructor__
#define __NObservatoryReconstructor__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <list>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NStarTrackerData.h"
#include "NMetrologyData.h"
#include "NHit.h"
#include "NObservatoryData.h"
#include "NOrientation.h"
#include "NModuleInterfacePhotonPropagation.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface performs the reconstruction of the pointing.
//! This is a special class, since it needs to be capable to run outside of 
//! NuSIM
//! The required classes for this task are:
//! NOrientation, MVector, NTime, NStarTrackerData, NStarTrackerDataSet,
//! NMetrology, NMetrologyDataSet, NObservatoryData, NHit, NGlobal, TBD.
//! PLUS: ROOT's TString, TRotation, TMatrix, and what ever I forgot.
//!
//! Attention: 
//! Before calling reconstruct all data is required to be set via the
//! Set...()-routines
//! SetCalibratedXXX routines should only be needed to be called once
//! SetTime, SetStarTrackerData, SetMetrologyData should be called per event
//! Reconstruct, should be called per hit
//!
//! A usage example can be found in NModuleObservatoryReconstructor

class NObservatoryReconstructor: public NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NObservatoryReconstructor();
  //! Default destructor
  virtual ~NObservatoryReconstructor();

 
  //! Set the calibrated orientation of the star tracker relative to the optical bench
  void SetCalibratedOrientationStarTrackerRelOpticalBench(const NOrientation& O) { m_CalibratedOrientationStarTrackerRelOpticalBench = O; }
 
  //! Set the calibrated orientation of the optical bench relative to the master frame (focal plane)
  void SetCalibratedOrientationOpticalBench(const NOrientation& O) { m_CalibratedOrientationOpticalBench = O; }
 
  //! Set the calibrated orientation of the optics relative to the optical bench 
  void SetCalibratedOrientationOpticsRelOpticalBench(const NOrientation& O1, const NOrientation& O2) { 
    m_CalibratedOrientationOpticsRelOpticalBench1 = O1; 
    m_CalibratedOrientationOpticsRelOpticalBench2 = O2; 
  }

  //! Set the calibrated bore sight 1 and 2 relative to the optics module
  void SetCalibratedBoreSightRelOM(const MVector& P1, const MVector& P2) {
    m_CalibratedBoreSightRelOM1 = P1;
    m_CalibratedBoreSightRelOM2 = P2;
  }

  //! Set the calibrated orientation of metrology laser 1 and 2 relative to the optical bench
  void SetCalibratedOrientationMetrologyLaserRelOpticalBench(const NOrientation& ML1, const NOrientation& ML2) {
    m_CalibratedOrientationMetrologyLaserRelOpticalBench1 = ML1;
    m_CalibratedOrientationMetrologyLaserRelOpticalBench2 = ML2;
  }

  //! Set the calibrated pointing of metrology laser 1 and 2 relative to the metrology laser reference system
  void SetCalibratedPointingMetrologyLaserRelML(const MVector& ML1, const MVector& ML2) {
    m_CalibratedPointingMetrologyLaserRelML1 = ML1;
    m_CalibratedPointingMetrologyLaserRelML2 = ML2;
  }

  //! Set the calibrated origin of metrology laser 1 and 2 relative to the metrology laser reference system
  void SetCalibratedOriginMetrologyLaserRelML(const MVector& ML1, const MVector& ML2) {
    m_CalibratedOriginMetrologyLaserRelML1 = ML1;
    m_CalibratedOriginMetrologyLaserRelML2 = ML2;
  }

  //! Set the calibrated orientation of metrology detector 1 and 2 relative to the master frame (focal plane)
  void SetCalibratedOrientationMetrologyDetectors(const NOrientation& MD1, const NOrientation& MD2) {
    m_CalibratedOrientationMetrologyDetector1 = MD1;
    m_CalibratedOrientationMetrologyDetector2 = MD2;
  }

  //! Set the calibrated orientation of focal plane module 1 and 2 relative to the master frame (focal plane)
  void SetCalibratedOrientationFocalPlaneModules(const NOrientation& FPM1, const NOrientation& FPM2) {
    m_CalibratedOrientationFocalPlaneModule1 = FPM1;
    m_CalibratedOrientationFocalPlaneModule2 = FPM2;
  }


  //! Set the time for the reconstruction
  void SetTime(const NTime& Time) { m_Time = Time; }

  //! Set upper and lower bound of the star tracker data
  //! For external use, we do not need a fully filled star tracker data set here, just the time & the measured tranfrom ation are required
  void SetStarTrackerData(const NStarTrackerData& LowerBound, const NStarTrackerData& UpperBound) {
    m_StarTrackerDataLowerBound = LowerBound;
    m_StarTrackerDataUpperBound = UpperBound;    
  }

  //! Set upper and lower bound of the metrology data
  //! For external use, we do not need a fully filled metrology data set here, just the time & calibrated laser hit position is required
  void SetMetrologyData(const NMetrologyData& LowerBound, const NMetrologyData& UpperBound) {
    m_MetrologyDataLowerBound = LowerBound;
    m_MetrologyDataUpperBound = UpperBound;       
  }

  //! Reconstruct one Hit, save the info in the observatory data element of the hit
  bool Reconstruct(NHit& Hit);


  // protected methods:
 protected:
  //! The Aspect reconstruction function. Returns the Rfbob, Tfbob
  NOrientation AspectSolve(const MVector& d1f, const MVector& df2);
  //! Find the vector from center of optics to the event interaction on the fb
  MVector FindOmegaF(NOrientation& Rfbob, const int module);
  //! Construct the transformation from focal plane fb to inertial space.
  NOrientation FindRfbin(const NOrientation& Rfbob, const NOrientation& Rstin);
  //! Return the optical axis in inertial coordinates.
  MVector FindOpticalAxisInSky(NOrientation Rstin, const int module);
  //! Return intersection of Optical Axis in detector coordinates
  MVector FindOpticalAxisAtDet(NOrientation Rfbob, NOrientation FP, const int module);


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The calibrated orientation of the optical bench relative to the master frame (focal plane)
  NOrientation m_CalibratedOrientationOpticalBench;

  //! The calibrated orientation of the star tracker relative to the optical bench
  NOrientation m_CalibratedOrientationStarTrackerRelOpticalBench;

  //! The calibrated orientation of the optics 1 relative to the optical bench 
  NOrientation m_CalibratedOrientationOpticsRelOpticalBench1;
  //! The calibrated orientation of the optics 2 relative to the optical bench 
  NOrientation m_CalibratedOrientationOpticsRelOpticalBench2;
  
  //! The calibrated bore sight 1 relative to the optics module
  MVector m_CalibratedBoreSightRelOM1;
  //! The calibrated bore sight 2 relative to the optics module
  MVector m_CalibratedBoreSightRelOM2;
 
  //! The calibrated orientation of metrology laser 1 relative to the optical bench
  NOrientation m_CalibratedOrientationMetrologyLaserRelOpticalBench1;
  //! The calibrated orientation of metrology laser 2 relative to the optical bench
  NOrientation m_CalibratedOrientationMetrologyLaserRelOpticalBench2;

  //! The calibrated pointing of metrology laser 1 relative to the metrology laser reference system
  MVector m_CalibratedPointingMetrologyLaserRelML1;
  //! The calibrated pointing of metrology laser 2 relative to the metrology laser reference system
  MVector m_CalibratedPointingMetrologyLaserRelML2;

  //! The calibrated origin of metrology laser 1 relative to the metrology laser reference system
  MVector m_CalibratedOriginMetrologyLaserRelML1;
  //! The calibrated origin of metrology laser 2 relative to the metrology laser reference system
  MVector m_CalibratedOriginMetrologyLaserRelML2;

  //! The calibrated orientation of the metrology detector 1 relative to the master frame (focal plane)
  NOrientation m_CalibratedOrientationMetrologyDetector1;
  //! The calibrated orientation of the metrology detector 2 relative to the master frame (focal plane)
  NOrientation m_CalibratedOrientationMetrologyDetector2;

  //! The calibrated orientation of the focal plane module 1 relative to the master frame (focal plane)
  NOrientation m_CalibratedOrientationFocalPlaneModule1;
  //! The calibrated orientation of the focal plane module 2 relative to the master frame (focal plane)
  NOrientation m_CalibratedOrientationFocalPlaneModule2;

  //! The time at which to reconstruct the data
  NTime m_Time;

  //! The lower bound of the star tracker data
  NStarTrackerData m_StarTrackerDataLowerBound;
  //! The upper bound of the star tracker data
  NStarTrackerData m_StarTrackerDataUpperBound;

  //! The lower bound of the metrology data
  NMetrologyData m_MetrologyDataLowerBound;
  //! The upper bound of the metrology data
  NMetrologyData m_MetrologyDataUpperBound;


#ifdef ___CINT___
 public:
  ClassDef(NObservatoryReconstructor, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
