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

  //! Enable or disable the CZT dead layer
  void SetUseCZTDeadLayer(bool UseCZTDeadLayer) { m_UseCZTDeadLayer = UseCZTDeadLayer; }
  //! Return true if the CZT dead layer is enabled
  bool GetUseCZTDeadLayer() const { return m_UseCZTDeadLayer; }
  
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

  //! The nuabs transmission probabilities
  MFunction m_NuAbs;
  
  // private members:
 private:
   
  //! Use the Beryllium window
  bool m_UseBerylliumWindow;

  //! Use the CZT dead layer
  bool m_UseCZTDeadLayer;

  
  //! Number of photo effects
  unsigned int m_NPhoto;
  //! Number of Compton scatters
  unsigned int m_NCompton;
  //! Number of Rayleigh scatters
  unsigned int m_NRayleigh;
  //! Number of fluorescence photons created
  unsigned int m_NFluorescence;

  //! Number of photons enetering the detectors
  unsigned int m_NPhotonsEnteringDetector;
  
  //! Event has been blocked in beryllium
  unsigned int m_NBlockedBeryllium ;
  //! Event has been blocked at the detector surface
  unsigned int m_NBlockedDetectorSurface;
  //! Event has been blocked at the detector plane, i.e., it hit passive material
  unsigned int m_NBlockedInDetectorPlane;
  //! Event passed through the detector without an interaction
  unsigned int m_NBlockedPassedWithoutInteractionThroughDetector;


#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorSimulatorDetailed, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
