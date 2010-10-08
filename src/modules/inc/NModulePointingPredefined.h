/*
 * NModulePointingPredefined.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModulePointingPredefined__
#define __NModulePointingPredefined__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModule.h"
#include "NModuleInterfacePointing.h"
#include "NModuleInterfaceStopCriterion.h"
#include "NPointingJitterDBEntry.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModulePointingPredefined : public NModule, public NModuleInterfacePointing, public NModuleInterfaceStopCriterion
{
  // public interface:
 public:
  //! Default constructor
  NModulePointingPredefined(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModulePointingPredefined();

  //! Initialize the module
  virtual bool Initialize();

  //! Return the pointing of the satellte at a given time
  virtual NPointing GetPointing(NTime Time);

  //! Check if stop criterion is fullfilled
  virtual bool StopCriterionFullFilled();

  //! Return the number of pointing slews between the given time
  virtual unsigned int GetNPointingSlews(NTime First, NTime Second);
  
  //! Return the time of the given pointing slew between the two times. 
  //! If multiple slews occur between the time, use ID to access the first , second, etc slew time
  //! Counting starts at 0!
  virtual NTime GetPointingSlewTime(const NTime& First, const NTime& Second, unsigned int SlewID = 0);

  //! Return a reference to the initial pointings
  vector<NPointing>& GetInitialPointingsByRef() { return m_InitialPointings; }

  //! Get the file name of the pointing jitter data base
  TString GetPointingJitterDBFileName() const { return m_PointingJitterDBFileName; }
  //! Set the file name of the pointing jitter data base
  void SetPointingJitterDBFileName(const TString PointingJitterDBFileName);

  //! Return true if the pointing are absolute
  bool GetAbsoluteTime() const { return m_AbsoluteTime; }
  //! Set if the pointing are absolute
  void SetAbsoluteTime(bool AbsoluteTime) { m_AbsoluteTime = AbsoluteTime; }

  //! Import pointings
  bool ImportPointings(TString FileName);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Constant describing motion pattern 0 - fixed
  static const int c_MotionPatternNone;
  //! Constant describing motion pattern 1 - random walk within X arcmin
  static const int c_MotionPatternRandomWalk;
  //! Constant describing motion pattern 2 - read from data base 
  static const int c_MotionPatternDB;
  
  //! ID of the smallest motion pattern
  static const int c_MotionPatternMin;
  //! ID of the largest motion pattern;
  static const int c_MotionPatternMax;

  // protected methods:
 protected:
  //! Read the pointing jitters DB
  bool ReadPointingJitterDB(TString FileName);

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The list of initial pointings before any random walk, error quaternion was applied.
  vector<NPointing> m_InitialPointings;

  //! The list of initial pointings before any random walk, error quaternion was applied - with corrected time sequence!
  vector<NPointing> m_SequencedInitialPointings;
  //! Start your search for the pointing at this index...
  unsigned int m_StartIndexSequencedInitialPointings;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapSequencedInitialPointings;
  
  //! Time of the last calculated pointing
  NTime m_Time;
  
  //! Flag indicating that the times given in the pointings are absolute or relative
  bool m_AbsoluteTime;
  
  //! The pointing jitter DB file name
  TString m_PointingJitterDBFileName;
  
  
  //! The motion pattern
  int m_MotionPattern;
  
  
  //! Start your search for the pointing  at this index...
  unsigned int m_StartIndexPointing;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapPointing;
   
  
  //! Start your search for the pointing jitter at this index...
  unsigned int m_StartIndexPointingJitters;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapPointingJitters;
  //! The pertubed alignments in space as a function of time...
  vector<NPointingJitterDBEntry> m_PointingJitters;   

  
  /*
  //! The last pointing call for the random walk
  NPointing m_RWPointing;
  */


#ifdef ___CINT___
 public:
  ClassDef(NModulePointingPredefined, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
