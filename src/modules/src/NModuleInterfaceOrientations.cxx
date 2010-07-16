/*
 * NModuleInterfaceOrientations.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceOrientations
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceOrientations.h"

// Standard libs:
#include <cstdlib>
#include <algorithm>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MTokenizer.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceOrientations)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceOrientations::NModuleInterfaceOrientations() : NOrientations()
{
  // Construct an instance of NModuleInterfaceOrientations

	// Some dummy initializations
	m_SpaceCraftRelInertial.SetTranslation(0, 0, 0);
	m_FocalPlaneRelSC.SetTranslation(0, 0, 0);
  m_OpticalBench.SetTranslation(0, 0, 10000);

  m_Optics1RelOB.SetTranslation(400, 200, 0);
  m_Optics2RelOB.SetTranslation(-400, 200, 0);
  m_Aperture1.SetTranslation(400, 200, 1000.0);
  m_Aperture2.SetTranslation(-400, 200, 1000.0);

  m_MetrologyLaser1RelOB.SetTranslation(500, 0, 0.0);
  m_MetrologyDetector1.SetTranslation(500, 0, 0.0);
  m_MetrologyLaser2RelOB.SetTranslation(-500, 0, 0.0);
  m_MetrologyDetector2.SetTranslation(-500, 0, 0.0);
  m_MetrologyLaser3RelOB.SetTranslation(0, 500, 0.0);
  m_MetrologyDetector3.SetTranslation(0, 500, 0.0);

  m_FocalPlaneModule1.SetTranslation(400, 200, 0.0);
  m_FocalPlaneModule2.SetTranslation(-400, 200, 0.0);

  m_FocalPlaneModule1Detector1RelFocalPlaneModule1.SetTranslation(-10.25, -10.25, 0.0);
  m_FocalPlaneModule1Detector1RelFocalPlaneModule1.SetRotation(0.0, 0.0, 90.0*c_Rad);
  m_FocalPlaneModule1Detector2RelFocalPlaneModule1.SetTranslation(-10.25, +10.25, 0.0);
  m_FocalPlaneModule1Detector2RelFocalPlaneModule1.SetRotation(0.0, 0.0, 180.0*c_Rad);
  m_FocalPlaneModule1Detector3RelFocalPlaneModule1.SetTranslation(+10.25, -10.25, 0.0);
  m_FocalPlaneModule1Detector3RelFocalPlaneModule1.SetRotation(0.0, 0.0, 270.0*c_Rad);
  m_FocalPlaneModule1Detector4RelFocalPlaneModule1.SetTranslation(+10.25, +10.25, 0.0);
  m_FocalPlaneModule1Detector4RelFocalPlaneModule1.SetRotation(0.0, 0.0, 0.0);

  m_FocalPlaneModule2Detector1RelFocalPlaneModule2.SetTranslation(-10.25, -10.25, 0.0);
  m_FocalPlaneModule2Detector1RelFocalPlaneModule2.SetRotation(0.0, 0.0, 90.0*c_Rad);
  m_FocalPlaneModule2Detector2RelFocalPlaneModule2.SetTranslation(-10.25, +10.25, 0.0);
  m_FocalPlaneModule2Detector2RelFocalPlaneModule2.SetRotation(0.0, 0.0, 180.0*c_Rad);
  m_FocalPlaneModule2Detector3RelFocalPlaneModule2.SetTranslation(+10.25, -10.25, 0.0);
  m_FocalPlaneModule2Detector3RelFocalPlaneModule2.SetRotation(0.0, 0.0, 270.0*c_Rad);
  m_FocalPlaneModule2Detector4RelFocalPlaneModule2.SetTranslation(+10.25, +10.25, 0.0);
  m_FocalPlaneModule2Detector4RelFocalPlaneModule2.SetRotation(0.0, 0.0, 0.0);
  
  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceOrientations::~NModuleInterfaceOrientations()
{
  // Delete this instance of NModuleInterfaceOrientations
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceOrientations::ReadOrientations(TString FileName)
{
	//! Read the data from file

	m_Orientations.clear();

	  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: "<<FileName<<endl;
    return false;
  }

  // Read all data
  TString Line;
  TString Positions;
  TString Rotations;
	int LineCounter = 0;
  while(!in.eof()) {
    Line.ReadLine(in);
		++LineCounter;
    if (Line.Length() > 0) {

      // cal xyz1
			if (LineCounter == 5) {
        //cout<<"cal xyz1"<<endl;
				// Read the calibrated orientations of optical axis and laser pointing
				MTokenizer T(',', false);
				T.Analyze(Line);
				if (T.GetNTokens() == 13) { 
					// Attention: MTokenizer SKIPS empty tokens!
					m_CalibratedOpticalAxis1RelOM1.SetXYZ(T.GetTokenAtAsDouble(1),T.GetTokenAtAsDouble(2),T.GetTokenAtAsDouble(3));
					m_CalibratedOpticalAxis2RelOM2.SetXYZ(T.GetTokenAtAsDouble(4),T.GetTokenAtAsDouble(5),T.GetTokenAtAsDouble(6));
					m_CalibratedPointingMetrologyLaser1RelML1.SetXYZ(T.GetTokenAtAsDouble(7),T.GetTokenAtAsDouble(8),T.GetTokenAtAsDouble(9));
					m_CalibratedPointingMetrologyLaser2RelML2.SetXYZ(T.GetTokenAtAsDouble(10),T.GetTokenAtAsDouble(11),T.GetTokenAtAsDouble(12));
				} else {
          cerr<<"Parsing failed: Something is wrong with your orientations file - optical axis and laser pointing: "<<T.GetNTokens()<<endl;
          cerr<<Line<<endl;
          in.close();
          return false;            
				}
			}

      // cal xyz2
      if (LineCounter == 6) {
        //cout<<"cal xyz2"<<endl;
				// Read the calibrated orientations of optical axis and laser pointing
				
        Positions = Line;
        Line.ReadLine(in);
        ++LineCounter;
        Rotations = Line;
        if (m_Calibrations.ParseCalibrations(Positions, Rotations) == false) {
          cerr<<"Parsing failed (calibrated orientations): Something is wrong with your orientations file"<<endl;
          cerr<<Line<<endl;
          in.close();
          return false;            
        }
			}

      // std dev component
			if (LineCounter == 8) {
        //cout<<"stdev comp"<<endl;
			  // Read the errors on metrology and star tracker
				MTokenizer T(',', false);
				T.Analyze(Line);
				if (T.GetNTokens() == 7) {
					m_PointingKnowledgeErrorStarTracker4 = T.GetTokenAtAsDouble(5); // arcmin
					m_CentroidingErrorMetrologyDetector1 = T.GetTokenAtAsDouble(1); // mm
					m_CentroidingErrorMetrologyDetector2 = T.GetTokenAtAsDouble(3); // mm
				} else {
          cerr<<"Parsing failed: Something is wrong with your orientations file - optical axis and laser pointing"<<endl;
          cerr<<Line<<endl;
          in.close();
          return false;            
				}
			}

      // std dev xyz
			if (LineCounter == 9) {
        //cout<<"stdev  xyz"<<endl;
        // Read orientation errors
        Positions = Line;
        if (m_ErrorsXYZ.ParseErrors(Positions) == false) {
          cerr<<"Parsing failed (errors): Something is wrong with your orientations file"<<endl;
          cerr<<Line<<endl;
          in.close();
          return false;            
        }
			}

      // std dev DCM
			if (LineCounter == 10) {
        //cout<<"stdev  DCM"<<endl;
        // Read orientation errors
        Positions = Line;
        if (m_ErrorsDCM.ParseErrors(Positions) == false) {
          cerr<<"Parsing failed (errors): Something is wrong with your orientations file"<<endl;
          cerr<<Line<<endl;
          in.close();
          return false;            
        }
			}

      if (Line[0] != ',') {
        Positions = Line;
        Line.ReadLine(in);
        if (Line[0] != ',') {
          Rotations = Line;
          
          NOrientations O;
          if (O.Parse(Positions, Rotations) == false) {
            cerr<<"Parsing failed: Something is wrong with your orientations file"<<endl;
            in.close();
            return false;            
          }
          m_Orientations.push_back(O);
        } else {
          cerr<<"Found positions, but no rotations: Something is wrong with your orientations file"<<endl;
          in.close();
          return false;
        }
      }
    }
  } 
  in.close();

  /* 
  // Recveresing hack:
  reverse(m_Orientations.begin(), m_Orientations.end());
  for (unsigned int i = 0; i < m_Orientations.size(); ++i) {
    m_Orientations[i].SetTime(NTime(i));
  }
  */

	return true;
}


