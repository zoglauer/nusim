/*
 * NModuleDetectorEffectsEngineSciSimCIE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorEffectsEngineSciSimCIE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorEffectsEngineSciSimCIE.h"

// Standard libs:
#include <cmath>
using namespace std;

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"
#include "TDirectory.h"

// MEGAlib libs:
#include "MAssert.h"

// NuSTAR libs:
// #include "NGUIOptions.h"
#include "NGUIOptionsDetectorEffectsEngineSciSimCIE.h"
#include "NGUIDiagnosticsDetectorEffectsEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorEffectsEngineSciSimCIE)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineSciSimCIE::NModuleDetectorEffectsEngineSciSimCIE(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorEffectsEngineSciSimCIE

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "SciSimCIE thread effects engine calculated";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorEffectsEngineSciSimCIE";

  // Set the tool tip text
  m_ToolTip = "This is the detector effects engine which belongs to the science simulator thread";

  // Set the module type
  m_ModuleType = c_DetectorEffectsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = true;
  m_Diagnostics = 0;
  
  m_PixelCenterPositionX = 0;
  m_PixelCenterPositionY = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineSciSimCIE::~NModuleDetectorEffectsEngineSciSimCIE()
{
  // Delete this instance of NModuleDetectorEffectsEngineSciSimCIE
  delete [] m_PixelCenterPositionX;
  delete [] m_PixelCenterPositionY;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineSciSimCIE::Initialize()
{
  // Initialize the module
  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsDetectorEffectsEngine();
  dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>
    (m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(),
					   m_Satellite.GetDetectorHalfDimension(),
					   m_Satellite.GetDetectorPixelsX(), m_Satellite.GetDetectorPixelsY());

  // Load the charge induction efficiency
  // m_ChargeInductionEfficiencyFileName = "$(NUSIM)/resource/data/CIE_300V.root";
  m_ChargeInductionEfficiencyHistName = "CIE";
  MFile::ExpandFileName(m_ChargeInductionEfficiencyFileName);
  if (LoadChargeInductionEfficiency() == false) {
    return false;
  }

  // Set up the energy resolution
  vector<double> X;
  X.push_back(0.0);
  X.push_back(30.0);
  X.push_back(60.0);
  X.push_back(80.0);
  vector<double> Y;
  Y.push_back(0.60/2.35);
  Y.push_back(0.60/2.35);
  Y.push_back(1.2/2.35);
  Y.push_back(1.6/2.35);
  m_EnergyResolution.Set(X, Y);

  m_LowTrigger = 50;
  m_HighTrigger = 5000;
  m_ChargeSharingDistance = 0.03;
  m_TriggerThreshold = 5;

  // Set up the pixel center positions
  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();
  delete m_PixelCenterPositionX;
  m_PixelCenterPositionX = new double[NPixelsX];
  delete m_PixelCenterPositionY;
  m_PixelCenterPositionY = new double[NPixelsY];

  if ( NPixelsX % 2 == 1 ) { // odd
    double CenterID = (static_cast<double>(NPixelsX) - 1.) / 2.;
    for ( int i=0; i<NPixelsX; ++i) {
      m_PixelCenterPositionX[i] = m_Satellite.GetDetectorAveragePixelSizeX() * (static_cast<double>(i) - CenterID);
    }
  } else { // even
    double CenterID = (-1. + static_cast<double>(NPixelsX)) / 2.;
    for ( int i=0; i<NPixelsX; ++i) {
      m_PixelCenterPositionX[i] = m_Satellite.GetDetectorAveragePixelSizeX() * (static_cast<double>(i) - CenterID);
    }
  }
  if ( NPixelsY % 2 == 1 ) { // odd
    double CenterID = (static_cast<double>(NPixelsY) - 1.) / 2.;
    for ( int i=0; i<NPixelsY; ++i) {
      m_PixelCenterPositionY[i] = m_Satellite.GetDetectorAveragePixelSizeY() * (static_cast<double>(i) - CenterID);
    }
  } else { // even
    double CenterID = (-1. + static_cast<double>(NPixelsY)) / 2.;
    for ( int i=0; i<NPixelsY; ++i) {
      m_PixelCenterPositionY[i] = m_Satellite.GetDetectorAveragePixelSizeY() * (static_cast<double>(i) - CenterID);
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Load ROOT histogram of charge induction efficiency
 */
