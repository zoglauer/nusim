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


// NuSIM
#include <NQuaternion.h>
#include <NObservatoryData.h>
#include <NOrientation.h>
#include "NAlignmentsDBEntry.h"

/****************************************************************************/
class DetMap
{

public:
  DetMap();
  ~DetMap();
  DetMap(const DetMap& map);
  int IsInside(double x, double y);
  bool AddVertice(double x, double y);
  bool TransformMapIn(NOrientation& R);
  bool TransformMapOut(NOrientation& R);
  bool Bench2Sky(NOrientation&,MVector& FM,float& MaxRa, float& AvgDec);
  bool ToRaDeg(float& MaxRa, float& AvgDec);
  void Clear() {fX.clear();fY.clear();fZ.clear();}
  void ToScreen();
  MVector GetCorner() {return MVector(fX[0],fY[0],fZ[0]);}
  
private:
  vector <float> fX;
  vector <float> fY;
  vector <float> fZ;
};

class ExposureMap
{
public:
  /// Default constructor
  ExposureMap();
  /// Default destructor
  ~ExposureMap();
  // Open event fits file
  bool OpenEvtFile(TString FileName);
  bool LoadEvtData();
  bool MakeDetMap();
  bool ExposeSky(int index);
  bool ProjectOnSky(float dt);
  bool WriteExposureMap(int index);
  bool ReadCalibratedAlignmentsDB(TString FileName);
  MVector ToSkyPix(MVector Coords);
  vector <NObservatoryData> MastData; 
  vector <NObservatoryData> CHU4Data;
  NAlignmentsDBEntry IdealAlignments;
	  
protected:
  fitsfile* m_File; 
  char FindDelimeter(ifstream& in);
  
private:
 

  /// Simulation file name
  TString m_FitsFileName;
  /// Output file name
  TString m_OutputFileName;
  /// RA in degree
 long axis1;
 long axis2;
 
 DetMap Det1;
 DetMap Det2;
 DetMap Det3;
 DetMap Det4;
 DetMap DetHold;
 vector <float> SkyMapX;
 vector <float> SkyMapY;
 vector <float> SkyMapZ;
 vector <int> TransformIndex;
 vector <float> TotalTime;
 vector <MVector> Skypix;
 // keep track of whether a piece of 
 // sky already got exposed.
 vector <float> SkyExposed;
 float xDelta, yDelta;
 float xCenterValue, yCenterValue;
 float MaxRa, AvgDec;
 MVector Corner;

};

/******************************************************************************
 * Default constructor
 */
ExposureMap::ExposureMap() 
{

 m_File = 0;

}


/******************************************************************************
 * Default destructor
 */
ExposureMap::~ExposureMap()
{
  // Intentionally left blanck
}

/******************************************************************************
 * Default constructor
 */
DetMap::DetMap() 
{

}

/******************************************************************************/
// The copy constructor
DetMap::DetMap(const DetMap& map) 
{

  fX = map.fX;
  fY = map.fY;
  fZ = map.fZ;

}


/******************************************************************************
 * Default destructor
 */
DetMap::~DetMap()
{
  // Intentionally left blanck
}

/******************************************************************************/
bool DetMap::AddVertice(double x, double y)
{
  fX.push_back(x);
  fY.push_back(y);
  fZ.push_back(0);
  
  return true;
}

/******************************************************************************/
void DetMap::ToScreen()
{
  for (int i=0; i< (int) fX.size(); i++) {
    cout<<"vertice "<<i<<": "<<fX[i]<<" "<<fY[i]<<endl;
  }
  cout<<"*************************"<<endl;
}

/****************************************************************************/

