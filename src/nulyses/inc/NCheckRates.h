/*
 * NCheckRates.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NCheckRates__
#define __NCheckRates__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:
class TH1D;
class TH2D;

////////////////////////////////////////////////////////////////////////////////


class NCheckRates : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NCheckRates();
  //! Default destructor
  virtual ~NCheckRates();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NCheckRates() {};
  //NCheckRates(const NCheckRates& NCTHit) {};

  bool CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H);

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  void ShowRates(TH1D* Rates, TH1D* SAA, double LifeTime);
  void ShowRates(TH2D* Rates, double LifeTime);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
   //! Write a GTI file
  bool m_WriteGTI;


#ifdef ___CINT___
 public:
  ClassDef(NCheckRates, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
