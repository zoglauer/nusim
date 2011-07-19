/*
 * NObservatoryReconstructor.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NObservatoryReconstructor
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NObservatoryReconstructor.h"
// Standard libs:
using namespace std;

// NuSTAR libs:
#include "math.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NObservatoryReconstructor)
#endif


////////////////////////////////////////////////////////////////////////////////


NObservatoryReconstructor::NObservatoryReconstructor()
{
  // Construct an instance of NObservatoryReconstructor
}


////////////////////////////////////////////////////////////////////////////////


NObservatoryReconstructor::~NObservatoryReconstructor()
{
  // Delete this instance of NObservatoryReconstructor
}


////////////////////////////////////////////////////////////////////////////////


bool NObservatoryReconstructor::Reconstruct(NHit& Hit) 
{
  // Reconstruct alignments, aspect and pointing...

  NObservatoryData Obs;
  
  // We do not store intermediate data such as interpolated values, since in 99.99% of all cases
  // an event should have only one hit.

  if (m_MetrologyDataLowerBound.GetTime() > m_Time) {
    cout<<"Error: Time of the lower bound metrology data set is above interpolation time: lb="<<m_MetrologyDataLowerBound.GetTime()<<" vs. t="<<m_Time<<endl;
    return false;
  }
  if (m_MetrologyDataUpperBound.GetTime() < m_Time) {
    cout<<"Error: Time of the lower bound metrology data set is below interpolation time: ub="<<m_MetrologyDataUpperBound.GetTime()<<" vs. t="<<m_Time<<endl;
    return false;
  }
  if (m_StarTrackerDataLowerBound.GetTime() > m_Time) {
    cout<<"Error: Time of the lower bound star tracker data set is above interpolation time: lb="<<m_StarTrackerDataLowerBound.GetTime()<<" vs. t="<<m_Time<<endl;
    return false;
  }
  if (m_StarTrackerDataUpperBound.GetTime() < m_Time) {
    cout<<"Error: Time of the lower bound star tracker data set is below interpolation time: ub="<<m_StarTrackerDataUpperBound.GetTime()<<" vs. t="<<m_Time<<endl;
    return false;
  }

  // Interpolate metrology data
  NMetrologyData MetrologyInterp;
  MetrologyInterp.Interpolate(m_MetrologyDataLowerBound, m_MetrologyDataUpperBound, m_Time);
  //cout<<"met interp ="<<MetrologyInterp.GetMetrologyDataSet1().GetCalibratedLaserHit()<<endl;
  //cout<<"met interp ="<<MetrologyInterp.GetMetrologyDataSet2().GetCalibratedLaserHit()<<endl;

  // Interpolate the Star Tracker data
  double Fraction = (m_Time.GetAsSeconds()-m_StarTrackerDataLowerBound.GetStarTrackerDataSet4().GetTime().GetAsSeconds())/
    (m_StarTrackerDataUpperBound.GetStarTrackerDataSet4().GetTime().GetAsSeconds()-m_StarTrackerDataLowerBound.GetStarTrackerDataSet4().GetTime().GetAsSeconds());
  NQuaternion Rstin;
  Rstin.SetInterpolated(m_StarTrackerDataLowerBound.GetStarTrackerDataSet4().GetMeasuredTransformation(),
                        m_StarTrackerDataUpperBound.GetStarTrackerDataSet4().GetMeasuredTransformation(), Fraction);


  // Step 2: Determine the conversion matrix
  MVector md1 = MetrologyInterp.GetMetrologyDataSet1().GetCalibratedLaserHit();
  MVector md2 = MetrologyInterp.GetMetrologyDataSet2().GetCalibratedLaserHit();
  NOrientation Rmdfb1 = m_CalibratedOrientationMetrologyDetector1;  
  NOrientation Rmdfb2 = m_CalibratedOrientationMetrologyDetector2;  
  //cout<<"t="<<m_Time<<"md1 "<<md1<<"md2 "<<md2<<endl; 

  Rmdfb1.TransformOut(md1);  // Get the interesection of laser on the metrology detector in FB coords
  Rmdfb2.TransformOut(md2);
  
  NQuaternion Robst = m_CalibratedOrientationStarTrackerRelOpticalBench.GetRotationQuaternion();
  NOrientation Robin;
  Robin.SetRotation(Robst*Rstin);
  
  NQuaternion Qobin = Rstin*Robst;
  NOrientation Rfbob = AspectSolve(md1, md2);
    
  //MVector testphoton(0.0,0.0,1.0); 
  //cout<<"Q testphoton="<<Rstin.Invert().Rotation(testphoton)<<endl;
  //cout<<Robst.Invert().Rotation(testphoton)<<endl;
  //cout<<"Robst"<<Robst<<endl;
  //cout<<"Rstin"<<Rstin<<endl;
  //cout<<"Robin"<<Robin.ToString()<<endl;
  //cout<<"Qobin"<<Qobin<<endl;
  //cout<<"testphoton="<<testphoton<<endl;
  //cout<<m_Time<<endl; 
  //cout<<"Rfbob"<<Rfbob.ToString()<<m_Time<<endl;
    
  MVector event = Hit.GetPosition();  // Get event position in detector coordinates
  //cout<<event<<endl; 
  int module = Hit.GetTelescope();    // Get module
  MVector OMF = FindOmegaF(Rfbob,module);          // It is the location of the optics in OB coords
  MVector OA = FindOpticalAxisInSky(Robin,module); // Optical axis pointing in sky coords
  NOrientation Rfbin = FindRfbin(Rfbob,Robin);     // The focal plane to inertial transformation. Again this might be redundant.
  
  NOrientation FP;
  if (module == 1) {
    FP = m_CalibratedOrientationFocalPlaneModule1;
  } else if (module == 2) {
    FP = m_CalibratedOrientationFocalPlaneModule2;
  } else {
    cout<<"Unknown module: "<<module<<endl;
    return false;
  }
  
  // Vector implementation
  FP.TransformOut(event);      // Transform event from detector coordinates into Focal Plane
  MVector FPevent=event;
  Rfbob.TransformIn(event); 
  MVector pF = OMF-event;
  pF.Unitize();
  //cout<<OA<<endl;
  //cout<<Robst.Rotation(pF)<<event<<OMF<<endl;
  //MVector OAin = Qobin.Rotation(OA);
  //MVector pFin = Qobin.Rotation(pF); 
  Robin.TransformIn(pF); 
  Robin.TransformIn(OA); 
  
  cout.precision(10);
    //cout<<module<<OA<<event<<endl; 
    //cout<<"DCM"<<module<<pF<<OA<<T3<<endl;
    //cout<<module<<pFin<<OAin<<endl;
    //cout<<module<<pF<<OA<<endl;
	//cout<<"RA "<<atan(pF[1]/pF[0])*c_Deg<<" DEC "<<asin(pF[2])*c_Deg<<endl;
    //cout<<"RA "<<atan(OA[1]/OA[0])*c_Deg<<" DEC "<<asin(OA[2])*c_Deg<<endl;

  // Point implementation
  MVector OADet=FindOpticalAxisAtDet(Rfbob, FP, module);  // Find intersection of the optical axis on the detector and express in FB
  FP.TransformOut(OADet);
  //Rfbob.TransformIn(OADet);
  //MVector Sky = event-OADet;
  //Robin.TransformOut(Sky);
  //double pls = 6./3600;
  //double Psi = atan2(-Sky[1],Sky[0]);
  //double Theta = atan((180./c_Pi)/sqrt(pow(Sky[0]*pls,2)+pow(Sky[1]*pls,2)));
  //cout<<Psi*c_Deg<<" "<<Theta*c_Deg<<endl; 
      
  // The necessary things to save are:
  // 1) OA  - direction in inertial space of the optical axis
  // 2) pF  - The direction of the event in inertial space
  // 3) Rfob - The Transformation between FB and OB
  // 4) Robin - The transformation from OB to inertial space
  

  // All data should be stored in the obervatory data set
  Obs.SetTime(m_Time);
  Obs.SetDirectionOpticalAxisInIS(OA);
  Obs.SetDirectionEventInIS(pF);   
  //Obs.SetDirectionEventInIS(event);
  //Obs.SetDirectionOpticalAxisInIS(OADet);
  Obs.SetOrientationFocalPlaneToOB(Rfbob);
  Obs.SetOrientationOBToIS(Robin);

  Hit.SetObservatoryData(Obs);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NObservatoryReconstructor::AspectSolve(const MVector& d1f, const MVector& d2f)
{

  // d1f and d2f are the intersection of the laser spot on metrology detector in FB coords
  double s1, s2;  // scaling factors
  MVector ml1, ml2, ml1d, ml2d;
  double M = m_CalibratedOrientationOpticalBench.GetTranslation().Z(); // length of Mast
  
  NOrientation MLob1 = m_CalibratedOrientationMetrologyLaserRelOpticalBench1;
  NOrientation MLob2 = m_CalibratedOrientationMetrologyLaserRelOpticalBench2;

  //cout<<"MLob1"<<MLob1.ToString()<<endl;
  //cout<<"MLob2"<<MLob2.ToString()<<endl;
  
  NPhoton Laser1, Laser2;
  //Laser1.SetPosition(MVector(0.0, 0.0, 0.0)); // place holders - this should not be hardcoded
  Laser1.SetPosition(m_CalibratedOriginMetrologyLaserRelML1); 
  Laser1.SetDirection(m_CalibratedPointingMetrologyLaserRelML1);   
  //Laser2.SetPosition(MVector(0.0, 0.0, 0.0)); // place holders - this should not be hardcoded
  Laser2.SetPosition(m_CalibratedOriginMetrologyLaserRelML2);
  Laser2.SetDirection(m_CalibratedPointingMetrologyLaserRelML2);

  MLob1.TransformOut(Laser1);// Laser coordinates are now in OpticalBench units
  MLob2.TransformOut(Laser2);// Laser coordinates are now in OpticalBench units
  
  ml1 = Laser1.GetPosition();
  ml1d = Laser1.GetDirection();
  ml2 = Laser2.GetPosition();
  ml2d = Laser2.GetDirection();

  s1 = (-M-ml1[2]+d1f[2])/ml1d[2];  //Construct a reference plane
  s2 = (-M-ml2[2]+d2f[2])/ml2d[2];
  
  //cout<<"Laser1 P and D: "<<ml1<<" "<<ml1d<<endl;
  //cout<<"Laser2 P and D: "<<ml2<<" "<<ml2d<<endl;
  // Find laser point intersecting the above constructed plane in OB coordinates
  MVector d1p = ml1+s1*ml1d;
  MVector d2p = ml2+s2*ml2d;
  
  // Create unit vectors in terms of detector spots in OB and FB
  MVector Zf(0,0,1);
  MVector Zp(0,0,1);
  MVector Yf(d2f-d1f);
  MVector Yp(d2p-d1p);
  Yf.Unitize();
  Yp.Unitize();
  MVector Xf=Yf.Cross(Zf);
  MVector Xp=Yp.Cross(Zp);
  
  //cout<<"DP="<< d1p<<" "<<d2p<<endl;
  //cout<< Xf << Xp <<  Yf << Yp <<endl;
  
  // The respective transformations as constructed from their new basis vectors
  NOrientation Tfbs;
  Tfbs.SetRotation(Xf, Yf, Zf);
  NOrientation Tobs;
  Tobs.SetRotation(Xp, Yp, Zp);
  // Solve for the rotation 
  TMatrixD inv = Tfbs.GetRotationMatrix();   // Invert Tfbs. 
  if (fabs(inv.Determinant()) < 1E-13) {
    cout<<"Determinant is (almost) zero: "<<inv.Determinant()<<" and thus has no inverse (which we try to calculate next)!"<<endl;
    cout<<"The following calculations are ... doubtful"<<endl;
    cout<<"Matrix: "<<inv(0,0)<<":"<<inv(1,0)<<":"<<inv(2,0)<<":"<<inv(0,0)<<":"<<inv(1,1)<<":"<<inv(2,1)<<":"<<inv(0,2)<<":"<<inv(1,2)<<":"<<inv(2,2)<<endl;
  }
  Tfbs.SetRotation(inv.Invert());
  NOrientation ReconstructedFBOB=Tobs*Tfbs;  // Transformation from Focal Plane to Optics Bench
  
  // Calculate centroids of the laser spots to find out what the translation is.
  MVector rcp=(1./2.)*(d1p+d2p);  // centroid in optic coords
  MVector rcf=(1./2.)*(d1f+d2f);  // centroid in focal coords
  //cout<<"rcp="<<rcp<<endl;
  // transform rcp into FB to find the translation.
  ReconstructedFBOB.TransformOut(rcp);  
  // Assign translation to the ReconstructedFBOB object.
  ReconstructedFBOB.SetTranslation(rcf - rcp); 

  return ReconstructedFBOB;

}


////////////////////////////////////////////////////////////////////////////////


MVector NObservatoryReconstructor::FindOmegaF(NOrientation& Rfbob, int module)
{
  MVector OM;
  if (module == 1) {
    OM = m_CalibratedOrientationOpticsRelOpticalBench1.GetTranslation();
  } else if (module == 2) {
    OM = m_CalibratedOrientationOpticsRelOpticalBench2.GetTranslation();
  } else {
    cout<<"Error in FindOmegaF: unknown module "<<module<<endl;
  }
 
     return OM;
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NObservatoryReconstructor::FindRfbin(const NOrientation& Rfbob,const NOrientation& Robin)
{
  // NOrientation Robst=m_Satellite.GetCalibratedOrientationStarTrackerRelOpticalBench(4);
  // Getting rid of the translation part
  NOrientation RfbobNoTrans = Rfbob;
  RfbobNoTrans.SetTranslation(0,0,0);
  
  // Combine transformation of Optics Bench to inertial space with Focalplane Bench to Optical Bench
  NOrientation Rfbin = Robin*RfbobNoTrans;  

  return Rfbin;
}


////////////////////////////////////////////////////////////////////////////////


MVector NObservatoryReconstructor::FindOpticalAxisInSky(NOrientation Robin, const int module)
{
  MVector OA;
  NOrientation OMP;

  if (module == 1) { 
    OA = -m_CalibratedBoreSightRelOM1;
    OMP = m_CalibratedOrientationOpticsRelOpticalBench1;
  } else if (module == 2) {
    OA = -m_CalibratedBoreSightRelOM2;
    OMP = m_CalibratedOrientationOpticsRelOpticalBench2;
  } else {
    cout<<"Error in FindOpticalAxisInSky: unknown module "<<module<<endl;
  }

  NQuaternion Romob=OMP.GetRotationQuaternion();
  MVector OAob=Romob.Rotation(OA);

  return OAob;
}

////////////////////////////////////////////////////////////////////////////////


MVector NObservatoryReconstructor::FindOpticalAxisAtDet(NOrientation Rfbob, NOrientation FP, const int module)
{
  MVector OA;
  NOrientation OMP;
  NPhoton OpticalAxis;
  
  MVector DetPlane(0.0, 0.0, 0.0); // Plane in MD coordinate system
  MVector DetPlaneNormal(0.0, 0.0, 1.0);

  if (module == 1) { 
    OA = m_CalibratedBoreSightRelOM1;
    OMP = m_CalibratedOrientationOpticsRelOpticalBench1;
  } else if (module == 2) {
    OA = m_CalibratedBoreSightRelOM2;
    OMP = m_CalibratedOrientationOpticsRelOpticalBench2;
  } else {
    cout<<"Error in FindOpticalAxisInSky: unknown module "<<module<<endl;
  }

  OpticalAxis.SetPosition(MVector(0.0, 0.0, 0.0)); // optical axis is at the center of the optic
  OpticalAxis.SetDirection(OA); 
  
  OMP.TransformOut(OpticalAxis);
  Rfbob.TransformOut(OpticalAxis);
  FP.TransformIn(OpticalAxis);
  PropagateToPlane(OpticalAxis, DetPlane, DetPlaneNormal); // where OA intersects Det plane
  MVector OADet = OpticalAxis.GetPosition(); // get new coordinates in Focal Plane
  
  return OADet;
}

// NObservatoryReconstructor.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
