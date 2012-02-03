// RMF object code. Definitions in RMF.h

#ifndef HAVE_RMF
#include "RMF.h"
#endif

#ifndef HAVE_SPIO
#include "SPIO.h"
#endif

// Class RMF

// default constructor

RMF::RMF()
{
}


// Destructor

RMF::~RMF()
{
}

// reading channel bounds extension from RMF file. 

void RMF::readChannelBounds(string filename)
{
  this->readChannelBounds(filename, 1);
  return;
}

// reading channel bounds extension from PHA file. this option allows multiple extensions in the same file

void RMF::readChannelBounds(string filename, Integer EBDnumber)
{

  string hduName("EBOUNDS");
  string DefString;

  // Read in the Channel bounds extension number EBDnumber
  // and set up an object called ebd with the contents

  const vector<string> hduKeys;
  const vector<string> primaryKey;

  auto_ptr<FITS> pInfile (new FITS(filename,Read,hduName,false,hduKeys,primaryKey,(int)EBDnumber));
  ExtHDU& ebd = pInfile->extension(hduName);
  
  // read the standard keywords and store in the object

  DefString = "UNKNOWN";
  ChannelType = SPreadKey(ebd, "CHANTYPE", DefString);

  EBDVersion = SPreadKey(ebd, "HDUVERS", DefString);
  if ( EBDVersion == "UNKNOWN" ) {
    EBDVersion = SPreadKey(ebd, "HDUVERS2", DefString);
    if ( EBDVersion == "UNKNOWN" ) {
      EBDVersion = SPreadKey(ebd, "EBDVERSN", DefString);
      if ( EBDVersion == "UNKNOWN" ) EBDVersion = "1.1.0";
    }
  }

  EBDExtensionName = SPreadKey(ebd, "EXTNAME", DefString);
  
  Telescope = SPreadKey(ebd, "TELESCOP", DefString);
  
  Instrument = SPreadKey(ebd, "INSTRUME", DefString);

  Detector = SPreadKey(ebd, "DETNAM", DefString);

  Filter = SPreadKey(ebd, "FILTER", DefString);

  // Read the E_MIN and E_MAX columns

  SPreadCol(ebd,"E_MIN",ChannelLowEnergy);
  SPreadCol(ebd,"E_MAX",ChannelHighEnergy);

  return;
}



// reading Matrix extension from RMF file. 

void RMF::readMatrix(string filename)
{
  this->readMatrix(filename, 1);
  return;
}

// reading Matrix extension from PHA file. this option allows multiple extensions in the same file

