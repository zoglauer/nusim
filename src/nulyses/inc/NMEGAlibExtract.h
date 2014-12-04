/*
 * NMEGAlibExtract.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NMEGAlibExtract__
#define __NMEGAlibExtract__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NMEGAlibExtract : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NMEGAlibExtract();
  //! Default destructor
  virtual ~NMEGAlibExtract();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NMEGAlibExtract() {};
  //NMEGAlibExtract(const NMEGAlibExtract& NCTHit) {};

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
  ClassDef(NMEGAlibExtract, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
