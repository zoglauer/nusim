/******************************************************************************
 *                                                                            *
 * NSource.hh                                                                 *
 *                                                                            *
 * Copyright (C) by Andreas Zoglauer.                                         *
 * All rights reserved.                                                       *
 *                                                                            *
 * Please see the file Licence.txt for further copyright information.         *
 *                                                                            *
 ******************************************************************************/


/******************************************************************************
 *
 * This class represents a source with all its basic data and in addition
 * it can create generate the start conditions for the event
 *
 */

#ifndef ___NSource___
#define ___NSource___

// ROOT:
#include "TF1.h"

// NuSIM:
#include "NPhoton.h"
#include "NSatellite.h"
#include "NTime.h"

// Standard libs:
#include <vector>
#include <list>
#include <iostream>
using namespace std;

// MEGAlib:
#include "MTokenizer.h"
#include "MFunction.h"
#include "MFunction2D.h"
#include "MFunction3DSpherical.h"
#include "MXmlNode.h"


/******************************************************************************/

class NSource
{
  // public interface:
public:
  //! Construction
  NSource(NSatellite& Satellite);
  //! Default destructor
  virtual ~NSource();
  
  //! Common initalization
  void Initialize();
  
  //! Reset some common data
  void Reset();

  //! Return the number of generated particles
  long GetNGeneratedParticles() const { return m_NGeneratedParticles; }

  //! Set the name of this source
  void SetName(TString Name) { m_Name = Name; }
  //! Return the name of this source
  TString GetName() const { return m_Name; }

  //! Calculate the next emission of this source
  bool CalculateNextEmission(NTime Time);

  //! Return the time to the next photon emission of this source:
  NTime GetNextEmission() const { return m_NextEmission; }

  //! Peform a time jump
  void PerformTimeJump(const NTime& Jump) { m_NextEmission += Jump; }

  //! Return true, if this source has still photons for the emission
  bool IsActive() const { return m_IsActive; }
  //! Return true, if this source has still photons for the emission
  void IsActive(bool Active) { m_IsActive = Active; }

  //! Return true, if the spectral type could be set correctly
  bool SetSpectralType(const int& SpectralType);
  //! Return the spectral type
  int GetSpectralType() const { return m_SpectralType; }

  //! Return the name of a spectral type
  static TString GetSpectralTypeName(const int SpectralType);
  //! Return the ID of a spectral type
  static int GetSpectralType(TString Name);

  /// Return true, if the light curve type could be set correctly
  bool SetLightCurveType(const int& LightCurveType);
  /// Return the light curve type
  int GetLightCurveType() const { return m_LightCurveType; }

  /// Return true, if the light curve could be set correctly
  bool SetLightCurve(const TString& FileName, const bool& Repeats);
  //! Return the energy file name
  TString GetLightCurveFileName() const { return m_LightCurveFileName; }

  //! Set if the light curve is looping
  void SetIsRepeatingLightCurve(const bool& IsRepeatingLightCurve) { m_IsRepeatingLightCurve = IsRepeatingLightCurve; }
  //! Return if the light curve is looping
  bool GetIsRepeatingLightCurve() const { return m_IsRepeatingLightCurve; }
  
  
  //! Return the name of a light curve type
  static TString GetLightCurveTypeName(const int LightCurveType);
  //! Return the ID of a light curve type
  static int GetLightCurveType(TString Name);


  //! Retrieve energy parameter 1
  double GetEnergyParameter1() const { return m_EnergyParam1; }
  //! Set energy parameter 1
  void SetEnergyParameter1(double E) { m_EnergyParam1 = E; }
  //! Retrieve energy parameter 2
  double GetEnergyParameter2() const { return m_EnergyParam2; }
  //! Set energy parameter 2
  void SetEnergyParameter2(double E) { m_EnergyParam2 = E; }
  //! Retrieve energy parameter 3
  double GetEnergyParameter3() const { return m_EnergyParam3; }
  //! Set energy parameter 3
  void SetEnergyParameter3(double E) { m_EnergyParam3 = E; }
  //! Retrieve energy parameter 4
  double GetEnergyParameter4() const { return m_EnergyParam4; }
  //! Set energy parameter 4
  void SetEnergyParameter4(double E) { m_EnergyParam4 = E; }
  //! Retrieve energy parameter 5
  double GetEnergyParameter5() const { return m_EnergyParam5; }
  //! Set energy parameter 5
  void SetEnergyParameter5(double E) { m_EnergyParam5 = E; }
  //! Retrieve energy parameter 6
  double GetEnergyParameter6() const { return m_EnergyParam6; }
  //! Set energy parameter 6
  void SetEnergyParameter6(double E) { m_EnergyParam6 = E; }
  //! Retrieve energy parameter 7
  double GetEnergyParameter7() const { return m_EnergyParam7; }
  //! Set energy parameter 7
  void SetEnergyParameter7(double E) { m_EnergyParam7 = E; }

