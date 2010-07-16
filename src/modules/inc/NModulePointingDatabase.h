/*
 * NModulePointingDatabase.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModulePointingDatabase__
#define __NModulePointingDatabase__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModule.h"
#include "NModuleInterfacePointing.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModulePointingDatabase : public NModule, public NModuleInterfacePointing
{
  // public interface:
 public:
  //! Default constructor
  NModulePointingDatabase(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModulePointingDatabase();

  //! Initialize the module
  virtual bool Initialize();

  //! Return the pointing of the satellte at a given time
  virtual NPointing GetPointing(NTime Time);

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
  ClassDef(NModulePointingDatabase, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
