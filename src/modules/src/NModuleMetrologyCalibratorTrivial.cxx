/*
 * NModuleMetrologyCalibratorTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleMetrologyCalibratorTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleMetrologyCalibratorTrivial.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NModule.h"
#include "NMetrologyData.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleMetrologyCalibratorTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyCalibratorTrivial::NModuleMetrologyCalibratorTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceMetrology()
{
  // Construct an instance of NModuleMetrologyCalibratorTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Trivial";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagMetrologyCalibratorTrivial";

  // Set the tool tip text
  m_ToolTip = "This is a trivial star tracker calibrator.";

  // Set all types this modules handles
  m_ModuleType = c_MetrologyCalibrator;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsMetrologyCalibratorTrivial)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsMetrologyCalibratorTrivial();
}


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyCalibratorTrivial::~NModuleMetrologyCalibratorTrivial()
{
  // Delete this instance of NModuleMetrologyCalibratorTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyCalibratorTrivial::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyCalibratorTrivial::AnalyzeMetrologyData(NMetrologyData& Data) 
{
  // Main data analysis routine, which updates the data to a new level 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleMetrologyCalibratorTrivial::ShowOptionsGUI()
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


bool NModuleMetrologyCalibratorTrivial::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleMetrologyCalibratorTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleMetrologyCalibratorTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
