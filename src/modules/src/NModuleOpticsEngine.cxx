/*
 * NModuleOpticsEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOpticsEngine
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOpticsEngine.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TMath.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MFile.h"
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsOpticsEngine.h"



////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOpticsEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleOpticsEngine::NModuleOpticsEngine(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleOpticsEngine

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "NuSTAR default optics";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagOpticsEngine";

  // Set the tool tip text
  m_ToolTip = "This is the default NuSTAR optics engine.";

  // Set all types this modules handles
  m_ModuleType = c_OpticsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsApertureSimulator)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsApertureSimulator();
  
  m_UseScattering = true;
  m_UseGhostRays = true;
  m_UseIdealOptics = false;
}


////////////////////////////////////////////////////////////////////////////////


NModuleOpticsEngine::~NModuleOpticsEngine()
{
  // Delete this instance of NModuleOpticsEngine
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::Initialize()
{
  // Initialize the module 

  m_FocalLength = 10150.0; //mm   
  m_ShellLength = 227; // mm
  m_Gap	= 2; // mm
  m_SubstrateThickness = .21; // mm
  m_AngularMesh = 5e-5;  // stepsize mrad in reflectivity file
  m_EnergyMesh = .1; // keV

  m_NShells = 133;
  m_NGroups = 10; 


  // Determine the angles (shell range)
  if (LoadAngles() == false) {
    cerr<<"NModuleOpticsEngine: Unable to load angles"<<endl;
    return false;
  }

  // Load MLI 
  if (LoadMLI() == false) {
    cerr<<"NModuleOpticsEngine: Unable to load MLI"<<endl;
    return false;
  }

  // Determine the reflectivity matrix
  if (LoadReflectivity() == false) {
    cerr<<"NModuleOpticsEngine: Unable to load reflectivities"<<endl;
    return false;
  }

  // Retrieve the geometry information
  if (LoadGeometry() == false) {
    cerr<<"NModuleOpticsEngine: Unable to load geometry"<<endl;
    return false;
  }

  // Two sanity checks to make sure the surrounding sphere is capable with what we have here
  cout<<1.05*m_Rm1[m_NShells]<<endl;
  if (m_Satellite.GetSurroundingSphereRadius() < 1.05*m_Rm1[m_NShells]) {
    cerr<<"The radius of the surrounding sphere ("<<m_Satellite.GetSurroundingSphereRadius()
      <<") is smaller than the outer shell with 5% safety ("<<1.1*m_Rm1[m_NShells]<<")"<<show;
    return false;
  }
  if (fabs(m_Satellite.GetSurroundingSphereZOffsetInOpticsModuleCoordinates() - m_ShellLength) > 0.00001) {
    cerr<<"The (half) shell length in the optics module ("<<m_ShellLength
      <<") does not agree with the one in the geometry module ("
      <<m_Satellite.GetSurroundingSphereZOffsetInOpticsModuleCoordinates()<<")!"<<show;
    return false;
  }


  m_ScatteredPhotons = 0;
  m_BlockedPhotonsPlaneNoReached = 0;
  m_BlockedPhotonsOpeningNotReached = 0;
  m_BlockedPhotonsEnergyTooHigh = 0;
  m_BlockedPhotonsDoNotExitOptics = 0;
  m_UpperGhosts = 0;
  m_LowerGhosts = 0;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::AnalyzeEvent(NEvent& Event) 
{
  // This combined optics and aperture simulator

  // STEP 1: 

  // (a) Retrieve the current photon parameters
  NPhoton Photon = Event.GetCurrentPhoton();

  // (b) Retrieve the aperture orienation and position relative to the detector
  NOrientation Orientation = m_Satellite.GetOrientationOptics(Event.GetTime(), Event.GetTelescope());

  // (c) Rotate into optics coordinate system & store
  Orientation.TransformIn(Photon);
  Event.SetInitialPhotonRelOM(Photon);

  // (d) Propagate photon to where-ever this module wants it
  //     Let's assume its the plane at the top of the optics module
  
  // ATTENTION: ignore everything which is already below the top of the opening cylinder of the optics module

  //! Check for MLI attenutation
  //! This is a temporary installment
  float MLIatt = InterpolateMLI(Photon.GetEnergy()); 
  if (gRandom->Rndm() > MLIatt) {
    Event.SetBlocked(true);
    ++m_BlockedPhotonsMLI;
  }
  
  MVector TopCenterOutermostRing(0.0, 0.0, m_ShellLength);
  MVector NormalToOutermostRing(0.0, 0.0, 1.0);
  if (PropagateToPlane(Photon, TopCenterOutermostRing, NormalToOutermostRing) == false) {
    // The plane is unreachable thus we quit here
    Orientation.TransformOut(Photon);
    Event.SetBlocked(true);
    ++m_BlockedPhotonsPlaneNoReached;
    return true;
  }

  // Eliminate everything outside the outer and within the inner shell
  MVector Pos = Photon.GetPosition();
  double RadialDistance = sqrt(Pos.X()*Pos.X() + Pos.Y()*Pos.Y());
  
  if (RadialDistance <= m_Rm2[1] || RadialDistance >= m_Rm1[m_NShells]) {
    // The plane is unreachable thus we quit here
    Orientation.TransformOut(Photon);
    Event.SetBlocked(true);
    ++m_BlockedPhotonsOpeningNotReached;
    return true;
  }
  if (Photon.GetEnergy() > 84.) {
    Event.SetBlocked(true);
    ++m_BlockedPhotonsEnergyTooHigh;
    return true;
  }

  // Step 2: Prepare and do the ray-trace:

  vector<float> RTDir(4); // "4" since the raytrace code assumes 1 is the first index
  vector<float> RTPos(4);
  vector<float> InPos(4);

  RTPos[1] = Pos[0];
  RTPos[2] = Pos[1];
  RTPos[3] = m_ShellLength;

  RTDir[1] =  Photon.GetDirection()[0];
  RTDir[2] =  Photon.GetDirection()[1];
  RTDir[3] =  Photon.GetDirection()[2];
  
  //************** Perfect Optic
  // Determine the location of the 'ideal' spot from the source angle
  
  NPhoton iPhoton;  // Ideal photon
  if (m_UseIdealOptics) {
    
	  NOrientation FPorient = m_Satellite.GetOrientationFocalPlaneModule(Event.GetTime(), Event.GetTelescope());
    
	  iPhoton.SetPosition(MVector(0.0, 0.0, 0.0)); 
	  iPhoton.SetDirection(MVector(RTDir[1], RTDir[2], RTDir[3]));
 
      Orientation.TransformIn(iPhoton);
	  FPorient.TransformOut(iPhoton);
	
	  MVector DetectorTop(0.0, 0.0, 0.0);
      MVector DetectorNormal(0.0, 0.0, 1.0);
	  PropagateToPlane(iPhoton, DetectorTop, DetectorNormal);
		
	  FPorient.TransformIn(iPhoton);
	  Orientation.TransformOut(iPhoton);
	
    InPos[1] = 0.0;
    InPos[2] = 0.0;
    InPos[3] = 0.0; 
    MovePhoton(InPos, RTDir, -m_FocalLength);
  }
  //*************
   
  int Code = RayTrace(Photon.GetEnergy(), RTDir, RTPos); 
  if (Code == 0) {
    Orientation.TransformOut(Photon);
    Event.SetBlocked(true);
    ++m_BlockedPhotonsDoNotExitOptics;
    return true;
  }
  if (Code == 3) ++m_UpperGhosts;
  if (Code == 2) ++m_LowerGhosts;

  if (m_UseGhostRays) ++m_ScatteredPhotons;
  else {
    if (Code == 1) ++m_ScatteredPhotons;
	else Event.SetBlocked(true);
  }
  //cout<<"Passed ray trace"<<endl;

  // Step 3: 

  // (a) Update the current photon data
  //mimp<<"Warning: had to invert direction x/y to make it work. kristin, remove warning when verified that there are no errors..."<<show;  
  //Due to the fact that the raytrace coordinate system is upside down as opposed to the optics coordinate system, and the module
  //is fed coordinates in optics coordinates, once the raytrace is done with them, they have to be put back into the optics coordinates. 
  //Z position coordinate has to be inverted, x/y direction coordinates have to be flipped.
  Photon.SetPosition(MVector(RTPos[1], RTPos[2], RTPos[3]));    
  Photon.SetDirection(MVector(RTDir[1], RTDir[2], RTDir[3]));
  
  //************* Perfect Optic
  // Construct the new direction cosines as being the vector that connects the exit location of the ray, RTpos, to the 'ideal' spot on the FP, InPos.
  if (m_UseIdealOptics) {
    MVector Pnew = MVector(InPos[1], InPos[2], InPos[3]) - MVector(RTPos[1], RTPos[2], RTPos[3]);   // Only focus at F
	//MVector Pnew = iPhoton.GetPosition() - MVector(RTPos[1], RTPos[2], RTPos[3]);  // follow FP and focus on the top
    Photon.SetDirection(MVector(Pnew[0],Pnew[1],Pnew[2]));
  }
  //*************

  //cout<<Photon.GetPosition()<<endl;

   // (b) Rotate back in world coordinate system
  Orientation.TransformOut(Photon);
  //cout<<"P after raytrace:"<<Photon.GetPosition()<<":"<<Photon.GetDirection()<<":"<<Photon.GetDirection().Angle(MVector(0.0, 0.0, -1.0))*c_Deg*60<<endl;

  // (c) Set the photon back into the event
  Event.SetCurrentPhoton(Photon);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::Finalize()
{  
  cout<<endl;
  cout<<"Optics engine summary:"<<endl;
  double EffectiveArea =0.0;
  double EffectiveAreaError = 0.0;
  if (m_ScatteredPhotons + m_BlockedPhotonsDoNotExitOptics > 0) {
    EffectiveArea = m_ScatteredPhotons*c_Pi*(m_Rm1[133]*m_Rm1[133]-m_Rm2[1]*m_Rm2[1])/(m_BlockedPhotonsDoNotExitOptics+m_ScatteredPhotons)/100;
    EffectiveAreaError = sqrt(m_ScatteredPhotons)*c_Pi*(m_Rm1[133]*m_Rm1[133]-m_Rm2[1]*m_Rm2[1])/(m_BlockedPhotonsDoNotExitOptics+m_ScatteredPhotons)/100;
  }
  cout<<"  Effective Area (average per module): ("<<EffectiveArea<<" +- "<<EffectiveAreaError<<") cm2"<<endl;
  cout<<endl;
  cout<<"  Number of upper mirror single reflections: "<<m_UpperGhosts<<endl;
  cout<<"  Number of lower mirror single reflections: "<<m_LowerGhosts<<endl;
  cout<<endl;
  cout<<"  Photons entering the optics: "<<m_ScatteredPhotons + m_BlockedPhotonsDoNotExitOptics<<endl;
  cout<<"  Photons exiting the optics: "<<m_ScatteredPhotons<<endl;
  cout<<"  Blocked photons: "<<m_BlockedPhotonsPlaneNoReached + m_BlockedPhotonsOpeningNotReached + m_BlockedPhotonsEnergyTooHigh + m_BlockedPhotonsDoNotExitOptics<<endl;
  cout<<"    Optics plane not reached from above: "<<m_BlockedPhotonsPlaneNoReached<<endl;
  cout<<"    Optics opening not reached:          "<<m_BlockedPhotonsOpeningNotReached<<endl;
  cout<<"    Photon energy above threshold:       "<<m_BlockedPhotonsEnergyTooHigh<<endl;
  cout<<"    Photon is blocked within optics:     "<<m_BlockedPhotonsDoNotExitOptics<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


int NModuleOpticsEngine::RayTrace(float e_photon_lo, 
                                  vector<float>& k,
                                  vector<float>& r)
{

  int
    j_index,
    e_index,           /* energy index */
    shell_index,
    mirror_group,
	flag=0;				/*flag for keeping track og ghost rays*/
  float photon_energy,
    Rmin,Rmax,    /* for restricting the simulation space *********/
    apex1,apex2,  /* cone apices */
    z_interaction,  /* z coordinate of photon reflection point */
    incidence_angle1,incidence_angle2,
    scatter;        /* level to which mirrors scatter photons in radians (normal distribution) */
  float RR; 
  float r0;

  /**************** END OF VARIABLE DEFINITIONS *****************************/
  k[1] = -k[1];
  k[2] = -k[2];
  k[3] = -k[3];
  r[1] = -r[1];
  r[2] = -r[2];
  r[3] = -r[3];
  
  r0 = sqrt(Square(r[1]) + Square(r[2]));

  // Use 0.0 if you don't want scattering
  if (m_UseScattering) scatter = 6e-5;
  else scatter = 0.0;
  Rmin = m_Rm2[1];
  Rmax = m_Rm1[133];

  e_index = (int)floor((e_photon_lo) / m_EnergyMesh);

  j_index = 1;
  photon_energy = e_photon_lo;
 
  incidence_angle1=0.;
  incidence_angle2=0.;
  /* set position */
  shell_index = FindIndexReverse(r0,m_Rm1,m_NShells); /* returns which shell photon will hit */
  apex1 = (m_Rm1[shell_index]-m_ShellLength*m_Alpha1[shell_index]) / tan(m_Alpha1[shell_index]);
  apex2 = (m_Rm1[shell_index]-m_ShellLength*m_Alpha1[shell_index]) / tan(m_Alpha2[shell_index]);
  if (shell_index == 66 || shell_index == 67 || shell_index == 68){
    //cout<<"Block 1: Shell index: "<<shell_index<<endl;
    return 0;
  }
  if (r0 < (m_Rm1[shell_index - 1] + m_SubstrateThickness)){
    //cout<<"Block 2: "<<r0<<":"<<(m_Rm1[shell_index - 1] + m_SubstrateThickness)<<endl;
    return 0;
  }
  if (Spider(r) == 1) {
    //cout<<"Block 3: Spider"<<endl;
    return 0;
  } 

  mirror_group = GetMirrorGroup(m_Alpha1[shell_index], m_NGroups+1);
  z_interaction = ConeReflectionPoint(k,r,apex1,m_Alpha1[shell_index]);

  if (z_interaction >= -m_Gap && z_interaction <= m_Gap) { 
   // miss between upper and lower mirror sections
      return 0;
  }
  if (z_interaction > m_Gap) {
    //cout<<"Block 4: "<<z_interaction<<" vs. Gap: "<<-m_Gap<<endl;
	// Ghostray: miss first reflection and flag it. 
	flag = 2;
  }
  if (z_interaction < -m_Gap && z_interaction > -m_ShellLength){   /* does it hit mirror, excluding gap */
    /*calculate reflected vector (new k,r)*/
    MovePhoton(r,k,z_interaction);
    incidence_angle1 = Reflection(r,k,m_Alpha1[shell_index],scatter);
    /*get the reflectivity and decide whether to kill the photon*/
    RR = AverageReflection(incidence_angle1,mirror_group,e_index);
    if (gRandom->Rndm() > RR) {
      //cout<<"Block 5: "<<RR<<endl;
      return 0;
    }
  }
    
  MovePhoton(r,k,-m_Gap); /* move to lower part of upper mirror */

  r0 = sqrt(Square(r[1]) + Square(r[2])); 
  if ((r0 < (m_Rm2[shell_index - 1] + m_SubstrateThickness)) || (r0 >= m_Rm2[shell_index])){
    //cout<<"Block 6: "<<endl;
    return 0;
  }
    
  MovePhoton(r,k,0); /* Intersection between upper and lower + GAP. */
  z_interaction = ConeReflectionPoint(k,r,apex2,m_Alpha2[shell_index]);
  
  if (z_interaction <= m_Gap) {
      return 0;
  }
	
  MovePhoton(r,k,m_Gap); /* move to upper part of lower mirror */
  
  if (z_interaction >=m_ShellLength) {
	// Ghost: Miss lower reflection and flag it.
	flag = 3;
  }
  
  if (z_interaction < m_ShellLength && z_interaction > m_Gap){
    /*calculate reflected vector (new k,r)*/
    MovePhoton(r,k,z_interaction);
    incidence_angle2 = Reflection(r,k,m_Alpha2[shell_index],scatter);
	/*get the reflectivity and decide whether to kill the photon*/
    RR = AverageReflection(incidence_angle2,mirror_group,e_index); 
    if (gRandom->Rndm() > RR)  {
      //cout<<"Block 8: "<<RR<<endl;
      return 0;
    }
  }

  MovePhoton(r,k,m_ShellLength);

  r0 = sqrt(Square(r[1]) + Square(r[2]));
  if (r0 < (m_Rm4[shell_index - 1] + m_SubstrateThickness)){
    //cout<<"Block 9"<<endl;
    return 0;
  }
  if (Spider(r) == 1) {
    //cout<<"Block 10"<<endl;
    return 0;
  } 
  
  k[1] = -k[1];
  k[2] = -k[2];
  k[3] = -k[3];
  r[1] = -r[1];
  r[2] = -r[2];
  r[3] = -r[3];
  
  
  if (flag == 2) return 2;
  if (flag == 3) return 3;
  
  return 1; /* succesfull double bounce hit */
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::LoadAngles()
{ 
  // Read the shell angles from file

  TString FileName = "$(NUSIM)/resource/data/OpticsAngles.dat";
  MFile::ExpandFileName(FileName);

  FILE* infile = fopen(FileName.Data(), "r");
  if (infile == 0) {
    cerr<<"Unable to open file: "<<FileName<<endl;
    return false;
  }

  int NGroups;
  if (fscanf(infile, "%d\n", &NGroups) != 1) return false;
  if (NGroups != m_NGroups) {
    cerr<<"Number of groups on file not identical with the number in the optics engine"<<endl;
    return false;
  }
  float ShellRange;
  m_ShellRange.push_back(0); // <-- not zero based...
  for (int i = 0; i <= NGroups; ++i) {
    if (fscanf(infile, "%f\n", &ShellRange) != 1) return false;
    m_ShellRange.push_back(ShellRange*1e-3); 
  }
  fclose(infile);

  return true;
}

