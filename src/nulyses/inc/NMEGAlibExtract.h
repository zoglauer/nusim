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

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! Use this directory
  TString m_OutputDirectory;
  //! Use occulted data?
  bool m_Occulted;

#ifdef ___CINT___
 public:
  ClassDef(NMEGAlibExtract, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
