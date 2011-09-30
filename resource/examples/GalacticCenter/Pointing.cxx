/*
  Use this file to generate the default pointing for the galactic center region.
  It requires files from the xephem astrolib from:
  http://www.clearskyinstitute.com/xephem/
  From the source code package, get the files 
  * astro.h
  * eq_gal.c
  * mjd.c
  * precess.c
  You compile with:
  g++ eq_gal.c mjd.c precess.c Pointing.cxx -o Pointing
*/


#include <iostream>
#include <fstream>
using namespace std;

#include "astro.h"

int main()
{
  double deg = 3.14159265359/180;
  double arcmin = deg/60.0;

  double HalfField = 6.7*arcmin;

  double LatMin = -0.4*deg + HalfField;
  double LatMax = +0.4*deg - HalfField;

  double LongMin = -1*deg + HalfField;
  double LongMax = +1*deg - HalfField;


  cout<<"HalfField-pointings: = "<<(LatMax-LatMin)/HalfField * (LongMax-LongMin)/HalfField<<endl;

  int LatPointings = int((LatMax-LatMin)/HalfField)+1;
  int LongPointings = int((LongMax-LongMin)/HalfField)+1;

  if (LatPointings % 2 == 1) LatPointings++;
  if (LongPointings % 2 == 1) LongPointings++;
  
  double Ra;
  double Lat;
  double LatStart = -double(LatPointings-1)/2.0*HalfField;
  double Dec;
  double Long;
  double LongStart = -double(LongPointings-1)/2.0*HalfField;
  ofstream fout;
  fout.open("Pointings.pat");
  for (unsigned int o = 0; o < LongPointings; ++o) {
    for (unsigned int a = 0; a < LatPointings; ++a) {
      Lat = LatStart + a*HalfField;
      Long = LongStart + o*HalfField;
      gal_eq(J2000, Lat, Long, &Ra, &Dec);
      cout<<"Pointing: lat="<<Lat/deg<<" deg, long="<<Long/deg<<"  ---  RA="<<Ra/deg<<"  DEC="<<Dec/deg<<endl;
      fout<<"RD "<<Ra/deg<<" "<<Dec/deg<<" 109 10"<<endl;
    }
  }  
  fout.close();

  return 0;
}
