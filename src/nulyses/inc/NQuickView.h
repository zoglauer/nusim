/*
 * NQuickView.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NQuickView__
#define __NQuickView__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH1.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NQuickView : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NQuickView();
  //! Default destructor
  virtual ~NQuickView();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NQuickView() {};
  //NQuickView(const NQuickView& NCTHit) {};

  bool CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H);

  bool Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
   //! Write a GTI file
  bool m_WriteGTI;

  TH1D* m_MergedSpectrum;
  double m_MergedSpectrumLifetime;

#ifdef ___CINT___
 public:
  ClassDef(NQuickView, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
