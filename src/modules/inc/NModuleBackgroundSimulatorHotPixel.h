/*
 * NModuleBackgroundSimulatorHotPixel.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleBackgroundSimulatorHotPixel__
#define __NModuleBackgroundSimulatorHotPixel__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>

// ROOT libs:

// MEGAlib libs:
#include "MFunction.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceEvent.h"
#include "NHotPixel.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleBackgroundSimulatorHotPixel : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleBackgroundSimulatorHotPixel(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleBackgroundSimulatorHotPixel();

  void SetFileName(TString FileName) { m_FileName = FileName; }

  //! Initialize the module
  virtual bool Initialize();

  //! Perform a time jump:
  virtual void PerformTimeJump(const NTime& TimeJump);

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
  //! Determine the next event
  void DetermineNext();
   
  // private methods:
 private:



  // protected members:
 protected:
  //! The name of the hot pixel file
  TString m_FileName;
  //! The hot pixels
  vector<NHotPixel> m_HotPixels;
  //! ID of the hot pixel
  unsigned int m_NextHotPixelID;
  
 // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleBackgroundSimulatorHotPixel, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
