/*
 * NModuleStarTrackerLoader.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleStarTrackerLoader
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleStarTrackerLoader.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsLoader.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleStarTrackerLoader)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleStarTrackerLoader::NModuleStarTrackerLoader(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceStarTracker(), NModuleInterfaceEntry(), NModuleInterfaceIO()
{
  // Construct an instance of NModuleStarTrackerLoader

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal star tracker data loader";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagStarTrackerLoader";

  // Set the tool tip text
  m_ToolTip = "This will be a universal star tracker loader, which can be placed anywhere in the star tracker path.";

  // Set the module type
  m_ModuleType = c_StarTrackerLoader;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_StarTrackerEngine);
  m_ModulePlacements.push_back(c_StarTrackerCalibrator);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsStarTrackerLoader)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsStarTrackerLoader();
}


////////////////////////////////////////////////////////////////////////////////


NModuleStarTrackerLoader::~NModuleStarTrackerLoader()
{
  // Delete this instance of NModuleStarTrackerLoader
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerLoader::Initialize()
{
  // Initialize the module 

  m_In.open(m_FileName);
  if (m_In.is_open() == false) {
    mgui<<"Unable to open file: "<<m_FileName<<error;
    return false;
  }

  // Read until the first CM
  TString Line;
  while(!m_In.eof()) {
    Line.ReadLine(m_In);
    if (Line.BeginsWith("CM") == true) {
      MTokenizer T(Line);
      if (T.GetNTokens() != 2) {
        mgui<<"Cannot parse file "<<m_FileName<<" correctly: CM-keyword is not OK!"<<show;
        return false;
      }
      m_ChosenType = T.GetTokenAtAsInt(1);
      break;
    }
  }

  if (m_ChosenType == NModule::c_Unknown) {
    mgui<<"Cannot find the original module type for saved events in file : "<<m_FileName<<"!"<<show;
    return false;
  }
  
  // Read first event
  NStarTrackerData Dummy;
  AnalyzeStarTrackerData(Dummy);

  if (m_In.eof() || m_StarTrackerData.IsEmpty() == true) {
    mgui<<"Not enough events found in file: "<<m_FileName<<"!"<<show;
    return false;
  }

  cout<<"Start time star tracker: "<<m_StarTrackerData.GetTime()<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerLoader::AnalyzeStarTrackerData(NStarTrackerData& Data) 
{
  // Main data analysis routine, which updates the data to a new level 

  // Copy the current event data to the event
  Data = m_StarTrackerData;

  // and buffer the next event
  m_StarTrackerData.Clear();
  TString Line;
  while(!m_In.eof()) {
    Line.ReadLine(m_In);
    if (Line.BeginsWith("S4") == true) {
      m_StarTrackerData.Parse(Line);
      break;
    }
  }

  m_Time = m_StarTrackerData.GetTime();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerLoader::Finalize()
{
  // Initialize the module 

  if (m_In.is_open() == true) {
    m_In.close();
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////


void NModuleStarTrackerLoader::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsLoader* Options = new NGUIOptionsLoader(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerLoader::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleStarTrackerLoader::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);
  
  return Node;
}


// NModuleStarTrackerLoader.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
