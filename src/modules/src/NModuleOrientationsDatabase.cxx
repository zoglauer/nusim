/*
 * NModuleOrientationsDatabase.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOrientationsDatabase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOrientationsDatabase.h"

// Standard libs:
#include <fstream>
#include <iostream>
using namespace std;

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsOrientationsDatabase.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOrientationsDatabase)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleOrientationsDatabase::NModuleOrientationsDatabase(NSatellite& Satellite) : NModuleOrientationsIdeal(Satellite)
{
  // Construct an instance of NModuleOrientationsDatabase

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Database";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleOrientationsDatabase";

  // Set the tool tip text
  m_ToolTip = "This module determines the orientations and alignments of optics, apertiure, and metrology\n"
    "However, in this special version everything is read in from a database file.";

  // Set all types this modules handles
  m_ModuleType = c_OrientationsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsJitter)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsJitter();
}


////////////////////////////////////////////////////////////////////////////////


NModuleOrientationsDatabase::~NModuleOrientationsDatabase()
{
  // Delete this instance of NModuleOrientationsDatabase
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::Initialize()
{
  // Initialize the module 

  if (LoadDefaultValues() == false) return false;

  // Load the data base
  if (ReadOrientations(m_FileName) == false) return false;

  if (m_Orientations.size() <= 2) {
    cout<<"NModuleOrientationsDatabase: Not enough orientations found ("<<m_Orientations.size()<<") in "<<m_FileName<<" ... I want at least 3..."<<endl;
    return false;
  }
  m_StartIndex = 0;
  m_TimeWrap = 0;

  return true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsDatabase::DetermineOrientations(const NTime& t)
{
  // Determine orientations if they are not yet present for the given time t
  // All are relative to the focal plane coordinate system
  // Remember distance are in mm!
  
  // Don't do anything if we are at the same time...
  if (m_Time == t) return;

  if (t.GetSeconds() >= 0) {
    if (m_Orientations[m_StartIndex].GetTime() + m_TimeWrap > t) {
      m_StartIndex = 0;
      while (m_Orientations[m_StartIndex].GetTime() + m_TimeWrap > t) {
        m_TimeWrap -= m_Orientations.back().GetTime();
      }
    }

    unsigned int NextIndex = 0;
    do {
      NextIndex = m_StartIndex + 1;
      if (NextIndex >= m_Orientations.size()) {
        NextIndex = 0;
        m_TimeWrap += m_Orientations.back().GetTime();
      }
      if (m_Orientations[NextIndex].GetTime()+m_TimeWrap > t) {
        break;
      } else {
        m_StartIndex = NextIndex;      
      }
    } while (true);
  } else {
    // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
    m_StartIndex = 0;
  }
  //cout<<"Time: "<<t<<" index: "<<m_StartIndex<<endl;

  // Copy the data - idealy we should interpolate 
  m_SpaceCraftRelInertial = m_Orientations[m_StartIndex].GetOrientationSpaceCraftRelInertial();
  m_FocalPlaneRelSC = m_Orientations[m_StartIndex].GetOrientationFocalPlaneRelSC();
  m_OpticalBench = m_Orientations[m_StartIndex].GetOrientationOpticalBench();
  m_Optics1RelOB = m_Orientations[m_StartIndex].GetOrientationOpticsRelOpticalBench(1);
  m_Optics2RelOB = m_Orientations[m_StartIndex].GetOrientationOpticsRelOpticalBench(2);
  m_Aperture1 = m_Orientations[m_StartIndex].GetOrientationAperture(1);
  m_Aperture2 = m_Orientations[m_StartIndex].GetOrientationAperture(2);
  m_MetrologyLaser1RelOB = m_Orientations[m_StartIndex].GetOrientationMetrologyLaserRelOpticalBench(1);
  m_MetrologyDetector1 = m_Orientations[m_StartIndex].GetOrientationMetrologyDetector(1);
  m_MetrologyLaser2RelOB = m_Orientations[m_StartIndex].GetOrientationMetrologyLaserRelOpticalBench(2);
  m_MetrologyDetector2 = m_Orientations[m_StartIndex].GetOrientationMetrologyDetector(2);
  m_MetrologyLaser3RelOB = m_Orientations[m_StartIndex].GetOrientationMetrologyLaserRelOpticalBench(3);
  m_MetrologyDetector3 = m_Orientations[m_StartIndex].GetOrientationMetrologyDetector(3);
  m_StarTracker4RelOB = m_Orientations[m_StartIndex].GetOrientationStarTrackerRelOpticalBench(4);
  m_FocalPlaneModule1 = m_Orientations[m_StartIndex].GetOrientationFocalPlaneModule(1);
  m_FocalPlaneModule2 = m_Orientations[m_StartIndex].GetOrientationFocalPlaneModule(2);

  m_Time = t;

  /*
  // In order to have some fun, we let the thing rotate as a function of time:
  double s = t.GetSeconds();

  double Radius = 8.0 + 6.0*sin(s/100);
  m_Optics1RelOB.SetTranslation(400+Radius*sin(s/1000), 200+Radius*cos(s/2000), 0.0);
  m_Optics2RelOB.SetTranslation(-400+Radius*sin(s/1000), 200+Radius*cos(s/2000), 0.0);
  m_Optics1RelOB.SetRotation((1.0/60.0*c_Rad)*sin(s/1500), (0.75/60.0*c_Rad)*sin(s/1500), (0.25/60.0*c_Rad)*sin(s/1500));
  m_Optics2RelOB.SetRotation((1.0/60.0*c_Rad)*sin(s/1500), (0.75/60.0*c_Rad)*sin(s/1500), (0.25/60.0*c_Rad)*sin(s/1500));
  */
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsDatabase::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsOrientationsDatabase* Options = new NGUIOptionsOrientationsDatabase(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleOrientationsDatabase::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "FileName", m_FileName);

  return Node;
}


// NModuleOrientationsDatabase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
