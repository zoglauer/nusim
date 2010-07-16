/*
 * NModuleDetectorSimulatorGeant4.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorSimulatorGeant4
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorSimulatorGeant4.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NInteraction.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorSimulatorGeant4)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorGeant4::NModuleDetectorSimulatorGeant4(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorSimulatorGeant4

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Geant4";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorSimulatorGeant4";

  // Set the tool tip text
  m_ToolTip = "I am still sure that at one point in time we need to compare the \"simple\" detector simulator output"
    "with a full Geant4 simulation --- that's the place were to put it...";

  // Set the module type
  m_ModuleType = c_DetectorEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsDetectorSimulatorGeant4)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsDetectorSimulatorGeant4();
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorGeant4::~NModuleDetectorSimulatorGeant4()
{
  // Delete this instance of NModuleDetectorSimulatorGeant4
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorGeant4::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorGeant4::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Generate interactions from the photon
  if (fabs(Event.GetCurrentPhoton().GetPosition().X()) < 4 &&
      fabs(Event.GetCurrentPhoton().GetPosition().Y()) < 4) {
    NInteraction I;
    I.SetPosition(Event.GetCurrentPhoton().GetPosition());
    I.SetEnergy(Event.GetCurrentPhoton().GetEnergy());

    Event.AddInteraction(I);
  }


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorSimulatorGeant4::ShowOptionsGUI()
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


bool NModuleDetectorSimulatorGeant4::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleDetectorSimulatorGeant4::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorSimulatorGeant4.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
