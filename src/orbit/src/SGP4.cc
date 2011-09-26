/* H+
 *      Title     : SGP4.cc
 *      Author    : Bryce A. Roberts
 *      Date      : 17 May 1998
 *      Synopsis  : propagates a satellite orbit based on a NORAD TLE
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */


#include <SGP4.h>

// perform time invariant calculations only once
bool vecutil::SGP4::init(void) {
  // set the year 2000 time once
  st2000.setUnix(946684800.0);

  double a1, del1, a0, del0, perigee, sStar, eta_2, eta_3, eta_4, pinvsq, 
    temp1, temp2, temp3, x3thetam1, x1m5th, xhdot1, theta_4, zeta, zeta_2, 
    zeta_3, zeta_5, beta0, beta0_3, beta0_4, C2;

  // recover original mean motion and semimajor axis from the elements
  a1=pow(KE/n0, TWOBYTHREE);
  del1=1.5*(CK2/(a1*a1))*((3*cos(i0)*cos(i0)-1.0)/pow(1-e0*e0, 1.5));
  a0=a1*(1-del1/3.0-del1*del1-(134.0/81.0)*del1*del1*del1);
  del0=(del1*a1*a1)/(a0*a0);

  // find original mean motion, n0dp (n0'') 
  n0dp=n0/(1+del0); 

  // find semimajor axis, a0dp (a0'') 
  a0dp=a0/(1-del0);

  // find the perigee (in km)
  perigee=(a0dp*(1.0-e0)-AE)*KM_PER_EARTH;
  
  // make decisions on what value of s to use based on perigee
  if (perigee<=156.0 && perigee>=98.0) {
    sStar=a0dp*(1.0-e0)-S-AE;
    q0ms4=pow(pow(Q0MS4, 0.25) + S - sStar, 4);
    }
  else if (perigee<98.0) {
    sStar=20/KM_PER_EARTH + AE;
    q0ms4=pow(pow(Q0MS4, 0.25) + S - sStar, 4);
    }
  else {
    sStar=S;
    q0ms4=Q0MS4;
    }

  theta=cos(i0); theta_2=theta*theta; 
  theta_4=theta_2*theta_2;
  zeta=1/(a0dp-sStar);
  
  zeta_2=zeta*zeta, zeta_3=zeta_2*zeta; 
  zeta_4=zeta_3*zeta;
  zeta_5=zeta_4*zeta;

  beta0_2=1-e0*e0;
  beta0=sqrt(beta0_2); beta0_3=beta0_2*beta0; 
  beta0_4=beta0_3*beta0;

  eta=a0dp*e0*zeta;
  eta_2=eta*eta, eta_3=eta_2*eta, eta_4=eta_3*eta;
  
  C2=q0ms4*(zeta_4)*n0dp*pow(1-eta_2, -7.0/2.0)*
    (a0dp*(1+ ((3.0/2.0)*eta_2) + 4*e0*eta + e0*eta_3) + 
     (3.0/2.0)*(CK2*zeta/(1-eta_2))*(-0.5 + 1.5*theta_2)*(8.0+24*eta_2 +
							  3*eta_4));
  C1=bstar*C2; C1_2=C1*C1; 
  C1_3=C1_2*C1; C1_4=C1_3*C1;
  C3=(q0ms4*(zeta_5)*A30*n0dp*AE*sin(i0))/(CK2*e0);
  C4=2*n0dp*q0ms4*(zeta_4)*a0dp*(beta0_2)*pow(1-eta_2, -7.0/2.0)*
    ((2*eta*(1+e0*eta)+0.5*e0+0.5*eta_3)-2*CK2*zeta/(a0dp*(1-eta_2))*
     (3*(1-3*theta_2)*(1 + 1.5*eta_2 -2*e0*eta-0.5*e0*eta_3)+
      0.75*(1-theta_2)*(2*eta_2-e0*eta - e0*eta_3)*cos(2*w0)));

  C5=2*q0ms4*(zeta_4)*a0dp*(beta0_2)*
    pow(1-eta_2, -7.0/2.0)*(1 + (11.0/4.0)*eta*(eta+e0)+e0*eta_3);

  D2=4*a0dp*zeta*C1_2;
  D3=(4.0/3.0)*a0dp*zeta_2*(17*a0dp+sStar)*C1_3;
  D4=TWOBYTHREE*a0dp*zeta_3*(221*a0dp+31*sStar)*C1_4;
  
  // constants for secular effects calculation
  pinvsq=1.0/(a0dp*a0dp*beta0_4);
  temp1=3.0*CK2*pinvsq*n0dp;
  temp2=temp1*CK2*pinvsq;
  temp3=1.25*CK4*pinvsq*pinvsq*n0dp;
  x3thetam1=3.0*theta_2 - 1.0;
  x1m5th=1.0 - 5.0*theta_2;
  xhdot1=-temp1*theta;

  mdot=n0dp+0.5*temp1*beta0*x3thetam1+0.0625*temp2*beta0*(13.0-
    78.0*theta_2+137.0*theta_4);

  wdot=-0.5*temp1*x1m5th+0.0625*temp2*(7.0-114.0*theta_2+
    395.0*theta_4)+temp3*(3.0-36.0*theta_2+49.0*theta_4);
  
  raandot=xhdot1+(0.5*temp2*(4.0-19.0*theta_2)+2.0*temp3*
    (3.0-7.0*theta_2))*theta;

  return(true);
  }

