/*
 * NModuleDetectorCalibratorSciSimCIE.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleDetectorCalibratorSciSimCIE__
#define __NModuleDetectorCalibratorSciSimCIE__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleDetectorCalibratorSciSimCIE : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleDetectorCalibratorSciSimCIE(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleDetectorCalibratorSciSimCIE();

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

  //! Set the gain for single-trigger events
  void   SetGainSingleTrigger(double GainSingleTrigger) { m_GainSingleTrigger = GainSingleTrigger; }
  //! Set the offset for single-trigger events
  void   SetOffsetSingleTrigger(double OffsetSingleTrigger) { m_OffsetSingleTrigger = OffsetSingleTrigger; }
  //! Set the gain for double-trigger events
  void   SetGainDoubleTrigger(double GainDoubleTrigger) { m_GainDoubleTrigger = GainDoubleTrigger; }
  //! Set the offset for double-trigger events
  void   SetOffsetDoubleTrigger(double OffsetDoubleTrigger) { m_OffsetDoubleTrigger = OffsetDoubleTrigger; }
  //! Set the gain for triple-trigger events
  void   SetGainTripleTrigger(double GainTripleTrigger) { m_GainTripleTrigger = GainTripleTrigger; }
  //! Set the offset for triple-trigger events
  void   SetOffsetTripleTrigger(double OffsetTripleTrigger) { m_OffsetTripleTrigger = OffsetTripleTrigger; }
  //! Set the gain for quadruple-trigger events
  void   SetGainQuadrupleTrigger(double GainQuadrupleTrigger) { m_GainQuadrupleTrigger = GainQuadrupleTrigger; }
  //! Set the offset for quadruple-trigger events
  void   SetOffsetQuadrupleTrigger(double OffsetQuadrupleTrigger) { m_OffsetQuadrupleTrigger = OffsetQuadrupleTrigger; }

  //! Get the gain for single-trigger events
  double GetGainSingleTrigger() const { return m_GainSingleTrigger; }
  //! Get the offset for single-trigger events
  double GetOffsetSingleTrigger() const { return m_OffsetSingleTrigger; }
  //! Get the gain for double-trigger events
  double GetGainDoubleTrigger() const { return m_GainDoubleTrigger; }
  //! Get the offset for double-trigger events
  double GetOffsetDoubleTrigger() const { return m_OffsetDoubleTrigger; }
  //! Get the gain for triple-trigger events
  double GetGainTripleTrigger() const { return m_GainTripleTrigger; }
  //! Get the offset for triple-trigger events
  double GetOffsetTripleTrigger() const { return m_OffsetTripleTrigger; }
  //! Get the gain for quadruple-trigger events
  double GetGainQuadrupleTrigger() const { return m_GainQuadrupleTrigger; }
  //! Get the offset for quadruple-trigger events
  double GetOffsetQuadrupleTrigger() const { return m_OffsetQuadrupleTrigger; }

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

  //! Gain for single-trigger events
  double m_GainSingleTrigger;
  //! Offset for single-trigger events
  double m_OffsetSingleTrigger;

  //! Gain for double-trigger events
  double m_GainDoubleTrigger;
  //! Offset for double-trigger events
  double m_OffsetDoubleTrigger;

  //! Gain for triple-trigger events
  double m_GainTripleTrigger;
  //! Offset for triple-trigger events
  double m_OffsetTripleTrigger;

  //! Gain for quadruple-trigger events
  double m_GainQuadrupleTrigger;
  //! Offset for quadruple-trigger events
  double m_OffsetQuadrupleTrigger;


#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorCalibratorSciSimCIE, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
