/******************************************************************************
 *                                                                            *
 * NSource.cc                                                                 *
 *                                                                            *
 * Copyright (C) by Andreas Zoglauer.                                         *
 * All rights reserved.                                                       *
 *                                                                            *
 * Please see the file Licence.txt for further copyright information.         *
 *                                                                            *
 ******************************************************************************/

// Cosima:
#include "NSource.h"

// Stdlib:
#include <limits>
using namespace std;

// ROOT:
#include "TRandom.h"
#include "TSystem.h"

// MEGAlib:
#include "MStreams.h"
#include "MAssert.h"
#include "MTokenizer.h"

// NuSIM:
#include "NExtractFitsImage.h"
#include "NModule.h"

/******************************************************************************/


const int NSource::c_Monoenergetic                = 1; 
const int NSource::c_Linear                       = 2; 
const int NSource::c_PowerLaw                     = 3; 
const int NSource::c_BrokenPowerLaw               = 4; 
const int NSource::c_FileDifferentialFlux         = 5; 
const int NSource::c_BlackBody                    = 6;
const int NSource::c_NormalizedFunctionInPhPerCm2PerSPerKeV  = 7;
const int NSource::c_NormalizedEnergyPositionFluxFunction = 8;

const int NSource::c_FirstSpectralType            = 1;
const int NSource::c_LastSpectralType             = 8;

const int NSource::c_Gaussian                     = 105; 
const int NSource::c_ThermalBremsstrahlung        = 106; 
const int NSource::c_Activation                   = 107; 
const int NSource::c_BandFunction                 = 108; 

const int NSource::c_LightCurveFlat               = 1;
const int NSource::c_LightCurveFile               = 2;

const int NSource::c_FirstLightCurveType          = 1;
const int NSource::c_LastLightCurveType           = 2;


const int NSource::c_FarField                     = 1;
const int NSource::c_NearField                    = 2;

const int NSource::c_FarFieldPoint                = 1;
const int NSource::c_FarFieldDisk                 = 2;
const int NSource::c_NearFieldPoint               = 3;
const int NSource::c_NearFieldBeam                = 4;
const int NSource::c_FarFieldFitsFile             = 5;
const int NSource::c_FarFieldNormalizedEnergyPositionFluxFunction = 6;

const int NSource::c_FirstBeamType                = 1;
const int NSource::c_LastBeamType                 = 6;


// Not used just kept since this is the original cosima code: 
const int NSource::c_NearFieldRestrictedPoint     = 100;
const int NSource::c_FarFieldFile                 = 105;
const int NSource::c_FarFieldGaussian             = 107;
const int NSource::c_FarFieldArea                 = 108;
const int NSource::c_FarFieldFileZenithDependent  = 109;
const int NSource::c_NearFieldLine                = 112;
const int NSource::c_NearFieldBox                 = 113;
const int NSource::c_NearFieldDisk                = 114;
const int NSource::c_NearFieldSphere              = 115;
const int NSource::c_NearFieldActivation          = 117;
const int NSource::c_NearFieldBeam1DProfile       = 118;
const int NSource::c_NearFieldBeam2DProfile       = 119;
const int NSource::c_NearFieldConeBeam            = 120;
const int NSource::c_NearFieldConeBeamGauss       = 121;
const int NSource::c_NearFieldIlluminatedDisk     = 122;
const int NSource::c_NearFieldIlluminatedSquare   = 123;
const int NSource::c_NearFieldVolume              = 124;
const int NSource::c_NearFieldFlatMap             = 125;

const int NSource::c_Invalid                      = -99999987;


/******************************************************************************
 * Construction
 */
NSource::NSource(NSatellite& Satellite) : m_Satellite(Satellite)
{
  Initialize();
}


/******************************************************************************
 * Construction by name
 */
void NSource::Initialize()
{
  m_Name = "A source";

  m_NextEmission = 0;

  m_NGeneratedParticles = 0;

  m_CoordinateSystem = c_Invalid;
  m_SpectralType = c_Invalid;
  m_BeamType = c_Invalid;
  m_LightCurveType = c_LightCurveFlat; // We have a default here since usually no light curve is given

  m_IsActive = true;

  // Intensity of this source
  m_InputFlux = c_Invalid;
  m_Flux = c_Invalid;
  
  // Light curve
  m_IsRepeatingLightCurve = false;
  m_LightCurveCycle = 0;
  m_LightCurveIntegration = 0.0;


  m_PositionParam1 = 0.0;
  m_PositionParam2 = 0.0;
  m_PositionParam3 = 0.0;
  m_PositionParam4 = 0.0;
  m_PositionParam5 = 0.0;
  m_PositionParam6 = 0.0;
  m_PositionParam7 = 0.0;
  m_PositionParam8 = 0.0;
  m_PositionParam9 = 0.0;
  m_PositionParam10 = 0.0;
  m_PositionParam11 = 0.0;

  m_EnergyParam1 = 0.0;
  m_EnergyParam2 = 0.0;
  m_EnergyParam3 = 0.0;
  m_EnergyParam4 = 0.0;
  m_EnergyParam5 = 0.0;
  m_EnergyParam6 = 0.0;

  m_EnergyFileName = "";
  m_EnergyFunctionString = "";

  m_PositionTF1 = 0;
  m_EnergyTF1 = 0;
}


/******************************************************************************
 * Construction by name
 */
void NSource::Reset()
{
  m_NextEmission = 0;
  m_NGeneratedParticles = 0;
  m_IsActive = true;
}


/******************************************************************************
 * Standard destructor
 */
NSource::~NSource()
{
  delete m_PositionTF1;
}


/******************************************************************************
 *  Return true, if the particle type could be set correctly
 */
bool NSource::SetSpectralType(const int& SpectralType)
{
  switch (SpectralType) {
  case c_Monoenergetic:
  case c_Linear:
  case c_PowerLaw:
  case c_BrokenPowerLaw:
  case c_Gaussian:
  case c_ThermalBremsstrahlung:
  case c_BlackBody:
  case c_BandFunction:
  case c_FileDifferentialFlux:
  case c_NormalizedFunctionInPhPerCm2PerSPerKeV:
  case c_NormalizedEnergyPositionFluxFunction:
    m_SpectralType = SpectralType;
    return true;
  default:
    mout<<m_Name<<": Unknown spectral type"<<show;
    return false;
  }

  return true;
}


/******************************************************************************
 * Return the name of a beam type
 */
TString NSource::GetSpectralTypeName(const int SpectralType)
{
  TString Name = "Unknown";

  switch (SpectralType) {
  case c_Monoenergetic:
    Name = "Mono energetic";
    break;
  case c_Linear:
    Name = "Linear";
    break;
  case c_PowerLaw:
    Name = "Power law";
    break;
  case c_BrokenPowerLaw:
    Name = "Broken power law";
    break;
  case c_Gaussian:
    Name = "Gaussian";
    break;
  case c_ThermalBremsstrahlung:
    Name = "Thermal bremsstrahlung";
    break;
  case c_BlackBody:
    Name = "Black body";
    break;
  case c_BandFunction:
    Name = "Band function";
    break;
  case c_FileDifferentialFlux:
    Name = "File with differential flux";
    break;
  case c_NormalizedFunctionInPhPerCm2PerSPerKeV:
    Name = "Normalized function in ph/cm2/s/keV";
    break;
  case c_NormalizedEnergyPositionFluxFunction:
    Name ="Normalized energy-position-flux function in ph/cm2/s/keV";
    break;
  default:
    mout<<"Unknown spectral type: "<<SpectralType<<endl;
    break;
  }

  return Name;
}


/******************************************************************************
 * Return the ID of a beam type
 */
int NSource::GetSpectralType(TString Name)
{
  int Type = c_Invalid;

  if (Name == "Mono energetic") {
    Type = c_Monoenergetic;
  } else if (Name == "Linear") {
    Type = c_Linear;
  } else if (Name == "Power law") {
    Type = c_PowerLaw;
  } else if (Name == "Broken power law") {
    Type = c_BrokenPowerLaw;
  } else if (Name == "Gaussian") {
    Type = c_Gaussian;
  } else if (Name == "Thermal bremsstrahlung") {
    Type = c_ThermalBremsstrahlung;
  } else if (Name == "Black body") {
    Type = c_BlackBody;
  } else if (Name == "Band function") {
    Type = c_BandFunction;
  } else if (Name == "File with differential flux") {
    Type = c_FileDifferentialFlux;
  } else if (Name == "Normalized function in ph/cm2/s/keV") {
    Type = c_NormalizedFunctionInPhPerCm2PerSPerKeV;
  } else if (Name == "Normalized energy-position-flux function in ph/cm2/s/keV") {
    Type = c_NormalizedEnergyPositionFluxFunction;
  } else {
    mout<<"Unknown spectral type: "<<Name<<endl;
  }
 
  return Type;
}

/******************************************************************************
 * Return true, if the region type could be set correctly
 */