bool NModuleDetectorEffectsEngineSciSimCIE::LoadChargeInductionEfficiency()
{
  TDirectory *OrgDirectory = gDirectory;

  if ( MFile::Exists(m_ChargeInductionEfficiencyFileName ) == false) {
    cout << "ERROR: unable to find charge induction efficiency file \""
	 << m_ChargeInductionEfficiencyFileName << "\"" << endl;
    gDirectory = OrgDirectory;
    return false;
  }

  m_ChargeInductionEfficiencyROOTFile = new TFile(m_ChargeInductionEfficiencyFileName.Data(), "READONLY");
  if (    m_ChargeInductionEfficiencyROOTFile == 0
       || m_ChargeInductionEfficiencyROOTFile->IsZombie() ) {
    cout << "ERROR: unable to open charge induction efficiency file \""
	 << m_ChargeInductionEfficiencyFileName << "\"" << endl;
    gDirectory = OrgDirectory;
    return false;
  }

  m_ChargeInductionEfficiency
    = (TH3D*) m_ChargeInductionEfficiencyROOTFile->Get(m_ChargeInductionEfficiencyHistName.Data());
  if (    m_ChargeInductionEfficiency == 0
       || m_ChargeInductionEfficiency->IsZombie() ) {
    cout << "ERROR: unable to get charge induction efficiency histogram \""
	 << m_ChargeInductionEfficiencyHistName << "\"" << endl;
    m_ChargeInductionEfficiencyROOTFile->Print();
    gDirectory = OrgDirectory;
    return false;
  }

  m_CIECorrectionFactor = 1.0 / m_ChargeInductionEfficiency->GetMaximum();



  // CIE Histogram is 3 by 3 pixels
  m_PixelWidthScaleFactorX
    = ((m_ChargeInductionEfficiency->GetXaxis()->GetXmax()
	- m_ChargeInductionEfficiency->GetXaxis()->GetXmin()) / 3.0)
    / m_Satellite.GetDetectorAveragePixelSizeX();

  m_PixelWidthScaleFactorY
    = ((m_ChargeInductionEfficiency->GetYaxis()->GetXmax()
	- m_ChargeInductionEfficiency->GetYaxis()->GetXmin()) / 3.0)
    / m_Satellite.GetDetectorAveragePixelSizeY();

  gDirectory = OrgDirectory;
  return true;
}

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 *   Interpolate values in 3-d histogram via trilinear interpolation
 *   Copy from TH3.cxx in ROOT ver. 5.26.00c
 */
double NModuleDetectorEffectsEngineSciSimCIE::InterpolateTH3(TH3* Histogram, double& x, double& y, double& z) {

  Int_t ubx = Histogram->GetXaxis()->FindBin(x);
  if ( x < Histogram->GetXaxis()->GetBinCenter(ubx) ) ubx -= 1;
  Int_t obx = ubx + 1;

  Int_t uby = Histogram->GetYaxis()->FindBin(y);
  if ( y < Histogram->GetYaxis()->GetBinCenter(uby) ) uby -= 1;
  Int_t oby = uby + 1;

  Int_t ubz = Histogram->GetZaxis()->FindBin(z);
  if ( z < Histogram->GetZaxis()->GetBinCenter(ubz) ) ubz -= 1;
  Int_t obz = ubz + 1;


  if (ubx <=0 || uby <=0 || ubz <= 0 ||
      obx > Histogram->GetNbinsX() || oby > Histogram->GetNbinsY() || obz > Histogram->GetNbinsZ() ) {
    Error("Interpolate","Cannot interpolate outside histogram domain.");
    return 0;
  }

  Double_t xw = Histogram->GetXaxis()->GetBinCenter(obx) - Histogram->GetXaxis()->GetBinCenter(ubx);
  Double_t yw = Histogram->GetYaxis()->GetBinCenter(oby) - Histogram->GetYaxis()->GetBinCenter(uby);
  Double_t zw = Histogram->GetZaxis()->GetBinCenter(obz) - Histogram->GetZaxis()->GetBinCenter(ubz);

  Double_t xd = (x - Histogram->GetXaxis()->GetBinCenter(ubx)) / xw;
  Double_t yd = (y - Histogram->GetYaxis()->GetBinCenter(uby)) / yw;
  Double_t zd = (z - Histogram->GetZaxis()->GetBinCenter(ubz)) / zw;


  Double_t v[] = { Histogram->GetBinContent( ubx, uby, ubz ), Histogram->GetBinContent( ubx, uby, obz ),
		   Histogram->GetBinContent( ubx, oby, ubz ), Histogram->GetBinContent( ubx, oby, obz ),
		   Histogram->GetBinContent( obx, uby, ubz ), Histogram->GetBinContent( obx, uby, obz ),
		   Histogram->GetBinContent( obx, oby, ubz ), Histogram->GetBinContent( obx, oby, obz ) };


  Double_t i1 = v[0] * (1 - zd) + v[1] * zd;
  Double_t i2 = v[2] * (1 - zd) + v[3] * zd;
  Double_t j1 = v[4] * (1 - zd) + v[5] * zd;
  Double_t j2 = v[6] * (1 - zd) + v[7] * zd;


  Double_t w1 = i1 * (1 - yd) + i2 * yd;
  Double_t w2 = j1 * (1 - yd) + j2 * yd;


  Double_t result = w1 * (1 - xd) + w2 * xd;

  return result;
}

