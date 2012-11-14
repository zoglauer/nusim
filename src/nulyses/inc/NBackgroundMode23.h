/*
 * NBackgroundMode23.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundMode23__
#define __NBackgroundMode23__


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


class NBackgroundMode23 : public NBackgroundModes
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundMode23();
  //! Default destructor
  virtual ~NBackgroundMode23();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NBackgroundMode23() {};
  //NBackgroundMode23(const NBackgroundMode23& NCTHit) {};

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            NPhaFile& P, int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

  TString m_WafersA;
  TString m_WafersB;

#ifdef ___CINT___
 public:
  ClassDef(NBackgroundMode23, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
