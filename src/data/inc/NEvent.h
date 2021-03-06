/*
 * NEvent.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NEvent__
#define __NEvent__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
#include <iostream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NAlignmentsDBEntry.h"
#include "NPhoton.h"
#include "NInteraction.h"
#include "NPixelHit.h"
#include "NShieldHit.h"
#include "NNinePixelHit.h"
#include "NHit.h"
#include "NMetrologyData.h"
#include "NStarTrackerData.h"
#include "NObservatoryData.h"
#include "NPHEFitsData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a measured event.
//! This class represents a measured event and contains all data belonging 
//! to this event along the measurement path, such as original photon parameters
//! current (or last) photon parameters, interaction positions, pixel lists,
//! calibrated photon data, event time and ID, telescope ID, and much more

class NEvent
{
  // public interface:
 public:
  //! Default constructor
  NEvent();
  //! Copy constructor
  NEvent(const NEvent& Event) { (*this) = Event; };
  //! Default destructor
  virtual ~NEvent();

  //! Assignment operator
  const NEvent& operator=(const NEvent& Event);

  //! Reset all data
  void Clear();

  //! Return true if the event is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the Origin of this event
  void SetOrigin(unsigned int Origin) { m_Empty = false; m_Origin = Origin; }
  //! Return the Origin of this event
  unsigned int GetOrigin() const { return m_Origin; }
  //! Return the Origin of this event as string
  TString GetOriginString() const;

  //! Set the ID of this event
  void SetID(unsigned long ID) { m_Empty = false; m_ID = ID; }
  //! Return the ID of this event
  unsigned long GetID() const { return m_ID; }

  //! Set the telescope module of this event
  void SetTelescope(unsigned int Telescope) { m_Empty = false; m_Telescope = Telescope; }
  //! Return the telescope module of this event
  unsigned int GetTelescope() const { return m_Telescope; }

  //! Set the time of this event
  void SetTime(NTime Time) { m_Empty = false; m_Time = Time; }
  //! Return the Time of this event
  NTime GetTime() const { return m_Time; }

  //! Set the time of this event
  void IsMeasured(bool IsMeasured) { m_Empty = false; m_IsMeasured = IsMeasured; }
  //! Return the Time of this event
  bool IsMeasured() const { return m_IsMeasured; }

  //! Set the detector life time for this event
  void SetDetectorLifeTime(NTime DetectorLifeTime) { m_Empty = false; m_DetectorLifeTime = DetectorLifeTime; }
  //! Return the detector life time for this event
  NTime GetDetectorLifeTime() const { return m_DetectorLifeTime; }

  //! Set the blocked flag - event got blocked by the optics, aperture, passive detector material, etc)
  void SetBlocked(bool Blocked = true) { m_Empty = false; m_Blocked = Blocked; }
  //! Return the blocked flag - event got blocked by the optics, aperture, passive detector material, etc)
  bool GetBlocked() const { return m_Blocked; }

  //! Set the lost in dead time flag 
  void SetLostInDeadTime(bool LostInDeadTime = true) { m_Empty = false; m_LostInDeadTime = LostInDeadTime; }
  //! Return the lost in dead time flag 
  bool GetLostInDeadTime() const { return m_LostInDeadTime; }

  //! Return the veto flag
  bool GetVeto() const { return (m_VetoLow == true || m_VetoHigh == true); }

  //! Set the vetoed flag
  void SetVetoLow(bool Veto = true) { m_Empty = false; m_VetoLow = Veto; }
  //! Return the veto flag
  bool GetVetoLow() const { return m_VetoLow; }

  //! Set the vetoed flag
  void SetVetoHigh(bool Veto = true) { m_Empty = false; m_VetoHigh = Veto; }
  //! Return the veto flag
  bool GetVetoHigh() const { return m_VetoHigh; }

  //! Set the triggered flag
  void SetTrigger(bool Trigger = true) { m_Empty = false; m_Trigger = Trigger; }
  //! Return the trigger flag
  bool GetTrigger() const { return m_Trigger; }

  //! Set the pile-up flag
  void SetPileUp(bool PileUp = true) { m_Empty = false; m_PileUp = PileUp; }
  //! Return the pile-up flag
  bool GetPileUp() const { return m_PileUp; }

  //! Set the number of pixel triggers
  void SetNPixelTriggers(int NPixelTriggers) { m_Empty = false; m_NPixelTriggers = NPixelTriggers; if (m_IsMeasured == false) { if (m_NPixelTriggers > 0) m_Trigger = true; else m_Trigger = false; } }
  int GetNPixelTriggers() const { return m_NPixelTriggers; }

  //! Set the event cut flag
  void SetEventCut(bool EventCut = true) { m_Empty = false; m_EventCut = EventCut; }
  //! Return the event cut flag
  bool GetEventCut() const { return m_EventCut; }

  //! Set the original photon parameters
  void SetOriginalPhoton(const NPhoton& Photon);
  //! Get A COPY the original photon
  NPhoton GetOriginalPhoton() { return m_OriginalPhoton; }
  //! Get A REFERENCE to the original photon
  NPhoton& GetOriginalPhotonRef() { return m_OriginalPhoton; }


  //! Set the initial parameters of the photon in the optics module
  void SetInitialPhotonRelOM(const NPhoton& Photon) { m_InitialPhotonRelOM = Photon; }
  //! Get A COPY the initial parameters of the photon in the optics module
  NPhoton GetInitialPhotonRelOM() { return m_InitialPhotonRelOM; }
  //! Get A REFERENCE to the initial parameters of the photon in the optics module
  NPhoton& GetInitialPhotonRelOMRef() { return m_InitialPhotonRelOM; }


  //! Set the original right ascension and declination of the photon not the reconstructed one
  void SetOriginalPhotonRaDec(double Ra, double Dec) { m_Empty = false; m_Ra = Ra; m_Dec = Dec; }
  //! Get the original right ascension - only reasonable when GetOrigin() == c_OriginSource
  double GetOriginalPhotonRa() const { return m_Ra; }
  //! Get the original declination - only reasonable when GetOrigin() == c_OriginSource
  double GetOriginalPhotonDec() const { return m_Dec; } 


  //! Set the current photon parameters 
  void SetCurrentPhoton(const NPhoton& Photon);
  //! Get A COPY the current photon
  NPhoton GetCurrentPhoton() { return m_CurrentPhoton; }
  //! Get A REFERENCE to the current photon
  NPhoton& GetCurrentPhotonRef() { return m_CurrentPhoton; }


  //! Return the number of interactions
  unsigned int GetNInteractions() const { return m_Interactions.size(); }
  //! Return A COPY interaction i
  NInteraction GetInteraction(unsigned int i);
  //! Return A REFERENCE to interaction i
  NInteraction& GetInteractionRef(unsigned int i);
  //! Add an interaction
  void AddInteraction(const NInteraction& Interaction) { m_Empty = false; m_Interactions.push_back(Interaction); }


  //! Return a COPY of the PHE data
  NPHEFitsData GetPHEData() { return m_PHEData; }
  //! Return a REFERENCE to the PHE data
  NPHEFitsData& GetPHEDataRef() { return m_PHEData; }
  //! Set the PHE data
  void SetPHEData(const NPHEFitsData& Data) { m_PHEData = Data; }


  //! Return the number of pixel hits
  unsigned int GetNPixelHits() const { return m_PixelHits.size(); }
  //! Return A COPY of pixel hit i
  NPixelHit GetPixelHit(unsigned int i);
  //! Return A REFERENCE to pixel hit i
  NPixelHit& GetPixelHitRef(unsigned int i);
  //! Add a pixel hit
  void AddPixelHit(const NPixelHit& PixelHit) { m_Empty = false; m_PixelHits.push_back(PixelHit); }
  //! Remove a pixel hit
  void RemovePixelHit(unsigned int i);


  //! Return the number of shield hits
  unsigned int GetNShieldHits() const { return m_ShieldHits.size(); }
  //! Return A COPY of pixel hit i
  NShieldHit GetShieldHit(unsigned int i);
  //! Return A REFERENCE to pixel hit i
  NShieldHit& GetShieldHitRef(unsigned int i);
  //! Add a pixel hit
  void AddShieldHit(const NShieldHit& ShieldHit) { m_Empty = false; m_ShieldHits.push_back(ShieldHit); }
  //! Remove a pixel hit
  void RemoveShieldHit(unsigned int i);


  //! Return the number of pixel hits
  unsigned int GetNNinePixelHits() const { return m_NinePixelHits.size(); }
  //! Return A COPY of pixel hit i
  NNinePixelHit GetNinePixelHit(unsigned int i);
  //! Return A REFERENCE to pixel hit i
  NNinePixelHit& GetNinePixelHitRef(unsigned int i);
  //! Add a pixel hit
  void AddNinePixelHit(const NNinePixelHit& NinePixelHit) { m_Empty = false; m_NinePixelHits.push_back(NinePixelHit); }
  //! Remove a pixel hit
  void RemoveNinePixelHit(unsigned int i);


  //! Return the number of hits
  unsigned int GetNHits() const { return m_Hits.size(); }
  //! Return A COPY of hit i
  NHit GetHit(unsigned int i);
  //! Return A REFERENCE to hit i
  NHit& GetHitRef(unsigned int i);
  //! Add a hit
  void AddHit(const NHit& Hit) { m_Empty = false; m_Hits.push_back(Hit); }

  
  //! Set all orientations at the time of the event
  void SetOrientations(const NAlignmentsDBEntry& Orientations) { m_Orientations = Orientations; }
  //! Get A COPY of all orientations
  NAlignmentsDBEntry GetOrientations() { return m_Orientations; }
  //! Get A REFERENCE to all orientations
  NAlignmentsDBEntry& GetOrientationsRef() { return m_Orientations; }
  

  //! Stream the content to an ASCII file 
  //! WhatToStream: 0: All, 1: Level 1 relevant, 2: Level 2 relevant
  bool Stream(ofstream& S, int Version, int WhatToStream = 0);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);



  //! ID representing an event of unknown source, i.e. neither source nor background
  static const unsigned int c_OriginUnknown    = 0;
  //! ID representing a source event
  static const unsigned int c_OriginSource     = 1;
  //! ID representing a background photon
  static const unsigned int c_OriginBackground = 2;
  //! ID representing a source photon which is a ghost ray
  static const unsigned int c_OriginGhostRay   = 3;
  //! ID representing a detector noise event
  static const unsigned int c_OriginNoise      = 4;

  

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! Origin ID of this event
  unsigned int m_Origin;

  //! ID of this event
  unsigned long m_ID;

  //! Telescope of this event
  unsigned int m_Telescope;

  //! True is this is a measured event
  bool m_IsMeasured;
  
  //! The right ascension
  double m_Ra;
  //! The declination
  double m_Dec;
  
  //! The orientations of the satellite, mast, etc at the event time
  NAlignmentsDBEntry m_Orientations;
  
  //! Original photon parameters 
  NPhoton m_OriginalPhoton;

  //! The initial parameters of the photon in the optics module
  NPhoton m_InitialPhotonRelOM;

  //! Current photon parameters (if this event is no longer in "photon-state", last valid photon parameters)  
  NPhoton m_CurrentPhoton;

  //! List of simulated interactions of photons and electrons (empty if this stage is not yet reached)
  vector<NInteraction> m_Interactions;

  //! The PHE data
  NPHEFitsData m_PHEData;
  
  //! List of UNCALIBRATED hits in the detector (empty if this stage is not yet reached)
  vector<NPixelHit> m_PixelHits;

  //! List of shield hits (empty if this stage is not yet reached)
  vector<NShieldHit> m_ShieldHits;

  //! List of UNCALIBRATED hits in the detector in nine-pixel-format (empty if this stage is not yet reached)
  vector<NNinePixelHit> m_NinePixelHits;

  //! List of calibrated hits in the detector (empty if this stage is not yet reached)
  vector<NHit> m_Hits;

  //! Time of this event
  NTime m_Time;

  //! Detector life time of this event
  NTime m_DetectorLifeTime;

  //! Flag indicating that this event got blocked (e.g. by the optics, the aperture, passive detector material, etc.) 
  bool m_Blocked;

  //! Flag indicating that this event got lost in dead time
  bool m_LostInDeadTime;

  //! Low level veto flag of this event
  bool m_VetoLow;

  //! High level veto flag of this event
  bool m_VetoHigh;

  //! Trigger flag of this event
  bool m_Trigger;

  //! Pile-up flag of this event
  bool m_PileUp;

  //! The number of pixels which have triggers
  int m_NPixelTriggers;

  //! Event cut flag of this event
  bool m_EventCut;

#ifdef ___CINT___
 public:
  ClassDef(NEvent, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
