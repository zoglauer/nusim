/*
 * NExposureMap.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NExposureMap__
#define __NExposureMap__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"
#include "NQuaternion.h"
#include "NDetectorMap.h"
#include "NObservatoryData.h"
#include "NSatellite.h"

// Fits:
#include <fitsio.h>

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NExposureMap
{
  // public interface:
 public:
  //! Standard constructor
  NExposureMap(NSatellite& Satellite);
  //! Default destructor
  virtual ~NExposureMap();

  //! Set the file name (excluding the ".exposuremap.fits") which is automatically attached
  void SetFileNamePrefix(TString FileNamePrefix);

  //! Set all relevant image parameters from the original fits file
  void SetImageParameters(float TCRVL1, float TCDLT1, float TLMAX1, float TCRVL2, float TCDLT2, float TLMAX2);

  //! Add one mast information
  void AddObservatoryData(const NQuaternion& Qfbob, const MVector& Tfbob, const NQuaternion& Qstar, const NTime& Time);

  //! Calculate the exposure map
  bool ExposeSky(int index);

  //! Write the exposure map to file
  bool WriteExposureMap(int index);


	  
protected:
  bool MakeDetMap();
  // bool ReadCalibratedAlignmentsDB(TString FileName);
  MVector ToSkyPix(MVector Coords);
  bool ProjectOnSky(float dt);
  
  fitsfile* m_File; 
  // char FindDelimeter(ifstream& in);
  
private:
 

  /// Simulation file name
  TString m_FitsFileName;
  /// Output file name
  TString m_OutputFileName;
  

  // protected methods:
 protected:
  //NExposureMap() {};
  //NExposureMap(const NExposureMap& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The reference to the satellite
  NSatellite& m_Satellite;
 
  //! The output file name
  TString m_FileName;
 
  vector<NObservatoryData> MastData; 
  vector<NObservatoryData> CHU4Data;
  vector<unsigned int> m_TelescopeID;
  //NAlignmentsDBEntry IdealAlignments;
  
  /// RA in degree
 long axis1;
 long axis2;
 
 NDetectorMap Det1;
 NDetectorMap Det2;
 NDetectorMap Det3;
 NDetectorMap Det4;
 NDetectorMap DetHold;
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


#ifdef ___CINT___
 public:
  ClassDef(NExposureMap, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
