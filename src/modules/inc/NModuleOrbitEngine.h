/*
 * NModuleOrbitEngine.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOrbitEngine__
#define __NModuleOrbitEngine__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModule.h"
#include "NModuleInterfaceOrbit.h"
#include "NOrbit.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModuleOrbitEngine : public NModule, public NModuleInterfaceOrbit
{
  // public interface:
 public:
  //! Default constructor
  NModuleOrbitEngine(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOrbitEngine();

  //! Initialize the module
  virtual bool Initialize();

  //! Return the orbit position at a given time
  virtual NOrbit GetOrbit(NTime Time);

  //! Get the orbit duration
  virtual NTime GetOrbitDuration() const { return m_OrbitDuration; }
  //! Set the orbit duration
  virtual void SetOrbitDuration(const NTime& Time) { m_OrbitDuration = Time; }

  //! Get the blackout duration
  virtual NTime GetBlackoutDuration() const { return m_BlackoutDuration; }
  //! Set the blackout duration
  virtual void SetBlackoutDuration(const NTime& Time) { m_BlackoutDuration = Time; }

  
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

  //! Return the number of orbit chnages:
  virtual int GetNOrbitChanges(NTime t1, NTime t2);
  //! Get the start time of the next black-out
  virtual NTime StartOfPreviousBlackout(NTime t) { return StartOfNextBlackout(t) - m_OrbitDuration; }
  //! Get the ideal time as a function of observation time
  virtual NTime FindIdealTime(NTime ObservationTime);
  


  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The duration of the current orbit
  NTime m_OrbitDuration;
  //! The blackout time
  NTime m_BlackoutDuration;




#ifdef ___CINT___
 public:
  ClassDef(NModuleOrbitEnginePosition, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
