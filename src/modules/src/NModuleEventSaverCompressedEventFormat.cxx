/*
 * NModuleEventSaverCompressedEventFormat.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleEventSaverCompressedEventFormat
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleEventSaverCompressedEventFormat.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsSaver.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleEventSaverCompressedEventFormat)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleEventSaverCompressedEventFormat::NModuleEventSaverCompressedEventFormat(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceExit(), NModuleInterfaceIO()
{
  // Construct an instance of NModuleEventSaverCompressedEventFormat

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "NuSTAR compressed event format";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEventSaverCompressedEventFormat";

  // Set the tool tip text
  m_ToolTip = "Saves events in the binary \"NuSTAR compressed event format\".";

  // Set the module type
  m_ModuleType = c_EventSaver;

  // Set all places in the pipeline, where this module can be placed
  m_ModulePlacements.push_back(c_DetectorCalibrator);


  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsEventSaverCompressedEventFormatDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsEventSaverCompressedEventFormatDummy();

  m_FileName = "NuSTARCompressedEventFormat.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleEventSaverCompressedEventFormat::~NModuleEventSaverCompressedEventFormat()
{
  // Delete this instance of NModuleEventSaverCompressedEventFormat
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSaverCompressedEventFormat::Initialize()
{
  // Initialize the module 

  MFile::ExpandFileName(m_FileName);
  m_Out.open(m_FileName, ios::binary);
  if (m_Out.is_open() == false) {
    mgui<<"Unable to open file: "<<m_FileName<<error;
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSaverCompressedEventFormat::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Dump the event data into a binary file in the NuSTAR compressed file format here...

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSaverCompressedEventFormat::Finalize()
{
  // Initialize the module 

  m_Out.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleEventSaverCompressedEventFormat::ShowOptionsGUI()
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


bool NModuleEventSaverCompressedEventFormat::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleEventSaverCompressedEventFormat::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);

  return Node;
}


// NModuleEventSaverCompressedEventFormat.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
