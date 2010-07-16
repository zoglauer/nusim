/*
 * NModuleBackgroundSimulatorFromDistributions.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulatorFromDistributions
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulatorFromDistributions.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulatorFromDistributions)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorFromDistributions::NModuleBackgroundSimulatorFromDistributions(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulatorFromDistributions

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Sample from distributions";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulatorFromDistributions";

  // Set the tool tip text
  m_ToolTip = "This module samples the detector background from a distribution."
    "Currently multiple interactions are considered as one at the center of \"energetic-gravity\"."
    "Shield hits are ignored";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulatorFromDistributions)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulatorFromDistributions();
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorFromDistributions::~NModuleBackgroundSimulatorFromDistributions()
{
  // Delete this instance of NModuleBackgroundSimulatorFromDistributions
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorFromDistributions::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  m_Time = m_Satellite.GetTimeIdeal() + 0.5*gRandom->Rndm()+10;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorFromDistributions::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning

  // Delete this????
  // Since this is an entry module, we start our work only if the time <= m_Time:
  if (m_Satellite.GetTimeIdeal() < m_Time) { 
    return true;
  }

  Event.SetOrigin(NEvent::c_OriginBackground);
  Event.SetTelescope((gRandom->Rndm() > 0.5) ? 1 : 2);
  if (Event.GetTelescope() == 1) {
    Event.SetTime(m_Satellite.GetTimeDetector1());
  } else {
    Event.SetTime(m_Satellite.GetTimeDetector2());
  }

  int Detector = gRandom->Integer(4)+1; // [1..4]

  NInteraction I;
  I.SetPosition(MVector(40*(gRandom->Rndm()-0.5), 40*(gRandom->Rndm()-0.5), 2*(gRandom->Rndm()-0.5)));
  I.SetEnergy(75*gRandom->Rndm()+5);


  Event.AddInteraction(I);

  m_Time = m_Satellite.GetTimeIdeal() + 0.5*gRandom->Rndm();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorFromDistributions::ShowOptionsGUI()
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


bool NModuleBackgroundSimulatorFromDistributions::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleBackgroundSimulatorFromDistributions::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleBackgroundSimulatorFromDistributions.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
