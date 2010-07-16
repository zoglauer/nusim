/*
 * NModuleTriggerEngineIdeal.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleTriggerEngineIdeal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleTriggerEngineIdeal.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleTriggerEngineIdeal)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleTriggerEngineIdeal::NModuleTriggerEngineIdeal(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleTriggerEngineIdeal

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagTriggerEngineIdeal";

  // Set the tool tip text
  m_ToolTip = "This is a trivial trigger scheme.";

  // Set the module type
  m_ModuleType = c_TriggerEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsVetoDeadTimeEngineDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsVetoDeadTimeEngineDummy();
}


////////////////////////////////////////////////////////////////////////////////


NModuleTriggerEngineIdeal::~NModuleTriggerEngineIdeal()
{
  // Delete this instance of NModuleTriggerEngineIdeal
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineIdeal::Initialize()
{
  // Initialize the module 

  m_LowTrigger = 50;
  m_HighTrigger = 5000;
  m_TriggerThreshold = 5;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineIdeal::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Check if a veto or trigger was raised
  
  // Step (1): Check if we have a pixel trigger and then create 9-pixel-hits
  int NTriggers = 0;
  for (unsigned int p = 0; p < Event.GetNPixelHits(); ++p) {
    if (Event.GetPixelHit(p).GetNoisedEnergy() > m_TriggerThreshold) {
      ++NTriggers;
    }
  }
  Event.SetNPixelTriggers(NTriggers);

  // Step (2): Check if a shield veto was raised
  for (unsigned int i = 0; i < Event.GetNShieldHits(); ++i) {
    // Raise veto
    if (Event.GetShieldHit(i).GetIdealEnergy() > m_HighTrigger) {
      Event.SetVetoHigh();
    } else if (Event.GetShieldHit(i).GetIdealEnergy() > m_LowTrigger) {
      Event.SetVetoLow();        
    }
  }
  
  if (Event.GetVeto() == false && Event.GetNPixelHits() == 0) {
    Event.SetBlocked(true);
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleTriggerEngineIdeal::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineIdeal::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode.GetValue();
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleTriggerEngineIdeal::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleTriggerEngineIdeal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
