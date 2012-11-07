/*
 * NDetectorHealth.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NDetectorHealth__
#define __NDetectorHealth__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH1.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NDetectorHealth : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NDetectorHealth();
  //! Default destructor
  virtual ~NDetectorHealth();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  bool Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

#ifdef ___CINT___
 public:
  ClassDef(NDetectorHealth, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
