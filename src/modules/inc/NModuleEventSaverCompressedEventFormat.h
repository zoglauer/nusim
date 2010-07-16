/*
 * NModuleEventSaverCompressedEventFormat.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleEventSaverCompressedEventFormat__
#define __NModuleEventSaverCompressedEventFormat__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceExit.h"
#include "NModuleInterfaceIO.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleEventSaverCompressedEventFormat : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceExit, public NModuleInterfaceIO
{
  // public interface:
 public:
  //! Default constructor
  NModuleEventSaverCompressedEventFormat(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleEventSaverCompressedEventFormat();

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
  //! The output stream
  ofstream m_Out;



#ifdef ___CINT___
 public:
  ClassDef(NModuleEventSaverCompressedEventFormat, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
