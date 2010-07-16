/*
 * NModuleInterfaceGeometryAndDetectorProperties.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceGeometryAndDetectorProperties__
#define __NModuleInterfaceGeometryAndDetectorProperties__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MResponseMatrixO1.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NInteraction.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface provides the base functionality for retrieving all detector properties
//! This is a special decorator for the satellite module. It provides function to retrieve all important detector properties

class NModuleInterfaceGeometryAndDetectorProperties
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceGeometryAndDetectorProperties();
  //! Default destructor
  virtual ~NModuleInterfaceGeometryAndDetectorProperties();

  //! Get the half dimensions of the SENSITIVE area of the detectors
  virtual MVector GetDetectorHalfDimension() { return m_HalfDimension; }
  //! Get the detector pixels in x-direction (detector coordinate system)
  virtual int GetDetectorPixelsX() { return m_PixelsX; }
  //! Get the detector pixels in y-direction (detector coordinate system)
  virtual int GetDetectorPixelsY() { return m_PixelsY; }

  //! Get the detector pixel size in x-direction (detector coordinate system)
  virtual double GetDetectorAveragePixelSizeX() { return 2.0*GetDetectorHalfDimension().X()/GetDetectorPixelsX(); }
  //! Get the detector pixel size in y-direction (detector coordinate system)
  virtual double GetDetectorAveragePixelSizeY() { return 2.0*GetDetectorHalfDimension().Y()/GetDetectorPixelsY(); }

  //! Get the distance to closest neighboring pixel
  virtual double GetDistanceToClostestNeighborPixel(int Telescope, int Detector, MVector Position);
  //! Create an interaction in the clostest neighboring pixel
  virtual NInteraction CreateInteractionClostestNeighborPixel(int Telescope, int Detector, MVector Position);

  //! Get the radius of a sphere surriounding on optics module
  virtual double GetOpticsSurroundingSphereRadius() { return m_SurroundingSphere; }

  //! Load the cross sections (during initilize)
  bool LoadCrossSections();

  //! Return the total absorption coefficient (= macroscopic cross section)
  virtual double GetCZTAbsorptionCoefficient(double Energy) { return 0.1*m_TotalCrossSectionCZT.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the photo absorption coefficient (= macroscopic cross section)
  virtual double GetCZTPhotoAbsorptionCoefficient(double Energy) { return 0.1*m_PhotoCrossSectionCZT.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the Compton scattering coefficient (= macroscopic cross section)
  virtual double GetCZTComptonAbsorptionCoefficient(double Energy) { return 0.1*m_ComptonCrossSectionCZT.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the pair absoption coefficient (= macroscopic cross section)
  virtual double GetCZTPairAbsorptionCoefficient(double Energy) { return 0.1*m_PairCrossSectionCZT.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the Rayleigh scattering coefficient (= macroscopic cross section)
  virtual double GetCZTRayleighAbsorptionCoefficient(double Energy) { return 0.1*m_RayleighCrossSectionCZT.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm

  //! Return the total absorption coefficient (= macroscopic cross section)
  virtual double GetBerylliumAbsorptionCoefficient(double Energy) { return 0.1*m_TotalCrossSectionBeryllium.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the photo absorption coefficient (= macroscopic cross section)
  virtual double GetBerylliumPhotoAbsorptionCoefficient(double Energy) { return 0.1*m_PhotoCrossSectionBeryllium.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the Compton scattering coefficient (= macroscopic cross section)
  virtual double GetBerylliumComptonAbsorptionCoefficient(double Energy) { return 0.1*m_ComptonCrossSectionBeryllium.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the pair absoption coefficient (= macroscopic cross section)
  virtual double GetBerylliumPairAbsorptionCoefficient(double Energy) { return 0.1*m_PairCrossSectionBeryllium.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm
  //! Return the Rayleigh scattering coefficient (= macroscopic cross section)
  virtual double GetBerylliumRayleighAbsorptionCoefficient(double Energy) { return 0.1*m_RayleighCrossSectionBeryllium.GetInterpolated(Energy); } // 0.1 = 1/cm -> 1/mm



  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The half dimensions of one detectors
  MVector m_HalfDimension;
  //! The detector pixels in x-direction (detector coordinate system)
  int m_PixelsX;
  //! The detector pixels in y-direction (detector coordinate system)
  int m_PixelsY;

  //! Radius of urounding sphere for an optics module
  double m_SurroundingSphere;

  // private members:
 private:
  //! The total macroscopic cross section
  MResponseMatrixO1 m_TotalCrossSectionCZT;
  //! The photo effect macroscopic cross section
  MResponseMatrixO1 m_PhotoCrossSectionCZT;
  //! The Compton scattering macroscopic cross section
  MResponseMatrixO1 m_ComptonCrossSectionCZT;
  //! The pair creation macroscopic cross section
  MResponseMatrixO1 m_PairCrossSectionCZT;
  //! The Rayleigh scattering macroscopic cross section
  MResponseMatrixO1 m_RayleighCrossSectionCZT;

  //! The total macroscopic cross section
  MResponseMatrixO1 m_TotalCrossSectionBeryllium;
  //! The photo effect macroscopic cross section
  MResponseMatrixO1 m_PhotoCrossSectionBeryllium;
  //! The Compton scattering macroscopic cross section
  MResponseMatrixO1 m_ComptonCrossSectionBeryllium;
  //! The pair creation macroscopic cross section
  MResponseMatrixO1 m_PairCrossSectionBeryllium;
  //! The Rayleigh scattering macroscopic cross section
  MResponseMatrixO1 m_RayleighCrossSectionBeryllium;


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceGeometryAndDetectorProperties, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
