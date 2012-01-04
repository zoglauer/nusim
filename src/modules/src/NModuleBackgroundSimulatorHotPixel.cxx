/*
 * NModuleBackgroundSimulatorHotPixel.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulatorHotPixel
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulatorHotPixel.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MResponseMatrixO1.h"

// NuSTAR libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulatorHotPixel)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorHotPixel::NModuleBackgroundSimulatorHotPixel(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulatorHotPixel

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Sub-module for background simulator: data base";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulatorHotPixel";

  // Set the tool tip text
  m_ToolTip = "This module samples the detector background from a events retrieved from a data base.";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulatorHotPixel)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulatorHotPixel();

  m_FileName = "$(NUSIM)/resource/data/Background.HotPixel.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorHotPixel::~NModuleBackgroundSimulatorHotPixel()
{
  // Delete this instance of NModuleBackgroundSimulatorHotPixel
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorHotPixel::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event


  // Load from file
  MFile::ExpandFileName(m_FileName);
  ifstream in;
  in.open(m_FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<m_FileName<<"\""<<endl;
    return false;
  }

  m_HotPixels.clear();
  
  unsigned int Version = 1;
  
  // Read the header
  TString Line;
  while (!in.eof()) {
    Line.ReadToDelim(in, '\n');
    
    if (Line.Length() > 2) {
      if (Line[0] != 'H' || Line[1] != 'P') {
        continue;
      }
      NHotPixel HP;
      HP.SetDetectorBoundaries(MVector(m_Satellite.GetDetectorHalfDimension().X(), 
                                       m_Satellite.GetDetectorHalfDimension().Y(), 
                                       m_Satellite.GetDetectorHalfDimension().Z()));
      if (HP.Parse(Line, Version) == false) {
        mgui<<"Parsing failed: Something is wrong with your calibrated alignments data base!"<<show;
        in.close();
        return false;            
      }
      m_HotPixels.push_back(HP);
    }
  } 
  in.close();

  // Determine the first hot pixel:
  DetermineNext();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorHotPixel::DetermineNext()
{
  // Find the next hot pixel:
  
  m_Time.SetMax();
  for (unsigned int i = 0; i < m_HotPixels.size(); ++i) {
    if (m_HotPixels[i].GetTimeOfNextEvent() < m_Time) {
      m_Time = m_HotPixels[i].GetTimeOfNextEvent();
      m_NextHotPixelID = i;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorHotPixel::PerformTimeJump(const NTime& TimeJump) 
{ 
  //! Perform a time jump:

  for (unsigned int i = 0; i < m_HotPixels.size(); ++i) {
    m_HotPixels[i].PerformTimeJump(TimeJump);
  }
} 


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorHotPixel::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning

  // Find the hot pixel which is next:
  
  Event.SetOrigin(NEvent::c_OriginNoise);

  // Determine a random telescope
  Event.SetTelescope(m_HotPixels[m_NextHotPixelID].GetTelescope());
  Event.SetTime(m_Time);
  
  // GetNextInteraction also advances the hot pixel to the next event
  Event.AddInteraction(m_HotPixels[m_NextHotPixelID].GetNextInteraction());

  // Determine time of next event
  DetermineNext();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorHotPixel::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorHotPixel::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleBackgroundSimulatorHotPixel::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  return Node;
}


// NModuleBackgroundSimulatorHotPixel.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
