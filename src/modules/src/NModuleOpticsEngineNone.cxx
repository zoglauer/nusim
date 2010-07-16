/*
 * NModuleOpticsEngineNone.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOpticsEngineNone
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOpticsEngineNone.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TMath.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MFile.h"

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOpticsEngineNone)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleOpticsEngineNone::NModuleOpticsEngineNone(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleOpticsEngineNone

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "No optics installed...";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagOpticsEngineNone";

  // Set the tool tip text
  m_ToolTip = "This optics engine is just a pass through - the event is not modified. Use this for e.g. detector calibrations.";

  // Set all types this modules handles
  m_ModuleType = c_OpticsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsApertureSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsApertureSimulator();
}


////////////////////////////////////////////////////////////////////////////////


NModuleOpticsEngineNone::~NModuleOpticsEngineNone()
{
  // Delete this instance of NModuleOpticsEngineNone
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOpticsEngineNone::ShowOptionsGUI()
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


bool NModuleOpticsEngineNone::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleOpticsEngineNone::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleOpticsEngineNone.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
