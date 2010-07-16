/*
 * NModuleBackgroundSimulatorShieldOnly.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleBackgroundSimulatorShieldOnly__
#define __NModuleBackgroundSimulatorShieldOnly__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>

// ROOT libs:

// MEGAlib libs:
#include "MFunction.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleBackgroundSimulatorShieldOnly : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleBackgroundSimulatorShieldOnly(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleBackgroundSimulatorShieldOnly();

  void SetFileName(TString FileName) { m_FileName = FileName; }

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

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  //! The name of the file
  TString m_FileName;
  //! The shield spectrum
  MFunction m_ShieldSpectrum;
  //! The TOTAL shield rate (both modules combined)
  double m_ShieldRate;

 // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleBackgroundSimulatorShieldOnly, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
