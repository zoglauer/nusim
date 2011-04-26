/*
 * NModuleEventSelector.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleEventSelector__
#define __NModuleEventSelector__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceIO.h"
#include "NModuleInterfaceEventSaverAscii.h"
#include "NModuleInterfaceEventSaverLevel2Fits.h"
#include "NModuleInterfaceEventSaverROOTTree.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleEventSelector : public NModule, public NModuleInterfaceEvent, 
  public NModuleInterfaceIO, public NModuleInterfaceEventSaverAscii,
  public NModuleInterfaceEventSaverLevel2Fits,
  public NModuleInterfaceEventSaverROOTTree
{
  // public interface:
 public:
  //! Default constructor
  NModuleEventSelector(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleEventSelector();

  //! Set the minimum energy
  void SetEnergyMin(const double EnergyMin) { m_EnergyMin = EnergyMin; }
  //! Get the minimum energy
  double GetEnergyMin() const { return m_EnergyMin; }
  //! Set the maximum energy
  void SetEnergyMax(const double EnergyMax) { m_EnergyMax = EnergyMax; }
  //! Get the maximum energy
  double GetEnergyMax() const { return m_EnergyMax; }

  //! Set the maximum depth
  void SetDepthMax(const double DepthMax) { m_DepthMax = DepthMax; }
  //! Get the maximum depth
  double GetDepthMax() const { return m_DepthMax; }

  //! Set the event selecton flag by bad depth calibration flag
  void SetSelectByBadDepthCal(const bool SelectByBadDepthCal) { m_SelectByBadDepthCal = SelectByBadDepthCal; }
  //! Get the event selecton flag by bad depth calibration flag
  bool GetSelectByBadDepthCal() const { return m_SelectByBadDepthCal; }

  //! Set the event selecton flag by depth cut
  void SetSelectByDepthCut(const bool SelectByDepthCut) { m_SelectByDepthCut = SelectByDepthCut; }
  //! Get the event selecton flag by depth cut
  bool GetSelectByDepthCut() const { return m_SelectByDepthCut; }

  //! Save before the event selctions
  void SetSaveBeforeSelections(const bool SaveBeforeSelections) { m_SaveBeforeSelections = SaveBeforeSelections; }
  //! Return if we should save before event selections
  double GetSaveBeforeSelections() const { return m_SaveBeforeSelections; }
  
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
  //! The minimum energy 
  double m_EnergyMin;
  //! The maximum energy
  double m_EnergyMax;

  //! The maximum depth
  double m_DepthMax;

  //! The event selecton flag by bad depth calibration flag
  bool m_SelectByBadDepthCal;
  //! The event selecton flag by depth cut
  bool m_SelectByDepthCut;
  
  //! True if we should save as fits file
  bool m_SaveAsFits;
  //! True if we should save as ROOT file
  bool m_SaveAsROOT;
  //! True if we should save as ROOT file for NuSTAR response
  bool m_SaveAsResponseROOT;
  //! True if we should save as dat file
  bool m_SaveAsDat;
  //! True if we should save before the event selection have been applied
  bool m_SaveBeforeSelections;


#ifdef ___CINT___
 public:
  ClassDef(NModuleEventSelector, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
