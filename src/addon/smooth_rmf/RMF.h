// Class definitions for RMF object

#ifndef HAVE_HEASP
#include "heasp.h"
#endif

#define HAVE_RMF 1

class RMF{
 public:

  Integer FirstChannel;              // First channel number 

  IntegerArray NumberGroups;         // Number of response groups for this energy bin 
  IntegerArray FirstGroup;           // First response group for this energy bin (counts from 0)

  IntegerArray FirstChannelGroup;   // First channel number in this group 
  IntegerArray NumberChannelsGroup; // Number of channels in this group 
  IntegerArray FirstElement;        // First response element for this group (counts from 0)
  IntegerArray OrderGroup;          // The grating order of this group 

  RealArray LowEnergy;              // Start energy of bin 
  RealArray HighEnergy;             // End energy of bin 

  RealArray Matrix;                 // Matrix elements 

  RealArray ChannelLowEnergy;       // Start energy of channel 
  RealArray ChannelHighEnergy;      // End energy of channel 

  Real AreaScaling;                 // Value of EFFAREA keyword 
  Real ResponseThreshold;           // Minimum value in response 

  string ChannelType;               // Value of CHANTYPE keyword 
  string RMFVersion;                // MATRIX extension format version 
  string EBDVersion;                // EBOUNDS extension format version 
  string Telescope;                             
  string Instrument;
  string Detector;
  string Filter;
  string RMFType;                   // Value of HDUCLAS3 keyword in MATRIX extension 
  string RMFExtensionName;          // Value of EXTNAME keyword in MATRIX extension 
  string EBDExtensionName;          // Value of EXTNAME keyword in EBOUNDS extension 

  // constructor

  RMF();

  // destructor

  ~RMF();

  // read file into object. 

  void readMatrix(string filename);
  void readMatrix(string filename, Integer RMFnumber);
  void readChannelBounds(string filename);
  void readChannelBounds(string filename, Integer RMFnumber);

  // Deep copy

  RMF& operator= (const RMF&);

  // Return information

  Integer NumberChannels();               // Number of spectrum channels 
  Integer NumberEnergyBins();             // Number of response energies 
  Integer NumberTotalGroups();            // Total number of response groups 
  Integer NumberTotalElements();          // Total number of response elements 

  Real ElementValue(Integer, Integer);    // Return the value for a particular channel
                                          // and energy

  Real* RowValues(Integer);          // Return the array for a particular energy  

  // Display information about the spectrum

  void disp();

  // Normalize the RMF so it sums to 1.0 for each energy bin

  void normalize();

  // Write response

  void writeMatrix(string filename);
  void writeChannelBounds(string filename);

};
