/*
 * NGUIOptionsPointingPredefined.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsPointingPredefined.h"

// Standard libs:

// ROOT libs:
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TFormula.h>
#include <TGResourcePool.h>
#include <TGCanvas.h>
#include <TGFileDialog.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MGUIEEntryList.h"

// NuSTAR libs:
#include "NModule.h"
#include "NModulePointingPredefined.h"
#include "NGUIOptionsSinglePointing.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsPointingPredefined)
#endif


////////////////////////////////////////////////////////////////////////////////


const int NGUIOptionsPointingPredefined::c_Import   = 850;
const int NGUIOptionsPointingPredefined::c_Absolute = 851;

  
////////////////////////////////////////////////////////////////////////////////


NGUIOptionsPointingPredefined::NGUIOptionsPointingPredefined(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor

  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsPointingPredefined::~NGUIOptionsPointingPredefined()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsPointingPredefined::Create()
{
  PreCreate();
  
  TGLayoutHints* BasicLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 20, 20, 10, 0);

  
  TGLabel* PointingLabel = new TGLabel(this, "Attention: On some systems a ROOT/X11 bug causes crashes after entering text below. In those cases, please edit the configuration file by hand!");
  AddFrame(PointingLabel, BasicLayout);

  m_AbsoluteTime = 
    new TGCheckButton(this, "Pointing times are absolute. Otherwise they are relative to the observation time set in the supervisor (they do not need to add up to anything).", c_Absolute);
  m_AbsoluteTime->Associate(this);
  if (dynamic_cast<NModulePointingPredefined*>(m_Module)->GetAbsoluteTime() == true) {
    m_AbsoluteTime->SetState(kButtonDown);
  } else {
    m_AbsoluteTime->SetState(kButtonUp);    
  }
  AddFrame(m_AbsoluteTime, BasicLayout);

 
  m_PointingViewer = new TGCanvas(this, 840, 160);
  m_PointingViewer->SetContainer(new TGCompositeFrame(m_PointingViewer->GetViewPort(), 50, 50));
  AddFrame(m_PointingViewer, BasicLayout);
  
  UpdateViewer();
 
  
  TGTextButton* ImportButton = new TGTextButton(this, "Import pointings from file (and replace the old ones)", c_Import);
  ImportButton->Associate(this);
  AddFrame(ImportButton, BasicLayout);
  
  
  TGLayoutHints* DBLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 20, 20, 20, 20);
  m_PointingJitterDB = new MGUIEFileSelector(this, "Pointing jitter database (leave empty for ideal pointings):", 
                                           dynamic_cast<NModulePointingPredefined*>(m_Module)->GetPointingJitterDBFileName());
  m_PointingJitterDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_PointingJitterDB, DBLayout);
  
  
  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsPointingPredefined::UpdateViewer()
{
  //if (m_PointingViewer == 0) return;
  
  TGCompositeFrame* Container = dynamic_cast<TGCompositeFrame*>(m_PointingViewer->GetContainer());
  
  
  Container->UnmapWindow();
  for (unsigned int p = 0; p < m_SinglePointings.size(); ++p) {
    Container->HideFrame(m_SinglePointings[p]);
  }
  Container->RemoveAll();
  
  
  m_SinglePointings.clear();

  vector<NPointing>& Pointings = dynamic_cast<NModulePointingPredefined*>(m_Module)->GetInitialPointingsByRef();
  
  TGLayoutHints* PointingLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2);
  for (unsigned int p = 0; p < Pointings.size(); ++p) {

    NGUIOptionsSinglePointing* SP = new NGUIOptionsSinglePointing(Container, &Pointings[p], p+1);
    SP->Create();
    SP->Associate(this);
    m_SinglePointings.push_back(SP);
    Container->AddFrame(SP, PointingLayout);
  }
  
  MapSubwindows();
  MapWindow();  
  Layout();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsPointingPredefined::ProcessMessage(long Message, long Parameter1, long Parameter2)
{
  // Modify here if you have more buttons

  vector<NPointing>& Pointings = dynamic_cast<NModulePointingPredefined*>(m_Module)->GetInitialPointingsByRef();

	bool Status = true;
	
  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_BUTTON:
      switch (Parameter1) {
        case c_Import:
          OnImport();
          break;
        case 995: // == NGUIOptionsSinglePointing::c_Delete:
          OnApply();
          if (Parameter2 >= 1 && Parameter2 <= long(Pointings.size())) {
            if (Pointings.size() > 1) { 
              Pointings.erase(Pointings.begin()+Parameter2-1);
            } else {
              mgui<<"You cannot delete the last pointing!"<<info;
              break;
            }
          }
          UpdateViewer();
          break;
        case 996: // == NGUIOptionsSinglePointing::c_AddAfter:
          OnApply();
          if (Parameter2 >= 1 && Parameter2 <= long(Pointings.size())) {
            NPointing N;
            Pointings.insert(Pointings.begin()+Parameter2, N);
          }
          UpdateViewer();
          break;
        case 997: // == NGUIOptionsSinglePointing::c_Up:
          OnApply();
          if (Parameter2 >= 2 && Parameter2 <= long(Pointings.size())) {
            NPointing N = Pointings[Parameter2-1];
            Pointings.erase(Pointings.begin()+Parameter2-1);
            Pointings.insert(Pointings.begin()+Parameter2-2, N);
          } else if (Parameter2 == 1) {
            mgui<<"You cannot move the top entry farther up!"<<info;
            break;
          }
          UpdateViewer();
          break;
        case 998: // == NGUIOptionsSinglePointing::c_Down:
          OnApply();
          if (Parameter2 >= 1 && Parameter2 <= long(Pointings.size())-1) {
            NPointing N = Pointings[Parameter2-1];
            Pointings.erase(Pointings.begin()+Parameter2-1);
            Pointings.insert(Pointings.begin()+Parameter2, N);
          } else if (Parameter2 == long(Pointings.size())) {
            mgui<<"You cannot move the bottom entry farther down!"<<info;
            break;
          }
          UpdateViewer();
          break;
        default:
          break;
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


bool NGUIOptionsPointingPredefined::OnImport()
{
  // Import a pointing list
  
  OnApply();
  
  // Load a file
  const char** Types = new const char*[4];
  Types[0] = "All files";
  Types[1] = "*";
  Types[2] = 0;
  Types[3] = 0;
  
  TGFileInfo Info;
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
  
  dynamic_cast<NModulePointingPredefined*>(m_Module)->ImportPointings(FileName);
  UpdateViewer();

  return true;
}

          
////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsPointingPredefined::OnApply()
{
	// Modify this to store the data in the module!
  
  for (unsigned int p = 0; p < m_SinglePointings.size(); ++p) {
    m_SinglePointings[p]->Update();
  }
  
  dynamic_cast<NModulePointingPredefined*>(m_Module)->SetPointingJitterDBFileName(m_PointingJitterDB->GetFileName());
  if (m_AbsoluteTime->GetState() == kButtonDown) {
    dynamic_cast<NModulePointingPredefined*>(m_Module)->SetAbsoluteTime(true);
  } else {
    dynamic_cast<NModulePointingPredefined*>(m_Module)->SetAbsoluteTime(false);    
  }
  
	return true;
}


// NGUIOptionsPointingPredefined: the end...
////////////////////////////////////////////////////////////////////////////////
