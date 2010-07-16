/*
 * NModuleTemplate.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleTemplate
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleTemplate.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleTemplate)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleTemplate::NModuleTemplate(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleTemplate

  // Set all module relevant information

  // Set the module name --- make sure that "ModuleTypeName + m_Name" is unique
  m_Name = "Template";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagTemplate";

  // Set the tool tip text
  m_ToolTip = "This is the module template.";

  // Set the module type
  m_ModuleType = c_Unknown;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsTemplate)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsTemplate();

  // Add additional initializations here:
}


////////////////////////////////////////////////////////////////////////////////


NModuleTemplate::~NModuleTemplate()
{
  // Delete this instance of NModuleTemplate
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTemplate::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTemplate::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleTemplate::ShowOptionsGUI()
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


bool NModuleTemplate::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode->GetValue();
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleTemplate::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleTemplate.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
