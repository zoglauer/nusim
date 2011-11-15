/*
 * NModuleDetectorSimulatorDetailed.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorSimulatorDetailed
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorSimulatorDetailed.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TH1D.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NInteraction.h"
#include "NGUIOptionsDetectorSimulator.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorSimulatorDetailed)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorDetailed::NModuleDetectorSimulatorDetailed(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleDetectorSimulatorDetailed

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Detailed: Compton, Photo, Rayleigh";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorSimulatorDetailed";

  // Set the tool tip text
  m_ToolTip = "This is detector simulator with rudamentary support for Compton and photo electric effect simulations.";

  // Set the module type
  m_ModuleType = c_DetectorEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsDetectorSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsDetectorSimulator();
  
  m_UseBerylliumWindow = true;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorSimulatorDetailed::~NModuleDetectorSimulatorDetailed()
{
  // Delete this instance of NModuleDetectorSimulatorDetailed
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::Initialize()
{
  // Initialize the module 

  // Read photo effect data
  m_Energies.clear();
  m_Multiplicities.clear();
  for (unsigned int i = 0; i < 100; ++i) {
    TString EName = "$(NUSIM)/resource/data/detectorsim/PEE_Fluorescence_Energy.";
    EName += double(i)+0.5;
    EName += ".dat";
    EName.ReplaceAll(" ", "");
    
    MFunction E;
    if (E.Set(EName, "DP") == false) {
      cout<<"Unable to load: "<<EName<<endl;
      return false;
    }
    m_Energies.push_back(E);

    TString MName = "$(NUSIM)/resource/data/detectorsim/PEE_Fluorescence_Multiplicity.";
    MName += double(i)+0.5;
    MName += ".dat";
    MName.ReplaceAll(" ", "");
    
    MFunction M;
    if (M.Set(MName) == false) {
      cout<<"Unable to load: "<<MName<<endl;
      return false;
    }
    m_Multiplicities.push_back(M);
  }

  // Read Rayleigh data
  m_RayleighAngles.clear();
  for (unsigned int i = 2; i <= 98; i += 4) {
    TString EName = "$(NUSIM)/resource/data/detectorsim/RA_Angle.";
    EName += i;
    EName += ".dat";
    EName.ReplaceAll(" ", "");
    
    MFunction E;
    if (E.Set(EName, "DP", MFunction::c_InterpolationLinLin) == false) {
      cout<<"Unable to load: "<<EName<<endl;
      return false;
    }
    m_RayleighAngles.push_back(E);
  }

  m_NPhoto = 0;
  m_NCompton = 0;
  m_NRayleigh = 0;
  m_NFluorescence = 0;

//   TH1D* Hist = new TH1D("M", "M", 10, 0, 10);
//   for (unsigned int b = 0; b < 177000; ++b) {
//     Hist->Fill(m_Multiplicities[30].GetRandom());
//   }
//   TCanvas* C = new TCanvas();
//   C->cd();
//   Hist->Draw();
//   C->Update();

  if (m_UseBerylliumWindow == false) {
    mout<<"Attention: Beryllium window is deactivated in detailed detector simulator"<<endl;
  }
  
  m_NBlockedBeryllium = 0;
  m_NBlockedInDetectorPlane = 0;
  m_NBlockedPassedWithoutInteractionThroughDetector = 0;
  m_NPhotonsEnteringDetector = 0;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::AnalyzeEvent(NEvent& Event) 
{
  // Start with a photon and end with ideal interaction information in the detector
	// At the end, the hits are in the individual CZT detectors  

  // Limitations of the current approach:
  // * No photon propagation from detector to detector
  // * All electrons are stopped immediately
  // * Rayleigh: No direction change
  // * Compton: Ignores binding energy, Doppler broading, and polarization
  // * Photo: No shells, no fluoresence

  // Generate interactions from the photon and add them to the event

  // (a) Retrieve the current photon parameters
  NPhoton Photon = Event.GetCurrentPhoton();

  // (b) Retrieve the focal plane orientation and position relative to the photon
  NOrientation Orientation = m_Satellite.GetOrientationFocalPlaneModule(Event.GetTime(), Event.GetTelescope());

  //cout<<"Pos on det: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (c) Switch into focal bench coordinate system:
  Orientation.TransformIn(Photon);

  // (d) Propagate the photon towards the beryllium window
  MVector BerylliumTop(0.0, 0.0, 200); // rough Beryllium position -- real one doesn't matter 
  MVector BerylliumNormal(0.0, 0.0, 1.0);
  PropagateToPlane(Photon, BerylliumTop, BerylliumNormal);

  // (e) Calculate absorption probabilty in Beryllium
  //     --- IGNORE Compton/Rayleigh scatter probability and other effects
  if (m_UseBerylliumWindow == true) {
    double BerylliumThickness = 0.1;
    double BerylliumLength = gRandom->Exp(1.0/m_Satellite.GetBerylliumAbsorptionCoefficient(Photon.GetEnergy()));
    if (BerylliumLength < BerylliumThickness) {
      Event.SetBlocked(true);
      ++m_NBlockedBeryllium;
      return true;
    }
  }

  // For testing: Set direction 0 0 -1 and position in the center of one of the detectors
  // Photon.SetDirection(MVector(0, 0, -1));
  // Photon.SetPosition(MVector(10.25, 10.25, 100));

  // (f) Propagate the photon towards the top of the detector plane:
  MVector DetectorTop(0.0, 0.0, m_Satellite.GetDetectorHalfDimension().Z());
  MVector DetectorNormal(0.0, 0.0, 1.0);
  PropagateToPlane(Photon, DetectorTop, DetectorNormal);

  //cout<<"Pos on det: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  // (g) Switch to the detector coordinate system:
  NOrientation DetectorOrientation;
  int Detector = 0;
  for (unsigned int d = 1; d <= 4; ++d) {
    DetectorOrientation = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetTelescope(), d);
    DetectorOrientation.TransformIn(Photon);
    //cout<<d<<": "<<Photon.GetPosition()<<endl;
    if (fabs(Photon.GetPosition().X()) <= m_Satellite.GetDetectorHalfDimension().X() &&
        fabs(Photon.GetPosition().Y()) <= m_Satellite.GetDetectorHalfDimension().Y() &&
        fabs(Photon.GetPosition().Z()) <= m_Satellite.GetDetectorHalfDimension().Z()) {
      Detector = d;
      break;
    } else {
      DetectorOrientation.TransformOut(Photon);
    }
  }
    
  //cout<<"Pos in det: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;

  if (Detector == 0) {
    // We did not hit the detector and can stop here
    //cout<<"Did not hit detector..."<<endl;
    Event.SetBlocked(true);
    ++m_NBlockedInDetectorPlane;
    DetectorOrientation.TransformOut(Photon);
    Orientation.TransformOut(Photon);
    return true;
  }
  
  // (h) Simulate an infinite plane of CZT in the detector module coordinate system
  ++m_NPhotonsEnteringDetector;
  
  // Really simulate
  SimulatePhoton(Photon, Event);
  
  // Check if we really passed through the detector unchanged
  if (Event.GetNInteractions() == 0) {
    Event.SetBlocked(true);
    ++m_NBlockedPassedWithoutInteractionThroughDetector;
    DetectorOrientation.TransformOut(Photon);
    Orientation.TransformOut(Photon);
    return true;
  }
  
  // (i) Transform the photon back -- just for book keeping
  DetectorOrientation.TransformOut(Photon);
  Orientation.TransformOut(Photon);
  //cout<<"Pos in FP: "<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<endl;
  Event.SetCurrentPhoton(Photon);

  // (j) Complete the IA
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    Event.GetInteractionRef(i).SetTelescope(Event.GetTelescope());
    Event.GetInteractionRef(i).SetDetector(Detector);
    //cout<<Event.GetInteractionRef(i).ToString()<<endl;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::SimulatePhoton(NPhoton& Photon, NEvent& Event)
{
  // Simulate the photon
  // This function:
  // (1) decides on the next interaction (Photo, Compton, Rayleigh),
  // (2) propagates the photon to the interaction position,
  // (3) verifies that we are still in the detector
  // (4) Switches to the process
  // (5) calls itself as long as the photon still has energy

  const unsigned int Photo = 0;
  const unsigned int Compton = 1;
  const unsigned int Rayleigh = 2;
  const unsigned int MaxIA = Rayleigh;

  // (1) Determine the next interaction:
  vector<double> Length(3, 0);
  Length[Photo] = gRandom->Exp(1.0/m_Satellite.GetCZTPhotoAbsorptionCoefficient(Photon.GetEnergy()));
  Length[Compton] = gRandom->Exp(1.0/m_Satellite.GetCZTComptonAbsorptionCoefficient(Photon.GetEnergy()));
  Length[Rayleigh] = gRandom->Exp(1.0/m_Satellite.GetCZTRayleighAbsorptionCoefficient(Photon.GetEnergy()));

  //cout<<Length[Photo]<<":"<<Length[Compton]<<":"<<Length[Rayleigh]<<endl;

  unsigned int NextIA = Photo;
  double NextLength = Length[Photo];
  for (unsigned int i = 1; i <= MaxIA; ++i) {
    if (Length[i] < NextLength) {
      NextLength = Length[i];
      NextIA = i;
    }
  }
  //cout<<"Next IA: "<<NextIA<<" with "<<NextLength<<" mm"<<endl;

  // (2) Progate photon to IA position:
  Propagate(Photon, NextLength);

  // (3) If we are no longer in the CZT -- assume it escaped
  if (fabs(Photon.GetPosition().X()) > m_Satellite.GetDetectorHalfDimension().X() ||
      fabs(Photon.GetPosition().Y()) > m_Satellite.GetDetectorHalfDimension().Y() ||
      fabs(Photon.GetPosition().Z()) > m_Satellite.GetDetectorHalfDimension().Z()) {
    if (Event.GetNInteractions() == 0) {
      Event.SetBlocked(true);
    }
    return false;
  }

  // (4) Do the process
  if (NextIA == Photo) {
    if (SimulatePhoto(Photon, Event) == false) return false;
  } else if (NextIA == Compton) {
    if (SimulateCompton(Photon, Event) == false) return false;
  } else if (NextIA == Rayleigh) {
    if (SimulateRayleigh(Photon, Event) == false) return false;
  }

  // (5) Call your self in case the photon has still energy:
  if (Photon.GetEnergy() < 0.001) {
    cout<<"If everything is right the photon should have no energy left..."<<Photon.GetEnergy()<<endl;
    return false;
  }

  return SimulatePhoton(Photon, Event);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::SimulateCompton(NPhoton& Photon, NEvent& Event)
{
  // The scattered gamma energy is sampled according to Klein-Nishina formula.
  // The random number techniques of Butcher & Messel are used (Nuc Phys 20(1960), 15).

  ++m_NCompton;

  // Limitation: Effects due to binding of atomic electrons as well as polarization are neglected.

  double Ei = Photon.GetEnergy();
  double Ei_m = Ei / 510.998910;
  MVector Dir0 = Photon.GetDirection();


  // Simulate the gamma-ray according to Butcher & Messel: Nuc Phys 20(1960), 15

  double Epsilon = 0.0;
  double EpsilonSquare = 0.0;
  double OneMinusCosTheta = 0.0;
  double SinThetaSquared = 0.0;

  double Epsilon0 = 1./(1. + 2.*Ei_m);
  double Epsilon0Square = Epsilon0*Epsilon0;
  double Alpha1 = - log(Epsilon0);
  double Alpha2 = 0.5*(1.- Epsilon0Square);

  double Reject = 0.0;

  do {
    if (Alpha1/(Alpha1+Alpha2) > gRandom->Rndm()) {
      Epsilon = exp(-Alpha1*gRandom->Rndm());
      EpsilonSquare = Epsilon*Epsilon; 
    } else {
      EpsilonSquare = Epsilon0Square + (1.0 - Epsilon0Square)*gRandom->Rndm();
      Epsilon = sqrt(EpsilonSquare);
    }

    OneMinusCosTheta = (1.- Epsilon)/(Epsilon*Ei_m);
    SinThetaSquared = OneMinusCosTheta*(2.-OneMinusCosTheta);
    Reject = 1.0 - Epsilon*SinThetaSquared/(1.0 + EpsilonSquare);

  } while (Reject < gRandom->Rndm());
 

  // Determine angles

  double CosTeta = 1.0 - OneMinusCosTheta; 
  double SinTeta = sqrt(SinThetaSquared);
  double Phi = 2*c_Pi * gRandom->Rndm();
   

  // Set the new photon parameters

  MVector Dg(SinTeta*cos(Phi), SinTeta*sin(Phi), CosTeta);
  Dg.RotateReferenceFrame(Dir0);
  Photon.SetDirection(Dg);

  double Eg = Epsilon*Ei;
  double Ee = Ei - Eg;
  Photon.SetEnergy(Eg);


  // Directly deposit the electron energy

  NInteraction I;
  I.SetIsDetector(true);
  I.SetPosition(Photon.GetPosition());
  I.SetEnergy(Ee);  
  Event.AddInteraction(I);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::SimulatePhoto(NPhoton& Photon, NEvent& Event)
{
  //! Simulate photo electric effect

  ++m_NPhoto;

  double Energy = Photon.GetEnergy();
  unsigned int EnergyBin = int(Photon.GetEnergy()); 
  double PhotonEnergy = 0.0;
  int Multiplicity = 0;

  if (Energy > 3) { // There is some noise in the data below 3 keV
    if (EnergyBin > m_Energies.size() - 1) {
      cout<<"Photo effect simulation is limited to energies up to "<<m_Energies.size()<<" keV - using highest energy bin."<<endl;
      EnergyBin = m_Energies.size() - 1;
    }
    //cout<<"Energy: "<<Energy<<" - bin: "<<EnergyBin<<endl;
    
    // Step 1: Determine random multiplicity
    double RM = m_Multiplicities[EnergyBin].GetRandom();
    //cout<<Energy<<":"<<EnergyBin<<":"<<RM<<endl;
    Multiplicity = int(RM + 0.5);
    //cout<<"Multiplicity: "<<Multiplicity<<" from: "<<RM<<endl;
    
    // If the multiplicty is larger than one assume one:
    if (Multiplicity > 1) Multiplicity = 1;
    
    // Step 2: Sample a photon if multiplicity is 1
    if (Multiplicity == 1) {
      int Counter = 100;
      do {
        PhotonEnergy = m_Energies[EnergyBin].GetRandom();
       
        if (--Counter == 0) {
          merr<<"Caught in (almost) infinite loop... - ignoring fluorenscence for photon energy "<<Energy<<" keV..."<<show;
          Multiplicity = 0;
          break;
        }
      } while (PhotonEnergy > Energy);
    }
  }  

  // Step 3: Deposit the non-fluorescence energy
  NInteraction I;
  I.SetIsDetector(true);
  I.SetPosition(Photon.GetPosition());
  I.SetEnergy(Photon.GetEnergy() - PhotonEnergy);  
  Event.AddInteraction(I);
  
  if (Multiplicity == 1) {
    ++m_NFluorescence;

    Photon.SetEnergy(PhotonEnergy);
    MVector Random;
    Random.SetMagThetaPhi(1.0, acos(1-2*gRandom->Rndm()), gRandom->Rndm()*2*c_Pi); 
    Photon.SetDirection(Random);
    
    //cout<<"New photon with energy: "<<PhotonEnergy<<endl;
    
    return true;
  } else {
    Photon.SetEnergy(0.0);
  }

  // False means simulation is finished...
  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::SimulateRayleigh(NPhoton& Photon, NEvent& Event)
{
  //! Simulate Rayleigh scattering
  
  ++m_NRayleigh;

  unsigned int EnergyBin = int(Photon.GetEnergy())/4;
  if (EnergyBin < 0 || EnergyBin >= m_RayleighAngles.size()) {
    merr<<"something is wrong with your RayleighAngle bins"<<endl;
    return true;
  }
  double ThetaChange = m_RayleighAngles[EnergyBin].GetRandom();
  double PhiChange = 2.0*c_Pi*gRandom->Rndm();

  MVector NewDir;
  NewDir.SetMagThetaPhi(1.0, ThetaChange*c_Rad, PhiChange);
  NewDir.Unitize();

  MVector PhotonDir = Photon.GetDirection();
  PhotonDir.RotateReferenceFrame(NewDir);
  Photon.SetDirection(PhotonDir);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::Finalize()
{
  cout<<endl;
  cout<<"Detector interaction simulator (detailed):"<<endl;
  cout<<"  Interaction types:"<<endl;
  cout<<"    Photo:        "<<m_NPhoto<<endl;
  cout<<"    Compton:      "<<m_NCompton<<endl;
  cout<<"    Rayleigh:     "<<m_NRayleigh<<endl;
  cout<<"    Fluorescence: "<<m_NFluorescence<<endl;
  cout<<"  Blocked events:"<<endl;
  cout<<"    In Beryllium:            "<<m_NBlockedBeryllium<<endl;
  cout<<"    In detector plane:       "<<m_NBlockedInDetectorPlane<<endl;
  cout<<"    No detector interaction: "<<m_NBlockedPassedWithoutInteractionThroughDetector<<" (Pass throughs and Rayleigh back scatters)"<<endl;
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorSimulatorDetailed::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsDetectorSimulator* Options = new NGUIOptionsDetectorSimulator(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorSimulatorDetailed::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* UseBerylliumWindowNode = Node->GetNode("UseBerylliumWindow");
  if (UseBerylliumWindowNode != 0) {
    m_UseBerylliumWindow = UseBerylliumWindowNode->GetValueAsBoolean();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleDetectorSimulatorDetailed::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  new MXmlNode(Node, "UseBerylliumWindow", m_UseBerylliumWindow);

  return Node;
}


// NModuleDetectorSimulatorDetailed.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
