/*
 * NGUIMain.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIMain__
#define __NGUIMain__


////////////////////////////////////////////////////////////////////////////////


// Standard libs

// ROOT libs
#include <TGMenu.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGFileDialog.h>
#include <TGIcon.h>
#include <TGPicture.h>

// MEGAlib libs
#include "NGlobal.h"
#include "MGUIDialog.h"

// NuSTAR libs:
#include "NTextButton.h"
#include "NInterface.h"
#include "NSupervisor.h"

// Forward declarations:
class MGUIEFileSelector;


////////////////////////////////////////////////////////////////////////////////


//! \brief The main GUI window
//! The class represents the main GUI window

class NGUIMain : public TGMainFrame
{
  // Public members:
 public:
  //! Default constructor
  NGUIMain(NInterface* Interface, NSupervisor* Supervisor);
  //! Default destructor
  virtual ~NGUIMain();

  //! Create the GUI
  virtual void Create();
  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);
  //! Called when the "x" is pressed
  virtual void CloseWindow();

  //! Handle some keys
  bool HandleKey(Event_t* Event);


  // IDs - unfortunately we have to hard code these - we are in trouble if we have more than 100 modules...
  static const int c_Start            = 101;
  static const int c_Exit             = 102;
  static const int c_Toggle           = 103;
  static const int c_Supervisor       = 104;
  static const int c_LoadConfig       = 105;
  static const int c_SaveConfig       = 106;
  static const int c_About            = 107;
  static const int c_Switch           = 108;

  static const int c_RemovePipeline   = 400;
  static const int c_OptionsPipeline  = 500;
  static const int c_ChangePipeline   = 600;
  static const int c_OptionsSatellite = 700;
  static const int c_ChangeSatellite  = 800;
 
  // protected members:
 protected:
  //! Create the GUI
  void CreatePipelineFrame(TGCompositeFrame* Parent);
  //! Create the GUI
  void CreateSatelliteFrame(TGCompositeFrame* Parent);
  //! Update the module section
  void UpdateButtons();

  //! Actions when the change button of a pipeline module has been pressed
	virtual bool OnChangePipeline(unsigned int ModuleID);
  //! Actions when the options button of a pipeline module has been pressed
	virtual bool OnOptionsPipeline(unsigned int ModuleID);

  //! Actions when the apply button has been pressed
	virtual bool OnApply();
  //! Actions when the start button has been pressed
	virtual bool OnStart();
  //! Actions when the exit button has been pressed
	virtual bool OnExit();
  //! Actions when the cancel button has been pressed
	virtual bool OnToggle();
  //! Actions when the cancel button has been pressed
	virtual bool OnSupervisor();
  //! Actions when the save menu/key has been pressed
  virtual bool OnSaveConfiguration();
  //! Actions when the load menu/key has been pressed
  virtual bool OnLoadConfiguration();
  //! Actions when the About menu has been pressed
  virtual bool OnAbout();
  //! Actions when the switch button has been pressed
  virtual bool OnSwitch();

  // private members:
 private:
  //! Reference to all interface functions
  NInterface* m_Interface;
  //! Reference to all GUI data
  NSupervisor* m_Supervisor;

  //! Flag indicating that we either show the pipeline or the satellite modules
  bool m_IsPipeline;

  //! True if the analysis is running
  bool m_Analyzing;

  // Some common used GUI elements

  //! Frame conatining either the pipeline or the satellite frame
  TGCompositeFrame* m_MasterFrame;

  //! Frame conatining either the pipeline or the satellite frame
  TGCompositeFrame* m_SatelliteFrame;

  //! Frame representing the data flow
  TGCompositeFrame* m_PipelineFrame;

  //! The switch button
  TGTextButton* m_SwitchButton;
  //! The cancel button
  TGTextButton* m_ToggleButton;
  //! The start button
  TGTextButton* m_StartButton;

  
  // Pipeline buttons
  NTextButton* m_SourceGeneratorButton;
  NTextButton* m_OpticsEngineButton;
  NTextButton* m_ApertureEngineButton; 
  NTextButton* m_DetectorEngineButton;
  NTextButton* m_BackgroundEngineButton;
  NTextButton* m_StarTrackerEngineButton; 
  NTextButton* m_MetrologyEngineButton;
  NTextButton* m_DetectorEffectsEngineButton;
  NTextButton* m_TriggerEngineButton;
  NTextButton* m_DetectorCalibratorButton;
  NTextButton* m_StarTrackerCalibratorButton;
  NTextButton* m_MetrologyCalibratorButton;
  NTextButton* m_ObservatoryReconstructorButton;
  //NTextButton* m_ObservatoryMergerButton;
  NTextButton* m_EventSelectorButton;
  NTextButton* m_ScienceAnalyzerButton;

  // Satellite buttons
  NTextButton* m_TimeButton;
  NTextButton* m_OrbitButton;
  NTextButton* m_PointingButton;
  NTextButton* m_OrientationsButton; 
  NTextButton* m_GeometryAndDetectorPropertiesButton; 


  int m_ButtonSizeX;
  int m_ButtonSizeY;
  int m_ButtonMargins;
  int m_ButtonDistanceX;
  int m_ButtonDistanceY;
  int m_TextJustify;

  int m_Width;
  int m_Height;
  

#ifdef ___CINT___
 public:
  ClassDef(NGUIMain, 0) // main window of the Nuclearizer GUI
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
