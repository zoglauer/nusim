/*
 * NModulePointingPredefined.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModulePointingPredefined__
#define __NModulePointingPredefined__


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
class NModulePointingPredefined : public NModule, public NModuleInterfacePointing
{
  // public interface:
 public:
  //! Default constructor
  NModulePointingPredefined(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModulePointingPredefined();

  //! Initialize the module
  virtual bool Initialize();

  //! Return the pointing of the satellte at a given time
  virtual NPointing GetPointing(NTime Time);

  //! Set the pointing center
  void SetPointingCenter(const NPointing& P) { m_PointingCenter = P; }
  //! Get the pointing center
  NPointing GetPointingCenter() const { return m_PointingCenter; }

  //! Get the number of motion patterns available
  int GetNMotionPatterns() { return c_MotionPatternMax - c_MotionPatternMin + 1; }

  //! Get the name of a certain motion pattern
  static TString GetMotionPatternName(int ID);

  //! Get the current motion pattern
  int GetMotionPattern() const { return m_MotionPattern; }
  //! Set the motion pattern
  void SetMotionPattern(int Pattern);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Constant describing motion pattern 0 - fixed
  static const int c_MotionPatternPredefined;
  //! Constant describing motion pattern 1 - random walk within X arcmin
  static const int c_MotionPatternRandomWalk;
  
  //! ID of the smallest motion pattern
  static const int c_MotionPatternMin;
  //! ID of the largest motion pattern;
  static const int c_MotionPatternMax;

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The motion pattern
  int m_MotionPattern;
  //! The central ponting
  NPointing m_PointingCenter;

  //! The last pointing call for the random walk
  NPointing m_RWPointing;
  //! Time of the last pointing call for the random walk
  NTime m_RWTime;


#ifdef ___CINT___
 public:
  ClassDef(NModulePointingPredefined, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
