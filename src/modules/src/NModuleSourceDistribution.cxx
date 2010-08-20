/*
 * NModuleSourceDistribution.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleSourceDistribution
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleSourceDistribution.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NModuleInterfaceEntry.h"
#include "NGUIOptionsSourceDistribution.h"
#include "NGUIDiagnosticsSourceDistribution.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleSourceDistribution)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleSourceDistribution::NModuleSourceDistribution(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleSourceDistribution

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "General source engine";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagSourceDistribution";

  // Set the tool tip text
  m_ToolTip = "This module represents a general source engine";

  // Set all types this modules handles
  m_ModuleType = c_SourceGenerator;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = true;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsSourceDistribution)
  // and implement all your GUI options
  m_Diagnostics = new NGUIDiagnosticsSourceDistribution();

  // Additional data:
}


////////////////////////////////////////////////////////////////////////////////


NModuleSourceDistribution::~NModuleSourceDistribution()
{
  // Delete this instance of NModuleSourceDistribution
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistribution::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsSourceDistribution();

  if (m_Sources.size() == 0) {
    cerr<<"No sources!"<<endl;
    return false;
  }
  
  dynamic_cast<NGUIDiagnosticsSourceDistribution*>(m_Diagnostics)->SetInitialPointing(m_Satellite.GetPointing(0).GetRa(), 
                                                                                      m_Satellite.GetPointing(0).GetDec());

  // Initial calculation of next event.
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    m_Sources[s]->CalculateNextEmission(m_Satellite.GetTimeIdeal());
  }

  DetermineNext();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleSourceDistribution::DetermineNext()
{
  // Determine which module starts:
  m_NextComponent = 0;
  m_Time = numeric_limits<double>::max();
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    if (m_Sources[s]->GetNextEmission() < m_Time) {
      m_Time = m_Sources[s]->GetNextEmission();
      m_NextComponent = s;
    }
  }  
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistribution::AnalyzeEvent(NEvent& Event) 
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

  // And set all of its initial parameters:
  m_Sources[m_NextComponent]->Generate(Photon, Telescope);

  //cout<<"Photon: "<<Photon.GetEnergy()<<":"<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // Determine RA & DEC from Photon.GetDirection() which is in focal plane coordinates
  double Ra = 0;
  double Dec = 0;

  // Need the reverse direction, only reverse Z. Has to do with how it was defined in the raytrace module.
  MVector SP = Photon.GetDirection();
  SP[2]=-SP[2];
  
  NPointing P = m_Satellite.GetPointing(m_Time);
  NOrientation OB = m_Satellite.GetOrientationOpticalBench(m_Time);
  NOrientation Robst = m_Satellite.GetOrientationStarTrackerRelOpticalBench(m_Time, 4);
  NQuaternion Qob = OB.GetRotationQuaternion();
  NQuaternion Qobin = P.GetQuaternion()*Robst.GetRotationQuaternion();
 
  // Need to flip x/y but only in OB system....
  SP = Qob.Rotation(SP);
  SP = Qobin.Rotation(SP);
 
  
  // Store RA and DEC of the original photon  
  Ra = atan2(SP[1], SP[0])*60*c_Deg;
  if (Ra < 0) Ra += 360*60;
  Dec = asin(SP[2])*60*c_Deg;

  Event.SetOriginalPhotonRaDec(Ra, Dec);
  
  
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

  m_Sources[m_NextComponent]->CalculateNextEmission(m_Satellite.GetTimeIdeal());
  DetermineNext();

  dynamic_cast<NGUIDiagnosticsSourceDistribution*>(m_Diagnostics)->AddOrigin(Ra, Dec);
  dynamic_cast<NGUIDiagnosticsSourceDistribution*>(m_Diagnostics)->AddEnergy(Photon.GetEnergy());
 
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleSourceDistribution::ShowOptionsGUI()
{
  //! Show the options GUI

  NGUIOptionsSourceDistribution* Options = new NGUIOptionsSourceDistribution(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistribution::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  m_Sources.clear();

  MXmlNode* SourcesNode = Node->GetNode("Sources");
  if (SourcesNode != 0) {
    for (unsigned int s = 0; s < SourcesNode->GetNNodes(); ++s) {
      if (SourcesNode->GetNode(s)->GetName() == "Source") {
        NSource* S = new NSource(m_Satellite);;
        S->ReadXmlConfiguration(SourcesNode->GetNode(s));
        m_Sources.push_back(S);
      }
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleSourceDistribution::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  MXmlNode* Sources = new MXmlNode(Node, "Sources");
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    Sources->AddNode(m_Sources[s]->CreateXmlConfiguration());
  }
 
  return Node;
}


// NModuleSourceDistribution.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