////////////////////////////////////////////////////////////////////////////////


MVector NModuleInterfaceOrientations::GetCalibratedPointingOpticalAxisRelOM(int OpticsID)
{
	//! Get the optical axis of optics module (independent of time)

  if (OpticsID == 1) {
    return m_CalibratedOpticalAxis1RelOM1;
  } else if (OpticsID == 2) {
    return m_CalibratedOpticalAxis2RelOM2;
  } else {
    cout<<"SEVERE ERROR: Optics ID = "<<OpticsID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return MVector();
  }
}


////////////////////////////////////////////////////////////////////////////////


MVector NModuleInterfaceOrientations::GetCalibratedPointingMetrologyLaserRelML(int MetrologyID)
{
	//! Get the pointing direction of the metrology laser (independent of time)

  if (MetrologyID == 1) {
    return m_CalibratedPointingMetrologyLaser1RelML1; 
  } else if (MetrologyID == 2) {
    return m_CalibratedPointingMetrologyLaser2RelML2; 
  } else if (MetrologyID == 3) {
    return m_CalibratedPointingMetrologyLaser3RelML3; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1, 2 or 3."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return MVector();
  }
}


////////////////////////////////////////////////////////////////////////////////


double NModuleInterfaceOrientations::GetCentroidingErrorMetrologyDetector(int MetrologyID)
{
	//! Get the pointing error of the star tracker

  if (MetrologyID == 1) {
    return m_CentroidingErrorMetrologyDetector1; 
  } else if (MetrologyID == 2) {
    return m_CentroidingErrorMetrologyDetector2; 
  } else if (MetrologyID == 3) {
    return m_CentroidingErrorMetrologyDetector3; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1, 2 or 3."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return 0.0;
  }
}


////////////////////////////////////////////////////////////////////////////////


double NModuleInterfaceOrientations::GetPointingErrorStarTracker(int StarTrackerID)
{
	//! Get the pointing error of the star tracker

  if (StarTrackerID == 4) {
    return m_PointingKnowledgeErrorStarTracker4;
  } else {
    cout<<"SEVERE ERROR: Star tracker ID = "<<StarTrackerID<<" not allowed. Must be 4."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return 0.0;
  }
}


// NModuleInterfaceOrientations.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