////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineSciSimCIE::AnalyzeEvent(NEvent& Event)
{
  // Main data analysis routine, which updates the event to a new level

  // Step 0: Create diagnostics window information
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {
      // (a) Create the BEFORE info for the diagnostics window
      NOrientation O
	= m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(),
								Event.GetInteraction(i).GetTelescope(),
								Event.GetInteraction(i).GetDetector());
      MVector Pos = Event.GetInteraction(i).GetPosition();
      O.TransformOut(Pos);
      dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>
	(m_Diagnostics)->AddBefore(Pos, Event.GetInteraction(i).GetEnergy());
    }
  }

  // All charge sharing, energy-loss, etc. is handled by the 9-pixel-RMF
  // It takes each interaction and generates NPixelHits in 3-by-3 square around
  // the energy deposit.
  // How this is handles interally in the 9-pixel-RMF is still TBD

  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();

  double IdealEnergy = 0.0;
  double NoisedEnergy = 0.0;

  double ChargeInductionEfficiency;

  int xPixel = 0;
  int yPixel = 0;

  vector<NPixelHit> PixelHits;

  double DistanceXFromPixelCenter, DistanceYFromPixelCenter, DistanceZFromCathode;

  // Step 1:
  // Translate into pixel hits
  // This is still a dummy since in reality the 9-pixel-RMF will be used.
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {

      IdealEnergy = Event.GetInteraction(i).GetEnergy();

      xPixel = int((Event.GetInteraction(i).GetPosition().X() + m_Satellite.GetDetectorHalfDimension().X())
		   / (2*m_Satellite.GetDetectorHalfDimension().X() / NPixelsX));
      yPixel = int((Event.GetInteraction(i).GetPosition().Y() + m_Satellite.GetDetectorHalfDimension().Y())
		   / (2*m_Satellite.GetDetectorHalfDimension().Y() / NPixelsY));

      massert(xPixel >= 0 && xPixel < NPixelsX);
      massert(yPixel >= 0 && yPixel < NPixelsY);

      // Create 9 pixel hits
      for (int xp = xPixel-1; xp <= xPixel+1; ++xp) {
        for (int yp = yPixel-1; yp <= yPixel+1; ++yp) {
          if (xp < 0 || xp > NPixelsX) continue;
          if (yp < 0 || yp > NPixelsY) continue;

	  DistanceXFromPixelCenter = Event.GetInteraction(i).GetPosition().X() - m_PixelCenterPositionX[xp];
	  DistanceYFromPixelCenter = Event.GetInteraction(i).GetPosition().Y() - m_PixelCenterPositionY[yp];
	  DistanceZFromCathode     = Event.GetInteraction(i).GetPosition().Z();                         // Anode:-1 mm <--> Cathode:1 mm
	  DistanceZFromCathode     = m_Satellite.GetDetectorHalfDimension().Z() - DistanceZFromCathode; // Anode: 2 mm <--> Cathode:0 mm

	  DistanceXFromPixelCenter *= m_PixelWidthScaleFactorX;
	  DistanceYFromPixelCenter *= m_PixelWidthScaleFactorY;

	  int binX = m_ChargeInductionEfficiency->GetXaxis()->FindBin(DistanceXFromPixelCenter);
	  int binY = m_ChargeInductionEfficiency->GetYaxis()->FindBin(DistanceYFromPixelCenter);

	  if (    binX <= 1 || binX >= m_ChargeInductionEfficiency->GetNbinsX()
	       || binY <= 1 || binY >= m_ChargeInductionEfficiency->GetNbinsY() ) { // Out of CIE histogram range
	    ChargeInductionEfficiency = 0;
	  } else {
	    if ( gROOT->GetVersionInt() >= 52600 ) {
	      ChargeInductionEfficiency
		= m_ChargeInductionEfficiency->Interpolate(DistanceXFromPixelCenter,
							   DistanceYFromPixelCenter,
							   DistanceZFromCathode);
	    } else { // ROOT version 5.22.00 or earlier does not have Interpolate method in TH3 class
	      ChargeInductionEfficiency
		= InterpolateTH3(m_ChargeInductionEfficiency,
				 DistanceXFromPixelCenter,
				 DistanceYFromPixelCenter,
				 DistanceZFromCathode);
	    }
	  }

	  // NoisedEnergy = IdealEnergy * ChargeInductionEfficiency;
	  NoisedEnergy = IdealEnergy * ChargeInductionEfficiency * m_CIECorrectionFactor;
	  /* Do NOT add electric noise
	  if ( NoisedEnergy > 0.0 )
	    NoisedEnergy = gRandom->Gaus(NoisedEnergy, m_EnergyResolution.Eval(NoisedEnergy));
	  else
	    NoisedEnergy = gRandom->Gaus(NoisedEnergy, m_EnergyResolution.Eval(0.0));
	  */

          NPixelHit P;
          P.SetTelescope(Event.GetInteraction(i).GetTelescope());
          P.SetDetector(Event.GetInteraction(i).GetDetector());
          P.SetXPixel(xp);
          P.SetYPixel(yp);

	  P.SetPreTriggerSampleSum(0.0);
	  P.SetPostTriggerSampleSum(NoisedEnergy);

          // Zero values mean no trigger
	  P.SetIdealAverageDepth(Event.GetInteraction(i).GetPosition().Z());
	  P.SetIdealEnergy(IdealEnergy);

	  P.SetNoisedAverageDepth(Event.GetInteraction(i).GetPosition().Z());
	  P.SetNoisedEnergy(NoisedEnergy);

	  PixelHits.push_back(P);
        }
      }
    } else {
      // Shield hits:
      NShieldHit S;
      S.SetTelescope(Event.GetInteraction(i).GetTelescope());
      S.SetIdealEnergy(Event.GetInteraction(i).GetEnergy());
      Event.AddShieldHit(S);
    }
  }


  // Step 2:
  // Combine energy-deposits/pulse-heights in the same pixels:
  for (unsigned int p = 0; p < PixelHits.size(); ++p) {
    //cout<<p<<": "<<PixelHits[p].GetXPixel()<<":"<<PixelHits[p].GetYPixel()<<":"<<PixelHits[p].GetPostTriggerSampleSum()<<endl;
    // Check if a similar pixel hit is already in the event
    bool Merged = false;
    for (unsigned int e = 0; e < Event.GetNPixelHits(); ++e) {
      if (PixelHits[p].GetTelescope() == Event.GetPixelHit(e).GetTelescope() &&
          PixelHits[p].GetDetector()  == Event.GetPixelHit(e).GetDetector() &&
          PixelHits[p].GetXPixel()    == Event.GetPixelHit(e).GetXPixel() &&
          PixelHits[p].GetYPixel()    == Event.GetPixelHit(e).GetYPixel()) {

        //cout<<"Duplicate pixel --- start merging: "<<endl;
        Event.GetPixelHitRef(e).AddPreTriggerSampleSum(PixelHits[p].GetPreTriggerSampleSum());
        Event.GetPixelHitRef(e).AddPostTriggerSampleSum(PixelHits[p].GetPostTriggerSampleSum());
        // Use energy weighted depths:
        if (Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealEnergy() > 0.0) {
          Event.GetPixelHitRef(e).SetIdealAverageDepth((Event.GetPixelHit(e).GetIdealAverageDepth() * Event.GetPixelHit(e).GetIdealEnergy()
							+ PixelHits[p].GetIdealAverageDepth() * PixelHits[p].GetIdealEnergy())
						       / (Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealEnergy()));
        }
        if (Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedEnergy() > 0.0) {
          Event.GetPixelHitRef(e).SetNoisedAverageDepth((Event.GetPixelHit(e).GetNoisedAverageDepth() * Event.GetPixelHit(e).GetNoisedEnergy()
							 + PixelHits[p].GetNoisedAverageDepth() * PixelHits[p].GetNoisedEnergy())
							/ (Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedEnergy()));
        }

        Event.GetPixelHitRef(e).SetIdealEnergy(Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealEnergy());
        Event.GetPixelHitRef(e).SetNoisedEnergy(Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedEnergy());
        Merged = true;
        break;
      }
    }
    if (Merged == false) {
     Event.AddPixelHit(PixelHits[p]);
    }
  }


  // Step 3:
  // Broaden energy peak in eath pixel:
  for (unsigned int p = 0; p < Event.GetNPixelHits(); ++p) {
    double BroadenedEnergy = Event.GetPixelHit(p).GetPostTriggerSampleSum();

    if ( BroadenedEnergy > 0.0 )
      BroadenedEnergy = gRandom->Gaus(BroadenedEnergy, m_EnergyResolution.Eval(BroadenedEnergy));
    else
      BroadenedEnergy = gRandom->Gaus(BroadenedEnergy, m_EnergyResolution.Eval(0.0));

    Event.GetPixelHitRef(p).SetPostTriggerSampleSum(BroadenedEnergy);
  }

  // Step inf: Create the AFTER info for the diagnostics window
  for (unsigned int h = 0; h < Event.GetNPixelHits(); ++h) {
    //cout<<"N pixel hits: "<<Event.GetNPixelHits()<<endl;

    if (Event.GetTelescope() != 1) continue;

    NPixelHit& P = Event.GetPixelHitRef(h);

    // if (P.GetNoisedEnergy() <= 0.0) continue;
    if (P.GetNoisedEnergy() <= 2.0) continue; // 2.0 keV is dummy trigger threshold

    MVector Pos;
    Pos[0] = (0.5 + P.GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    Pos[1] = (0.5 + P.GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
    Pos[2] = P.GetNoisedAverageDepth();
    if (Pos[2] > m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = 0.999*m_Satellite.GetDetectorHalfDimension().Z();
    if (Pos[2] < -m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = -0.999*m_Satellite.GetDetectorHalfDimension().Z();

    NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), P.GetTelescope(), P.GetDetector());
    O.TransformOut(Pos);
    dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddAfter(Pos, P.GetPostTriggerSampleSum());
  }

  if (Event.GetNShieldHits() == 0 && Event.GetNPixelHits() == 0) {
    Event.SetBlocked(true);
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorEffectsEngineSciSimCIE::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  // NGUIOptions* Options = new NGUIOptions(this);
  NGUIOptions* Options = new NGUIOptionsDetectorEffectsEngineSciSimCIE(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineSciSimCIE::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* CIEFileNameNode = Node->GetNode("CIEFileName");
  if (CIEFileNameNode != 0) {
    m_ChargeInductionEfficiencyFileName = CIEFileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleDetectorEffectsEngineSciSimCIE::CreateXmlConfiguration()
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  // MXmlNode* SomeTagNode = new MXmlNode(Node, "CIEFileName", m_ChargeInductionEfficiencyFileName);
  new MXmlNode(Node, "CIEFileName", CleanPath(m_ChargeInductionEfficiencyFileName));

  return Node;
}

// NModuleDetectorEffectsEngineSciSimCIE.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
