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
#include <limits>
using namespace std;

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
  if (gROOT->IsBatch() == false) {
    m_HasDiagnosticsGUI = true;
    m_Diagnostics = new NGUIDiagnosticsSourceDistribution();
  } else {
    m_HasDiagnosticsGUI = false;
    m_Diagnostics = 0;    
  }
  
  // Additional data:
  m_PointingPatternFileName = "$(NUSIM)/ATestPattern.dat";
  m_NTestPhotons = 1000000;
  m_DistanceBetweenPointings = 7.3;
  m_MaxAllowedDistanceForAcceptance = 3.0;
  m_Yaw = 180.0*60.0;
  
  m_StoreInitialPhoton = false;
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

  if (m_Sources.size() == 0) {
    cerr<<"No sources!"<<endl;
    return false;
  }
  
  if (gROOT->IsBatch() == false) {
    delete m_Diagnostics;
    m_Diagnostics = new NGUIDiagnosticsSourceDistribution();
    dynamic_cast<NGUIDiagnosticsSourceDistribution*>(m_Diagnostics)->SetInitialPointing(m_Satellite.GetPointing(0).GetRa(), 
                                                                                        m_Satellite.GetPointing(0).GetDec());
  }
  
  // Initial calculation of next event.
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    m_Sources[s]->Reset();
    m_Sources[s]->CalculateNextEmission(m_Satellite.GetTime());
  }

  DetermineNext();

  m_RaMin = numeric_limits<double>::max();
  m_RaMax = -numeric_limits<double>::max();
  m_DecMin = numeric_limits<double>::max();
  m_DecMax = -numeric_limits<double>::max();

  //return GeneratePointingPattern();

  if (m_StoreInitialPhoton == true) {
    if (OpenAsciiFile(NModuleInterfaceIO::GetBaseFileName() + ".photon.dat", m_ASCIIFileVersion) == false) return false;
  }

  m_NStartedPhotons = 0;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleSourceDistribution::DetermineNext()
{
  // Determine which module starts:
  m_NextComponent = 0;
  m_Time.SetMax();
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    if (m_Sources[s]->IsActive() == false) continue;
    if (m_Sources[s]->GetNextEmission().GetAsSeconds() < 0.0) {
      cerr<<"Source has negative time: "<<m_Sources[s]->GetName()<<endl;
    }
    if (m_Sources[s]->GetNextEmission() < m_Time) {
      m_Time = m_Sources[s]->GetNextEmission();
      m_NextComponent = s;
    }
  }
}
  

////////////////////////////////////////////////////////////////////////////////


