/*
 * NModuleObservatorySaver.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleObservatorySaver
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleObservatorySaver.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleObservatorySaver)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleObservatorySaver::NModuleObservatorySaver(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceObservatory(), NModuleInterfaceExit()
{
  // Construct an instance of NModuleObservatorySaver

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal observatory data saver";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagObservatorySaver";

  // Set the tool tip text
  m_ToolTip = "This will be a universal observatory data saver, which can be placed anywhere in the observatory path.";

  // Set the module type
  m_ModuleType = c_ObservatorySaver;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_ObservatoryMerger);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsObservatorySaver)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsObservatorySaver();
}


////////////////////////////////////////////////////////////////////////////////


NModuleObservatorySaver::~NModuleObservatorySaver()
{
  // Delete this instance of NModuleObservatorySaver
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatorySaver::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatorySaver::AnalyzeObservatoryData(NObservatoryData& Data) 
{
  // Main data analysis routine, which updates the data to a new level 

  cout<<"Not saving data at time: "<<Data.GetTime()<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleObservatorySaver::ShowOptionsGUI()
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


bool NModuleObservatorySaver::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleObservatorySaver::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleObservatorySaver.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
