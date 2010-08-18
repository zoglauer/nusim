/*
 * NModuleInterfaceGeometryAndDetectorProperties.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceGeometryAndDetectorProperties
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceGeometryAndDetectorProperties.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceGeometryAndDetectorProperties)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceGeometryAndDetectorProperties::NModuleInterfaceGeometryAndDetectorProperties()
{
  // Construct an instance of NModuleInterfaceGeometryAndDetectorProperties


  // Never forget: mm!
  m_HalfDimension = MVector(9.69, 9.69, 1.0);
  m_PixelsX = 32; // That's intentionally not correct 
  m_PixelsY = 32;

  m_SurroundingSphereRadius = 500.0;
  m_SurroundingSphereZOffsetInOpticsModuleCoordinates = 227.0;
  
  LoadCrossSections();
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceGeometryAndDetectorProperties::~NModuleInterfaceGeometryAndDetectorProperties()
{
  // Delete this instance of NModuleInterfaceGeometryAndDetectorProperties
}


////////////////////////////////////////////////////////////////////////////////


double NModuleInterfaceGeometryAndDetectorProperties::GetDistanceToClostestNeighborPixel(int Telescope, int Detector, MVector Position)
{
  //! Get the distance to closest neighboring pixel. Position is in detector

  // For the time being this information is not telescope/detector specific

  // Make position relative to -/- edge
  Position[0] += GetDetectorAveragePixelSizeX()*GetDetectorPixelsX()/2;
  Position[1] += GetDetectorAveragePixelSizeY()*GetDetectorPixelsY()/2;
  
  // Determine position in pixel
  while (Position[0] > GetDetectorAveragePixelSizeX()) {
    Position[0] -= GetDetectorAveragePixelSizeX();
  }
  while (Position[1] > GetDetectorAveragePixelSizeY()) {
    Position[1] -= GetDetectorAveragePixelSizeY();
  }

  // Make position relative to center
  Position[0] -= GetDetectorAveragePixelSizeX()/2;
  Position[1] -= GetDetectorAveragePixelSizeY()/2;

  if (fabs(Position[0]) > fabs(Position[1])) {
    return GetDetectorAveragePixelSizeX()/2 - fabs(Position[0]);
  } else {
    return GetDetectorAveragePixelSizeY()/2 - fabs(Position[1]);
  }
}


////////////////////////////////////////////////////////////////////////////////


NInteraction NModuleInterfaceGeometryAndDetectorProperties::CreateInteractionClostestNeighborPixel(int Telescope, int Detector, MVector Position)
{
  //! Create an interaction in the clostest neighboring pixel

  // For the time being this information is not telescope/detector specific

  MVector Pos = Position;
  
  // Make position relative to -/- edge
  Pos[0] += GetDetectorAveragePixelSizeX()*GetDetectorPixelsX()/2;
  Pos[1] += GetDetectorAveragePixelSizeY()*GetDetectorPixelsY()/2;
  
  // Determine position in pixel
  while (Pos[0] > GetDetectorAveragePixelSizeX()) {
    Pos[0] -= GetDetectorAveragePixelSizeX();
  }
  while (Pos[1] > GetDetectorAveragePixelSizeY()) {
    Pos[1] -= GetDetectorAveragePixelSizeY();
  }

  // Make position relative to center
  Pos[0] -= GetDetectorAveragePixelSizeX()/2;
  Pos[1] -= GetDetectorAveragePixelSizeY()/2;

  // Create a dummy position the same distance from the CLOSTEST edge as the original position
  NInteraction I;
  if (fabs(Pos[0]) > fabs(Pos[1])) {
    if (Pos[0] > 0) {
      Position[0] += 2*(GetDetectorAveragePixelSizeX()/2 - fabs(Pos[0]));
    } else {
      Position[0] -= 2*(GetDetectorAveragePixelSizeX()/2 - fabs(Pos[0]));
    }
  } else {
    if (Pos[1] > 0) {
      Position[1] += 2*(GetDetectorAveragePixelSizeY()/2 - fabs(Pos[1]));
    } else {
      Position[1] -= 2*(GetDetectorAveragePixelSizeY()/2 - fabs(Pos[1]));
    }
  }
  I.SetPosition(Position);

  return I;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceGeometryAndDetectorProperties::LoadCrossSections()
{
  // Load absorption file:

  bool CrossSectionsPresent = true;

  TString TotalAbs = "${NUSIM}/resource/data/Xsection.Total.CZT.rsp";
  if (MFile::FileExists(TotalAbs) == false) {
    cout<<"   ***  Warning  ***  in material CZT"<<endl;
    cout<<"Can't find cross section file "<<TotalAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_TotalCrossSectionCZT.Read(TotalAbs) == false) {
      cout<<"   ***  Error  ***  in material CZT"<<endl;
      cout<<"Can't open cross section file "<<TotalAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    }
  }

  TString PhotoAbs = "${NUSIM}/resource/data/Xsection.Photo.CZT.rsp";
  if (MFile::FileExists(PhotoAbs) == false) {
    cout<<"   ***  Warning  ***  in material CZT"<<endl;
    cout<<"Can't find cross section file "<<PhotoAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_PhotoCrossSectionCZT.Read(PhotoAbs) == false) {
      cout<<"   ***  Error  ***  in material CZT"<<endl;
      cout<<"Can't open cross section file "<<PhotoAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    }
  }

  TString ComptonAbs = "${NUSIM}/resource/data/Xsection.Compton.CZT.rsp";
  if (MFile::FileExists(ComptonAbs) == false) {
    cout<<"   ***  Warning  ***  in material CZT"<<endl;
    cout<<"Can't find cross section file "<<ComptonAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_ComptonCrossSectionCZT.Read(ComptonAbs) == false) {
      cout<<"   ***  Error  ***  in material CZT"<<endl;
      cout<<"Can't open cross section file "<<ComptonAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    } 
  }

  TString PairAbs = "${NUSIM}/resource/data/Xsection.Pair.CZT.rsp";
  if (MFile::FileExists(PairAbs) == false) {
    cout<<"   ***  Warning  ***  in material CZT"<<endl;
    cout<<"Can't find cross section file "<<PairAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_PairCrossSectionCZT.Read(PairAbs) == false) {
      cout<<"   ***  Error  ***  in material CZT"<<endl;
      cout<<"Can't open cross section file "<<PairAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    } 
  }

  TString RayleighAbs = "${NUSIM}/resource/data/Xsection.Rayleigh.CZT.rsp";
  if (MFile::FileExists(RayleighAbs) == false) {
    cout<<"   ***  Warning  ***  in material CZT"<<endl;
    cout<<"Can't find cross section file "<<RayleighAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_RayleighCrossSectionCZT.Read(RayleighAbs) == false) {
      cout<<"   ***  Error  ***  in material CZT"<<endl;
      cout<<"Can't open cross section file "<<RayleighAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    } 
  }
 
  TotalAbs = "${NUSIM}/resource/data/Xsection.Total.Beryllium.rsp";
  if (MFile::FileExists(TotalAbs) == false) {
    cout<<"   ***  Warning  ***  in material Beryllium"<<endl;
    cout<<"Can't find cross section file "<<TotalAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_TotalCrossSectionBeryllium.Read(TotalAbs) == false) {
      cout<<"   ***  Error  ***  in material Beryllium"<<endl;
      cout<<"Can't open cross section file "<<TotalAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    }
  }

  PhotoAbs = "${NUSIM}/resource/data/Xsection.Photo.Beryllium.rsp";
  if (MFile::FileExists(PhotoAbs) == false) {
    cout<<"   ***  Warning  ***  in material Beryllium"<<endl;
    cout<<"Can't find cross section file "<<PhotoAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_PhotoCrossSectionBeryllium.Read(PhotoAbs) == false) {
      cout<<"   ***  Error  ***  in material Beryllium"<<endl;
      cout<<"Can't open cross section file "<<PhotoAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    }
  }

  ComptonAbs = "${NUSIM}/resource/data/Xsection.Compton.Beryllium.rsp";
  if (MFile::FileExists(ComptonAbs) == false) {
    cout<<"   ***  Warning  ***  in material Beryllium"<<endl;
    cout<<"Can't find cross section file "<<ComptonAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_ComptonCrossSectionBeryllium.Read(ComptonAbs) == false) {
      cout<<"   ***  Error  ***  in material Beryllium"<<endl;
      cout<<"Can't open cross section file "<<ComptonAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    } 
  }

  PairAbs = "${NUSIM}/resource/data/Xsection.Pair.Beryllium.rsp";
  if (MFile::FileExists(PairAbs) == false) {
    cout<<"   ***  Warning  ***  in material Beryllium"<<endl;
    cout<<"Can't find cross section file "<<PairAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_PairCrossSectionBeryllium.Read(PairAbs) == false) {
      cout<<"   ***  Error  ***  in material Beryllium"<<endl;
      cout<<"Can't open cross section file "<<PairAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    } 
  }

  RayleighAbs = "${NUSIM}/resource/data/Xsection.Rayleigh.Beryllium.rsp";
  if (MFile::FileExists(RayleighAbs) == false) {
    cout<<"   ***  Warning  ***  in material Beryllium"<<endl;
    cout<<"Can't find cross section file "<<RayleighAbs<<endl;
    CrossSectionsPresent = false;
  } else {
    if (m_RayleighCrossSectionBeryllium.Read(RayleighAbs) == false) {
      cout<<"   ***  Error  ***  in material Beryllium"<<endl;
      cout<<"Can't open cross section file "<<RayleighAbs<<endl;
      CrossSectionsPresent = false;
      return false;
    } 
  }
   
  return CrossSectionsPresent;
}


// NModuleInterfaceGeometryAndDetectorProperties.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