int DetMap::IsInside(double x, double y)
{
//*.         Function which returns 1 if point x,y lies inside the
//*.              polygon defined by the graph points
//*.                                0 otherwise
//*.
//*.     The loop is executed with the end-point coordinates of a
//*.     line segment (X1,Y1)-(X2,Y2) and the Y-coordinate of a
//*.     horizontal line.
//*.     The counter inter is incremented if the line (X1,Y1)-(X2,Y2)
//*.     intersects the horizontal line.
//*.     In this case XINT is set to the X-coordinate of the
//*.     intersection point.
//*.     If inter is an odd number, then the point x,y is not within
//*.     the polygon.
//*.
//*.         This routine is based on an original algorithm
//*.         developed by R.Nierhaus.
//*.

   float xint;
   int inter = 0;
   for (int i=0;i<(int)fX.size()-1;i++) {
      if (fY[i] == fY[i+1]) continue;
      if (y < fY[i] && y < fY[i+1]) continue;
      if (fY[i] < y && fY[i+1] < y) continue;
      xint = fX[i] + (y-fY[i])*(fX[i+1]-fX[i])/(fY[i+1]-fY[i]);
      if (x < xint) inter++;
   }
   if (inter%2) return 1;
   return 0;
}
/****************************************************************************/
bool DetMap::TransformMapIn(NOrientation& R)
{

  MVector v;

   for (int i=0; i<(int)fX.size();i++){
    v.SetXYZ(fX[i],fY[i],fZ[i]);
	R.TransformIn(v);
	fX[i] = v[0];
	fY[i] = v[1];
    fZ[i] = v[2];
  }

  return true;
  
}

/****************************************************************************/

bool DetMap::TransformMapOut(NOrientation& R)
{

  MVector v;

  for (int i=0; i<(int)fX.size();i++){
    v.SetXYZ(fX[i],fY[i],fZ[i]);
	R.TransformOut(v);
	fX[i] = v[0];
	fY[i] = v[1];
    fZ[i] = v[2];
  }
 
  return true;
  
}

/****************************************************************************/

bool DetMap::Bench2Sky(NOrientation& R, MVector& FM, float& MaxRa, float& AvgDec)
{

  MVector v;
  
  //cout<<"OB land:"<<"Dx = "<<fX[0]<<" Dy = "<<fY[0]<<endl;
  
  for (int i=0; i<(int)fX.size();i++){
      v.SetXYZ(FM[0]-fX[i],FM[1]-fY[i],FM[2]-fZ[i]);
	  v.Unitize();
	  R.TransformIn(v);
	  fX[i] = v[0];
	  fY[i] = v[1];
      fZ[i] = v[2];	
  }
  ToRaDeg(MaxRa, AvgDec);
  //cout<<"OB land:"<<"Dx = "<<fX[0]<<" Dy = "<<fY[0]<<endl;
  
  return true;

}

/****************************************************************************/

bool DetMap::ToRaDeg(float& MaxRa, float& AvgDec)
{
   for (int i=0; i<(int)fX.size();i++){
	  fX[i] = atan2(fY[i],fX[i])*c_Deg;
	  if (fX[i] < 0.0) fX[i] += 180.0;
	  fX[i] = MaxRa+(fX[i]-MaxRa)*cos(AvgDec/rad);
	  fY[i] = asin(fZ[i])*c_Deg;
	  if (fY[i] < 0.0) fY[i] += 180.0;
   }
   //cout<<fX[0]<<" "<<fY[0]<<endl;
   
   return true;
}


/******************************************************************************/


