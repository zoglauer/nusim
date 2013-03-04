/*
 * NBackgroundMode4.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundMode4__
#define __NBackgroundMode4__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH3.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"
#include "NBackgroundModes.h"
#include "NPhaFile.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundMode4 : public NBackgroundModes
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundMode4();
  //! Default destructor
  virtual ~NBackgroundMode4();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NBackgroundMode4() {};
  //NBackgroundMode4(const NBackgroundMode4& NCTHit) {};

  bool Show(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            NPhaFile& P, TH3F* DB, int SourcePosX, int SourcePosY, double DistanceCutOff);
  TH3F* LoadDataBase(TString FileName);
  
  bool SaveAsFits(TH1D* ScaledSpectrum, char Module, int DetectorID, int RawX, int RawY) ;

  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  TString m_DataBaseNameA;
  TString m_DataBaseNameB;
  TH3F* m_DBA;
  TH3F* m_DBB;

  bool m_DumpOneSpectrumPerPixel;

#ifdef ___CINT___
 public:
  ClassDef(NBackgroundMode4, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
