/*
 * NModule.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModule__
#define __NModule__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "MXmlNode.h"

// NuSTAR libs:
#include "NSatellite.h"
#include "NGUIDiagnostics.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief The is the base class for all modules
//! This class is the base class of all modules. It stores the module type, its name, its XML tag, its tool tip, 
//! and provides a wide variety of common access classes.
//! Derived modules are usually enhanced by decorator classes, the module interfaces.
//! These make the individual modules part of the general analysis pipelines.

class NModule
{
  // public interface:
 public:
  //! Default constructor
  NModule(NSatellite& m_Satellite);
  //! Default destructor
  virtual ~NModule();

  //! Return a reference to the satellite
  NSatellite& GetSatelliteRef() { return m_Satellite; }

  //! Return the module type
  int GetModuleType() const { return m_ModuleType; }

  //! Return the type name of a module:
  static TString GetModuleTypeString(int ModuleType);

  //! Return true is the type/placement exits
  static bool IsUsedType(int Type);
  
  //! Return the type name of this module:
  TString GetModuleTypeString();

  //! Return the name of this module:
  TString GetName() const { return m_Name; }

  //! Return the module type name plus name of this module:
  TString GetFullName() { return GetModuleTypeString() + ": " + m_Name; }

  //! Return the XML tag of this module:
  TString GetXmlTag() const { return m_XmlTag; }

  //! Return the XML tag of this module:
  TString GetToolTip() const { return m_ToolTip; }

  //! Return all potential placements
  unsigned int GetNModulePlacements();
  //! Return the placement at position i
  int GetModulePlacement(unsigned int i);


  // Module types:

  static const int c_Unknown;

  // Satellite modules (attention if you add one here, add it also to the Validator):
  static const int c_Geometry;
  static const int c_OrbitEngine;
  static const int c_OrientationsEngine;
  static const int c_PointingEngine;
  static const int c_TimeEngine;
  static const int c_GeometryAndDetectorProperties;

  // Pipeline modules:
  // Pipeline - simulator
  static const int c_SourceGenerator;
  static const int c_OpticsEngine;
  static const int c_ApertureEngine;
  static const int c_DetectorEngine;
  static const int c_DetectorEffectsEngine;
  static const int c_TriggerEngine;

  static const int c_BackgroundEngine;

  // Pipeline - calibration
  static const int c_DetectorCalibrator;

  // Pipeline - analysis
  static const int c_EventSelector;
  static const int c_ScienceAnalyzer;

  // Pipeline - load save
  static const int c_EventLoader;
  static const int c_EventSaver;

  // Pipeline - metrology
  static const int c_MetrologyEngine;
  static const int c_MetrologyCalibrator;
  static const int c_MetrologyLoader;
  static const int c_MetrologySaver;

  // Pipeline - star tracker
  static const int c_StarTrackerEngine;
  static const int c_StarTrackerCalibrator;
  static const int c_StarTrackerLoader;
  static const int c_StarTrackerSaver;

  // Pipeline - observatory
  static const int c_ObservatoryReconstructor;
  static const int c_ObservatoryMerger;
  static const int c_ObservatoryLoader;
  static const int c_ObservatorySaver;

  //! Initialize the module --- has to be overwritten
  virtual bool Initialize() = 0;

  //! Finalize (not a C# delete!) the module --- can be overwritten
  virtual bool Finalize() { return true; }

  //! Show the options GUI --- has to be overwritten!
  virtual void ShowOptionsGUI() {}

  //! True if this module has an associated diagnostics GUI
  bool HasDiagnosticsGUI() { return m_HasDiagnosticsGUI; }

  //! Return the diagnostics GUI. Returns zero if the is no diagnostics GUI --- only called by TBD.
  virtual NGUIDiagnostics* GetDiagnosticsGUI() { return m_Diagnostics; }

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:
  //! Set the name of this module
  void SetName(TString Name) { m_Name = Name; }


  // private methods:
 private:



  // protected members:
 protected:
  //! Reference (!) to the satellite/geometry description
  NSatellite& m_Satellite;

  //! The module type
  int m_ModuleType;

  //! The potential places this module can take
  vector<int> m_ModulePlacements;

  //! Name of this module
  TString m_Name;
  //! Name of the XML tag --- has to be uniquie
  TString m_XmlTag;
  //! The tool tip information of this module
  TString m_ToolTip;

  //! True, if this module has an diagnostics GUI
  bool m_HasDiagnosticsGUI;
  //! The diagnostics GUI
  NGUIDiagnostics* m_Diagnostics;

  // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModule, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