////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::LoadMLI()
{ 
  // Read the shell angles from file

  TString FileName = "$(NUSIM)/resource/data/OpticsMLI.dat";
  MFile::ExpandFileName(FileName);

  FILE* infile = fopen(FileName.Data(), "r");
  if (infile == 0) {
    cerr<<"Unable to open file: "<<FileName<<endl;
    return false;
  }

  int NEnergy;
  if (fscanf(infile, "%d\n", &NEnergy) != 1) return false;

  float MLI_attenuation;
  float MLI_energy;
  for (int i = 0; i < NEnergy; ++i) {
    if (fscanf(infile, "%f %f\n",&MLI_energy, &MLI_attenuation) != 2) return false;
    m_MLIenergy.push_back(MLI_energy);
	m_MLIatt.push_back(MLI_attenuation);
  }
  fclose(infile);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::LoadReflectivity()
{
  // READ IN THE REFLECTIVITY FILE(S)

  int ne = 850;
  int na = 161;

  vector<vector<vector<float> > > R(m_NGroups+1, vector<vector<float> >(ne, vector<float>(na))); 
  m_Reflectivity = R;

  TString FileName = "$(NUSIM)/resource/data/OpticsReflectivity.dat";
  MFile::ExpandFileName(FileName);

  int i, l, j;
  float *ref_e; 
  char Rfile[10][80], directory[80], buffer[80];
  FILE *infile;


  /* READ IN THE FILENAMES */
  infile = fopen(FileName.Data(), "r");
  if (infile == 0) {
    cerr<<"Unable to read reflectivity master file: "<<FileName<<endl;
    return false;
  }
  if (fscanf(infile,"%s \n", directory) != 1) {
    cerr<<"Unable to read directory from reflectivity master file: "<<FileName<<endl;
    return false;
  }
  for (i=0; i < m_NGroups; i++) {
    if (fscanf(infile,"%s\n",Rfile[i]) != 1) {
      cerr<<"Unable to read reflectivity file from reflectivity master file: "<<FileName<<endl;
      return false;
    }
  } 
  fclose(infile);

  
  /* GET R-file size */

  ref_e = new float[ne+1]; // vector(0, ne);
  for (i=0; i < m_NGroups; i++) {
    FileName = directory;
    FileName += Rfile[i];
    MFile::ExpandFileName(FileName);
    
    infile = fopen(FileName.Data(),"r");
    if (infile == 0) {
      cerr<<"Unable to open file: "<<FileName.Data()<<endl;
      return false;
    }
    if (fscanf(infile, "%s %s \n", buffer, buffer) != 2) {
      cerr<<"Unable to parse first line in file: "<<FileName.Data()<<endl;
      return false;
    }
    if (fscanf(infile, "%s \n",buffer) != 1) {
      cerr<<"Unable to parse second line in file: "<<FileName.Data()<<endl;
      return false;
    }
    for (j=0; j < ne; j++){
      if (fscanf(infile, "%f ", &ref_e[j]) != 1) {
        cerr<<"Unable to parse file (energy value): "<<FileName.Data()<<endl;
        return false;
      }
      for (l=0; l < na; l++) {
        if (fscanf(infile, "%f ", &m_Reflectivity[i+1][j][l]) != 1) {
          cerr<<"Unable to parse file (reflectivity value): "<<FileName.Data()<<endl;
          return false;
        }
      }
    }
    fclose(infile);
  } /* for (i=0; i < m_NGroups; i++) */


  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::LoadGeometry()
{
  TString FileName = "$(NUSIM)/resource/data/OpticsGeometry.dat";
  MFile::ExpandFileName(FileName);

  FILE* infile = fopen(FileName.Data(), "r");
  if (infile == 0) {
    cerr<<"Unable to read file: "<<FileName<<endl;
    return false;
  }

  // Buffer is just a dummy, thus we don't care if it is overwritten
  // A getline() would have made more sense ??? 
  char buffer[80]; 
  if (fscanf(infile,"%s \n", buffer) != 1) {
    cerr<<"Unable to scan file: "<<FileName<<endl;
    return false;
  }
  if (fscanf(infile,"%s %s \n", buffer, buffer) != 2) {
    cerr<<"Unable to scan file: "<<FileName<<endl;
    return false;
  }
  if (fscanf(infile,"%s %s \n", buffer, buffer) != 2) {
    cerr<<"Unable to scan file: "<<FileName<<endl;
    return false;
  }
  if (fscanf(infile,"%s %s \n", buffer, buffer) != 2) {
    cerr<<"Unable to scan file: "<<FileName<<endl;
    return false;
  }

  // Indicea are 1 not 0 based
  m_Rm1.push_back(0);
  m_Rm2.push_back(0);
  m_Rm3.push_back(0);
  m_Rm4.push_back(0);
  m_Alpha1.push_back(0);
  m_Alpha2.push_back(0);

  float Alpha1, Rm1, Rm2, Rm3, Rm4;
  for (int i = 1; i <= m_NShells; ++i) {
    if (fscanf(infile,"%f %f %f %f %f \n", &Alpha1, &Rm1, &Rm2, &Rm3, &Rm4) != 5) return false;
    m_Rm1.push_back(Rm1);
    m_Rm2.push_back(Rm2);
    m_Rm3.push_back(Rm3);
    m_Rm4.push_back(Rm4);
    m_Alpha1.push_back(Alpha1/1000.);
    m_Alpha2.push_back(3*m_Alpha1.back());
  }
  fclose(infile);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOpticsEngine::MovePhoton(vector<float>& r, vector<float>& k, float z_final)
{
  r[1] = r[1] + (z_final - r[3])*k[1]/k[3];
  r[2] = r[2] + (z_final - r[3])*k[2]/k[3];
  r[3] = z_final;
}


////////////////////////////////////////////////////////////////////////////////


float NModuleOpticsEngine::Reflection(vector<float>& r,vector<float>& k,float alpha,float scatter)
{
  float n1,n2,n3,n_perp,r0,k_dot_n;
  float r_sphere[4],r_pancake[4],theta,phi,dot,len;
  
  r0 = sqrt(Square(r[1]) + Square(r[2]));
  n3 = -sin(alpha);
  n_perp = sqrt(1. - Square(n3));
  n1 = -n_perp * r[1]/r0;
  n2 = -n_perp * r[2]/r0;

  if (scatter != 0.){
    /* PERTURB NORMAL VECTOR */
    /* first pick on a sphere, then squash point onto the plane perp. to normal */
    theta = asin(2.*gRandom->Rndm()-1.);
    phi = 2*c_Pi*gRandom->Rndm();
    r_sphere[1] = sin(theta)*cos(phi);
    r_sphere[2] = sin(theta)*sin(phi);
    r_sphere[3] = cos(theta);
    dot = r_sphere[1]*n1 + r_sphere[2]*n2 + r_sphere[3]*n3;
    r_pancake[1] = r_sphere[1] - dot*n1;
    r_pancake[2] = r_sphere[2] - dot*n2;
    r_pancake[3] = r_sphere[3] - dot*n3;
    /* normalize vector */
    len = sqrt(Square(r_pancake[1]) + Square(r_pancake[2]) + Square(r_pancake[3]));
    r_pancake[1] = r_pancake[1]/len;
    r_pancake[2] = r_pancake[2]/len;
    r_pancake[3] = r_pancake[3]/len;
    /* make normally distributed perp. vectors */
    len = scatter*Gasdev();
    r_pancake[1] *= len;
    r_pancake[2] *= len;
    r_pancake[3] *= len;
    /* add to normal vector and renormalize */
    len = sqrt(1 + Square(len));
    n1 = (n1 + r_pancake[1])/len;
    n2 = (n2 + r_pancake[2])/len;
    n3 = (n3 + r_pancake[3])/len;
  }

  k_dot_n = n1*k[1] + n2*k[2] +n3*k[3];
  k[1] = k[1] - 2*k_dot_n*n1;
  k[2] = k[2] - 2*k_dot_n*n2;
  k[3] = k[3] - 2*k_dot_n*n3;

  return fabs(c_Pi/2. - acos(k_dot_n));
}


////////////////////////////////////////////////////////////////////////////////


int NModuleOpticsEngine::FindIndex(float x, vector<float>& array, int n)
{
  int outer,inner,test;

  inner = n;
  outer = 1;
  test = (1+n)/2;

  while (inner > outer + 1){
    if (x > array[test])
      inner = test;
    else if (x < array[test])
      outer = test;
    else {
      outer = test;
      break;
    }
    test = (inner+outer)/2;
  }
  return outer;
}


////////////////////////////////////////////////////////////////////////////////


int NModuleOpticsEngine::FindIndexReverse(float x, vector<float>& array,int n)
{
  int outer,inner,test;

  inner = 1;
  outer = n;
  test = (1+n)/2;

  while (inner+1 < outer){
    if (x > array[test])
      inner = test;
    else if (x < array[test])
      outer = test;
    else {
      outer = test;
      break;
    }
    test = (inner+outer)/2;
  }
  return outer;
}


////////////////////////////////////////////////////////////////////////////////


int NModuleOpticsEngine::GetMirrorGroup(float alpha, int n)
{
  int i;

  i = 1;
  while (alpha > m_ShellRange[i]) {
    i++;
  }
  return i-1;
}

////////////////////////////////////////////////////////////////////////////////

float NModuleOpticsEngine::InterpolateMLI(float e_photon) {

  int energy_index = FindIndexReverse(e_photon,m_MLIenergy,15); /* returns which shell photon will hit */
  
  float InterpolatedMLIatt = m_MLIatt[energy_index-1] + (m_MLIatt[energy_index]-m_MLIatt[energy_index-1])*
                             (e_photon - m_MLIenergy[energy_index-1])/(m_MLIenergy[energy_index] - m_MLIenergy[energy_index-1]);

  return InterpolatedMLIatt;
}

////////////////////////////////////////////////////////////////////////////////


float NModuleOpticsEngine::ConeReflectionPoint(vector<float>& k,vector<float>& r,float apex,float alpha)
{
  float A,B,C,Z;
  
  A = (Square(k[1]) + Square(k[2]))/Square(k[3]) - Square(tan(alpha));
  B = 2. * ( r[1]*k[1] + r[2]*k[2] ) / k[3] 
    - 2. * ( Square(k[1]) + Square(k[2]) ) * r[3]/ Square(k[3]) 
    + 2. * apex * Square(tan(alpha));
  C = Square(r[1]) + Square(r[2]) - Square(apex * tan(alpha))
    - 2. * (r[1]*k[1] + r[2]*k[2]) * r[3]/k[3] 
    + ( Square(k[1]) + Square(k[2]) ) * Square(r[3]) / Square(k[3]);
  
  Z = ( -B + sqrt(fabs(Square(B)-4.*A*C )) ) / ( 2.*A );
  return Z;
}


////////////////////////////////////////////////////////////////////////////////


float NModuleOpticsEngine::AverageReflection(float inc_angle, int group, int e_idx)
{
  int t_index_lo,t_index_hi; /* bracketing angular indices */
  float R, ref_t_lo, ref_t_hi;

  R = 0.;
  t_index_lo = (int) floor(inc_angle / m_AngularMesh);
  t_index_hi = (int) ceil(inc_angle / m_AngularMesh);
  ref_t_lo = m_Reflectivity[group][e_idx][t_index_lo];
  ref_t_hi = m_Reflectivity[group][e_idx][t_index_hi];

  R = (ref_t_lo - ref_t_hi)/(t_index_lo - t_index_hi)*(inc_angle/m_AngularMesh - t_index_hi) + ref_t_hi;
//printf(" R = %f %f %f group %d ang %f \n",R,ref_t_lo, ref_t_hi, group, inc_angle);
  return R;
}


////////////////////////////////////////////////////////////////////////////////


float NModuleOpticsEngine::Gasdev()
{
	static int iset=0;
	static float gset;
	float fac,r,v1,v2;

	if  (iset == 0) {
		do {
			v1=2.0*gRandom->Rndm()-1.0;
			v2=2.0*gRandom->Rndm()-1.0;
			r=v1*v1+v2*v2;
		} while (r >= 1.0 || r == 0.0);
		fac=sqrt(-2.0*log(r)/r);
		gset=v1*fac;
		iset=1;
		return v2*fac;
	} else {
		iset=0;
		return gset;
	}
}


////////////////////////////////////////////////////////////////////////////////


int NModuleOpticsEngine::Spider(vector<float>& r)
{ 

  float rr, 
        sector_gap, 
        spacer_width, 
        spider_width, 
        spacer_pos,
        end_of_sixths,
        check_spacer,
        check_spacer_u,
        check_spacer_l,
        check_gap,
        ang,
        radeg,
        mod30,umod30,
        mod60,umod60,
        mod15,umod15,
        mod75,umod75;


  int reject=0;
  
  radeg = 180./3.14159;

  rr = sqrt(r[1]*r[1]+r[2]*r[2]);
  ang = atan(r[1]/r[2])*radeg;
  end_of_sixths=105; // mm
  sector_gap = 1.0/rr*radeg; // mm
  spacer_width = 2.6/rr*radeg; // mm
  spider_width = 7.0/rr*radeg/2.; //mm
  spacer_pos = 2.4/rr*radeg; // mm
  check_spacer = 0.5*spacer_width;
  check_gap = sector_gap/2.;
  check_spacer_u = spacer_pos+0.5*spacer_width;
  check_spacer_l = spacer_pos-0.5*spacer_width;


  mod60 = fabs(ang-(int)(ang/60.)*60.);
  umod60 = 60-mod60;

  if ( (umod60 < spider_width) || (mod60 < spider_width) ) reject = 1;
  
  if (rr < end_of_sixths){
    mod15 = fabs(ang-(int)(ang/15.)*15.);
    umod15 = 15-mod15;
    
    if ( (umod60 < check_gap) || (mod60 < check_gap) ) reject = 1;
    if ( ( (umod60 < check_spacer_u) && (umod60 > check_spacer_l)) || 
         ( (mod60 < check_spacer_u) && (mod60 > check_spacer_l))) reject=1; 
    if ( (mod60 > check_spacer_u) && (mod60 < 60-check_spacer_u)){
      if ( (umod15 < check_spacer) || (mod15 < check_spacer)) reject=1; 
    }
  }
  if (rr > end_of_sixths) { 
    mod30 = fabs(ang-(int)(ang/30.)*30.);
    umod30 = 30-mod30;
    mod75 = fabs(ang- (int)(ang/7.5)*7.5);
    umod75 = 7.5-mod75;

    if ( (umod30 < check_gap) || (mod30 < check_gap) ) reject = 1;
    if ( ( (umod30 < check_spacer_u) && (umod30 > check_spacer_l)) || 
         ( (mod30 < check_spacer_u) && (mod30 > check_spacer_l))) reject=1; 

    if ( (mod30 > check_spacer_u) && (mod30 < 30-check_spacer_u)){
    if ( (umod75 < check_spacer) || (mod75 < check_spacer)) reject=1;
    } 
  }


  return reject;
}




////////////////////////////////////////////////////////////////////////////////


void NModuleOpticsEngine::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsOpticsEngine* Options = new NGUIOptionsOpticsEngine(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOpticsEngine::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* UseScatteringNode = Node->GetNode("UseScattering");
  if (UseScatteringNode != 0) {
    m_UseScattering = UseScatteringNode->GetValueAsBoolean();
  }
  MXmlNode* UseGhostRaysNode = Node->GetNode("UseGhostRays");
  if (UseGhostRaysNode != 0) {
    m_UseGhostRays = UseGhostRaysNode->GetValueAsBoolean();
  }
  MXmlNode* UseIdealOpticsNode = Node->GetNode("UseIdealOptics");
  if (UseIdealOpticsNode != 0) {
    m_UseIdealOptics = UseIdealOpticsNode->GetValueAsBoolean();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleOpticsEngine::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  new MXmlNode(Node, "UseScattering", m_UseScattering);
  new MXmlNode(Node, "UseGhostRays", m_UseGhostRays);
  new MXmlNode(Node, "UseIdealOptics", m_UseIdealOptics);

  return Node;
}


// NModuleOpticsEngine.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
