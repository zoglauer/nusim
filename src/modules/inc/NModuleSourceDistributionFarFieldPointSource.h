/*
 * NModuleSourceDistributionFarFieldPointSource.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleSourceDistributionFarFieldPointSource__
#define __NModuleSourceDistributionFarFieldPointSource__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceEntry.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////

/**
 * The class is an entry module determining the inital parameters of the
 * event above the optics.
 */

class NModuleSourceDistributionFarFieldPointSource : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleSourceDistributionFarFieldPointSource(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleSourceDistributionFarFieldPointSource();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Set the right ascension in deg
  void SetRightAscension(double RightAscension) { m_RightAscension = RightAscension; }
  //! Get the right ascension in deg
  double GetRightAscension() const { return m_RightAscension; }

  //! Set the declination in deg
  void SetDeclination(double Declination) { m_Declination = Declination; }
  //! Get the declination in deg
  double GetDeclination() const { return m_Declination; }

  //! Set the extent of the source
  void SetRadiusExtent(double RadiusExtent) { m_RadiusExtent = RadiusExtent; }
  //! Get the extent of the source
  double GetRadiusExtent() const { return m_RadiusExtent; }

  //! Set the energy
  void SetEnergy(double Energy) { m_Energy = Energy; }
  //! Get the energy
  double GetEnergy() const { return m_Energy; }

  //! Set the flux
  void SetFlux(double Flux) { m_Flux = Flux; }
  //! Get the flux
  double GetFlux() const { return m_Flux; }


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! Source position right ascension in deg
  double m_RightAscension;
  //! Source position declination in deg
  double m_Declination;
  //! Line energy
  double m_Energy;
  //! Line flux
  double m_Flux;
  //! The extent of the line
  double m_RadiusExtent;
    


#ifdef ___CINT___
 public:
  ClassDef(NModuleSourceDistributionFarFieldPointSource, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
