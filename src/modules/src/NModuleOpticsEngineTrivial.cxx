/*
 * NModuleOpticsEngineTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOpticsEngineTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOpticsEngineTrivial.h"

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
ClassImp(NModuleOpticsEngineTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleOpticsEngineTrivial::NModuleOpticsEngineTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleOpticsEngineTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Trivial";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagOpticsEngineTrivial";

  // Set the tool tip text
  m_ToolTip = "This is an optics engine which does a trivial ideal approximation of the real one.";

  // Set all types this modules handles
  m_ModuleType = c_OpticsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsApertureSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsApertureSimulator();
}


////////////////////////////////////////////////////////////////////////////////


NModuleOpticsEngineTrivial::~NModuleOpticsEngineTrivial()
{
  // Delete this instance of NModuleOpticsEngineTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngineTrivial::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngineTrivial::AnalyzeEvent(NEvent& Event) 
{
  // This combined optics and aperture simulator

  // STEP 1: 

  // (a) Retrieve the current photon parameters
  NPhoton Photon = Event.GetCurrentPhoton();

  // (b) Retrieve the aperture orienation and position relative to the detector
  NOrientation Orientation = m_Satellite.GetOrientationOptics(Event.GetTime(), Event.GetTelescope());

  // (c) Rotate into aperture coordinate system
  Orientation.TransformIn(Photon);

  // (d) Propagate photon to where-ever this module wants it
  //     Let's assume its the plane at the top of the optics module
  
  // ATTENTION: ignore everyhting which is already below the top of the opening cylinder of the optics module

  MVector TopCenterOutermostRing(0.0, 0.0, 0.0);
  MVector NormalToOutermostRing(0.0, 0.0, 1.0);
  if (PropagateToPlane(Photon, TopCenterOutermostRing, NormalToOutermostRing) == false) {
    // The plane is unreachable thus we quit here
    Orientation.TransformOut(Photon);
    Event.SetBlocked(true);
    return true;
  }
  // That's just an example

  
  // Step 2: Perform the optics simulation/modification of photon direction via responses 

  // ------------- add you code here -----------

  // This is some dummy code, assuming the optics is infinitesimal thin and focusses the photon
  // onto an infinitesimal spot.

  // Spot on the detector
  MVector Spot(0.0, 0.0, 0.0);
  // Transfer to focal bench
  m_Satellite.GetOrientationFocalPlaneModule(Event.GetTime(), Event.GetTelescope()).TransformOut(Spot);
  // Transfer to opotics module center:
  m_Satellite.GetOrientationOptics(Event.GetTime(), Event.GetTelescope()).TransformIn(Spot);
  Spot[0] = 0.0;
  Spot[1] = 0.0;

  // If we are off-axis then the spot moves: 3 mm for each arcmin --- in the direction of the original photons movements
  MVector ProjectedPhotonDir(Photon.GetDirection().X(), Photon.GetDirection().Y(), 0.0);
  ProjectedPhotonDir.Unitize();

  double AngleInArcMin = MVector(0.0, 0.0, -1.0).Angle(Photon.GetDirection())*c_Deg*60;
  ProjectedPhotonDir *= 3*AngleInArcMin; // 3 mm per arcmin

  Spot += ProjectedPhotonDir;

  MVector Direction = (Spot-Photon.GetPosition()).Unit();

  // This dummy only broadens the distribution and puts the photon position to the top of the detector
  
//   double Amount = 0.00004;
//   Direction.SetX(gRandom->Gaus(Direction.GetX(), Amount));
//   Direction.SetY(gRandom->Gaus(Direction.GetY(), Amount));
//   Direction.SetZ(gRandom->Gaus(Direction.GetZ(), Amount));

  
  Photon.SetDirection(Direction.Unit());


  // -------------------------------------------

  // Step 3: 

  // (a) Update the current photon data
  //Photon.SetPosition(MVector(Photon.GetPosition().X() + Radius*cos(Phi), Photon.GetPosition().Y() + Radius*sin(Phi), 0.0));

  // (b) Rotate back in world coordinate system
  Orientation.TransformOut(Photon);

  // (c) Set the photon back into the event
  Event.SetCurrentPhoton(Photon);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOpticsEngineTrivial::ShowOptionsGUI()
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


bool NModuleOpticsEngineTrivial::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleOpticsEngineTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleOpticsEngineTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
