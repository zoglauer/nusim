/*
 * NModuleEventSaver.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleEventSaver
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleEventSaver.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsSaver.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleEventSaver)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleEventSaver::NModuleEventSaver(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceExit(), NModuleInterfaceIO(), NModuleInterfaceEventSaverAscii(Satellite)
{
  // Construct an instance of NModuleEventSaver

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Universal saver";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEventSaver";

  // Set the tool tip text
  m_ToolTip = "This will be a universal event saver, which can be placed anywhere in the photon path.";

  // Set the module type
  m_ModuleType = c_EventSaver;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_OpticsEngine);
  m_ModulePlacements.push_back(c_ApertureEngine);
  m_ModulePlacements.push_back(c_DetectorEngine);
  m_ModulePlacements.push_back(c_DetectorEffectsEngine);
  m_ModulePlacements.push_back(c_TriggerEngine);
  m_ModulePlacements.push_back(c_DetectorCalibrator);
  m_ModulePlacements.push_back(c_EventSelector);
  m_ModulePlacements.push_back(c_ScienceAnalyzer);

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsEventSaverDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiagnosticsEventSaverDummy();

  m_FileName = "Dummy.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleEventSaver::~NModuleEventSaver()
{
  // Delete this instance of NModuleEventSaver
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSaver::Initialize()
{
  // Initialize the module 

  return OpenAsciiFile(m_FileName, m_ASCIIFileVersion, m_ChosenType);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSaver::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  return SaveEventAscii(Event, m_ASCIIFileVersion);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSaver::Finalize()
{
  // Initialize the module 

  return CloseAsciiFile();
}


////////////////////////////////////////////////////////////////////////////////


void NModuleEventSaver::ShowOptionsGUI()
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


bool NModuleEventSaver::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleEventSaver::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", CleanPath(m_FileName));

  return Node;
}


// NModuleEventSaver.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
