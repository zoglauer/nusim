/*
 * NModuleSourceDistribution.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleSourceDistribution__
#define __NModuleSourceDistribution__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceEntry.h"
#include "NSource.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////

/**
 * The class is an entry module determining the inital parameters of the
 * event above the optics.
 */

class NModuleSourceDistribution : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleSourceDistribution(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleSourceDistribution();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Finalize the module
  virtual bool Finalize();

  //! Return a reference to the sources
  vector<NSource*>& GetSourcesRef() { return m_Sources; }

  //! Determine which emission is next
  void DetermineNext();
  
  
  //! Set the pointing pattern output file name
  void SetPointingPatternFileName(TString PointingPatternFileName) { m_PointingPatternFileName = PointingPatternFileName; }
  //! Get the pointing pattern output file name
  TString GetPointingPatternFileName() const { return m_PointingPatternFileName; }
  
  //! Set the number of test photons for generating the pointing pattern
  void SetNTestPhotons(int NTestPhotons) { m_NTestPhotons = NTestPhotons; }
  //! Get the number of test photons for generating the pointing pattern
  int GetNTestPhotons() const { return m_NTestPhotons; }
  
  //! Set the distance between pointings in arcmin
  void SetDistanceBetweenPointings(double DistanceBetweenPointings) { m_DistanceBetweenPointings = DistanceBetweenPointings; }
  //! Get the distance between pointings in arcmin
  double GetDistanceBetweenPointings() const { return m_DistanceBetweenPointings; }
  
  //! Set the maximum radial distance from the pointing direction within at least one photon must have it's origin in order to accept the pointing in the pattern
  void SetMaxAllowedDistanceForAcceptance(double MaxAllowedDistanceForAcceptance) { m_MaxAllowedDistanceForAcceptance = MaxAllowedDistanceForAcceptance; }
  //! Get the maximum radial distance from the pointing direction within at least one photon must have it's origin in order to accept the pointing in the pattern
  double GetMaxAllowedDistanceForAcceptance() const { return m_MaxAllowedDistanceForAcceptance; }
 
  //! Generate an optimized pointing pattern
  bool GeneratePointingPattern();
  
  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! A list of all sources
  vector<NSource*> m_Sources;
  //! The source which is next
  int m_NextComponent;

  //! A file name containing a pointing pattern
  TString m_PointingPatternFileName;
  //! The number of test photons for generating the pointing pattern
  int m_NTestPhotons;
  //! The distance between pointings in arcmin
  double m_DistanceBetweenPointings;
  //! The maximum radial distance from the pointing direction within at least one photon must have it's origin in order to accept the pointing in the pattern 
  double m_MaxAllowedDistanceForAcceptance;

  // Diagnostics
  //! Minimum RA of all started photons
  double m_RaMin;
  //! Maximum RA of all started photons
  double m_RaMax;
  //! Minimum DEC of all started photons
  double m_DecMin;
  //! Maximum DEC of all started photons
  double m_DecMax;

#ifdef ___CINT___
 public:
  ClassDef(NModuleSourceDistribution, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
