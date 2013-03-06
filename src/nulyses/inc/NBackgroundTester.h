/*
 * NBackgroundTester.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundTester__
#define __NBackgroundTester__


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


class NBackgroundTester : public NBackgroundModes
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundTester();
  //! Default destructor
  virtual ~NBackgroundTester();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  bool Compare(NFilteredEvents& FE, NHousekeeping& H, NBadPixel& B,
               int SourcePosX, int SourcePosY, double SourceCutOff,
               int BkgPosX, int BkgPosY, double BkgCutOff);
  
   
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

  vector<TString> m_SBDirectory;
  vector<TString> m_SBModule;
  vector<int> m_SBSourcePosX;
  vector<int> m_SBSourcePosY;
  vector<int> m_SBSourceSize;
  vector<int> m_SBBkgPosX;
  vector<int> m_SBBkgPosY;
  vector<int> m_SBBkgSize;
  
  vector<TString> m_SDDirectory;
  vector<TString> m_SDSourceModule;
  vector<int> m_SDSourcePosX;
  vector<int> m_SDSourcePosY;
  vector<int> m_SDSourceSize;
  vector<TString> m_SDDBName;
  
#ifdef ___CINT___
 public:
  ClassDef(NBackgroundTester, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
