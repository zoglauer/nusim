/*
 * NModuleMetrologySaver.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleMetrologySaver__
#define __NModuleMetrologySaver__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceMetrology.h"
#include "NModuleInterfaceExit.h"
#include "NModuleInterfaceIO.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleMetrologySaver : public NModule, public NModuleInterfaceMetrology, public NModuleInterfaceExit, public NModuleInterfaceIO
{
  // public interface:
 public:
  //! Default constructor
  NModuleMetrologySaver(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleMetrologySaver();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeMetrologyData(NMetrologyData& Data);

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
  //! The output stream
  ofstream m_Out;


#ifdef ___CINT___
 public:
  ClassDef(NModuleMetrologySaver, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
