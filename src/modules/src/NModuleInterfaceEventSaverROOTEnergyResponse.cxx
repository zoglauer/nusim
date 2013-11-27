/*
 * NModuleInterfaceEventSaverROOTEnergyResponse.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceEventSaverROOTEnergyResponse
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceEventSaverROOTEnergyResponse.h"

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
ClassImp(NModuleInterfaceEventSaverROOTEnergyResponse)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverROOTEnergyResponse::NModuleInterfaceEventSaverROOTEnergyResponse(NSatellite& Satellite) : m_Satellite(Satellite)
{
  // Construct an instance of NModuleInterfaceEventSaverROOTEnergyResponse

  m_File = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverROOTEnergyResponse::~NModuleInterfaceEventSaverROOTEnergyResponse()
{
  // Delete this instance of NModuleInterfaceEventSaverROOTEnergyResponse
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverROOTEnergyResponse::OpenEnergyResponseROOTFile(TString FileName)
{
  //! Load and initialize the file

  // cout<<"OpenFits"<<endl;

  TDirectory* OrgDirectory = gDirectory;

  MFile::ExpandFileName(FileName);

  m_File = 0;
  m_File = new TFile(FileName.Data(), "recreate", "NuSim ROOT File", 1);
  if ( m_File == 0 || m_File->IsZombie() ) {
    mgui<<"Error in creating ROOT file: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  m_Response = new TH2D("Response",
			Form("Detector Response Produced by NuSim ver. %s (SVN rev. %d)",
			     g_Version.Data(), g_SVNRevision),
			4096, 1.6, 165.44,  // ARF
			4096, 1.6, 165.44); // RMF Ebounds
			// 820, 2.95, 84.95,  // ARF
			// 820, 2.95, 84.95); // RMF Ebounds

  m_Response->SetXTitle("Primary Energy (keV)");
  m_Response->SetYTitle("Observed Energy (keV)");
  m_Response->SetZTitle("Counts");

  gDirectory = OrgDirectory;

  return true;
}



////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverROOTEnergyResponse::SaveEnergyResponse(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level

  // cout<<"SaveROOT"<<endl;

  // from NHits
  int NHits = Event.GetNHits();
  if      ( NHits == 0 ) return true;
  else if ( NHits > 1 ) mout << "Warning: NHits (" << NHits << ")> 1" << endl;

  double ReconstructedEnergy = Event.GetHit(0).GetEnergy();
  double PrimaryEnergy       = Event.GetOriginalPhoton().GetEnergy();

  m_Response->Fill(PrimaryEnergy, ReconstructedEnergy);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverROOTEnergyResponse::CloseEnergyResponseROOTFile()
{
  //! Close the file
  // cout<<"CloseROOT"<<endl;

  
  TDirectory* OrgDirectory = gDirectory;

  m_File->cd();
  if (m_Response->GetEntries() > 0) {
    m_Response->SetEntries(m_Satellite.GetEffectiveObservationTime().GetAsSeconds());
    m_Response->Write();
  }
  m_File->Close();

  m_File = 0;

  OrgDirectory->cd();

  return true;
}


// NModuleInterfaceEventSaverROOTEnergyResponse.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
