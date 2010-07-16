/*
 * NModuleMetrologyLoader.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleMetrologyLoader
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleMetrologyLoader.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsLoader.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleMetrologyLoader)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleMetrologyLoader::NModuleMetrologyLoader(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceMetrology(), NModuleInterfaceEntry(), NModuleInterfaceIO()
{
  // Construct an instance of NModuleMetrologyLoader

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal metrology data loader";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagMetrologyLoader";

  // Set the tool tip text
  m_ToolTip = "This will be a universal metrology loader, which can be placed anywhere in the metrology path.";

  // Set the module type
  m_ModuleType = c_MetrologyLoader;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_MetrologyEngine);
  m_ModulePlacements.push_back(c_MetrologyCalibrator);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsMetrologyLoader)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsMetrologyLoader();
}


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyLoader::~NModuleMetrologyLoader()
{
  // Delete this instance of NModuleMetrologyLoader
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyLoader::Initialize()
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
    } else if (Line.BeginsWith("ME") == true) {
      break;
    }
  }

  if (m_ChosenType == NModule::c_Unknown) {
    mgui<<"Cannot find the original module type for saved events in file : "<<m_FileName<<"!"<<show;
    return false;
  }
  
  // Read first event
  NMetrologyData Dummy;
  AnalyzeMetrologyData(Dummy);

  if (m_In.eof() || m_MetrologyData.IsEmpty() == true) {
    mgui<<"Not enough events found in file: "<<m_FileName<<"!"<<show;
    return false;
  }

  cout<<"Start time metrology: "<<m_MetrologyData.GetTime()<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyLoader::AnalyzeMetrologyData(NMetrologyData& Data) 
{
  // Main data analysis routine, which updates the data to a new level 

  // Copy the current event data to the event
  Data = m_MetrologyData;

  // and buffer the next event
  m_MetrologyData.Clear();
  TString Line;
  while(!m_In.eof()) {
    Line.ReadLine(m_In);
    if (Line.BeginsWith("ME") == true && Line.BeginsWith("ME EN") == false) {
      break;
    } else if (Line.BeginsWith("SE") == true) {
      // Read to the end
      while (!m_In.eof()) Line.ReadLine(m_In);
      break;
    } else {
      m_MetrologyData.Parse(Line);
    }
  }

  m_Time = m_MetrologyData.GetTime();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyLoader::Finalize()
{
  // Initialize the module 

  if (m_In.is_open() == true) {
    m_In.close();
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////


void NModuleMetrologyLoader::ShowOptionsGUI()
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


bool NModuleMetrologyLoader::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleMetrologyLoader::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);
  
  return Node;
}


// NModuleMetrologyLoader.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
