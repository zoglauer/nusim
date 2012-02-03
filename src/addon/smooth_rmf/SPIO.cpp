// Utility routines

#ifndef HAVE_SPIO
#include "SPIO.h"
#endif

// Read a keyword.

template<class T> T SPreadKey(ExtHDU& ext, string KeyName, T DefValue)
{

  T keyValue = DefValue;
  bool verbosity = FITS::verboseMode();
  FITS::setVerboseMode(false);

  // check whether there is a keyword called KeyName. If KeyName does not exist this
  // method fails silently and returns the input default value.

  try {
    ext.readKey(KeyName, keyValue);
  } catch (CCfits::HDU::NoSuchKeyword&) {
  }

  FITS::clearErrors();
  FITS::setVerboseMode(verbosity);

  return keyValue;

}

// Read a keyword. Note for type I files RowNumber will necessarily be 1.

template<class T> T SPreadKey(ExtHDU& ext, string KeyName, Integer RowNumber, T DefValue)
{

  T keyValue = DefValue;
  bool verbosity = FITS::verboseMode();
  FITS::setVerboseMode(false);

  // First check whether there is a column with this name with a different value for
  // for each row. If there is no column then fall back to the keyword. Note that this
  // method fails silently and returns the input default value.

  try {
    vector<T> Data;
    ext.column(KeyName).read(Data,RowNumber,1);
    keyValue = Data[0];
  } catch (CCfits::Table::NoSuchColumn&) {
    // No column so fall back to looking for a keyword called KeyName
    try {
      ext.readKey(KeyName, keyValue);
    } catch (CCfits::HDU::NoSuchKeyword&) {
    }
  }

  FITS::clearErrors();
  FITS::setVerboseMode(verbosity);

  return keyValue;

}

// Read a column

template<class T>void SPreadCol(ExtHDU& ext, string ColName, valarray<T>& Data)
{
  return SPreadCol(ext, ColName, 1, Data);
}

// Read a column - for type I data the RowNumber is necessarily 1.

template<class T>void SPreadCol(ExtHDU& ext, string ColName, Integer RowNumber, valarray<T>& Data)
{

  T keyValue;
  bool verbosity = FITS::verboseMode();

  // first check whether there is a keyword called ColName

  try {
    FITS::setVerboseMode(false);
    ext.readKey(ColName, keyValue);
    Data.resize(1);
    Data = keyValue;
    return;
  } catch (HDU::NoSuchKeyword&) {
    FITS::clearErrors();
    FITS::setVerboseMode(verbosity);

    // no keyword so try to read the column

    try {
      Column& Col = ext.column(ColName);
      try {
	FITS::setVerboseMode(false);
	Col.read(Data,RowNumber);
      } catch(Column::WrongColumnType) {
	FITS::clearErrors();
	FITS::setVerboseMode(verbosity);
	if ( RowNumber == 1 ) {
	  Col.read(Data,1,Col.rows());
	}
      }
    } catch(Table::NoSuchColumn&){
      Data.resize(0);
      return;
    }
  }

  return;

}

// Read a vector column

template<class T>void SPreadVectorCol(ExtHDU& ext, string ColName, vector<valarray<T> >& Data)
{

  bool verbosity = FITS::verboseMode();

  // first try to read the column as a scalar in case it is a vector of length 1

  try {
    Column& Col = ext.column(ColName);
    try {
      FITS::setVerboseMode(false);
      valarray<T> tempData(Data.size());
      Col.read(tempData,1,Col.rows());
      Data.resize(tempData.size());
      for (size_t i=0; i<tempData.size(); i++) Data[i].resize(1);
      for (size_t i=0; i<tempData.size(); i++) Data[i][0] = tempData[i];
    } catch(...) {
      FITS::clearErrors();
      FITS::setVerboseMode(verbosity);
      Col.readArrays(Data,1,Col.rows());
    }
  } catch(Table::NoSuchColumn&){
    Data.resize(0);
  }
  return;

}

// Write a keyword - at present just a wrap-up of addKey

template <class T> void SPwriteKey(Table& table, string KeyName, T KeyValue, string Comment)

{
  table.addKey(KeyName, KeyValue, Comment);

  return;
}

// Write a column for a valarray. If the data size is 1 or all 
// values are the same then just write a keyword

template <class T> void SPwriteCol(Table& table, string ColName, valarray<T>& Data)
{

  if ( Data.size() == 0 ) {
    return;
  } else if ( !SPneedCol(Data) ) {
    table.addKey(ColName, Data[0], " ");
  } else {
    table.column(ColName).write(Data, 1);
  }

  return;
}

// Write a column for a valarray. If the data size is 1 or all 
// values are the same then just write a keyword if the forceCol bool is false.

template <class T> void SPwriteCol(Table& table, string ColName, valarray<T>& Data, bool forceCol)
{

  if ( Data.size() == 0 ) {
    return;
  } else if ( !SPneedCol(Data) && !forceCol ) {
    table.addKey(ColName, Data[0], " ");
  } else {
    table.column(ColName).write(Data, 1);
  }

  return;
}

// Write a column for a vector. If the data size is 1 or all values are the 
// same then just write a keyword

template <class T> void SPwriteCol(Table& table, string ColName, vector<T>& Data)
{

  if ( Data.size() == 0 ) {
    return;
  } else if ( !SPneedCol(Data) ) {
    table.addKey(ColName, Data[0], " ");
  } else {
    table.column(ColName).write(Data, 1);
  }

  return;
}

// Write a column for a vector. If the data size is 1 or all values are the 
// same then just write a keyword if the forceCol bool is false

