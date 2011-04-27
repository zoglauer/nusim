/*
 * NModuleDetectorCalibratorSciSimCIE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorCalibratorSciSimCIE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorCalibratorSciSimCIE.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "TRandom.h"

// NuSTAR libs:
#include "NHit.h"
#include "NPixelHit.h"
// #include "NGUIOptions.h"
#include "NGUIOptionsDetectorCalibratorSciSimCIE.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorCalibratorSciSimCIE)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorCalibratorSciSimCIE::NModuleDetectorCalibratorSciSimCIE(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorCalibratorSciSimCIE

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "SciSimCIE thread calibrator";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEnergyAndDepthCalibrationSciSimCIE";

  // Set the tool tip text
  m_ToolTip = "This is the detector data calibration engine which belongs to the science simulator thread";

  // Set all types this modules handles
  m_ModuleType = c_DetectorCalibrator;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsEnergyAndDepthCalibration)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsEnergyAndDepthCalibration();


  // set gain and offset for single-trigger events
  // m_GainSingleTrigger = 1.0;
  // m_OffsetSingleTrigger = 0.0;
  m_GainSingleTrigger = 1.00144e+00;
  m_OffsetSingleTrigger = 2.67552e-02;
  // set gain and offset for double-trigger events
  // m_GainDoubleTrigger = 1.0;
  // m_OffsetDoubleTrigger = 0.0;
  m_GainDoubleTrigger = 1.00387e+00;
  m_OffsetDoubleTrigger = -6.59802e-02;
  // set gain and offset for triple-trigger events
  // m_GainTripleTrigger = 1.0;
  // m_OffsetTripleTrigger = 0.0;
  m_GainTripleTrigger = 1.01172e+00;
  m_OffsetTripleTrigger = 1.73344e-01;
  // set gain and offset for quadruple-trigger events
  // m_GainQuadrupleTrigger = 1.0;
  // m_OffsetQuadrupleTrigger = 0.0;
  m_GainQuadrupleTrigger = 1.01066e+00;
  m_OffsetQuadrupleTrigger = -3.17478e-01;

  m_DepthCutFileName = "$NUSIM/resource/data/DepthCut.root";
  m_DepthCutROOTFile = 0;
  m_DepthCutSingleTrigger = 0;
  m_DepthCutDoubleTrigger = 0;

  // 0.3 means typical energy resolution (FWHM) of observed pedestal signals
  m_DepthCutOffsetY = -0.3;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorCalibratorSciSimCIE::~NModuleDetectorCalibratorSciSimCIE()
{
  // Delete this instance of NModuleDetectorCalibratorSciSimCIE
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorSciSimCIE::Initialize()
{
  // Initialize the module
  // Load the depth cut from ROOT file
  MFile::ExpandFileName(m_DepthCutFileName);
  if (LoadDepthCutSpline() == false) {
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Load ROOT TSpline3 object for depth cut
 */
