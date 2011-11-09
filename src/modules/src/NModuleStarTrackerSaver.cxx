/*
 * NModuleStarTrackerSaver.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleStarTrackerSaver
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleStarTrackerSaver.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsSaver.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleStarTrackerSaver)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleStarTrackerSaver::NModuleStarTrackerSaver(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceStarTracker(), NModuleInterfaceExit(), NModuleInterfaceIO()
{
  // Construct an instance of NModuleStarTrackerSaver

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal star tracker data saver";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagStarTrackerSaver";

  // Set the tool tip text
  m_ToolTip = "This will be a universal star tracker data saver, which can be placed anywhere in the star tracker path.";

  // Set the module type
  m_ModuleType = c_StarTrackerSaver;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_StarTrackerCalibrator);
  m_ModulePlacements.push_back(c_ObservatoryReconstructor);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsStarTrackerSaver)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsStarTrackerSaver();
}


////////////////////////////////////////////////////////////////////////////////


NModuleStarTrackerSaver::~NModuleStarTrackerSaver()
{
  // Delete this instance of NModuleStarTrackerSaver
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerSaver::Initialize()
{
  // Initialize the module 


  m_Out.open(m_FileName);
  if (m_Out.is_open() == false) {
    mgui<<"Unable to open file: "<<m_FileName<<error;
    return false;
  }

  m_Out<<"# NuSIM star tracker file"<<endl;
  m_Out<<endl;
  m_Out<<"# NuSIM version:"<<endl;
  m_Out<<"VE "<<g_Version<<endl;
  m_Out<<"# NuSIM revision:"<<endl;
  m_Out<<"RV "<<g_SVNRevision<<endl;
  m_Out<<"# ASCII file version:"<<endl;
  m_Out<<"VF "<<m_ASCIIFileVersion<<endl;
  m_Out<<endl;
  m_Out<<"# Stored after the following module type: "<<endl;
  m_Out<<"CM "<<m_ChosenType<<endl;
  m_Out<<endl;
  m_Out<<"# Start data..."<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerSaver::AnalyzeStarTrackerData(NStarTrackerData& Data) 
{
  // Main data analysis routine, which updates the data to a new level 

  Data.Stream(m_Out, m_ASCIIFileVersion);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerSaver::Finalize()
{
  // Initialize the module 

  if (m_Out.is_open() == true) {
    m_Out<<"EN"<<endl;
    m_Out.close();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleStarTrackerSaver::ShowOptionsGUI()
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


bool NModuleStarTrackerSaver::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleStarTrackerSaver::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);

  return Node;
}


// NModuleStarTrackerSaver.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
