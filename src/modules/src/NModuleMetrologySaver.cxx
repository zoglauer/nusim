/*
 * NModuleMetrologySaver.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleMetrologySaver
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleMetrologySaver.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsSaver.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleMetrologySaver)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleMetrologySaver::NModuleMetrologySaver(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceMetrology(), NModuleInterfaceExit(), NModuleInterfaceIO()
{
  // Construct an instance of NModuleMetrologySaver

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal metrology data saver";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagMetrologySaver";

  // Set the tool tip text
  m_ToolTip = "This will be a universal metrology data saver, which can be placed anywhere in the metrology path.";

  // Set the module type
  m_ModuleType = c_MetrologySaver;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_MetrologyCalibrator);
  m_ModulePlacements.push_back(c_ObservatoryReconstructor);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsMetrologySaver)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsMetrologySaver();
}


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologySaver::~NModuleMetrologySaver()
{
  // Delete this instance of NModuleMetrologySaver
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologySaver::Initialize()
{
  // Initialize the module 


  m_Out.open(m_FileName);
  if (m_Out.is_open() == false) {
    mgui<<"Unable to open file: "<<m_FileName<<error;
    return false;
  }

  m_Out<<"# NuSIM metrology file"<<endl;
  m_Out<<endl;
  m_Out<<"# Stored after the following module type: "<<endl;
  m_Out<<"CM "<<m_ChosenType<<endl;
  m_Out<<endl;
  m_Out<<"# Start data..."<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologySaver::AnalyzeMetrologyData(NMetrologyData& Data) 
{
  // Main data analysis routine, which updates the data to a new level 

   Data.Stream(m_Out);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologySaver::Finalize()
{
  // Initialize the module 

  if (m_Out.is_open() == true) {
    m_Out<<"EN"<<endl;
    m_Out.close();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleMetrologySaver::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsSaver* Options = new NGUIOptionsSaver(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologySaver::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleMetrologySaver::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);

  return Node;
}


// NModuleMetrologySaver.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
