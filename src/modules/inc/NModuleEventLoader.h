/*
 * NModuleEventLoader.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleEventLoader__
#define __NModuleEventLoader__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceIO.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleEventLoader : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry, public NModuleInterfaceIO
{
  // public interface:
 public:
  //! Default constructor
  NModuleEventLoader(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleEventLoader();

  // Get absolute observation start time
  virtual NTime GetAbsoluteObservationStartTime() const { return m_AbsoluteObservationStartTime; }
  // Get absolute observation start time
  virtual NTime GetAbsoluteObservationEndTime() const { return m_AbsoluteObservationEndTime; }

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Finalize the module
  virtual bool Finalize();

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:

  // private members:
 private:
  //! The input stream
  ifstream m_In;

  //! We buffer one event, since we must know the next event time
  NEvent m_Event;

  //! The absolute observation start time
  NTime m_AbsoluteObservationStartTime;
  //! The absolute observation end time
  NTime m_AbsoluteObservationEndTime;



#ifdef ___CINT___
 public:
  ClassDef(NModuleEventLoader, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
