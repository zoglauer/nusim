/*
 * NModuleSourceDistributionFarFieldPointSource.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleSourceDistributionFarFieldPointSource
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleSourceDistributionFarFieldPointSource.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NModuleInterfaceEntry.h"
#include "NGUIOptionsSourceDistributionFarFieldPointSource.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleSourceDistributionFarFieldPointSource)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleSourceDistributionFarFieldPointSource::NModuleSourceDistributionFarFieldPointSource(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleSourceDistributionFarFieldPointSource

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Point source in far field";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagSourceDistributionFarField";

  // Set the tool tip text
  m_ToolTip = "This module generates photons of a point source in the far field";

  // Set all types this modules handles
  m_ModuleType = c_SourceGenerator;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsSourceDistribution)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiagnosticsSourceDistributionFarFieldPointSource();

  // Additional data:
  m_RightAscension = 0.0;
  m_Declination = 0.0;
  m_RadiusExtent = 0.0;
  m_Energy = 67.9;
  m_Flux = 1; // ph/cm2/s
}


////////////////////////////////////////////////////////////////////////////////


NModuleSourceDistributionFarFieldPointSource::~NModuleSourceDistributionFarFieldPointSource()
{
  // Delete this instance of NModuleSourceDistributionFarFieldPointSource
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistributionFarFieldPointSource::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  m_Time = m_Satellite.GetTimeIdeal() + gRandom->Exp(1.0/(m_Flux*c_Pi*m_Satellite.GetOpticsSurroundingSphereRadius()*m_Satellite.GetOpticsSurroundingSphereRadius()));

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistributionFarFieldPointSource::AnalyzeEvent(NEvent& Event) 
{
  // Set the initial position of the photon, somewhere above the detector
  // The coordinate system is in TBD (currently: detector coordinate system)

  // Step 1: Convert from galactic coordinates into detector coordinates
  //         Choose a position above the optics
  //         Make sure its high enough above to ensure tilts, so that it still is
  //         above the optics during tilts, etc.

  // Tip: We might define a fixed plane here

  // Retrieve the current pointing of the satellite
  //NPointing* P = m_Satellite->GetCurrentPointing();

  // Determine in which telescope we are:
  int Telescope = (gRandom->Rndm() > 0.5) ? 1 : 2;
  

  // Step 2: Set the original photon of the Event
  
  // Create a photon
  NPhoton Photon;

  MVector Dir;
  //Dir.SetMagThetaPhi(1.0, m_RightAscension/60.0*c_Rad, m_Declination/60*c_Rad); 

  // Modify direction according to extend of disk (assume on-axis for now)
  //double Theta = acos(1 - gRandom->Rndm()*(1 - cos(m_RadiusExtent/60.0*c_Rad)));
  //double Phi = gRandom->Rndm()*2*c_Pi;
  Dir.SetMagThetaPhi(1.0, acos(1 - gRandom->Rndm()*(1 - cos(m_RadiusExtent/60.0*c_Rad))), gRandom->Rndm()*2*c_Pi); 

  double Theta = m_RightAscension/60.0*c_Rad;
  double Phi = m_Declination/60*c_Rad;

  // Rotate the direction vector according to theta and phi
  // left-handed rotation-matrix: first theta (rotation around y-axis) then phi (rotation around z-axis):
  // | +cosp -sinp 0 |   | +cost 0 +sint |   | x |
  // | +sinp +cosp 0 | * |   0   1   0   | * | y | 
  // |   0     0   1 |   | -sint 0 +cost |   | z |        
  double px = Dir.X(), py = Dir.Y(), pz = Dir.Z();
  Dir[0] = (px*cos(Theta)+pz*sin(Theta))*cos(Phi) - py*sin(Phi);
  Dir[1] = (px*cos(Theta)+pz*sin(Theta))*sin(Phi) + py*cos(Phi);
  Dir[2] = -px*sin(Theta)+pz*cos(Theta);

  Photon.SetDirection(Dir);

  Photon.SetPolarization(MVector(0.0, 0.0, 0.0));
  Photon.SetEnergy(m_Energy);

  // Determine its start position
  
  // (a) Switch into the optics module
  NOrientation Optics = m_Satellite.GetOrientationOptics(m_Time, Telescope);
  //Optics.TransformIn(Photon);

  // if we have an incidence angle larger 5 degree we ignore this hole operation
  

  // (b) determine a random position on a disk surrounding the optics module in the inverse direction of the photon
  double Radius = m_Satellite.GetOpticsSurroundingSphereRadius();

  // (i) Create a random position on a disk curently pointing in zenith direction (0, 0, Radius) in the optics module coordinate system
  double x = 0.0, y = 0.0, z = 0.0;
  while (true) {
    x = Radius*(2*(gRandom->Rndm()-0.5));
    y = Radius*(2*(gRandom->Rndm()-0.5));
    if (sqrt(x*x+y*y) <= Radius) break;
  }
  z = Radius;


  // (ii) Rotate it into the source center direction
  Theta = Photon.GetDirection().Theta();
  Phi = Photon.GetDirection().Phi();

  // Rotate according to theta and phi
  // left-handed rotation-matrix: first theta (rotation around y-axis) then phi (rotation around z-axis):
  // | +cosp -sinp 0 |   | +cost 0 +sint |   | x |
  // | +sinp +cosp 0 | * |   0   1   0   | * | y | 
  // |   0     0   1 |   | -sint 0 +cost |   | z |        
  MVector Position;
  Position[0] = (x*cos(Theta)+z*sin(Theta))*cos(Phi) - y*sin(Phi);
  Position[1] = (x*cos(Theta)+z*sin(Theta))*sin(Phi) + y*cos(Phi);
  Position[2] = -x*sin(Theta)+z*cos(Theta);

  // (iii) Set the start position:

  // (c) Switch back to world coordinate system by just applying the translation, not the rotation
  MVector Null(0.0, 0.0, 0.0);
  Optics.TransformOut(Null);
  
  // (iii) Set the start position:
  Photon.SetPosition(Position + Null);
  Photon.SetDirection(-Photon.GetDirection());

  // (d) Store all the data:
  Event.SetTelescope(Telescope);
  if (Telescope == 1) {
    Event.SetTime(m_Satellite.GetTimeDetector1());
  } else {
    Event.SetTime(m_Satellite.GetTimeDetector2());
  }

  Event.SetOriginalPhoton(Photon);
  Event.SetOrigin(NEvent::c_OriginSource);

  // Step 3: Since this is an ENTRY module, set the time when the NEXT event is
  //         started

  m_Time = m_Satellite.GetTimeIdeal() + gRandom->Exp(1.0/(m_Flux*c_Pi*m_Satellite.GetOpticsSurroundingSphereRadius()*m_Satellite.GetOpticsSurroundingSphereRadius()));

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleSourceDistributionFarFieldPointSource::ShowOptionsGUI()
{
  //! Show the options GUI

  NGUIOptionsSourceDistributionFarFieldPointSource* Options = new NGUIOptionsSourceDistributionFarFieldPointSource(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistributionFarFieldPointSource::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* RightAscensionNode = Node->GetNode("RightAscension");
  if (RightAscensionNode != 0) {
    m_RightAscension = RightAscensionNode->GetValueAsDouble();
  }
  MXmlNode* DeclinationNode = Node->GetNode("Declination");
  if (DeclinationNode != 0) {
    m_Declination = DeclinationNode->GetValueAsDouble();
  }
  MXmlNode* RadiusExtentNode = Node->GetNode("RadiusExtent");
  if (RadiusExtentNode != 0) {
    m_RadiusExtent = RadiusExtentNode->GetValueAsDouble();
  }
  MXmlNode* EnergyNode = Node->GetNode("Energy");
  if (EnergyNode != 0) {
    m_Energy = EnergyNode->GetValueAsDouble();
  }
  MXmlNode* FluxNode = Node->GetNode("Flux");
  if (FluxNode != 0) {
    m_Flux = FluxNode->GetValueAsDouble();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleSourceDistributionFarFieldPointSource::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "RightAscension", m_RightAscension);
  new MXmlNode(Node, "Declination", m_Declination);
  new MXmlNode(Node, "RadiusExtent", m_RadiusExtent);
  new MXmlNode(Node, "Energy", m_Energy);
  new MXmlNode(Node, "Flux", m_Flux);
 
  return Node;
}


// NModuleSourceDistributionFarFieldPointSource.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
