/*
 * NModuleOrientationsIdeal.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOrientationsIdeal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOrientationsIdeal.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MFile.h"
#include "MTokenizer.h"

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOrientationsIdeal)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleOrientationsIdeal::NModuleOrientationsIdeal(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceOrientations()
{
  // Construct an instance of NModuleOrientationsIdeal

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleOrientationsIdeal";

  // Set the tool tip text
  m_ToolTip = "This module determines the orientations and alignments of optics, apertiure, and metrology\n"
    "However, in this special version everything is assumed to be fixed at the ideal position.";

  // Set all types this modules handles
  m_ModuleType = c_OrientationsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOrientationsIdeal)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOrientationsIdeal();
}


////////////////////////////////////////////////////////////////////////////////


NModuleOrientationsIdeal::~NModuleOrientationsIdeal()
{
  // Delete this instance of NModuleOrientationsIdeal
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsIdeal::Initialize()
{
  // Initialize the module 

  m_FileName = "$(NUSIM)/resource/data/DefaultOrientations.dat";
  MFile::ExpandFileName(m_FileName);

  if (LoadDefaultValues() == false) return false;

  return true;
}
  

////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsIdeal::LoadDefaultValues()
{
  // Loads the default values

  // Determine orientations if they are not yet present for the given time t
  // All are relative to the focal plane coordinate system
  // Remember distance are in mm, angles in radians!!

	// Load the data base
  TString FileName = "$(NUSIM)/resource/data/DefaultOrientations.dat";
  MFile::ExpandFileName(FileName);
	if (ReadOrientations(FileName) == false) return false;

  if (m_Orientations.size() != 1) {
    cout<<"NModuleOrientationsIdeal:Not enough or too much orientations found... I want exactly 1..."<<endl;
    return false;
  }

  // Copy the data - idealy we should interpolate 
  m_SpaceCraftRelInertial = m_Orientations[0].GetOrientationSpaceCraftRelInertial();
  m_FocalPlaneRelSC = m_Orientations[0].GetOrientationFocalPlaneRelSC();
  m_OpticalBench = m_Orientations[0].GetOrientationOpticalBench();
  m_Optics1RelOB = m_Orientations[0].GetOrientationOpticsRelOpticalBench(1);
  m_Optics2RelOB = m_Orientations[0].GetOrientationOpticsRelOpticalBench(2);
  m_Aperture1 = m_Orientations[0].GetOrientationAperture(1);
  m_Aperture2 = m_Orientations[0].GetOrientationAperture(2);
  m_MetrologyLaser1RelOB = m_Orientations[0].GetOrientationMetrologyLaserRelOpticalBench(1);
  m_MetrologyDetector1 = m_Orientations[0].GetOrientationMetrologyDetector(1);
  m_MetrologyLaser2RelOB = m_Orientations[0].GetOrientationMetrologyLaserRelOpticalBench(2);
  m_MetrologyDetector2 = m_Orientations[0].GetOrientationMetrologyDetector(2);
  m_MetrologyLaser3RelOB = m_Orientations[0].GetOrientationMetrologyLaserRelOpticalBench(3);
  m_MetrologyDetector3 = m_Orientations[0].GetOrientationMetrologyDetector(3);
  m_StarTracker4RelOB = m_Orientations[0].GetOrientationStarTrackerRelOpticalBench(4);
  m_FocalPlaneModule1 = m_Orientations[0].GetOrientationFocalPlaneModule(1);
  m_FocalPlaneModule2 = m_Orientations[0].GetOrientationFocalPlaneModule(2);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsIdeal::DetermineOrientations(const NTime& Time)
{
  // Since all orientations are ideal no pertubations occur here!
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsIdeal::ShowOptionsGUI()
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


bool NModuleOrientationsIdeal::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleOrientationsIdeal::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleOrientationsIdeal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
