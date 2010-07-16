/*
 * NModuleInterfaceMetrology.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceMetrology__
#define __NModuleInterfaceMetrology__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NMetrologyData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules in the metrology pipeline
//! This interface represents the function used for generating/analyzing/etc. metrology data.
//! It main function is AnalyzeMetrologyData.

class NModuleInterfaceMetrology
{
  // public interface:
 public:
  //! Standard constructor
  NModuleInterfaceMetrology();
  //! Default destructor
  virtual ~NModuleInterfaceMetrology();

  // Main data analysis routine, which updates the star tracker data
  // Needs to be overwritten!
  virtual bool AnalyzeMetrologyData(NMetrologyData& Data) { return false; }

  // protected methods:
 protected:
  //NModuleInterfaceMetrology() {};
  //NModuleInterfaceMetrology(const NModuleInterfaceMetrology& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceMetrology, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
