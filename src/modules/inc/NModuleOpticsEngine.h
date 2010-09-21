/*
 * NModuleOpticsEngine.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOpticsEngine__
#define __NModuleOpticsEngine__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>





// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfacePhotonPropagation.h"


// Forward declarations:


////////////////////////////////////////////////////////////////////////////////



class NModuleOpticsEngine : public NModule, public NModuleInterfaceEvent, public NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NModuleOpticsEngine(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOpticsEngine();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Finalize the module
  virtual bool Finalize();

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Enable or disable scattering
  void SetUseScattering(bool UseScattering) { m_UseScattering = UseScattering; }
  //! Return true if scattering is enabled
  bool GetUseScattering() const { return m_UseScattering; }

  //! Enable or disable ghost rays
  void SetUseGhostRays(bool UseGhostRays) { m_UseGhostRays = UseGhostRays; }
  //! Return true if ghost rays are enabled
  bool GetUseGhostRays() const { return m_UseGhostRays; }

  //! Enable or disable ideal optics
  void SetUseIdealOptics(bool UseIdealOptics) { m_UseIdealOptics = UseIdealOptics; }
  //! Return true if ideal optics is enabled
  bool GetUseIdealOptics() const { return m_UseIdealOptics; }
  
  // protected methods:
 protected:
  //! Perform the actual ray trace
  int RayTrace(float e_photon_lo, 
               vector<float>& k,
               vector<float>& r);

  //! Read in the shell angles
  bool LoadAngles();

  //! Read in the MLI attentuation
  bool LoadMLI();

  //! Read the reflectivity
  bool LoadReflectivity();

  //! Read the geometry
  bool LoadGeometry();

  void MovePhoton(vector<float>& r, vector<float>& k, float z_final);
  float Reflection(vector<float>& r,vector<float>& k,float alpha,float scatter);
  int FindIndex(float x, vector<float>& array, int n);
  int FindIndexReverse(float x, vector<float>& array, int n);
  int GetMirrorGroup(float alpha, int n);
  float ConeReflectionPoint(vector<float>& k,vector<float>& r,float apex,float alpha);
  float AverageReflection(float inc_angle,int group,int e_idx);
  float Gasdev();
  int Spider(vector<float>& r);
  float InterpolateMLI(float e_photon);

  float Square(float a) { return a*a; }


  // private methods:
 private:



  // protected members:
 protected:
  //! The number of shells
  int m_NShells;
  //! The number of groups (what groups)
  int m_NGroups;
  //!
  vector<float> m_ShellRange;
  //!
  vector<vector<vector<float> > > m_Reflectivity;
  //!
  vector<float> m_MLIatt;
  //!
  vector<float> m_MLIenergy;

  vector<float> m_Rm1;
  vector<float> m_Rm2;
  vector<float> m_Rm3;
  vector<float> m_Rm4;
  vector<float> m_Alpha1;
  vector<float> m_Alpha2;

  // Special geometry variables

  //! Focal length of the optics
  float m_FocalLength;
  //! Length of the shells
  float m_ShellLength;
  //! Gap between upper and lower mirror sections
  float m_Gap;  
  //! The substrate thickness
  float m_SubstrateThickness;
  //! Angular grid distance
  float m_AngularMesh;
  //! Energy grid distance
  float m_EnergyMesh;

  //! True if scattering is activated
  bool m_UseScattering;

  //! True if ideal optics is activated
  bool m_UseIdealOptics;
  
  //! True if ghost rays are activated
  bool m_UseGhostRays;
  
  // private members:
 private:
  //! Number of photons which are blocked due to MLI
  unsigned int m_BlockedPhotonsMLI;
  //! Number photons which are blocked because they do not reached the optics plane from above
  unsigned int m_BlockedPhotonsPlaneNoReached;
  //! Number of photons which are blocked, because they do not reach the mirror opening
  unsigned int m_BlockedPhotonsOpeningNotReached;
  //! Number of photons which are blocked because their energy is above the scatter limit
  unsigned int m_BlockedPhotonsEnergyTooHigh;
  //! Number of photons which do not exit the optics module
  unsigned int m_BlockedPhotonsDoNotExitOptics;

  unsigned int m_ScatteredPhotons;
  //! Single reflected photons from the upper mirror
  unsigned int m_UpperGhosts;
  //! Single reflected photons from the lower mirror
  unsigned int m_LowerGhosts;

#ifdef ___CINT___
 public:
  ClassDef(NModuleOpticsEngine, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
