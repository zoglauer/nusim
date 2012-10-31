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
#include "NPhaFile.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundMode4 : public NBaseTool
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
            NPhaFile& P, TH3D* DB, int SourcePosX, int SourcePosY, double DistanceCutOff);
  bool LoadDataBase(TString FileName, TH3D* DB);
  
  
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

  TString m_DataBaseNameA;
  TString m_DataBaseNameB;
  TH3D* m_DBA;
  TH3D* m_DBB;

  

#ifdef ___CINT___
 public:
  ClassDef(NBackgroundMode4, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
