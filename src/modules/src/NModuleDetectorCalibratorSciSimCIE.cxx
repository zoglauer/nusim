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
#include "NGUIOptions.h"


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

    for ( int i=0; i<9; ++i ) {
      Energies[i]
	= Event.GetNinePixelHit(0).GetPostTriggerSampleSum(i+1)
	- Event.GetNinePixelHit(0).GetPreTriggerSampleSum(i+1);

      if ( Event.GetNinePixelHit(0).GetTrigger(i+1) == true )
	TrigEnergy += Energies[i];
      else
	NonTrigEnergy += Energies[i];
    }

    double ReconstructedEnergy;
    int NTrigs = Event.GetNPixelTriggers();

    if ( NTrigs == 9 ) {
      ReconstructedEnergy = TrigEnergy;
    } else {
      ReconstructedEnergy = TrigEnergy - NonTrigEnergy / (9 - NTrigs) * NTrigs;
    }

    H.SetEnergy(ReconstructedEnergy);
    H.SetEnergyResolution(0.0);

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

  NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);

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

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleDetectorCalibratorSciSimCIE::CreateXmlConfiguration()
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorCalibratorSciSimCIE.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
