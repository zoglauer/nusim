/*
 * NBackgroundMode123.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundMode123__
#define __NBackgroundMode123__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"
#include "NPhaFile.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundMode123 : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundMode123();
  //! Default destructor
  virtual ~NBackgroundMode123();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NBackgroundMode123() {};
  //NBackgroundMode123(const NBackgroundMode123& NCTHit) {};

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            NPhaFile& P, int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  vector<TString> m_PhaA;
  vector<TString> m_PhaB;
  
  vector<TString> m_RegA;
  vector<TString> m_RegB;

  bool m_InclusionMode;

#ifdef ___CINT___
 public:
  ClassDef(NBackgroundMode123, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
