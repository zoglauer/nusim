/*
 * NModuleOrbitEngineTLE.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOrbitEngineTLE__
#define __NModuleOrbitEngineTLE__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModule.h"
#include "NModuleInterfaceOrbit.h"
#include "NModuleInterfaceIO.h"
#include "NOrbit.h"

// Bryce tool
#include <GUtil.h>
#include <SGP4.h>
#include <OrbitalOccultationIntervals.h>
#include <OrbitalNightIntervals.h>

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModuleOrbitEngineTLE : public NModule, public NModuleInterfaceOrbit, public NModuleInterfaceIO
{
  // public interface:
 public:
  //! Default constructor
  NModuleOrbitEngineTLE(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOrbitEngineTLE();

  //! Initialize the module
  virtual bool Initialize();
  //! Initialize the module
  virtual bool Finalize();

  //! Enable saving the file as fits
  void SetSave(bool Save = true) { m_Save = Save; }
  //! Enable saving the file as fits
  bool GetSave() const { return m_Save; }

  //! Return the orbit position at a given time
  virtual NOrbit GetOrbit(NTime Time);
  
  //! Get the orbit duration
  virtual NTime GetOrbitDuration(NTime Time);

  //! Get the orbit duration
  virtual TString GetTLEFileName() const { return m_TLEFileName; }
  //! Set the orbit duration
  virtual void SetTLEFileName(const TString& FileName) { m_TLEFileName = FileName; }

  
  //! Get the start time of the next black-out
  virtual NTime StartOfNextBlackout(NTime t);
  //! Get the end time of the next black-out
  virtual NTime EndOfNextBlackout(NTime t);
  //! Return true if we are within blackout
  virtual bool IsBlackout(NTime t);
  //! Get the duration of the sum of all blackouts between t1 and t2
  virtual NTime GetBlackoutDuration(NTime t1, NTime t2);
  //! Get the effective observation time between times
  virtual NTime GetEffectiveObservationTime(NTime t1, NTime t2);

  //! Get the start time of the next black-out
  virtual NTime StartOfPreviousBlackout(NTime t);
  //! Get the ideal time as a function of observation time
  virtual NTime FindIdealTime(NTime ObservationTime);
  
  //! Get time of next switch to night
  virtual NTime GetNextEndNightTime(NTime t);
  //! Get time of last switch to night
  virtual NTime GetLastEndNightTime(NTime t);
  //! Get time of next switch to day
  virtual NTime GetNextBeginNightTime(NTime t);
  //! Get time of last switch to day
  virtual NTime GetLastBeginNightTime(NTime t);
  //! Return true if we are within night time
  virtual bool IsNight(NTime t);

  //! Return if we provide day and night cycles
  virtual bool OrbitProvidesDayAndNightCycles() const { return true; }


  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:
  //! Advance the time until "Time" in 1 sec intervals
  bool AdvanceTime(const NTime& Time);
  //! Advance the time by a fixed value (currently one hour)
  bool AdvanceTime();
   
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The TLE file name
  TString m_TLEFileName;
  
  
  //! The SGP4 orbit tracking object
  vecutil::SGP4 m_SGP4;

  //! The accecpted limb angle
  double m_LimbAngle;
  
  //! The maximum time the vector have been calculated
  NTime m_Time;
  
  //! A vector of all "begin night-time" times
  vector<NTime> m_BeginNightTime;
  //! A vector of all "end night-time" times
  vector<NTime> m_EndNightTime;
  
  //! A vector of all "begin occulatation-time" times
  vector<NTime> m_BeginOccultationTime;
  //! A vector of all "end occultation-time" times
  vector<NTime> m_EndOccultationTime;
  
  //! Flag indicating whether of not to save the data
  bool m_Save;
  
  

#ifdef ___CINT___
 public:
  ClassDef(NModuleOrbitEngineTLEPosition, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
