/*
 * NModuleImageDeconvolver.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleImageDeconvolver
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleImageDeconvolver.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleImageDeconvolver)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleImageDeconvolver::NModuleImageDeconvolver(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleImageDeconvolver

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Image deconvolver";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDeconvolverDummy";

  // Set the tool tip text
  m_ToolTip = "On  time in the far future this module will allow to perform image deconvolutions."
    "based on some fancy list-mode algorithm.";

  // Set all modules, which have to be done before this module
  m_ModuleType = c_ScienceAnalyzer;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsDeconvolverDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsDeconvolverDummy();
}


////////////////////////////////////////////////////////////////////////////////


NModuleImageDeconvolver::~NModuleImageDeconvolver()
{
  // Delete this instance of NModuleImageDeconvolver
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleImageDeconvolver::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleImageDeconvolver::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleImageDeconvolver::ShowOptionsGUI()
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


bool NModuleImageDeconvolver::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleImageDeconvolver::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleImageDeconvolver.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
