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
#include "NModuleInterfaceEventSaverROOTEnergyResponse.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleEventSelector : public NModule, public NModuleInterfaceEvent, 
  public NModuleInterfaceIO, public NModuleInterfaceEventSaverAscii,
  public NModuleInterfaceEventSaverLevel2Fits,
  public NModuleInterfaceEventSaverROOTTree,
  public NModuleInterfaceEventSaverROOTEnergyResponse
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

  //! Set if we should save the events as fits file
  void SetSaveEventsAsFits(const bool SaveAsFits) { m_SaveAsFits = SaveAsFits; }
  //! Return true, if we should save the events as fits file
  bool GetSaveEventsAsFits() const { return m_SaveAsFits; }

  //! Set if we should save the events as dat file
  void SetSaveEventsAsDat(const bool SaveAsDat) { m_SaveAsDat = SaveAsDat; }
  //! Return true, if we should save the events as dat file
  bool GetSaveEventsAsDat() const { return m_SaveAsDat; }

  //! Set if we should save the events as ROOT file
  void SetSaveEventsAsROOT(const bool SaveAsROOT) { m_SaveAsROOT = SaveAsROOT; }
  //! Return true, if we should save the events as ROOT file
  bool GetSaveEventsAsROOT() const { return m_SaveAsROOT; }

  //! Set if we should save the energy response as ROOT file
  void SetSaveEnergyResponseAsROOT(const bool SaveAsResponseROOT) { m_SaveAsResponseROOT = SaveAsResponseROOT; }
  //! Return true, if we should save the energy response as ROOT file
  bool GetSaveEnergyResponseAsROOT() const { return m_SaveAsResponseROOT; }

  //! Save before the event selections
  void SetSaveBeforeSelections(const bool SaveBeforeSelections) { m_SaveBeforeSelections = SaveBeforeSelections; }
  //! Return if we should save before event selections
  bool GetSaveBeforeSelections() const { return m_SaveBeforeSelections; }
  
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

  //! The pre-selection counts in the band 5-10 keV
  unsigned int m_Counts3To80;
  //! The pre-selection counts in the band 5-10 keV
  unsigned int m_Counts5To10;
  //! The pre-selection counts in the band 10-20 keV
  unsigned int m_Counts10To20;
  //! The pre-selection counts in the band 20-60 keV
  unsigned int m_Counts20To60;


#ifdef ___CINT___
 public:
  ClassDef(NModuleEventSelector, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
