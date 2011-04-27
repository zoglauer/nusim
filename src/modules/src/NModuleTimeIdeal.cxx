/*
 * NModuleTimeIdeal.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleTimeIdeal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleTimeIdeal.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleTimeIdeal)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleTimeIdeal::NModuleTimeIdeal(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceTime()
{
  // Construct an instance of NModuleTimeIdeal

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal time engine";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleTimeIdeal";

  // Set the tool tip text
  m_ToolTip = "This module keeps takes care of measured times in the instrument."
    "Since this is an ideal module all times are ideal.";

  // Set all types this modules handles
  m_ModuleType = c_TimeEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsPointing)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsPointing();
}


////////////////////////////////////////////////////////////////////////////////


NModuleTimeIdeal::~NModuleTimeIdeal()
{
  // Delete this instance of NModuleTimeIdeal
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTimeIdeal::Initialize()
{
  // Initialize the module 

  m_Ideal = 0;
  m_EffectiveObservationTime = 0;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleTimeIdeal::GetTimeSatelliteBus() 
{ 
  // Get the time as measured by the instrument
  // Since this is an ideal module - we just return the ideal time

  return m_Ideal; 
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleTimeIdeal::GetTimeDetector1() 
{ 
  // Get the time as measured by detector 1
  // Since this is an ideal module - we just return the ideal time

  return m_Ideal;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleTimeIdeal::GetTimeDetector2() 
{ 
  // Get the time as measured by detector 2
  // Since this is an ideal module - we just return the ideal time

  return m_Ideal; 
}


////////////////////////////////////////////////////////////////////////////////


void NModuleTimeIdeal::ShowOptionsGUI()
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


bool NModuleTimeIdeal::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleTimeIdeal::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleTimeIdeal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