bool NSource::SetBeamType(const int& BeamType)
{
  switch (BeamType) {
  case c_NearFieldPoint:
  case c_NearFieldLine:
  case c_NearFieldBox:
  case c_NearFieldDisk:
  case c_NearFieldSphere:
  case c_NearFieldBeam:
  case c_NearFieldActivation:
  case c_NearFieldRestrictedPoint:
  case c_NearFieldBeam1DProfile:
  case c_NearFieldBeam2DProfile:
  case c_NearFieldConeBeam:
  case c_NearFieldConeBeamGauss:
  case c_NearFieldIlluminatedDisk:
  case c_NearFieldIlluminatedSquare:
  case c_NearFieldVolume:
  case c_NearFieldFlatMap:
    m_CoordinateSystem = c_NearField;
    m_BeamType = BeamType;
    break;
  case c_FarFieldPoint:
  case c_FarFieldDisk:
  case c_FarFieldArea:
  case c_FarFieldGaussian:
  case c_FarFieldFile:
  case c_FarFieldFitsFile:
  case c_FarFieldNormalizedEnergyPositionFluxFunction:
  case c_FarFieldFileZenithDependent:
  case c_NormalizedEnergyPositionFluxFunction:
    m_CoordinateSystem = c_FarField;
    m_BeamType = BeamType;
    break;
  default:
    mout<<m_Name<<": Unknown beam type (in SetBeamType): "<<BeamType<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Return the name of a beam type
 */
TString NSource::GetBeamTypeName(const int BeamType)
{
  TString Name = "Unknown";

  switch (BeamType) {
  case c_NearFieldPoint:
    Name = "Point source (near field)";
    break;
  case c_NearFieldRestrictedPoint:
    Name = "Point source restricted (near field)";
    break;
  case c_NearFieldBeam:
    Name = "Pencil beam (near field)";
    break;
  case c_FarFieldPoint:
    Name = "Point source (far field)";
    break;
  case c_FarFieldDisk:
    Name = "Disk source (far field)";
    break;
  case c_FarFieldFile:
    Name = "From file (far field)";
    break;
  case c_FarFieldFitsFile:
    Name = "From FITS file (far field)";
    break;
  case c_FarFieldNormalizedEnergyPositionFluxFunction:
    Name = "From normalized energy-position-flux function (far field)";
    break;
  default:
    mout<<"Unknown beam type (in GetBeamTypeName): "<<BeamType<<endl;
    break;
  }

  return Name;
}


/******************************************************************************
 * Return the ID of a beam type
 */
int NSource::GetBeamType(TString Name)
{
  int Type = c_Invalid;

  if (Name == "Point source (near field)") {
    Type = c_NearFieldPoint;
  } else if (Name == "Point source restricted (near field)") {
    Type = c_NearFieldRestrictedPoint;
  } else if (Name == "Pencil beam (near field)") {
    Type = c_NearFieldBeam;
  } else if (Name == "Point source (far field)") {
    Type = c_FarFieldPoint;
  } else if (Name == "Disk source (far field)") {
    Type = c_FarFieldDisk;
  } else if (Name == "From file (far field)") {
    Type = c_FarFieldFile;
  } else if (Name == "From FITS file (far field)") {
    Type = c_FarFieldFitsFile;
  } else if (Name == "From normalized energy-position-flux function (far field)") {
    Type = c_FarFieldNormalizedEnergyPositionFluxFunction;
  } else {
    mout<<"Unknown beam type (in GetBeamType): "<<Name<<endl;
  }
 
  return Type;
}


/******************************************************************************
 *  Return true, if the particle type could be set correctly
 */
bool NSource::SetLightCurveType(const int& LightCurveType)
{
  switch (LightCurveType) {
  case c_LightCurveFlat:
  case c_LightCurveFile:
    m_LightCurveType = LightCurveType;
    return true;
  default:
    mout<<m_Name<<": Unknown light curve type (in SetLightCurveType): "<<LightCurveType<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Return the name of a beam type
 */
TString NSource::GetLightCurveTypeName(const int BeamType)
{
  TString Name = "Unknown";

  switch (BeamType) {
  case c_LightCurveFlat:
    Name = "Flat - no light curve";
    break;
  case c_LightCurveFile:
    Name = "From file";
    break;
  default:
    mout<<"Unknown light curve type (in GetLightCurveTypeName): "<<BeamType<<endl;
    break;
  }

  return Name;
}


/******************************************************************************
 * Return the ID of a beam type
 */
int NSource::GetLightCurveType(TString Name)
{
  int Type = c_Invalid;

  if (Name == "Flat - no light curve") {
    Type = c_LightCurveFlat;
  } else if (Name == "From file") {
    Type = c_LightCurveFile;
  } else {
    mout<<"Unknown light curve type (in GetLightCurveType): "<<Name<<endl;
  }
 
  return Type;
}


/******************************************************************************
 * Return true, if the position vector could be set correctly. For a 
 * description of the meaning of the parameters see the documentation. 
 */
bool NSource::SetPosition(double PositionParam1, 
                          double PositionParam2, 
                          double PositionParam3, 
                          double PositionParam4, 
                          double PositionParam5, 
                          double PositionParam6, 
                          double PositionParam7, 
                          double PositionParam8, 
                          double PositionParam9, 
                          double PositionParam10, 
                          double PositionParam11)
{
  m_PositionParam1 = PositionParam1;
  m_PositionParam2 = PositionParam2;
  m_PositionParam3 = PositionParam3;
  m_PositionParam4 = PositionParam4;
  m_PositionParam5 = PositionParam5;
  m_PositionParam6 = PositionParam6;
  m_PositionParam7 = PositionParam7;
  m_PositionParam8 = PositionParam8;
  m_PositionParam9 = PositionParam9;
  m_PositionParam10 = PositionParam10;
  m_PositionParam11 = PositionParam11;

  // Some sanity checks:
  if (m_BeamType == c_FarFieldPoint) {
    if (m_PositionParam1 < -90*60 || m_PositionParam1 > 90*60) {
      mout<<m_Name<<": Theta must be within [0..180*60] arcmin"<<endl;
      return false;
    }
  } else if (m_BeamType == c_FarFieldDisk) {
    if (m_PositionParam1 < -90*60 || m_PositionParam1 > 90*60) {
      mout<<m_Name<<": Theta must be within [0..180*60] arcmin"<<endl;
      return false;
    }
    if (m_PositionParam3 <= 0) {
      mout<<m_Name<<": The extent of the disk source must be positive"<<endl;
      return false;
    }
  } else if (m_BeamType == c_FarFieldArea) {
    if (m_PositionParam1 < 0 || m_PositionParam1 > 180*60) {
      mout<<m_Name<<": Minimum theta must be within [0..180*60] arcmin"<<endl;
      return false;
    }
    if (m_PositionParam2 < 0 || m_PositionParam2 > 180*60) {
      mout<<m_Name<<": Maximum theta must be within [0..180*60] arcmin"<<endl;
      return false;
    }
    if (m_PositionParam2 <= m_PositionParam1) {
      mout<<m_Name<<": Maximum theta must be larger than minimum theta"<<endl;
      return false;
    }
    if (m_PositionParam4 <= m_PositionParam3) {
      mout<<m_Name<<": Maximum phi must be larger than minimum theta"<<endl;
      return false;
    }
  } else if (m_BeamType == c_FarFieldGaussian) {
    // not implemented
  } else if (m_BeamType == c_FarFieldFileZenithDependent) {
    // nothing
  } else if (m_BeamType == c_NearFieldPoint) {
    // nothing
  } else if (m_BeamType == c_NearFieldRestrictedPoint) {
    // nothing
  } else if (m_BeamType == c_NearFieldLine) {
    if (m_PositionParam1 == m_PositionParam4 &&
        m_PositionParam2 == m_PositionParam5 &&
        m_PositionParam3 == m_PositionParam6) {
      mout<<m_Name<<": Position 1 must be different from position 2"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldBox) {
    if (m_PositionParam1 == m_PositionParam4 &&
        m_PositionParam2 == m_PositionParam5 &&
        m_PositionParam3 == m_PositionParam6) {
      mout<<m_Name<<": Position 1 must be different from position 2"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldSphere) {
    if (m_PositionParam4 <= 0 || m_PositionParam5 <= 0 || m_PositionParam6 <= 0) {
      mout<<m_Name<<": The radii must be larger than zero"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldDisk) {
    if (m_PositionParam6 <= 0 || m_PositionParam7 <= 0 || m_PositionParam8 <= 0) {
      mout<<m_Name<<": Inner radius, outer radius, and height must be larger than zero"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldBeam) {
    if (m_PositionParam4 == 0 && m_PositionParam5 == 0 && m_PositionParam6 == 0) {
      mout<<m_Name<<": The direction must not be (0, 0, 0)"<<endl;
      return false;
    }
    if (m_PositionParam7 <= 0) {
      mout<<m_Name<<": The radius must be larger than zero"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldBeam1DProfile || m_BeamType == c_NearFieldBeam2DProfile || m_BeamType == c_NearFieldFlatMap) {
    if (m_PositionParam4 == 0 && m_PositionParam5 == 0 && m_PositionParam6 == 0) {
      mout<<m_Name<<": The direction must not be (0, 0, 0)"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldConeBeam) {
    if (m_PositionParam4 == 0 && m_PositionParam5 == 0 && m_PositionParam6 == 0) {
      mout<<m_Name<<": The direction must not be (0, 0, 0)"<<endl;
      return false;
    }
    if (m_PositionParam7 <= 0) {
      mout<<m_Name<<": The opening angle must be larger than zero"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldConeBeamGauss) {
    if (m_PositionParam4 == 0 && m_PositionParam5 == 0 && m_PositionParam6 == 0) {
      mout<<m_Name<<": The direction must not be (0, 0, 0)"<<endl;
      return false;
    }
    if (m_PositionParam7 <= 0) {
      mout<<m_Name<<": The opening angle must be larger than zero"<<endl;
      return false;
    }
    if (m_PositionParam8 <= 0) {
      mout<<m_Name<<": The gaussian 1-sigma value must be larger than zero"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldIlluminatedDisk) {
    if (m_PositionParam4 <= 0) {
      mout<<m_Name<<": The radius of the disk must be larger than zero"<<endl;
      return false;
    }
    if (m_PositionParam5 < 0 || m_PositionParam5 > c_Pi) {
      mout<<m_Name<<": The phi orientation of the disk must be within [0..pi]"<<endl;
      return false;
    }
    if (m_PositionParam7 < 0 || m_PositionParam7 > c_Pi) {
      mout<<m_Name<<": The phi orientation of the beam must be within [0..pi]"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldIlluminatedSquare) {
    if (m_PositionParam4 <= 0) {
      mout<<m_Name<<": The half length of the box must be larger than zero"<<endl;
      return false;
    }
    if (m_PositionParam5 < 0 || m_PositionParam5 > c_Pi) {
      mout<<m_Name<<": The phi orientation of the box must be within [0..pi]"<<endl;
      return false;
    }
    if (m_PositionParam7 < 0 || m_PositionParam7 > c_Pi) {
      mout<<m_Name<<": The phi orientation of the beam must be within [0..pi]"<<endl;
      return false;
    }
  }
  
  return UpgradePosition();
}


/******************************************************************************
 * Return true, if all tests and upgrades for the position parameters
 * are sucessfull
 */
bool NSource::UpgradePosition()
{
  // Attention: This function might be called twice:

  if (m_BeamType == c_NearFieldRestrictedPoint) {
    // CartesianRestrictedPoint is a special case of CartesianConeBeam
    // Thus upgrade the former to the later:

//     // Set the beam direction: m_PositionParam4, m_PositionParam5, m_PositionParam6
//     // Direction from the origin to the center of the surrounding sphere
//     m_PositionParam4 = m_StartAreaPosition.X() - m_PositionParam1;
//     m_PositionParam5 = m_StartAreaPosition.Y() - m_PositionParam2;
//     m_PositionParam6 = m_StartAreaPosition.Z() - m_PositionParam3;

//     // Set the cone angle (i.e. half opening angle): m_PositionParam7
//     // Pay attention to the case, when the start position is inside the sphere
//     if (MVector(m_PositionParam4, m_PositionParam5, m_PositionParam6).mag() > m_StartAreaParam1) {
//       m_PositionParam7 = asin(m_StartAreaParam1/(m_StartAreaPosition - MVector(m_PositionParam1, m_PositionParam2, m_PositionParam3)).mag());
//     } else {
//       m_PositionParam7 = c_Pi;
//     }
  }

  else if (m_BeamType == c_NearFieldConeBeamGauss) {
    if (m_PositionTF1 != 0) delete m_PositionTF1;
    m_PositionTF1 = new TF1("CartesianConeBeamGauss", "exp(-(x*x)/(2.*[0]*[0])) * sin(x)", 0.0, m_PositionParam7);
    m_PositionTF1->SetParameter(0, m_PositionParam8);
  }

  return true;
}


/******************************************************************************
 * Return true, if the file containing the spectrum could be set correctly
 */  
bool NSource::SetPosition(TString FileName)
{
  m_PositionFileName = FileName;
  
  if (m_BeamType == c_FarFieldFileZenithDependent) {
    if (m_PositionFunction.Set(FileName, "DP") == false) {
      mout<<m_Name<<": SphericalFileZenithDependent: Unable to load beam!"<<endl;
      return false;
    }
    //m_PositionFunction.Plot();
    m_PositionParam1 = m_PositionFunction.GetXMin();
    m_PositionParam2 = m_PositionFunction.GetXMax();
    m_PositionParam3 = m_PositionFunction.GetYMax();
    if (m_PositionParam3 == 0.0) {
      mout<<m_Name<<": SphericalFileZenithDependent: Maximum is zero: "<<m_PositionFunction.GetYMax()<<endl;
      return false;
    }
    if (m_PositionFunction.GetSize() < 2) {
      mout<<m_Name<<": At least two entries in the file are required!"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldBeam1DProfile) {
    if (m_PositionFunction.Set(FileName, "DP") == false) {
      mout<<m_Name<<": CartesianBeam1DProfile: Unable to load beam profile!"<<endl;
      return false;
    }
  } else if (m_BeamType == c_NearFieldBeam2DProfile || m_BeamType == c_NearFieldFlatMap) {
    if (m_PositionFunction2D.Set(FileName, "AV") == false) {
      mout<<m_Name<<": Unable to load 2D distribution!"<<endl;
      return false;
    }
  } else if (m_BeamType == c_FarFieldFitsFile) {
    NExtractFitsImage Extractor;
    if (Extractor.Extract(FileName, m_PositionFunction2D) == false) {
      mout<<m_Name<<": Unable to load 2D distribution from fits file!"<<endl;
      return false;
    }
  } 

  return true;
}


/******************************************************************************
 * Return true, if the energy vector could be set correctly. For a 
 * description of the meaning of the parameters see the documentation.
 */
bool NSource::SetEnergy(double EnergyParam1, 
                        double EnergyParam2, 
                        double EnergyParam3, 
                        double EnergyParam4, 
                        double EnergyParam5, 
                        double EnergyParam6,
                        double EnergyParam7)
{
  m_EnergyParam1 = EnergyParam1;
  m_EnergyParam2 = EnergyParam2;
  m_EnergyParam3 = EnergyParam3;
  m_EnergyParam4 = EnergyParam4;
  m_EnergyParam5 = EnergyParam5;
  m_EnergyParam6 = EnergyParam6;
  m_EnergyParam7 = EnergyParam7;

  // Perform some sanity checks:
  if (m_SpectralType == c_Monoenergetic) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The energy must be larger than 0!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_Linear) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The minimum energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= m_EnergyParam1) {
      mout<<m_Name<<": The maximum energy must be larger than the minimum energy!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_PowerLaw) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The minimum energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= m_EnergyParam1) {
      mout<<m_Name<<": The maximum energy must be larger than the minimum energy!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_BrokenPowerLaw) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The minimum energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= m_EnergyParam1) {
      mout<<m_Name<<": The maximum energy must be larger than the minimum energy!"<<endl;
      return false;
    }
    if (m_EnergyParam3 <= m_EnergyParam1 || m_EnergyParam3 > m_EnergyParam2) {
      mout<<m_Name<<": The break energy must be within the minimum and maximum energy!"<<endl;
      return false;
    }
    if (m_EnergyParam4 <= 0) {
      mout<<m_Name<<": The first index (alpha) must be positive !"<<endl;
      return false;      
    }
    if (m_EnergyParam5 <= 0) {
      mout<<m_Name<<": The second index (alpha) must be positive!"<<endl;
      return false;      
    }
  } else if (m_SpectralType == c_Gaussian) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= 0) {
      mout<<m_Name<<": The sigma must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam3 <= 0) {
      mout<<m_Name<<": The cut-off must be larger than 0!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_ThermalBremsstrahlung) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The minimum energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= m_EnergyParam1) {
      mout<<m_Name<<": The maximum energy must be larger than the minimum energy!"<<endl;
      return false;
    }
    if (m_EnergyParam3 <= 0) {
      mout<<m_Name<<": The temperature must be positive!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_BlackBody) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The minimum energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= m_EnergyParam1) {
      mout<<m_Name<<": The maximum energy must be larger than the minimum energy!"<<endl;
      return false;
    }
    if (m_EnergyParam3 <= 0) {
      mout<<m_Name<<": The temperature must be positive!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_BandFunction) {
    // requires reimplementation
  } else if (m_SpectralType == c_FileDifferentialFlux) {
  } else if (m_SpectralType == c_NormalizedFunctionInPhPerCm2PerSPerKeV) {
    if (m_EnergyParam1 <= 0) {
      mout<<m_Name<<": The minimum energy must be larger than 0!"<<endl;
      return false;
    }
    if (m_EnergyParam2 <= m_EnergyParam1) {
      mout<<m_Name<<": The maximum energy must be larger than the minimum energy!"<<endl;
      return false;
    }
  } else if (m_SpectralType == c_Activation) {
  }

  return UpgradeEnergy();
}


/******************************************************************************
 * Set the energy function string
 */
bool NSource::SetEnergyFunctionString(const TString& EnergyFunctionString)
{
  if (EnergyFunctionString == "") return true;

  m_EnergyFunctionString = EnergyFunctionString;

  if (m_SpectralType != c_NormalizedFunctionInPhPerCm2PerSPerKeV) return true;

  if (m_EnergyParam1 == 0.0 && m_EnergyParam1 == m_EnergyParam2) {
    mout<<m_Name<<": The energy window has not been initialized!"<<endl;
    return false;
  }

  TFormula Formula;
  if (Formula.Compile(EnergyFunctionString) != 0) {
    mout<<m_Name<<": The function is not compilable!"<<endl;
    return false;
  }

  m_EnergyTF1 = new TF1(m_Name, EnergyFunctionString, m_EnergyParam1, m_EnergyParam2);
  m_InputFlux = m_EnergyTF1->Integral(m_EnergyParam1, m_EnergyParam2);
  // The curve is in ph/cm2/s/keV, but we need ph/mm2/s/keV
  m_InputFlux /= 100;

  // We have to call upgrade flux again, since we chanegd the input flux
  UpgradeFlux();
  
  return true;
}


/******************************************************************************
 * Return true, if all tests and upgrade to the energy parameters could be 
 * performed
 */
bool NSource::UpgradeEnergy() 
{
  if (m_SpectralType == c_BrokenPowerLaw) {
    m_EnergyParam6 = pow(m_EnergyParam3, -m_EnergyParam4+m_EnergyParam5);
  } else if (m_SpectralType == c_BlackBody) {
    massert(m_EnergyParam1 > 0);
    massert(m_EnergyParam2 > m_EnergyParam1);
    massert(m_EnergyParam3 > 0);

    const double MaximumShift = 1.593624260;
    m_EnergyParam4 = BlackBody(MaximumShift*m_EnergyParam3, m_EnergyParam3);
    if (MaximumShift*m_EnergyParam3 < m_EnergyParam1) m_EnergyParam4 = BlackBody(m_EnergyParam1, m_EnergyParam3);
    if (MaximumShift*m_EnergyParam3 > m_EnergyParam2) m_EnergyParam4 = BlackBody(m_EnergyParam2, m_EnergyParam3);
  } else if (m_SpectralType == c_BandFunction) {
    massert(m_EnergyParam1 > 0);
    massert(m_EnergyParam2 > 0);
    massert(m_EnergyParam1 < m_EnergyParam2);
    massert(m_EnergyParam3 > m_EnergyParam4);
    massert(m_EnergyParam5 > 0);
    
    // Calculate Maximum:
    m_EnergyParam6 = BandFunction(m_EnergyParam1, m_EnergyParam3, m_EnergyParam4, m_EnergyParam5);
    cout<<"Band-Max: "<<m_EnergyParam6 <<endl;
    //m_EnergyParam6 = BandFunction(m_EnergyParam3*m_EnergyParam5, m_EnergyParam3, m_EnergyParam4, m_EnergyParam5);
  }
  
  return true;
}


/******************************************************************************
 * Return true, if the file containing the spectrum could be set correctly
 */
bool NSource::SetEnergy(const TString& FileName)
{
  m_EnergyFileName = FileName;

  if (m_SpectralType == c_FileDifferentialFlux) {
    if (m_EnergyFunction.Set(FileName, "DP") == false) {
      mout<<m_Name<<": Unable to load spectrum correctly!"<<endl;
      return false;
    }

    if (m_EnergyFunction.GetXMin() < 0.0 ||
        m_EnergyFunction.GetXMax() < 0.0 ||
        m_EnergyFunction.GetYMin() < 0.0 ||
        m_EnergyFunction.GetYMax() < 0.0) {
      mout<<m_Name<<": Neither x nor y values are allowed to be negative!"<<endl;
      return false;
    }
    if (m_EnergyFunction.GetSize() < 2) {
      mout<<m_Name<<": At least two entries in the file are required!"<<endl;
      return false;
    }
  }
  
  return true;
}


/******************************************************************************
 * Return true, if the flux could be set correctly
 */
bool NSource::SetFlux(const double& Flux) 
{ 
  if (Flux > 0) {
    m_InputFlux = Flux;
    return UpgradeFlux();
  } 

  if (m_SpectralType == c_NormalizedFunctionInPhPerCm2PerSPerKeV) return true;
  
  merr<<m_Name<<": Flux is negative: "<<Flux<<show;
  return false;
}


/******************************************************************************
 * Return true, if the intensity could be set correctly
 */
bool NSource::UpgradeFlux() 
{ 
  if (m_InputFlux <= 0) {
    merr<<m_Name<<": UpgradeFlux: Input flux is negative: "<<m_InputFlux<<show;
    return false;
  }
    
  
  m_Flux = m_InputFlux;

  if (m_CoordinateSystem == c_FarField) {
    if (m_Satellite.GetGeometryAndDetectorPropertiesModule() == 0) {
      merr<<m_Name<<": UpgradeFlux: Satellite doesn't have a geometry module!"<<show;
      return false;
    }
  
    double Radius = m_Satellite.GetSurroundingSphereRadius();
    m_Flux *= c_Pi*Radius*Radius;
    // Since we have two telescopes, we have to multiply the flux by a a factor of 2 if we want to use only one source for two telescopes
    m_Flux *= 2;
  } else {
//     if (m_BeamType == c_NearFieldRestrictedPoint) {

//       // NearFieldRestrictedPoint is a special case of NearFieldPoint
//       // We emit only into a special angle thus we have to downscale the flux
//       double ConeAngle = asin(m_StartAreaParam1/(m_StartAreaPosition - MVector(m_PositionParam1, m_PositionParam2, m_PositionParam3)).mag());
//       double Area = 2*c_Pi*(1-cos(ConeAngle));
//       m_Flux *= Area/(4*c_Pi);
//     }
  }

//   if (m_Flux <= 0 && m_StartAreaType != c_StartAreaUnknown && m_IsSuccessor == false) {
//     mout<<m_Name<<": We do not have a positive flux ("<<m_Flux<<")... Reason unknown... aborting..."<<endl;
//     abort();
//     return false;
//   }

  if (m_Flux <= 0) {
    merr<<m_Name<<": UpgradeFlux: Source doesn't have a positive flux: "<<m_Flux<<show;
    return false;
  } 

  UpgradeLightCurve();

  return true;
}


/******************************************************************************
 * Return true if the combined normalized energy=beam-flux-function could be set
 */
bool NSource::SetNormalizedEnergyPositionFluxFunction(TString FileName)
{
  m_NormalizedEnergyPositionFluxFileName = FileName;
  if (m_BeamType != c_FarFieldNormalizedEnergyPositionFluxFunction) return true;
  if (MFile::Exists(FileName) == false) return false;
  
  m_NormalizedEnergyPositionFluxFunction.Set(FileName, "AP");

  // Integrate over to determine the flux
  m_InputFlux = m_NormalizedEnergyPositionFluxFunction.Integrate();
  // Switch to mm2
  m_InputFlux /= 100;

  UpgradeFlux();
  
  return true;
}


/******************************************************************************
 * Return true, if the light curve could be set correctly
 */
bool NSource::SetLightCurve(const TString& FileName, const bool& Repeats)
{
  if (m_LightCurveType == c_LightCurveFile) {
    if (MFile::Exists(FileName) == false) {
      mout<<m_Name<<": LightCurveFile: File does not exist!"<<endl;
      return false;    
    }
    if (m_LightCurveFunction.Set(FileName, "DP") == false) {
      mout<<m_Name<<": LightCurveFile: Unable to load light curve!"<<endl;
      return false;
    }
    
    if (m_LightCurveFunction.GetSize() < 2) {
      mout<<m_Name<<": LightCurveFile: At least two entries in the file are required!"<<endl;
      return false;
    }
    
    // Scale the time axis with Geant4's times
    m_LightCurveFunction.ScaleX(second);
    m_LightCurveFileName = FileName;
    m_IsRepeatingLightCurve = Repeats;
  } else if (m_LightCurveType == c_LightCurveFlat) {
    // Nothing...
  } else {
    mout<<m_Name<<": Unknown light curve type (in SetLightCurve): "<<m_LightCurveType<<endl;
    return false;
  }
  
  UpgradeLightCurve();
  
  return true;
}


/******************************************************************************
 * Return true, if the enhanced light curve calculations could be performed
 */
bool NSource::UpgradeLightCurve()
{
  if (m_LightCurveType == c_LightCurveFile) {
    // Make sure the y-value is the flux in particles/second
    
    if (m_Flux > 0 && m_LightCurveFunction.Integrate() > 0) {
      m_LightCurveFunction.ScaleY((m_LightCurveFunction.GetXMax() - m_LightCurveFunction.GetXMin())/m_LightCurveFunction.Integrate());
    }
    // cout<<"Final light-curve integration: "<<m_LightCurveFunction.Integrate()<<endl;
  }
  
  return true;
}


/******************************************************************************
 * Return the time to the next photon emission of this source:
 * The error in this routine is <= Scale
 */
bool NSource::CalculateNextEmission(NTime Time)
{
  double NextEmission = 0;
  
  // This is the main time loop:
  if (m_Flux <= 0) {
    merr<<m_Name<<": CalculateNextEmission: Source doesn't have a positive flux: "<<m_Flux<<":"<<m_InputFlux<<endl;
    return false;
  }
  
  if (m_LightCurveType == c_LightCurveFlat) {
    // A random exponential can be used to simulate
    // random arrival times of the photons (Poisson Arrival Model)
    NextEmission = gRandom->Exp(1.0/m_Flux);
    m_NextEmission.Set(NextEmission);
    m_NextEmission += Time;
  } else if (m_LightCurveType == c_LightCurveFile) {
    double dIntegral = gRandom->Exp(1.0/m_Flux);
    //cout<<"dIntegral: "<<dIntegral<<endl;
    NextEmission = m_LightCurveFunction.FindX(m_NextEmission.GetAsSeconds(), dIntegral, m_IsRepeatingLightCurve);
    if (m_IsRepeatingLightCurve == false && NextEmission >= m_LightCurveFunction.GetXMax()) {
      m_IsActive = false;
      m_NextEmission.SetMax();
      mout<<m_Name<<": light-curve data exceeded."<<endl;
    } else {
      //cout<<"Next emission: "<<NextEmission<<":"<<dIntegral<<endl;
      m_NextEmission.Set(NextEmission);
    }
  }
  
  if (m_NextEmission.GetAsSeconds() < 0) {
    merr<<m_Name<<": CalculateNextEmission: Next emission time is in the past: "<<m_NextEmission<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Generate an inititial energy in the particle gun
 */
bool NSource::Generate(NPhoton& Photon, int Telescope)
{
  // DO NOT CHANGE THIS SEQUENCE, ENERGY MUST COME BEFORE POSITION!
  if (GenerateEnergy(Photon) == false) return false;
  if (GeneratePosition(Photon, Telescope) == false) return false;

  return true;
}

/******************************************************************************
 * Generate an inititial energy in the particle gun
 */
bool NSource::GenerateEnergy(NPhoton& Photon)
{
  double Probability = 0.0;
  double Energy = 0.0;
  
  switch (m_SpectralType) {
  case c_Monoenergetic:
    Energy = m_EnergyParam1;
    break;
  case c_Linear:
    Energy = m_EnergyParam1 + gRandom->Rndm()*(m_EnergyParam2-m_EnergyParam1);
    break;
  case c_PowerLaw:
    while (true) {
      Energy = m_EnergyParam1 + gRandom->Rndm()*(m_EnergyParam2-m_EnergyParam1);
      if (gRandom->Rndm() < pow(m_EnergyParam1/Energy, m_EnergyParam3)) break;
    }
    break;
  case c_BrokenPowerLaw:
    // This algorithm can be very slow...
    while (true) {
      Energy = m_EnergyParam1 + gRandom->Rndm()*(m_EnergyParam2-m_EnergyParam1);
      if (Energy > m_EnergyParam3) {
        Probability = 
          m_EnergyParam6*pow(m_EnergyParam1, m_EnergyParam4)/pow(Energy, m_EnergyParam5);
      } else {
        Probability = pow(m_EnergyParam1/Energy, m_EnergyParam4);
      }
      if (gRandom->Rndm() < Probability) break;
    }
    break;
  case c_Gaussian:
    Energy = gRandom->Gaus(m_EnergyParam1, m_EnergyParam2);
    break;
  case c_ThermalBremsstrahlung:
    while (true) {
      Energy = m_EnergyParam1 + gRandom->Rndm()*(m_EnergyParam2-m_EnergyParam1);
      if (gRandom->Rndm() <= 
          (1.0/Energy * exp(-Energy/m_EnergyParam3))/
          (1.0/m_EnergyParam1 * exp(-m_EnergyParam1/m_EnergyParam3))) break;
    }    
    break;
  case c_BlackBody:
    while (true) {
      Energy = m_EnergyParam1 + gRandom->Rndm()*(m_EnergyParam2-m_EnergyParam1);
      if (gRandom->Rndm() <= BlackBody(Energy, m_EnergyParam3)/m_EnergyParam4) break;
    }    
    break;
  case c_BandFunction:  mout<<m_Name<<": New flux: "<<m_Flux<<show;

    while (true) {
      Energy = m_EnergyParam1 + gRandom->Rndm()*(m_EnergyParam2-m_EnergyParam1);
      double BandValue = BandFunction(Energy, m_EnergyParam3, m_EnergyParam4, m_EnergyParam5);
      if (BandValue > m_EnergyParam6) {
        mout<<"Precalculated maximum of band function is wrong!!"<<endl;
      }
      if (gRandom->Rndm() <= BandValue/m_EnergyParam6) break;
    }    
    break;
  case c_FileDifferentialFlux:
    // All functionality is in MFunction:
    Energy = m_EnergyFunction.GetRandom();
    break;
  case c_NormalizedFunctionInPhPerCm2PerSPerKeV:
    // All functionality is in MFunction:
    Energy = m_EnergyTF1->GetRandom();
    break;
  case c_NormalizedEnergyPositionFluxFunction:
    // All functionality is in MFunction3DSpherical:
    // We temporarily store the position here in m_PositionParam1, m_PositionParam2
    m_NormalizedEnergyPositionFluxFunction.GetRandom(m_PositionParam1, m_PositionParam2, Energy);
    break;
  default:
    merr<<"Energy type not yet implemented: "<<m_SpectralType<<endl;
    Energy = 55.5;
    return false;
    break;
  }

  Photon.SetEnergy(Energy);
  
  return true;
}


/******************************************************************************
 * Generate an inititial position in the particle gun
 */
bool NSource::GeneratePosition(NPhoton& Photon, int Telescope)
{
  MVector Position;
  MVector Direction;
  
	if (m_CoordinateSystem == c_FarField) {
    double RA = 0.0;
    double DEC = 0.0;

    double Theta = 0.0;
    double Phi = 0.0;
    if (m_BeamType == c_FarFieldPoint || 
        m_BeamType == c_FarFieldDisk ||
        m_BeamType == c_FarFieldArea ||
        m_BeamType == c_FarFieldFitsFile ||
        m_BeamType == c_FarFieldNormalizedEnergyPositionFluxFunction ||
        m_BeamType == c_FarFieldFileZenithDependent) {
      if (m_BeamType == c_FarFieldPoint) {
        // Fixed start direction
        DEC = m_PositionParam1/60*c_Rad;
        RA = m_PositionParam2/60*c_Rad;
      } else if (m_BeamType == c_FarFieldDisk) {
        // Start with random position in disk on axis
        MVector Dir;
        Dir.SetMagThetaPhi(1.0, acos(1 - gRandom->Rndm()*(1 - cos(m_PositionParam3/60*c_Rad))), gRandom->Rndm()*2*c_Pi); 

        DEC = m_PositionParam1/60*c_Rad;
        RA = m_PositionParam2/60*c_Rad;

        // For the following rotation we must make sure DEC is from 0..pi and not pi/2..-pi/2:
        DEC = c_Pi/2 - DEC;
        
        // Rotate the direction vector according to theta and phi
        // left-handed rotation-matrix: first theta (rotation around y-axis) then phi (rotation around z-axis):
        // | +cosp -sinp 0 |   | +cost 0 +sint |   | x |
        // | +sinp +cosp 0 | * |   0   1   0   | * | y | 
        // |   0     0   1 |   | -sint 0 +cost |   | z |        
        double px = Dir.X(), py = Dir.Y(), pz = Dir.Z();
        Dir[0] = (px*cos(DEC)+pz*sin(DEC))*cos(RA) - py*sin(RA);
        Dir[1] = (px*cos(DEC)+pz*sin(DEC))*sin(RA) + py*cos(RA);
        Dir[2] = -px*sin(DEC)+pz*cos(DEC);

        DEC = Dir.Theta();
        RA = Dir.Phi();
        
        // Switch back to real DEC's
        DEC = c_Pi/2 - DEC;
        
      } else if (m_BeamType == c_FarFieldArea) {

        DEC = acos(cos(m_PositionParam1/60*c_Rad) - gRandom->Rndm()*(cos(m_PositionParam1/60*c_Rad) - cos(m_PositionParam2/60*c_Rad)));
        RA = m_PositionParam3/60*c_Rad + gRandom->Rndm()*(m_PositionParam4/60*c_Rad - m_PositionParam3/60*c_Rad);

      } else if (m_BeamType == c_FarFieldFileZenithDependent) {
        // Determine a random position on the sphere between 
        // theta min and theta max in the file:

        while (true) {
          DEC = acos(cos(m_PositionParam1) - gRandom->Rndm()*(cos(m_PositionParam1) - cos(m_PositionParam2)));
          RA = gRandom->Rndm()*c_Pi;
          if (gRandom->Rndm() <= m_PositionFunction.Eval(Theta)/m_PositionParam3) {
            break;
          }
        }    
      } else if (m_BeamType == c_FarFieldFitsFile) {
        // Determine random RA / DEC
        m_PositionFunction2D.GetRandom(RA, DEC);
        RA *= c_Rad;
        DEC *= c_Rad;
      } else if (m_BeamType == c_FarFieldNormalizedEnergyPositionFluxFunction) {
        // RA / DEC have been predetermied during GenerateEnergy()
        RA = m_PositionParam1 * c_Rad;
        DEC = m_PositionParam2 * c_Rad;
      }
      
      // Procedure
      // The focal bench points at the pointing you give in the pointing engine.

      // set up a rotation in the RA/DEC system. 
      // -> create a test vector in RA/DEC
      // -> rotate it into the FB system
      // -> determine its direction in optical bench
      // -> determine its direction in optics module
      // -> start photon relative to optics module

      // Create a pointing to create a rotation quaternion:
      NPointing PhotonOrigin;
      PhotonOrigin.SetRaDecYaw(RA*60*c_Deg, DEC*60*c_Deg, 180.0*60);
      
      // Create a test photon (can I just use a test direction too???)
      NPhoton Test;
      Test.SetDirection(MVector(0.0, 0.0, 1.0));
      Test.SetPosition(MVector(0.0, 0.0, 0.0));
      
      // First determine what the inertial vector is by transforming 
      // a test vector (0,0,1) with the source RA, DEC. 
      NOrientation PhotonOriginOrientation;
      PhotonOriginOrientation.SetRotation(PhotonOrigin.GetQuaternion());
      PhotonOriginOrientation.TransformOut(Test);
      
      // Now that we have the inertial vector of the Source, transfrom it 
      // into the FP system using the spacecraft pointing inertial Quarternion
      NOrientation FBSystem;
      FBSystem.SetRotation(m_Satellite.GetPointing(m_NextEmission).GetQuaternion());
      FBSystem.TransformIn(Test);
	  //cout<<"testdirection"<<Test.GetDirection()<<endl;
          
      // Into star tracker system:
      //NOrientation StarTrackerSystem;
      //StarTrackerSystem.SetRotation(m_Satellite.GetPointing(m_NextEmission).GetQuaternion());
      //StarTrackerSystem.TransformIn(Test);
        
      // Into optical bench:
      //m_Satellite.GetCalibratedOrientationStarTrackerRelOpticalBench(4).TransformIn(Test);
       
      // Into optical bench
      m_Satellite.GetOrientationOpticalBench(m_NextEmission).TransformIn(Test);
      
      // Into optics:
      m_Satellite.GetOrientationOpticsRelOpticalBench(m_NextEmission, Telescope).TransformIn(Test);
     

      // 
      Theta = Test.GetDirection().Theta();
      Phi = Test.GetDirection().Phi();
      
      //cout<<"T: "<<Theta<<" - P: "<<Phi<<endl;

      
      // Generate start direction:
      Direction.SetMagThetaPhi(1, Theta, Phi);
      Direction = -Direction;

      // The position is handled as if we are in the optics module coordinate system

      // Determine start position on disk on sphere
      double x = 0.0, y = 0.0, z = 0.0;
      double Radius = m_Satellite.GetSurroundingSphereRadius();
      // Disk is currently fixed on (0, 0, Radius)
      while (true) {
        x = Radius*(2*(gRandom->Rndm()-0.5));
        y = Radius*(2*(gRandom->Rndm()-0.5));
        if (sqrt(x*x+y*y) <= Radius) break;
      }
      z = Radius;
     
      // Rotate according to theta and phi
      // left-handed rotation-matrix: first theta (rotation around y-axis) then phi (rotation around z-axis):
      // | +cosp -sinp 0 |   | +cost 0 +sint |   | x |
      // | +sinp +cosp 0 | * |   0   1   0   | * | y | 
      // |   0     0   1 |   | -sint 0 +cost |   | z |        
      Position[0] = (x*cos(Theta)+z*sin(Theta))*cos(Phi) - y*sin(Phi);
      Position[1] = (x*cos(Theta)+z*sin(Theta))*sin(Phi) + y*cos(Phi);
      Position[2] = -x*sin(Theta)+z*cos(Theta);

      // Move to top of optics:
      Position[2] += m_Satellite.GetSurroundingSphereZOffsetInOpticsModuleCoordinates();

      Photon.SetPosition(Position);      
      Photon.SetDirection(Direction);
      
      // Translate into the optical bench
      m_Satellite.GetOrientationOpticsRelOpticalBench(m_NextEmission, Telescope).TransformOut(Photon);
      
      // Rotate and transform into the world frame = focal bench system
      m_Satellite.GetOrientationOpticalBench(m_NextEmission).TransformOut(Photon);
      
      //cout<<"Dir WF: "<<Photon.GetDirection()<<":"<<Photon.GetPosition()<<endl;
    } 

    else if  (m_BeamType == c_FarFieldGaussian) {
      mout<<"Mode SphericalGaussian not yet implemented!"<<endl;
      return false;
    } 
  } 

  else if (m_CoordinateSystem == c_NearField) {

    // Start position:

    if (m_BeamType == c_NearFieldPoint || 
        m_BeamType == c_NearFieldRestrictedPoint ||
        m_BeamType == c_NearFieldConeBeam ||
        m_BeamType == c_NearFieldConeBeamGauss) {
      Photon.SetPosition(MVector(m_PositionParam1, m_PositionParam2, m_PositionParam3));
    } 

    else if (m_BeamType == c_NearFieldBeam ||
             m_BeamType == c_NearFieldBeam1DProfile ||
             m_BeamType == c_NearFieldBeam2DProfile ||
             m_BeamType == c_NearFieldDisk ||
             m_BeamType == c_NearFieldFlatMap) {
      // Create the circular beam:
      MVector Temp;
      double Radius = 0, Theta = 0, Phi = 0;

      if (m_BeamType == c_NearFieldBeam || 
          m_BeamType == c_NearFieldBeam1DProfile) {
        Phi = 2*c_Pi*gRandom->Rndm();
        if (m_BeamType == c_NearFieldBeam) {
          Radius = m_PositionParam7*sqrt(gRandom->Rndm());
        } else {
          // Using GetRandomTimesX multiplies the spectrum with x.
          // The means that the profile is correctly scaled to allow distributing the linear profile over the disc
          Radius = m_PositionFunction.GetRandomTimesX();
        }
        Temp[0] = Radius*cos(Phi);
        Temp[1] = Radius*sin(Phi);
        Temp[2] = 0.0;
      } else if (m_BeamType == c_NearFieldBeam2DProfile ||
                 m_BeamType == c_NearFieldFlatMap) {
        double x = 0, y = 0;
        m_PositionFunction2D.GetRandom(x, y);

        // Rotate around z-axis:
        // (a) convert into spherical coordinates
        Radius = sqrt(x*x + y*y);

        if (x == 0) {
          if (y > 0) {
            Phi = c_Pi/2;
          } else if (y < 0) {
            Phi = -c_Pi/2;
          } else {
            Phi = 0.0;
          }
        } else if (x > 0) {
          Phi = atan(y/x);
        } else if (x < 0) {
          if (y >= 0) {
            Phi = atan(y/x) + c_Pi;
          } else {
            Phi = atan(y/x) - c_Pi;
          }
        }

        // (b) add rotation
        Phi += m_PositionParam7; // Already in rad...

        // (c) convert back
        Temp[0] = Radius*cos(Phi);
        Temp[1] = Radius*sin(Phi);
        Temp[2] = 0.0;

      } else if (m_BeamType == c_NearFieldDisk) {
        Phi = m_PositionParam10 + gRandom->Rndm()*(m_PositionParam11-m_PositionParam10);
        Radius = sqrt(m_PositionParam7*m_PositionParam7 + (m_PositionParam8*m_PositionParam8-m_PositionParam7*m_PositionParam7)*gRandom->Rndm());

        Temp[0] = Radius*cos(Phi);
        Temp[1] = Radius*sin(Phi);
        Temp[2] = (gRandom->Rndm()-0.5)*m_PositionParam9;
      }
      
      // Rotate into normal direction (first around theta, then phi):
      Direction.SetX(m_PositionParam4);
      Direction.SetY(m_PositionParam5);
      Direction.SetZ(m_PositionParam6);
      Theta = Direction.Theta();
      Phi = Direction.Phi();

      Position[0] = 
        (Temp[0]*cos(Theta)+Temp[2]*sin(Theta))*cos(Phi) - Temp[1]*sin(Phi);
      Position[1] = 
        (Temp[0]*cos(Theta)+Temp[2]*sin(Theta))*sin(Phi) + Temp[1]*cos(Phi);
      Position[2] = 
        -Temp[0]*sin(Theta)+Temp[2]*cos(Theta);
      
      // Translate:
      Position[0] += m_PositionParam1;
      Position[1] += m_PositionParam2;
      Position[2] += m_PositionParam3;    
      
      Photon.SetPosition(Position);
    } 

    // Start direction:

    if (m_BeamType == c_NearFieldPoint || 
        m_BeamType == c_NearFieldLine || 
        m_BeamType == c_NearFieldBox || 
        m_BeamType == c_NearFieldDisk || 
        m_BeamType == c_NearFieldSphere ||
        m_BeamType == c_NearFieldActivation ||
        m_BeamType == c_NearFieldVolume ||
        m_BeamType == c_NearFieldFlatMap) {
      Direction.SetMagThetaPhi(1.0, acos(1-2*gRandom->Rndm()), gRandom->Rndm()*2*c_Pi); 
      Photon.SetDirection(Direction);
    } 

    else if  (m_BeamType == c_NearFieldBeam ||
              m_BeamType == c_NearFieldBeam1DProfile ||
              m_BeamType == c_NearFieldBeam2DProfile) {
      Direction.SetX(m_PositionParam4);
      Direction.SetY(m_PositionParam5);
      Direction.SetZ(m_PositionParam6);

      Photon.SetDirection(Direction);
    } 

    else if  (m_BeamType == c_NearFieldRestrictedPoint ||
              m_BeamType == c_NearFieldConeBeam ||
              m_BeamType == c_NearFieldConeBeamGauss) {

      // Initial remark: During SetPosition all parameters have been set in a way that 
      // CartesianRestrictedPoint is HERE identical with CartesianConeBeam

      // Determine theta, and phi of the beam direction: 
      MVector BeamDirection;
      BeamDirection.SetXYZ(m_PositionParam4, m_PositionParam5, m_PositionParam6);
      double BeamTheta = BeamDirection.Theta();
      double BeamPhi = BeamDirection.Phi();
      
      // Generate a random zenith angle Theta (in [0, m_PositionParam7]) and 
      // an azimuth angle Phi (in [0, 2pi]) relative to an on-axis cone-beam direction
      double Theta = 0, Phi = 0;
      if (m_BeamType == c_NearFieldRestrictedPoint ||
          m_BeamType == c_NearFieldConeBeam) {
        // We have a flat distribution in angle space
        Phi = 2*c_Pi * gRandom->Rndm();
        Theta = acos(1.0 - gRandom->Rndm() * (1.0 - cos(m_PositionParam7)));
      } else if (m_BeamType == c_NearFieldConeBeamGauss) {
        // We have a gaussian profile in angle space
        Phi = 2*c_Pi * gRandom->Rndm();
        Theta = m_PositionTF1->GetRandom();
      }

      // Now we have the random direction relative to an on-axis cone-beam
      MVector DirectionOnAxis;
      DirectionOnAxis.SetMagThetaPhi(1.0, Theta, Phi);

      // Rotate this direction in beam direction:
      // Right-handed rotation-matrix: first BeamTheta (rotation around y-axis),
      // then BeamPhi (rotation around z-axis):
      // | +cosp -sinp 0 |   | +cost 0 +sint |   | x |
      // | +sinp +cosp 0 | * |   0   1   0   | * | y | 
      // |   0     0   1 |   | -sint 0 +cost |   | z |
      Direction[0] = (DirectionOnAxis[0]*cos(BeamTheta)+DirectionOnAxis[2]*sin(BeamTheta))*cos(BeamPhi) - DirectionOnAxis[1]*sin(BeamPhi);
      Direction[1] = (DirectionOnAxis[0]*cos(BeamTheta)+DirectionOnAxis[2]*sin(BeamTheta))*sin(BeamPhi) + DirectionOnAxis[1]*cos(BeamPhi);
      Direction[2] = -DirectionOnAxis[0]*sin(BeamTheta)+DirectionOnAxis[2]*cos(BeamTheta);

      // Give the information to the particle gun:
      Photon.SetDirection(Direction);
    }
  }

  return true;
}


/******************************************************************************
 * Shape of black body emission (Temperature in keV)
 */  
double NSource::BlackBody(double Energy, double Temperature) const
{
  if (Energy == 0 || Temperature == 0) return 0.0;
  return (Energy*Energy)/(exp(Energy/Temperature)-1);
}


/******************************************************************************
 * Shape of a Band function
 */  
double NSource::BandFunction(const double Energy, double Alpha, 
                              const double Beta, const double E0) const
{
  if (Energy == 0 || Alpha - Beta < 0) return 0.0;

  double Ebreak = (Alpha - Beta)*E0;
  if (Energy <= Ebreak) {
    return pow(Energy, Alpha)*exp(-Energy/E0);
  } else {
    return pow(Ebreak, Alpha-Beta)*exp(-Alpha+Beta)*pow(Energy, Beta);
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NSource::ParseLine(TString Line, int Version)
{
  // This is NOT a universal parser - just a quick hack to convert some data
  // Format: 164700.1-460559  1  251.75063  -46.10000  3   5.0  80.0  2.0  0.000035200

  Initialize();

  MTokenizer T(' ', false);
  T.Analyze(Line);
  if (T.GetNTokens() < 3) {
    cout<<"Line parser source: Number of tokens not OK"<<endl;
    return false;
  }
  int Pos = 0;
  m_Name = T.GetTokenAtAsString(Pos++);
  
  m_BeamType = T.GetTokenAtAsInt(Pos++);
  if (m_BeamType == NSource::c_FarFieldPoint) {
    if (T.GetNTokens() < Pos + 2) {
      cout<<m_Name<<": Line parser source: Not enough tokens for beam"<<endl;
      return false;
    }
    m_PositionParam2 = T.GetTokenAtAsDouble(Pos++)*60;
    m_PositionParam1 = T.GetTokenAtAsDouble(Pos++)*60;
  } else if (m_BeamType == NSource::c_FarFieldDisk) {
    if (T.GetNTokens() < Pos + 3) {
      cout<<m_Name<<": Line parser source: Not enough tokens for beam"<<endl;
      return false;
    }
    m_PositionParam2 = T.GetTokenAtAsDouble(Pos++)*60;
    m_PositionParam1 = T.GetTokenAtAsDouble(Pos++)*60;
    m_PositionParam3 = T.GetTokenAtAsDouble(Pos++)*60;
  } else if (m_BeamType == NSource::c_FarFieldFitsFile) {
    if (T.GetNTokens() < Pos + 1) {
      cout<<m_Name<<": Line parser source: Not enough tokens for beam"<<endl;
      return false;
    }
    m_PositionFileName = T.GetTokenAtAsString(Pos++);
  } else if (m_BeamType == NSource::c_FarFieldNormalizedEnergyPositionFluxFunction) {
    m_NormalizedEnergyPositionFluxFileName  = T.GetTokenAtAsString(Pos++);
  } else {
    cout<<m_Name<<": Line parser source: This beam type is not yet implemented: "<<m_BeamType<<endl;
    return false;
  }

  if (m_BeamType != NSource::c_FarFieldNormalizedEnergyPositionFluxFunction) {
    m_SpectralType = T.GetTokenAtAsInt(Pos++);
    if (m_SpectralType == NSource::c_Monoenergetic) {
      if (T.GetNTokens() < Pos + 1) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyParam1 = T.GetTokenAtAsDouble(Pos++);
    } else if (m_SpectralType == NSource::c_Linear) {
      if (T.GetNTokens() < Pos + 2) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyParam1 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam2 = T.GetTokenAtAsDouble(Pos++);
    } else if (m_SpectralType == NSource::c_PowerLaw) {
      if (T.GetNTokens() < Pos + 3) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyParam1 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam2 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam3 = T.GetTokenAtAsDouble(Pos++);
    } else if (m_SpectralType == NSource::c_BrokenPowerLaw) {
      if (T.GetNTokens() < Pos + 5) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyParam1 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam2 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam3 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam4 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam5 = T.GetTokenAtAsDouble(Pos++);
    } else if (m_SpectralType == NSource::c_FileDifferentialFlux) {
      if (T.GetNTokens() < Pos + 1) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyFileName = T.GetTokenAtAsString(Pos++);
    } else if (m_SpectralType == NSource::c_BlackBody) {
      if (T.GetNTokens() < Pos + 3) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyParam1 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam2 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam3 = T.GetTokenAtAsDouble(Pos++);
    } else if (m_SpectralType == NSource::c_NormalizedFunctionInPhPerCm2PerSPerKeV) {
      if (T.GetNTokens() < Pos + 3) {
        cout<<m_Name<<": Line parser source: Not enough tokens for spectrum"<<endl;
        return false;
      }
      m_EnergyParam1 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyParam2 = T.GetTokenAtAsDouble(Pos++);
      m_EnergyFunctionString = T.GetTokenAtAsString(Pos++);
     // cout<<"FUnction: "<<m_EnergyFunctionString<<endl;
    } else {
      cout<<m_Name<<": Line parser source: This spectral type is not yet implemented: "<<m_SpectralType<<endl;
      return false;
    }
  
  
    if (m_SpectralType != NSource::c_NormalizedFunctionInPhPerCm2PerSPerKeV) {
      if (m_BeamType == NSource::c_FarFieldPoint ||
        m_BeamType == NSource::c_FarFieldDisk ||
        m_BeamType == NSource::c_FarFieldFitsFile) {
        m_InputFlux = T.GetTokenAtAsDouble(Pos++)/100; // /100 to switch from external ph/s/cm2 to internal ph/s/mm2
      } else {
        cout<<m_Name<<": Line parser source - flux parsing: This beam type is not yet implemented: "<<m_BeamType<<endl;
        return false;
      }
    }
  } else {
    m_SpectralType = NSource::c_NormalizedEnergyPositionFluxFunction;
    m_InputFlux = 1.0;
  }
  
  // Light curve section
  if (T.GetNTokens() == Pos) {
    m_LightCurveType = NSource::c_LightCurveFlat;
  } else {
    m_LightCurveType = T.GetTokenAtAsInt(Pos++);
    if (m_LightCurveType == NSource::c_LightCurveFlat) {
      // nothing...
    } else if (m_LightCurveType == NSource::c_LightCurveFile) {
      if (T.GetNTokens() < Pos + 2) {
        cout<<m_Name<<": Line parser source: Not enough tokens to parse for light curve by file"<<endl;
        return false;
      }
      m_LightCurveFileName = T.GetTokenAtAsString(Pos++);
      m_IsRepeatingLightCurve = T.GetTokenAtAsBoolean(Pos++);      
    } else {
      cout<<m_Name<<": Line parser source: This light curve type is not yet implemented: "<<m_LightCurveType<<endl;
      return false;
    }
  }
  
  
  // Do an official "set" to initialize all the other variables:
  if (SetSpectralType(m_SpectralType) == false) return false;
  if (SetEnergy(m_EnergyParam1, m_EnergyParam2, m_EnergyParam3, m_EnergyParam4, m_EnergyParam5) == false) return false;
  if (SetBeamType(m_BeamType) == false) return false;
  if (SetPosition(m_PositionParam1, m_PositionParam2, m_PositionParam3,
              m_PositionParam4, m_PositionParam5, m_PositionParam6,
              m_PositionParam7, m_PositionParam8, m_PositionParam9,
              m_PositionParam10, m_PositionParam11) == false) return false;
  if (SetPosition(m_PositionFileName) == false) return false;
  if (SetFlux(m_InputFlux) == false) return false; // Set the flux before the energy !!
  if (SetEnergy(m_EnergyFileName) == false) return false;
  if (SetEnergyFunctionString(m_EnergyFunctionString) == false) return false;
  if (SetNormalizedEnergyPositionFluxFunction(m_NormalizedEnergyPositionFluxFileName) == false) return false;
  if (SetLightCurveType(m_LightCurveType) == false) return false;
  if (m_LightCurveType == NSource::c_LightCurveFile) {
    if (SetLightCurve(m_LightCurveFileName, m_IsRepeatingLightCurve) == false) return false;
  }

  return true;
}
  

////////////////////////////////////////////////////////////////////////////////


bool NSource::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  Initialize();

  MXmlNode* NameNode = Node->GetNode("Name");
  if (NameNode != 0) {
    m_Name = NameNode->GetValueAsString();
  }
  MXmlNode* SpectralTypeNode = Node->GetNode("SpectralType");
  if (SpectralTypeNode != 0) {
    m_SpectralType = SpectralTypeNode->GetValueAsInt();
  }
  MXmlNode* BeamTypeNode = Node->GetNode("BeamType");
  if (BeamTypeNode != 0) {
    m_BeamType = BeamTypeNode->GetValueAsInt();
  }
  MXmlNode* LightCurveTypeNode = Node->GetNode("LightCurveType");
  if (LightCurveTypeNode != 0) {
    m_LightCurveType = LightCurveTypeNode->GetValueAsInt();
  }
  MXmlNode* FluxNode = Node->GetNode("Flux");
  if (FluxNode != 0) {
    m_InputFlux = FluxNode->GetValueAsDouble();
  }
  MXmlNode* PositionParam1Node = Node->GetNode("PositionParam1");
  if (PositionParam1Node != 0) {
    m_PositionParam1 = PositionParam1Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam2Node = Node->GetNode("PositionParam2");
  if (PositionParam2Node != 0) {
    m_PositionParam2 = PositionParam2Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam3Node = Node->GetNode("PositionParam3");
  if (PositionParam3Node != 0) {
    m_PositionParam3 = PositionParam3Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam4Node = Node->GetNode("PositionParam4");
  if (PositionParam4Node != 0) {
    m_PositionParam4 = PositionParam4Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam5Node = Node->GetNode("PositionParam5");
  if (PositionParam5Node != 0) {
    m_PositionParam5 = PositionParam5Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam6Node = Node->GetNode("PositionParam6");
  if (PositionParam6Node != 0) {
    m_PositionParam6 = PositionParam6Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam7Node = Node->GetNode("PositionParam7");
  if (PositionParam7Node != 0) {
    m_PositionParam7 = PositionParam7Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam8Node = Node->GetNode("PositionParam8");
  if (PositionParam8Node != 0) {
    m_PositionParam8 = PositionParam8Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam9Node = Node->GetNode("PositionParam9");
  if (PositionParam9Node != 0) {
    m_PositionParam9 = PositionParam9Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam10Node = Node->GetNode("PositionParam10");
  if (PositionParam10Node != 0) {
    m_PositionParam10 = PositionParam10Node->GetValueAsDouble();
  }
  MXmlNode* PositionParam11Node = Node->GetNode("PositionParam11");
  if (PositionParam11Node != 0) {
    m_PositionParam11 = PositionParam11Node->GetValueAsDouble();
  }
  MXmlNode* PositionFileNameNode = Node->GetNode("PositionFileName");
  if (PositionFileNameNode != 0) {
    m_PositionFileName = PositionFileNameNode->GetValueAsString();
  }
  MXmlNode* EnergyParam1Node = Node->GetNode("EnergyParam1");
  if (EnergyParam1Node != 0) {
    m_EnergyParam1 = EnergyParam1Node->GetValueAsDouble();
  }
  MXmlNode* EnergyParam2Node = Node->GetNode("EnergyParam2");
  if (EnergyParam2Node != 0) {
    m_EnergyParam2 = EnergyParam2Node->GetValueAsDouble();
  }
  MXmlNode* EnergyParam3Node = Node->GetNode("EnergyParam3");
  if (EnergyParam3Node != 0) {
    m_EnergyParam3 = EnergyParam3Node->GetValueAsDouble();
  }
  MXmlNode* EnergyParam4Node = Node->GetNode("EnergyParam4");
  if (EnergyParam4Node != 0) {
    m_EnergyParam4 = EnergyParam4Node->GetValueAsDouble();
  }
  MXmlNode* EnergyParam5Node = Node->GetNode("EnergyParam5");
  if (EnergyParam5Node != 0) {
    m_EnergyParam5 = EnergyParam5Node->GetValueAsDouble();
  }
  MXmlNode* EnergyFileNameNode = Node->GetNode("EnergyFileName");
  if (EnergyFileNameNode != 0) {
    m_EnergyFileName = EnergyFileNameNode->GetValueAsString();
  }
  MXmlNode* EnergyFunctionStringNode = Node->GetNode("EnergyFunctionString");
  if (EnergyFunctionStringNode != 0) {
    m_EnergyFunctionString = EnergyFunctionStringNode->GetValueAsString();
  }
  MXmlNode* NormalizedEnergyPositionFluxFileNameNode = Node->GetNode("NormalizedEnergyPositionFluxFileName");
  if (NormalizedEnergyPositionFluxFileNameNode != 0) {
    m_NormalizedEnergyPositionFluxFileName = NormalizedEnergyPositionFluxFileNameNode->GetValueAsString();
  }
  MXmlNode* LightCurveFileNameNode = Node->GetNode("LightCurveFileName");
  if (LightCurveFileNameNode != 0) {
    m_LightCurveFileName = LightCurveFileNameNode->GetValueAsString();
  }
  MXmlNode* IsRepeatingLightCurveNode = Node->GetNode("IsRepeatingLightCurve");
  if (IsRepeatingLightCurveNode != 0) {
    m_IsRepeatingLightCurve = IsRepeatingLightCurveNode->GetValueAsBoolean();
  }


  // Do an official "set" to initialize all the other variables:
  if (SetSpectralType(m_SpectralType) == false) return false;
  if (SetEnergy(m_EnergyParam1, m_EnergyParam2, m_EnergyParam3, m_EnergyParam4, m_EnergyParam5) == false) return false; 
  if (SetBeamType(m_BeamType) == false) return false;
  if (SetLightCurveType(m_LightCurveType) == false) return false;
  if (SetPosition(m_PositionParam1, m_PositionParam2, m_PositionParam3,
              m_PositionParam4, m_PositionParam5, m_PositionParam6, 
              m_PositionParam7, m_PositionParam8, m_PositionParam9, 
              m_PositionParam10, m_PositionParam11) == false) return false;
  if (SetPosition(m_PositionFileName) == false) return false;
  if (SetFlux(m_InputFlux) == false) return false; // Set the flux before the energy !!
  if (SetEnergy(m_EnergyFileName) == false) return false;
  if (SetEnergyFunctionString(m_EnergyFunctionString) == false) return false;
  if (SetNormalizedEnergyPositionFluxFunction(m_NormalizedEnergyPositionFluxFileName) == false) return false;
  if (SetLightCurve(m_LightCurveFileName, m_IsRepeatingLightCurve) == false) return false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NSource::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, "Source");
  
  new MXmlNode(Node, "Name", m_Name);
  new MXmlNode(Node, "SpectralType", m_SpectralType);
  new MXmlNode(Node, "BeamType", m_BeamType);
  new MXmlNode(Node, "LightCurveType", m_LightCurveType);
  new MXmlNode(Node, "Flux", m_InputFlux);
  new MXmlNode(Node, "PositionParam1", m_PositionParam1);
  new MXmlNode(Node, "PositionParam2", m_PositionParam2);
  new MXmlNode(Node, "PositionParam3", m_PositionParam3);
  new MXmlNode(Node, "PositionParam4", m_PositionParam4);
  new MXmlNode(Node, "PositionParam5", m_PositionParam5);
  new MXmlNode(Node, "PositionParam6", m_PositionParam6);
  new MXmlNode(Node, "PositionParam7", m_PositionParam7);
  new MXmlNode(Node, "PositionParam8", m_PositionParam8);
  new MXmlNode(Node, "PositionParam9", m_PositionParam9);
  new MXmlNode(Node, "PositionParam10", m_PositionParam10);
  new MXmlNode(Node, "PositionParam11", m_PositionParam11);
  new MXmlNode(Node, "PositionFileName", CleanPath(m_PositionFileName));
  new MXmlNode(Node, "EnergyParam1", m_EnergyParam1);
  new MXmlNode(Node, "EnergyParam2", m_EnergyParam2);
  new MXmlNode(Node, "EnergyParam3", m_EnergyParam3);
  new MXmlNode(Node, "EnergyParam4", m_EnergyParam4);
  new MXmlNode(Node, "EnergyParam5", m_EnergyParam5);
  new MXmlNode(Node, "EnergyParam6", m_EnergyParam6);
  new MXmlNode(Node, "EnergyParam7", m_EnergyParam7);
  new MXmlNode(Node, "EnergyFileName", CleanPath(m_EnergyFileName));
  new MXmlNode(Node, "EnergyFunctionString", m_EnergyFunctionString);
  new MXmlNode(Node, "NormalizedEnergyPositionFluxFileName", CleanPath(m_NormalizedEnergyPositionFluxFileName));
  new MXmlNode(Node, "LightCurveFileName", CleanPath(m_LightCurveFileName));
  new MXmlNode(Node, "IsRepeatingLightCurve", m_IsRepeatingLightCurve);
  
  return Node;
}


////////////////////////////////////////////////////////////////////////////////


TString NSource::CleanPath(TString Path)
{
  //! Check for $NUSIM paths in the given file name and replace it if possible

  TString ToBeReplaced = "$NUSIM";
  
  if (ToBeReplaced == "" || ToBeReplaced == "/") return Path;

  MFile::ExpandFileName(ToBeReplaced);
  
  TString ToBeReplacedTrunk = ToBeReplaced;
  ToBeReplacedTrunk += "_trunk/";
  ToBeReplaced += "/";

  // First replace the extended version
  Path = Path.ReplaceAll(ToBeReplacedTrunk, "$NUSIM/");
  Path = Path.ReplaceAll(ToBeReplaced, "$NUSIM/");

  return Path;  
}


/*
 * NSource.cc: the end...
 ******************************************************************************/
