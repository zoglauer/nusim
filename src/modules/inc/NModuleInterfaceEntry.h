/*
 * NModuleInterfaceEntry.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceEntry__
#define __NModuleInterfaceEntry__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NModuleInterfaceTimeJump.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for modules which are entry points into the simulations
//! This interface represents an entry point into the simulation/analysis.
//! Its special feature is to store the time when the next event has to be started
//! or the next data set has to be generated, etc.

class NModuleInterfaceEntry : public NModuleInterfaceTimeJump
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEntry();
  //! Default destructor
  virtual ~NModuleInterfaceEntry();

  //! Return the time of the next event
  virtual NTime GetTimeOfNextEvent() { return m_Time; } 

  //! Force the time of the next event
  virtual void ForceTimeOfNextEvent(const NTime& Time) { m_Time = Time; } 

  //! Perform a time jump:
  virtual void PerformTimeJump(const NTime& TimeJump) { m_Time += TimeJump; } 

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The time, when the next event occurs
  NTime m_Time;

  // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEntry, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
