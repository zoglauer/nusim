/*
 * NModuleDetectorCalibratorIdeal.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorCalibratorIdeal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorCalibratorIdeal.h"

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
ClassImp(NModuleDetectorCalibratorIdeal)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorCalibratorIdeal::NModuleDetectorCalibratorIdeal(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorCalibratorIdeal

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal interactions";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEnergyAndDepthCalibrationIdeal";

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


NModuleDetectorCalibratorIdeal::~NModuleDetectorCalibratorIdeal()
{
  // Delete this instance of NModuleDetectorCalibratorIdeal
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorIdeal::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorIdeal::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level
	// The position is in the end the position in the focal plane module

  // Upgrade from pixel hits to reals hits

  double NPixelsX = m_Satellite.GetDetectorPixelsX();
  double NPixelsY = m_Satellite.GetDetectorPixelsY();
  
  for (unsigned int i = 0; i < Event.GetNPixelHits(); ++i) {
    NHit H;

    //cout<<"Pixel: "<<Event.GetPixelHit(i).GetXPixel()<<":"<<Event.GetPixelHit(i).GetYPixel()<<endl;

		double Scaler = 1000000; // mm -> nm
    double x = ((0.5 + Event.GetPixelHit(i).GetXPixel())/Scaler) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    double y = ((0.5 + Event.GetPixelHit(i).GetYPixel())/Scaler) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
		double z = Event.GetPixelHit(i).GetIdealAverageDepth();

    //cout<<"Hit x, y, z (D): "<<x<<":"<<y<<":"<<z<<endl;

    // We have to calculate the depth at some point...
    MVector Position(x, y, z);

    // Rotate to module:
    NOrientation Orientation = 
      m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetPixelHit(i).GetTelescope(), Event.GetPixelHit(i).GetDetector());
    Orientation.TransformOut(Position);

    //cout<<"Hit x, y, z (FPM): "<<Position<<endl;

    H.SetTelescope(Event.GetPixelHit(i).GetTelescope());
    H.SetDetector(Event.GetPixelHit(i).GetDetector());

    H.SetPosition(Position);
    H.SetPositionResolution(MVector(0.000001, 0.000001, 0.000001));
    H.SetEnergy(double(Event.GetPixelHit(i).GetIdealEnergy()));
    H.SetEnergyResolution(0.0);

    Event.AddHit(H);
  }

  /*
  if (Event.GetNHits() > 0 && Event.GetNInteractions() > 0) {
    // Debugging: Determine the average distance between the first interaction point and the hit position in CZT pixels:
    MVector Dist = Event.GetInteraction(0).GetPosition();
    NOrientation Orientation = 
      m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetHit(0).GetTelescope(), Event.GetHit(0).GetDetector());
    Orientation.TransformOut(Dist);

    Dist -= Hits[0].GetPosition(); // now we are a real distance...

    //cout<<"Ideal calib: Distance hit - first IA in FPM coordinates in NuSTAR CZT pixels: x="<<Dist.X()/m_Satellite.GetDetectorAveragePixelSizeX()<<",  y="<<Dist.Y()/m_Satellite.GetDetectorAveragePixelSizeY()<<endl;
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorCalibratorIdeal::ShowOptionsGUI()
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


bool NModuleDetectorCalibratorIdeal::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleDetectorCalibratorIdeal::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorCalibratorIdeal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