void RMF::readMatrix(string filename, Integer RMFnumber)
{

  string hduName("MATRIX");
  string DefString;
  bool verbosity = FITS::verboseMode();

  // Read in the MATRIX extension number RMFnumber
  // and set up an object called rmf with the contents

  const vector<string> hduKeys;
  const vector<string> primaryKey;

  try {
    FITS::setVerboseMode(false);
    auto_ptr<FITS> pInfile (new FITS(filename,Read,hduName,false,hduKeys,primaryKey,(int)RMFnumber));
  } catch (...) {
    cerr << "Error going to extension " << RMFnumber << '\n';
    hduName = "SPECRESP MATRIX";
  }
  cout << "hduname = " << hduName << '\n';
  FITS::clearErrors();
  FITS::setVerboseMode(verbosity);
  
  auto_ptr<FITS> pInfile (new FITS(filename,Read,hduName,false,hduKeys,primaryKey,(int)RMFnumber));
  ExtHDU& rmf = pInfile->extension(hduName);
  
  // read the standard keywords and store in the object

  DefString = "UNKNOWN";
  ChannelType = SPreadKey(rmf, "CHANTYPE", DefString);

  RMFVersion = SPreadKey(rmf, "HDUVERS", DefString);
  if ( RMFVersion == "UNKNOWN" ) {
    RMFVersion = SPreadKey(rmf, "HDUVERS2", DefString);
    if ( RMFVersion == "UNKNOWN" ) {
      RMFVersion = SPreadKey(rmf, "RMFVERSN", DefString);
      if ( RMFVersion == "UNKNOWN" ) RMFVersion = "1.1.0";
    }
  }

  RMFExtensionName = SPreadKey(rmf, "EXTNAME", DefString);
  
  Telescope = SPreadKey(rmf, "TELESCOP", DefString);
  
  Instrument = SPreadKey(rmf, "INSTRUME", DefString);

  Detector = SPreadKey(rmf, "DETNAM", DefString);

  Filter = SPreadKey(rmf, "FILTER", DefString);

  RMFType = SPreadKey(rmf, "HDUCLAS3", DefString);

  AreaScaling = SPreadKey(rmf, "EFFAREA", (Real)1.0);

  ResponseThreshold = SPreadKey(rmf, "LO_THRES", (Real)1.0);

  Integer Nrows;
  Nrows = SPreadKey(rmf, "NAXIS2", (Integer)0);

  Integer NtotGroups;
  NtotGroups = SPreadKey(rmf, "NUMGRP", (Integer)0);
  
  Integer NtotElts;
  NtotElts = SPreadKey(rmf, "NUMELT", (Integer)0);
 
  // Read the ENERG_LO and ENERG_HI columns

  SPreadCol(rmf,"ENERG_LO",LowEnergy);
  SPreadCol(rmf,"ENERG_HI",HighEnergy);

  // Get the number of groups for each energy bin

  SPreadCol(rmf,"N_GRP",NumberGroups);

  // Set up the FirstGroup array - note that it counts from 0

  FirstGroup.resize(Nrows);
  Integer igrp = 0;
  for (size_t i=0; i<(size_t)Nrows; i++) {
    FirstGroup[i] = igrp;
    igrp += NumberGroups[i];
  }
  
  // If the NUMGRP keyword was not read then sum up this column to calculate it

  if ( NtotGroups == 0 ) {
    for (size_t i=0; i<(size_t)Nrows; i++) NtotGroups += NumberGroups[i];
  }

  // Read the first channel and number of channels for each group - grab the whole data 
  // into temporary arrays then place in object. Also set the FirstElement array.

  vector<IntegerArray> fchan, nchan;

  SPreadVectorCol(rmf, "F_CHAN", fchan);
  SPreadVectorCol(rmf, "N_CHAN", nchan);

  FirstChannelGroup.resize(NtotGroups);
  NumberChannelsGroup.resize(NtotGroups);
  FirstElement.resize(NtotGroups);
  size_t ipt = 0;
  size_t ielt = 0;
  for (size_t i=0; i<(size_t)Nrows; i++) {
    for (size_t j=0; j<(size_t)fchan[i].size(); j++) {
      if ( nchan[i][j] > 0 ) {
	FirstChannelGroup[ipt] = fchan[i][j];
	NumberChannelsGroup[ipt] = nchan[i][j];
	FirstElement[ipt] = ielt;
	ielt += NumberChannelsGroup[ipt];
	ipt++;
      }
    }
  }
  
  // Check for a TLMIN for the F_CHAN column

  try {
    int ChannelIndex = rmf.column("F_CHAN").index();
    ostringstream KeyStream;
    KeyStream << "TLMIN" << ChannelIndex;
    rmf.readKey(KeyStream.str(),FirstChannel);
    FirstChannel = SPreadKey(rmf, KeyStream.str(), (Integer)1);
  } catch(Table::NoSuchColumn&) {
    FirstChannel = 1;
  } catch(HDU::NoSuchKeyword&) {
    FirstChannel = 1;
  }

  // If the NUMELT keyword was not read then sum up the N_CHAN column to calculate it

  if ( NtotElts == 0 ) {
    for (size_t i=0; i<(size_t)NtotGroups; i++) NtotElts += NumberChannelsGroup[i];
  }

  // Read the matrix column into a temporary array then set the Matrix array

  vector<RealArray> elements;
  SPreadVectorCol(rmf, "MATRIX", elements);

  Matrix.resize(NtotElts);
  ipt = 0;
  for (size_t i=0; i<(size_t)Nrows; i++) {
    for (size_t j=0; j<elements[i].size(); j++) {
      Matrix[ipt++] = elements[i][j];
    }
  }

  // Read the optional order information

  vector<IntegerArray> order;
  try {
    FITS::setVerboseMode(false);
    SPreadVectorCol(rmf, "ORDER", order);
  } catch (...) {
  }

  FITS::clearErrors();
  FITS::setVerboseMode(verbosity);

  if ( order.size() > 0 ) {
    OrderGroup.resize(NtotGroups);
    ipt = 0;
    for (size_t i=0; i<(size_t)Nrows; i++) {
      for (size_t j=0; j<(size_t)NumberGroups[i]; j++) {
	OrderGroup[ipt++] = order[j][i];
      }
    }
  }

  return;
}


