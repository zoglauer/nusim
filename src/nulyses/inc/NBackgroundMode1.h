/*
 * NBackgroundMode1.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundMode1__
#define __NBackgroundMode1__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"
#include "NBackgroundModes.h"
#include "NPhaFile.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundMode1 : public NBackgroundModes
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundMode1();
  //! Default destructor
  virtual ~NBackgroundMode1();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NBackgroundMode1() {};
  //NBackgroundMode1(const NBackgroundMode1& NCTHit) {};

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            NPhaFile& P, int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

  bool m_InclusionMode;

#ifdef ___CINT___
 public:
  ClassDef(NBackgroundMode1, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
