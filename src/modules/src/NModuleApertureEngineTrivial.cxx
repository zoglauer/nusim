/*
 * NModuleApertureEngineTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleApertureEngineTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleApertureEngineTrivial.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TMath.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleApertureEngineTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleApertureEngineTrivial::NModuleApertureEngineTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleApertureEngineTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "NuSTAR default aperture";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagApertureEngineTrivial";

  // Set the tool tip text
  m_ToolTip = "This is a trivial aperture simulator.";

  // Set all types this modules handles
  m_ModuleType = c_ApertureEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsApertureSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsApertureSimulator();
}


////////////////////////////////////////////////////////////////////////////////


NModuleApertureEngineTrivial::~NModuleApertureEngineTrivial()
{
  // Delete this instance of NModuleApertureEngineTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleApertureEngineTrivial::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleApertureEngineTrivial::AnalyzeEvent(NEvent& Event) 
{
  // This combined optics and aperture simulator

  // STEP 1: 

  // (a) Retrieve the current photon parameters
  NPhoton Photon = Event.GetCurrentPhoton();

  // (b) Retrieve the aperture orientation and position relative to the detector
  NOrientation Orientation = m_Satellite.GetOrientationAperture(Event.GetTime(), Event.GetTelescope());

  //cout<<"Pos pre ap: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (c) Rotate into aperture coordinate system
  Orientation.TransformIn(Photon);

  //cout<<"Pos in ap: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (d) Propagate photon to where-ever this module wants it
  //     Let's assume its the plane of the outer-most aperture ring:
  MVector TopCenterOutermostRing(0.0, 0.0, 0.0);
  MVector NormalToOutermostRing(0.0, 0.0, 1.0);
  if (PropagateToPlane(Photon, TopCenterOutermostRing, NormalToOutermostRing) == false) {
    // The plane is unreachable thus we quit here
    Orientation.TransformOut(Photon);
    Event.SetBlocked(true);
    return true;
  }
  // That's just an example

  
  // STEP 2: Perform the aperture simulation HERE:

  double Radius = sqrt(Photon.GetPosition().X()*Photon.GetPosition().X() +
                       Photon.GetPosition().Y()*Photon.GetPosition().Y());
  
  if (Radius > 58) { // Inner opening of the upper aperture ring
    Event.SetBlocked(true);
    return true;
  }


  // STEP 3:

  // (a) Rotate back in world coordinate system
  Orientation.TransformOut(Photon);
  
  //cout<<"Pos ex ap: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (b) Set the photon back into the event
  Event.SetCurrentPhoton(Photon);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleApertureEngineTrivial::ShowOptionsGUI()
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


bool NModuleApertureEngineTrivial::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleApertureEngineTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleApertureEngineTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
