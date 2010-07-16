/*
 * NSupervisor.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NSupervisor__
#define __NSupervisor__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
#include <map>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NSatellite.h"
#include "NModule.h"

// Forward declarations:
class NModuleInterfaceEvent;
class NModuleInterfaceEntry;
class NModuleInterfaceStarTracker;
class NModuleInterfaceMetrology;
class NModuleInterfaceObservatory;
class NModuleInterfaceMerger;
class NGUIDiagnosticsMain;
class NGUIMain;

////////////////////////////////////////////////////////////////////////////////


//! \brief The central supervisor class in NuSIM
//! The class represents the central data analysis supervisor in NuSIM
//! It is responsible for managing the modules, for the simulations, and for laoding and saving data

class NSupervisor
{
  // public interface:
 public:
  //! Default constructor
  NSupervisor();
  //! Default destructor
  virtual ~NSupervisor();

  //! Reset all data
  void Clear();

  //! Run the simulation/amalysis
  bool Run();

  //! Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }
  //! Toggle Diagnostics window
  void ToggleDiagnosticsWindow();


  //! Load all data from a file
  bool Load(TString FileName);
  //! Save all data to a file
  bool Save(TString FileName);


  //! Return the number of available modules
  unsigned int GetNAvailableModules() { return m_AvailableModules.size(); }
  //! Return the available modules at position i --- no error checks are performed  
  NModule* GetAvailableModule(unsigned int i);
  //! Return the modules at position i in the current sequence --- no error checks are performed  
  NModule* GetAvailableModule(TString Name);
  //! Return the modules at position i in the current sequence --- no error checks are performed  
  NModule* GetAvailableModuleByXmlTag(TString Name);


  //! Return a list of the available modules of given type
  vector<NModule*> GetModuleList(const int ModuleType);
  //! Set the active volume for the given type
  void SetActiveModule(NModule* ActiveModule, int ModuleType = NModule::c_Unknown);
  //! Get the active volume for the given type
  NModule* GetActiveModule(const int ModuleType);
  //! Get the active volume of the given type
  void RemoveActiveModule(const int ModuleType);
  //! Get the active volume of the given type
  void RemoveAllActiveModules() { m_ActiveModules.clear(); }


  //! Return the number of modules in all current sequences
  unsigned int GetNModules() { return m_Modules.size(); }
  //! Return the modules at position i in all current sequences --- no error checks are performed  
  NModule* GetModule(unsigned int i);

  //! Return the observation time in seconds
  NTime GetObservationTime() const { return m_ObservationTime; }
  //! Set the observation time in seconds
  void SetObservationTime(NTime Time) { m_ObservationTime = Time; }

  //! Return the update interval in started events
  int GetUpdateInterval() const { return m_UpdateInterval; }
  //! Set the update interval in started events
  void SetUpdateInterval(int UpdateInterval) { m_UpdateInterval = UpdateInterval; }

  //! Get highest analysis level
  int GetHighestAnalysisLevel() const;

  // The GUI Layout options
  //! Get the X size of the GUI buttons in pixels
  int GetButtonSizeX() { return m_ButtonSizeX; }
  //! Get the Y size of the GUI buttons in pixels
  int GetButtonSizeY() { return m_ButtonSizeY; }
  //! Get the Text margins around the text in the GUI buttons
  int GetButtonMargins() { return m_ButtonMargins; }
  //! Get the X distance between the GUI buttons in pixels
  int GetButtonDistanceX() { return m_ButtonDistanceX; }
  //! Get the Y distance between the GUI buttons in pixels
  int GetButtonDistanceY() { return m_ButtonDistanceY; }

  // There are several analysis levels corresponding
  static const int c_DataRaw           = 0;
  static const int c_DataCalibrated    = 1;
  static const int c_DataReconstructed = 2;

  // protected methods:
 protected:
  //! Validate the sequence of possible modules
  bool Validate(int ModuleType);
  //! Validate the sequence of possible modules for a specific sequence 
  bool ValidateSequence(int ModuleType, vector<int> Sequence);
  //! Validate the sequence of possible modules for a specific sequence 
  bool ValidateConsistency();

  //! This function redetermines the entry modules
  bool GenerateAllLists();

  //! Return true if the given module type is a source module
  bool IsSourceModule(int ModuleType);
  //! Return true if the given module type is a background module
  bool IsBackgroundModule(int ModuleType);
  //! Return true if the given module type is a metrology module
  bool IsMetrologyModule(int ModuleType);
  //! Return true if the given module type is a star tracker module
  bool IsStarTrackerModule(int ModuleType);
  //! Return true if the given module type is a observatory module
  bool IsObservatoryModule(int ModuleType);
  //! Return true if the given module type is a geometry module
  bool IsGeometryModule(int ModuleType);
  //! Return true if the given module type is a geometry module
  bool IsMergerModule(int ModuleType);

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The satellite data
  NSatellite m_Satellite;

  //! The observation time which shall be simulated in seconds
  NTime m_ObservationTime;

  //! Update frequency of the diagnostics window in started events
  int m_UpdateInterval;


  //! List of all available modules
  vector<NModule*> m_AvailableModules;


  //! All active modules
  map<int, NModule*> m_ActiveModules;

  //! Sequence of all active modules
  vector<NModule*> m_Modules; // <--- to be removed

  //! Sequence of currently used pipeline modules
  vector<NModuleInterfaceEvent*> m_PipelineModules;
  //! All active entry modules
  vector<NModuleInterfaceEntry*> m_PipelineEntryModules;
  //! Sequence of currently used pipeline modules
  vector<NModuleInterfaceEvent*> m_BackgroundModules;
  //! Sequence of all used star tracker modules
  vector<NModuleInterfaceStarTracker*> m_StarTrackerModules;
  //! Sequence of all used metrology modules
  vector<NModuleInterfaceMetrology*> m_MetrologyModules;
  //! Sequence of all used observatory modules
  vector<NModuleInterfaceObservatory*> m_ObservatoryModules;
  //! Sequence of all used merging modules
  vector<NModuleInterfaceMerger*> m_MergerModules;

  //! Sequence of currently used satellite modules
  vector<NModule*> m_SatelliteModules;


  //! Default source sequence
  vector<int> m_SourceSequence;
  //! Default background sequence
  vector<int> m_BackgroundSequence;
  //! Default metrology sequence
  vector<int> m_MetrologySequence;
  //! Default observatory sequence
  vector<int> m_ObservatorySequence;
  //! Default Star tracker sequence
  vector<int> m_StarTrackerSequence;
  //! List of merger modules
  vector<int> m_MergerSequence;

  // The GUI Layout options
  //! X size of the GUI buttons in pixels
  int m_ButtonSizeX;
  //! Y size of the GUI buttons in pixels
  int m_ButtonSizeY;
  //! Text margins around the text in the GUI buttons
  int m_ButtonMargins;
  //! X distance between the GUI buttons in pixels
  int m_ButtonDistanceX;
  //! Y distance between the GUI buttons in pixels
  int m_ButtonDistanceY;


  //! True if the analysis is running
  bool m_Running;

  //! True signals the supervisor to interrupt the current run
  bool m_Interrupt;

  //! Toggle the diagnostics window
  bool m_ToggleDiagnostics;

  //! The dignostics window
  NGUIDiagnosticsMain* m_DiagnosticsGUI;

#ifdef ___CINT___
 public:
  ClassDef(NSupervisor, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
