/*
 * NModuleDetectorSimulatorPropagatorOnly.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorSimulatorPropagatorOnly
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorSimulatorPropagatorOnly.h"

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
ClassImp(NModuleDetectorSimulatorPropagatorOnly)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorPropagatorOnly::NModuleDetectorSimulatorPropagatorOnly(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleDetectorSimulatorPropagatorOnly

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Only propagate to detector plane!";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorSimulatorPropagatorOnly";

  // Set the tool tip text
  m_ToolTip = "The module only propagates the photon to the detector plane. It does not generate interactions!";

  // Set the module type
  m_ModuleType = c_DetectorEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsDetectorSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsDetectorSimulator();
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorPropagatorOnly::~NModuleDetectorSimulatorPropagatorOnly()
{
  // Delete this instance of NModuleDetectorSimulatorPropagatorOnly
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorPropagatorOnly::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorPropagatorOnly::AnalyzeEvent(NEvent& Event) 
{
  // Start with a photon and end with ideal interaction information in the detector

  // Generate interactions from the photon and add them to the event

  // (a) Retrieve the current photon parameters
  NPhoton& Photon = Event.GetCurrentPhotonRef();

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
    Orientation.TransformOut(Photon);
    return true;
  }

  // (f) Propagate the photon towards the top of the detector plane:
  MVector DetectorTop(0.0, 0.0, m_Satellite.GetDetectorHalfDimension().Z());
  MVector DetectorNormal(0.0, 0.0, 1.0);
  PropagateToPlane(Photon, DetectorTop, DetectorNormal);

  Orientation.TransformOut(Photon);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorSimulatorPropagatorOnly::ShowOptionsGUI()
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


bool NModuleDetectorSimulatorPropagatorOnly::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleDetectorSimulatorPropagatorOnly::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorSimulatorPropagatorOnly.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
