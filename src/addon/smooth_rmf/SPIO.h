// Useful template functions

#ifndef HAVE_HEASP
#include "heasp.h"
#endif

#define HAVE_SPIO 1


template <class T> T SPreadKey(ExtHDU&, string, T);

template <class T> T SPreadKey(ExtHDU&, string, Integer, T);

template <class T> void SPreadCol(ExtHDU&, string, valarray<T>&);

template <class T> void SPreadCol(ExtHDU&, string, Integer, valarray<T>&);

template <class T> void SPreadVectorCol(ExtHDU&, string, vector<valarray<T> >&);

template <class T> void SPwriteKey(Table&, string, T, string);

template <class T> void SPwriteCol(Table&, string, valarray<T>&);

template <class T> void SPwriteCol(Table&, string, valarray<T>&, bool);

template <class T> void SPwriteCol(Table&, string, vector<T>&);

template <class T> void SPwriteCol(Table&, string, vector<T>&, bool);

template <class T> void SPwriteVectorCol(Table&, string, vector<valarray<T> >&);

template <class T> void SPwriteVectorCol(Table&, string, vector<valarray<T> >&, bool);

template <class T> bool SPneedCol(const T&);

template <class T> bool SPneedCol(const T&, bool& isvector);

string SPstring(const vector<string>& Data);
