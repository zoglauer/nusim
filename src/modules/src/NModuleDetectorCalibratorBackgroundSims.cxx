/*
 * NModuleDetectorCalibratorBackgroundSims.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorCalibratorBackgroundSims
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorCalibratorBackgroundSims.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NHit.h"
#include "NPixelHit.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorCalibratorBackgroundSims)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorCalibratorBackgroundSims::NModuleDetectorCalibratorBackgroundSims(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorCalibratorBackgroundSims

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Replica of background sims";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEnergyAndDepthCalibrationBackgroundSims";

  // Set the tool tip text
  m_ToolTip = "This module is the calibrator which goes with the ideal detector effects engine.";

  // Set all types this modules handles
  m_ModuleType = c_DetectorCalibrator;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsEnergyAndDepthCalibration)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsEnergyAndDepthCalibration();
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorCalibratorBackgroundSims::~NModuleDetectorCalibratorBackgroundSims()
{
  // Delete this instance of NModuleDetectorCalibratorBackgroundSims
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorBackgroundSims::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorBackgroundSims::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Upgrade from pixel hits to reals hits

  double NPixelsX = m_Satellite.GetDetectorPixelsX();
  double NPixelsY = m_Satellite.GetDetectorPixelsY();

  for (unsigned int i = 0; i < Event.GetNPixelHits(); ++i) {
    NHit H;

    double x = (0.5 + Event.GetPixelHit(i).GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    double y = (0.5 + Event.GetPixelHit(i).GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
    double z = Event.GetPixelHit(i).GetNoisedAverageDepth();

    // We have to calculate the depth at some point...
    MVector Position(x, y, z);

    // Rotate to module:
    NOrientation Orientation = 
      m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetPixelHit(i).GetTelescope(), Event.GetPixelHit(i).GetDetector());
    Orientation.TransformOut(Position);

    H.SetTelescope(Event.GetPixelHit(i).GetTelescope());
    H.SetDetector(Event.GetPixelHit(i).GetDetector());

    H.SetPosition(Position);
    H.SetPositionResolution(MVector(0.0, 0.0, 0.0));
    H.SetEnergy(Event.GetPixelHit(i).GetNoisedEnergy());
    H.SetEnergyResolution(0.0);

    Event.AddHit(H);
  }
//   double TotalEnergy = 0.0;
//   for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
//     TotalEnergy += Event.GetHit(i).GetEnergy();
//   }
//   cout<<"Energy: "<<TotalEnergy<<endl;


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorCalibratorBackgroundSims::ShowOptionsGUI()
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


bool NModuleDetectorCalibratorBackgroundSims::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleDetectorCalibratorBackgroundSims::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorCalibratorBackgroundSims.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
