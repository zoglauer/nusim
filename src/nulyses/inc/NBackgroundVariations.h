/*
 * NBackgroundVariations.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundVariations__
#define __NBackgroundVariations__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include <TH1.h>

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundVariations : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundVariations();
  //! Default destructor
  virtual ~NBackgroundVariations();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NBackgroundVariations() {};
  //NBackgroundVariations(const NBackgroundVariations& NCTHit) {};

  bool Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  double m_LifeTime;
  double m_ActivePixels;
  TH1D* m_SpectrumWafer0;
  TH1D* m_SpectrumWafer1;
  TH1D* m_SpectrumWafer2;
  TH1D* m_SpectrumWafer3;

#ifdef ___CINT___
 public:
  ClassDef(NBackgroundVariations, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
