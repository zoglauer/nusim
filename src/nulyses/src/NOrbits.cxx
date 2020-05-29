/*
 * NOrbits.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NOrbits
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NOrbits.h"

// Standard libs:
#include <limits>
#include <iomanip>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NOrbits)
#endif


////////////////////////////////////////////////////////////////////////////////


NOrbits::NOrbits()
{
  // Construct an instance of NOrbits
}


////////////////////////////////////////////////////////////////////////////////


NOrbits::~NOrbits()
{
  // Delete this instance of NOrbits
}


////////////////////////////////////////////////////////////////////////////////


void NOrbits::Clean()
{
  NBase::Clean();

  m_TimeAfterSAA.clear();
  m_OccultedFlag.clear();
  m_SafelyOcculted.clear();
  m_SafelyOnSource.clear();
  m_SAAFlag.clear();
  m_SAAAFlag.clear();
  m_SAABFlag.clear();
  m_SlewFlag.clear();
  m_DayFlag.clear();
  m_GeomagneticCutOff.clear();
  m_Longitude.clear();
  m_Latitude.clear();
  m_Altitude.clear();
  
  m_OccultationSteps.clear();
  m_OccultationValues.clear();
  m_SAASteps.clear();
  m_SAAValues.clear();
  m_DaySteps.clear();
  m_DayValues.clear();
  m_SlewSteps.clear();
  m_SlewValues.clear();
}


////////////////////////////////////////////////////////////////////////////////


double NOrbits::GetMinGeomagneticCutOff() {
  double Min = numeric_limits<double>::max();
  for (unsigned int i = 0; i < m_GeomagneticCutOff.size(); ++i) {
    if (m_GeomagneticCutOff[i] < Min) {
      Min = m_GeomagneticCutOff[i];
    }
  }
  return Min;
}


////////////////////////////////////////////////////////////////////////////////


double NOrbits::GetMaxGeomagneticCutOff() {
  double Max = 0;
  for (unsigned int i = 0; i < m_GeomagneticCutOff.size(); ++i) {
    if (m_GeomagneticCutOff[i] > Max) {
      Max = m_GeomagneticCutOff[i];
    }
  }
  return Max;
}


////////////////////////////////////////////////////////////////////////////////


bool NOrbits::Read(const TString& OrbitsFileName)
{
  TString FileName = OrbitsFileName;
  if (MFile::Exists(FileName) == false) {
    FileName += ".gz";
    if (MFile::Exists(FileName) == false) {
      cout<<"The file \""<<FileName<<"\" does not exists!"<<endl;
      return false;
    }
  }
 
 cout<<"Reading orbit data from file "<<FileName<<endl;
 
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
  
  fitsfile* EventFile = 0;
  if (fits_open_file(&EventFile, FileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open Orbits file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<FileName<<endl;

  // Move to the second hdu
  fits_movabs_hdu(EventFile, 2, NULL, &status);

  // Get the number of columns
  int NColumns = 0;
  map<TString, int> Columns;
  fits_get_num_cols(EventFile, &NColumns, &status);
  for (int c = 1; c <= NColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(EventFile, CASEINSEN, templt, value, &col, &status);
    Columns[value] = c;
    //cout<<value<<endl;
  }
  //cout<<"N columns: "<<NColumns<<endl;

  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(EventFile, &NRows, &status);
  //cout<<"Rows: "<<NRows<<endl;

  int PreviousDay = -1;
  int PreviousSAA = -1;
  int PreviousOccultation = -1;
  int PreviousSlew = -1;
  // Loop over the events and all columns and generate MComptonEvent's
  for (long r = 1; r <= NRows; ++r) {

    if (fits_read_col_dbl(EventFile, Columns["TIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (time) failed!"<<endl;
      break;
    }
    double Time = valdbl;
    
    if (fits_read_col_int(EventFile, Columns["SAA"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SAA) failed!"<<endl;
      break;
    }
    double SAAFlag = valint;
    if (valint != PreviousSAA) {
      m_SAASteps.push_back(Time);
      m_SAAValues.push_back(valint);
      PreviousSAA = valint;
    }
    double TimeAfterSAA = 0;
    if (PreviousSAA == 0) {
      TimeAfterSAA = Time - m_SAASteps.back();
    }
    
    if (fits_read_col_int(EventFile, Columns["SAA_A"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SAA_A) failed!"<<endl;
      break;
    }
    int SAAAFlag = valint;
    
    if (fits_read_col_int(EventFile, Columns["SAA_B"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SAA_B) failed!"<<endl;
      break;
    }
    int SAABFlag = valint;


    if (fits_read_col_int(EventFile, Columns["OCCULTED"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (OCCULTED) failed!"<<endl;
      break;
    }
    double OccultedFlag = valint;
    if (valint != PreviousOccultation) {
      m_OccultationSteps.push_back(Time);
      m_OccultationValues.push_back(valint);
      PreviousOccultation = valint;
    }
    
    
    if (fits_read_col_int(EventFile, Columns["DAY"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (DAY) failed!"<<endl;
      break;
    }
    int DayFlag = valint;
    if (valint != PreviousDay) {
      m_DaySteps.push_back(Time);
      m_DayValues.push_back(valint);
      PreviousDay = valint;
    }
    
    if (fits_read_col_int(EventFile, Columns["SLEW"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SLEW) failed!"<<endl;
      break;
    }
    if (valint != PreviousSlew) {
      m_SlewSteps.push_back(Time);
      m_SlewValues.push_back(valint);
      PreviousSlew = valint;
    }
    int SlewFlag = valint;
    
    if (fits_read_col_dbl(EventFile, Columns["GEOCOR"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (GEOCOR) failed!"<<endl;
      break;
    }
    double GeomagneticCutOff = valdbl;
    
    if (fits_read_col_dbl(EventFile, Columns["GEODETIC"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (GEODETIC) failed!"<<endl;
      break;
    }
    double Latitude = valdbl;
    
    if (fits_read_col_dbl(EventFile, Columns["GEODETIC"], r, 2, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (GEODETIC) failed!"<<endl;
      break;
    }
    double Longitude = valdbl;
    
    if (fits_read_col_dbl(EventFile, Columns["GEODETIC"], r, 3, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (GEODETIC) failed!"<<endl;
      break;
    }
    double Altitude = valdbl;
    
    Add(Time, TimeAfterSAA, SAAFlag, SAAAFlag, SAABFlag, SlewFlag, DayFlag, OccultedFlag, GeomagneticCutOff, Longitude, Latitude, Altitude);
  }
  
  /*
  cout<<"Occs:"<<endl;
  for (unsigned int o = 0; o < m_OccultationSteps.size(); ++o) {
    cout<<setprecision(10)<<m_OccultationSteps[o]<<": "<<m_OccultationValues[o]<<endl;
    if (m_OccultationValues[o] == 0 && o > 0) {
      cout<<"Occultation duration: "<<m_OccultationSteps[o] - m_OccultationSteps[o-1]<<endl; 
    }
  }
  */
  
  // Determine "safe" occultation flags:
  double SafeTime = 900;
  double TimeSinceOn = 0;
  for (unsigned int o = 0; o < m_Time.size(); ++o) {
    if (m_OccultedFlag[o] == 1) {
      TimeSinceOn += 1.0;
      if (TimeSinceOn >= SafeTime) {
        m_SafelyOcculted[o] = 1; 
      }
    } else {
      TimeSinceOn = 0.0; 
    }
  }
  TimeSinceOn = 0.0;
  for (unsigned int o = m_Time.size() - 1; o < m_Time.size(); --o) {
    if (m_OccultedFlag[o] == 1) {
      TimeSinceOn += 1.0;
      if (TimeSinceOn < SafeTime) {
        m_SafelyOcculted[o] = 0; 
      }
    } else {
      TimeSinceOn = 0.0; 
    }
  }
  
  // Sanity checks
  if (m_Time.size() == 0) {
    cout<<"Error: No orbit data found!"<<endl;
    return false;
  }
  
  // Verify we are in 1 sec intervals and there are no gaps in data:
  if ((unsigned long) m_Time.back() - (unsigned long) m_Time[0] + 1 != m_Time.size()) {
    cout<<"Error: The orbit data is either not in 1 sec bins or there are gaps in the orbit data!"<<endl;
    cout<<"Start: "<<setprecision(10)<<m_Time[0]<<" End: "<<setprecision(10)<<m_Time.back()<<"  Diff: "<<(unsigned long) m_Time.back() - (unsigned long) m_Time[0] + 1<<"  Size: "<<m_Time.size()<<endl;
    return false;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int NOrbits::FindOrbitIndex(double Time)
{
  unsigned int Size = m_Time.size();
  
  // Test for Size == 0 upon reading
  
  if (Time <= m_Time[0]) {
    return 0;
  }
  if (Time >= m_Time.back()) {
    return Size - 1; 
  }
  
  // Return the first value which is not lower than time
  vector<double>::iterator I = lower_bound(m_Time.begin(), m_Time.end(), Time);
  
  // The above if statements should prevent this in theory:
  if (I == m_Time.end()) {
    cout<<"Something gastly happend which should be impossible: I did not find the correct time value in the orbit  time array! Returning the first index. Everything you do now might be wrong..."<<endl; 
    return 0;
  }
  
  // It sits on the edge:
  if (Time == (*I)) return (unsigned int) (I - m_Time.begin());
  
  // Other wise we are above due to "not lower than"...
  return (unsigned int) (I - m_Time.begin() - 1);
}



// NOrbits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
