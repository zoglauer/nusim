/*
 * NPointSourceFitter.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPointSourceFitter__
#define __NPointSourceFitter__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TVector3.h"
#include "TH2D.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NPointSourceFitter : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NPointSourceFitter();
  //! Default destructor
  virtual ~NPointSourceFitter();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NPointSourceFitter() {};
  //NPointSourceFitter(const NPointSourceFitter& NCTHit) {};

  bool CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H);

  TH2D* Show(TString Title, NFilteredEvents& E, double& SigmaMax, double& Assymetry, double& RAPeak, double& DECPeak);
  
  // private methods:
 private:
   //! Create a vector
   TVector3 Vectorize(double Ra, double Dec) { TVector3 V; V.SetMagThetaPhi(1.0, c_Pi/2.0 - Dec, Ra); return V; }
   


  // protected members:
 protected:


  // private members:
 private:
   //! The output file
   TString m_OutputFile;


#ifdef ___CINT___
 public:
  ClassDef(NPointSourceFitter, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
