/*
 * NModuleDetectorSimulatorIdeal.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorSimulatorIdeal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorSimulatorIdeal.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NInteraction.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorSimulatorIdeal)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorIdeal::NModuleDetectorSimulatorIdeal(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleDetectorSimulatorIdeal

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal interactions";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorSimulatorIdeal";

  // Set the tool tip text
  m_ToolTip = "This is an ideal detector simulator --- all interactions are ideal photo-effect events.";

  // Set the module type
  m_ModuleType = c_DetectorEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsDetectorSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsDetectorSimulator();
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorIdeal::~NModuleDetectorSimulatorIdeal()
{
  // Delete this instance of NModuleDetectorSimulatorIdeal
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorIdeal::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorIdeal::AnalyzeEvent(NEvent& Event) 
{
  // Start with a photon and end with ideal interaction information in the detector
	// At the end, the hits are in the individual CZT detectors  

  // Generate interactions from the photon and add them to the event

  // (a) Retrieve the current photon parameters
  NPhoton Photon = Event.GetCurrentPhoton();

  // (b) Retrieve the focal plane orientation and position relative to the photon
  NOrientation Orientation = m_Satellite.GetOrientationFocalPlaneModule(Event.GetTime(), Event.GetTelescope());

  //cout<<"Pos on det: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (c) Switch into focal bench coordinate system:
  Orientation.TransformIn(Photon);

  // (d) Propagate the photon towards the beryllium window
  MVector BerylliumTop(0.0, 0.0, 200); // rough Beryllium position -- real one doesn't matter 
  MVector BerylliumNormal(0.0, 0.0, 1.0);
  PropagateToPlane(Photon, BerylliumTop, BerylliumNormal);

  // (e) Calculate absorption probabilty in Beryllium
  //     --- IGNORE Compton/Rayleigh scatter probability and other effects
  double BerylliumThickness = 0.095*mm; // From kristin!
  double BerylliumLength = gRandom->Exp(1.0/m_Satellite.GetCalibratedBerylliumAbsorptionCoefficient(Photon.GetEnergy()));
  if (BerylliumLength < BerylliumThickness) {
    Event.SetBlocked(true);
    return true;
  }

  // (f) Propagate the photon towards the top of the detector plane:
  MVector DetectorTop(0.0, 0.0, m_Satellite.GetDetectorHalfDimension().Z());
  MVector DetectorNormal(0.0, 0.0, 1.0);
  PropagateToPlane(Photon, DetectorTop, DetectorNormal);

  //cout<<"Pos on det: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (g) Check in which detector we are in (could be simplified, but that's safer this way)
  NOrientation DetectorOrientation;
  int Detector = 0;
  for (unsigned int d = 1; d <= 4; ++d) {
    DetectorOrientation = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetTelescope(), d);
    DetectorOrientation.TransformIn(Photon);
    if (fabs(Photon.GetPosition().X()) <= m_Satellite.GetDetectorHalfDimension().X() &&
        fabs(Photon.GetPosition().Y()) <= m_Satellite.GetDetectorHalfDimension().Y() &&
        fabs(Photon.GetPosition().Z()) <= m_Satellite.GetDetectorHalfDimension().Z()) {
      Detector = d;
      break;
    } else {
      DetectorOrientation.TransformOut(Photon);
    }
  }

  if (Detector == 0) {
    // We did not hit the detector and can stop here
    //cout<<"Did not hit detector..."<<endl;
    Event.SetBlocked(true);
    return true;
  }

  // (h) Fake the depth by determining a length based on TOTAL (not just photo) absorption cross-sections
  //     --- IGNORE Compton/Rayleigh scatter probability
  double Length = gRandom->Exp(1.0/m_Satellite.GetCZTAbsorptionCoefficient(Photon.GetEnergy()));
  
  // (i) Propagate the photon a little farther
  Propagate(Photon, Length);

  // (j) If we are no longer in the CZT -- assume it escaped
  if (fabs(Photon.GetPosition().X()) > m_Satellite.GetDetectorHalfDimension().X() ||
      fabs(Photon.GetPosition().Y()) > m_Satellite.GetDetectorHalfDimension().Y() ||
      fabs(Photon.GetPosition().Z()) > m_Satellite.GetDetectorHalfDimension().Z()) {
    Event.SetBlocked(true);
    DetectorOrientation.TransformOut(Photon);
    Orientation.TransformOut(Photon);
    return true;
  }

  // (k) Create an interaction & store the information in the event 
  NInteraction I;
  I.SetIsDetector(true);
  I.SetTelescope(Event.GetTelescope());
  I.SetDetector(Detector);
  I.SetPosition(Photon.GetPosition());
  I.SetEnergy(Photon.GetEnergy());
  
  //cout<<"IA Pos in CZT element: "<<Photon.GetPosition()<<" E="<<Photon.GetEnergy()<<endl;
  Event.AddInteraction(I);

  // (l) Transform the photon back -- just for book keeping
  DetectorOrientation.TransformOut(Photon);
  Orientation.TransformOut(Photon);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorSimulatorIdeal::ShowOptionsGUI()
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


bool NModuleDetectorSimulatorIdeal::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleDetectorSimulatorIdeal::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorSimulatorIdeal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
