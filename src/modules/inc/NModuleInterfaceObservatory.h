/*
 * NModuleInterfaceObservatory.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceObservatory__
#define __NModuleInterfaceObservatory__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NObservatoryData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules in the observatory pipeline
//! This interface represents the function used for generating/analyzing/etc. observatory data.
//! It main function is AnalyzeObservatoryData.

class NModuleInterfaceObservatory
{
  // public interface:
 public:
  //! Standard constructor
  NModuleInterfaceObservatory();
  //! Default destructor
  virtual ~NModuleInterfaceObservatory();

  // Main data analysis routine, which updates the observatory data
  // Needs to be overwritten!
  virtual bool AnalyzeObservatoryData(NObservatoryData& Data) { return false; }

  // protected methods:
 protected:
  //NModuleInterfaceObservatory() {};
  //NModuleInterfaceObservatory(const NModuleInterfaceObservatory& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceObservatory, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
