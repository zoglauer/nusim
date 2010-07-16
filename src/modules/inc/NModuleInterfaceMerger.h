/*
 * NModuleInterfaceMerger.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceMerger__
#define __NModuleInterfaceMerger__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is intended for all modules which merge data from one stream into another
//! This object represents an interface for merging alternate data stream into the main
//! data stream assuming that they have different time scales to ensure correct
//! data interpolation.
//! A class deriving from NModuleInterfaceMerger will have some possibility to
//! temporarily store data, from which other data is extracted.
//! The function "HasEnoughData(NTime t)" ensures, that at time t enough data
//! is available.


class NModuleInterfaceMerger
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceMerger();
  //! Default destructor
  virtual ~NModuleInterfaceMerger();

  //! Verify that enough data is available
  //! Has to be overwritte
  virtual bool HasEnoughData(NTime Time) { return false; }

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
  ClassDef(NModuleInterfaceMerger, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
