// definitions for heasp library

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>
#include <stdexcept>
#include <ctime>
#include <valarray>
#include <vector>

#include <CCfits/CCfits>

using namespace std;
using namespace CCfits;

typedef int Integer;
typedef float Real;
typedef valarray<Integer> IntegerArray;
typedef valarray<Real> RealArray;

#define HAVE_HEASP 1
