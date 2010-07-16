/*
 * NModuleInterfaceStarTracker.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceStarTracker__
#define __NModuleInterfaceStarTracker__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NStarTrackerData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules in the star tracker pipeline
//! This interface represents the function used for generating/analyzing/etc. star tracker data.
//! It main function is AnalyzeStarTrackerData.

class NModuleInterfaceStarTracker
{
  // public interface:
 public:
  //! Standard constructor
  NModuleInterfaceStarTracker();
  //! Default destructor
  virtual ~NModuleInterfaceStarTracker();

  // Main data analysis routine, which updates the star tracker data
  // Needs to be overwritten!
  virtual bool AnalyzeStarTrackerData(NStarTrackerData& Data) { return false; }

  // protected methods:
 protected:
  //NModuleInterfaceStarTracker() {};
  //NModuleInterfaceStarTracker(const NModuleInterfaceStarTracker& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceStarTracker, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
