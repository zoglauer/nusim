/*
 * NModuleBackgroundSimulator.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulator
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulator.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MFile.h"
#include "MTokenizer.h"

// NuSIM libs:
#include "NGUIOptionsBackgroundSimulator.h"
#include "NModuleBackgroundSimulatorShieldOnly.h"
#include "NModuleBackgroundSimulatorDataBase.h"
#include "NModuleBackgroundSimulatorAperture.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulator)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulator::NModuleBackgroundSimulator(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulator

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "The master background simulator";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulator";

  // Set the tool tip text
  m_ToolTip = "This module samples the detector background from various sources (aperture, shield only, and a data base)";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulator();

  m_ShieldOnly = new NModuleBackgroundSimulatorShieldOnly(Satellite);
  m_Aperture = new NModuleBackgroundSimulatorAperture(Satellite);
  m_DataBase = new NModuleBackgroundSimulatorDataBase(Satellite);  

  m_ApertureSpectrum = "$(NUSIM)/resource/data/Background.ApertureSpectrum.dat";
  m_ApertureFieldOfView = "$(NUSIM)/resource/data/Background.ApertureFieldOfView.dat";
  m_DetectorDataBase = "$(NUSIM)/resource/data/Background.Detector.dat";
  m_ShieldHitSpectrum = "$(NUSIM)/resource/data/Background.Shield.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulator::~NModuleBackgroundSimulator()
{
  // Delete this instance of NModuleBackgroundSimulator
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulator::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event
  
  if (m_ApertureSpectrum == "" && m_ApertureFieldOfView == "") {
    cerr<<"Attention: Background: No aperture files given."<<endl;
    cerr<<"This is OK for testing only!!"<<endl;
  }
  if (m_ApertureSpectrum != "" || m_ApertureFieldOfView != "") {
    if (MFile::Exists(m_ApertureSpectrum) == false) {
      cerr<<"Error: Background: Aperture spectrum file does not exist: "<<m_ApertureSpectrum<<endl;
      return false;
    }
    if (MFile::Exists(m_ApertureFieldOfView) == false) {
      cerr<<"Error: Background: Aperture field-of-view file does not exist: "<<m_ApertureFieldOfView<<endl;
      return false;
    }
  }

  if (m_DetectorDataBase == "") {
    cerr<<"Attention: Background: No detector hit data base file given."<<endl;
    cerr<<"This is OK for testing only."<<endl;
  }
  if (m_DetectorDataBase != "" && MFile::Exists(m_DetectorDataBase) == false) {
    cerr<<"Error: Background: Detector hit data base file does not exist: "<<m_DetectorDataBase<<endl;
    return false;
  }

  if (m_ShieldHitSpectrum == "") {
    cerr<<"Attention: Background: No shield hit file given."<<endl;
    cerr<<"This is OK when you are not interested in shield vetoes (you will get some vetoes from events which also have detector hits)."<<endl;
  }
  if (m_ShieldHitSpectrum != "" && MFile::Exists(m_ShieldHitSpectrum) == false) {
    cerr<<"Error: Background: Shield hit file does not exist: "<<m_ShieldHitSpectrum<<endl;
    return false;
  }

  if (m_ApertureSpectrum != "" || m_ApertureFieldOfView != "") {
    MFile::ExpandFileName(m_ApertureSpectrum);
    m_Aperture->SetFileNameSpectrum(m_ApertureSpectrum);
    MFile::ExpandFileName(m_ApertureFieldOfView);
    m_Aperture->SetFileNameFieldOfView(m_ApertureFieldOfView);
    if (m_Aperture->Initialize() == false) return false;
  }

  if (m_DetectorDataBase != "") {
    MFile::ExpandFileName(m_DetectorDataBase);
    m_DataBase->SetFileName(m_DetectorDataBase);
    if (m_DataBase->Initialize() == false) return false;
  }

  if (m_ShieldHitSpectrum != "") {  
    MFile::ExpandFileName(m_ShieldHitSpectrum);
    m_ShieldOnly->SetFileName(m_ShieldHitSpectrum);
    if (m_ShieldOnly->Initialize() == false) return false;
  }

  DetermineNext();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulator::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning


  bool Return = m_NextComponent->AnalyzeEvent(Event);

  DetermineNext();

  return Return;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulator::DetermineNext()
{
  // Determine which background module is next

  // Determine which module starts:
  m_NextComponent = 0;
  m_Time = numeric_limits<double>::max();
  if (m_ShieldHitSpectrum != "") {  
    if (m_ShieldOnly->GetTimeOfNextEvent() < m_Time) {
      m_Time = m_ShieldOnly->GetTimeOfNextEvent();
      m_NextComponent = m_ShieldOnly;
    }
  }
  if (m_ApertureSpectrum != "" || m_ApertureFieldOfView != "") {
    if (m_Aperture->GetTimeOfNextEvent() < m_Time) {
      m_Time = m_Aperture->GetTimeOfNextEvent();
      m_NextComponent = m_Aperture;
    }
  }
  if (m_DetectorDataBase != "") {
    if (m_DataBase->GetTimeOfNextEvent() < m_Time) {
      m_Time = m_DataBase->GetTimeOfNextEvent();
      m_NextComponent = m_DataBase;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulator::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  NGUIOptionsBackgroundSimulator* Options = new NGUIOptionsBackgroundSimulator(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulator::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* ApertureSpectrumNode = Node->GetNode("ApertureSpectrum");
  if (ApertureSpectrumNode != 0) {
    m_ApertureSpectrum = ApertureSpectrumNode->GetValue();
  }

  MXmlNode* ApertureFieldOfViewNode = Node->GetNode("ApertureFieldOfView");
  if (ApertureFieldOfViewNode != 0) {
    m_ApertureFieldOfView = ApertureFieldOfViewNode->GetValue();
  }

  MXmlNode* DetectorDataBaseNode = Node->GetNode("DetectorDataBase");
  if (DetectorDataBaseNode != 0) {
    m_DetectorDataBase = DetectorDataBaseNode->GetValue();
  }

  MXmlNode* ShieldHitSpectrumNode = Node->GetNode("ShieldHitSpectrum");
  if (ShieldHitSpectrumNode != 0) {
    m_ShieldHitSpectrum = ShieldHitSpectrumNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleBackgroundSimulator::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "ApertureSpectrum", m_ApertureSpectrum);
  new MXmlNode(Node, "ApertureFieldOfView", m_ApertureFieldOfView);
  new MXmlNode(Node, "DetectorDataBase", m_DetectorDataBase);
  new MXmlNode(Node, "ShieldHitSpectrum", m_ShieldHitSpectrum);

  return Node;
}


// NModuleBackgroundSimulator.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
