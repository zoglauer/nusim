/*
 * NModuleEventSelectorBackgroundSims.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleEventSelectorBackgroundSims__
#define __NModuleEventSelectorBackgroundSims__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleEventSelectorBackgroundSims : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleEventSelectorBackgroundSims(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleEventSelectorBackgroundSims();

  //! Set the minimum energy
  void SetEnergyMin(const double EnergyMin) { m_EnergyMin = EnergyMin; }
  //! Get the minimum energy
  double GetEnergyMin() const { return m_EnergyMin; }
  //! Set the maximum energy
  void SetEnergyMax(const double EnergyMax) { m_EnergyMax = EnergyMax; }
  //! Get the maximum energy
  double GetEnergyMax() const { return m_EnergyMax; }
  
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



#ifdef ___CINT___
 public:
  ClassDef(NModuleEventSelectorBackgroundSims, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