// Deep copy

RMF& RMF::operator= (const RMF& beta)
{

  FirstChannel = beta.FirstChannel;
  AreaScaling = beta.AreaScaling;
  ResponseThreshold = beta.ResponseThreshold;
  ChannelType = beta.ChannelType;
  RMFVersion = beta.RMFVersion;
  EBDVersion = beta.EBDVersion;
  Telescope = beta.Telescope;
  Instrument = beta.Instrument;
  Detector = beta.Detector;
  Filter = beta.Filter;
  RMFType = beta.RMFType;
  RMFExtensionName = beta.RMFExtensionName;
  EBDExtensionName = beta.EBDExtensionName;

  NumberGroups.resize(beta.NumberGroups.size());
  for (size_t i=0; NumberGroups.size(); i++) NumberGroups[i] = beta.NumberGroups[i];
  FirstGroup.resize(beta.FirstGroup.size());
  for (size_t i=0; FirstGroup.size(); i++) FirstGroup[i] = beta.FirstGroup[i];
  

  FirstChannelGroup.resize(beta.FirstChannelGroup.size());
  for (size_t i=0; FirstChannelGroup.size(); i++) FirstChannelGroup[i] = beta.FirstChannelGroup[i];
  NumberChannelsGroup.resize(beta.NumberChannelsGroup.size());
  for (size_t i=0; NumberChannelsGroup.size(); i++) NumberChannelsGroup[i] = beta.NumberChannelsGroup[i];
  FirstElement.resize(beta.FirstElement.size());
  for (size_t i=0; FirstElement.size(); i++) FirstElement[i] = beta.FirstElement[i];
  OrderGroup.resize(beta.OrderGroup.size());
  for (size_t i=0; OrderGroup.size(); i++) OrderGroup[i] = beta.OrderGroup[i];
  

  LowEnergy.resize(beta.LowEnergy.size());
  for (size_t i=0; LowEnergy.size(); i++) LowEnergy[i] = beta.LowEnergy[i];
  HighEnergy.resize(beta.HighEnergy.size());
  for (size_t i=0; HighEnergy.size(); i++) HighEnergy[i] = beta.HighEnergy[i];
  

  Matrix.resize(beta.Matrix.size());
  for (size_t i=0; Matrix.size(); i++) Matrix[i] = beta.Matrix[i];
  

  ChannelLowEnergy.resize(beta.ChannelLowEnergy.size());
  for (size_t i=0; ChannelLowEnergy.size(); i++) ChannelLowEnergy[i] = beta.ChannelLowEnergy[i];
  ChannelHighEnergy.resize(beta.ChannelHighEnergy.size());
  for (size_t i=0; ChannelHighEnergy.size(); i++) ChannelHighEnergy[i] = beta.ChannelHighEnergy[i];
  
  return *this;
}

// Return information

Integer RMF::NumberChannels()               // Number of spectrum channels 
{
  return ChannelLowEnergy.size();
}

Integer RMF::NumberEnergyBins()             // Number of response energies 
{
  return LowEnergy.size();
}

Integer RMF::NumberTotalGroups()            // Total number of response groups 
{
  return FirstChannelGroup.size();
}

Integer RMF::NumberTotalElements()          // Total number of response elements 
{
  return Matrix.size();
}

Real RMF::ElementValue(Integer ChannelNumber, Integer EnergyBin)    // Return the value for a particular channel
{                                           // and energy

  if ( ChannelNumber < FirstChannel || ChannelNumber >= FirstChannel+NumberChannels() || 
       EnergyBin < 0 || EnergyBin >= NumberEnergyBins() ) return 0.0;

  // loop round groups for this energy bin

  for(size_t i=FirstGroup[EnergyBin];i<(size_t)(FirstGroup[EnergyBin]+NumberGroups[EnergyBin]);i++) {

    if( ChannelNumber >= FirstChannelGroup[i] && 
        ChannelNumber < FirstChannelGroup[i]+NumberChannelsGroup[i]) {

      return(Matrix[FirstElement[i]+ChannelNumber-FirstChannelGroup[i]]);

    }

  }

  return 0.0;

}

// Return pointer to an array for a particular energy  

