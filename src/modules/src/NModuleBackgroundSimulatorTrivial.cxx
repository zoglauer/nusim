/*
 * NModuleBackgroundSimulatorTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulatorTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulatorTrivial.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulatorTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorTrivial::NModuleBackgroundSimulatorTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulatorTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Background simulator trivial";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulatorTrivial";

  // Set the tool tip text
  m_ToolTip = "This is a simple background simulator just generating a flat spectrum at a fixed count rate.";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulatorTrivial)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulatorTrivial();
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorTrivial::~NModuleBackgroundSimulatorTrivial()
{
  // Delete this instance of NModuleBackgroundSimulatorTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorTrivial::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  m_Time = m_Satellite.GetTime() + 0.5*gRandom->Rndm()+10;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorTrivial::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning

  // Since this is an entry module, we start our work only if the time <= m_Time:
  if (m_Satellite.GetTime() < m_Time) { 
    return true;
  }

  Event.SetOrigin(NEvent::c_OriginBackground);
  Event.SetTelescope((gRandom->Rndm() > 0.5) ? 1 : 2);
  Event.SetTime(m_Satellite.GetTime());

  NInteraction I;
  I.SetIsDetector(true);
  I.SetDetector(gRandom->Integer(4)+1);
  I.SetTelescope(Event.GetTelescope());
  I.SetPosition(MVector(2*(gRandom->Rndm()-0.5)*m_Satellite.GetDetectorHalfDimension().X(), 
                        2*(gRandom->Rndm()-0.5)*m_Satellite.GetDetectorHalfDimension().Y(), 
                        2*(gRandom->Rndm()-0.5)*m_Satellite.GetDetectorHalfDimension().Z()));
  I.SetEnergy(75*gRandom->Rndm()+5);

  Event.AddInteraction(I);

  m_Time = m_Satellite.GetTime() + 0.5*gRandom->Rndm();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorTrivial::ShowOptionsGUI()
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


bool NModuleBackgroundSimulatorTrivial::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleBackgroundSimulatorTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleBackgroundSimulatorTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
