/*
 * NModuleBackgroundSimulatorAperture.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleBackgroundSimulatorAperture__
#define __NModuleBackgroundSimulatorAperture__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>

// ROOT libs:

// MEGAlib libs:
#include "MFunction.h"
#include "MFunction2D.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleBackgroundSimulatorAperture : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleBackgroundSimulatorAperture(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleBackgroundSimulatorAperture();

  //! Set the file name of the spectrum
  void SetFileNameSpectrum(TString Name) { m_FileNameSpectrum = Name; }
  //! Set the file name of the spectrum
  void SetFileNameFieldOfView(TString Name) { m_FileNameDistribution = Name; }

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
  //! The file name representing the spectrum
  TString m_FileNameSpectrum;
  //! The spectrum
  MFunction m_Spectrum;

  //! The name of the file representing the detector opening angle dfistribution
  TString m_FileNameDistribution;
  //! The distribution on the detector
  MFunction2D m_Distribution;

  //! The count rate
  double m_Rate;



 // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleBackgroundSimulatorAperture, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
