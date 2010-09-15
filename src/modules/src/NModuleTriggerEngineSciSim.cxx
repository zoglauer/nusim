/*
 * NModuleTriggerEngineSciSim.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleTriggerEngineSciSim
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleTriggerEngineSciSim.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsTriggerEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleTriggerEngineSciSim)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleTriggerEngineSciSim::NModuleTriggerEngineSciSim(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceDeadTime()
{
  // Construct an instance of NModuleTriggerEngineSciSim

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "SciSim thread trigger engine";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagTriggerAndPackagingEngineSciSim";

  // Set the tool tip text
  m_ToolTip = "The is the satellite-like trigger and hit packaging engine which belongs to the science simulator thread";

  // Set the module type
  m_ModuleType = c_TriggerEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsVetoDeadTimeEngineDummy)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsVetoDeadTimeEngineDummy();
}


////////////////////////////////////////////////////////////////////////////////


NModuleTriggerEngineSciSim::~NModuleTriggerEngineSciSim()
{
  // Delete this instance of NModuleTriggerEngineSciSim
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineSciSim::Initialize()
{
  // Initialize the module 

  m_LowTrigger = 75;
  m_HighTrigger = 5000;
  m_TriggerThreshold = 5;

  NModuleInterfaceDeadTime::Initialize();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineSciSim::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Step (1): Check if we have a pixel trigger and then create 9-pixel-hits
  int NTriggers = 0;
  int HighestID = 0;
  double HighestEnergy = 0.0;
  for (unsigned int p = 0; p < Event.GetNPixelHits(); ++p) {
    double SampleSumDiff = Event.GetPixelHit(p).GetPostTriggerSampleSum() - Event.GetPixelHit(p).GetPreTriggerSampleSum();
    if (SampleSumDiff > HighestEnergy) {
      HighestEnergy = SampleSumDiff;
      HighestID = p;
    }
    if (SampleSumDiff > m_TriggerThreshold) {
      ++NTriggers;
    }
  }
  Event.SetNPixelTriggers(NTriggers);

  
  if (NTriggers > 0) {
    // For the time being create one 9-Pixel-hit with the center being the pixel with the highest ID
    NNinePixelHit Niner;
    Niner.SetTelescope(Event.GetPixelHit(HighestID).GetTelescope());
    Niner.SetDetector(Event.GetPixelHit(HighestID).GetDetector());
    Niner.SetXPixel(Event.GetPixelHit(HighestID).GetXPixel());
    Niner.SetYPixel(Event.GetPixelHit(HighestID).GetYPixel());
    
    Niner.SetIdealAverageDepth(0);
    Niner.SetNoisedAverageDepth(0);
    Niner.SetIdealEnergy(0);
    Niner.SetNoisedEnergy(0);
    
    // Dummy loop over surrounding pixels
    int ID = 0;
    int NPixelsX = m_Satellite.GetDetectorPixelsX();
    int NPixelsY = m_Satellite.GetDetectorPixelsY();
    for (int xp = Niner.GetXPixel()-1; xp <= Niner.GetXPixel()+1; ++xp) {
      for (int yp = Niner.GetYPixel()-1; yp <= Niner.GetYPixel()+1; ++yp) {
        ++ID;
        if (xp < 0 || xp > NPixelsX) continue;
        if (yp < 0 || yp > NPixelsY) continue;
        
        // Find the correct pixel hit in the event
        for (unsigned int p = 0; p < Event.GetNPixelHits(); ++p) {
          NPixelHit& P = Event.GetPixelHitRef(p);
          if (P.GetTelescope() == Niner.GetTelescope() &&
              P.GetDetector() == Niner.GetDetector() &&
              P.GetXPixel() == xp && P.GetYPixel() == yp) {
            Niner.SetPreTriggerSampleSum(ID, P.GetPreTriggerSampleSum()); 
            Niner.SetPostTriggerSampleSum(ID, P.GetPostTriggerSampleSum());
          
            if (P.GetPostTriggerSampleSum() - P.GetPreTriggerSampleSum() > m_TriggerThreshold) {
              Niner.SetTrigger(ID, true);
            }
          
            // Use energy weighted depths:
            if (Niner.GetIdealEnergy() + P.GetIdealEnergy() > 0) {
              Niner.SetIdealAverageDepth((Niner.GetIdealAverageDepth()*Niner.GetIdealEnergy() + P.GetIdealAverageDepth()*P.GetIdealEnergy())/(Niner.GetIdealEnergy() + P.GetIdealEnergy()));
            }
            if (Niner.GetNoisedEnergy() + P.GetNoisedEnergy() > 0) {
              Niner.SetNoisedAverageDepth((Niner.GetNoisedAverageDepth()*Niner.GetNoisedEnergy() + P.GetNoisedAverageDepth()*P.GetNoisedEnergy())/(Niner.GetNoisedEnergy() + P.GetNoisedEnergy()));
            }
            
            Niner.SetIdealEnergy(Niner.GetIdealEnergy() + P.GetIdealEnergy());
            Niner.SetNoisedEnergy(Niner.GetNoisedEnergy() + P.GetNoisedEnergy());

            break;
          }
        }
      }
    }
    
    Event.AddNinePixelHit(Niner);
  }
  
  // Step (2): Check if a shield veto was raised
  for (unsigned int i = 0; i < Event.GetNShieldHits(); ++i) {
    // Raise veto
    if (Event.GetShieldHit(i).GetIdealEnergy() > m_HighTrigger) {
      Event.SetVetoHigh();
    } else if (Event.GetShieldHit(i).GetIdealEnergy() > m_LowTrigger) {
      Event.SetVetoLow();        
    }
  }

  if (Event.GetVeto() == false && Event.GetNNinePixelHits() == 0) {
    Event.SetBlocked(true);
    return true;
  }
  
  if (m_ApplyDeadTime == true) {
    // First check is we are in dead time
    if (IsLostInDeadTime(Event) == true) {
      return true;
    }

    // Raise the dead time
    SetDeadTimeDetectorHit(Event.GetTime(), Event.GetTelescope());
  } else {
    if (Event.GetTelescope() == 1) {
      m_NEventsNotLostInDeadTime1++;
    } else if (Event.GetTelescope() == 2) {
      m_NEventsNotLostInDeadTime2++;      
    }

  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineSciSim::Finalize()
{
  // Finalzie the module
  
  cout<<endl;
  cout<<"Trigger engine (SciSim) summary:"<<endl;
  cout<<"  Triggers - telesope 1:"<<endl;
  cout<<"     Events passed:              "<<m_NEventsNotLostInDeadTime1<<endl;
  if (m_ApplyDeadTime == true) {
    cout<<"     Events lost in dead time:   "<<m_NEventsLostInDeadTime1<<endl;
    cout<<"     Life time:                  "<<(m_Satellite.GetTimeIdeal() - m_DeadTimeCouter1*m_DetectorDeadTime.GetSeconds())/m_Satellite.GetTimeIdeal().GetSeconds()<<" per second"<<endl;
    double Input = m_NEventsNotLostInDeadTime1/(1 - m_NEventsNotLostInDeadTime1*m_DetectorDeadTime.GetSeconds()/m_Satellite.GetTimeIdeal().GetSeconds());
    cout<<"     Input hits as sanity check: "<<Input
      <<" vs. "<<m_NEventsLostInDeadTime1+m_NEventsNotLostInDeadTime1<<endl;
  }
  cout<<"  Triggers - telesope 2:"<<endl;
  cout<<"     Events passed:              "<<m_NEventsNotLostInDeadTime2<<endl;
  if (m_ApplyDeadTime == true) {
    cout<<"     Events lost in dead time:   "<<m_NEventsLostInDeadTime2<<endl;
    cout<<"     Life time:                  "<<(m_Satellite.GetTimeIdeal() - m_DeadTimeCouter2*m_DetectorDeadTime.GetSeconds())/m_Satellite.GetTimeIdeal().GetSeconds()<<" per second"<<endl;
    double Input = m_NEventsNotLostInDeadTime2/(1 - m_NEventsNotLostInDeadTime2*m_DetectorDeadTime.GetSeconds()/m_Satellite.GetTimeIdeal().GetSeconds());
    cout<<"     Input hits as sanity check: "<<Input
      <<" vs. "<<m_NEventsLostInDeadTime2+m_NEventsNotLostInDeadTime2<<endl;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleTriggerEngineSciSim::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsTriggerEngine* Options = new NGUIOptionsTriggerEngine(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleTriggerEngineSciSim::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* ApplyDeadTimeNode = Node->GetNode("ApplyDeadTime");
  if (ApplyDeadTimeNode != 0) {
    m_ApplyDeadTime = ApplyDeadTimeNode->GetValueAsBoolean();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleTriggerEngineSciSim::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "ApplyDeadTime", m_ApplyDeadTime);

  return Node;
}


// NModuleTriggerEngineSciSim.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