Real* RMF::RowValues(Integer EnergyBin)
{
  Real *values = new Real(NumberChannels());

  for (size_t i=0; i<(size_t)NumberChannels(); i++) values[i] = 0.0;

  // Loop round response groups for this energy

  for (size_t i=0; i<(size_t)NumberGroups[EnergyBin]; i++) {

    size_t igroup = i + FirstGroup[EnergyBin];
    size_t ivec = FirstChannelGroup[igroup];
    size_t ielt = FirstElement[igroup];

    // loop round elements in this group - adding them to the output array

    for (size_t j=0; j<(size_t)NumberChannelsGroup[igroup]; j++) values[ivec+j] += Matrix[ielt+j];

  }

  return values;
}

  // Display information about the RMF

void RMF::disp()
{

  cout << "Response information : " << endl;

  cout << "   FirstChannel        = " << FirstChannel << endl;
  cout << "   AreaScaling         = " << AreaScaling << endl;
  cout << "   ResponseThreshold   = " << ResponseThreshold << endl;
  cout << "   ChannelType         = " << ChannelType << endl;
  cout << "   RMFVersion          = " << RMFVersion << endl;
  cout << "   EBDVersion          = " << EBDVersion << endl;
  cout << "   Telescope           = " << Telescope << endl;
  cout << "   Instrument          = " << Instrument << endl;
  cout << "   Detector            = " << Detector << endl;
  cout << "   Filter              = " << Filter << endl;
  cout << "   RMFType             = " << RMFType << endl;
  cout << "   RMFExtensionName    = " << RMFExtensionName << endl;
  cout << "   EBDExtensionName    = " << EBDExtensionName << endl;

  cout << "   NumberChannels      = " << NumberChannels() << endl;
  cout << "   NumberEnergyBins    = " << NumberEnergyBins() << endl;
  cout << "   NumberTotalGroups   = " << NumberTotalGroups() << endl;
  cout << "   NumberTotalElements = " << NumberTotalElements() << endl;


  if ( NumberGroups.size() > 1 ) cout << "   NumberGroups array of size " << NumberGroups.size() << endl;
  if ( FirstGroup.size() > 1 ) cout << "   FirstGroup array of size " << FirstGroup.size() << endl;

  if ( FirstChannelGroup.size() > 1 ) cout << "   FirstChannelGroup array of size " << FirstChannelGroup.size() << endl;
  if ( NumberChannelsGroup.size() > 1 ) cout << "   NumberChannelsGroup array of size " << NumberChannelsGroup.size() << endl;
  if ( FirstElement.size() > 1 ) cout << "   FirstElement array of size " << FirstElement.size() << endl;

  if ( OrderGroup.size() > 1 ) cout << "   OrderGroup array of size " << OrderGroup.size() << endl;

  if ( LowEnergy.size() > 1 ) cout << "   LowEnergy array of size " << LowEnergy.size() << endl;
  if ( HighEnergy.size() > 1 ) cout << "   HighEnergy array of size " << HighEnergy.size() << endl;

  if ( Matrix.size() > 1 ) cout << "   Matrix array of size " << Matrix.size() << endl;

  if ( ChannelLowEnergy.size() > 1 ) cout << "   ChannelLowEnergy array of size " << ChannelLowEnergy.size() << endl;
  if ( ChannelHighEnergy.size() > 1 ) cout << "   ChannelHighEnergy array of size " << ChannelHighEnergy.size() << endl;

  // debug
  //
  //  for (size_t i=0; i<NumberGroups.size(); i++) {
  //    cout << i << "  " << NumberGroups[i];
  //    for (size_t j=0; j<NumberGroups[i]; j++) {
  //      cout << "  " << FirstChannelGroup[j+FirstGroup[i]] << "  " << NumberChannelsGroup[j+FirstGroup[i]];
  //    }
  //    cout << endl;
  //  }

  return;
}

// Normalize the RMF so it sums to 1.0 for each energy bin

void RMF::normalize()
{

  // Loop over energies

  for (size_t ie=0; ie<(size_t)NumberEnergyBins(); ie++) {

    // sum up the response in this energy

    Real sumresp = 0.0;

    for (size_t i=0; i<(size_t)NumberGroups[ie]; i++) {
       size_t igrp = i + FirstGroup[ie];
       for (size_t j=0; j<(size_t)NumberChannelsGroup[igrp]; j++) {
         sumresp += Matrix[j+FirstElement[igrp]];
       }
    }

    // divide through by the summed response

    for (size_t i=0; i<(size_t)NumberGroups[ie]; i++) {
       size_t igrp = i + FirstGroup[ie];
       for (size_t j=0; j<(size_t)NumberChannelsGroup[igrp]; j++) {
         Matrix[j+FirstElement[igrp]] /= sumresp;
       }
    }

  }

  return;
}

