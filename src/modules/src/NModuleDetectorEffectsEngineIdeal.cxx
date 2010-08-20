/*
 * NModuleDetectorEffectsEngineIdeal.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorEffectsEngineIdeal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorEffectsEngineIdeal.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"
#include "NGUIDiagnosticsDetectorEffectsEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorEffectsEngineIdeal)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineIdeal::NModuleDetectorEffectsEngineIdeal(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorEffectsEngineIdeal

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Ideal";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorEffectsEngineIdeal";

  // Set the tool tip text
  m_ToolTip = "This is an ideal detector effects engine - ideal energy resolution, no charge sharing, etc.\n"
    "It should be used in combination with the ideal energy calibrator.";

  // Set the module type
  m_ModuleType = c_DetectorEffectsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = true;
  m_Diagnostics = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineIdeal::~NModuleDetectorEffectsEngineIdeal()
{
  // Delete this instance of NModuleDetectorEffectsEngineIdeal
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineIdeal::Initialize()
{
  // Initialize the module 

  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsDetectorEffectsEngine();
//   dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(), 
//                                                                                             m_Satellite.GetDetectorHalfDimension(), 
//                                                                                             m_Satellite.GetDetectorPixelsX(),
//                                                                                             m_Satellite.GetDetectorPixelsY());

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineIdeal::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Store the initial data


  // Convert the interactions to pixel hits, i.e. pixel IDs and

  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();

  // Generate the before data
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true && Event.GetTelescope() == 1) {
      //if (Event.GetInteraction(i).GetEnergy() < 5) continue;
      // (a) Create the BEFORE info for the diagnostics window
      NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetInteraction(i).GetTelescope(), Event.GetInteraction(i).GetDetector());
      MVector Pos = Event.GetInteraction(i).GetPosition();
      O.TransformOut(Pos);
      dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddBefore(Pos, Event.GetInteraction(i).GetEnergy());
    }
  }

  // nm-size pixels...
  double Scaler = 1000000; // mm -> nm

  // Create the pixel hit(s)
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {
      NPixelHit P;
      P.SetTelescope(Event.GetInteraction(i).GetTelescope());
      P.SetDetector(Event.GetInteraction(i).GetDetector());

      //cout<<"IA: "<<Event.GetInteraction(i).GetPosition().X()<<":"<<Event.GetInteraction(i).GetPosition().Y()<<":"<<Event.GetInteraction(i).GetPosition().Z()<<endl;

      P.SetXPixel(int(Scaler*(Event.GetInteraction(i).GetPosition().X() + m_Satellite.GetDetectorHalfDimension().X()) / (2*m_Satellite.GetDetectorHalfDimension().X() / NPixelsX)));
      P.SetYPixel(int(Scaler*(Event.GetInteraction(i).GetPosition().Y() + m_Satellite.GetDetectorHalfDimension().Y()) / (2*m_Satellite.GetDetectorHalfDimension().Y() / NPixelsY)));
      P.SetPreTriggerSampleSum(0);
      P.SetPostTriggerSampleSum(Event.GetInteraction(i).GetEnergy());
      P.SetIdealEnergy(Event.GetInteraction(i).GetEnergy());
      P.SetIdealAverageDepth(Event.GetInteraction(i).GetPosition().Z());
      P.SetNoisedEnergy(Event.GetInteraction(i).GetEnergy());
      P.SetNoisedAverageDepth(Event.GetInteraction(i).GetPosition().Z());
 
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
    } else {
      // Shield hits: 
      NShieldHit S;
      S.SetTelescope(Event.GetInteraction(i).GetTelescope());
      S.SetIdealEnergy(Event.GetInteraction(i).GetEnergy());
      Event.AddShieldHit(S);     
    }
  }


  // Create the after data
  if (Event.GetTelescope() == 1) {
    for (unsigned int h = 0; h < Event.GetNPixelHits(); ++h) {
      NPixelHit& P = Event.GetPixelHitRef(h); 
      // (c) Create the AFTER info for the diagnostics window
      MVector Pos;
      Pos[0] = ((0.5 + P.GetXPixel())/Scaler) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
      Pos[1] = ((0.5 + P.GetYPixel())/Scaler) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
      Pos[2] = P.GetIdealAverageDepth();
      if (Pos[2] > m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = 0.999*m_Satellite.GetDetectorHalfDimension().Z();
      if (Pos[2] < -m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = -0.999*m_Satellite.GetDetectorHalfDimension().Z();
      NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), P.GetTelescope(), P.GetDetector());
      O.TransformOut(Pos);
      dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddAfter(Pos, P.GetIdealEnergy());  
    }
  }

  if (Event.GetNShieldHits() == 0 && Event.GetNPixelHits() == 0) {
    Event.SetBlocked(true);
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorEffectsEngineIdeal::ShowOptionsGUI()
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


bool NModuleDetectorEffectsEngineIdeal::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleDetectorEffectsEngineIdeal::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorEffectsEngineIdeal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
