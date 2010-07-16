/*
 * NModuleMetrologyEngineTrivial.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleMetrologyEngineTrivial__
#define __NModuleMetrologyEngineTrivial__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceMetrology.h"
#include "NModuleInterfacePhotonPropagation.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleMetrologyEngineTrivial : public NModule, public NModuleInterfaceMetrology, 
                                      public NModuleInterfaceEntry,
                                      public NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NModuleMetrologyEngineTrivial(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleMetrologyEngineTrivial();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeMetrologyData(NMetrologyData& Data);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Set the latitude
  void SetUpdateInterval(double UpdateInterval) { m_UpdateInterval = UpdateInterval; }
  //! Get the latitude
  double GetUpdateInterval() const { return m_UpdateInterval; }


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  double m_UpdateInterval;


  // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleMetrologyEngineTrivial, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
