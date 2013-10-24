/*
 * NModuleInterfacePileUp.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfacePileUp
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfacePileUp.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfacePileUp)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfacePileUp::NModuleInterfacePileUp() : NModuleInterfaceTimeJump()
{
  // Construct an instance of NModuleInterfacePileUp

  m_ApplyPileUp = false;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfacePileUp::~NModuleInterfacePileUp()
{
  // Delete this instance of NModuleInterfacePileUp
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfacePileUp::Initialize()
{
  //! Initialize

  m_TruePileUpTimeScale = 2*us;
  m_FalsePileUpTimeScale = 6*us;
  
  m_DelayedList1.clear();
  m_DelayedList2.clear();

  m_NDelayedEvents1 = 0;
  m_NDelayedEvents2 = 0;
  
  m_NPileUps1 = 0;
  m_NPileUps2 = 0;
  
  m_NNotPileUps1 = 0;
  m_NNotPileUps2 = 0;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////



bool NModuleInterfacePileUp::Finalize()
{
  //! Finalize
  
  cout<<"Pile-up statistics:"<<endl;
  cout<<"  Module 1:"<<endl;
  //cout<<"    Time behind real observation time: "<<m_DelayedList1.front().GetTime()<<" sec"<<endl;;
  cout<<"    Number of piled up events: "<<m_NPileUps1<<endl;
  cout<<"    Number of not piled up events: "<<m_NNotPileUps1<<endl;
  cout<<"    Number of empty delayed events: "<<m_NDelayedEvents1<<endl;
  cout<<"  Module 2:"<<endl;
  //cout<<"    Time behind real observation time: "<<m_DelayedList2.front().GetTime()<<" sec"<<endl;;
  cout<<"    Number of piled up events: "<<m_NPileUps2<<endl;
  cout<<"    Number of not piled up events: "<<m_NNotPileUps2<<endl;
  cout<<"    Number of empty delayed events: "<<m_NDelayedEvents2<<endl;
  
  
  return true;
}
  

////////////////////////////////////////////////////////////////////////////////


NEvent NModuleInterfacePileUp::CreateDelayedPileUpEvent(NEvent& Event) 
{
  //! Create a delayed event (delayed relative to the original event), but now with pile-up!

  // Step 1: Add it to the appropriate list 
  list<NEvent>& List = (Event.GetTelescope() == 1) ? m_DelayedList1 : m_DelayedList2;
  List.push_back(Event);

  // Step 2: If we just have one event or not enough time between events we return an empty dummy event
  unsigned int NEvents = List.size();
  NTime TimeDiff = List.front().GetTime() = List.back().GetTime();
  //cout<<Event.GetTelescope()<<" - PU: Time diff = "<<TimeDiff<<endl;
  if (NEvents == 1 || TimeDiff < m_TruePileUpTimeScale + m_FalsePileUpTimeScale) {
    NEvent Empty;
    Empty.SetTelescope(Event.GetTelescope());
    Empty.SetBlocked(true);
    
    if (Event.GetTelescope() == 1) {
      m_NDelayedEvents1++;
    } else {
      m_NDelayedEvents2++;
    }
    return Empty;
  }
  
  // Step 3: check for the real trigger to set the correct time and end time
  // --> Later
  
  // Step 4: Merge the events into a new one
  list<NEvent>::iterator I = List.begin();
  NEvent New = *I;
  New.SetPileUp(false);
  //cout<<New.GetTelescope()<<" - Starting "<<New.GetTime()<<endl;
  ++I;
  while (I != List.end()) {
    if ((*I).GetTime() < List.front().GetTime() + m_TruePileUpTimeScale) {
      // Add fully
      Add(New, *I, 1.0); 
      //cout<<New.GetTelescope()<<" - Adding "<<(*I).GetTime()<<endl;
    } else if ((*I).GetTime() < List.front().GetTime() + m_FalsePileUpTimeScale) {
      // Add partially
      double Partial = ((*I).GetTime() - m_TruePileUpTimeScale - List.front().GetTime()) / m_FalsePileUpTimeScale;
      //cout<<New.GetTelescope()<<" - Adding partial "<<(*I).GetTime()<<" "<<Partial<<endl;
      Add(New, *I, Partial); 
    }
    ++I;
    //cout<<"New.Pile Up? "<<(New.GetPileUp() ? "true" : "false")<<endl;
  }
  
  // Step 4: If we have a delayed trigger clean the list - 
  // but this has to be done in a separate function since we do not know yet if we have a trigger!
  
  // Some statistics
  if (New.GetTelescope() == 1) {
    if (New.GetPileUp() == true) {
      m_NPileUps1++;
    } else {
      m_NNotPileUps1++;
    }
  } else {
    if (New.GetPileUp() == true) {
      m_NPileUps2++;
    } else {
      m_NNotPileUps2++;
    }
  }
  
  
  return New;
}  
  

////////////////////////////////////////////////////////////////////////////////

  
void NModuleInterfacePileUp::CleanUp(NEvent& Event)
{
  //! The created event has triggered - so do some clean up
  
  list<NEvent>& List = (Event.GetTelescope() == 1) ? m_DelayedList1 : m_DelayedList2;
  
  //cout<<Event.GetTelescope()<<" - Removing someting"<<List.front().GetTime()<<" vs. "<<Event.GetTime()<<" vs. "<<Event.GetTime() + m_TruePileUpTimeScale + m_FalsePileUpTimeScale<<endl;
  
  while (List.front().GetTime() < Event.GetTime() + m_TruePileUpTimeScale + m_FalsePileUpTimeScale) {
    //cout<<Event.GetTelescope()<<" - Removing: "<<List.front().GetTime()<<endl;
    List.pop_front(); 
  }  
}
  

////////////////////////////////////////////////////////////////////////////////

  
void NModuleInterfacePileUp::CleanUpOne(NEvent& Event)
{
  //! The created event has triggered - so do some clean up
  
  list<NEvent>& List = (Event.GetTelescope() == 1) ? m_DelayedList1 : m_DelayedList2;
  
  //cout<<Event.GetTelescope()<<" - Removing: "<<List.front().GetTime()<<endl;
  List.pop_front(); 
}
  

////////////////////////////////////////////////////////////////////////////////

  
void NModuleInterfacePileUp::CleanUpAll(NEvent& Event)
{
  //! The created event has triggered - so do some clean up
  
  list<NEvent>& List = (Event.GetTelescope() == 1) ? m_DelayedList1 : m_DelayedList2;
  
  //cout<<Event.GetTelescope()<<" - Removing All"<<endl;
  List.clear();
}
  

////////////////////////////////////////////////////////////////////////////////


void NModuleInterfacePileUp::Add(NEvent& Event, NEvent& Add, double Ratio)
{
  //! Add an event to another one as pile up...

  //cout<<"Adding: "<<Event.GetTime()<<" "<<Add.GetTime()<<" --> "<<Add.GetTime() - Event.GetTime()<<endl;
  
  // Step 1: Error checks
  if (Event.GetTelescope() != Add.GetTelescope()) {
    mout<<"Pile up error: Different telescopes!"<<endl; 
  }
  
  // We do have a pileup event
  Event.SetPileUp(true);
  
  // Simply add the interactions
  for (unsigned int i = 0; i < Add.GetNInteractions(); ++i) {
    //cout<<"Adding IA!"<<endl;
    Event.AddInteraction(Add.GetInteraction(i));
  }
  
  // Merge the pixel hits correctly
  // ATTENTION: SIMILAR code is in the detector effects engine
  for (unsigned int p = 0; p < Add.GetNPixelHits(); ++p) {
    //cout<<p<<": "<<Add.GetPixelHit(p).GetXPixel()<<":"<<Add.GetPixelHit(p).GetYPixel()<<":"<<Add.GetPixelHit(p).GetPostTriggerSampleSum()<<endl;
    // Check if a similar pixel hit is already in the event
    bool Merged = false;
    for (unsigned int e = 0; e < Event.GetNPixelHits(); ++e) {
      if (Add.GetPixelHit(p).GetTelescope() == Event.GetPixelHit(e).GetTelescope() &&
          Add.GetPixelHit(p).GetDetector()  == Event.GetPixelHit(e).GetDetector() &&
          Add.GetPixelHit(p).GetXPixel()    == Event.GetPixelHit(e).GetXPixel() &&
          Add.GetPixelHit(p).GetYPixel()    == Event.GetPixelHit(e).GetYPixel()) {

        //cout<<"Duplicate pixel --- start merging: "<<endl;
        Event.GetPixelHitRef(e).AddPreTriggerSampleSum(Ratio * Add.GetPixelHit(p).GetPreTriggerSampleSum());
        Event.GetPixelHitRef(e).AddPostTriggerSampleSum(Ratio * Add.GetPixelHit(p).GetPostTriggerSampleSum());
        // Use energy weighted depths:
        if (Event.GetPixelHit(e).GetIdealEnergy() + Ratio * Add.GetPixelHit(p).GetIdealEnergy() > 0.0) {
          Event.GetPixelHitRef(e).SetIdealAverageDepth((Event.GetPixelHit(e).GetIdealAverageDepth() * Ratio * Event.GetPixelHit(e).GetIdealEnergy()
              + Add.GetPixelHit(p).GetIdealAverageDepth() * Ratio * Add.GetPixelHit(p).GetIdealEnergy())
                   / (Event.GetPixelHit(e).GetIdealEnergy() + Ratio * Add.GetPixelHit(p).GetIdealEnergy()));
        }
        if (Event.GetPixelHit(e).GetNoisedEnergy() + Ratio * Add.GetPixelHit(p).GetNoisedEnergy() > 0.0) {
          Event.GetPixelHitRef(e).SetNoisedAverageDepth((Event.GetPixelHit(e).GetNoisedAverageDepth() * Event.GetPixelHit(e).GetNoisedEnergy()
               + Add.GetPixelHit(p).GetNoisedAverageDepth() * Ratio * Add.GetPixelHit(p).GetNoisedEnergy())
              / (Event.GetPixelHit(e).GetNoisedEnergy() + Ratio * Add.GetPixelHit(p).GetNoisedEnergy()));
        }

        Event.GetPixelHitRef(e).SetIdealEnergy(Event.GetPixelHit(e).GetIdealEnergy() + Ratio * Add.GetPixelHit(p).GetIdealEnergy());
        Event.GetPixelHitRef(e).SetNoisedEnergy(Event.GetPixelHit(e).GetNoisedEnergy() + Ratio * Add.GetPixelHit(p).GetNoisedEnergy());
        Merged = true;
        break;
      }
    }
    if (Merged == false) {
      Event.AddPixelHit(Add.GetPixelHit(p));
    }
  }
}


// NModuleInterfacePileUp.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