  //! Return true, if the region type could be set correctly
  bool SetBeamType(const int& BeamType);
  //! Return the region type
  int GetBeamType() const { return m_BeamType; }

  //! Retrieve position parameter 1
  double GetPositionParameter1() const { return m_PositionParam1; }
  //! Set position parameter 1
  void SetPositionParameter1(double E) { m_PositionParam1 = E; }
  //! Retrieve position parameter 2
  double GetPositionParameter2() const { return m_PositionParam2; }
  //! Set position parameter 2
  void SetPositionParameter2(double E) { m_PositionParam2 = E; }
  //! Retrieve position parameter 3
  double GetPositionParameter3() const { return m_PositionParam3; }
  //! Set position parameter 3
  void SetPositionParameter3(double E) { m_PositionParam3 = E; }
  //! Retrieve position parameter 4
  double GetPositionParameter4() const { return m_PositionParam4; }
  //! Set position parameter 4
  void SetPositionParameter4(double E) { m_PositionParam4 = E; }
  //! Retrieve position parameter 5
  double GetPositionParameter5() const { return m_PositionParam5; }
  //! Set position parameter 5
  void SetPositionParameter5(double E) { m_PositionParam5 = E; }
  //! Retrieve position parameter 6
  double GetPositionParameter6() const { return m_PositionParam6; }
  //! Set position parameter 6
  void SetPositionParameter6(double E) { m_PositionParam6 = E; }
  //! Retrieve position parameter 7
  double GetPositionParameter7() const { return m_PositionParam7; }
  //! Set position parameter 7
  void SetPositionParameter7(double E) { m_PositionParam7 = E; }
  //! Retrieve position parameter 8
  double GetPositionParameter8() const { return m_PositionParam8; }
  //! Set position parameter 8
  void SetPositionParameter8(double E) { m_PositionParam8 = E; }
  //! Retrieve position parameter 9
  double GetPositionParameter9() const { return m_PositionParam9; }
  //! Set position parameter 9
  void SetPositionParameter9(double E) { m_PositionParam9 = E; }
  //! Retrieve position parameter 10
  double GetPositionParameter10() const { return m_PositionParam10; }
  //! Set position parameter 10
  void SetPositionParameter10(double E) { m_PositionParam10 = E; }
  //! Retrieve position parameter 11
  double GetPositionParameter11() const { return m_PositionParam11; }
  //! Set position parameter 11
  void SetPositionParameter11(double E) { m_PositionParam11 = E; }

  //! Return the name of a beam type
  static TString GetBeamTypeName(const int BeamType);
  //! Return the ID of a beam type
  static int GetBeamType(TString Name);

  //! Return true, if the intensity could be set correctly
  bool SetFlux(const double& Flux);
  //! Return the intensity
  double GetFlux() const { return m_InputFlux; } 

  //! Return true if the combined normalized energy-beam-flux-function could be set
  bool SetNormalizedEnergyPositionFluxFunction(TString FileName);
  //! Return the energy file name
  TString GetNormalizedEnergyPositionFluxFileName() { return m_NormalizedEnergyPositionFluxFileName; }

  //! Return true, if the position vector could be set correctly
  bool SetPosition(double PositionParam1 = 0.0, 
                   double PositionParam2 = 0.0, 
                   double PositionParam3 = 0.0, 
                   double PositionParam4 = 0.0, 
                   double PositionParam5 = 0.0, 
                   double PositionParam6 = 0.0, 
                   double PositionParam7 = 0.0, 
                   double PositionParam8 = 0.0, 
                   double PositionParam9 = 0.0, 
                   double PositionParam10 = 0.0, 
                   double PositionParam11 = 0.0);
  //! Return true, if the file containing the beam could be set correctly
  bool SetPosition(TString FileName);
  //! Return the energy file name
  TString GetPositionFileName() { return m_PositionFileName; }

  //! Return true, if the energy vector could be set correctly
  bool SetEnergy(double EnergyParam1 = 0.0, 
                 double EnergyParam2 = 0.0, 
                 double EnergyParam3 = 0.0, 
                 double EnergyParam4 = 0.0, 
                 double EnergyParam5 = 0.0, 
                 double EnergyParam6 = 0.0,
                 double EnergyParam7 = 0.0);
  //! Return true, if the file containing the spectrum could be set correctly
  bool SetEnergy(const TString& FileName); 
  //! Return the energy file name
  TString GetEnergyFileName() const { return m_EnergyFileName; }
  //! Set the energy function string
  bool SetEnergyFunctionString(const TString& EnergyFunctionString);
  //! Return the energy function string
  TString GetEnergyFunctionString() const { return m_EnergyFunctionString; }

