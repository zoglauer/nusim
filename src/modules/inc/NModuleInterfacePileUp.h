/*
 * NModuleInterfacePileUp.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfacePileUp__
#define __NModuleInterfacePileUp__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <list>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NEvent.h"
#include "NModuleInterfaceTimeJump.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! The class is an PileUp decorator for a trigget module.

class NModuleInterfacePileUp : public NModuleInterfaceTimeJump
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfacePileUp();
  //! Default destructor
  virtual ~NModuleInterfacePileUp();

  //! Perform a time jump:
  virtual void PerformTimeJump(const NTime& TimeJump) { 
    m_DelayedList1.clear(); m_DelayedList2.clear();
  } 

  //! Initialize
  virtual bool Initialize();
  
  //! Finalize
  virtual bool Finalize();
  
  //! Set if dead time is to be applied
  void SetApplyPileUp(bool Apply) { m_ApplyPileUp = Apply; }
  //! Check if dead time is to be applied
  bool GetApplyPileUp() const { return m_ApplyPileUp; }
  
  //! Create a delayed event (delayed relative to the original event), but now with pile-up!
  NEvent CreateDelayedPileUpEvent(NEvent& Event);
  
  //! The created event has trigger - so do some clean up
  void CleanUp(NEvent& Event);
  //! The created event has trigger - so do some clean up - everything
  void CleanUpAll(NEvent& Event);
  //! The created event has trigger - so do some clean up
  void CleanUpOne(NEvent& Event);

  
  // protected methods:
 protected:
  //! Add an event to another one as pile up...
  void Add(NEvent& Event, NEvent& Add, double Ratio);


  // private methods:
 private:



  // protected members:
 protected:
  //! True if dead time should be applied
  bool m_ApplyPileUp;
  
  //! The true pile-up time scale
  NTime m_TruePileUpTimeScale;
  //! The false pile-up scale - i.e. where the enrgy is corrupted
  NTime m_FalsePileUpTimeScale;
  
  //! A delayed event list for detector 1
  list<NEvent> m_DelayedList1;
  //! A delayed event list for detector 2
  list<NEvent> m_DelayedList2;
  
  //! Number of delayed events detector 1
  unsigned int m_NDelayedEvents1;
  //! Number of delayed events detector 2
  unsigned int m_NDelayedEvents2;
  
  //! Number of piled-up events detector 1
  unsigned int m_NPileUps1;
  //! Number of piled-up events detector 2
  unsigned int m_NPileUps2;
  
  //! Number of not-piled up events detector 1
  unsigned int m_NNotPileUps1;
  //! Number of not-piled up events detector 2
  unsigned int m_NNotPileUps2;
  
  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfacePileUp, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
