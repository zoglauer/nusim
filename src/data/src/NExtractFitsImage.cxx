/*
 * NExtractFitsImage.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NExtractFitsImage
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NExtractFitsImage.h"

// Standard libs:

// ROOT libs:
#include "TH2D.h"
#include "TCanvas.h"

// HEAsoft
#include "fitsio.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NExtractFitsImage)
#endif


////////////////////////////////////////////////////////////////////////////////


NExtractFitsImage::NExtractFitsImage()
{
  // Construct an instance of NExtractFitsImage
}


////////////////////////////////////////////////////////////////////////////////


NExtractFitsImage::~NExtractFitsImage()
{
  // Delete this instance of NExtractFitsImage
}


////////////////////////////////////////////////////////////////////////////////


bool NExtractFitsImage::Extract(TString FileName, MFunction2D& Image)
{
  //! Extract the image as a (MEGAlib) 2D function. Returns false on error
  
  // Load all fits files
  MFile::ExpandFileName(FileName);
  
  int Status = 0;
  
  fitsfile* File = 0; 
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
  
  // Check if we have an image
  char Type[20];
  char Key[20];
  strcpy(Key, "XTENSION");
  fits_read_keyword(File, Key, Type, NULL, &Status);
  if ((Status == 0 && strstr(Type , "IMAGE") != NULL) || Status == 202) {
    Status = 0;
  } else {
    mgui<<"No image found in fits file!"<<show;
    return false;
  }


  // Retrieve the dimensions and data type of the image:
  int NDimensions;
  int DataType;
  long AxisDimension[9];  
  fits_get_img_param(File, 9, &DataType, &NDimensions, AxisDimension, &Status);
  if (Status != 0 || NDimensions == 0) {
    mgui<<"The image in the fits file has no dimensions..."<<show;
    return false;
  }
   
   
  // Create some dummy arrays
  //unsigned int tfSize[9];
  long StartPixel[9];
  long NPixel = 1;
  for (int dim = 0; dim < NDimensions; ++dim) {
    NPixel *= AxisDimension[dim];
    StartPixel[dim] = 1;
    //tfSize[dim] = AxisDimension[NDimensions - dim - 1];
  }


  // Retrieve axes:
  char Dummy[30];
  fits_read_keyword(File, "CTYPE1", Dummy, NULL, &Status);
  if (TString(Dummy) != "'RA---TAN'") {
    mgui<<"We have an unhandled x-axis type in the fits file: \""<<Dummy<<"\" (expected: 'RA---TAN')"<<show;
    //return false;
  }
  if (Status != 0) {
    mgui<<"Cannot retrieve CTYPE1 keyword..."<<show;
    return false;
  }
  //cout<<"xAxisType: "<<Dummy<<endl;
  
  fits_read_keyword(File, "CRVAL1", Dummy, NULL, &Status);
  if (Status != 0) {
    mgui<<"Cannot retrieve CRVAL1 keyword..."<<show;
    return false;
  }  
  double xCenterValue = atof(Dummy);
  //cout<<"xCenterValue: "<<xCenterValue<<endl;
  
  fits_read_keyword(File, "CRPIX1", Dummy, NULL, &Status);
  if (Status != 0) {
    mgui<<"Cannot retrieve CRPIX1 keyword..."<<show;
    return false;
  }  
  double xCenterPixel = atof(Dummy);
  //cout<<"xCenterPixel: "<<xCenterPixel<<endl;
  
  fits_read_keyword(File, "CDELT1", Dummy, NULL, &Status);
  if (Status != 0) {
    Status = 0;
    fits_read_keyword(File, "CD1_1", Dummy, NULL, &Status);
    if (Status != 0) {
      mgui<<"Cannot retrieve CDELT1 keyword..."<<show;
      return false;
    }
  }
  double xDelta = -fabs(atof(Dummy));
  //cout<<"xDelta: "<<xDelta<<endl;

  
  fits_read_keyword(File, "CTYPE2", Dummy, NULL, &Status);
  if (Status != 0) {
    mgui<<"Cannot retrieve CTYPE2 keyword..."<<show;
    return false;
  }  
  if (TString(Dummy) != "'DEC--TAN'") {
    mgui<<"We have an unhandled y-axis type in the fits file: \""<<Dummy<<"\" (expected: 'DEC--TAN')"<<show;
    //return false;
  }
  //cout<<"yAxisType: "<<Dummy<<endl;
  
  fits_read_keyword(File, "CRVAL2", Dummy, NULL, &Status);
  if (Status != 0) {
    mgui<<"Cannot retrieve CRVAL2 keyword..."<<show;
    return false;
  }  
  double yCenterValue = atof(Dummy);
  //cout<<"yCenterValue: "<<yCenterValue<<endl;

  fits_read_keyword(File, "CRPIX2", Dummy, NULL, &Status);
  if (Status != 0) {
    mgui<<"Cannot retrieve CRPIX2 keyword..."<<show;
    return false;
  }  
  double yCenterPixel = atof(Dummy);
  //cout<<"yCenterPixel: "<<yCenterPixel<<endl;

  
  fits_read_keyword(File, "CDELT2", Dummy, NULL, &Status);
  if (Status != 0) {
    Status = 0;
    fits_read_keyword(File, "CD2_2", Dummy, NULL, &Status);
    if (Status != 0) {
      mgui<<"Cannot retrieve CDELT2 keyword..."<<show;
      return false;
    }
  }
  double yDelta = -fabs(atof(Dummy));
  //cout<<"yDelta: "<<yDelta<<endl;
   
  // Approximate projection on spherical coordinates:
  xDelta /= cos(yCenterValue*c_Rad);
  
  
  // Finally create the Function2D
  vector<double> xAxis;
  for (int x = StartPixel[0]; x <= AxisDimension[0]; ++x) {
    xAxis.insert(xAxis.begin(), xCenterValue + xDelta*(x - xCenterPixel));
  }
  vector<double> yAxis;
  for (int y = StartPixel[1]; y <= AxisDimension[1]; ++y) {
    yAxis.push_back(yCenterValue + yDelta*(yCenterPixel - y));
  }
  
  vector<double> zAxis;
  zAxis.resize(NPixel);
  // Filled later
  
  
  // Extract the data from the fits image
  int AnyNull;
  if (DataType == LONG_IMG) { // 32
    int IntNullVal = 0;
    int* IntArray = new int[NPixel];
    fits_read_pix(File, TINT, StartPixel, NPixel, &IntNullVal, IntArray, &AnyNull, &Status);
    if (Status != 0) {
      mgui<<"Failed to read data from fits file: "<<Status<<show;
      return false;
    }
    
    for (int x = StartPixel[0]; x <= AxisDimension[0]; ++x) {
      for (int y = StartPixel[1]; y <= AxisDimension[1]; ++y) {
        int FitsIndex = x-StartPixel[0] + (y-StartPixel[1])*AxisDimension[0];
        int FunctionIndex = (AxisDimension[0]-x) + (y-StartPixel[1])*AxisDimension[0];
        if (FitsIndex < 0 || FitsIndex >= NPixel) {
          cout<<"Fits index out of bounds"<<endl;
          continue;
        }
        if (FunctionIndex < 0 || FunctionIndex >= NPixel) {
          cout<<"Function index out of bounds"<<endl;
          continue;
        }
        zAxis[FunctionIndex] = IntArray[FitsIndex];
      }
    }
    
    delete [] IntArray;
    
  } else if (DataType == FLOAT_IMG) { // -32
    float FloatNullVal = 0;
    float* FloatArray = new float[NPixel];
    fits_read_pix(File, TFLOAT, StartPixel, NPixel, &FloatNullVal, FloatArray, &AnyNull, &Status);
    if (Status != 0) {
      mgui<<"Failed to read data from fits file: "<<Status<<show;
      return false;
    }

    for (int x = StartPixel[0]; x <= AxisDimension[0]; ++x) {
      for (int y = StartPixel[1]; y <= AxisDimension[1]; ++y) {
        int FitsIndex = x-StartPixel[0] + (y-StartPixel[1])*AxisDimension[0];
        int FunctionIndex = (AxisDimension[0]-x) + (y-StartPixel[1])*AxisDimension[0];
        if (FitsIndex < 0 || FitsIndex >= NPixel) {
          cout<<"Fits index out of bounds"<<endl;
          continue;
        }
        if (FunctionIndex < 0 || FunctionIndex >= NPixel) {
          cout<<"Function index out of bounds"<<endl;
          continue;
        }
        zAxis[FunctionIndex] = FloatArray[FitsIndex];
      }
    }
    delete [] FloatArray;
    
  } else if (DataType == DOUBLE_IMG) { // -64
    double FloatNullVal = 0;
    double* FloatArray = new double[NPixel];
    fits_read_pix(File, TDOUBLE, StartPixel, NPixel, &FloatNullVal, FloatArray, &AnyNull, &Status);
    if (Status != 0) {
      mgui<<"Failed to read data from fits file: "<<Status<<show;
      return false;
    }

    for (int x = StartPixel[0]; x <= AxisDimension[0]; ++x) {
      for (int y = StartPixel[1]; y <= AxisDimension[1]; ++y) {
        int FitsIndex = x-StartPixel[0] + (y-StartPixel[1])*AxisDimension[0];
        int FunctionIndex = (AxisDimension[0]-x) + (y-StartPixel[1])*AxisDimension[0];
        if (FitsIndex < 0 || FitsIndex >= NPixel) {
          cout<<"Fits index out of bounds"<<endl;
          continue;
        }
        if (FunctionIndex < 0 || FunctionIndex >= NPixel) {
          cout<<"Function index out of bounds"<<endl;
          continue;
        }
        zAxis[FunctionIndex] = FloatArray[FitsIndex];
      }
    }
    delete [] FloatArray;
    
  } else if (DataType == SHORT_IMG) { // 16
    short ShortNullVal = 0;
    short* ShortArray = new short[NPixel];
    fits_read_pix(File, TSHORT, StartPixel, NPixel, &ShortNullVal, ShortArray, &AnyNull, &Status);
    if (Status != 0) {
      mgui<<"Failed to read data from fits file: "<<Status<<show;
      return false;
    }

    for (int x = StartPixel[0]; x <= AxisDimension[0]; ++x) {
      for (int y = StartPixel[1]; y <= AxisDimension[1]; ++y) {
        int FitsIndex = x-StartPixel[0] + (y-StartPixel[1])*AxisDimension[0];
        int FunctionIndex = (AxisDimension[0]-x) + (y-StartPixel[1])*AxisDimension[0];
        if (FitsIndex < 0 || FitsIndex >= NPixel) {
          cout<<"Fits index out of bounds"<<endl;
          continue;
        }
        if (FunctionIndex < 0 || FunctionIndex >= NPixel) {
          cout<<"Function index out of bounds"<<endl;
          continue;
        }
        zAxis[FunctionIndex] = ShortArray[FitsIndex];
      }
    }
    delete [] ShortArray;
  } else {
    mgui<<"The fits image contains an unhandled data type: "<<DataType<<" vs. "<<FLOAT_IMG<<show;
    return false;
  }
  
  
  // Fix values below zero
  bool ContainsSubZeroValues = false;
  for (unsigned int i = 0; i < zAxis.size(); ++i) {
    if (zAxis[i] < 0) {
      zAxis[i] = 0;
      ContainsSubZeroValues = true;
    }
  }
  if (ContainsSubZeroValues == true) {
    mgui<<"Values below zero in the fits file have been assigned the value zero!"<<show;
  }
  
  
  // Set the image
  Image.Set(xAxis, yAxis, zAxis);
  
  //Image.Plot();
  
  return true;
}


// NExtractFitsImage.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
