/*
 * NFilteredEvents.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NFilteredEvents
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NFilteredEvents.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NFilteredEvents)
#endif


////////////////////////////////////////////////////////////////////////////////


NFilteredEvents::NFilteredEvents() : m_CleanLevel(0), m_LiveTime(1), m_DetectorArea(1)
{
  // Construct an instance of NFilteredEvents
}


////////////////////////////////////////////////////////////////////////////////


NFilteredEvents::~NFilteredEvents()
{
  // Delete this instance of NFilteredEvents
}


////////////////////////////////////////////////////////////////////////////////


void NFilteredEvents::Clean()
{
  NBase::Clean();

  m_CleanLevel = 0;
  
  m_LiveTime = 0;
  
  m_StartTime = 0;
  m_ObservationDate = "2000-01-01T12:00:00";
  m_Object = "ERROR: UNKNOWN";  
  
  m_DetectorArea = 0;

  m_PI.clear();
  m_Energy.clear();
  m_SurrEnergy.clear();
  m_Grade.clear();
  m_DetectorID.clear();
  m_RawX.clear();
  m_RawY.clear();
  m_Det1X.clear();
  m_Det1Y.clear();
  
  m_GTIStart.clear();
  m_GTIStop.clear();
  m_GoodGTI.clear();
}


////////////////////////////////////////////////////////////////////////////////


bool NFilteredEvents::Read(const TString& FileName)
{
  cout<<"Reading filtered events from "<<FileName<<endl;
 
  if (MFile::Exists(FileName) == false) {
    cout<<"The file \""<<FileName<<"\" does not exists!"<<endl;
    return false;
  }

  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];

  int anynul;
  
  double nulldbl = 0.0;
  double valdbl = 0;
  
  int nullint = 0;
  int valint = 0;
  
  fitsfile* File = 0;
  if (fits_open_file(&File, FileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open filtered events file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<FileName<<endl;

  // Move to the second hdu
  fits_movabs_hdu(File, 2, NULL, &status);

  fits_read_key(File, TDOUBLE, "LiveTime", &valdbl, value, &status);
  m_LiveTime = valdbl;

  fits_read_key(File, TDOUBLE, "TSTART", &valdbl, value, &status);
  m_StartTime = valdbl;
  
  fits_read_key(File, TSTRING, "DATE-OBS", value, NULL, &status);
  m_ObservationDate = value;
  
  fits_read_key(File, TSTRING, "OBJECT", value, NULL, &status);
  m_Object = value;
      

  // Get the number of columns
  int NColumns = 0;
  map<TString, int> Columns;
  fits_get_num_cols(File, &NColumns, &status);
  for (int c = 1; c <= NColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(File, CASEINSEN, templt, value, &col, &status);
    Columns[value] = c;
    //cout<<"Columns: "<<value<<endl;
  }
 
  //cout<<"N columns orbit: "<<NColumns<<endl;

  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(File, &NRows, &status);
  //cout<<"Rows: "<<NRows<<endl;

  // Loop over the events and all columns and generate MComptonEvent's
  for (long r = 1; r <= NRows; ++r) {
    if (fits_read_col_dbl(File, Columns["TIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (time) failed!"<<endl;
      break;
    }
    double Time = valdbl;

    if (fits_read_col_int(File, Columns["PI"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (PI) failed!"<<endl;
      break;
    }
    int PI = valint;
    double Energy = 0.04*double(valint)+1.6; //valdbl/10.0 + 3.0;
    

    if (fits_read_col_int(File, Columns["SURRPI"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SURRPI) failed!"<<endl;
      break;
    }
    //int SurrPI = valint;
    double SurrEnergy = 0.04*double(valint)+1.6;

    if (fits_read_col_int(File, Columns["GRADE"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (GRADE) failed!"<<endl;
      break;
    }
    int Grade = valint;
    
    if (fits_read_col_int(File, Columns["DET_ID"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (DET_ID) failed!"<<endl;
      break;
    }
    int DetectorID = valint;
    
    if (fits_read_col_int(File, Columns["RAWX"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (RAWX) failed!"<<endl;
      break;
    }
    int RawX = valint;
    
    if (fits_read_col_int(File, Columns["RAWY"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (RAWY) failed!"<<endl;
      break;
    }
    int RawY = valint;
    
    if (fits_read_col_int(File, Columns["DET1X"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (DET1X) failed!"<<endl;
      break;
    }
    int Det1X = valint;
    
    if (fits_read_col_int(File, Columns["DET1Y"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (DET1Y) failed!"<<endl;
      break;
    }
    int Det1Y = valint;
    
    Add(Time, PI, Energy, SurrEnergy, Grade, DetectorID, RawX, RawY, Det1X, Det1Y);
  } 

  // Move to the thris hdu - GTI
  fits_movabs_hdu(File, 3, NULL, &status);

  // Get the number of columns
  int NGTIColumns = 0;
  map<TString, int> GTIColumns;
  fits_get_num_cols(File, &NGTIColumns, &status);
  for (int c = 1; c <= NGTIColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(File, CASEINSEN, templt, value, &col, &status);
    GTIColumns[value] = c;
    //cout<<"Columns: "<<value<<endl;
  }
 
  //cout<<"N GTI column: "<<NGTIColumns<<endl;

  // Get the number of rows:
  long NGTIRows = 0;
  fits_get_num_rows(File, &NGTIRows, &status);
  //cout<<"Rows: "<<NGTIRows<<endl;

  // Loop over the events and all columns
  for (long r = 1; r <= NGTIRows; ++r) {
    if (fits_read_col_dbl(File, GTIColumns["START"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (START) failed!"<<endl;
      break;
    }
    double Start = valdbl;

    if (fits_read_col_dbl(File, GTIColumns["STOP"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (STOP) failed!"<<endl;
      break;
    }
    double Stop = valdbl;
    
    
    bool Good = false;
    int i = FindFirstIndexLarger(Start);
    if (i > 0 && m_Time[i] <= Stop) Good = true;
    //cout<<"GTI: "<<Start<<"-"<<Stop<<": Good? "<<((Good == true) ? "Yes" : "NO")<<endl;
    AddGTI(Start, Stop, Good);
  }
  
  
  m_DetectorArea = 0.06048*0.06048*32*32*4;
  
  if (FileName.EndsWith("01_cl.evt") == true) {
    m_CleanLevel = 1;
  } else if  (FileName.Contains("02_cl.evt") == true) {
    m_CleanLevel = 2;
  }
  
  return true;
}


// NFilteredEvents.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
