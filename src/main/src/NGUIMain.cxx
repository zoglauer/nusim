/*
 * NGUIMain.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIMain.h"

// Standard libs:

// ROOT libs:
#include <KeySymbols.h>
#include <TApplication.h>
#include <TGPicture.h>
#include <TStyle.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TString.h>
#include <TGClient.h>
#include <TGResourcePool.h>
#include <TG3DLine.h>
#include <TColor.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MFile.h"
#include "MGUIEFileSelector.h"

// NuSTAR libs:
#include "NModule.h"
#include "NGUIModuleSelector.h"
#include "NGUISupervisor.h"
#include "NGUIAbout.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIMain)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIMain::NGUIMain(NInterface* Interface, NSupervisor* Supervisor)
  : TGMainFrame(gClient->GetRoot(), 350, 300, kVerticalFrame),
    m_Interface(Interface), m_Supervisor(Supervisor), m_IsPipeline(true)
{
  const unsigned int Number = 8;
  double Red[Number]   = { 0.0000, 0.2500, 0.5000, 1.0000, 1.0000, 1.0000, 1.0000, 1.0000 };
  double Green[Number] = { 0.0000, 0.0000, 0.0000, 0.0000, 0.5000, 1.0000, 1.0000, 1.0000 };
  double Blue[Number]  = { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.5000, 0.7500 };
  double Stops[Number] = { 0.0000, 0.0000, 0.1000, 0.3000, 0.5000,  0.700, 0.9000, 1.0000 };
    
  // Arithmetic-mean for nice black'n'white prints:
  double Min = (Red[0]+Green[0]+Blue[0])/3.0;
  double Max = (Red[Number-1]+Green[Number-1]+Blue[Number-1])/3.0;
  Stops[0] = 0.0;
  Stops[Number-1] = 1.0;
  for (unsigned int i = 1; i < Number-1; ++i) {
    double Value = ((Red[i]+Green[i]+Blue[i])/3.0-Min)/(Max-Min);
    Stops[i] = Value; 
  }
  TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 100);

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);

  BindKey(this, gVirtualX->KeysymToKeycode(kKey_L), kAnyModifier);
  BindKey(this, gVirtualX->KeysymToKeycode(kKey_S), kAnyModifier);
  BindKey(this, gVirtualX->KeysymToKeycode(kKey_E), kAnyModifier);
  BindKey(this, gVirtualX->KeysymToKeycode(kKey_Return), kAnyModifier);
  BindKey(this, gVirtualX->KeysymToKeycode(kKey_Enter), kAnyModifier);
  BindKey(this, gVirtualX->KeysymToKeycode(kKey_Escape), kAnyModifier);

  m_ButtonSizeX = m_Supervisor->GetButtonSizeX();
  m_ButtonSizeY = m_Supervisor->GetButtonSizeY();
  m_ButtonMargins = m_Supervisor->GetButtonMargins();
  m_ButtonDistanceX = m_Supervisor->GetButtonDistanceX();
  m_ButtonDistanceY = m_Supervisor->GetButtonDistanceY();
  m_TextJustify = kTextLeft | kTextCenterY;

  m_Width = 100;
  m_Height = 100;

  m_Analyzing = false;

  Create();
}


////////////////////////////////////////////////////////////////////////////////


NGUIMain::~NGUIMain()
{
  // Deep Cleanup automatically deletes all used GUI elements
}


////////////////////////////////////////////////////////////////////////////////


void NGUIMain::Create()
{
  // Create the main window

  // We start with a name and an icon...
  SetWindowName("NuSim");  


  // In the beginning we build the menus and define their layout, ...	
  TGLayoutHints* MenuBarItemLayoutLeft = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);
  TGLayoutHints* MenuBarItemLayoutRight = new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 0, 0, 0);
  
  // We continue with the menu bar and its layout ...
  TGLayoutHints* MenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);
  
  TGMenuBar* MenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame | kRaisedFrame);
  AddFrame(MenuBar, MenuBarLayout);

  TGPopupMenu* MenuFile = new TGPopupMenu(gClient->GetRoot());
  MenuFile->AddEntry("Load Configuration", c_LoadConfig);
  MenuFile->AddEntry("Save Configuration", c_SaveConfig);
  MenuFile->AddSeparator();
  MenuFile->AddEntry("Exit", c_Exit);
  MenuFile->Associate(this);
  MenuBar->AddPopup("File", MenuFile, MenuBarItemLayoutLeft);

  TGPopupMenu* MenuInfo = new TGPopupMenu(fClient->GetRoot());
  MenuInfo->AddEntry("About", c_About);
  MenuInfo->Associate(this);
  MenuBar->AddPopup("Info", MenuInfo, MenuBarItemLayoutRight);



  // The central frame

  TGLayoutHints* MasterFrameLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
  m_MasterFrame = new TGCompositeFrame(this, 100, 100);
  AddFrame(m_MasterFrame, MasterFrameLayout);

  CreatePipelineFrame(m_MasterFrame);




  // Final buttons

	// Frame around the buttons:
	TGHorizontalFrame* ButtonFrame = new TGHorizontalFrame(this, 150, 25);
	TGLayoutHints* ButtonFrameLayout =	new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX | kLHintsCenterX, 10, 10, 20, 10);
	AddFrame(ButtonFrame, ButtonFrameLayout);

	TGTextButton* SupervisorButton = new TGTextButton(ButtonFrame, "Supervisor", c_Supervisor); 
  TGLayoutHints* SupervisorButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);
  SupervisorButton->SetMargins(20, 20, 2, 2);
  SupervisorButton->Associate(this);
  ButtonFrame->AddFrame(SupervisorButton, SupervisorButtonLayout);

	m_SwitchButton = new TGTextButton(ButtonFrame, "Switch to satellite modules", c_Switch); 
  TGLayoutHints* SwitchButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 20, 0, 0, 0);
  m_SwitchButton->SetMargins(20, 20, 2, 2);
  m_SwitchButton->Associate(this);
  ButtonFrame->AddFrame(m_SwitchButton, SwitchButtonLayout);

	m_ToggleButton = new TGTextButton(ButtonFrame, "Toggle Diagnostics GUI", c_Toggle); 
  TGLayoutHints* ToggleButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 20, 0, 0, 0);
  m_ToggleButton->SetMargins(20, 20, 2, 2);
  m_ToggleButton->Associate(this);
  ButtonFrame->AddFrame(m_ToggleButton, ToggleButtonLayout);

	m_StartButton = new TGTextButton(ButtonFrame, "Start Analysis", c_Start); 
  TGLayoutHints* StartButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 20, 0, 0, 0);
  m_StartButton->SetMargins(20, 20, 2, 2);
  m_StartButton->Associate(this);
	ButtonFrame->AddFrame(m_StartButton, StartButtonLayout);
 

  MapSubwindows();
  MapWindow();  
  Layout();

  SetWMSize(m_Width, m_Height);
  SetWMSizeHints(m_Width, m_Height, m_Width, m_Height, 0, 0);

  return;
}


////////////////////////////////////////////////////////////////////////////////


void NGUIMain::CreateSatelliteFrame(TGCompositeFrame* Parent)
{
  // Section 2: Modules

  // In this section we just setup the buttons for the modules, 
  // The displayed names are set in UpdateModules()

  int StartX = 20;
  int StartY = 150;

  Pixel_t yellow = TColor::RGB2Pixel(255, 255, 153);


  TGLayoutHints* SatelliteFrameLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
  m_SatelliteFrame = new TGCompositeFrame(Parent, 100, 100);
  m_SatelliteFrame->SetLayoutBroken(true);
  Parent->AddFrame(m_SatelliteFrame, SatelliteFrameLayout);

  // Main label
  const TGFont* lFont = gClient->GetFont("-*-helvetica-bold-r-*-*-24-*-*-*-*-*-iso8859-1");
  if (!lFont) lFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t LargeFont = lFont->GetFontStruct();

  TString TitleIconName("$(NUSIM)/resource/icons/NuSIM_Yellow.xpm");
  MFile::ExpandFileName(TitleIconName);
  
  if (MFile::Exists(TitleIconName) == true) {
    const TGPicture* TitlePicture = fClient->GetPicture(TitleIconName);
    if (TitlePicture == 0) {
      mgui<<"Can't find picture "<<TitleIconName<<"! Aborting!"<<error;
      return;
    }
    TGIcon* TitleIcon = new TGIcon(m_SatelliteFrame, TitlePicture, TitlePicture->GetWidth()+2, TitlePicture->GetHeight()+2);
	  m_SatelliteFrame->AddFrame(TitleIcon);
    TitleIcon->MoveResize(StartX + 2.0*m_ButtonSizeX + 1.5*m_ButtonDistanceX - TitlePicture->GetWidth()/2, 60 - TitlePicture->GetHeight()/2, TitlePicture->GetWidth()+2, TitlePicture->GetHeight()+2);
  } else {
    TGLabel* MainLabel = new TGLabel(m_SatelliteFrame, "NuSim");
    MainLabel->SetTextFont(LargeFont);
	  m_SatelliteFrame->AddFrame(MainLabel);
    MainLabel->MoveResize(StartX + 2.0*m_ButtonSizeX + 1.5*m_ButtonDistanceX - 45, 60, 80, 36);
  }

  // Sub-title
  const TGFont* iFont = gClient->GetFont("-*-helvetica-medium-o-*-*-12-*-*-*-*-*-iso8859-1");
  if (!iFont) iFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t ItalicFont = iFont->GetFontStruct();

  ostringstream Text;
  Text<<"The NuSTAR science simulator (Version: "<<g_Version<<", SVN revision "<<g_SVNRevision<<")"<<endl;

  TGLabel* SubTitle = new TGLabel(m_SatelliteFrame, Text.str().c_str());
  SubTitle->SetTextFont(ItalicFont);
	m_SatelliteFrame->AddFrame(SubTitle);
  SubTitle->Move(StartX + 2.0*m_ButtonSizeX + 1.5*m_ButtonDistanceX - 180, 100);


  // The buttons
  m_OrbitButton = new NTextButton(m_SatelliteFrame, "Orbit engine:\nNo module", NModule::c_OrbitEngine);
  m_OrbitButton->SetTextJustify(m_TextJustify);
  m_OrbitButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_OrbitButton->SetBackgroundColor(yellow);
  m_OrbitButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_OrbitButton->SetEnabled(false);
  m_SatelliteFrame->AddFrame(m_OrbitButton);
  m_OrbitButton->MoveResize(StartX + 0.5*(m_ButtonSizeX+m_ButtonDistanceX), StartY, m_ButtonSizeX, m_ButtonSizeY);

  m_PointingButton = new NTextButton(m_SatelliteFrame, "Pointing engine:\nNo module", NModule::c_PointingEngine);
  m_PointingButton->SetTextJustify(m_TextJustify);
  m_PointingButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_PointingButton->SetBackgroundColor(yellow);
  m_PointingButton->Associate(this);
  m_SatelliteFrame->AddFrame(m_PointingButton);
  m_PointingButton->MoveResize(StartX + 1.5*(m_ButtonSizeX+m_ButtonDistanceX), StartY, m_ButtonSizeX, m_ButtonSizeY);

  m_OrientationsButton = new NTextButton(m_SatelliteFrame, "Orientation engine:\nNo module", NModule::c_OrientationsEngine);
  m_OrientationsButton->SetTextJustify(m_TextJustify);
  m_OrientationsButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_OrientationsButton->SetBackgroundColor(yellow);
  m_OrientationsButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_OrientationsButton->SetEnabled(false);
  m_SatelliteFrame->AddFrame(m_OrientationsButton);
  m_OrientationsButton->MoveResize(StartX + 2.5*(m_ButtonSizeX+m_ButtonDistanceX), StartY, m_ButtonSizeX, m_ButtonSizeY);

  m_TimeButton = new NTextButton(m_SatelliteFrame, "Time engine:\nNo module", NModule::c_TimeEngine);
  m_TimeButton->SetTextJustify(m_TextJustify);
  m_TimeButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_TimeButton->SetBackgroundColor(yellow);
  m_TimeButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_TimeButton->SetEnabled(false);
  m_SatelliteFrame->AddFrame(m_TimeButton);
  m_TimeButton->MoveResize(StartX + 1.0*(m_ButtonSizeX+m_ButtonDistanceX), StartY + 1.0*(m_ButtonSizeY+m_ButtonDistanceY), m_ButtonSizeX, m_ButtonSizeY);

  m_GeometryAndDetectorPropertiesButton = new NTextButton(m_SatelliteFrame, "Detector properties:\nNo module", NModule::c_GeometryAndDetectorProperties);
  m_GeometryAndDetectorPropertiesButton->SetTextJustify(m_TextJustify);
  m_GeometryAndDetectorPropertiesButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_GeometryAndDetectorPropertiesButton->SetBackgroundColor(yellow);
  m_GeometryAndDetectorPropertiesButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_GeometryAndDetectorPropertiesButton->SetEnabled(false);
  m_SatelliteFrame->AddFrame(m_GeometryAndDetectorPropertiesButton);
  m_GeometryAndDetectorPropertiesButton->MoveResize(StartX + 2.0*(m_ButtonSizeX+m_ButtonDistanceX), StartY + 1.0*(m_ButtonSizeY+m_ButtonDistanceY), m_ButtonSizeX, m_ButtonSizeY);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIMain::CreatePipelineFrame(TGCompositeFrame* Parent)
{
  // Section 2: Modules

  // In this section we just setup the buttons for the modules, 
  // The displayed names are set in UpdateModules()

  int StartX = 20;
  int StartY = 20;
  int CurrentY = StartY;

  Pixel_t red = TColor::RGB2Pixel(255, 153, 153);
  Pixel_t green = TColor::RGB2Pixel(153, 255, 153);
  Pixel_t blue = TColor::RGB2Pixel(153, 153, 255);

  TGLayoutHints* PipelineFrameLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
  m_PipelineFrame = new TGCompositeFrame(Parent, 100, 100);
  m_PipelineFrame->SetLayoutBroken(true);
  Parent->AddFrame(m_PipelineFrame, PipelineFrameLayout);
  m_IsPipeline = true;


  // Main label
  const TGFont* lFont = gClient->GetFont("-*-helvetica-bold-r-*-*-24-*-*-*-*-*-iso8859-1");
  if (!lFont) lFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t LargeFont = lFont->GetFontStruct();

  TString TitleIconName("$(NUSIM)/resource/icons/NuSIM_Red.xpm");
  MFile::ExpandFileName(TitleIconName);
  
  if (MFile::Exists(TitleIconName) == true) {
    const TGPicture* TitlePicture = fClient->GetPicture(TitleIconName);
    if (TitlePicture == 0) {
      mgui<<"Can't find picture "<<TitleIconName<<"! Aborting!"<<error;
      return;
    }
    TGIcon* TitleIcon = new TGIcon(m_PipelineFrame, TitlePicture, TitlePicture->GetWidth()+2, TitlePicture->GetHeight()+2);
	  m_PipelineFrame->AddFrame(TitleIcon);
    TitleIcon->MoveResize(StartX + 2.5*m_ButtonSizeX + 2.0*m_ButtonDistanceX - TitlePicture->GetWidth()/2, 60 - TitlePicture->GetHeight()/2, TitlePicture->GetWidth()+2, TitlePicture->GetHeight()+2);
  } else {
    TGLabel* MainLabel = new TGLabel(m_PipelineFrame, "NuSim");
    MainLabel->SetTextFont(LargeFont);
	  m_PipelineFrame->AddFrame(MainLabel);
    MainLabel->MoveResize(StartX + 2.5*m_ButtonSizeX + 2*m_ButtonDistanceX - 45, 60, 80, 36);
  }

  // Sub-title
  const TGFont* iFont = gClient->GetFont("-*-helvetica-medium-o-*-*-12-*-*-*-*-*-iso8859-1");
  if (!iFont) iFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t ItalicFont = iFont->GetFontStruct();

  ostringstream Text;
  Text<<"The NuSTAR science simulator (Version: "<<g_Version<<", SVN revision "<<g_SVNRevision<<")"<<endl;

  TGLabel* SubTitle = new TGLabel(m_PipelineFrame, Text.str().c_str());
  SubTitle->SetTextFont(ItalicFont);
	m_PipelineFrame->AddFrame(SubTitle);
  SubTitle->Move(StartX + 2.5*m_ButtonSizeX + 2*m_ButtonDistanceX - 180, 100);


  TGVertical3DLine* VLine = 0;
  TGHorizontal3DLine* HLine = 0;

  m_SourceGeneratorButton = new NTextButton(m_PipelineFrame, "Source generator:\nNo module", NModule::c_SourceGenerator);
  m_SourceGeneratorButton->SetTextJustify(m_TextJustify);
  m_SourceGeneratorButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_SourceGeneratorButton->SetBackgroundColor(red);
  m_SourceGeneratorButton->Associate(this);
  m_PipelineFrame->AddFrame(m_SourceGeneratorButton);
  m_SourceGeneratorButton->MoveResize(StartX, StartY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 0.5*m_ButtonSizeX, CurrentY+m_ButtonSizeY);


  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_OpticsEngineButton = new NTextButton(m_PipelineFrame, "Optics Engine:\nNo module", NModule::c_OpticsEngine);
  m_OpticsEngineButton->SetTextJustify(m_TextJustify);
  m_OpticsEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_OpticsEngineButton->SetBackgroundColor(red);
  m_OpticsEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_OpticsEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_OpticsEngineButton);
  m_OpticsEngineButton->MoveResize(StartX, CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 0.5*m_ButtonSizeX, CurrentY+m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_ApertureEngineButton = new NTextButton(m_PipelineFrame, "Aperture Engine:\nNo module", NModule::c_ApertureEngine);
  m_ApertureEngineButton->SetTextJustify(m_TextJustify);
  m_ApertureEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_ApertureEngineButton->SetBackgroundColor(red);
  m_ApertureEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_ApertureEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_ApertureEngineButton);
  m_ApertureEngineButton->MoveResize(StartX, CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 0.5*m_ButtonSizeX, CurrentY+m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_DetectorEngineButton = new NTextButton(m_PipelineFrame, "Detector Engine:\nNo module", NModule::c_DetectorEngine);
  m_DetectorEngineButton->SetTextJustify(m_TextJustify);
  m_DetectorEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_DetectorEngineButton->SetBackgroundColor(red);
  m_DetectorEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_DetectorEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_DetectorEngineButton);
  m_DetectorEngineButton->MoveResize(StartX, CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  HLine = new TGHorizontal3DLine(m_PipelineFrame, 0.5*m_ButtonDistanceX, 2);
  m_PipelineFrame->AddFrame(HLine);
  HLine->Move(StartX + 0.5*m_ButtonSizeX, CurrentY+1.5*m_ButtonSizeY+m_ButtonDistanceY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, 0.5*m_ButtonSizeY+m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 0.5*m_ButtonSizeX, CurrentY+m_ButtonSizeY);

  m_BackgroundEngineButton = new NTextButton(m_PipelineFrame, "Background Engine:\nNo module", NModule::c_BackgroundEngine);
  m_BackgroundEngineButton->SetTextJustify(m_TextJustify);
  m_BackgroundEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_BackgroundEngineButton->SetBackgroundColor(red);
  m_BackgroundEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_BackgroundEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_BackgroundEngineButton);
  m_BackgroundEngineButton->MoveResize(StartX + 1*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  HLine = new TGHorizontal3DLine(m_PipelineFrame, 0.5*m_ButtonDistanceX+3, 2);
  m_PipelineFrame->AddFrame(HLine);
  HLine->Move(StartX + 1.5*m_ButtonSizeX + 0.5*m_ButtonDistanceX, CurrentY+1.5*m_ButtonSizeY+m_ButtonDistanceY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, 0.5*m_ButtonSizeY+m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 1.5*m_ButtonSizeX + m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  m_StarTrackerEngineButton = new NTextButton(m_PipelineFrame, "Star Tracker Engine:\nNo module", NModule::c_StarTrackerEngine);
  m_StarTrackerEngineButton->SetTextJustify(m_TextJustify);
  m_StarTrackerEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_StarTrackerEngineButton->SetBackgroundColor(red);
  m_StarTrackerEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_StarTrackerEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_StarTrackerEngineButton);
  m_StarTrackerEngineButton->MoveResize(StartX + 2*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, 2*m_ButtonSizeY + 3*m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 2.5*m_ButtonSizeX + 2*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  m_MetrologyEngineButton = new NTextButton(m_PipelineFrame, "Metrology Engine:\nNo module", NModule::c_MetrologyEngine);
  m_MetrologyEngineButton->SetTextJustify(m_TextJustify);
  m_MetrologyEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_MetrologyEngineButton->SetBackgroundColor(red);
  m_MetrologyEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_MetrologyEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_MetrologyEngineButton);
  m_MetrologyEngineButton->MoveResize(StartX + 3*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, 2*m_ButtonSizeY + 3*m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 3.5*m_ButtonSizeX + 3*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);
  
  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_DetectorEffectsEngineButton = new NTextButton(m_PipelineFrame, "Detector EffectsEngine:\nNo module", NModule::c_DetectorEffectsEngine);
  m_DetectorEffectsEngineButton->SetTextJustify(m_TextJustify);
  m_DetectorEffectsEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_DetectorEffectsEngineButton->SetBackgroundColor(red);
  m_DetectorEffectsEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_DetectorEffectsEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_DetectorEffectsEngineButton);
  m_DetectorEffectsEngineButton->MoveResize(StartX + 0.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + m_ButtonSizeX + 0.5*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_TriggerEngineButton = new NTextButton(m_PipelineFrame, "Trigger Engine:\nNo module", NModule::c_TriggerEngine);
  m_TriggerEngineButton->SetTextJustify(m_TextJustify);
  m_TriggerEngineButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_TriggerEngineButton->SetBackgroundColor(red);
  m_TriggerEngineButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_TriggerEngineButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_TriggerEngineButton);
  m_TriggerEngineButton->MoveResize(StartX + 0.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + m_ButtonSizeX + 0.5*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_DetectorCalibratorButton = new NTextButton(m_PipelineFrame, "Detector Calibrator:\nNo module", NModule::c_DetectorCalibrator);
  m_DetectorCalibratorButton->SetTextJustify(m_TextJustify);
  m_DetectorCalibratorButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_DetectorCalibratorButton->SetBackgroundColor(green);
  m_DetectorCalibratorButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_DetectorCalibratorButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_DetectorCalibratorButton);
  m_DetectorCalibratorButton->MoveResize(StartX + 0.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, 2*m_ButtonSizeY + 3*m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + m_ButtonSizeX + 0.5*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  m_StarTrackerCalibratorButton = new NTextButton(m_PipelineFrame, "Star Tracker Calibrator:\nNo module", NModule::c_StarTrackerCalibrator);
  m_StarTrackerCalibratorButton->SetTextJustify(m_TextJustify);
  m_StarTrackerCalibratorButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_StarTrackerCalibratorButton->SetBackgroundColor(green);
  m_StarTrackerCalibratorButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_StarTrackerCalibratorButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_StarTrackerCalibratorButton);
  m_StarTrackerCalibratorButton->MoveResize(StartX + 2*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY + 0.5*m_ButtonSizeY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 2.5*m_ButtonSizeX + 2*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  HLine = new TGHorizontal3DLine(m_PipelineFrame, 0.5*m_ButtonDistanceX, 2);
  m_PipelineFrame->AddFrame(HLine);
  HLine->Move(StartX + 2.5*m_ButtonSizeX + 2*m_ButtonDistanceX, CurrentY + 1.5*m_ButtonSizeY + m_ButtonDistanceY);

  m_MetrologyCalibratorButton = new NTextButton(m_PipelineFrame, "Metrology Calibrator:\nNo module", NModule::c_MetrologyCalibrator);
  m_MetrologyCalibratorButton->SetTextJustify(m_TextJustify);
  m_MetrologyCalibratorButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_MetrologyCalibratorButton->SetBackgroundColor(green);
  m_MetrologyCalibratorButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_MetrologyCalibratorButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_MetrologyCalibratorButton);
  m_MetrologyCalibratorButton->MoveResize(StartX + 3*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY + 0.5*m_ButtonSizeY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 3.5*m_ButtonSizeX + 3*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  HLine = new TGHorizontal3DLine(m_PipelineFrame, 0.5*m_ButtonDistanceX+3, 2);
  m_PipelineFrame->AddFrame(HLine);
  HLine->Move(StartX + 3.5*m_ButtonSizeX + 2.5*m_ButtonDistanceX, CurrentY + 1.5*m_ButtonSizeY + m_ButtonDistanceY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_ObservatoryReconstructorButton = new NTextButton(m_PipelineFrame, "Observatory reconstructor:\nNo module", NModule::c_ObservatoryReconstructor);
  m_ObservatoryReconstructorButton->SetTextJustify(m_TextJustify);
  m_ObservatoryReconstructorButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_ObservatoryReconstructorButton->SetBackgroundColor(green);
  m_ObservatoryReconstructorButton->Associate(this);
  if (m_Supervisor->GetAstrophysicsMode() == true) m_ObservatoryReconstructorButton->SetEnabled(false);
  m_PipelineFrame->AddFrame(m_ObservatoryReconstructorButton);
  m_ObservatoryReconstructorButton->MoveResize(StartX + 2.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY+0.5*m_ButtonSizeY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + 3*m_ButtonSizeX + 2.5*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;


//   m_ObservatoryMergerButton = new NTextButton(m_PipelineFrame, "Metrology Merger:\nNo module", NModule::c_ObservatoryMerger);
//   m_ObservatoryMergerButton->SetTextJustify(m_TextJustify);
//   m_ObservatoryMergerButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
//   m_ObservatoryMergerButton->SetBackgroundColor(green);
//   m_ObservatoryMergerButton->Associate(this);
//   m_PipelineFrame->AddFrame(m_ObservatoryMergerButton);
//   m_ObservatoryMergerButton->MoveResize(StartX + 2.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  HLine = new TGHorizontal3DLine(m_PipelineFrame, 2*m_ButtonDistanceX + 2.0*m_ButtonSizeX + 2, 2);
  m_PipelineFrame->AddFrame(HLine);
  HLine->Move(StartX + m_ButtonSizeX + 0.5*m_ButtonDistanceX, CurrentY + 0.5*m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_EventSelectorButton = new NTextButton(m_PipelineFrame, "Event Selector:\nNo module", NModule::c_EventSelector);
  m_EventSelectorButton->SetTextJustify(m_TextJustify);
  m_EventSelectorButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_EventSelectorButton->SetBackgroundColor(blue);
  m_EventSelectorButton->Associate(this);
  m_PipelineFrame->AddFrame(m_EventSelectorButton);
  m_EventSelectorButton->MoveResize(StartX + 0.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  VLine = new TGVertical3DLine(m_PipelineFrame, 2, m_ButtonDistanceY);
  m_PipelineFrame->AddFrame(VLine);
  VLine->Move(StartX + m_ButtonSizeX + 0.5*m_ButtonDistanceX, CurrentY+m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  m_ScienceAnalyzerButton = new NTextButton(m_PipelineFrame, "Science Analyzer:\nNo module", NModule::c_ScienceAnalyzer);
  m_ScienceAnalyzerButton->SetTextJustify(m_TextJustify);
  m_ScienceAnalyzerButton->SetMargins(m_ButtonMargins, m_ButtonMargins, m_ButtonMargins, m_ButtonMargins);
  m_ScienceAnalyzerButton->SetBackgroundColor(blue);
  m_ScienceAnalyzerButton->Associate(this);
  m_PipelineFrame->AddFrame(m_ScienceAnalyzerButton);
  m_ScienceAnalyzerButton->MoveResize(StartX + 0.5*(m_ButtonSizeX+m_ButtonDistanceX), CurrentY, m_ButtonSizeX, m_ButtonSizeY);

  CurrentY += m_ButtonSizeY+m_ButtonDistanceY;

  UpdateButtons();

  CurrentY += 20;


  if (m_Width < 2*StartX + 4*m_ButtonSizeX + 3*m_ButtonDistanceX) {
    m_Width =  2*StartX + 4*m_ButtonSizeX + 3*m_ButtonDistanceX;
  }
  if (m_Height < CurrentY+54) {
    m_Height = CurrentY+54;
  }
}


////////////////////////////////////////////////////////////////////////////////


void NGUIMain::UpdateButtons()
{
  // Set the GUI text of all the pipeline module

  NModule* Module;
  TString NoModule = "-";
  TString NoModuleToolTip = "Please right click to set a module.";
  int ToolTipDelay = 1000;

  if (m_IsPipeline == true) {
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_SourceGenerator)) != 0) {
      m_SourceGeneratorButton->SetText(NModule::GetModuleTypeString(NModule::c_SourceGenerator) + ":\n" + Module->GetName());
      m_SourceGeneratorButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_SourceGeneratorButton->SetText(NModule::GetModuleTypeString(NModule::c_SourceGenerator) + ":\n" + NoModule);
      m_SourceGeneratorButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_OpticsEngine)) != 0) {
      m_OpticsEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_OpticsEngine) + ":\n" + Module->GetName());
      m_OpticsEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_OpticsEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_OpticsEngine) + ":\n" + NoModule);
      m_OpticsEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_ApertureEngine)) != 0) {
      m_ApertureEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_ApertureEngine) + ":\n" + Module->GetName());
      m_ApertureEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_ApertureEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_ApertureEngine) + ":\n" + NoModule);
      m_ApertureEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_DetectorEngine)) != 0) {
      m_DetectorEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_DetectorEngine) + ":\n" + Module->GetName());
      m_DetectorEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_DetectorEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_DetectorEngine) + ":\n" + NoModule);
      m_DetectorEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_BackgroundEngine)) != 0) {
      m_BackgroundEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_BackgroundEngine) + ":\n" + Module->GetName());
      m_BackgroundEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_BackgroundEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_BackgroundEngine) + ":\n" + NoModule);
      m_BackgroundEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_StarTrackerEngine)) != 0) {
      m_StarTrackerEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_StarTrackerEngine) + ":\n" + Module->GetName());
      m_StarTrackerEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_StarTrackerEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_StarTrackerEngine) + ":\n" + NoModule);
      m_StarTrackerEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_MetrologyEngine)) != 0) {
      m_MetrologyEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_MetrologyEngine) + ":\n" + Module->GetName());
      m_MetrologyEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_MetrologyEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_MetrologyEngine) + ":\n" + NoModule);
      m_MetrologyEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_DetectorEffectsEngine)) != 0) {
      m_DetectorEffectsEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_DetectorEffectsEngine) + ":\n" + Module->GetName());
      m_DetectorEffectsEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_DetectorEffectsEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_DetectorEffectsEngine) + ":\n" + NoModule);
      m_DetectorEffectsEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_TriggerEngine)) != 0) {
      m_TriggerEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_TriggerEngine) + ":\n" + Module->GetName());
      m_TriggerEngineButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_TriggerEngineButton->SetText(NModule::GetModuleTypeString(NModule::c_TriggerEngine) + ":\n" + NoModule);
      m_TriggerEngineButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_DetectorCalibrator)) != 0) {
      m_DetectorCalibratorButton->SetText(NModule::GetModuleTypeString(NModule::c_DetectorCalibrator) + ":\n" + Module->GetName());
      m_DetectorCalibratorButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_DetectorCalibratorButton->SetText(NModule::GetModuleTypeString(NModule::c_DetectorCalibrator) + ":\n" + NoModule);
      m_DetectorCalibratorButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_StarTrackerCalibrator)) != 0) {
      m_StarTrackerCalibratorButton->SetText(NModule::GetModuleTypeString(NModule::c_StarTrackerCalibrator) + ":\n" + Module->GetName());
      m_StarTrackerCalibratorButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_StarTrackerCalibratorButton->SetText(NModule::GetModuleTypeString(NModule::c_StarTrackerCalibrator) + ":\n" + NoModule);
      m_StarTrackerCalibratorButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_MetrologyCalibrator)) != 0) {
      m_MetrologyCalibratorButton->SetText(NModule::GetModuleTypeString(NModule::c_MetrologyCalibrator) + ":\n" + Module->GetName());
      m_MetrologyCalibratorButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_MetrologyCalibratorButton->SetText(NModule::GetModuleTypeString(NModule::c_MetrologyCalibrator) + ":\n" + NoModule);
      m_MetrologyCalibratorButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_ObservatoryReconstructor)) != 0) {
      m_ObservatoryReconstructorButton->SetText(NModule::GetModuleTypeString(NModule::c_ObservatoryReconstructor) + ":\n" + Module->GetName());
      m_ObservatoryReconstructorButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_ObservatoryReconstructorButton->SetText(NModule::GetModuleTypeString(NModule::c_ObservatoryReconstructor) + ":\n" + NoModule);
      m_ObservatoryReconstructorButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
//     if ((Module = m_Supervisor->GetActiveModule(NModule::c_ObservatoryMerger)) != 0) {
//       m_ObservatoryMergerButton->SetText(NModule::GetModuleTypeString(NModule::c_ObservatoryMerger) + ":\n" + Module->GetName());
//       m_ObservatoryMergerButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
//     } else {
//       m_ObservatoryMergerButton->SetText(NModule::GetModuleTypeString(NModule::c_ObservatoryMerger) + ":\n" + NoModule);
//       m_ObservatoryMergerButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
//     }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_EventSelector)) != 0) {
      m_EventSelectorButton->SetText(NModule::GetModuleTypeString(NModule::c_EventSelector) + ":\n" + Module->GetName());
      m_EventSelectorButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_EventSelectorButton->SetText(NModule::GetModuleTypeString(NModule::c_EventSelector) + ":\n" + NoModule);
      m_EventSelectorButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_ScienceAnalyzer)) != 0) {
      m_ScienceAnalyzerButton->SetText(NModule::GetModuleTypeString(NModule::c_ScienceAnalyzer) + ":\n" + Module->GetName());
      m_ScienceAnalyzerButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_ScienceAnalyzerButton->SetText(NModule::GetModuleTypeString(NModule::c_ScienceAnalyzer) + ":\n" + NoModule);
      m_ScienceAnalyzerButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
  } else {
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_TimeEngine)) != 0) {
      m_TimeButton->SetText(NModule::GetModuleTypeString(NModule::c_TimeEngine) + ":\n" + Module->GetName());
      m_TimeButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_TimeButton->SetText(NModule::GetModuleTypeString(NModule::c_TimeEngine) + ":\n" + NoModule);
      m_TimeButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_OrbitEngine)) != 0) {
      m_OrbitButton->SetText(NModule::GetModuleTypeString(NModule::c_OrbitEngine) + ":\n" + Module->GetName());
      m_OrbitButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_OrbitButton->SetText(NModule::GetModuleTypeString(NModule::c_OrbitEngine) + ":\n" + NoModule);
      m_OrbitButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_OrientationsEngine)) != 0) {
      m_OrientationsButton->SetText(NModule::GetModuleTypeString(NModule::c_OrientationsEngine) + ":\n" + Module->GetName());
      m_OrientationsButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_OrientationsButton->SetText(NModule::GetModuleTypeString(NModule::c_OrientationsEngine) + ":\n" + NoModule);
      m_OrientationsButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_PointingEngine)) != 0) {
      m_PointingButton->SetText(NModule::GetModuleTypeString(NModule::c_PointingEngine) + ":\n" + Module->GetName());
      m_PointingButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_PointingButton->SetText(NModule::GetModuleTypeString(NModule::c_PointingEngine) + ":\n" + NoModule);
      m_PointingButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
    if ((Module = m_Supervisor->GetActiveModule(NModule::c_GeometryAndDetectorProperties)) != 0) {
      m_GeometryAndDetectorPropertiesButton->SetText(NModule::GetModuleTypeString(NModule::c_GeometryAndDetectorProperties) + ":\n" + Module->GetName());
      m_GeometryAndDetectorPropertiesButton->SetToolTipText(Module->GetToolTip(), ToolTipDelay);
    } else {
      m_GeometryAndDetectorPropertiesButton->SetText(NModule::GetModuleTypeString(NModule::c_GeometryAndDetectorProperties) + ":\n" + NoModule);
      m_GeometryAndDetectorPropertiesButton->SetToolTipText(NoModuleToolTip, ToolTipDelay);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::HandleKey(Event_t* Event)
{
  // Here we handle all keys...

  char   tmp[10];
  unsigned int keysym;

  // Test if we have a key release:
  if (Event->fType != kKeyRelease) return false;

  // First we get the key...
  gVirtualX->LookupString(Event, tmp, sizeof(tmp), keysym);
  
  // ... and than we do what we need to do...
  
  // The following keys need an initialized hardware
  switch ((EKeySym) keysym) {
  case kKey_Escape:
    OnExit();
    break;
  case kKey_Return:
	case kKey_Enter:
    OnStart();
    break;
	case kKey_l:
	case kKey_L:
    OnLoadConfiguration();
    break;
	case kKey_s:
	case kKey_S:
    OnSaveConfiguration();
    break;
  default:
    break;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::ProcessMessage(long Message, long Parameter1, 
                              long Parameter2)
{
  // Process the messages for this application

  bool Status = true;

  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_MENU:
      if (Parameter1 == c_LoadConfig) {
        Status = OnLoadConfiguration();
      } else if (Parameter1 == c_SaveConfig) {
        Status = OnSaveConfiguration();
      } else if (Parameter1 == c_Exit) {
        Status = OnExit();
      } else if (Parameter1 == c_About) {
        Status = OnAbout();
      }
      break;
    case kCM_BUTTON:
      if (Parameter1 == c_Exit) {
        Status = OnExit();
      } else if (Parameter1 == c_Start) {
        Status = OnStart();
      } else if (Parameter1 == c_Toggle) {
        Status = OnToggle();
      } else if (Parameter1 == c_Supervisor) {
        Status = OnSupervisor();
      } else if (Parameter1 == c_Switch) {
        Status = OnSwitch();
      } else if (Parameter1 == NModule::c_SourceGenerator ||
                 Parameter1 == NModule::c_OpticsEngine ||
                 Parameter1 == NModule::c_ApertureEngine ||
                 Parameter1 == NModule::c_DetectorEngine ||
                 Parameter1 == NModule::c_BackgroundEngine ||
                 Parameter1 == NModule::c_DetectorEffectsEngine ||
                 Parameter1 == NModule::c_TriggerEngine ||
                 Parameter1 == NModule::c_MetrologyEngine ||
                 Parameter1 == NModule::c_StarTrackerEngine ||
                 Parameter1 == NModule::c_DetectorCalibrator ||
                 Parameter1 == NModule::c_StarTrackerCalibrator ||
                 Parameter1 == NModule::c_MetrologyCalibrator ||
                 Parameter1 == NModule::c_ObservatoryReconstructor ||
                 //Parameter1 == NModule::c_ObservatoryMerger ||
                 Parameter1 == NModule::c_EventSelector ||
                 Parameter1 == NModule::c_ScienceAnalyzer ||
                 Parameter1 == NModule::c_TimeEngine ||
                 Parameter1 == NModule::c_OrbitEngine ||
                 Parameter1 == NModule::c_OrientationsEngine ||
                 Parameter1 == NModule::c_PointingEngine ||
                 Parameter1 == NModule::c_GeometryAndDetectorProperties) {
        OnOptionsPipeline(Parameter1);
      }
      break;
    case kMDI_SIZE: // NTextButton right-click hack...
      if (Parameter1 == NModule::c_SourceGenerator ||
          Parameter1 == NModule::c_OpticsEngine ||
          Parameter1 == NModule::c_ApertureEngine ||
          Parameter1 == NModule::c_DetectorEngine ||
          Parameter1 == NModule::c_BackgroundEngine ||
          Parameter1 == NModule::c_DetectorEffectsEngine ||
          Parameter1 == NModule::c_TriggerEngine ||
          Parameter1 == NModule::c_MetrologyEngine ||
          Parameter1 == NModule::c_StarTrackerEngine ||
          Parameter1 == NModule::c_DetectorCalibrator ||
          Parameter1 == NModule::c_StarTrackerCalibrator ||
          Parameter1 == NModule::c_MetrologyCalibrator ||
          Parameter1 == NModule::c_ObservatoryReconstructor ||
          //Parameter1 == NModule::c_ObservatoryMerger ||
          Parameter1 == NModule::c_EventSelector ||
          Parameter1 == NModule::c_ScienceAnalyzer ||
          Parameter1 == NModule::c_TimeEngine ||
          Parameter1 == NModule::c_OrbitEngine ||
          Parameter1 == NModule::c_OrientationsEngine ||
          Parameter1 == NModule::c_PointingEngine ||
          Parameter1 == NModule::c_GeometryAndDetectorProperties) {
        OnChangePipeline(Parameter1);
      }
      break;
    default:
      break;
    }
  default:
    break;
  }
  
  return Status;
}


////////////////////////////////////////////////////////////////////////////////


void NGUIMain::CloseWindow()
{
  // Call exit for controlled good-bye

  OnExit();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnSwitch()
{
  // Switch between satellite and pipeline display

  if (m_IsPipeline == true) {
    m_PipelineFrame->UnmapWindow();
    m_MasterFrame->RemoveAll();
    
    // Deep clean-up should take care of all sub-frames...
    delete m_PipelineFrame;

    CreateSatelliteFrame(m_MasterFrame);

    m_SwitchButton->SetText("Switch to data pipeline");

    m_IsPipeline = false;
  } else {
    m_SatelliteFrame->UnmapWindow();
    m_MasterFrame->RemoveAll();

    // Deep clean-up should take care of all sub-frames...
    delete m_SatelliteFrame;

    CreatePipelineFrame(m_MasterFrame);

    m_SwitchButton->SetText("Switch to satellite options");

    m_IsPipeline = true;
  }

  UpdateButtons();

  MapSubwindows();
  MapWindow();  
  Layout();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnChangePipeline(unsigned int ModuleID)
{
  // Switch Pipeline/Satellite module selector

  vector<NModule*> PossibleModules = m_Supervisor->GetModuleList(ModuleID);
  NGUIModuleSelector* S = new NGUIModuleSelector(PossibleModules, m_Supervisor->GetActiveModule(ModuleID));
  gClient->WaitForUnmap(S);

  if (S->OKPressed() == true) {
    if (S->GetSelectedModule() != 0) {
      m_Supervisor->SetActiveModule(S->GetSelectedModule(), ModuleID);
    } else {
      m_Supervisor->RemoveActiveModule(ModuleID);
    }
    UpdateButtons();
  }

  S->CloseWindow();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnOptionsPipeline(unsigned int ModuleID)
{
  NModule* M = 0;
  if ((M = m_Supervisor->GetActiveModule(ModuleID)) != 0) {
    M->ShowOptionsGUI();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnExit()
{
  OnApply();

  m_Interface->Exit();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnToggle()
{
  m_Interface->ToggleDiagnosticsWindow();
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnSupervisor()
{
  NGUISupervisor* S = new NGUISupervisor(m_Supervisor);
  gClient->WaitForUnmap(S);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnStart()
{
  // 
  if (m_Analyzing == true) {
    m_Interface->Interrupt();
    return true;
  }

  if (OnApply() == false) return false;

  m_Interface->Save();
  m_Analyzing = true;
  m_StartButton->SetText("Stop Analysis");
  m_Interface->Analyze();
  m_Analyzing = false;
  m_StartButton->SetText("Start Analysis");

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnApply()
{
//   if (MFile::Exists(m_FileSelectorLoad->GetFileName()) == false) {
//     mgui<<"The data file \""<<m_FileSelectorLoad->GetFileName()<<"\" does not exist."<<show;
//     return false;
//   }
//   if (MFile::Exists(m_FileSelectorGeometry->GetFileName()) == false) {
//     mgui<<"The geometry file \""<<m_FileSelectorGeometry->GetFileName()<<"\" does not exist."<<show;
//     return false;
//   }

//   m_Supervisor->SetLoadFileName(m_FileSelectorLoad->GetFileName());
//   m_Supervisor->SetSaveFileName(m_FileSelectorSave->GetFileName());
//   m_Supervisor->SetGeometryFileName(m_FileSelectorGeometry->GetFileName());

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnLoadConfiguration()
{
  const char** Types = new const char*[4];
  Types[0] = "Configuration file";
  Types[1] = "*.cfg";
  Types[2] = 0;
  Types[3] = 0;


  TGFileInfo Info;
  Info.fFileTypes = (const char **) Types;
  new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &Info);
  
  // Get the filename ...
  if ((char *) Info.fFilename != 0) {
    m_Supervisor->Load(TString(Info.fFilename));

    UpdateButtons();
  } 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnSaveConfiguration()
{
  // Save a configuration file...

  if (OnApply() == false) return false;

  const char** Types = new const char*[4];
  Types[0] = "Configuration file";
  Types[1] = "*.cfg";
  Types[2] = 0;
  Types[3] = 0;
  

  TGFileInfo Info;
  Info.fFileTypes = (const char **) Types;
  //Info.fIniDir = StrDup(gSystem->DirName(m_Supervisor->GetCurrentFile()));
  new TGFileDialog(gClient->GetRoot(), this, kFDSave, &Info);
  
  // Get the filename ...
  if ((char *) Info.fFilename != 0) {
    m_Supervisor->Save(TString(Info.fFilename));
  } 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIMain::OnAbout()
{
  // Launch the about dialog

  NGUIAbout* About = new NGUIAbout(gClient->GetRoot(), this);
  About->Create();

  return true;
}


// NGUIMain: the end...
////////////////////////////////////////////////////////////////////////////////
