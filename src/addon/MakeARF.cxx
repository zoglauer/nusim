/*
 * ExposureMap.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// ExposureMap.cxx
//
////////////////////////////////////////////////////////////////////////////////


// Standard:
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <fitsio.h>
#include <cstdlib>
#include <csignal>
#include <map>
#include <algorithm>
#include <time.h>
using namespace std;

// ROOT
#include <TString.h>
#include "TGClient.h"

// MEGAlib
#include "MTokenizer.h"


// NuSIM
#include <NQuaternion.h>
#include <NObservatoryData.h>
#include <NOrientation.h>
#include "NAlignmentsDBEntry.h"

/****************************************************************************/
class PSF
{
public:
  PSF();
  ~PSF();
  bool LoadPSF(int type);
  
  vector <float> cube;
   
};
/************************/
class Target
{
public:
  /// Default constructor
  Target();
  /// Default destructor
  ~Target();
  // Open event fits file
  bool Initialize();
  bool OpenEvtFile();
  bool GenerateARF();
  bool ReadCalibratedAlignmentsDB();
  MVector LoadDSRegion(TString FileName);
  bool LoadEffArea();
  bool LoadVignet();
  bool SetTarget(MVector region);
  bool WriteFitsSpectrum(TString FileName, TString Type, long data[], float exposure);
  bool WriteARF(float* Elow, float* Ehigh, float* data);
  float GetPSFFraction(float region, int oaa);
  int GetDetIndex();
  NAlignmentsDBEntry IdealAlignments;
  NOrientation Rfbob;
  NOrientation Rstar;  
  TString SourceInFile;
  TString DS9File;
  TString SourceOutFile;
  TString BkgFile;
  TString ARFFile;

  
protected:
  fitsfile* m_File; 
  char FindDelimeter(ifstream& in);

  
private:
  
  PSF psfimg;
 
  /// Simulation file name
  TString m_FitsFileName;
  /// Output file name
  TString m_OutputFileName;
  TString DataBase;
  long axis1;
  long axis2;
 
  float m_Ra;
  float m_Dec;
  float m_R;
  
  MVector Vob, Vsky, Vdet;
  float m_oaa;
  
  float xDelta, yDelta;
  float xCenterValue, yCenterValue;
  float AvgDec;
  float oaat_hist1[29];
  float ARF1[820];
  float oaat_hist2[29];
  float ARF2[820];
  float AE[820];
  float Elow[820];
  float Ehigh[820];
  float VElow[820];
  float VEhigh[820];
  float theta[29];
  float vignet[23780];
  
  
};
/****************************************************************************/
/****************************************************************************/
/******************************************************************************
 * Default constructor
 */
PSF::PSF() 
{

}

/******************************************************************************
 * Default destructor
 */
PSF::~PSF()
{
  // Intentionally left blanck
}

/******************************************************************************
 * Default constructor
 */
Target::Target() 
{

 m_File = 0;

}

/******************************************************************************
 * Default destructor
 */
Target::~Target()
{
  // Intentionally left blanck
}

/****************************************************************************/
/****************************************************************************/
bool Target::Initialize()
{

  LoadEffArea();
  LoadVignet();
  psfimg.LoadPSF(16);
  for (int i=0; i<29; i++) {
    oaat_hist1[i] = 0;
	oaat_hist2[i] = 0;
  }

  //cout<<psfimg.oaa0[100*201+100]<<endl;
  
  return 0;

}

