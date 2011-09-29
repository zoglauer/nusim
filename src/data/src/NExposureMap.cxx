/*
 * NExposureMap.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NExposureMap
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NExposureMap.h"

// Standard libs:
#include <iomanip>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NExposureMap)
#endif


////////////////////////////////////////////////////////////////////////////////


NExposureMap::NExposureMap(NSatellite& Satellite) : m_Satellite(Satellite)
{
  // Construct an instance of NExposureMap

  m_File = 0;
  m_FileName = "!Data.exposure.fits";
}


////////////////////////////////////////////////////////////////////////////////


NExposureMap::~NExposureMap()
{
  // Delete this instance of NExposureMap
}


////////////////////////////////////////////////////////////////////////////////


void NExposureMap::Clear()
{
  // Clear the data
 
  m_Qfbob.clear();
  m_Tfbob.clear();
  m_Qstar.clear();
  m_Time.clear();
  
  Det1.Clear();
  Det2.Clear();
  Det3.Clear();
  Det4.Clear();
  DetHold.Clear();
 
  SkyMapX.clear();
  SkyMapY.clear();
  SkyMapZ.clear();
  TransformIndex.clear();
  TotalTime.clear();
  Skypix.clear();
  SkyExposed.clear();
}


////////////////////////////////////////////////////////////////////////////////


void NExposureMap::SetFileNamePrefix(TString FileNamePrefix)
{
  m_FileName = "!";
  m_FileName += FileNamePrefix;
  m_FileName += ".exposure.fits";  
}


////////////////////////////////////////////////////////////////////////////////


void NExposureMap::SetImageParameters(float TCRVL1, float TCDLT1, float TLMAX1, float TCRVL2, float TCDLT2, float TLMAX2)
{
  // Set all relevant image parameters from the original fits file
  
  // Extract size of the field
  // The field is pinned to pix (0,0) which in sky coords is (TCRVL1,TCRVL2).
  // The opposite corner is (TCRVL1+TLMAX1*TCDLT1,TCRVL2+TLMAX2*TCDLT2).
  

  xCenterValue = TCRVL1;
  //cout<<"xCenterValue: "<<xCenterValue<<endl;
  
  xDelta = -fabs(TCDLT1);
  //cout<<"xDelta: "<<xDelta<<endl;
  
  yCenterValue = TCRVL2;
  //cout<<"yCenterValue: "<<yCenterValue<<endl;
  
  yDelta = fabs(TCDLT2);
  
  float xMax = ceil(TLMAX1);
  float yMax = ceil(TLMAX2);
    
  AvgDec = yCenterValue+yMax*0.5*yDelta;
  MaxRa = xCenterValue;
  
  //create sky map
  //create dimension vectors x, y
  for (int x = 0; x <= xMax; ++x) SkyMapX.push_back(xCenterValue+x*xDelta);
  for (int y = 0; y <= yMax; ++y) SkyMapY.push_back(yCenterValue+y*yDelta);
  //create 2d map
  for (int x = 0; x <= xMax; ++x) {
    for (int y = 0; y <= yMax; ++y) {
      SkyMapZ.push_back(0);
      SkyExposed.push_back(0);
	  }
  }
  //cout<<SkyMapX[0]<<" "<<SkyMapX[xMax-1]<<endl;
  //cout<<SkyMapY[0]<<" "<<SkyMapY[yMax-1]<<endl;
 
}


////////////////////////////////////////////////////////////////////////////////


void NExposureMap::AddObservatoryData(NQuaternion& Qfbob, MVector& Tfbob, NQuaternion& Qstar, NTime& Time)
{  
  m_Qfbob.push_back(Qfbob);
  m_Tfbob.push_back(Tfbob);
  m_Qstar.push_back(Qstar);
  m_Time.push_back(Time);
}


////////////////////////////////////////////////////////////////////////////////


bool NExposureMap::WriteExposureMap(int index)
{
   
	m_File = 0;
  int Status = 0;
  long fpixel, nelements;
	float *array[SkyMapY.size()];
		
  /* initialize FITS image parameters */

	int bitpix   =  FLOAT_IMG;
	long naxis    =   2;  /* 2-dimensional image                            */    
  long naxes[2] = { SkyMapX.size(), SkyMapY.size() };   /* image is 300 pixels wide by 200 rows */

  array[0] = (float *)malloc( naxes[0] * naxes[1] * sizeof( float ) );

  for (int ii = 1; ii < naxes[1]; ii++) {
    array[ii] = array[ii-1] + naxes[0];
  }

  for (int jj = 0; jj < naxes[1]; jj++) {   
    for (int ii = 0; ii < naxes[0]; ii++) {
      array[jj][ii] = SkyMapZ[ii + jj*naxes[0]];
	}
  }

  cout<<"File name:"<<m_FileName.Data()<<endl;
  fits_create_file(&m_File, m_FileName.Data(), &Status); /* create new FITS file */
  if (Status != 0) {
    cout<<"Unable to open file: "<<m_FileName<<endl;
    return false;
  }

    /* write the required keywords for the primary array image.     */
    /* Since bitpix = USHORT_IMG, this will cause cfitsio to create */
    /* a FITS image with BITPIX = 16 (signed short integers) with   */
    /* BSCALE = 1.0 and BZERO = 32768.  This is the convention that */
    /* FITS uses to store unsigned integers.  Note that the BSCALE  */
    /* and BZERO keywords will be automatically written by cfitsio  */
    /* in this case.                                                */

    fits_create_img(m_File,  bitpix, naxis, naxes, &Status);

    fpixel = 1;                               /* first pixel to write      */
    nelements = naxes[0] * naxes[1];          /* number of pixels to write */

    /* write the array of unsigned integers to the FITS file */
    fits_write_img(m_File, TFLOAT, fpixel, nelements, array[0], &Status);
  
	free( array[0] );  /* free previously allocated memory */
	
	//Write header keywords
	char bunit[10], tctyp1[10], tctyp2[10], cunit[10], radecsys[10],
	     mtype1[10], mtype2[10],mform1[10],mform2[10], tctyp1p[10], tctyp2p[10], wcsty[10],
		 hduvers[10], hduclass[10], hduclas1[10], hduclas2[10], hduname[10];

	strcpy(bunit, "seconds");
    strcpy(cunit,"deg");
    strcpy(tctyp1,"RA---TAN");
    strcpy(tctyp2,"DEC--TAN");
	strcpy(radecsys,"FK5");
	strcpy(mform1,"X,Y");
	strcpy(mtype1,"SKY");
	strcpy(tctyp1p,"X");
	strcpy(tctyp2p,"Y");
	strcpy(wcsty,"PHYSICAL");
    strcpy(mtype2,"EQPOS");
    strcpy(mform2,"RA,DEC");
	strcpy(hduvers,"1.0.0");
	strcpy(hduclass,"OGIP");
	strcpy(hduclas1,"IMAGE");
	strcpy(hduclas2,"EXPOSURE");
	strcpy(hduname,"EXPMAP");
	
	//float one = 1.0;
    float tcrpx1 = 0;
	float tcrpx2 = 0;
 
 	fits_write_key(m_File, TSTRING, "HDUVERS", hduvers," ", &Status);
 	fits_write_key(m_File, TSTRING, "HDUCLASS", hduclass," ", &Status);
	fits_write_key(m_File, TSTRING, "HDUCLAS1", hduclas1," ", &Status);
	fits_write_key(m_File, TSTRING, "HDUCLAS2", hduclas2," ", &Status);
	fits_write_key(m_File, TSTRING, "HDUNAME", hduname," ", &Status);
	fits_write_key(m_File, TSTRING, "BUNIT", bunit," ", &Status);
	
    fits_write_key(m_File, TFLOAT, "CDELT1", &xDelta, "Platescale", &Status); 	  
    fits_write_key(m_File, TFLOAT, "CDELT2", &yDelta, "Platescale", &Status); 	  
    fits_write_key(m_File, TFLOAT, "CRVAL1", &xCenterValue, "Transform to celestial coords", &Status); 	  
    fits_write_key(m_File, TFLOAT, "CRVAL2", &yCenterValue, "Transform to celestial coords", &Status); 	  
    fits_write_key(m_File, TFLOAT, "CRPIX1", &tcrpx1, "Pixel reference point", &Status); 	  
    fits_write_key(m_File, TFLOAT, "CRPIX2", &tcrpx2, "Pixel reference point", &Status); 	  
    fits_write_key(m_File, TSTRING, "CUNIT1", cunit," ", &Status);
    fits_write_key(m_File, TSTRING, "CUNIT2", cunit," ", &Status);
    fits_write_key(m_File, TSTRING, "CTYPE1", tctyp1," ", &Status);
    fits_write_key(m_File, TSTRING, "CTYPE2", tctyp2," ", &Status); 
	fits_write_key(m_File, TSTRING, "RADECSYS", radecsys," ", &Status); 
 
	fits_close_file(m_File, &Status);                /* close the file */
    
    return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NExposureMap::MakeDetMap()
{

  Det1.AddVertice(-20.25,20.25);
  Det1.AddVertice(-0.25,20.25);
  Det1.AddVertice(-0.25,0.25);
  Det1.AddVertice(-20.25,0.25);
  Det1.AddVertice(-20.25,20.25);

  Det2.AddVertice(0.25,20.25);
  Det2.AddVertice(20.25,20.25);
  Det2.AddVertice(20.25,0.25);
  Det2.AddVertice(0.25,0.25);
  Det2.AddVertice(0.25,20.25);


  Det3.AddVertice(0.25,-0.25);
  Det3.AddVertice(20.25,-0.25);
  Det3.AddVertice(20.25,-20.25);
  Det3.AddVertice(0.25,-20.25);
  Det3.AddVertice(0.25,-0.25);


  Det4.AddVertice(-20.25,-0.25);
  Det4.AddVertice(-0.25,-0.25);
  Det4.AddVertice(-0.25,-20.25);
  Det4.AddVertice(-20.25,-20.25);
  Det4.AddVertice(-20.25,-0.25);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NExposureMap::ProjectOnSky(float dt)
{
  
  //Det1.ToRaDeg();
  //Det2.ToRaDeg();
  //Det3.ToRaDeg();
  //Det4.ToRaDeg();


  for (int i=0; i<(int) SkyMapX.size();i++){
    for (int j=0; j<(int) SkyMapY.size();j++){
	    if (Det1.IsInside(SkyMapX[i], SkyMapY[j])) {
		    SkyMapZ[i+SkyMapX.size()*j] += dt;
	      continue;  
	    }	
	    if (Det2.IsInside(SkyMapX[i], SkyMapY[j])) {
		    SkyMapZ[i+SkyMapX.size()*j] += dt;
	      continue;  
	    }	
	    if (Det3.IsInside(SkyMapX[i], SkyMapY[j])) {
		    SkyMapZ[i+SkyMapX.size()*j] += dt;
	      continue;  
	    }	
	    if (Det4.IsInside(SkyMapX[i], SkyMapY[j])) {
		    SkyMapZ[i+SkyMapX.size()*j] += dt;
	      continue;  
	    }	
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MVector NExposureMap::ToSkyPix(MVector Coords)
{
  
  MVector pix;
  // find the index that the corner falls in.
  // Doing this analytically for speed
  pix[0] = floor((Coords[0]-xCenterValue)/xDelta);
  pix[1] = floor((Coords[1]-yCenterValue)/yDelta);
  
  return pix;
}


////////////////////////////////////////////////////////////////////////////////


bool NExposureMap::ExposeSky(int index)
{

  int hit = 0;
  NOrientation Rfbob;
  NOrientation Qstar;
  MVector FM;
  NOrientation FPM;
  
  cout<<"ToDo later: Make sure we are using both modules here..."<<endl;
  FPM = m_Satellite.GetCalibratedOrientationFocalPlaneModule(1);
  FM = m_Satellite.GetCalibratedOrientationOpticsRelOpticalBench(1).GetTranslation();
    
  unsigned int Progress = m_Time.size()/1000;
  if (Progress < 10) Progress = 10;
  for (unsigned int i = 0; i < m_Time.size()-1; i++) {
    if (i%Progress == 0) {
      cout<<"\r"<<"Progress part 1/2: "<<setprecision(4)<<100.0*i/m_Time.size()<<"%    "<<flush;
    }
    
    Rfbob.Clear();
    Rfbob.SetTranslation(m_Tfbob[i]);
    Rfbob.SetRotation(m_Qfbob[i]);
    Qstar.Clear();
    Qstar.SetRotation(m_Qstar[i]);


    float dt = m_Satellite.GetEffectiveObservationTime(m_Time[i], m_Time[i+1]).GetAsSeconds();
    
    DetHold.Clear();
    DetHold.AddVertice(20.25,20.25);
    DetHold.TransformMapOut(FPM);
    DetHold.TransformMapIn(Rfbob);
    DetHold.Bench2Sky(Qstar, FM, MaxRa, AvgDec);
    Corner = ToSkyPix(DetHold.GetCorner());
	
	  // fill in first entry
	  if (i == 0) {
	    TransformIndex.push_back(i);  // first index occurance of transform
	    TotalTime.push_back(dt);
	    Skypix.push_back(Corner);
	    continue;
	  }
    
	  hit = 0;
	  // Check that this transform has already occured
	  for (int j=0;j < (int)TransformIndex.size(); j++){
	    if (Corner[0] == Skypix[j][0] && Corner[1] == Skypix[j][1]) {
		    hit = 1;
		    TotalTime[j] += dt;
	    }
	  }	
	  // And if it hasn't then add it to transform
	  if (hit == 0) {
	    //cout<<Corner<<endl;
	    TransformIndex.push_back(i);  // first index occurance of transform
	    TotalTime.push_back(dt);
	    Skypix.push_back(Corner);
	  }
  }
  cout<<"\r"<<"Progress part 1/2: "<<setprecision(4)<<100.0<<"%    "<<flush;
  cout<<endl;
  
  
  Progress = TransformIndex.size()/1000;
  if (Progress < 10) Progress = 10;
  for (unsigned int i = 0; i < TransformIndex.size(); i++) {
    if (i%Progress == 0) {
      cout<<"\r"<<"Progress part 2/2: "<<setprecision(4)<<100.0*i/TransformIndex.size()<<"%    "<<flush;
    }

    //cout<<TransformIndex[i]<<endl;
    //Now do the exposure map calculation
    Rfbob.Clear();
    Rfbob.SetTranslation(m_Tfbob[TransformIndex[i]]);
    Rfbob.SetRotation(m_Qfbob[TransformIndex[i]]);
    Qstar.Clear();
    Qstar.SetRotation(m_Qstar[TransformIndex[i]]);

	  MakeDetMap();
	  Det1.TransformMapOut(FPM);
	  Det2.TransformMapOut(FPM);
	  Det3.TransformMapOut(FPM);
	  Det4.TransformMapOut(FPM);

	  Det1.TransformMapIn(Rfbob);
	  Det2.TransformMapIn(Rfbob);
	  Det3.TransformMapIn(Rfbob);
	  Det4.TransformMapIn(Rfbob);
	  
	  Det1.Bench2Sky(Qstar, FM, MaxRa, AvgDec);
	  Det2.Bench2Sky(Qstar, FM, MaxRa, AvgDec);
	  Det3.Bench2Sky(Qstar, FM, MaxRa, AvgDec);
	  Det4.Bench2Sky(Qstar, FM, MaxRa, AvgDec);
		
	  ProjectOnSky(TotalTime[i]);
	  Det1.Clear();
	  Det2.Clear();
	  Det3.Clear();
	  Det4.Clear();
  }
  cout<<"\r"<<"Progress part 2/2: "<<setprecision(4)<<100.0<<"%    "<<flush;
  cout<<endl;	

  WriteExposureMap(index);

  // Clear arrays
  SkyMapZ.clear();
  TransformIndex.clear();
  TotalTime.clear();
  Skypix.clear();

  
  return true;
}


// NExposureMap.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
