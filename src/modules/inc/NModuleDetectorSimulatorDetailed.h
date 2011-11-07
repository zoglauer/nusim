/*
 * NModuleDetectorSimulatorDetailed.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleDetectorSimulatorDetailed__
#define __NModuleDetectorSimulatorDetailed__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MFunction.h"

// NuSIM lib:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfacePhotonPropagation.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleDetectorSimulatorDetailed : public NModule, public NModuleInterfaceEvent, public NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NModuleDetectorSimulatorDetailed(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleDetectorSimulatorDetailed();
  
  //! Enable or disable the Beryllium window
  void SetUseBerylliumWindow(bool UseBerylliumWindow) { m_UseBerylliumWindow = UseBerylliumWindow; }
  //! Return true if the Beryllium window is enabled
  bool GetUseBerylliumWindow() const { return m_UseBerylliumWindow; }
  
  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Finalize the analysis
  virtual bool Finalize();

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:
  //! Simulate the photon
  bool SimulatePhoton(NPhoton& P, NEvent& Event);

  //! Simulate Compton scattering
  bool SimulateCompton(NPhoton& P, NEvent& Event);
  //! Simulate photo electric effect
  bool SimulatePhoto(NPhoton& P, NEvent& Event);
  //! Simulate Rayleigh scattering
  bool SimulateRayleigh(NPhoton& P, NEvent& Event);


  // private methods:
 private:



  // protected members:
 protected:
  vector<MFunction> m_Energies;
  vector<MFunction> m_Multiplicities;
  vector<MFunction> m_RayleighAngles;


  // private members:
 private:
  // Statistics
  int m_NPhoto;
  int m_NCompton;
  int m_NRayleigh;
  int m_NFluorescence;

  //! Use the Beryllium window
  bool m_UseBerylliumWindow;
  

#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorSimulatorDetailed, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
