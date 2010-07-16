/*
 * NPHEFitsData.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPHEFitsData__
#define __NPHEFitsData__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a the data read in by the PHE fits file
//! This class represents a object containing all data read in by the PHE fits file

class NPHEFitsData
{
  // public interface:
 public:
  //! Default constructor
  NPHEFitsData();
  //! Default destructor
  virtual ~NPHEFitsData();

  //! Reset all data
  void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the reset time
  void SetResetTime(double ResetTime) { m_ResetTime = ResetTime; m_Empty = false; }
  //! Get the reset time
  double GetResetTime() const { return m_ResetTime; }

  //! Set the frame
  void SetFrame(int Frame) { m_Frame = Frame; m_Empty = false; }
  //! Get the frame
  int GetFrame() const { return m_Frame; }

  //! Set the frame
  void SetFlag(int Flag) { m_Flag = Flag; m_Empty = false; }
  //! Get the frame
  int GetFlag() const { return m_Flag; }

  //! Set the sub frame
  void SetSubFrame(int SubFrame) { m_SubFrame = SubFrame; m_Empty = false; }
  //! Get the sub frame
  int GetSubFrame() const { return m_SubFrame; }

  //! Set the RawX
  void SetRawX(int RawX) { m_RawX = RawX; m_Empty = false; }
  //! Get the RawX
  int GetRawX() const { return m_RawX; }

  //! Set the RawY
  void SetRawY(int RawY) { m_RawY = RawY; m_Empty = false; }
  //! Get the RawY
  int GetRawY() const { return m_RawY; }

  //! Set the StartCap
  void SetStartCap(int StartCap) { m_StartCap = StartCap; m_Empty = false; }
  //! Get the StartCap
  int GetStartCap() const { return m_StartCap; }

  //! Set the CZTID
  void SetCZTID(int CZTID) { m_CZTID = CZTID; m_Empty = false; }
  //! Get the CZTID
  int GetCZTID() const { return m_CZTID; }

  //! Set the rise time
  void SetRiseTime(double RiseTime) { m_RiseTime = RiseTime; m_Empty = false; }
  //! Get the rise time
  double GetRiseTime() const { return m_RiseTime; }

  //! Set the elapsed time
  void SetElapsedTime(int ElapsedTime) { m_ElapsedTime = ElapsedTime; m_Empty = false; }
  //! Get the elapsed time
  int GetElapsedTime() const { return m_ElapsedTime; }

  //! Set the number of hardware triggers
  void SetNHardwareTriggers(int NHardwareTriggers) { m_NHardwareTriggers = NHardwareTriggers; m_Empty = false; }
  //! Get the number of hardware triggers
  int GetNHardwareTriggers() const { return m_NHardwareTriggers; }



  //! Set the pre-trigger pulse height
  void SetPreTriggerSampleSum(unsigned int PixelID, int PreTriggerSampleSum);
  //! Return the pre-trigger pulse height
  int GetPreTriggerSampleSum(unsigned int PixelID) const;

  //! Set the post-trigger pulse height
  void SetPostTriggerSampleSum(unsigned int PixelID, int PostTriggerSampleSum);
  //! Return the post-trigger pulse height
  int GetPostTriggerSampleSum(unsigned int PixelID) const;

  //! Set the trigger flag
  void SetTriggerMap(unsigned int PixelID, int TriggerMap);
  //! Get the trigger flag
  int GetTriggerMap(unsigned int PixelID) const;

  //! Set the number software triggers
  void SetNSoftwareTriggers(int NSoftwareTriggers) { m_NSoftwareTriggers = NSoftwareTriggers; m_Empty = false; }
  //! Get the number software triggers
  int GetNSoftwareTriggers() const { return m_NSoftwareTriggers; }

  //! Set the PHA
  void SetPHA(int PHA) { m_PHA = PHA; m_Empty = false; }
  //! Get the PHA
  int GetPHA() const { return m_PHA; }

  //! Set the energy
  void SetEnergy(double Energy) { m_Energy = Energy; m_Empty = false; }
  //! Get the energy
  double GetEnergy() const { return m_Energy; }

  //! Set the CZT temperature
  void SetCZTTemperature(double CZTTemperature) { m_CZTTemperature = CZTTemperature; m_Empty = false; }
  //! Get the CZT temperature
  double GetCZTTemperature() const { return m_CZTTemperature; }

  //! Set the event time
  void SetEventTime(double EventTime) { m_EventTime = EventTime; m_Empty = false; }
  //! Get the event time
  double GetEventTime() const { return m_EventTime; }


  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line);


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

  //! RESETTIME: Time since last reset
  double m_ResetTime;
  //! FRAME: ???
  int m_Frame;
  //! FLAG: ???
  int m_Flag;
  //! SUBFRAME: ????
  int m_SubFrame;
  //! RAWX: x row
  int m_RawX;
  //! RAWY: y row
  int m_RawY;
  
  //! STARTCAP:
  int m_StartCap;
  //! CZTID:
  int m_CZTID;
  //! RISETIME:
  double m_RiseTime;
  //! ELAPSET:
  int m_ElapsedTime;
  //! NHTRIG:
  int m_NHardwareTriggers;
  
  //! PREVAL: The pre-trigger sample sum (i.e. pre-pulseheight)
  vector<double> m_PreTriggerSampleSum;
  //! POSTVAL: The post-trigger sample sum (i.e. post-pulseheight)
  vector<double> m_PostTriggerSampleSum;
  //! TRIGGERMAP: The trigger flag
  vector<int> m_TriggerMap;
  
  //! NSTRIG: ???
  int m_NSoftwareTriggers;
  //! PHA: ???
  int m_PHA;
  //! ENERGY: Energy
  double m_Energy;
  //! CZTTEMP: CZT temperature ?
  double m_CZTTemperature;
  //! EVTIME: Event time ?
  double m_EventTime;



#ifdef ___CINT___
 public:
  ClassDef(NPHEFitsData, 0) // no description
#endif

};

//! Global operator for IO:
std::ostream& operator<<(std::ostream& os, const NPHEFitsData& PHE);

#endif


////////////////////////////////////////////////////////////////////////////////
