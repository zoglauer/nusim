/*
 * NModuleBackgroundSimulatorShieldOnly.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulatorShieldOnly
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulatorShieldOnly.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MResponseMatrixO1.h"

// NuSTAR libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulatorShieldOnly)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorShieldOnly::NModuleBackgroundSimulatorShieldOnly(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulatorShieldOnly

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Sub-module for background simulator: data base";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulatorShieldOnly";

  // Set the tool tip text
  m_ToolTip = "This module samples the detector background from a events retrieved from a data base.";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulatorShieldOnly)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulatorShieldOnly();

  m_FileName = "$(NUSIM)/resource/data/Background.Shield.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorShieldOnly::~NModuleBackgroundSimulatorShieldOnly()
{
  // Delete this instance of NModuleBackgroundSimulatorShieldOnly
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorShieldOnly::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  MResponseMatrixO1 Data;
  Data.Read(m_FileName);

  cout<<"File name: "<<m_FileName<<endl;

  // Determine the total count rate:
  m_ShieldRate = 0.0;
  for (unsigned int a = 0; a < Data.GetNBins(); ++a) {
    m_ShieldRate += Data.GetBinContent(a);
  }

  // Normalize from cts/sec to counts/sec/keV
  for (unsigned int a = 0; a < Data.GetNBins(); ++a) {
    Data.SetBinContent(a, Data.GetBinContent(a)/Data.GetBinArea(a));
  }

  // Convert to MFunction to for random simulation
  m_ShieldSpectrum.Set(Data);

  // (Random) time of next event
  m_Time = gRandom->Exp(1.0/m_ShieldRate);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorShieldOnly::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning

  Event.SetOrigin(NEvent::c_OriginBackground);

  // Determine a random telescope
  Event.SetTelescope(gRandom->Integer(2)+1);

  Event.SetTime(m_Satellite.GetTime());

  NInteraction I;
  I.SetIsDetector(false);
  I.SetTelescope(Event.GetTelescope());
  I.SetEnergy(m_ShieldSpectrum.GetRandom());

  Event.AddInteraction(I);

  // Determine time of next event
  m_Time += gRandom->Exp(1.0/m_ShieldRate);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorShieldOnly::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorShieldOnly::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleBackgroundSimulatorShieldOnly::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  return Node;
}


// NModuleBackgroundSimulatorShieldOnly.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
