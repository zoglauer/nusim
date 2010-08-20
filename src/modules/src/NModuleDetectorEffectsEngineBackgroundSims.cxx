/*
 * NModuleDetectorEffectsEngineBackgroundSims.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorEffectsEngineBackgroundSims
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorEffectsEngineBackgroundSims.h"

// Standard libs:
#include <cmath>
using namespace std;

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MAssert.h"

// NuSTAR libs:
#include "NGUIOptions.h"
#include "NGUIDiagnosticsDetectorEffectsEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorEffectsEngineBackgroundSims)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineBackgroundSims::NModuleDetectorEffectsEngineBackgroundSims(NSatellite& Satellite) : NModuleDetectorEffectsEngineSciSim(Satellite)
{
  // Construct an instance of NModuleDetectorEffectsEngineBackgroundSims

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Replica of background sims";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorEffectsEngineBackgroundSims";

  // Set the tool tip text
  m_ToolTip = "This is a replica of the detector effects engine used for the original background analysis.";

  // Set the module type
  m_ModuleType = c_DetectorEffectsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = true;
  m_Diagnostics = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineBackgroundSims::~NModuleDetectorEffectsEngineBackgroundSims()
{
  // Delete this instance of NModuleDetectorEffectsEngineBackgroundSims
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineBackgroundSims::Initialize()
{
  // Initialize the module 

  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsDetectorEffectsEngine();
  dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(), 
                                                                                            m_Satellite.GetDetectorHalfDimension(), 
                                                                                            m_Satellite.GetDetectorPixelsX(),
                                                                                            m_Satellite.GetDetectorPixelsY());
  
  // Create the charge loss response
  m_ChargeLossFileName = "$(NUSIM)/resource/data/ChargeLossResponse.dat";
  MFile::ExpandFileName(m_ChargeLossFileName);
  m_ChargeLossInputEnergy = 122;
  if (GenerateChargeLossResponse() == false) {
    return false;
  }

  // Set up the energy resolution
  vector<double> X;
  X.push_back(0.0);
  X.push_back(30.0);
  X.push_back(60.0);
  vector<double> Y;
  Y.push_back(0.60/2.35);
  Y.push_back(0.60/2.35);
  Y.push_back(1.0/2.35);
  m_EnergyResolution.Set(X, Y);

  m_LowTrigger = 50;
  m_HighTrigger = 5000;
  m_ChargeSharingDistance = 0.03;
  m_TriggerThreshold = 5;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineBackgroundSims::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Store the initial data


  // Convert the interactions to pixel hits, i.e. pixel IDs and

  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();

  double Energy = 0.0;
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {
      if (Event.GetInteraction(i).GetEnergy() < m_TriggerThreshold) continue;
      // (a) Create the BEFORE info for the diagnostics window
      if (Event.GetTelescope() == 1) {
        NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetInteraction(i).GetTelescope(), Event.GetInteraction(i).GetDetector());
        MVector Pos = Event.GetInteraction(i).GetPosition();
        O.TransformOut(Pos);
        dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddBefore(Pos, Event.GetInteraction(i).GetEnergy());
      }
      
      // (b) Create the pixel hit(s)
      NPixelHit P;
      P.SetTelescope(Event.GetInteraction(i).GetTelescope());
      P.SetDetector(Event.GetInteraction(i).GetDetector());
      P.SetPreTriggerSampleSum(0);
      P.SetPostTriggerSampleSum(0);
      P.SetXPixel(int((Event.GetInteraction(i).GetPosition().X() + m_Satellite.GetDetectorHalfDimension().X()) / (2*m_Satellite.GetDetectorHalfDimension().X() / NPixelsX)));
      P.SetYPixel(int((Event.GetInteraction(i).GetPosition().Y() + m_Satellite.GetDetectorHalfDimension().Y()) / (2*m_Satellite.GetDetectorHalfDimension().Y() / NPixelsY)));

      massert(P.GetXPixel() >= 0 && P.GetXPixel() < NPixelsX);
      massert(P.GetYPixel() >= 0 && P.GetYPixel() < NPixelsY);

      Energy = Event.GetInteraction(i).GetEnergy();
      P.SetIdealEnergy(Energy);
      
      Energy *= m_ChargeLoss.GetInterpolated(0.0, 0.0, Event.GetInteraction(i).GetPosition().Z(), true);
      Energy = gRandom->Gaus(Energy, m_EnergyResolution.Eval(Energy));
      P.SetNoisedEnergy(Energy);
      P.SetPostTriggerSampleSum(Energy);

      P.SetIdealAverageDepth(Event.GetInteraction(i).GetPosition().Z());
      P.SetNoisedAverageDepth(Event.GetInteraction(i).GetPosition().Z() + 2*(gRandom->Rndm()-0.5));
      
      
      // Add it --- if there alreday exists a hit in the same pixel, just add it
      bool Found = false;
      for (unsigned int h = 0; h < Event.GetNPixelHits(); ++h) {
        NPixelHit& Hit = Event.GetPixelHitRef(h);
        if (Hit.GetTelescope() == P.GetTelescope() &&
            Hit.GetDetector() == P.GetDetector() &&
            Hit.GetXPixel() == P.GetXPixel() &&
            Hit.GetYPixel() == P.GetYPixel()) {
          if (Hit.GetIdealEnergy() + P.GetIdealEnergy() > 0.0) {
            Hit.SetIdealAverageDepth((Hit.GetIdealAverageDepth()*Hit.GetIdealEnergy() + P.GetIdealAverageDepth()*P.GetIdealEnergy())/(Hit.GetIdealEnergy() + P.GetIdealEnergy()));
          }
          Hit.SetIdealEnergy(Hit.GetIdealEnergy() + P.GetIdealEnergy());
          if (Hit.GetNoisedEnergy() + P.GetNoisedEnergy() > 0.0) {
            Hit.SetNoisedAverageDepth((Hit.GetNoisedAverageDepth()*Hit.GetNoisedEnergy() + P.GetNoisedAverageDepth()*P.GetNoisedEnergy())/(Hit.GetNoisedEnergy() + P.GetNoisedEnergy()));
          }
          Hit.SetNoisedEnergy(Hit.GetNoisedEnergy() + P.GetNoisedEnergy());
          Found = true;
          break;
        }
      }
      if (Found == false) {
        Event.AddPixelHit(P);
      }

      // (c) Create the AFTER info for the diagnostics window
      if (Event.GetTelescope() == 1) {
        MVector Pos;
        Pos[0] = (0.5 + P.GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
        Pos[1] = (0.5 + P.GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
        Pos[2] = P.GetNoisedAverageDepth();
        if (Pos[2] > m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = 0.999*m_Satellite.GetDetectorHalfDimension().Z();
        if (Pos[2] < -m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = -0.999*m_Satellite.GetDetectorHalfDimension().Z();

        NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), P.GetTelescope(), P.GetDetector());
        O.TransformOut(Pos);
      
        dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddAfter(Pos, P.GetNoisedEnergy());
      }
    } else {
      // Shield hits: 
      NShieldHit S;
      S.SetTelescope(Event.GetInteraction(i).GetTelescope());
      S.SetIdealEnergy(Event.GetInteraction(i).GetEnergy());
      Event.AddShieldHit(S);     
    }
  }
  
  if (Event.GetNShieldHits() == 0 && Event.GetNPixelHits() == 0) {
    Event.SetBlocked(true);
  }


//   cout<<Event.GetID()<<endl;

//   double TotalEnergy = 0.0;
//   for (unsigned int i = 0; i < Event.GetNPixelHits(); ++i) {
//     if (Event.GetInteraction(i).IsDetector() == true) {
//       if (Event.GetInteraction(i).GetEnergy() < 5) continue;
//       TotalEnergy += Event.GetPixelHit(i).GetADUnitsCathode();
//     }
//   }
//   cout<<"Energy: "<<TotalEnergy<<endl;

  return true;
}


// NModuleDetectorEffectsEngineBackgroundSims.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
