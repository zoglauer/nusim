/*
 * NModuleInterfaceTime.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceTime__
#define __NModuleInterfaceTime__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief The class is a decorator for a satellite module.
//! This interface class provides the functions for retrieving all times.


class NModuleInterfaceTime
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceTime();
  //! Default destructor
  virtual ~NModuleInterfaceTime();

  //! Set the ideal time
  virtual void SetTimeIdeal(const NTime& Ideal) {}
  //! Advance the ideal time
  virtual void AdvanceTimeIdeal(const NTime& Time) {}

  //! Get the ideal time
  virtual NTime GetTimeIdeal() { return NTime(); }

  //! Get the time as measured by the instrument
  virtual NTime GetTimeSatelliteBus() { return NTime(); }

  //! Get the time as measured by detector 1
  virtual NTime GetTimeDetector1() { return NTime(); }

  //! Get the time as measured by detector 2
  virtual NTime GetTimeDetector2() { return NTime(); }


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
  ClassDef(NModuleInterfaceTime, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