void NModuleSourceDistribution::PerformTimeJump(const NTime& TimeJump)
{
  //! Perform a time jump:

  if (m_Time.IsMax() == true) return;
  
  m_Time += TimeJump;
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    if (m_Sources[s]->IsActive() == false) continue;
    m_Sources[s]->PerformTimeJump(TimeJump);
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

  /* Old relative to star tracker:
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
  */
  
  // New relative to FB
  
 // Need the reverse direction, to find out where the photon came from.  
  MVector SP = Photon.GetDirection();
  SP=-SP;

  // SP is alreday in focal bench coordinates, thus we only have to apply the pointing rotation:
  NPointing P = m_Satellite.GetPointing(m_Time);
  SP = P.GetQuaternion().Rotation(SP);
  
  
  // Store RA and DEC of the original photon  
  Ra = atan2(SP[1], SP[0])*60*c_Deg;
  if (Ra < 0) Ra += 360*60;
  Dec = asin(SP[2])*60*c_Deg;

  Event.SetOriginalPhotonRaDec(Ra, Dec);

  
  // For diagnostisc:
  if (Ra < m_RaMin) m_RaMin = Ra;
  if (Ra > m_RaMax) m_RaMax = Ra;
  if (Dec < m_DecMin) m_DecMin = Dec;
  if (Dec > m_DecMax) m_DecMax = Dec;
  
  
  // (d) Store all the data:
  Event.SetTelescope(Telescope);
  Event.SetTime(m_Satellite.GetTime());

  Event.SetOriginalPhoton(Photon);
  Event.SetOrigin(NEvent::c_OriginSource);

  if (m_StoreInitialPhoton == true) {
    if (IsAsciiFileOpen() == true) {
      if (SavePhotonAscii(Photon, m_ASCIIFileVersion) == false) return false;
    }
  }


  // Step 3: Since this is an ENTRY module, set the time when the NEXT event is
  //         started

  m_Sources[m_NextComponent]->CalculateNextEmission(m_Satellite.GetTime());
  DetermineNext();

  if (gROOT->IsBatch() == false) {
    dynamic_cast<NGUIDiagnosticsSourceDistribution*>(m_Diagnostics)->AddOrigin(Ra, Dec);
    dynamic_cast<NGUIDiagnosticsSourceDistribution*>(m_Diagnostics)->AddEnergy(Photon.GetEnergy());
  }
    
  ++m_NStartedPhotons;
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistribution::Finalize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  cout<<endl;
  cout<<"Source generator summary: "<<endl;
  cout<<"  Number of different sources: "<<m_Sources.size()<<endl;
  cout<<"  Number of started photons:   "<<m_NStartedPhotons<<endl;
  cout<<"  Photon emission directions: "<<endl;
  cout<<"    DEC (min/max): "<<m_DecMin/60<<" to "<<m_DecMax/60<<endl;
  cout<<"    RA (min/max):  "<<m_RaMin/60<<" to "<<m_RaMax/60<<endl;

  if (m_StoreInitialPhoton == true) {
    if (IsAsciiFileOpen() == true) {
      if (CloseAsciiFile() == false) return false;
    }
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleSourceDistribution::GeneratePointingPattern()
{
  //! Generate an optimized pointing pattern

  // We have to initialize a few modules here:
  if (m_Satellite.Initialize() == false) {
    mgui<<"Unable to initialize the satellite - aborting..."<<error;
    return false;
  }
  if (dynamic_cast<NModule*>(m_Satellite.GetPointingModule())->Initialize() == false) {
    mgui<<"Unable to initialize the pointing modulee - aborting..."<<error;
    return false;
  }
  if (dynamic_cast<NModule*>(m_Satellite.GetOrientationsModule())->Initialize() == false) {
    mgui<<"Unable to initialize the orientations module - aborting..."<<error;
    return false;
  }

    
  // Since during the pointing pattern generation no external time-synchroniztaion happens initilaize the time with zero  
  m_Time = 0.0;

  double RaMin = numeric_limits<double>::max();
  double DecMin = numeric_limits<double>::max();
  double RaMax = -numeric_limits<double>::max();
  double DecMax = -numeric_limits<double>::max();

  // Initial calculation of next event.
  for (unsigned int s = 0; s < m_Sources.size(); ++s) {
    m_Sources[s]->CalculateNextEmission(m_Time);
  }
  DetermineNext();

  // Initial calculation of next event.
  NPhoton Photon;
  double Ra, Dec;
  
  // Calculate some test events:
  vector<MVector> Tests;
  unsigned int s_max = m_NTestPhotons;
  for (unsigned int s = 0; s < s_max; ++s) {
    if (s % (s_max/10) == 0) {
      cout<<"Simulated "<<s<<"/"<<s_max<<" events"<<endl;
    }

    m_Sources[m_NextComponent]->Generate(Photon, 1);

    // Need the reverse direction, to find out where the photon came from.  
    MVector SP = Photon.GetDirection();
    SP=-SP;

    // SP is alreday in focal bench corrdinates, thus we only have to apply the pointing rotation:
    NPointing P = m_Satellite.GetPointing(m_Time);
    SP = P.GetQuaternion().Rotation(SP);
  
    // Store RA and DEC of the original photon  
    Ra = atan2(SP[1], SP[0])*60*c_Deg;
    if (Ra < 0) Ra += 360*60;
    Dec = asin(SP[2])*60*c_Deg;
  
    // For diagnostisc:
    if (Ra < RaMin) RaMin = Ra;
    if (Ra > RaMax) RaMax = Ra;
    if (Dec < DecMin) DecMin = Dec;
    if (Dec > DecMax) DecMax = Dec;

    MVector TestDir;
    TestDir.SetMagThetaPhi(1.0, c_Pi/2 - Dec/60*c_Rad, Ra/60*c_Rad);
    Tests.push_back(TestDir);

    m_Sources[m_NextComponent]->CalculateNextEmission(m_Time);
    DetermineNext();
  }

  
  double DecCenter = 0.5*(DecMax+DecMin);

  // And set all of its initial parameters:
  double PointingDistanceDec = m_DistanceBetweenPointings; // arcmin
  double PointingDistanceRa = PointingDistanceDec/cos(DecCenter/60*c_Rad); // arcmin
  cout<<"Pointing dis: Ra: "<<PointingDistanceRa<<"  Dec:"<<PointingDistanceDec<<endl;

  //! Increase to cover some area around the sources:
  DecMin -= PointingDistanceDec;
  DecMax += PointingDistanceDec;
  RaMin -= PointingDistanceRa;
  RaMax += PointingDistanceRa;

  int RaPoints = int((RaMax-RaMin)/PointingDistanceRa) + 1;
  int DecPoints = int((DecMax - DecMin)/PointingDistanceDec) + 1; 

  vector<vector<double> > RaPointings;
  vector<vector<double> > DecPointings;
  vector<vector<bool> > AcceptedPointings;
  for (int d = 0; d <= DecPoints; ++d) {
    vector<double> Ras;
    vector<double> Decs;
    vector<bool> Accepted;
    for (int r = 0; r <= RaPoints; ++r) {
      //if (d % 2 == 0) {
        Ra = RaMin + r*PointingDistanceRa;
      //} else {
      //  Ra = RaMin + (r+0.5)*PointingDistanceRa;
      //}
      Dec = DecMin + d*PointingDistanceDec;
      
      Ras.push_back(Ra);
      Decs.push_back(Dec);

      // Now check if there is any source within PointingDistanceDec:
      MVector TestDir;
      TestDir.SetMagThetaPhi(1.0, c_Pi/2 - Dec/60*c_Rad, Ra/60*c_Rad);
    
      bool Found = false;
      for (unsigned int t = 0; t < Tests.size(); ++t) {
        if (Tests[t].Angle(TestDir) < m_MaxAllowedDistanceForAcceptance/60*c_Rad) {
          Found = true;
          break;
        }
      }
      if (Found == true) { 
        cout<<"Accepting: "<<Ra/60.0<<" "<<Dec/60.0<<endl;
        Accepted.push_back(true);
      } else {
        Accepted.push_back(false);
        cout<<"Skipping: "<<Ra/60.0<<" "<<Dec/60.0<<endl;
      }
    }
    AcceptedPointings.push_back(Accepted);
    RaPointings.push_back(Ras);
    DecPointings.push_back(Decs);
  }

  // Clean accepted vectors
  for (int d = 0; d <= DecPoints; ++d) {
    int Started = -1;
    for (int r = 0; r <= RaPoints; ++r) {
      if (AcceptedPointings[d][r] == true) { 
        Started = r;
        break;        
      }
    }
    int Ended = -1;
    for (int r = RaPoints; r >= 0; --r) {
      if (AcceptedPointings[d][r] == true) { 
        Ended = r;
        break;        
      }
    }
    // Make sure everything in between is filled
    for (int r = 0; r <= RaPoints; ++r) {
      if (r >= Started && r <= Ended) {
        AcceptedPointings[d][r] = true;
      }
    }
  }

  // Dump to file
  ofstream out;
  out.open(m_PointingPatternFileName);  
  for (int d = 0; d <= DecPoints; ++d) {
    for (int r = 0; r <= RaPoints; ++r) {
      if (AcceptedPointings[d][r] == true) { 
        out<<"RD "<<RaPointings[d][r]/60.0<<" "<<DecPointings[d][r]/60.0<<" "<<m_Yaw/60.0<<" "<<10.0<<endl;
      }
    }
  }

  out.close();

  // We always return false, since this is just for testing...
  return false;
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
        if (S->ReadXmlConfiguration(SourcesNode->GetNode(s)) == false) {
          merr<<"Error reading Xml entry for a source "<<S->GetName()<<show;    
        } else {
          m_Sources.push_back(S);
        }
      }
    }
  }
  MXmlNode* PointingPatternFileNameNode = Node->GetNode("PointingPatternFileName");
  if (PointingPatternFileNameNode != 0) {
    m_PointingPatternFileName = PointingPatternFileNameNode->GetValue();
  }
  MXmlNode* NTestPhotonsNode = Node->GetNode("NTestPhotons");
  if (NTestPhotonsNode != 0) {
    m_NTestPhotons = NTestPhotonsNode->GetValueAsInt();
  }
  MXmlNode* DistanceBetweenPointingsNode = Node->GetNode("DistanceBetweenPointings");
  if (DistanceBetweenPointingsNode != 0) {
    m_DistanceBetweenPointings = DistanceBetweenPointingsNode->GetValueAsDouble();
  }
  MXmlNode* MaxAllowedDistanceForAcceptanceNode = Node->GetNode("MaxAllowedDistanceForAcceptance");
  if (MaxAllowedDistanceForAcceptanceNode != 0) {
    m_MaxAllowedDistanceForAcceptance = MaxAllowedDistanceForAcceptanceNode->GetValueAsDouble();
  }
  MXmlNode* YawNode = Node->GetNode("Roll");
  if (YawNode != 0) {
    m_Yaw = YawNode->GetValueAsDouble();
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
  new MXmlNode(Node, "PointingPatternFileName", CleanPath(m_PointingPatternFileName));
  new MXmlNode(Node, "NTestPhotons", m_NTestPhotons);
  new MXmlNode(Node, "DistanceBetweenPointings", m_DistanceBetweenPointings);
  new MXmlNode(Node, "MaxAllowedDistanceForAcceptance", m_MaxAllowedDistanceForAcceptance);
  new MXmlNode(Node, "Roll", m_Yaw);
  
  return Node;
}


// NModuleSourceDistribution.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
