/*
 * NModuleEventLoader.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleEventLoader
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleEventLoader.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:
#include "NGUIOptionsLoader.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleEventLoader)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleEventLoader::NModuleEventLoader(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry(), NModuleInterfaceIO()
{
  // Construct an instance of NModuleEventLoader

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal loader";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEventLoader";

  // Set the tool tip text
  m_ToolTip = "This will be a universal event loader, which can be placed anywhere in the photon path.";

  // Set the module type
  m_ModuleType = c_EventLoader;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_SourceGenerator);
  m_ModulePlacements.push_back(c_OpticsEngine);
  m_ModulePlacements.push_back(c_ApertureEngine);
  m_ModulePlacements.push_back(c_DetectorEngine);
  m_ModulePlacements.push_back(c_BackgroundEngine);
  m_ModulePlacements.push_back(c_DetectorEffectsEngine);
  m_ModulePlacements.push_back(c_TriggerEngine);
  m_ModulePlacements.push_back(c_DetectorCalibrator);
  m_ModulePlacements.push_back(c_EventSelector);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsEventLoaderDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsEventLoaderDummy();
}


////////////////////////////////////////////////////////////////////////////////


NModuleEventLoader::~NModuleEventLoader()
{
  // Delete this instance of NModuleEventLoader
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventLoader::Initialize()
{
  // Initialize the module 

  MFile::ExpandFileName(m_FileName);
  m_In.open(m_FileName);
  if (m_In.is_open() == false) {
    mgui<<"Event loader: Unable to open file: "<<m_FileName<<error;
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
    } else if (Line.BeginsWith("SE") == true) {
      break;
    }
  }

  if (m_ChosenType == NModule::c_Unknown) {
    mgui<<"Cannot find the original module type for saved events in file : "<<m_FileName<<"!"<<show;
    return false;
  }
  
  // Read first event
  NEvent Dummy;
  AnalyzeEvent(Dummy);

  if (m_In.eof() || m_Event.IsEmpty() == true) {
    mgui<<"Not enough events found in file: "<<m_FileName<<"!"<<show;
    return false;
  }

  cout<<"Time: "<<m_Event.GetTime()<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventLoader::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Copy the current event data to the event
  Event = m_Event;

  // and buffer the next event
  m_Event.Clear();
  TString Line;
  while(!m_In.eof()) {
    Line.ReadLine(m_In);
    if (Line.BeginsWith("SE") == true) {
      break;
    } else if (Line.BeginsWith("EN") == true) {
      // Read to the end
      while (!m_In.eof()) Line.ReadLine(m_In);
      break;
    } else {
      m_Event.Parse(Line);
    }
  }

  m_Time = m_Event.GetTime();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventLoader::Finalize()
{
  // Initialize the module 

  if (m_In.is_open() == true) {
    m_In.close();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleEventLoader::ShowOptionsGUI()
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


bool NModuleEventLoader::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleEventLoader::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);
  

  return Node;
}


// NModuleEventLoader.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