// Write response matrix extension. If file already exists appends.

void RMF::writeMatrix(string filename)
{

  string Blank = " ";

  vector<string> ttype;
  vector<string> tform;
  vector<string> tunit;

  // Create a new FITS file instance  

  std::auto_ptr<FITS> pFits(0);
      
  try {                
    pFits.reset( new FITS(filename,Write) );
  } catch (FITS::CantCreate) {
    return;       
  }

  // calculate the maximum number of groups and elements per row

  Integer Nrows = NumberGroups.size();
  
  Integer MaxGroups=(NumberGroups.max());
  Integer MaxElts=0;
  for (size_t i=0; i<(size_t)Nrows; i++) {
    Integer NumElts=0;
    for (size_t j=0; j<(size_t)NumberGroups[i]; j++) {
      NumElts += NumberChannelsGroup[j+FirstGroup[i]];
    }
    if ( NumElts > MaxElts ) MaxElts = NumElts;
  }

  // set up the fchan and nchan vector arrays

  vector<IntegerArray> fchan(Nrows), nchan(Nrows);
  for (size_t i=0; i<(size_t)Nrows; i++) {
    fchan[i].resize(MaxGroups);
    fchan[i] = 0;
    nchan[i].resize(MaxGroups);
    nchan[i] = 0;
    for (size_t j=0; j<(size_t)NumberGroups[i]; j++) {
      fchan[i][j] = FirstChannelGroup[j+FirstGroup[i]];
      nchan[i][j] = NumberChannelsGroup[j+FirstGroup[i]];
    }
  }

  // set up the array of matrix elements

  vector<RealArray> elements(Nrows);
  for (size_t i=0; i<(size_t)Nrows; i++) {
    Integer NumElts=0;
    for (size_t j=0; j<(size_t)NumberGroups[i]; j++) {
      NumElts += NumberChannelsGroup[j+FirstGroup[i]];
    }
    elements[i].resize(NumElts);
    elements[i] = 0.0;
    for (size_t j=0; j<(size_t)NumElts; j++) {
      elements[i][j] = Matrix[j+FirstElement[FirstGroup[i]]];
    }
  }

  // if required set up the order vector array

  vector<IntegerArray> order(Nrows);
  if ( OrderGroup.size() > 0 ) {

    for (size_t i=0; i<(size_t)Nrows; i++) {
      order[i].resize(MaxGroups);
      order[i] = 0;
      for (size_t j=0; j<(size_t)NumberGroups[i]; j++) {
	order[i][j] = OrderGroup[j+FirstGroup[i]];
      }
    }

  }

  // set up the column descriptors for those attributes which need to be 
  // written as columns

  ttype.push_back("ENERG_LO");
  tform.push_back("E");
  tunit.push_back("keV");

  ttype.push_back("ENERG_HI");
  tform.push_back("E");
  tunit.push_back("keV");

  ttype.push_back("N_GRP");
  tform.push_back("I");
  tunit.push_back(" ");

  stringstream RepeatStream;
  RepeatStream << MaxGroups;
  string Repeat(RepeatStream.str());

  ttype.push_back("F_CHAN");
  tform.push_back(Repeat+"J");
  tunit.push_back(" ");

  ttype.push_back("N_CHAN");
  tform.push_back(Repeat+"J");
  tunit.push_back(" ");
     
  if ( SPneedCol(OrderGroup) ) {
    ttype.push_back("ORDER");
    tform.push_back(Repeat+"J");
    tunit.push_back(" ");
  }

  RepeatStream.str("");
  RepeatStream << MaxElts;
  Repeat = RepeatStream.str();

  ttype.push_back("MATRIX");
  tform.push_back("PE("+Repeat+")");
  tunit.push_back(" ");

  // Create the new extension

  Table* prmf = pFits->addTable("MATRIX",Nrows,ttype,tform,tunit);
  Table& rmf = *prmf;

  // Write the standard keywords
  
  SPwriteKey(rmf, "HDUCLASS", (string)"OGIP", Blank);
    
  SPwriteKey(rmf, "HDUCLAS1", (string)"RESPONSE", Blank);

  SPwriteKey(rmf, "HDUCLAS2", (string)"RSP_MATRIX", Blank);
    
  SPwriteKey(rmf, "HDUCLAS3", RMFType, Blank);
    
  SPwriteKey(rmf, "CHANTYPE", ChannelType, "Channel type");

  SPwriteKey(rmf, "HDUVERS", RMFVersion, "OGIP version number");

  SPwriteKey(rmf, "TELESCOP", Telescope, Blank);

  SPwriteKey(rmf, "INSTRUME", Instrument, Blank);

  SPwriteKey(rmf, "DETNAM", Detector, Blank);

  SPwriteKey(rmf, "FILTER", Filter, Blank);

  SPwriteKey(rmf, "EFFAREA", AreaScaling, Blank);

  SPwriteKey(rmf, "LO_THRES", ResponseThreshold, Blank);

  SPwriteKey(rmf, "DETCHANS", NumberChannels(), "Number of channels in rmf");

  SPwriteKey(rmf, "NUMGRP", NumberTotalGroups(), "Total number of response groups");

  SPwriteKey(rmf, "NUMELT", NumberTotalElements(), "Total number of response elements");

  SPwriteKey(rmf, "TLMIN4", FirstChannel, "First channel number");

  // Write the arrays - if an array is of size 1 or all the same value 
  // it will be written as a keyword

  SPwriteCol(rmf, "ENERG_LO", LowEnergy, true);
  SPwriteCol(rmf, "ENERG_HI", HighEnergy, true);

  SPwriteCol(rmf, "N_GRP", NumberGroups, true);


  SPwriteVectorCol(rmf, "F_CHAN", fchan, true);
  SPwriteVectorCol(rmf, "N_CHAN", nchan, true);


  SPwriteVectorCol(rmf, "MATRIX", elements, true);

  if ( OrderGroup.size() > 0 ) {
    SPwriteVectorCol(rmf, "ORDER", order);
  }
  
  return;
}