/******************************************************************************/
bool PSF::LoadPSF(int noaa)
{

  int Status = 0;
  int AnyNull;
  float FloatNullVal = 0;
  fitsfile* File = 0; 
  
  for (int i=0;i < noaa; ++i) {
  
    char FileName[50]; 
	sprintf(FileName,"resource/data/responsefiles/PSF%d.fits",i);
  
	fits_open_file(&File, FileName, READONLY, &Status);
    if (Status != 0) {
      mgui<<"Unable to open file: "<<endl;
      mgui<<FileName<<show;
      return false;
    }

    // Switch to the right extension:
    int HDUNumber = 1;
    fits_movabs_hdu(File, HDUNumber, NULL, &Status);
    if (Status != 0) {
      mgui<<"HDU number "<<HDUNumber<<" not found in fits file!"<<show;
      return false;
    }

    // Retrieve the dimensions and data type of the image:
    int NDimensions;
    int DataType;
    long AxisDimension[2];  
    fits_get_img_param(File, 2, &DataType, &NDimensions, AxisDimension, &Status);
    if (Status != 0 || NDimensions == 0) {
      mgui<<"The image in the fits file has no dimensions..."<<show;
      return false;
    }

    // Create some dummy arrays
    long StartPixel[2];
    long NPixel = 1;
    for (int dim = 0; dim < NDimensions; ++dim) {
      NPixel *= AxisDimension[dim];
      StartPixel[dim] = 1;
    }

    float* FloatArray = new float[NPixel];
    fits_read_pix(File, TFLOAT, StartPixel, NPixel, &FloatNullVal, FloatArray, &AnyNull, &Status);
    if (Status != 0) {
      mgui<<"Failed to read data from fits file: "<<Status<<show;
      return false;
    }

    for (int x = StartPixel[0]; x <= AxisDimension[0]; ++x) {
      for (int y = StartPixel[1]; y <= AxisDimension[1]; ++y) {
        cube.push_back(FloatArray[(y-StartPixel[1]) + (x-StartPixel[0])*(AxisDimension[1]-StartPixel[1]+1)]);
	  }
    }
    delete [] FloatArray;
  }

  return 0;
	
}
/******************************************************************************/

bool Target::LoadEffArea()
{
//! Load and initialize the file

  // cout<<"OpenFits"<<endl;
  
  int Status = 0;
  float floatnull;
  int anynull;
  m_File = 0;
  
  fits_open_file(&m_File, "resource/data/responsefiles/nustar_effarea_v2.1.fits", READONLY, &Status);
  if (Status != 0) {
    cout<<"Unable to open file: "<<endl;
    //cout<<FileName<<endl;
    return false;
  }
  
  int HDUNumber = 2;
  fits_movabs_hdu(m_File, HDUNumber, NULL, &Status);
  if (Status != 0) {
    cout<<"HDU number "<<HDUNumber<<" not found in fits file!"<<cout;
    return false;
  }

   
  fits_read_col(m_File, TFLOAT, 1, 1, 1, 820, &floatnull, Elow, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 2, 1, 1, 820, &floatnull, Ehigh, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 3, 1, 1, 820, &floatnull, AE, &anynull,&Status);

  fits_close_file(m_File, &Status); 

  //cout<<AE[100]<<endl;

  return 0;
}

/******************************************************************************/

bool Target::LoadVignet()
{
//! Load and initialize the file

  // cout<<"OpenFits"<<endl;
  
  int Status = 0;
  float floatnull;
  int anynull;
  m_File = 0;
  
  fits_open_file(&m_File, "resource/data/responsefiles/nustar_vign_v2.1.fits", READONLY, &Status);
  if (Status != 0) {
    cout<<"Unable to open file: "<<endl;
    //cout<<FileName<<endl;
    return false;
  }
  
  int HDUNumber = 2;
  fits_movabs_hdu(m_File, HDUNumber, NULL, &Status);
  if (Status != 0) {
    cout<<"HDU number "<<HDUNumber<<" not found in fits file!"<<cout;
    return false;
  }

  
  fits_read_col(m_File, TFLOAT, 1, 1, 1, 820, &floatnull, VElow, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 2, 1, 1, 820, &floatnull, VEhigh, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 3, 1, 1, 29, &floatnull, theta, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 4, 1, 1, 23780, &floatnull, vignet, &anynull,&Status);

  fits_close_file(m_File, &Status); 

  //cout<<vignet[821]<<endl;

  return 0;
}
/******************************************************************************/
MVector Target::LoadDSRegion(TString FileName)
{

  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
    return false;
  }

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);

  // Read the header
  
  TString Line;
  Line.ReadToDelim(in, Delimeter); // Region file format
  Line.ReadToDelim(in, Delimeter); // filename of fits file
  Line.ReadToDelim(in, Delimeter); // settings
  Line.ReadToDelim(in, Delimeter); // coordinate type
  
  Line.ReadToDelim(in, Delimeter); // region coordinates

  MTokenizer T;
  // trim string in a silly away
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator('(');
  T.Analyze(Line, false);
  Line = T.GetTokenAtAsString(1);
  T.SetSeparator(')');
  T.Analyze(Line, false);
  Line = T.GetTokenAtAsString(0);
  T.SetSeparator(',');
  T.Analyze(Line, false);
     
  MVector Src;
  Src[0] = T.GetTokenAtAsFloat(0); 
  Src[1] = T.GetTokenAtAsFloat(1); 
  Src[2] = T.GetTokenAtAsFloat(2); 

  //cout<<Src<<endl;

  return Src;
}

