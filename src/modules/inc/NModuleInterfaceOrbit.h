/*
 * NModuleInterfaceOrbit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceOrbit__
#define __NModuleInterfaceOrbit__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NOrbit.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! The class is an orbit decorator for a module.
//! It provides the functions for retrieving all orbit data

class NModuleInterfaceOrbit
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceOrbit();
  //! Default destructor
  virtual ~NModuleInterfaceOrbit();

  //! Get the optics orientation at time t
  virtual NOrbit GetOrbit(NTime t) { return NOrbit(); }

  //! Get the orbit duration
  virtual NTime GetOrbitDuration(NTime Time) const { return NTime(96.3*minute); }

  //! Get the start time of the next black-out
  virtual NTime StartOfNextBlackout(NTime t) { return NTime(0); }
  //! Get the end time of the next black-out
  virtual NTime EndOfNextBlackout(NTime t) { return NTime(0); }
  //! Return true if we are within blackout
  virtual bool IsBlackout(NTime t) { return false; }
  //! Get the number of blackouts in between
  virtual NTime GetBlackoutDuration(NTime t1, NTime t2) { return NTime(0); }
  //! Get the effective observation time between times
  virtual NTime GetEffectiveObservationTime(NTime t1, NTime t2) { return NTime(0); }

  //! Get the ideal time as a function of observation time
  virtual NTime FindIdealTime(NTime ObservationTime) { return 0; }

  //! Get time of next switch to night
  virtual NTime GetNextEndNightTime(NTime t) { return NTime(0); }
  //! Get time of last switch to night
  virtual NTime GetLastEndNightTime(NTime t) { return NTime(0); }
  //! Get time of next switch to day
  virtual NTime GetNextBeginNightTime(NTime t) { return NTime(0); }
  //! Get time of last switch to day
  virtual NTime GetLastBeginNightTime(NTime t) { return NTime(0); }
  //! Return true if we are within night time
  virtual bool IsNight(NTime t) { return false; }

  //! Return if we provide day and night cycles
  virtual bool OrbitProvidesDayAndNightCycles() const { return false; }

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:

  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceOrbit, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
