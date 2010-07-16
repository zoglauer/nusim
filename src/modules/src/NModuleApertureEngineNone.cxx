/*
 * NModuleApertureEngineNone.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleApertureEngineNone
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleApertureEngineNone.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TMath.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleApertureEngineNone)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleApertureEngineNone::NModuleApertureEngineNone(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleApertureEngineNone

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "No aperture installed...";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagApertureEngineNone";

  // Set the tool tip text
  m_ToolTip = "Use this if no aperture has been installed, e.g. during detector calibrations.";

  // Set all types this modules handles
  m_ModuleType = c_ApertureEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsApertureSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsApertureSimulator();
}


////////////////////////////////////////////////////////////////////////////////


NModuleApertureEngineNone::~NModuleApertureEngineNone()
{
  // Delete this instance of NModuleApertureEngineNone
}


////////////////////////////////////////////////////////////////////////////////


void NModuleApertureEngineNone::ShowOptionsGUI()
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


bool NModuleApertureEngineNone::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleApertureEngineNone::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleApertureEngineNone.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