/******************************************************************************/

bool Target::OpenEvtFile()
{  
  //! Load and initialize the file

  //cout<<"OpenFits"<<endl;
  
  int Status = 0;
  int nfound;
  m_File = 0;
  
  fits_open_file(&m_File, SourceInFile, READONLY, &Status);
  if (Status != 0) {
    cout<<"Unable to open file: "<<endl;
    cout<<SourceInFile<<endl;
    return false;
  }

  // Switch to the right extension:
  int HDUNumber = 2;
  fits_movabs_hdu(m_File, HDUNumber, NULL, &Status);
  if (Status != 0) {
    cout<<"HDU number "<<HDUNumber<<" not found in fits file!"<<cout;
    return false;
  }
 
  char Dummy[30]; 
  fits_read_key(m_File, TSTRING, "ALIGNDB", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve ALIGNDB keyword..."<<show;
	return false;
  }
  DataBase = Dummy;
  DataBase = "resource/data/AlignmentDatabases/"+DataBase;

  fits_read_keyword(m_File, "TCRVL4", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TCRVL1 keyword..."<<show;
    return false;
  }  
  xCenterValue = atof(Dummy);
  //cout<<"xCenterValue: "<<xCenterValue<<endl;
  
  fits_read_keyword(m_File, "TCDLT4", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve TCDLT1 keyword..."<<show;
	return false;
  }
  xDelta = -fabs(atof(Dummy));
  //cout<<"xDelta: "<<xDelta<<endl;
  
  fits_read_keyword(m_File, "TCRVL5", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TCRVL2 keyword..."<<show;
    return false;
  }  
  yCenterValue = atof(Dummy);
  //cout<<"yCenterValue: "<<yCenterValue<<endl;
  
  fits_read_keyword(m_File, "TCDLT5", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve CDELT2 keyword..."<<show;
	return false;
  }
  yDelta = fabs(atof(Dummy));
  
  /*fits_read_keyword(m_File, "TLMAX1", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TLMAX1 keyword..."<<show;
    return false;
  }  
  float xMax = ceil(atof(Dummy));*/
  
  fits_read_keyword(m_File, "TLMAX5", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve TLMAX5 keyword..."<<show;
	return false;
  }
  float yMax = ceil(atof(Dummy));
    
   // Retrieve the dimensions:
  long AxisDimension[2];  
  fits_read_keys_lng(m_File, "NAXIS", 1, 2, AxisDimension, &nfound, &Status);
  if (Status != 0 || AxisDimension[1] == 0) {
    cout<<"The image in the fits file has no dimensions..."<<endl;
    return false;
  }
  axis1 = AxisDimension[0];
  axis2 = AxisDimension[1];
  
  AvgDec = yCenterValue+yMax*0.5*yDelta;
  
  //cout<<"axis1 ="<<axis1<<" axis2="<<axis2<<endl;

  return true;
}

/****************************************************************************/

