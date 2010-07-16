/*
 * NModuleGeometryAndDetectorProperties.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleGeometryAndDetectorProperties__
#define __NModuleGeometryAndDetectorProperties__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceGeometryAndDetectorProperties.h"

// MEGAlib:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleGeometryAndDetectorProperties : public NModule, public NModuleInterfaceGeometryAndDetectorProperties
{
  // public interface:
 public:
  //! Default constructor
  NModuleGeometryAndDetectorProperties(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleGeometryAndDetectorProperties();

  //! Initialize the module
  virtual bool Initialize();

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



#ifdef ___CINT___
 public:
  ClassDef(NModuleGeometryAndDetectorProperties, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