bool ExposureMap::OpenEvtFile(TString FileName)
{  
  //! Load and initialize the file

  // cout<<"OpenFits"<<endl;
  
  int Status = 0;
  int nfound;
  m_File = 0;
  
  fits_open_file(&m_File, FileName, READONLY, &Status);
  if (Status != 0) {
    cout<<"Unable to open file: "<<endl;
    cout<<FileName<<endl;
    return false;
  }

  // Switch to the right extension:
  int HDUNumber = 2;
  fits_movabs_hdu(m_File, HDUNumber, NULL, &Status);
  if (Status != 0) {
    cout<<"HDU number "<<HDUNumber<<" not found in fits file!"<<cout;
    return false;
  }
  
  // Extract size of the field
  // The field is pinned to pix (0,0) which in sky coords is (TCRVL1,TCRVL2).
  // The opposite corner is (TCRVL1+TLMAX1*TCDLT1,TCRVL2+TLMAX2*TCDLT2).
  
  char Dummy[30];
  fits_read_keyword(m_File, "TCRVL1", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TCRVL1 keyword..."<<show;
    return false;
  }  
  xCenterValue = atof(Dummy);
  //cout<<"xCenterValue: "<<xCenterValue<<endl;
  
  fits_read_keyword(m_File, "TCDLT1", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve TCDLT1 keyword..."<<show;
	return false;
  }
  xDelta = -fabs(atof(Dummy));
  //cout<<"xDelta: "<<xDelta<<endl;
  
  fits_read_keyword(m_File, "TCRVL2", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TCRVL2 keyword..."<<show;
    return false;
  }  
  yCenterValue = atof(Dummy);
  //cout<<"yCenterValue: "<<yCenterValue<<endl;
  
  fits_read_keyword(m_File, "TCDLT2", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve CDELT2 keyword..."<<show;
	return false;
  }
  yDelta = fabs(atof(Dummy));
  
  fits_read_keyword(m_File, "TLMAX1", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TLMAX1 keyword..."<<show;
    return false;
  }  
  float xMax = ceil(atof(Dummy));
  
  fits_read_keyword(m_File, "TLMAX2", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve TLMAX2 keyword..."<<show;
	return false;
  }
  float yMax = ceil(atof(Dummy));
    
  AvgDec = yCenterValue+yMax*0.5*yDelta;
  MaxRa = xCenterValue+xMax*xDelta/cos(AvgDec/rad);
  
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
 
  // Retrieve the dimensions:
  long AxisDimension[2];  
  fits_read_keys_lng(m_File, "NAXIS", 1, 2, AxisDimension, &nfound, &Status);
  if (Status != 0 || AxisDimension[1] == 0) {
    cout<<"The image in the fits file has no dimensions..."<<endl;
    return false;
  }
  axis1 = AxisDimension[0];
  axis2 = AxisDimension[1];
  
  //cout<<"axis1 ="<<axis1<<" axis2="<<axis2<<endl;

  return true;
}

/****************************************************************************/

bool ExposureMap::LoadEvtData()
{

  int Status = 0;
  float floatnull;
  double doublenull;
  int anynull;

  double fT[axis2];
  float fQx[axis2];
  float fQy[axis2];
  float fQz[axis2];
  float fQr[axis2];
  float fTx[axis2];
  float fTy[axis2];
  float fTz[axis2];
  float fQSx[axis2];
  float fQSy[axis2];
  float fQSz[axis2];
  float fQSr[axis2];
  
  fits_read_col(m_File, TDOUBLE, 5, 1, 1, axis2, &doublenull, fT, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 8, 1, 1, axis2, &floatnull, fQx, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 9, 1, 1, axis2, &floatnull, fQy, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 10, 1, 1, axis2, &floatnull, fQz, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 11, 1, 1, axis2, &floatnull, fQr, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 12, 1, 1, axis2, &floatnull, fTx, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 13, 1, 1, axis2, &floatnull, fTy, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 14, 1, 1, axis2, &floatnull, fTz, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 15, 1, 1, axis2, &floatnull, fQSx, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 16, 1, 1, axis2, &floatnull, fQSy, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 17, 1, 1, axis2, &floatnull, fQSz, &anynull,&Status);
  fits_read_col(m_File, TFLOAT, 18, 1, 1, axis2, &floatnull, fQSr, &anynull,&Status);
  
  fits_close_file(m_File, &Status); 

  NTime timedummy;
  NOrientation dummy;
  NOrientation dummystar;
  NObservatoryData Obs;
  for (int i=0;i<axis2;i++){
    dummy.SetTranslation((double)fTx[i],(double)fTy[i],(double)fTz[i]);
	dummy.SetRotation((double)fQx[i],(double)fQy[i],(double)fQz[i],(double)fQr[i]);
    Obs.SetOrientationFocalPlaneToOB(dummy);
	dummystar.SetRotation((double)fQSx[i],(double)fQSy[i],(double)fQSz[i],(double)fQSr[i]);
    Obs.SetOrientationOBToIS(dummystar);
	timedummy.Set((double) fT[i]);
	Obs.SetTime(timedummy);
	MastData.push_back(Obs);
  }
  
  return true;
}
/****************************************************************************/