bool Target::WriteARF(float* Elow, float* Ehigh, float* data)
{

  TString FileName = "!"+ARFFile; // To overwrite file
  
  int Status = 0;
  m_File = 0;
  
  fits_create_file(&m_File, FileName, &Status);
  if (Status != 0) {
    cout<<"Error in creating file: "<<endl;
    cout<<FileName<<show;
    m_File = 0;
    return false;
  }
  //! create binary table extension
  char ExtensionName[] = "SPECRESP";
  long nrow = 0;
  int tfield = 3;
  char* ttype[] = {"ENERG_LO","ENERG_HI","SPECRESP"};
  char* tform[] = {"1e","1e","1e"};
  char* tunit[] = {"kev","kev","cm2"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<ExtensionName<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  fits_write_key(m_File, TSTRING, "TELESCOP", const_cast<char*>("NuSTAR"), " ", &Status);
  fits_write_key(m_File, TSTRING, "INSTRUME", const_cast<char*>("FPM"), " ", &Status);
  fits_write_key(m_File, TSTRING, "CREATOR",  const_cast<char*>("NuSIM"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "HDUCLASS",  const_cast<char*>("OGIP"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "HDUCLAS1",  const_cast<char*>("RESPONSE"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "HDUCLAS2",  const_cast<char*>("SPECRESP"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "HDUVERS",  const_cast<char*>("1.1.0"), " ", &Status);  

  char Words[30];

  fits_write_col(m_File, TFLOAT, 1, 1, 1, 820, Elow, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('elow') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 2, 1, 1, 820, Ehigh, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('ehigh') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 3, 1, 1, 820, data, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('specresp') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }

  fits_close_file(m_File, &Status);
  
  return 0;

}

/****************************************************************************/

bool Target::WriteFitsSpectrum(TString FileName, TString Type, long data[], float exposure)
{
  
  FileName = "!"+FileName; // To overwrite file
  
  int Status = 0;
  m_File = 0;
  float unity=1.0;
  int detchans = 820;
  int tlmin1 = 0;
  int tlmax1 = 819;
  
  int* group = new int[detchans];
  int* qual = new int[detchans];
  int* chans = new int[detchans];
  // create data
  for (int i=0;i<detchans;i++){
    group[i] = 1;
	qual[i] = 0;
	chans[i] = i;
  }
  
  fits_create_file(&m_File, FileName, &Status);
  if (Status != 0) {
    cout<<"Error in creating file: "<<endl;
    cout<<FileName<<show;
    m_File = 0;
    return false;
  }
  //! create binary table extension
  char ExtensionName[] = "SPECTRUM";
  long nrow = 0;
  int tfield = 4;
  char* ttype[] = {"CHANNEL","COUNTS","QUALITY","GROUPING"};
  char* tform[] = {"1i","1j","1i","1i"};
  char* tunit[] = {"PI","","",""};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<ExtensionName<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator('/');
 
  fits_write_key(m_File, TSTRING, "TELESCOP", const_cast<char*>("NuSTAR"), " ", &Status);
  fits_write_key(m_File, TSTRING, "INSTRUME", const_cast<char*>("FPM"), " ", &Status);
  fits_write_key(m_File, TSTRING, "CREATOR",  const_cast<char*>("NuSIM"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "CHANTYPE", const_cast<char*>("PI"), " ", &Status);
  fits_write_key(m_File,TLOGICAL,"POISSERR",const_cast<char*>("T")," ",&Status);
  fits_write_key(m_File, TSTRING, "HDUCLASS",  const_cast<char*>("OGIP"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "HDUCLAS1",  const_cast<char*>("SPECTRUM"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "HDUVERS",  const_cast<char*>("1.2.1"), " ", &Status);  
  fits_write_key(m_File, TINT, "DETCHANS", &detchans, " ", &Status);  
  fits_write_key(m_File, TINT, "TLMIN1", &tlmin1, " ", &Status);  
  fits_write_key(m_File, TINT, "TLMAX1", &tlmax1, " ", &Status);  
  
  fits_write_key(m_File, TFLOAT, "AREASCAL",  &unity, " ", &Status);  
  fits_write_key(m_File, TFLOAT, "CORRSCAL",  &unity, " ", &Status);  
  fits_write_key(m_File, TFLOAT, "BACKSCAL",  &unity, " ", &Status);  
  if (Type == "background"){
    fits_write_key(m_File, TSTRING, "RESPFILE",  const_cast<char*>("none"), " ", &Status);  
    fits_write_key(m_File, TSTRING, "ANCRFILE",  const_cast<char*>("none"), " ", &Status);  
    fits_write_key(m_File, TSTRING, "BACKFILE",  const_cast<char*>("none"), " ", &Status);  
  }
  if (Type == "source"){
    T.Analyze(ARFFile, false);
    fits_write_key(m_File, TSTRING, "RESPFILE",  const_cast<char*>("nustar_v3.2.rmf"), " ", &Status);  
    fits_write_key(m_File, TSTRING, "ANCRFILE",  const_cast<char*>(T.GetTokenAtAsString(T.GetNTokens()-1).Data()), " ", &Status);  
    T.Analyze(BkgFile, false);
	fits_write_key(m_File, TSTRING, "BACKFILE",  const_cast<char*>(T.GetTokenAtAsString(T.GetNTokens()-1).Data()), " ", &Status);  
  }
  fits_write_key(m_File, TFLOAT, "EXPOSURE", &exposure, " ", &Status);

  
  char Words[30];

  fits_write_col(m_File, TINT, 1, 1, 1, 820, chans, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('channels') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TLONG, 2, 1, 1, 820, data, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('counts') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TINT, 3, 1, 1, 820, qual, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('quality') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TINT, 4, 1, 1, 820, group, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error : fits_write_col('grouping') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }

  fits_close_file(m_File, &Status);
 
  return 0;

}

/****************************************************************************/


bool Target::ReadCalibratedAlignmentsDB()
{
  //! Read the data from file
  //! For the time being this is identical with the ReadPerturbedAlignmentsDB(TString FileName)
  //! But I don't expect it to stay this way since the detector orientations are missing, 
  //! and I would out which more stuff in here...

  // Load from file
  //MFile::ExpandFileName(FileName);
  ifstream in;
  in.open(DataBase);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<DataBase<<"\""<<endl;
    return false;
  }

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);

  // Read the header
  
  // If there is any error in the file, it will always be caught in ParseDB...
  TString Line;
  Line.ReadToDelim(in, Delimeter); // version
  Line.ReadToDelim(in, Delimeter); // type
  Line.ReadToDelim(in, Delimeter); // description
  Line.ReadToDelim(in, Delimeter); // column title 1
  
  Line.ReadToDelim(in, Delimeter);
  TString Positions = Line;
      
  Line.ReadToDelim(in, Delimeter);
  TString Rotations = Line;

  NAlignmentsDBEntry O;
  if (O.ParseDB(Positions, Rotations) == false) {
    mgui<<"Parsing failed: Something is wrong with your calibrated alignments data base!"<<show;
    in.close();
    return false;            
  }
  IdealAlignments = O;
  
  in.close();

  return true;
}

/****************************************************************************/

char Target::FindDelimeter(ifstream& in)
{  
  //! Find the line-delimeter of the file and rewind file
 
 
  // Let's read until we find the first delimeter:
  char Delimeter;
  do {
    in.get(Delimeter);
  } while (in.eof() == false && int(Delimeter) != 10 && int(Delimeter) != 13); // 10: \n o new line/line feed --- 13: carriage return
  if (int(Delimeter) == 13) {
    char c;
    in.get(c);
    if (int(c) == 10) Delimeter = c;
  }
  in.seekg(0, ios::beg); // rewind

  return Delimeter;
}

/****************************************************************************/

bool Target::SetTarget(MVector region)
{  

  // Transform to radians
  m_Ra = (xCenterValue+region[0]*xDelta/cos(AvgDec/rad))/c_Deg;
  m_Dec = (yCenterValue+region[1]*yDelta)/c_Deg;
  m_R = region[2];
  //cout<<m_Ra<<" "<<m_Dec<<endl;
   
  // turn into vector	  
  Vsky[0]=cos(m_Dec)*cos(m_Ra);
  Vsky[1]=cos(m_Dec)*sin(m_Ra);
  Vsky[2]=sin(m_Dec);  
  
  Vob = Vsky.Unitize();
  //cout<<Vsky<<endl;
  //cout<<Rstar.ToString()<<endl;
  Rstar.TransformOut(Vob);
  m_oaa = acos(Vob[2])*c_Deg*60.;
  //cout<<Vob<<m_oaa<<endl;
  
  // Calculate normalization to turn into pixel coordinates
  MVector FPM = IdealAlignments.GetOrientationFocalPlaneModule(1).GetTranslation();
  NOrientation Rfbfm = IdealAlignments.GetOrientationFocalPlaneModule(1);
  MVector FM = IdealAlignments.GetOrientationOpticsRelOpticalBench(1).GetTranslation();
  MVector FBOB = IdealAlignments.GetOrientationOpticalBench().GetTranslation();
  float FL = FM[2]+(FBOB[2]-FPM[2]);

  // reverse normalization
  
  //cout<<Vob<<endl;  
  Vob = Vob*FL;
  //cout<<Vob<<endl;
  Vob = FM-Vob;
  //cout<<Vob<<FL<<FM<<endl;
  
  Vdet=Vob;
  Rfbob.TransformOut(Vdet);
  //cout<<Vdet<<endl;
  Rfbfm.TransformIn(Vdet);
  //cout<<Vdet<<endl;
  
  
  return 0;
  
}

/****************************************************************************/
int Target::GetDetIndex()
{

  int iX = floor((Vdet[0]+25)/0.2);
  int iY = floor((Vdet[1]+25)/0.2);
  
  //cout<<Vdet<<endl;
  //cout<<iX<<" "<<iY<<endl;
  
  return iY*250+iX;
  
}

/****************************************************************************/
float Target::GetPSFFraction(float region, int oaa)
{

  // one PSF image pixel is 0.2x0.2 mm

  
  float AccumulatedPSF=0;
 
  //cout<<Vdet<<" "<<region<<endl; 
  for (int i=0;i < 40401; i++) {
    float pixX = floor(i/201); 
	float pixY = i - pixX*201;
	pixX = (pixX-100)*0.2; // mm
	pixY = (pixY-100)*0.2; // mm
	float radius = sqrt(pow(pixX,2)+pow(pixY,2));
    pixX += Vdet[0];
	pixY += Vdet[1];

	if (radius > region) continue;
	
	//Check gaps and edges
	if ((pixX > -20.25 && pixX < -0.25) || (pixX > 0.25 && pixX < 20.25)) {
	  if ((pixY > -20.25 && pixY < -0.25) || (pixY > 0.25 && pixY < 20.25)) {
  	    int n = 40401*oaa+i;
        AccumulatedPSF += psfimg.cube[n];
      }
	}
  }
  
  //cout<<"PSFfraction = "<<AccumulatedPSF<<endl;

  return AccumulatedPSF;

}	
/****************************************************************************/

bool Target::GenerateARF()
{

  OpenEvtFile();
  ReadCalibratedAlignmentsDB();

  int Status = 0;
  float floatnull;
  double doublenull;
  int anynull;

  cout<<"Calculate ARF..."<<endl;

  double* fT= new double[axis2];
  double* fL= new double[axis2];
  int* iModule = new int[axis2];
  float* fX = new float[axis2];
  float* fY = new float[axis2];
  int* iPHA = new int[axis2];
  int* iType = new int[axis2];
  float* fQx = new float[axis2];
  float* fQy = new float[axis2];
  float* fQz = new float[axis2];
  float* fQr = new float[axis2];
  float* fTx = new float[axis2];
  float* fTy = new float[axis2];
  float* fTz = new float[axis2];
  float* fQSx = new float[axis2];
  float* fQSy = new float[axis2];
  float* fQSz = new float[axis2];
  float* fQSr = new float[axis2];
 
  fits_read_col(m_File, TDOUBLE, 1, 1, 1, axis2, &doublenull, fT, &anynull,&Status);
  fits_read_col(m_File, TDOUBLE, 2, 1, 1, axis2, &doublenull, fL, &anynull,&Status); 
  fits_read_col(m_File, TINT, 3, 1, 1, axis2, &anynull, iModule, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 4, 1, 1, axis2, &floatnull, fX, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 5, 1, 1, axis2, &floatnull, fY, &anynull,&Status);
  fits_read_col(m_File, TINT, 6, 1, 1, axis2, &anynull, iPHA, &anynull,&Status);
  fits_read_col(m_File, TINT, 9, 1, 1, axis2, &anynull, iType, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 11, 1, 1, axis2, &floatnull, fQx, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 12, 1, 1, axis2, &floatnull, fQy, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 13, 1, 1, axis2, &floatnull, fQz, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 14, 1, 1, axis2, &floatnull, fQr, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 15, 1, 1, axis2, &floatnull, fTx, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 16, 1, 1, axis2, &floatnull, fTy, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 17, 1, 1, axis2, &floatnull, fTz, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 18, 1, 1, axis2, &floatnull, fQSx, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 19, 1, 1, axis2, &floatnull, fQSy, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 20, 1, 1, axis2, &floatnull, fQSz, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 21, 1, 1, axis2, &floatnull, fQSr, &anynull,&Status);
  
  fits_close_file(m_File, &Status); 
  
  MVector region;
  region = LoadDSRegion(DS9File);
  float regionAtDet = region[2]*yDelta/c_Deg*10150.;
  
  float exposure1=0;
  float exposure2=0;
  float lifetime1 = 0;
  float lifetime2 = 0;
  long* source1 = new long[820];
  long* bkg1 = new long[820];
  long* source2 = new long[820];
  long* bkg2 = new long[820];
  for (int i=0; i<820;i++) {
    source1[i] = 0;
	bkg1[i] = 0;
	source2[i] = 0;
	bkg2[i] = 0;
	ARF1[i] = 0;
	ARF2[i] = 0;
  }
  
  
  vector <int> DetIndex1;
  vector <float> TotalTime1;
  vector <int> oaa1;
  vector <int> DetIndex2;
  vector <float> TotalTime2;
  vector <int> oaa2;

  int exists1 = 0;
  int exists2 = 0;
  double last_t1 = fT[0];
  double last_t2 = fT[0];
  double dt = 0;
 
  for (int i=0;i<axis2-1;i++){
	Rfbob.SetTranslation((double)fTx[i],(double)fTy[i],(double)fTz[i]);
	Rfbob.SetRotation((double)fQx[i],(double)fQy[i],(double)fQz[i],(double)fQr[i]);
    Rstar.SetRotation((double)fQSx[i],(double)fQSy[i],(double)fQSz[i],(double)fQSr[i]);
	if (iModule[i] == 1) {
	  dt = fT[i]-last_t1; 
	  exposure1 += dt;
	  if (fL[i] > 0) lifetime1 += fL[i];
	  else lifetime1+=dt;
	  last_t1 = fT[i];
	}
	if (iModule[i] == 2) {
	  dt = fT[i]-last_t2; 
	  exposure2 += dt;
	  if (fL[i] > 0) lifetime2 += fL[i];
	  else lifetime2+=dt;
	  last_t2 = fT[i];
    }
	SetTarget(region);
	
	// Check what effective area bin the event falls in 
	// and add the PSF fraction to the other PSF fractions
	// for the same bin.
	// The idea here is that EA(t,alpha) can be devided into
	// N*EA'(t,1) + M*EA'(t,2) + .... 
	// and that 
	// EA'(t,1) = EA(t,1)*(PSF(1)*i + PSF(2)*j + ....)*dt
	// where PSF(n)*tn is PSF of shape n that appeared i times.
	// We here assumed a constant time step which is ofcourse not the
	// case. Instead we will factor in the duration the PSF fraction 
	// so that PSF(1)*t1 and all the t = t1 + t2 + .... will add up to
	// the total time spend at EA(t,1).
	//cout<<m_oaa<<endl;
	
	//cout<<m_oaa<<endl;
	
	if (m_oaa < 11.5) {

	  // fill in first entry
	  if (i==0){
	    if (iModule[i] == 1) {
	      DetIndex1.push_back(GetDetIndex());  // first index, using 0.02 resolution
	      TotalTime1.push_back(dt);
		  oaa1.push_back(round(m_oaa));
	      continue;
		}
		if (iModule[i] == 2) {
	      DetIndex2.push_back(GetDetIndex());  // first index, using 0.02 resolution
	      TotalTime2.push_back(dt);
		  oaa2.push_back(round(m_oaa));
	      continue;
		}
	  }
    
      if (iModule[i] == 1) exists1 = 0;
      if (iModule[i] == 2) exists2 = 0;
	 
	  // Check that this pixel is already exposed and at what oaa
	  if (iModule[i] == 1) {
	    for (int j=0;j < (int)DetIndex1.size(); j++){
		  if (DetIndex1[j] == GetDetIndex() && oaa1[j] == round(m_oaa)) {
			exists1 = 1;
		    TotalTime1[j] += dt;
		  }
	    }
	    if (exists1 == 0) {
		  DetIndex1.push_back(GetDetIndex());  // pixel
	      TotalTime1.push_back(dt);
	      oaa1.push_back(round(m_oaa));
	    }
	  }
	  if (iModule[i] == 2) {
	    for (int j=0;j < (int)DetIndex2.size(); j++){
		  if (DetIndex2[j] == GetDetIndex() && oaa2[j] == round(m_oaa)) {
		    exists2 = 1;
		    TotalTime2[j] += dt;
		  }
	    }
		if (exists2 == 0) {
	      DetIndex2.push_back(GetDetIndex());  // pixel
	      TotalTime2.push_back(dt);
 	      oaa2.push_back(round(m_oaa));
	    }
	  }
	  
	  // And if it hasnt then add it to transform
	  
    } //end loop oaa<11.5 arcmin
	
	//  In here add to the source spectrum and background spectrum. 
	float rr = sqrt(pow(fX[i]-region[0],2)+pow(fY[i]-region[1],2));	 
	if (rr < region[2] && iPHA[i] < 820) {
	  if (iModule[i] == 1) source1[iPHA[i]] += 1;
	  if (iModule[i] == 2) source2[iPHA[i]] += 1;
	  if (iType[i] == 2) {
	    if (iModule[i] == 1) bkg1[iPHA[i]] += 1;
	    if (iModule[i] == 2) bkg2[iPHA[i]] += 1;
	  }
    }
	
  } //End loop over transforms

  cout<<DetIndex1.size()<<endl;

  for (int j=0;j<(int)DetIndex1.size();j++) 
    oaat_hist1[int(oaa1[j]*2)] += TotalTime1[j]*GetPSFFraction(regionAtDet, int(oaa1[j]*2));
  for (int j=0;j<(int)DetIndex2.size();j++) 
    oaat_hist2[int(oaa2[j]*2)] += TotalTime2[j]*GetPSFFraction(regionAtDet, int(oaa2[j]*2));
  
  for (int i=0; i<820;i++) {
    for (int j=0; j<29; j++) {
	  ARF1[i] += AE[i]*vignet[j*820+i]*oaat_hist1[j]/exposure1; 
	  ARF2[i] += AE[i]*vignet[j*820+i]*oaat_hist2[j]/exposure2; 
	}
  }

  cout<<"oaa = "<<m_oaa<<" "<<oaa1[0]<<endl;
  cout<<"PSFfraction = "<<GetPSFFraction(regionAtDet,int(oaa1[0]*2))<<endl;
  cout<<"exposure:"<<exposure1<<" Lifetime:"<<lifetime1<<" "<<lifetime2<<endl;
  cout<<"source counts @ 10 keV: "<<source1[71]<<endl;
  cout<<"Effective Area @ 3 keV "<< ARF1[0]<<" "<<AE[0]<<endl;
  cout<<"Effective Area @ 10 keV "<<ARF1[71]<<" "<<AE[71]<<endl;
   
  WriteFitsSpectrum(SourceOutFile, "source", source1, lifetime1);
  WriteFitsSpectrum(BkgFile, "bkg", bkg1, lifetime1);
  WriteARF(Elow,Ehigh,ARF1);
  
  // We now have a histogram of dimensions offaxis angle that matches the loaded 
  // effective area. And we have a cumulative PSF fraction with duration.
  // Now we multiply the whole thing together and normalize.
  
  // Finally write out all files to FITS.
  //
  delete [] fT;
  delete [] fL;
  delete [] iModule;
  delete [] fX;
  delete [] fY;
  delete [] iPHA;
  delete [] iType;
  delete [] fQx;
  delete [] fQy;
  delete [] fQz;
  delete [] fQr;
  delete [] fTx;
  delete [] fTy;
  delete [] fTz;
  delete [] fQSx;
  delete [] fQSy;
  delete [] fQSz;
  delete [] fQSr;
 
  
  return true;
}

/****************************************************************************/


int main(int argc, char** argv)
{
  // Main function... the beginning...

  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: MakeARF <arg1> <arg2>"<<endl;
  Usage<<"    arguments:"<<endl;
  Usage<<"         arg1:   full path and name of nusim event fits file"<<endl;
  Usage<<"         arg2:   full path and name of the ds9 region file"<<endl;
  Usage<<endl;

  string Option;

  // Check for help
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "-h" || Option == "--help" || Option == "?" || Option == "-?") {
      cout<<Usage.str()<<endl;
      return false;
    }
  }



  Target src;
  src.Initialize();
 
  TString InFile = argv[1];
  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator('.');
  T.Analyze(InFile, false);
  
  src.SourceInFile = InFile;
  src.DS9File = argv[2];
  src.SourceOutFile = T.GetTokenAtAsString(0) + ".pi";
  src.BkgFile = T.GetTokenAtAsString(0) + "_bkg.fits";
  src.ARFFile = T.GetTokenAtAsString(0) + ".arf";
  
  src.GenerateARF();

  cout<<"Files written: "<<endl;
  cout<<src.SourceOutFile<<endl;
  cout<<src.BkgFile<<endl;
  cout<<src.ARFFile<<endl;
    
  cout<<"Done!"<<endl;
  
   
  return 0;
}


// Main: the end...
//////////////////////////////////////////////////////////////////////////////////
