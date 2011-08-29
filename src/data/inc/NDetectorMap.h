/*
 * NDetectorMap.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NDetectorMap__
#define __NDetectorMap__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NOrientation.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NDetectorMap
{
  // public interface:
 public:
  //! Standard constructor
  NDetectorMap();
  //! Copy constructor
  NDetectorMap(const NDetectorMap& Map);
  //! Default destructor
  virtual ~NDetectorMap();

  void Clear() { fX.clear(); fY.clear(); fZ.clear(); }

  int IsInside(double x, double y);
  bool AddVertice(double x, double y);
  bool TransformMapIn(NOrientation& R);
  bool TransformMapOut(NOrientation& R);
  bool Bench2Sky(NOrientation&, MVector& FM, float& MaxRa, float& AvgDec);
  bool ToRaDeg(float& MaxRa, float& AvgDec);
  void ToScreen();
  MVector GetCorner() { return MVector(fX[0],fY[0],fZ[0]); }


  // protected methods:
 protected:
  //NDetectorMap() {};
  //NDetectorMap(const NDetectorMap& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  vector<float> fX;
  vector<float> fY;
  vector<float> fZ;



#ifdef ___CINT___
 public:
  ClassDef(NDetectorMap, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
