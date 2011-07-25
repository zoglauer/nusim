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
using namespace std;

// ROOT
#include <TString.h>


// NuSIM
#include <NQuaternion.h>
#include <NObservatorydata.h>
#include <NOrientation.h>

/****************************************************************************/
class DetMap
{

public:
  DetMap();
  ~DetMap();
  int IsInside(double x, double y);
  bool AddVertice(double x, double y);
  bool TransformMapIn(NOrientation& R);
  bool TransformMapOut(NOrientation& R);
  bool Bench2Sky(NOrientation&,MVector& FM);
  bool ToRaDeg();
  void Clear() {fX.clear();fY.clear();fZ.clear();}
  void ToScreen();
  
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
  bool ExposeSky(MVector& FM, NOrientation& R);
  bool ProjectOnSky();
  bool WriteExposureMap();
  vector <NObservatoryData> MastData; 
  vector <NObservatoryData> CHU4Data;  
protected:
  fitsfile* m_File; 

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
 vector <float> SkyMapX;
 vector <float> SkyMapY;
 vector <float> SkyMapZ;
 // keep track of whether a piece of 
 // sky already got exposed.
 vector <float> SkyExposed;

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

bool DetMap::Bench2Sky(NOrientation& R, MVector& FM)
{

  MVector v;
  
  for (int i=0; i<(int)fX.size();i++){
      v.SetXYZ(FM[0]-fX[i],FM[1]-fY[i],FM[2]-fZ[i]);
	  v.Unitize();
	  R.TransformIn(v);
	  fX[i] = v[0];
	  fY[i] = v[1];
      fZ[i] = v[2];	
  }
  //cout<<"OB land:"<<"Dx = "<<fX[0]<<" Dy = "<<fY[0]<<endl;
  return true;

}

/****************************************************************************/

bool DetMap::ToRaDeg()
{
   for (int i=0; i<(int)fX.size();i++){
	  fX[i] = atan2(fY[i],fX[i])*c_Deg;
	  if (fX[i] < 0.0) fX[i] += 180.0;
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
  double xCenterValue = atof(Dummy);
  //cout<<"xCenterValue: "<<xCenterValue<<endl;
  
  fits_read_keyword(m_File, "TCDLT1", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve TCDLT1 keyword..."<<show;
	return false;
  }
  double xDelta = -fabs(atof(Dummy));
  //cout<<"xDelta: "<<xDelta<<endl;
  
  fits_read_keyword(m_File, "TCRVL2", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TCRVL2 keyword..."<<show;
    return false;
  }  
  double yCenterValue = atof(Dummy);
  //cout<<"yCenterValue: "<<yCenterValue<<endl;
  
  fits_read_keyword(m_File, "TCDLT2", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve CDELT2 keyword..."<<show;
	return false;
  }
  double yDelta = fabs(atof(Dummy));
  //cout<<"yDelta: "<<yDelta<<endl;
  
  fits_read_keyword(m_File, "TLMAX1", Dummy, NULL, &Status);
  if (Status != 0) {
    cout<<"Cannot retrieve TLMAX1 keyword..."<<show;
    return false;
  }  
  double xMax = ceil(atof(Dummy));
  //cout<<"yCenterValue: "<<yCenterValue<<endl;
  
  fits_read_keyword(m_File, "TLMAX2", Dummy, NULL, &Status);
  if (Status != 0) {
	cout<<"Cannot retrieve TLMAX2 keyword..."<<show;
	return false;
  }
  double yMax = ceil(atof(Dummy));
  //cout<<"yDelta: "<<yDelta<<endl;

  //create sky map

  for (int x = 0; x <= xMax; ++x) SkyMapX.push_back(xCenterValue+x*xDelta);
  for (int y = 0; y <= yMax; ++y) SkyMapY.push_back(yCenterValue+y*yDelta);
  for (int x = 0; x <= xMax; ++x) {
    for (int y = 0; y <= yMax; ++y) {
    SkyMapZ.push_back(0);
    SkyExposed.push_back(0);
	}
  }
  cout<<SkyMapX[0]<<" "<<SkyMapX[xMax-1]<<endl;
  cout<<SkyMapY[0]<<" "<<SkyMapY[yMax-1]<<endl;
 
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
  int anynull;

  float fT[axis2];
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
  
  fits_read_col(m_File, TFLOAT, 5, 1, 1, axis2, &floatnull, fT, &anynull,&Status);
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

bool ExposureMap::WriteExposureMap()
{
   
	m_File = 0;
    int Status = 0;
    long  fpixel, nelements;
	unsigned short *array[SkyMapY.size()];
		
    /* initialize FITS image parameters */
    char FileName[] = "!data.exposuremap.fit";             /* name for new FITS file */
    int bitpix   =  USHORT_IMG; /* 16-bit unsigned short pixel values       */
    long naxis    =   2;  /* 2-dimensional image                            */    
    long naxes[2] = { SkyMapX.size(), SkyMapY.size() };   /* image is 300 pixels wide by 200 rows */

    array[0] = (unsigned short *)malloc( naxes[0] * naxes[1]
                                        * sizeof( unsigned short ) );

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
    fits_write_img(m_File, TUSHORT, fpixel, nelements, array[0], &Status);
  
    fits_close_file(m_File, &Status);                /* close the file */
	free( array[0] );  /* free previously allocated memory */
    
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

bool ExposureMap::ProjectOnSky()
{
 
  vector <float>::iterator itx, ity;
  
  Det1.ToRaDeg();
  Det2.ToRaDeg();
  Det3.ToRaDeg();
  Det4.ToRaDeg();

  for (int i=0; i<(int) SkyMapX.size();i++){
    for (int j=0; j<(int) SkyMapY.size();j++){
	  if (Det1.IsInside(SkyMapX[i], SkyMapY[j])) {
        SkyMapZ[i+SkyMapX.size()*j] += 1;
	    continue;  
	  }	
	  if (Det2.IsInside(SkyMapX[i], SkyMapY[j])) {
		SkyMapZ[i+SkyMapX.size()*j] += 1;
	    continue;  
	  }	
	  if (Det3.IsInside(SkyMapX[i], SkyMapY[j])) {
		SkyMapZ[i+SkyMapX.size()*j] += 1;
	    continue;  
	  }	
	  if (Det4.IsInside(SkyMapX[i], SkyMapY[j])) {
		SkyMapZ[i+SkyMapX.size()*j] += 1;
	    continue;  
	  }	
    }
  }

  return true;
 
}



/****************************************************************************/
bool ExposureMap::ExposeSky(MVector& FM, NOrientation& FPM)
{

  NOrientation Rfbob;
  NOrientation Qstar;
  for (int i=0;i<100;i++){
	MakeDetMap();
    Det1.TransformMapOut(FPM);
    Det2.TransformMapOut(FPM);
    Det3.TransformMapOut(FPM);
    Det4.TransformMapOut(FPM);
  
    Rfbob=MastData[i].GetOrientationFocalPlaneToOB();
    Qstar=MastData[i].GetOrientationOBToIS();

	Det1.TransformMapIn(Rfbob);
	Det2.TransformMapIn(Rfbob);
    Det3.TransformMapIn(Rfbob);
    Det4.TransformMapIn(Rfbob);

	Det1.Bench2Sky(Qstar,FM);
	Det2.Bench2Sky(Qstar,FM);
	Det3.Bench2Sky(Qstar,FM);
	Det4.Bench2Sky(Qstar,FM);
	
	ProjectOnSky();
	Det1.ToScreen();
	Det2.ToScreen();
	Det3.ToScreen();
	Det4.ToScreen();

	cout<<"event number = "<<i<<endl;
	Det1.Clear();
	Det2.Clear();
	Det3.Clear();
	Det4.Clear();
  }
  WriteExposureMap();
  
  return true;

}

/****************************************************************************/


int main(int argc, char** argv)
{
  // Main function... the beginning...

  ExposureMap Map1;
 
  NOrientation FPMA;
  NOrientation FPMB;
  MVector FM1(-250.825,218.4792524,11.61130798);
  MVector FM2(257.175,218.4792524,11.61130798);
  
  FPMA.SetTranslation(MVector(-250.8249974,411.1892622,421.75498));
  FPMA.SetRotation(0,0,0.866,0.5);
  FPMB.SetTranslation(MVector(257.1750026,411.1892622,421.75498));
  FPMB.SetRotation(0,0,0.866,0.5);
  
  Map1.OpenEvtFile("/Users/kristin/data/nusim/nusim/data.events.fits");
  Map1.LoadEvtData();
  Map1.ExposeSky(FM1, FPMA);
  
   
  return 0;
}


// Main: the end...
//////////////////////////////////////////////////////////////////////////////////
