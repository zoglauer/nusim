/* program to smooth an RMF with a Gaussian kernel to emulate electronic noise
   Uses Keith Arnaud's rmf class in heasp.tar
   A. Ptak, 2/2012
   
*/
#ifndef HAVE_HEASP
#include "heasp.h"
#endif
#ifndef HAVE_RMF
#include "RMF.h"
#endif
#ifndef HAVE_SPIO
#include "SPIO.h"
#endif
#include <cmath>
#include <cstdlib>
#ifndef PI
#define PI 3.1415926535897931
#endif

using namespace std;

int main(int argc, char* argv[]) {

  if (argc < 4) {
    cout << "Usage: " << argv[0] << " inrmf outrmf noise_level (rmfnum)\n";
    cout << "Image rows are convolved with Guassian based on noise_level (in eV)\n";
    return(1);
  }

  RMF rmf;
  int rmfnum = 1;
  if (argc > 4)
    rmfnum = strtol(argv[4], 0, NULL);
  cout << "Selecting rmf #" << rmfnum << " from " << argv[1] << '\n';
  rmf.readMatrix(argv[1], rmfnum);
  rmf.readChannelBounds(argv[1]);
  cout << "Read " << argv[1] << '\n';
  rmf.disp();
  cout << rmf.Instrument << '\n';
  cout << "size of ChannelLowEnergy = " << rmf.ChannelLowEnergy.size() << '\n';
  cout << "size of LowEnergy = " << rmf.LowEnergy.size() << '\n';
  cout << "Matrix size = " << rmf.Matrix.size() << '\n';
  cout << "Number of Total Elements = " << rmf.NumberTotalElements() << '\n';
  cout << "Number of Channels = " << rmf.NumberChannels() << '\n';
  double sigma_ev = strtod(argv[3], NULL);
  cout << "e- noise in eV =  " << sigma_ev << '\n';
  // Get sigma in channels
  double dE = rmf.ChannelHighEnergy[0]-rmf.ChannelLowEnergy[0];
  cout << "Channel size = " << dE << " keV\n";
  double sigma = (sigma_ev/1000.)/dE;
  cout << "e- noise in channels = " << sigma << '\n';
  double norm = sqrt(2*PI)*sigma;

  // Now step through each row and convolve
  RealArray tmpdata;
  tmpdata.resize(rmf.NumberChannels());
  double z, zz;
  size_t i, j, k, irow;

  // files to dump a sample row
  ofstream fil1("row1000_orig.dat");
  ofstream fil2("row1000_conv.dat");
  double QEbefore, QEafter;
  for (i=0; i<rmf.NumberEnergyBins(); i++) {
    // reset tmpdata
    irow = i*rmf.NumberChannels();
    cout << (i+1) << " " << rmf.LowEnergy[i] << " - " << rmf.HighEnergy[i] << " keV: ";
    QEbefore = 0.;
    QEafter = 0.;
    for (j=0; j<rmf.NumberChannels(); j++) {
      tmpdata[j] = 0.;
      QEbefore += rmf.Matrix[irow + j];
      for (k=0; k<rmf.NumberChannels(); k++) {
	  z = (1.*k-j)/sigma;
	  zz = z*z/2.;
	  if (zz < 10.)
	    tmpdata[j] += rmf.Matrix[irow + k]*exp(-zz)/norm;
	  if (tmpdata[j] < 0.)
	    tmpdata[j] = 0.;
      }
    }

    for (j=0; j<rmf.NumberChannels(); j++) {
      if (i == 1000) {
	fil1 << j << ' ' << rmf.Matrix[irow + j] << '\n';
	fil2 << j << ' ' << tmpdata[j] <<'\n';
      }
      rmf.Matrix[irow + j] = tmpdata[j];
      QEafter += tmpdata[j];
    }
    if (i == 1000) {
      fil1.flush();
      fil2.flush();
    }
    cout << QEbefore << " " << QEafter << "\n";
  }
  //double E0, Escale, yE0, yEscale;
  cout << "Saving smoothed RMF to " << argv[2] << '\n';
  rmf.writeMatrix(argv[2]);
  rmf.writeChannelBounds(argv[2]);
  return(0);
  
}
