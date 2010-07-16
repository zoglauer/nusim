/*
 * NModuleTriggerEnginePHE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleTriggerEnginePHE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleTriggerEnginePHE.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleTriggerEnginePHE)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleTriggerEnginePHE::NModuleTriggerEnginePHE(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleTriggerEnginePHE

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "PHE special";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagTriggerAndPackagingEnginePHE";

  // Set the tool tip text
  m_ToolTip = "This is a specialized trigger engine for the PHE detector effects path";

  // Set the module type
  m_ModuleType = c_TriggerEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsVetoDeadTimeEngineDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsVetoDeadTimeEngineDummy();
}


////////////////////////////////////////////////////////////////////////////////


NModuleTriggerEnginePHE::~NModuleTriggerEnginePHE()
{
  // Delete this instance of NModuleTriggerEnginePHE
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEnginePHE::Initialize()
{
  // Initialize the module 



  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEnginePHE::AnalyzeEvent(NEvent& Event) 
{


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleTriggerEnginePHE::ShowOptionsGUI()
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


bool NModuleTriggerEnginePHE::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleTriggerEnginePHE::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleTriggerEnginePHE.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
