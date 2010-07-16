/*
 * NModule.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModule
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModule.h"

// Standard libs:
#include "MStreams.h"

// ROOT libs:

// MEGAlib libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModule)
#endif


////////////////////////////////////////////////////////////////////////////////


const int NModule::c_Unknown                  = 0;

// Satellite modules (attention if you add one here, add it also to the Validator):
const int NModule::c_Geometry                        = 1;
const int NModule::c_OrbitEngine                     = 2;
const int NModule::c_OrientationsEngine              = 3;
const int NModule::c_PointingEngine                  = 4;
const int NModule::c_TimeEngine                      = 5;
const int NModule::c_GeometryAndDetectorProperties   = 6;

// Pipeline modules:
// Pipeline - simulator
const int NModule::c_SourceGenerator                 = 10;
const int NModule::c_OpticsEngine                    = 11;
const int NModule::c_ApertureEngine                  = 12;
const int NModule::c_DetectorEngine                  = 13;
const int NModule::c_DetectorEffectsEngine           = 14;
const int NModule::c_TriggerEngine                   = 15;
const int NModule::c_BackgroundEngine                = 16;
const int NModule::c_DetectorCalibrator              = 17;

// Pipeline - load save
const int NModule::c_EventLoader                     = 30;
const int NModule::c_EventSaver                      = 31;

// Pipeline - analysis
const int NModule::c_EventSelector                   = 40;
const int NModule::c_ScienceAnalyzer                 = 41;

// Pipeline - Star tracker
const int NModule::c_StarTrackerEngine               = 70;
const int NModule::c_StarTrackerCalibrator           = 71;
const int NModule::c_StarTrackerLoader               = 73;
const int NModule::c_StarTrackerSaver                = 74;

// Pipeline - Metrology
const int NModule::c_MetrologyEngine                 = 80;
const int NModule::c_MetrologyCalibrator             = 81;
const int NModule::c_MetrologyLoader                 = 83;
const int NModule::c_MetrologySaver                  = 84;

// Pipeline - Observatory
const int NModule::c_ObservatoryReconstructor        = 90;
const int NModule::c_ObservatoryMerger               = 91;
const int NModule::c_ObservatoryLoader               = 92;
const int NModule::c_ObservatorySaver                = 93;


////////////////////////////////////////////////////////////////////////////////


NModule::NModule(NSatellite& Satellite) : m_Satellite(Satellite)
{
  // Construct an instance of NModule

  m_ModuleType = c_Unknown;

  m_Name = "The programmer forgot to give this module a name...";
  m_XmlTag = "BaseClass"; // No spaces allowed
  m_ToolTip = "The programmer forgot to give this module a tool tip.";

  m_HasDiagnosticsGUI = false;
  m_Diagnostics = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModule::~NModule()
{
  // Delete this instance of NModule
}


////////////////////////////////////////////////////////////////////////////////


TString NModule::GetModuleTypeString() 
{
  //! Return the type string of the module

  return GetModuleTypeString(m_ModuleType);
}


////////////////////////////////////////////////////////////////////////////////


TString NModule::GetModuleTypeString(int Type)
{
  //! Return a specvific type string
  
  if (Type == c_Geometry) {
    return "Geometry";
  } else if (Type == c_TimeEngine) {
    return "Time engine";
  } else if (Type == c_OrbitEngine) {
    return "Orbit engine";
  } else if (Type == c_OrientationsEngine) {
    return "Orientation and alignment engine";
  } else if (Type == c_PointingEngine) {
    return "Pointing engine";
  } else if (Type == c_GeometryAndDetectorProperties) {
    return "Geometry and detector properties";

  // Pipeline modules:
  // Pipeline - simulator
  } else if (Type == c_SourceGenerator) {
    return "Source engine";
  } else if (Type == c_OpticsEngine) {
    return "Optics engine";
  } else if (Type == c_ApertureEngine) {
    return "Aperture engine";
  } else if (Type == c_DetectorEngine) {
    return "Detector interaction simulator";
  } else if (Type == c_BackgroundEngine) {
    return "Background engine";
  } else if (Type == c_DetectorEffectsEngine) {
    return "Detector effects engine";
  } else if (Type == c_TriggerEngine) {
    return "Trigger/packaging engine";
  } else if (Type == c_MetrologyEngine) {
    return "Metrology engine";
  } else if (Type == c_StarTrackerEngine) {
    return "Star tracker engine";

  // Pipeline - analysis
  } else if (Type == c_DetectorCalibrator) {
    return "Detector data calibrator";
  } else if (Type == c_EventSelector) {
    return "Event selector";
  } else if (Type == c_ScienceAnalyzer) {
    return "Science analyzer";

  } else if (Type == c_DetectorCalibrator) {
    return "Detector calibrator";
  } else if (Type == c_MetrologyCalibrator) {
    return "Metrology calibrator";
  } else if (Type == c_StarTrackerCalibrator) {
    return "Star Tracker calibrator";
  } else if (Type == c_ObservatoryReconstructor) {
    return "Observatory reconstructor";
  } else if (Type == c_ObservatoryMerger) {
    return "Observatory merger";
  } else if (Type == c_StarTrackerLoader) {
    return "Star tracker data loader";
  } else if (Type == c_MetrologyLoader) {
    return "Metrology data loader";
  } else if (Type == c_ObservatoryLoader) {
    return "Observatory data loader";
  } else if (Type == c_StarTrackerSaver) {
    return "Star tracker data saver";
  } else if (Type == c_MetrologySaver) {
    return "Metrology data saver";
  } else if (Type == c_ObservatorySaver) {
    return "Observatory data saver";

  // Pipeline - load save
  } else if (Type == c_EventLoader) {
    return "Event loader";
  } else if (Type == c_EventSaver) {
    return "Event saver";
  }


  TString Return("Unknown module type with ID: ");
  Return += Type;

  return Return;
}


////////////////////////////////////////////////////////////////////////////////


bool NModule::IsUsedType(int Type)
{
  //! Return true is the type/placement exits
  
  if (Type == c_Geometry ||
      Type == c_TimeEngine ||
      Type == c_OrbitEngine ||
      Type == c_OrientationsEngine ||
      Type == c_PointingEngine ||
      Type == c_GeometryAndDetectorProperties ||
      Type == c_SourceGenerator ||
      Type == c_OpticsEngine ||
      Type == c_ApertureEngine ||
      Type == c_DetectorEngine ||
      Type == c_BackgroundEngine ||
      Type == c_DetectorEffectsEngine ||
      Type == c_TriggerEngine ||
      Type == c_MetrologyEngine ||
      Type == c_StarTrackerEngine ||
      Type == c_DetectorCalibrator ||
      Type == c_EventSelector ||
      Type == c_ScienceAnalyzer ||
      Type == c_DetectorCalibrator ||
      Type == c_MetrologyCalibrator ||
      Type == c_StarTrackerCalibrator ||
      Type == c_ObservatoryReconstructor ||
      Type == c_ObservatoryMerger ||
      Type == c_StarTrackerLoader ||
      Type == c_MetrologyLoader ||
      Type == c_ObservatoryLoader ||
      Type == c_StarTrackerSaver ||
      Type == c_MetrologySaver ||
      Type == c_ObservatorySaver ||
      Type == c_EventLoader ||
      Type == c_EventSaver) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NModule::GetNModulePlacements()
{ 
  // Return all potential placements

  if (m_ModulePlacements.size() == 0) {
    // ... then the only placement is the place of the module type
    m_ModulePlacements.push_back(m_ModuleType);
  }

  return m_ModulePlacements.size(); 
}
 

////////////////////////////////////////////////////////////////////////////////


int NModule::GetModulePlacement(unsigned int i) 
{ 
  // Return the placement at position i

  if (m_ModulePlacements.size() == 0) {
    // ... then the only placement is the place of the module type
    m_ModulePlacements.push_back(m_ModuleType);
  }
  
  if (i >= m_ModulePlacements.size()) {
    merr<<"Index out of bounds"<<endl;
    return m_ModuleType;
  }

  return m_ModulePlacements.at(i); 
}


////////////////////////////////////////////////////////////////////////////////


bool NModule::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModule::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  return Node;
}


// NModule.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