bool ExposureMap::WriteExposureMap(int index)
{
   
	m_File = 0;
    int Status = 0;
    long  fpixel, nelements;
	float *array[SkyMapY.size()];
		
    /* initialize FITS image parameters */
    char FileName[]="!data.exposuremap.fit";

	int bitpix   =  FLOAT_IMG;
	long naxis    =   2;  /* 2-dimensional image                            */    
    long naxes[2] = { SkyMapX.size(), SkyMapY.size() };   /* image is 300 pixels wide by 200 rows */

    array[0] = (float *)malloc( naxes[0] * naxes[1]
                                        * sizeof( float ) );

    for(int ii=1; ii<naxes[1]; ii++ )
      array[ii] = array[ii-1] + naxes[0];

     for (int jj = 0; jj < naxes[1]; jj++)
    {   for (int ii = 0; ii < naxes[0]; ii++)
        {
            array[jj][ii] = SkyMapZ[ii + jj*naxes[0]];
        }
    }


    fits_create_file(&m_File, FileName, &Status); /* create new FITS file */
    if (Status != 0) {
      cout<<"Unable to open file: "<<endl;
      cout<<FileName<<endl;
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
	
	/*fits_write_key(m_File, TSTRING, "MFORM1", mform1," ", &Status);
	fits_write_key(m_File, TSTRING, "MTYPE1", mtype1," ", &Status);
	fits_write_key(m_File, TSTRING, "TCTYP1P", tctyp1p," ", &Status);
	fits_write_key(m_File, TSTRING, "WCSTYP1", wcsty," ", &Status);
    fits_write_key(m_File, TFLOAT, "TCDLT1P", &one, "", &Status); 	  
    fits_write_key(m_File, TFLOAT, "TCVLT1P",&SkyMapX[SkyMapX.size()],"",&Status);
    fits_write_key(m_File, TFLOAT, "TCRPX1P", &SkyMapX[0], "Pixel reference point", &Status); 	  

	fits_write_key(m_File, TSTRING, "MFORM2", mform2," ", &Status);
	fits_write_key(m_File, TSTRING, "MTYPE2", mtype2," ", &Status);
	fits_write_key(m_File, TSTRING, "TCTYP2P", tctyp2p," ", &Status);
	fits_write_key(m_File, TSTRING, "WCSTYP2", wcsty," ", &Status);
	fits_write_key(m_File, TFLOAT, "TCDLT2P", &one, "", &Status); 	  
    fits_write_key(m_File, TFLOAT, "TCVLT2P",&SkyMapY[SkyMapY.size()],"",&Status);
    fits_write_key(m_File, TFLOAT, "TCRPX2P", &SkyMapY[0], "Pixel reference point", &Status); */	  

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

/****************************************************************************/

bool ExposureMap::MakeDetMap()
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


/****************************************************************************/

bool ExposureMap::ProjectOnSky(float dt)
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

/****************************************************************************/

MVector ExposureMap::ToSkyPix(MVector Coords)
{
  
  MVector pix;
  // find the index that the corner falls in.
  // Doing this analytically for speed
  pix[0] = floor((Coords[0]-xCenterValue)/xDelta);
  pix[1] = floor((Coords[1]-xCenterValue)/xDelta);
  
  return pix;
}

/****************************************************************************/
bool ExposureMap::ExposeSky(int index)
{

  int hit = 0;
  NOrientation Rfbob;
  NOrientation Qstar;
  MVector FM;
  NOrientation FPM;
  FPM = IdealAlignments.GetOrientationFocalPlaneModule(1);
  FM = IdealAlignments.GetOrientationOpticsRelOpticalBench(1).GetTranslation();
    
  for (int i=0;i<(int)MastData.size()-1;i++){
  //for (int i=0;i< 10;i++){

	Rfbob=MastData[i].GetOrientationFocalPlaneToOB();
    Qstar=MastData[i].GetOrientationOBToIS();

    float dt = MastData[i+1].GetTime().GetAsSeconds()-MastData[i].GetTime().GetAsSeconds();
	
	DetHold.Clear();
	DetHold.AddVertice(20.25,20.25);
	DetHold.TransformMapOut(FPM);
	DetHold.TransformMapIn(Rfbob);
    DetHold.Bench2Sky(Qstar,FM, MaxRa, AvgDec);
	Corner = ToSkyPix(DetHold.GetCorner());
	
	// fill in first entry
	if (i==0){
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
	// And if it hasnt then add it to transform
	if (hit == 0) {
	  //cout<<Corner<<endl;
	  TransformIndex.push_back(i);  // first index occurance of transform
	  TotalTime.push_back(dt);
	  Skypix.push_back(Corner);
	}
  }
  for (int i=0;i<(int)TransformIndex.size();i++){
  	//cout<<TotalTime[i]<<endl;
	//cout<<TransformIndex[i]<<endl;
	//Now do the exposure map calculation
	Rfbob=MastData[TransformIndex[i]].GetOrientationFocalPlaneToOB();
    Qstar=MastData[TransformIndex[i]].GetOrientationOBToIS();

	MakeDetMap();
	Det1.TransformMapOut(FPM);
	Det2.TransformMapOut(FPM);
	Det3.TransformMapOut(FPM);
	Det4.TransformMapOut(FPM);

	Det1.TransformMapIn(Rfbob);
	Det2.TransformMapIn(Rfbob);
	Det3.TransformMapIn(Rfbob);
	Det4.TransformMapIn(Rfbob);
	  
	Det1.Bench2Sky(Qstar,FM,MaxRa, AvgDec);
	Det2.Bench2Sky(Qstar,FM,MaxRa, AvgDec);
	Det3.Bench2Sky(Qstar,FM,MaxRa, AvgDec);
	Det4.Bench2Sky(Qstar,FM,MaxRa, AvgDec);
		
	ProjectOnSky(TotalTime[i]);
	Det1.Clear();
	Det2.Clear();
	Det3.Clear();
	Det4.Clear();
  }
	
  cout<<"Write Map ...."<<endl;
  WriteExposureMap(index);

  // Clear arrays
  SkyMapZ.clear();
  TransformIndex.clear();
  TotalTime.clear();
  Skypix.clear();

  
  return true;

}

/****************************************************************************/


bool ExposureMap::ReadCalibratedAlignmentsDB(TString FileName)
{
  //! Read the data from file
  //! For the time being this is identical with the ReadPerturbedAlignmentsDB(TString FileName)
  //! But I don't expect it to stay this way since the detector orientations are missing, 
  //! and I would out which more stuff in here...

  // Load from file
  //MFile::ExpandFileName(FileName);
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
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

char ExposureMap::FindDelimeter(ifstream& in)
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


int main(int argc, char** argv)
{
  // Main function... the beginning...

  ExposureMap Map1;
  
  Map1.ReadCalibratedAlignmentsDB("/users/nusim/soft/NuSim/svn/resource/data/AlignmentDatabases/NuSIM_OrientationsALIGN_008.csv");
  
  Map1.OpenEvtFile("/users/nusim/soft/NuSim/svn/GalacticCenter.events.fits");
  //Map1.OpenEvtFile(argv[1]);
  
  cout<<"Reading in file...."<<endl;
  //Map1.OpenEvtFile("/Users/kristin/data/nusim/nusim/test.events.fits");
  Map1.LoadEvtData();
  cout<<"Calculating Exposure Map...."<<endl;
  Map1.ExposeSky(1);
  //detectors are clocked the same way
  //Map1.ExposeSky(2);
  
  cout<<"Done!"<<endl;
  
   
  return 0;
}


// Main: the end...
//////////////////////////////////////////////////////////////////////////////////
