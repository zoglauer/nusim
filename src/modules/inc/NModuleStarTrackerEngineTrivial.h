/*
 * NModuleStarTrackerEngineTrivial.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleStarTrackerEngineTrivial__
#define __NModuleStarTrackerEngineTrivial__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceStarTracker.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleStarTrackerEngineTrivial : public NModule, public NModuleInterfaceStarTracker, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleStarTrackerEngineTrivial(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleStarTrackerEngineTrivial();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeStarTrackerData(NStarTrackerData& Data);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Set the update interval
  void SetUpdateInterval(double UpdateInterval) { m_UpdateInterval = UpdateInterval; }
  //! Get the update interval
  double GetUpdateInterval() const { return m_UpdateInterval; }

  //! Enable the blur
  void EnableBlur(bool BlurEnabled = true) { m_BlurEnabled = BlurEnabled; }
  //! Enable the blur
  bool IsBlurEnabled() const { return m_BlurEnabled; }

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  //! The interval between updates
  double m_UpdateInterval;

  //! Flag indicating whether of not to use the blur
  bool m_BlurEnabled;

  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleStarTrackerEngineTrivial, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