template <class T> void SPwriteCol(Table& table, string ColName, vector<T>& Data, bool forceCol)
{

  if ( Data.size() == 0 ) {
    return;
  } else if ( !SPneedCol(Data) && !forceCol ) {
    table.addKey(ColName, Data[0], " ");
  } else {
    table.column(ColName).write(Data, 1);
  }

  return;
}

// Write a column for a vector of valarrays. If the data size is 1 or all values 
// are the same then just write a keyword. If all values are the same within all
// valarrays then write a scalar column

template <class T> void SPwriteVectorCol(Table& table, string ColName, vector<valarray<T> >& Data)
{
  bool isvector;
  bool needcol;

  if ( Data.size() == 0 ) {
    return;
  } else {
    needcol = SPneedCol(Data, isvector);
    if ( !needcol ) {
      table.addKey(ColName, Data[0][0], " ");
    } else if ( needcol && !isvector) {
      valarray<T> tempData(Data.size());
      for (size_t i=0; i<Data.size(); i++) tempData[i] = Data[i][0];
      table.column(ColName).write(tempData, 1);
    } else {
      table.column(ColName).writeArrays(Data,1);
    }
  }

  return;
}

// Write a column for a vector of valarrays. If the data size is 1 or all values 
// are the same then just write a keyword if the forceCol bool is false. If all values are the same within all
// valarrays then write a scalar column

template <class T> void SPwriteVectorCol(Table& table, string ColName, vector<valarray<T> >& Data, bool forceCol)
{
  bool isvector;
  bool needcol;

  if ( Data.size() == 0 ) {
    return;
  } else {
    needcol = SPneedCol(Data, isvector);
    if ( !needcol && !forceCol ) {
      table.addKey(ColName, Data[0][0], " ");
    } else {
      if ( !isvector) {
	valarray<T> tempData(Data.size());
	for (size_t i=0; i<Data.size(); i++) tempData[i] = Data[i][0];
	table.column(ColName).write(tempData, 1);
      } else {
	table.column(ColName).writeArrays(Data,1);
      }
    }
  }

  return;
}

// check whether a given column is required and if it needs to be a vector column
// note that T is assumed to be a valarray or vector of a valarray of some type.

template <class T> bool SPneedCol(const T& Data, bool& isvector)
{

  isvector = false;
  for (size_t i=0; i<Data.size(); i++) {
    isvector = isvector || (Data[i].size() > 1);
  }

  bool needcol = isvector;
  if (!needcol) {
    for (size_t i=1; i<Data.size(); i++) {
      if (Data[i][0] != Data[0][0]) needcol = true;
    }
  }

  return needcol;
}

// check whether a given column is required. For this overloaded version
// T is assumed to be a valarray or a vector of a scalar of some type.

template <class T> bool SPneedCol(const T& Data)
{

  bool needcol = false;
  if ( Data.size() > 1 ) {
    for (size_t i=1; i<Data.size(); i++) {
      if (Data[i] != Data[0]) needcol = true;
    }
  }

  return needcol;
}

// Return a FITS string specification for the longest string in the input
// vector<string>

string SPstring(const vector<string>& Data)
{

  size_t length = Data[0].size();
  for (size_t i=1; i<Data.size(); i++) {
    if ( Data[i].size() > length ) length = Data[i].size();
  }

  stringstream tmpStream;
  tmpStream << length;

  return tmpStream.str();
}



// required to make the linker instantiate correctly

template string SPreadKey(ExtHDU&, string, string);
template bool SPreadKey(ExtHDU&, string, bool);
template Real SPreadKey(ExtHDU&, string, Real);
template Integer SPreadKey(ExtHDU&, string, Integer);

template string SPreadKey(ExtHDU&, string, Integer, string);
template bool SPreadKey(ExtHDU&, string, Integer, bool);
template Real SPreadKey(ExtHDU&, string, Integer, Real);
template Integer SPreadKey(ExtHDU&, string, Integer, Integer);

template void SPreadCol(ExtHDU&, string, valarray<Real>&);
template void SPreadCol(ExtHDU&, string, valarray<Integer>&);

template void SPreadCol(ExtHDU&, string, Integer, valarray<Real>&);
template void SPreadCol(ExtHDU&, string, Integer, valarray<Integer>&);

template void SPreadVectorCol(ExtHDU&, string, vector<valarray<Real> >& Data);
template void SPreadVectorCol(ExtHDU&, string, vector<valarray<Integer> >& Data);

template void SPwriteKey(Table&, string, Integer, string);
template void SPwriteKey(Table&, string, bool, string);
template void SPwriteKey(Table&, string, Real, string);
template void SPwriteKey(Table&, string, string, string);

template void SPwriteCol(Table&, string, IntegerArray&);
template void SPwriteCol(Table&, string, RealArray&);
template void SPwriteCol(Table&, string, vector<string>&);
template void SPwriteCol(Table&, string, vector<Real>&);

template void SPwriteCol(Table&, string, IntegerArray&, bool);
template void SPwriteCol(Table&, string, RealArray&, bool);
template void SPwriteCol(Table&, string, vector<string>&, bool);
template void SPwriteCol(Table&, string, vector<Real>&, bool);

template void SPwriteVectorCol(Table&, string, vector<IntegerArray>&);
template void SPwriteVectorCol(Table&, string, vector<RealArray>&);

template void SPwriteVectorCol(Table&, string, vector<IntegerArray>&, bool);
template void SPwriteVectorCol(Table&, string, vector<RealArray>&, bool);

template bool SPneedCol(const vector<RealArray>&, bool&);
template bool SPneedCol(const vector<IntegerArray>&, bool&);

template bool SPneedCol(const vector<Integer>&);
template bool SPneedCol(const vector<Real>&);
template bool SPneedCol(const vector<string>&);
template bool SPneedCol(const vector<bool>&);

