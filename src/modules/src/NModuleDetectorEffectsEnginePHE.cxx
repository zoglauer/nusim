/*
 * NModuleDetectorEffectsEnginePHE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorEffectsEnginePHE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorEffectsEnginePHE.h"

// Standard libs:
#include <cmath>
using namespace std;

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"

// HEAdas
#include "fitsio.h"

// NuSTAR libs:
#include "NGUIOptionsDetectorEffectsEnginePHE.h"
#include "NGUIDiagnosticsDetectorEffectsEngine.h"



////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorEffectsEnginePHE)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEnginePHE::NModuleDetectorEffectsEnginePHE(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorEffectsEnginePHE

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "PHE fits file based";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorEffectsEnginePHE";

  // Set the tool tip text
  m_ToolTip = "This is a detector effects engine utilizing PHE fits files.";

  // Set the module type
  m_ModuleType = c_DetectorEffectsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = true;
  m_Diagnostics = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEnginePHE::~NModuleDetectorEffectsEnginePHE()
{
  // Delete this instance of NModuleDetectorEffectsEnginePHE
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEnginePHE::Initialize()
{
  // Initialize the module 

  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsDetectorEffectsEngine();
  dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(), 
                                                                                            m_Satellite.GetDetectorHalfDimension(), 
                                                                                            m_Satellite.GetDetectorPixelsX(),
                                                                                            m_Satellite.GetDetectorPixelsY());
  m_PHEs.clear();                                                                                          

  MFile::ExpandFileName(m_PHEFileName);
  if (MFile::Exists(m_PHEFileName) == false) {
    mgui<<"Unable to find the file: "<<endl;
    mgui<<m_PHEFileName<<show;
    return false;
  }
  
  // Load all fits files
  
  int status = 0;
  char value[1000];
  char templt[100];
  
  fitsfile* PheFile = 0; 
    
  if (fits_open_file(&PheFile, m_PHEFileName, READONLY, &status)) {
    mgui<<"Unable to open file: "<<endl;
    mgui<<m_PHEFileName<<show;
    return false;
  }

  // Initialize 
  long NRows = 0;

  // Switch to the right extension:
  // Remark: The extension "2" has no name so we cannot find it by name
  fits_movabs_hdu(PheFile, 2, NULL, &status);

  // Save the columns by name as their order might change
  map<TString, int> ColNames;
  int NCols = 0;
  fits_get_num_cols(PheFile, &NCols, &status);
  for (int c = 1; c <= NCols; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(PheFile, CASEINSEN, templt, value, &col, &status);
    ColNames[value] = c;
    cout<<"Column: "<<ColNames[value]<<": "<<value<<endl;
  }
 
  fits_get_num_rows(PheFile, &NRows, &status);
  cout<<"N Rows: "<<NRows<<endl;

  // It seems to start at row 1 and not at row 0...
  
  int anynul;
 
  double nulldbl = 0.0;
  double valdbl = 0;
    
  int nullint = 0;
  int valint = 0;

  int nullintarr = 0;
  int valintarr[9];
  
  bool ReadingError = false;
  for (long Rows = 1; Rows <= NRows; ++Rows) {

    if (Rows%10000 == 0) cout<<"Read "<<Rows<<" rows of fits file"<<endl;
    
    NPHEFitsData Data;
    
    if (fits_read_col_dbl(PheFile, ColNames["RESETTIME"], Rows, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cerr<<"Column read failed: FRAME at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetResetTime(valdbl);

    if (fits_read_col_int(PheFile, ColNames["FRAME"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: FRAME at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetFrame(valint);

    if (fits_read_col_int(PheFile, ColNames["FLAG"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: FLAG at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetFlag(valint);

    if (fits_read_col_int(PheFile, ColNames["SUBFRAME"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: SUBFRAME at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetSubFrame(valint);

    if (fits_read_col_int(PheFile, ColNames["RAWX"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: RAWX at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetRawX(valint);

    if (fits_read_col_int(PheFile, ColNames["RAWY"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: RAWY at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetRawY(valint);

    if (fits_read_col_int(PheFile, ColNames["STARTCAP"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: STARTCAP at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetStartCap(valint);

    if (fits_read_col_int(PheFile, ColNames["CZTID"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: CZTID at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetCZTID(valint);
    
    if (fits_read_col_dbl(PheFile, ColNames["RISETIME"], Rows, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cerr<<"Column read failed: RISETIME at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetRiseTime(valdbl);

    if (fits_read_col_int(PheFile, ColNames["ELAPSET"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: ELAPSET at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetElapsedTime(valint);

    if (fits_read_col_int(PheFile, ColNames["NHTRIG"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: NHTRIG at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetNHardwareTriggers(valint);

 
    if (fits_read_col_int(PheFile, ColNames["PREVAL"], Rows, 1, 9, nullintarr, &valintarr[0], &anynul, &status)) {
      cerr<<"Column read failed: PREVAL at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    for (unsigned int i = 0; i < 9; ++i) {
      Data.SetPreTriggerSampleSum(i+1, valintarr[i]);
    }
    
    if (fits_read_col_int(PheFile, ColNames["POSTVAL"], Rows, 1, 9, nullintarr, &valintarr[0], &anynul, &status)) {
      cerr<<"Column read failed: POSTVAL at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    for (unsigned int i = 0; i < 9; ++i) {
      Data.SetPostTriggerSampleSum(i+1, valintarr[i]);
    }
    
    if (fits_read_col_int(PheFile, ColNames["TRIGGERMAP"], Rows, 1, 9, nullintarr, &valintarr[0], &anynul, &status)) {
      cerr<<"Column read failed: TRIGGERMAP at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    for (unsigned int i = 0; i < 9; ++i) {
      Data.SetTriggerMap(i+1, valintarr[i]);
    }
    
    if (fits_read_col_int(PheFile, ColNames["NSTRIG"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: NSTRIG at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetNSoftwareTriggers(valint);
    
    if (fits_read_col_int(PheFile, ColNames["PHA"], Rows, 1, 1, nullint, &valint, &anynul, &status)) {
      cerr<<"Column read failed: PHA at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetPHA(valint);
    
    if (fits_read_col_dbl(PheFile, ColNames["ENERGY"], Rows, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cerr<<"Column read failed: ENERGY at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetEnergy(valdbl);
    
    if (fits_read_col_dbl(PheFile, ColNames["CZTTEMP"], Rows, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cerr<<"Column read failed: CZTTEMP at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetCZTTemperature(valdbl);
    
    if (fits_read_col_dbl(PheFile, ColNames["EVTIME"], Rows, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cerr<<"Column read failed: EVTIME at row "<<Rows<<endl;
      ReadingError = true;
      break;
    }
    Data.SetEventTime(valdbl);
     
    m_PHEs.push_back(Data);
    
    
    if (m_PHEs.size() >= 250000) {
      mout<<"For testing having 250000 hits should be more than enough..."<<endl;
      break;
    }
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEnginePHE::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // That's a dummy only!!

  // (1) Grab the last photon not the interactions and switch to the focal bench coordinate system:
  NPhoton Photon = Event.GetCurrentPhoton();
  NOrientation OrientationFPM = m_Satellite.GetOrientationFocalPlaneModule(Event.GetTime(), Event.GetTelescope());
  OrientationFPM.TransformIn(Photon);
  
  // Moved a bit into the detector...
  Photon.SetPosition(Photon.GetPosition() + MVector(0.0, 0.0, -0.000001));
  
  // (2) Determine the detector or end the processing if outside
  NOrientation DetectorOrientation;
  int Detector = 0;
  for (unsigned int d = 1; d <= 4; ++d) {
    DetectorOrientation = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetTelescope(), d);
    DetectorOrientation.TransformIn(Photon);
    if (fabs(Photon.GetPosition().X()) <= m_Satellite.GetDetectorHalfDimension().X() &&
        fabs(Photon.GetPosition().Y()) <= m_Satellite.GetDetectorHalfDimension().Y() &&
        fabs(Photon.GetPosition().Z()) <= m_Satellite.GetDetectorHalfDimension().Z()) {
      Detector = d;
      break;
    } else {
      DetectorOrientation.TransformOut(Photon);
    }
  }

  if (Detector == 0) {
    // We did not hit the detector and can stop here
    //cout<<"Did not hit detector..."<<endl;
    Event.SetBlocked(true);
    return true;
  }


  // (3) Determine the x & y pixel location within the detector:
  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();

  int xPixel = int((Photon.GetPosition().X() + m_Satellite.GetDetectorHalfDimension().X()) / (2*m_Satellite.GetDetectorHalfDimension().X() / NPixelsX));
  int yPixel = int((Photon.GetPosition().Y() + m_Satellite.GetDetectorHalfDimension().Y()) / (2*m_Satellite.GetDetectorHalfDimension().Y() / NPixelsY));

  massert(xPixel >= 0 && xPixel < NPixelsX);
  massert(yPixel >= 0 && yPixel < NPixelsY);
  

  // (4) Grab a random PHE
  NPHEFitsData PHE = m_PHEs[gRandom->Integer(m_PHEs.size())];
  
  // (5) Set the detector and the x and y pixel in the the PHEs
  mout<<"ToDo: Set all new PHE data based on the events"<<endl;
   
  // (6) Store the PHE in the event
  Event.SetPHEData(PHE);
  // Missing: Store all other data, so that it the event can be used beyond the PHE pipeline... 
  
  // (7) Update the GUI
  
  // Before:
  if (Event.GetTelescope() == 1) {
    NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetTelescope(), Detector);
    MVector Pos = Photon.GetPosition();
    O.TransformOut(Pos);
    dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddBefore(Pos, Photon.GetEnergy());
  }
  
  // After:
  // TBD we should use the PHE data here


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorEffectsEnginePHE::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsDetectorEffectsEnginePHE* Options = new NGUIOptionsDetectorEffectsEnginePHE(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEnginePHE::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* PHEFileNameNode = Node->GetNode("PHEFileName");
  if (PHEFileNameNode != 0) {
    m_PHEFileName = PHEFileNameNode->GetValue();
  }


  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleDetectorEffectsEnginePHE::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "PHEFileName", m_PHEFileName);

  return Node;
}


// NModuleDetectorEffectsEnginePHE.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
