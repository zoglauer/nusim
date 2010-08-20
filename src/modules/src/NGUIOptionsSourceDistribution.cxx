/*
 * NGUIOptionsSourceDistribution.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsSourceDistribution.h"

// Standard libs:
#include <vector>
#include <fstream>
using namespace std;

// ROOT libs:
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TFormula.h>
#include <TGResourcePool.h>
#include <TGFileDialog.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MGUIEEntryList.h"

// NuSTAR libs:
#include "NModule.h"
#include "NModuleSourceDistribution.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsSourceDistribution)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSourceDistribution::NGUIOptionsSourceDistribution(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSourceDistribution::~NGUIOptionsSourceDistribution()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSourceDistribution::Create()
{
  PreCreate();

  TGLayoutHints* StandardLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX | kLHintsExpandY, 20, 20, 10, 10);

  m_Tab = new TGTab(this, 300, 450);
  AddFrame(m_Tab, StandardLayout);

  vector<NSource*>& Sources = dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetSourcesRef();
  for (unsigned int s = 0; s < Sources.size(); ++s) {
    CreateTab(Sources[s]);
  }

	TGHorizontalFrame* ButtonFrame = new TGHorizontalFrame(this, 150, 25);
	TGLayoutHints* ButtonFrameLayout =	new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsCenterX, 20, 20, 5, 0);
	AddFrame(ButtonFrame, ButtonFrameLayout);

  TGTextButton* AddButton = new TGTextButton(ButtonFrame, "Add new source", c_Add); 
  TGLayoutHints* AddButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);
  AddButton->SetMargins(20, 20, 2, 2);
  AddButton->Associate(this);
  ButtonFrame->AddFrame(AddButton, AddButtonLayout);

  TGTextButton* ListButton = new TGTextButton(ButtonFrame, "Add sources from list", c_List); 
  TGLayoutHints* ListButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 10, 10, 0, 0);
  ListButton->SetMargins(20, 20, 2, 2);
  ListButton->Associate(this);
  ButtonFrame->AddFrame(ListButton, ListButtonLayout);

	TGTextButton* RemoveButton = new TGTextButton(ButtonFrame, "Remove current source", c_Remove); 
  TGLayoutHints* RemoveButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 0, 0, 0, 0);
  RemoveButton->SetMargins(20, 20, 2, 2);
  RemoveButton->Associate(this);
  ButtonFrame->AddFrame(RemoveButton, RemoveButtonLayout);

  PostCreate();

  Resize(500, 570);
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSourceDistribution::ProcessMessage(long Message, long Parameter1, long Parameter2)
{
  // Modify here if you have more buttons

	bool Status = true;
	
  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_BUTTON:
      if (Parameter1 == c_Add) {
        Status = OnAdd();
      } else if (Parameter1 == c_List) {
        Status = OnList();
      } else if (Parameter1 == c_Remove) {
        Status = OnRemove();
      } 
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  
  if (Status == false) {
    return false;
  }

  // Call also base class
  return NGUIOptions::ProcessMessage(Message, Parameter1, Parameter2);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSourceDistribution::CreateTab(NSource* Source)
{
  m_TabFrames.push_back(m_Tab->AddTab(Source->GetName()));

  NGUIOptionsSource* Frame = new NGUIOptionsSource(Source, m_TabFrames.back());
  Frame->Create();
  TGLayoutHints* StandardLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX | kLHintsExpandY, 20, 20, 10, 10);
  m_TabFrames.back()->AddFrame(Frame, StandardLayout);

  m_SourceFrames.push_back(Frame);
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSourceDistribution::OnList()
{
  // Modify this to store the data in the module!

  // Load a file
  const char** Types = new const char*[4];
  Types[0] = "All files";
  Types[1] = "*";
  Types[2] = 0;
  Types[3] = 0;
  
  TGFileInfo Info;
  //Info.fFilename = StrDup(gSystem->BaseName(FileName));
  //Info.fIniDir = StrDup(gSystem->DirName(FileName));
  Info.fFileTypes = Types;
  
  new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &Info);

  // Get the filename ...
  TString FileName;
  if ((char *) Info.fFilename != 0) {
    FileName = TString((char *) Info.fFilename);
    cout<<"New Name: "<<FileName<<endl;
    if (FileName.CompareTo("") == 0) {
      return false;
    }
  } 
  // ... or return when cancel has been pressed
  else {
    return false;
  }

  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    mgui<<"Unable to opne file: "<<FileName<<show;
    return false;
  }

  TString Line;
  vector<NSource*>& Sources = dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetSourcesRef();
  while (Line.ReadLine(in)) {   
    NSource* Source = new NSource(m_Module->GetSatelliteRef());
    if (Source->ParseLine(Line) == true) {
      Sources.push_back(Source);
      CreateTab(Sources.back());
      m_Tab->SetTab(Sources.size()-1);
    } else {
      cout<<"Unable to parse line: "<<Line<<endl;
      delete Source; 
    }
  }
  in.close();

  m_Tab->Layout();
  m_Tab->MapSubwindows();
  m_Tab->MapWindow();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSourceDistribution::OnAdd()
{
  // Modify this to store the data in the module!

  vector<NSource*>& Sources = dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetSourcesRef();
  NSource* Source = new NSource(m_Module->GetSatelliteRef());
  
  TString Name("Source #");
  Name += Sources.size()+1;
  Source->SetName(Name);
  Source->SetBeamType(NSource::c_FarFieldPoint);
  Source->SetPosition(0.0, 0.0);
  Source->SetSpectralType(NSource::c_Monoenergetic);
  Source->SetEnergy(30.0);
  Source->SetFlux(1.0);
 
  Sources.push_back(Source);

  CreateTab(Sources.back());

  m_Tab->SetTab(Sources.size()-1);
  m_Tab->Layout();
  m_Tab->MapSubwindows();
  m_Tab->MapWindow();

  return true;
}



////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSourceDistribution::OnRemove()
{
	// Modify this to store the data in the module!

  vector<NSource*>& Sources = dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetSourcesRef();

  int Position = m_Tab->GetCurrent();
  m_Tab->RemoveTab(Position);
  //delete Sources[Position];
  Sources.erase(Sources.begin()+Position);
  m_TabFrames.erase(m_TabFrames.begin()+Position);
  m_SourceFrames.erase(m_SourceFrames.begin()+Position);
  //delete m_SourceFrames[Position];

  m_Tab->Layout();
  m_Tab->MapSubwindows();
  m_Tab->MapWindow();

	return true;
}



////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSourceDistribution::OnApply()
{
	// Modify this to store the data in the module!

  for (unsigned int s = 0; s < m_SourceFrames.size(); ++s) {
    m_SourceFrames[s]->UpdateSource();
  }

	return true;
}


// NGUIOptionsSourceDistribution: the end...
////////////////////////////////////////////////////////////////////////////////
