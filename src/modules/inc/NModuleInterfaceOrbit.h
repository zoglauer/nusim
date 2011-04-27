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

  //! Get the start time of the next black-out
  virtual NTime StartOfNextBlackout(NTime t) { return NTime(0); }
  //! Get the end time of the next black-out
  virtual NTime EndOfNextBlackout(NTime t) { return NTime(0); }
  //! Return true if we are within blackout
  virtual bool IsBlackout(NTime t) { return false; }
  //! Get the number of blackouts in between
  virtual NTime GetBlackoutDuration(NTime t1, NTime t2) { return 0; }

  //! CalculateNextOrbit

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
