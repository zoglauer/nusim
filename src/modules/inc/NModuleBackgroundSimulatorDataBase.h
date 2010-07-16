/*
 * NModuleBackgroundSimulatorDataBase.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleBackgroundSimulatorDataBase__
#define __NModuleBackgroundSimulatorDataBase__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleBackgroundSimulatorDataBase : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleBackgroundSimulatorDataBase(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleBackgroundSimulatorDataBase();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Get the file name
  TString GetFileName() const { return m_FileName; }
  //! Set the file name
  void SetFileName(const TString FileName) { m_FileName = FileName; }

  // protected methods:
 protected:
  //! Return true, if the data base could be returned sucessfully
  bool OpenDataBase();
  //! Read next event, return false on error
  bool ReadNextEvent(vector<NInteraction>& Interaction);

  //! Mix some properties of the event, e.g. positions, energies
  void Mixer();

  // private methods:
 private:



  // protected members:
 protected:
  //! The file name
  TString m_FileName;

  //! The background data base file
  ifstream m_DB;

  //! The number og background events in the file
  int m_BackgroundEvents;
  //! The total simulation/observation time
  double m_BackgroundTime;

  //! Interactions of the next event
  vector<NInteraction> m_Interactions;

 // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleBackgroundSimulatorDataBase, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