bool NModuleDetectorCalibratorSciSimCIE::LoadDepthCutSpline()
{
  TDirectory *OrgDirectory = gDirectory;

  m_DepthCutSingleTriggerName = "DepthCutNTrigs1Spline";
  m_DepthCutDoubleTriggerName = "DepthCutNTrigs2Spline";

  if ( m_DepthCutROOTFile != 0 ) {
    if ( m_DepthCutROOTFile->GetName() == m_DepthCutFileName ) {
      mout << "ROOT file name for depth cut is the same. Skip to load ROOT file for depth cut." << show;
      return true;
    } else {
      m_DepthCutROOTFile->Close();
      delete m_DepthCutROOTFile;
      m_DepthCutROOTFile = 0;
      m_DepthCutSingleTrigger = 0;
      m_DepthCutDoubleTrigger = 0;
    }
  }

  if ( MFile::Exists(m_DepthCutFileName) == false) {
    cout << "ERROR: unable to find ROOT file for depth cut \""
         << m_DepthCutFileName << "\"" << endl;
    gDirectory = OrgDirectory;
    return false;
  }

  m_DepthCutROOTFile = new TFile(m_DepthCutFileName.Data(), "READONLY");
  if ( m_DepthCutROOTFile == 0 || m_DepthCutROOTFile->IsZombie() ) {
    cout << "ERROR: unable to open ROOT file for depth cut \""
         << m_DepthCutFileName << "\"" << endl;
    gDirectory = OrgDirectory;
    return false;
  }

  m_DepthCutSingleTrigger
    = (TSpline3*) m_DepthCutROOTFile->Get(m_DepthCutSingleTriggerName.Data());
  if (    m_DepthCutSingleTrigger == 0 || m_DepthCutSingleTrigger->IsZombie() ) {
    cout << "ERROR: unable to get ROOT object for depth cut \""
         << m_DepthCutSingleTriggerName << "\"" << endl;
    m_DepthCutROOTFile->Print();
    gDirectory = OrgDirectory;
    return false;
  }

  m_DepthCutDoubleTrigger
    = (TSpline3*) m_DepthCutROOTFile->Get(m_DepthCutDoubleTriggerName.Data());
  if (    m_DepthCutDoubleTrigger == 0 || m_DepthCutDoubleTrigger->IsZombie() ) {
    cout << "ERROR: unable to get ROOT object for depth cut \""
         << m_DepthCutDoubleTriggerName << "\"" << endl;
    m_DepthCutROOTFile->Print();
    gDirectory = OrgDirectory;
    return false;
  }

  gDirectory = OrgDirectory;
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorSciSimCIE::AnalyzeEvent(NEvent& Event)
{
  // Main data analysis routine, which updates the event to a new level

  // Upgrade from pixel hits to reals hits

  double NPixelsX = m_Satellite.GetDetectorPixelsX();
  double NPixelsY = m_Satellite.GetDetectorPixelsY();

  for (unsigned int i = 0; i < Event.GetNNinePixelHits(); ++i) {
    NNinePixelHit& Niner = Event.GetNinePixelHitRef(i);

    NHit H;

    mimp<<"Do we really want to randomize the position within the pixel?"<<show;
    double x = (gRandom->Rndm() + Niner.GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    double y = (gRandom->Rndm() + Niner.GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
    //double x = (0.5 + Niner.GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    //double y = (0.5 + Niner.GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
    double z = Niner.GetNoisedAverageDepth();

    // Randomize x & y within pixel:

    // We have to calculate the depth at some point...
    MVector Position(x, y, z);

    //cout<<"Pos calib (CZT): ID="<<i<<": x="<<Niner.GetXPixel()<<"  y="<<Niner.GetYPixel()<<"   Pos: "<<Position<<endl;

    // Rotate to module:
    NOrientation Orientation =
      m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Niner.GetTelescope(), Niner.GetDetector());
    Orientation.TransformOut(Position);

    //cout<<"Pos calib (FP): ID="<<i<<": "<<Position<<endl;

    H.SetTelescope(Niner.GetTelescope());
    H.SetDetector(Niner.GetDetector());

    H.SetPosition(Position);
    H.SetPositionResolution(MVector(0.0, 0.0, 0.0));

    double TrigEnergy  = 0.0;
    double NonTrigEnergy = 0.0;
    double Energies[9];

    for ( int j=0; j<9; ++j ) {
      Energies[j] = Niner.GetPostTriggerSampleSum(j+1) - Niner.GetPreTriggerSampleSum(j+1);
      if ( Niner.GetTrigger(j+1) == true ) {
	TrigEnergy    += Energies[j];
      } else {
	NonTrigEnergy += Energies[j];
      }
    }

    double ReconstructedEnergy;
    int NTrigs = Niner.GetNTriggers();

    if ( NTrigs == 9 ) {
      ReconstructedEnergy = TrigEnergy;
    } else {
      ReconstructedEnergy = TrigEnergy - NonTrigEnergy / (9 - NTrigs) * NTrigs;
    }

    if      ( NTrigs == 1 )
      ReconstructedEnergy = ReconstructedEnergy * m_GainSingleTrigger + m_OffsetSingleTrigger;
    else if ( NTrigs == 2 )
      ReconstructedEnergy = ReconstructedEnergy * m_GainDoubleTrigger + m_OffsetDoubleTrigger;
    else if ( NTrigs == 3 )
      ReconstructedEnergy = ReconstructedEnergy * m_GainTripleTrigger + m_OffsetTripleTrigger;
    else if ( NTrigs == 4 )
      ReconstructedEnergy = ReconstructedEnergy * m_GainQuadrupleTrigger + m_OffsetQuadrupleTrigger;

    H.SetEnergy(ReconstructedEnergy);
    H.SetEnergyResolution(0.0);



    //====================================
    // Set flag for bad depth calibration
    //====================================

    for (unsigned int j = 0; j < Event.GetNInteractions(); ++j) {
      double InteractionPositionZ = Event.GetInteraction(j).GetPosition().Z(); // Anode:-1 mm <--> Cathode:1 mm
      if (    InteractionPositionZ < -m_Satellite.GetDetectorHalfDimension().Z()
	   || InteractionPositionZ >  m_Satellite.GetDetectorHalfDimension().Z() ) {
	H.SetBadDepthCalibration(true);
      }
    }


    //========================
    // Set flag for depth cut
    //========================
    H.SetDepthCut(false); // valid event

    if ( NTrigs == 1 ) {

      double CutEnergyY = m_DepthCutOffsetY * sqrt(8.0 / (9.0-NTrigs));

      if ( ReconstructedEnergy < m_DepthCutSingleTrigger->GetXmin() ) {
	CutEnergyY += 0.0;
      } else if ( ReconstructedEnergy > m_DepthCutSingleTrigger->GetXmax() ) {
	CutEnergyY += m_DepthCutSingleTrigger->Eval(m_DepthCutSingleTrigger->GetXmax()) / m_DepthCutSingleTrigger->GetXmax() * ReconstructedEnergy;
      } else {
	CutEnergyY += m_DepthCutSingleTrigger->Eval(ReconstructedEnergy);
      }

      if ( NonTrigEnergy / (9 - NTrigs) < CutEnergyY )
	H.SetDepthCut(true); // invalid event filtered out by depth cut

    } else if ( NTrigs == 2 ) {

      double CutEnergyY = m_DepthCutOffsetY * sqrt(8.0 / (9.0-NTrigs));

      if ( ReconstructedEnergy < m_DepthCutDoubleTrigger->GetXmin() ) {
	CutEnergyY += 0.0;
      } else if ( ReconstructedEnergy > m_DepthCutDoubleTrigger->GetXmax() ) {
	CutEnergyY += m_DepthCutDoubleTrigger->Eval(m_DepthCutDoubleTrigger->GetXmax()) / m_DepthCutDoubleTrigger->GetXmax() * ReconstructedEnergy;
      } else {
	CutEnergyY += m_DepthCutDoubleTrigger->Eval(ReconstructedEnergy);
      }

      if ( NonTrigEnergy / (9 - NTrigs) < CutEnergyY )
	H.SetDepthCut(true);  // invalid event filtered out by depth cut

    }

    // Comment in if you comment out hit merging:
    Event.AddHit(H);
  }


  /* Old code:
  vector<NHit> Hits;
  for (unsigned int i = 0; i < Event.GetNPixelHits(); ++i) {
    NHit H;

    double x = (0.5 + Event.GetPixelHit(i).GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    double y = (0.5 + Event.GetPixelHit(i).GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();

		double z = m_Satellite.GetDetectorHalfDimension().Z()*(4*Event.GetPixelHit(i).GetADUnitsAnode()/Event.GetPixelHit(i).GetADUnitsCathode() - 3);

    // We have to calculate the depth at some point...
    MVector Position(x, y, z);

    //cout<<"Pos calib (CZT): ID="<<i<<": x="<<Event.GetPixelHit(i).GetXPixel()<<"  y="<<Event.GetPixelHit(i).GetYPixel()<<"   Pos: "<<Position<<endl;

    // Rotate to module:
    NOrientation Orientation =
      m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetPixelHit(i).GetTelescope(), Event.GetPixelHit(i).GetDetector());
    Orientation.TransformOut(Position);

    //cout<<"Pos calib (FP): ID="<<i<<": "<<Position<<endl;

    H.SetTelescope(Event.GetPixelHit(i).GetTelescope());
    H.SetDetector(Event.GetPixelHit(i).GetDetector());

    H.SetPosition(Position);
    H.SetPositionResolution(MVector(0.0, 0.0, 0.0));
    H.SetEnergy(Event.GetPixelHit(i).GetADUnitsCathode());
    H.SetEnergyResolution(0.0);

    Hits.push_back(H);

    // Comment in if you comment out hit merging:
    // Event.AddHit(H);
  }

  // If we have multiple hits, merge them by combining their energy, and energy weighting their position: pc = (E1*P1 + E2*P2)/(E1+E2) --- that's what we used for our MEGA Silicon detectors
  if (Hits.size() > 1) {
    double CombinedEnergy = Hits[0].GetEnergy();
    MVector EnergyWeightedPosition = Hits[0].GetPosition();

    for (unsigned int h = 1; h < Hits.size(); ++h) {
      EnergyWeightedPosition = (EnergyWeightedPosition*CombinedEnergy + Hits[h].GetEnergy()*Hits[h].GetPosition()) * (1.0/(CombinedEnergy + Hits[h].GetEnergy()));
      CombinedEnergy += Hits[h].GetEnergy();
    }
    Hits[0].SetEnergy(CombinedEnergy);
    Hits[0].SetPosition(EnergyWeightedPosition);
    Event.AddHit(Hits[0]);
  } else if (Hits.size() == 1) {
    Event.AddHit(Hits[0]);
  }

  if (Event.GetNHits() > 0 && Event.GetNInteractions() > 0) {
    // Debugging: Determine the average distance between the first interaction point and the hit position in CZT pixels:
    MVector Dist = Event.GetInteraction(0).GetPosition();
    NOrientation Orientation =
      m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetHit(0).GetTelescope(), Event.GetHit(0).GetDetector());
    Orientation.TransformOut(Dist);

    Dist -= Hits[0].GetPosition(); // now we are a real distance...

    cout<<"NUSTAR calib: Distance hit - first IA in FPM coordinates in CZT pixels: x="<<Dist.X()/m_Satellite.GetDetectorAveragePixelSizeX()<<",  y="<<Dist.Y()/m_Satellite.GetDetectorAveragePixelSizeY()<<endl;
  }
  */


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorCalibratorSciSimCIE::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  // NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);
  NGUIOptionsDetectorCalibratorSciSimCIE* Options = new NGUIOptionsDetectorCalibratorSciSimCIE(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);

}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorCalibratorSciSimCIE::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode.GetValue();
  }
  */

  MXmlNode* GainSingleTriggerNode = Node->GetNode("GainSingleTrigger");
  if (GainSingleTriggerNode != 0) {
    m_GainSingleTrigger = GainSingleTriggerNode->GetValueAsDouble();
  }
  MXmlNode* OffsetSingleTriggerNode = Node->GetNode("OffsetSingleTrigger");
  if (OffsetSingleTriggerNode != 0) {
    m_OffsetSingleTrigger = OffsetSingleTriggerNode->GetValueAsDouble();
  }

  MXmlNode* GainDoubleTriggerNode = Node->GetNode("GainDoubleTrigger");
  if (GainDoubleTriggerNode != 0) {
    m_GainDoubleTrigger = GainDoubleTriggerNode->GetValueAsDouble();
  }
  MXmlNode* OffsetDoubleTriggerNode = Node->GetNode("OffsetDoubleTrigger");
  if (OffsetDoubleTriggerNode != 0) {
    m_OffsetDoubleTrigger = OffsetDoubleTriggerNode->GetValueAsDouble();
  }

  MXmlNode* GainTripleTriggerNode = Node->GetNode("GainTripleTrigger");
  if (GainTripleTriggerNode != 0) {
    m_GainTripleTrigger = GainTripleTriggerNode->GetValueAsDouble();
  }
  MXmlNode* OffsetTripleTriggerNode = Node->GetNode("OffsetTripleTrigger");
  if (OffsetTripleTriggerNode != 0) {
    m_OffsetTripleTrigger = OffsetTripleTriggerNode->GetValueAsDouble();
  }

  MXmlNode* GainQuadrupleTriggerNode = Node->GetNode("GainQuadrupleTrigger");
  if (GainQuadrupleTriggerNode != 0) {
    m_GainQuadrupleTrigger = GainQuadrupleTriggerNode->GetValueAsDouble();
  }
  MXmlNode* OffsetQuadrupleTriggerNode = Node->GetNode("OffsetQuadrupleTrigger");
  if (OffsetQuadrupleTriggerNode != 0) {
    m_OffsetQuadrupleTrigger = OffsetQuadrupleTriggerNode->GetValueAsDouble();
  }

  MXmlNode* DepthCutFileNameNode = Node->GetNode("DepthCutFileName");
  if (DepthCutFileNameNode != 0) {
    m_DepthCutFileName = DepthCutFileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleDetectorCalibratorSciSimCIE::CreateXmlConfiguration()
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  new MXmlNode(Node, "GainSingleTrigger",   m_GainSingleTrigger);
  new MXmlNode(Node, "OffsetSingleTrigger", m_OffsetSingleTrigger);

  new MXmlNode(Node, "GainDoubleTrigger",   m_GainDoubleTrigger);
  new MXmlNode(Node, "OffsetDoubleTrigger", m_OffsetDoubleTrigger);

  new MXmlNode(Node, "GainTripleTrigger",   m_GainTripleTrigger);
  new MXmlNode(Node, "OffsetTripleTrigger", m_OffsetTripleTrigger);

  new MXmlNode(Node, "GainQuadrupleTrigger",   m_GainQuadrupleTrigger);
  new MXmlNode(Node, "OffsetQuadrupleTrigger", m_OffsetQuadrupleTrigger);

  new MXmlNode(Node, "DepthCutFileName", CleanPath(m_DepthCutFileName));


  return Node;
}


// NModuleDetectorCalibratorSciSimCIE.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
