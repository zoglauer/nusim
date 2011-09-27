/*
 * NDetectorMap.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NDetectorMap
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NDetectorMap.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NDetectorMap)
#endif


////////////////////////////////////////////////////////////////////////////////


NDetectorMap::NDetectorMap()
{
  // Construct an instance of NDetectorMap

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NDetectorMap::~NDetectorMap()
{
  // Delete this instance of NDetectorMap
}


////////////////////////////////////////////////////////////////////////////////


NDetectorMap::NDetectorMap(const NDetectorMap& map) 
{
  fX = map.fX;
  fY = map.fY;
  fZ = map.fZ;
}


////////////////////////////////////////////////////////////////////////////////


bool NDetectorMap::AddVertice(double x, double y)
{
  fX.push_back(x);
  fY.push_back(y);
  fZ.push_back(0);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NDetectorMap::ToScreen()
{
  for (int i=0; i< (int) fX.size(); i++) {
    cout<<"vertice "<<i<<": "<<fX[i]<<" "<<fY[i]<<endl;
  }
  cout<<"*************************"<<endl;
}


////////////////////////////////////////////////////////////////////////////////


int NDetectorMap::IsInside(double x, double y)
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


////////////////////////////////////////////////////////////////////////////////


bool NDetectorMap::TransformMapIn(NOrientation& R)
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


////////////////////////////////////////////////////////////////////////////////


bool NDetectorMap::TransformMapOut(NOrientation& R)
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


////////////////////////////////////////////////////////////////////////////////


bool NDetectorMap::Bench2Sky(NOrientation& R, MVector& FM, float& MaxRa, float& AvgDec)
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


////////////////////////////////////////////////////////////////////////////////


bool NDetectorMap::ToRaDeg(float& MaxRa, float& AvgDec)
{
   for (int i=0; i<(int)fX.size();i++){
	  fX[i] = atan2(fY[i],fX[i])*c_Deg;
	  if (fX[i] < 0.0) fX[i] += 360.0;
	  fX[i] = MaxRa+(fX[i]-MaxRa)*cos(AvgDec/rad);
	  fY[i] = asin(fZ[i])*c_Deg;
	  //if (fY[i] < 0.0) fY[i] += 180.0;
   }
   //cout<<fX[0]<<" "<<fY[0]<<endl;
   
   return true;
}



// NDetectorMap.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
