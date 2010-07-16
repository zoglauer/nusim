/*
 * NModuleDetectorEffectsEngineSciSim.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleDetectorEffectsEngineSciSim__
#define __NModuleDetectorEffectsEngineSciSim__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MResponseMatrixO3.h"
#include "MFunction.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleDetectorEffectsEngineSciSim : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleDetectorEffectsEngineSciSim(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleDetectorEffectsEngineSciSim();

  //! Initialize the module
  virtual bool Initialize();

  //! Load the charge loss response
  bool GenerateChargeLossResponse();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

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

  //! The carge loss file
  TString m_ChargeLossFileName;
  //! Input energy for original charge loss calculation
  double m_ChargeLossInputEnergy;
  //! The XY grid points of the cahrge loss data
  vector<double> m_ChargeLossXYGrid;
  //! The z grid points of the charge loss data
  vector<double> m_ChargeLossZGrid;
  //! The charge loss data
  MResponseMatrixO3 m_ChargeLoss;

  //! The energy resolution function
  MFunction m_EnergyResolution; 

  //! Low veto trigger
  double m_LowTrigger;
  //! High veto trigger
  double m_HighTrigger;

  //! The charge sharing distance
  double m_ChargeSharingDistance;

  //! The trigger threshold
  double m_TriggerThreshold;
  
  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorEffectsEngineSciSim, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
