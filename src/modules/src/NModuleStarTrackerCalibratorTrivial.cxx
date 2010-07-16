/*
 * NModuleStarTrackerCalibratorTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleStarTrackerCalibratorTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleStarTrackerCalibratorTrivial.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NModule.h"
#include "NStarTrackerData.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleStarTrackerCalibratorTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleStarTrackerCalibratorTrivial::NModuleStarTrackerCalibratorTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceStarTracker()
{
  // Construct an instance of NModuleStarTrackerCalibratorTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Pass through - placeholder";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagStarTrackerCalibratorTrivial";

  // Set the tool tip text
  m_ToolTip = "This is a trivial star tracker calibrator";

  // Set all types this modules handles
  m_ModuleType = c_StarTrackerCalibrator;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsStarTrackerCalibratorTrivial)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsStarTrackerCalibratorTrivial();
}


////////////////////////////////////////////////////////////////////////////////


NModuleStarTrackerCalibratorTrivial::~NModuleStarTrackerCalibratorTrivial()
{
  // Delete this instance of NModuleStarTrackerCalibratorTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerCalibratorTrivial::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerCalibratorTrivial::AnalyzeStarTrackerData(NStarTrackerData& Data) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Generate the initial star tracker data as measured by 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleStarTrackerCalibratorTrivial::ShowOptionsGUI()
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


bool NModuleStarTrackerCalibratorTrivial::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleStarTrackerCalibratorTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleStarTrackerCalibratorTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
