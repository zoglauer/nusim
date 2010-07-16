/*
 * NModuleInterfaceEvent.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceEvent__
#define __NModuleInterfaceEvent__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NEvent.h"
#include "NSatellite.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules in the event pipeline
//! This interface represents the function used for generating/analyzing/etc. events.
//! It main function is AnalyzeEvent.

class NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEvent();
  //! Default destructor
  virtual ~NModuleInterfaceEvent();

  //! Main data analysis routine, which updates the event to a new level 
  //! Has to be overwritten in derived class
  virtual bool AnalyzeEvent(NEvent& Event) { return false; };


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
  ClassDef(NModuleInterfaceEvent, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
