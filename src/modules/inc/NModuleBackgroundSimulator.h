/*
 * NModuleBackgroundSimulator.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleBackgroundSimulator__
#define __NModuleBackgroundSimulator__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:
class NModuleBackgroundSimulatorShieldOnly;
class NModuleBackgroundSimulatorDataBase;
class NModuleBackgroundSimulatorAperture;


////////////////////////////////////////////////////////////////////////////////


class NModuleBackgroundSimulator : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleBackgroundSimulator(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleBackgroundSimulator();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  //! Get the file name of the detector data base
  TString GetApertureSpectrum() const { return m_ApertureSpectrum; }
  //! Set the file name of the detector data base
  void SetApertureSpectrum(const TString FileName) { m_ApertureSpectrum = FileName; }

  //! Get the file name of the detector data base
  TString GetApertureFieldOfView() const { return m_ApertureFieldOfView; }
  //! Set the file name of the detector data base
  void SetApertureFieldOfView(const TString FileName) { m_ApertureFieldOfView = FileName; }

  //! Get the file name of the detector data base
  TString GetDetectorDataBase() const { return m_DetectorDataBase; }
  //! Set the file name of the detector data base
  void SetDetectorDataBase(const TString FileName) { m_DetectorDataBase = FileName; }

  //! Get the file name of the shield hit distribution
  TString GetShieldHitSpectrum() const { return m_ShieldHitSpectrum; }
  //! Set the file name of the shield hit distribution
  void SetShieldHitSpectrum(const TString FileName) { m_ShieldHitSpectrum = FileName; }


  // protected methods:
 protected:

  //! Mix some properties of the event, e.g. positions, energies
  void DetermineNext();

  // private methods:
 private:



  // protected members:
 protected:

 // private members:
 private:
  //! The data base part of the simulator (all detector hits with shield contribution)
  NModuleBackgroundSimulatorDataBase* m_DataBase;
  //! The aperture part of the background
  NModuleBackgroundSimulatorAperture* m_Aperture;
  //! The shield only part of the distribution (hits which happen only in the shield)
  NModuleBackgroundSimulatorShieldOnly* m_ShieldOnly;
  
  //! The module which is next:
  NModuleInterfaceEvent* m_NextComponent;

  //! The aperture spectrum file name
  TString m_ApertureSpectrum;
  //! The aperture field-of-view file name
  TString m_ApertureFieldOfView;
  //! The data base file name
  TString m_DetectorDataBase;
  //! The shield hit distribution file name
  TString m_ShieldHitSpectrum;


#ifdef ___CINT___
 public:
  ClassDef(NModuleBackgroundSimulator, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