  //! Generate an inititial data of the photon
  bool Generate(NPhoton& Photon, int Telescope);


  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  // This is NOT a universal parser - just a quick hack to convert some data
  bool ParseLine(TString Line);


  //! Id of an invalid type
  static const int c_Invalid; 

  //! Id of a spherical coordinate system
  static const int c_FarField;
  //! Id of a cartesian coordinate system
  static const int c_NearField;


  //! Id of the first spectral type
  static const int c_FirstSpectralType;
  //! Id of the last spectral type
  static const int c_LastSpectralType;

  //! Id of a monoenergetic energy distribution
  static const int c_Monoenergetic; 
  //! Id of a linear energy distribution
  static const int c_Linear; 
  //! Id of a power law energy distribution
  static const int c_PowerLaw; 
  //! Id of a broken power law energy distribution
  static const int c_BrokenPowerLaw; 
  //! Id of a Gaussian energy distribution
  static const int c_Gaussian; 
  //! Id of a thermal bremsstrahlung distribution: 1/E*exp(E/E_T)
  static const int c_ThermalBremsstrahlung; 
  //! Id of a black body distribution: E^2/(exp(E/E_T)-1)
  static const int c_BlackBody; 
  //! Id of a band function distribution
  static const int c_BandFunction; 
  //! Id of a Spectrum from file in format of a differential flux p/cm2/sec/keV
  static const int c_FileDifferentialFlux; 
  //! Id of an activation, i.e. decays
  static const int c_Activation;
  //! The ID of a normalized function in ph/cm2/s/keV
  static const int c_NormalizedFunctionInPhPerCm2PerSPerKeV;
  //! Id of a beam distribution from combined energy-position-flux-function
  static const int c_NormalizedEnergyPositionFluxFunction;

  //! Id of an unknown start area
  static const int c_StartAreaUnknown;
  //! Id of a spherical start area
  static const int c_StartAreaSphere;
  //! Id of a tubical start area 
  static const int c_StartAreaTube;


  //! Id of the first beam type
  static const int c_FirstBeamType;
  //! Id of the last beam type
  static const int c_LastBeamType;

  //! Id of a point like source in spherical coordinates
  static const int c_FarFieldPoint;
  //! Id of a area like source in spherical coordinates
  static const int c_FarFieldArea;
  //! Id of a area like source in spherical coordinates
  static const int c_FarFieldDisk;
  //! Id of a Gaussian like source in spherical coordinates
  static const int c_FarFieldGaussian;
  //! Id of a beam distribution from file in form of a zenith dependent distribution
  static const int c_FarFieldFile;
  //! Id of a beam distribution from file in form of a zenith dependent distribution
  static const int c_FarFieldFileZenithDependent;
  //! Id of a beam distribution from file in form of FITS file
  static const int c_FarFieldFitsFile;
  //! Id of a beam distribution from combined energy-position-flux-function
  static const int c_FarFieldNormalizedEnergyPositionFluxFunction;

  //! Id of a point like source in Cartesian coordinates
  static const int c_NearFieldPoint;
  //! Id of a beam in Cartesian coordinates
  static const int c_NearFieldRestrictedPoint;
  //! Id of a line like source in Cartesian coordinates
  static const int c_NearFieldLine;
  //! Id of a box like source in Cartesian coordinates
  static const int c_NearFieldBox;
  //! Id of a spherical like source in Cartesian coordinates
  static const int c_NearFieldSphere;
  //! Id of a disk like source in Cartesian coordinates
  static const int c_NearFieldDisk;
  //! Id of a beam in Cartesian coordinates
  static const int c_NearFieldBeam;
  //! Id of a beam in Cartesian coordinates with a 1D (radial) profile
  static const int c_NearFieldBeam1DProfile;
  //! Id of a beam in Cartesian coordinates with a 2D (map) profile
  static const int c_NearFieldBeam2DProfile;
  //! Id of a divergent beam (with homogeneous intensity)
  //! from a point in Cartesian coordinates
  static const int c_NearFieldConeBeam;
  //! Id of a divergent beam (with Gaussian intensity profile) 
  //! from a point in Cartesian coordinates
  static const int c_NearFieldConeBeamGauss;
  //! Id of a illuminated disk 
  static const int c_NearFieldIlluminatedDisk;
  //! Id of a illuminated box
  static const int c_NearFieldIlluminatedSquare;
  //! Id of a activation, i.e. decays evenly distributed in volumes
  static const int c_NearFieldActivation;
  //! Id of a radiating volume, i.e. the start positions are randomly distributed within the volume
  static const int c_NearFieldVolume;
  //! Id of a 2D structure emitting particles in isotropically in 3D
  static const int c_NearFieldFlatMap;


