/*
 * NModuleTimeEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleTimeEngine
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleTimeEngine.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsTimeEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleTimeEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleTimeEngine::NModuleTimeEngine(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceTime()
{
  // Construct an instance of NModuleTimeEngine

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal time engine";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleTimeIdeal";

  // Set the tool tip text
  m_ToolTip = "This module keeps takes care of measured times in the instrument."
    "The times are ideal, i.e. not noised.";

  // Set all types this modules handles
  m_ModuleType = c_TimeEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsPointing)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiagnosticsPointing();

  m_AbsoluteObservationStartTime.Set(2012, 2, 3, 18, 12, 30);
}


////////////////////////////////////////////////////////////////////////////////


NModuleTimeEngine::~NModuleTimeEngine()
{
  // Delete this instance of NModuleTimeEngine
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTimeEngine::Initialize()
{
  // Initialize the module 

  m_Time = 0;
  m_EffectiveObservationTime = 0;
  // m_AbsoluteObservationEndTime is set by the supervisor
  m_AbsoluteObservationEndTime = 0;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleTimeEngine::ShowOptionsGUI()
{
  //! Show the options GUI 

  NGUIOptionsTimeEngine* Options = new NGUIOptionsTimeEngine(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTimeEngine::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* AbsoluteObservationStartTimeNode = Node->GetNode("AbsoluteObservationStartTimeSeconds");
  if (AbsoluteObservationStartTimeNode != 0) {
    m_AbsoluteObservationStartTime.Set(AbsoluteObservationStartTimeNode->GetValueAsLong(), 0);
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleTimeEngine::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "AbsoluteObservationStartTimeSeconds", (long) m_AbsoluteObservationStartTime.GetAsSeconds());

  return Node;
}


// NModuleTimeEngine.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