vecutil::StateVector 
  vecutil::SGP4::getStateVector(const tmtc::TmTcTime& st) const {
  double mdf, wdf, raandf, del_w, del_m, mp, w, RAAN, e, a, IL,
    beta_2, n, axn, ILl, aynl, ILt, ayn, U, initial, eCosE,
    eSinE, eL_2, pL, r, rDot, rfDot, cosu, sinu, u, rk, uk, RAANk, ik,
    rDotk, rfDotk, mx, my, ux, uy, uz, vx, vy, vz, tsince;
  int i;

  // find the time since the epoch, in minutes
  tsince=(st-epoch).getMinutes();

  // secular effects of atmospheric drag and gravitation
  mdf=M0+(mdot*tsince);
  wdf=w0+(wdot*tsince);
  raandf=raan+(raandot*tsince);
  
  del_w=bstar*C3*(cos(w0))*tsince;
  del_m=-TWOBYTHREE*q0ms4*bstar*zeta_4*(AE/(e0*eta))*(pow(1+eta*cos(mdf),3)
    -pow(1+eta*cos(M0), 3));
  
  mp=mdf+del_w+del_m;
  w=wdf-del_w-del_m;
  
  RAAN=raandf-(21.0/2.0)*((n0dp*CK2*theta)/(a0dp*a0dp*beta0_2))*
    C1*tsince*tsince;
  e=e0-bstar*C4*tsince-bstar*C5*(sin(mp)-sin(M0));
  
  a=a0dp*pow((1+tsince*(-C1+tsince*(-D2+tsince*(-D3-D4*tsince)))), 2);
  
  IL=mp+w+RAAN+n0dp*tsince*tsince*(1.5*C1+tsince*((D2+2*C1_2)+
    tsince*(0.25*(3*D3+12*C1*D2+10*C1_3)+tsince*(0.2*(3*D4+12*
    C1*D3+6*D2*D2+30*C1_2*D2+15*C1_4)))));

  beta_2=1-e*e;
  n=KE/pow(a, 3.0/2.0);

  // find the long-period periodic terms
  axn=e*cos(w);
  ILl=(A30*sin(i0))/(8*CK2*a*beta_2)*(e*cos(w))*((3+5*theta)/(1+theta));
  aynl=(A30*sin(i0))/(4*CK2*a*beta_2);
  ILt=IL+ILl;
  ayn=e*sin(w)+aynl;
    
  // iteratively solve Kepler's equation
  U=fmod(ILt-RAAN, TWOPI);

  initial=U;
  for (i=0; i<10; i++) {
    U=(initial-ayn*cos(U)+axn*sin(U)-U)/(1.0-ayn*sin(U)-axn*cos(U))+U;
    }

  // preliminary quantities for short period periodics
  eCosE=axn*cos(U)+ayn*sin(U); 
  eSinE=axn*sin(U)-ayn*cos(U);
  eL_2=axn*axn+ayn*ayn;
  pL=a*(1-eL_2);
  r=a*(1-eCosE);
  rDot=KE*(sqrt(a)/r)*eSinE;
  rfDot=KE*sqrt(pL)/r;
  cosu=(a/r)*(cos(U)-axn+(ayn*eSinE)/(1+sqrt(1-eL_2)));
  sinu=(a/r)*(sin(U)-ayn-(axn*eSinE)/(1+sqrt(1-eL_2)));
  u=fmod(atan2(sinu, cosu)+TWOPI, TWOPI);

  // update for short-period periodics
  rk=r*(1-1.5*CK2*sqrt(1-eL_2)/(pL*pL)*(3*theta_2-1))+
    (CK2/(2*pL))*(1-theta_2)*cos(2*u);
  uk=u+-(CK2/(4*pL*pL))*(7*theta_2-1.0)*sin(2*u);;
  RAANk=RAAN+((3*CK2*theta)/(2*pL*pL))*sin(2*u);
  ik=i0+((3*CK2*theta)/(2*pL*pL))*sin(i0)*cos(2*u);
  rDotk=rDot-((CK2*n)/pL)*(1-theta_2)*sin(2*u);
  rfDotk=rfDot+((CK2*n)/pL)*((1-theta_2)*cos(2*u)-1.5*(1-3*theta_2));
  
  // find the position and velocity components
  mx=-sin(RAANk)*cos(ik);
  my=cos(RAANk)*cos(ik);
  
  ux=mx*sin(uk)+cos(RAANk)*cos(uk);
  uy=my*sin(uk)+sin(RAANk)*cos(uk);
  uz=sin(ik)*sin(uk);
  
  vx=mx*cos(uk)-cos(RAANk)*sin(uk);
  vy=my*cos(uk)-sin(RAANk)*sin(uk);
  vz=sin(ik)*cos(uk);

  Dirvector dvPos(rk*ux*KM_PER_EARTH, rk*uy*KM_PER_EARTH, rk*uz*KM_PER_EARTH);

  Dirvector dvVel(rDotk*ux+rfDotk*vx,rDotk*uy+rfDotk*vy,rDotk*uz+rfDotk*vz);
  dvVel=dvVel*(MIN_PER_DAY*KM_PER_EARTH/(AE*86400.0));

  // precess position, velocity vectors to J2000 ECI
  dvPos.precess(st, st2000); dvVel.precess(st, st2000);

  return(StateVector(st, dvPos, dvVel, StateVector::CENTRAL_EARTH));
  }

