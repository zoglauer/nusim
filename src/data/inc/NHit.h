/*
 * NHit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NHit__
#define __NHit__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "MVector.h"

// NuSTAR libs:
#include "NObservatoryData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a reconstructed hit in the detector associated with energy and position
//! This class represents a reconstructed hit, i.e. a calibrated hit where the 
//! transformation from ADunits and pixels ID present in the NPixelHit to energy and position
//! already took place. It also conatins the respective resolutions.
//! In the analysis chain this event representation is preceeded by NPixelHit objects and succeeded by a list of pixel hits.

class NHit
{
  // public interface:
 public:
  //! Standard constructor
  NHit();
  //! Copy constructor
  NHit(const NHit& Hit) { (*this) = Hit; }
  //! Default destructor
  virtual ~NHit();

  //! Assignment operator
  const NHit& operator=(const NHit& Hit);

  //! Reset all data
  void Clear();

  //! Return true if the hit is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the Telescope ID
  void SetTelescope(int Telescope) { m_Empty = false; m_Telescope = Telescope; }
  //! Return the Telescope ID
  int GetTelescope() const { return m_Telescope; }

  //! Set the Detector ID
  void SetDetector(int Detector) { m_Empty = false; m_Detector = Detector; }
  //! Return the Detector ID
  int GetDetector() const { return m_Detector; }

  //! Set the position of the hit (coordinates tbd)
  void SetPosition(const MVector& Position) { m_Empty = false; m_Position = Position; }
  //! Return the position of the hit (coordinates tbd)
  MVector GetPosition() const { return m_Position; }

  //! Set the position of the hit (coordinates tbd)
  void SetPositionResolution(const MVector& PositionResolution) { m_Empty = false; m_PositionResolution = PositionResolution; }
  //! Return the position of the hit (coordinates tbd)
  MVector GetPositionResolution() const { return m_PositionResolution; }

  //! Set the energy in keV
  void SetEnergy(double Energy) { m_Empty = false; m_Energy = Energy; }
  //! Return the energy in keV
  double GetEnergy() const { return m_Energy; }

  //! Set the energy resolution in keV
  void SetEnergyResolution(double EnergyResolution) { m_Empty = false; m_EnergyResolution = EnergyResolution; }
  //! Return the energy resolution in keV
  double GetEnergyResolution() const { return m_EnergyResolution; }
  
  //! Set the observatory data
  void SetObservatoryData(const NObservatoryData& ObservatoryData) { m_Empty = false; m_ObservatoryData = ObservatoryData; }
  //! Return A COPY of the observatory data
  NObservatoryData GetObservatoryData() { return m_ObservatoryData; }
  //! Return A REFERENCE of the observatory data
  NObservatoryData& GetObservatoryDataRef() { return m_ObservatoryData; }
  
  //! Set the hit flag for bad depth calibration
  void SetBadDepthCalibration(bool BadDepthCalibration) { m_Empty = false; m_BadDepthCalibration = BadDepthCalibration; }
  //! Return the hit flag for bad depth calibration
  bool GetBadDepthCalibration() const { return m_BadDepthCalibration; }
  
  //! Set the depth cut flag of the hit (true: invalid hit, false: valid)
  void SetDepthCut(bool DepthCut) { m_Empty = false; m_DepthCut = DepthCut; }
  //! Return the depth cut flag of the hit
  bool GetDepthCut() const { return m_DepthCut; }
  
  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line);


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! Telescope ID
  int m_Telescope;
  //! Detector ID
  int m_Detector;

  //! Position of the hit
  MVector m_Position;
  //! Position resolutionof the hit
  MVector m_PositionResolution;

  //! Energy of the hit
  double m_Energy;
  //! Energy resolution of the hit
  double m_EnergyResolution;
  
  //! The observatory data
  NObservatoryData m_ObservatoryData;

  //! Hit flag for bad depth calibration
  bool m_BadDepthCalibration;

  //! Depth cut flag
  bool m_DepthCut;

#ifdef ___CINT___
 public:
  ClassDef(NHit, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