// Write channel bounds extension. If file already exists appends.

void RMF::writeChannelBounds(string filename)
{

  string Blank = " ";

  vector<string> ttype;
  vector<string> tform;
  vector<string> tunit;

  // Create a new FITS file instance  

  std::auto_ptr<FITS> pFits(0);
      
  try {                
    pFits.reset( new FITS(filename,Write) );
  } catch (FITS::CantCreate) {
    return;       
  }

  // set up the column descriptors for those attributes which need to be 
  // written as columns

  ttype.push_back("CHANNEL");
  tform.push_back("I");
  tunit.push_back(" ");

  ttype.push_back("E_MIN");
  tform.push_back("E");
  tunit.push_back("keV");

  ttype.push_back("E_MAX");
  tform.push_back("E");
  tunit.push_back("keV");

  // Create the new extension

  Table* pebd = pFits->addTable("EBOUNDS",ChannelLowEnergy.size(),ttype,tform,tunit);
  Table& ebd = *pebd;

  // Write the standard keywords
  
  SPwriteKey(ebd, "HDUCLASS", (string)"OGIP", Blank);
    
  SPwriteKey(ebd, "HDUCLAS1", (string)"RESPONSE", Blank);

  SPwriteKey(ebd, "HDUCLAS2", (string)"EBOUNDS", Blank);
    
  SPwriteKey(ebd, "CHANTYPE", ChannelType, "Channel type");

  SPwriteKey(ebd, "HDUVERS", EBDVersion, "OGIP version number");

  SPwriteKey(ebd, "TELESCOP", Telescope, Blank);

  SPwriteKey(ebd, "INSTRUME", Instrument, Blank);

  SPwriteKey(ebd, "DETNAM", Detector, Blank);

  SPwriteKey(ebd, "FILTER", Filter, Blank);

  SPwriteKey(ebd, "DETCHANS", NumberChannels(), "Number of channels in ebd");

  // Generate and write the COLUMN array

  RealArray Channel(NumberChannels());
  for (size_t i=0; i<(size_t)NumberChannels(); i++) Channel[i] = i + FirstChannel;
  SPwriteCol(ebd, "CHANNEL", Channel);

  // Write the E_MIN and E_MAX arrays - if an array is of size 1 or all the 
  // same value it will be written as a keyword

  SPwriteCol(ebd, "E_MIN", ChannelLowEnergy);
  SPwriteCol(ebd, "E_MAX", ChannelHighEnergy);

  return;
}