  //! Id of the first light-curve type
  static const int c_FirstLightCurveType;
  //! Id of the last light-curve type
  static const int c_LastLightCurveType;

  /// Id of a flat light curve, i.e. light curve not used 
  static const int c_LightCurveFlat;
  /// Id of a file based light curve
  static const int c_LightCurveFile;



  // protected methods:
protected:
  //! Upgrade the position parameters and do some sanity checks
  bool UpgradePosition();
  //! Upgrade the energy parameters and do some sanity checks
  bool UpgradeEnergy();
  //! Upgrade the energy parameters and do some sanity checks
  bool UpgradeFlux();
  /// Upgrade the energy parameters and do some sanity checks
  bool UpgradeLightCurve();
  //! Shape of black body emission (Temperature in keV)
  double BlackBody(double Energy, double Temperature) const; 
  //! Shape of a Band function
  double BandFunction(const double Energy, double Alpha, const double Beta, const double E0) const; 

  //! Check for $NUSIM paths in the given file name and replace it if possible
  TString CleanPath(TString Path);

  // protected members:
protected:

  //! Generate an inititial energy in the particle gun
  bool GenerateEnergy(NPhoton& Photon);
  //! Generate an inititial position in the particle gun
  bool GeneratePosition(NPhoton& Photon, int Telescope);


  // private members:
private:
  //! Name of this source
  TString m_Name;

  //! Refernce to the satellite
  NSatellite& m_Satellite;

  //! Time of the next emission
  NTime m_NextEmission;

  //! Id of the coordiante system
  int m_CoordinateSystem;
  //! Id of the spectral type
  int m_SpectralType;
  //! Id of the region type
  int m_BeamType;
  /// Id of the particle type
  int m_LightCurveType;

  //! Radius of the surrounding sphere
  double m_StartRadius;

  //! True if this source has still events to emit
  bool m_IsActive;

  //! Input flux --- not yet correctly normalized
  double m_InputFlux;
  //! Flux of this source
  double m_Flux;

  //! TotalEnergyFlux of this source
  double m_TotalEnergyFlux;
  
  /// The light curve
  MFunction m_LightCurveFunction;
  /// True if the light curve is repeating (i.e. pulsar vs. GRB)
  bool m_IsRepeatingLightCurve;
  /// The current light curve cycle
  unsigned int m_LightCurveCycle;
  /// The current light curve integration value
  double m_LightCurveIntegration;
  
  //! Number of generated particles
  long m_NGeneratedParticles;

  //! Parameter 1 of the position 
  double m_PositionParam1;
  //! Parameter 2 of the position 
  double m_PositionParam2;
  //! Parameter 3 of the position 
  double m_PositionParam3;
  //! Parameter 4 of the position 
  double m_PositionParam4;
  //! Parameter 5 of the position 
  double m_PositionParam5;
  //! Parameter 6 of the position 
  double m_PositionParam6;
  //! Parameter 7 of the position 
  double m_PositionParam7;
  //! Parameter 8 of the position 
  double m_PositionParam8;
  //! Parameter 9 of the position 
  double m_PositionParam9;
  //! Parameter 10 of the position 
  double m_PositionParam10;
  //! Parameter 11 of the position 
  double m_PositionParam11;

  //! A file containing the beam distribution
  MFunction m_PositionFunction;
  //! A file containing the beam distribution as 2D function
  MFunction2D m_PositionFunction2D;
  //! A ROOT TF1 function containing some auxiliary aspects of the position distribution
  //! e.g. for CartesianConeBeamGaus: the probability density function for the beam internal zenith angle
  TF1* m_PositionTF1;

  //! The file name of the energy functiomn
  TString m_PositionFileName;

  //! Parameter 1 of the energy 
  double m_EnergyParam1;
  //! Parameter 2 of the energy 
  double m_EnergyParam2;
  //! Parameter 3 of the energy 
  double m_EnergyParam3;
  //! Parameter 4 of the energy 
  double m_EnergyParam4;
  //! Parameter 5 of the energy 
  double m_EnergyParam5;
  //! Parameter 6 of the energy 
  double m_EnergyParam6;
  //! Parameter 7 of the energy 
  double m_EnergyParam7;

  //! A file containing the spectrum
  MFunction m_EnergyFunction;

  //! A file containing the spectrum
  TF1* m_EnergyTF1;

  //! A string containing the spectrum
  TString m_EnergyFunctionString;
  
  //! The file name of the energy function
  TString m_EnergyFileName;
  
  //! A file containing the spectrum
  TString m_NormalizedEnergyPositionFluxFileName;
  
  //! A file containing the spectrum
  MFunction3DSpherical m_NormalizedEnergyPositionFluxFunction;
  
  //! The file name of the light curve function
  TString m_LightCurveFileName;

};

#endif


/*
 * NSource.hh: the end...
 ******************************************************************************/
