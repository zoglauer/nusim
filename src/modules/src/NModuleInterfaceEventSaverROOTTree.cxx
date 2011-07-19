/*
 * NModuleInterfaceEventSaverROOTTree.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceEventSaverROOTTree
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceEventSaverROOTTree.h"

// Standard libs:
// #include "math.h"

// HEAsoft
// #include "fitsio.h"

// ROOT libs:
#include "TDirectory.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSIM libs:
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceEventSaverROOTTree)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverROOTTree::NModuleInterfaceEventSaverROOTTree(NSatellite& Satellite) : m_Satellite(Satellite)
{
  // Construct an instance of NModuleInterfaceEventSaverROOTTree

  m_File = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverROOTTree::~NModuleInterfaceEventSaverROOTTree()
{
  // Delete this instance of NModuleInterfaceEventSaverROOTTree
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverROOTTree::OpenROOTFile(TString FileName)
{
  //! Load and initialize the file

  // cout<<"OpenFits"<<endl;

  TDirectory* OrgDirectory = gDirectory;

  MFile::ExpandFileName(FileName);

  m_File = 0;

  Reference_Ra  = m_Satellite.GetPointing(0).GetRa() /60.0;
  Reference_Dec = m_Satellite.GetPointing(0).GetDec()/60.0;
  Pixsize = 6.;

  m_File = new TFile(FileName.Data(), "recreate", "NuSim ROOT File", 1);
  if ( m_File == 0 || m_File->IsZombie() ) {
    mgui<<"Error in creating ROOT file: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  m_EventTree = new TTree("EventTree",
			  Form("Event Tree Produced by NuSim ver. %s (SVN rev. %d)",
			       g_Version.Data(), g_SVNRevision));

  //! Create one branched with event information
  m_EventTree->Branch("Time",                 &m_Time,                "Time/D");
  m_EventTree->Branch("Origin",               &m_Origin,              "Origin/I");
  m_EventTree->Branch("PrimaryEnergy",        &m_PrimaryEnergy,       "PrimaryEnergy/D");
  m_EventTree->Branch("PrimaryPosition",      &m_PrimaryPosition);
  m_EventTree->Branch("PrimaryDirection",     &m_PrimaryDirection);
  m_EventTree->Branch("RA",                   &m_RA,                  "RA/D");
  m_EventTree->Branch("Dec",                  &m_Dec,                 "Dec/D");
  m_EventTree->Branch("XPix",                 &m_XPix,                "XPix/D");
  m_EventTree->Branch("YPix",                 &m_YPix,                "YPix/D");
  m_EventTree->Branch("TelescopeID",          &m_TelID,               "TelescopeID/I");
  m_EventTree->Branch("DetectorID",           &m_DetID,               "DetectorID/I");
  m_EventTree->Branch("Column",               &m_Column,              "Column/I");
  m_EventTree->Branch("Row",                  &m_Row,                 "Row/I");
  m_EventTree->Branch("BadDepthCal",          &m_BadDepthCal,         "BadDepthCal/O");
  m_EventTree->Branch("DepthCut",             &m_DepthCut,            "DepthCut/O");
  m_EventTree->Branch("NTrigs",               &m_NTrigs,              "NTrigs/I");
  m_EventTree->Branch("Grade",                &m_Grade,               "Grade/I");
  m_EventTree->Branch("TrigEnergy",           &m_TrigEnergy,          "TrigEnergy/D");
  m_EventTree->Branch("NonTrigEnergy",        &m_NonTrigEnergy,       "NonTrigEnergy/D");
  m_EventTree->Branch("ReconstructedEnergy",  &m_ReconstructedEnergy, "ReconstructedEnergy/D");
  m_EventTree->Branch("Energies",              m_Energies,            "Energies[9]/D");

  gDirectory = OrgDirectory;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverROOTTree::SaveEventTree(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level

  // cout<<"SaveROOT"<<endl;

  // from NHits
  int NHits = Event.GetNHits();
  if      ( NHits == 0 ) return true;
  else if ( NHits > 1 ) mout << "Warning: NHits (" << NHits << ")> 1" << endl;
  NHit& Hit = Event.GetHitRef(0);

  // m_TrigEnergy = Hit.GetEnergy();
  m_ReconstructedEnergy = Hit.GetEnergy();
  m_BadDepthCal         = Hit.GetBadDepthCalibration();
  m_DepthCut            = Hit.GetDepthCut();

  m_Time   = Event.GetTime().GetAsSeconds();
  m_Origin = Event.GetOrigin();
  m_Dec    = Hit.GetObservatoryData().GetDec();
  m_RA     = Hit.GetObservatoryData().GetRaScaled();
  m_XPix   = (m_RA - Reference_Ra *60.)*60./Pixsize;
  m_YPix   = (m_Dec- Reference_Dec*60.)*60./Pixsize;

  // from NPhoton
  m_PrimaryEnergy    = Event.GetOriginalPhoton().GetEnergy();
  MVector aPosition  = Event.GetOriginalPhoton().GetPosition();
  MVector aDirection = Event.GetOriginalPhoton().GetDirection();
  m_PrimaryPosition .SetXYZ(aPosition.X(), aPosition.Y(), aPosition.Z());
  m_PrimaryDirection.SetXYZ(aDirection.X(), aDirection.Y(), aDirection.Z());

  // from NNinePixelHist
  int NNinePixelHits = Event.GetNNinePixelHits();
  if ( NNinePixelHits > 1 ) mout << "Warning: NNinePixelHits (" << NNinePixelHits << ")> 1" << endl;
  NNinePixelHit& Niner = Event.GetNinePixelHitRef(0);

  m_TelID  = Niner.GetTelescope();
  m_DetID  = Niner.GetDetector();
  m_Column = Niner.GetXPixel();
  m_Row    = Niner.GetYPixel();
  m_NTrigs = Niner.GetNTriggers();
  m_Grade  = Niner.GetTriggerGrade();

  m_TrigEnergy  = 0.0;
  m_NonTrigEnergy = 0.0;
  for ( int i=0; i<9; ++i ) {
    m_Energies[i] = Niner.GetPostTriggerSampleSum(i+1) - Niner.GetPreTriggerSampleSum(i+1);

    if ( Niner.GetTrigger(i+1) == true ) {
      m_TrigEnergy += m_Energies[i];
    } else {
      m_NonTrigEnergy += m_Energies[i];
    }
  }

  m_EventTree->Fill();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverROOTTree::CloseROOTFile()
{
  //! Close the file
  // cout<<"CloseROOT"<<endl;

  TDirectory* OrgDirectory = gDirectory;

  m_File->cd();
  if ( m_EventTree->GetEntries() > 0 ) m_EventTree->Write();
  m_File->Close();

  m_File = 0;

  OrgDirectory->cd();

  return true;
}


// NModuleInterfaceEventSaverROOTTree.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
