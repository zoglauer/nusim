#!/usr/bin/env python

from sys import *
from os  import *

if not(len(argv) == 2 or len(argv) == 3) :
    print "Usage:", argv[0], "[ROOT file] (Output FITS file)"
    exit()

ROOTFileName = argv[1]
if not path.isfile(ROOTFileName) :
    print "Error:", ROOTFileName, "does not exits"
    exit()

OutFitsName = "NuSTAR.rmf"

FPM = "FPMA"
DET = "DET0"
DepthCut = "DEPTHCUT(NOMINAL)"

if len(argv) == 3 :
    OutFitsName = argv[2]

    OutFitsBaseName = path.basename(OutFitsName)

    FPM = "FPM" + OutFitsBaseName[2]
    DET = "DET" + OutFitsBaseName[OutFitsBaseName.find("_") - 1]
    if OutFitsBaseName.find("nocut") == -1 :
        DepthCut = "DEPTHCUT(NOMINAL)"
    else :
        DepthCut = "DEPTHCUT(NONE)"

    print FPM, DET, DepthCut

## -------------------------------------------------------------------

from numpy import arange
from ROOT import * # ROOT
from pyfits import *

## -------------------------------------------------------------------

gROOT.SetBatch(1)
gStyle.SetLegendBorderSize(0)

canv = TCanvas("canv", "ROOT Canvas", 1000, 1000)
# canv.SetLogx(1); canv.SetLogy(1)
# canv.SetLogx(0); canv.SetLogy(1)
canv.SetGrid(1,1)
canv.SetFillStyle(4000)

## ===================================================================
## Response matrix
## -------------------------------------------------------------------

ROOTFile = TFile(ROOTFileName, "readonly")

if ROOTFile == None :
    print "Error: Cannot open ROOT file:", ROOTFileName
    exit()

RMF = ROOTFile.Get("Response")
if RMF == None :
    print "Error: Cannot open response histogram"
    exit()

## RMF.Rebin2D(2, 2)
## RMF.Rebin2D(4, 4)
## RMF.Rebin2D(10, 10)

RMF.SetXTitle("Primary Energy (keV)")
RMF.SetYTitle("Observed Energy (keV)")
RMF.SetZTitle("Counts")

RMF.UseCurrentStyle()
PriEneAxis = RMF.GetXaxis()
ObsEneAxis = RMF.GetYaxis()

## ===================================================================
## Parameters necessary ror Fits header
## -------------------------------------------------------------------

## OutFitsName = "NuSTAR.rmf"
## OutFitsName = "FPMA_Det0_NoPt.rmf"
Version     = 1
StartTime   = "2010-01-01"
StartDate   = "00:00:00"

## -------------------------------------------------------------------

def WriteHeaderKeyword(aHeader) :
    aHeader.update("TELESCOP", "NuSTAR",      "Telescope (mission) name")
    aHeader.update("INSTRUME", FPM,        "Instrument name (FPMA or FPMB)")
    ## aHeader.update("DETCHANS", str(RMF.GetNbinsY()), "Number of channels in rmf")
    if   aHeader["EXTNAME"] == "MATRIX" :
        aHeader.update("DETCHANS", RMF.GetNbinsY(), "Number of channels in rmf")
    elif aHeader["EXTNAME"] == "EBOUNDS" :
        aHeader.update("DETCHANS", RMF.GetNbinsY(), "Number of channels in ebd")
    aHeader.update("CHANTYPE", "PI"   ,       "Channel type")
    aHeader.update("HDUCLASS", "OGIP",        "format conforms to OGIP standard")
    aHeader.update("HDUCLAS1", "RESPONSE",    "dataset is response function")
    aHeader.update("HDUVERS1", "1.0.0",       "version of HDUCLAS1 format")
    if   aHeader["EXTNAME"] == "MATRIX" :
        aHeader.update("HDUCLAS2", "RSP_MATRIX",  "Extension contains an RMF")
    elif aHeader["EXTNAME"] == "EBOUNDS" :
        aHeader.update("HDUCLAS2", "EBOUNDS",     "Extension contains EBOUNDS")
    aHeader.update("HDUVERS2", "1.3.0",       "version of HDUCLAS2 format")
    if   aHeader["EXTNAME"] == "MATRIX" :
        aHeader.update("HDUCLAS3", "REDIST",      "details of matrix")
    aHeader.update("HDUVERS",  "1.3.0",       "OGIP version number")
    aHeader.update("RMFVERSN", "1992a",       "OGIP classification of FITS format")
    aHeader.update("CCLS0001", "CPF",         "Data set is a Calibration Product File")
    if   aHeader["EXTNAME"] == "MATRIX" :
        aHeader.update("CCNM0001", "MATRIX",      "Type of Calibration data")
    elif aHeader["EXTNAME"] == "EBOUNDS" :
        aHeader.update("CCNM0001", "EBOUNDS",     "Type of Calibration data")
    aHeader.update("CDTP0001", "DATA",        "Calibration file contains data")
    aHeader.update("CBD10001", "RAWX(0-31)",  "Parameter Boundary")
    aHeader.update("CVSD0001", StartTime,     "UTC date when file should first be used")
    aHeader.update("CVST0001", StartDate,     "UTC time when file should first be used")
    aHeader.update("Version",  Version,       "Extension version number")
    aHeader.update("FILENAME", path.basename(OutFitsName),   "File name")
    aHeader.update("CREATOR",  "NuSTAR team", "Creator")
    aHeader.update("ORIGIN",   "Caltech",     "Source of FITS file")
    aHeader.update("DETNAM",   DET,           "CZT Detector ID (0,1,2 or 3)")
    aHeader.update("CDES0001", "NuSTAR FPMA Response Matrix", "Description")
    aHeader.update("CBD20001", "RAWY(0-31)",  "Parameter Boundary")
    aHeader.update("CBD30001", "ENERG(1-150)keV", "Parameter Boundaries")
    aHeader.update("CBD40001", DepthCut,      "Parameter Boundaries")

## ===================================================================

NPrimPhotons = RMF.GetEntries() / 2
print "# of primary photons =", NPrimPhotons

## -------------------------------------------------------------------

## ActiveArea = (0.06048**2) * (10**2)
## PrimArea = (0.06048**2) * (10**2)
ActiveArea = 0.5**2 * TMath.Pi()
PrimArea   = 0.5**2 * TMath.Pi()

print "Active CZT Area =", ActiveArea, "cm2"

PrimEneMin =   1.6  # keV
## PrimEneMax = 165.44  # keV
PrimEneMax = 100.0  # keV
dPrimEne   = PrimEneMax - PrimEneMin

## ScaleFactor = NPrimPhotons / (TMath.Pi() * 3.0**2) * ActiveArea / 200.0 * PriEneAxis.GetBinWidth(1)
ScaleFactor = NPrimPhotons / PrimArea * ActiveArea \
    / dPrimEne * PriEneAxis.GetBinWidth(1)
print "ScaleFactor =", 1.0 / ScaleFactor
RMF.Scale(1.0 / ScaleFactor)

RMF.Draw("colz")
RMF.GetZaxis().SetRangeUser(1.0e-5, RMF.GetMaximum())
canv.SetLogz()
## canv.Print("RMF.eps")
canv.Print("RMF.gif")

canv.Clear()
canv.SetCanvasSize(1400, 1000)

PriQE = RMF.ProjectionX("PriQE", 1, RMF.GetNbinsY())

PriQE.SetLineColor(kRed)
PriQE.SetYTitle("Detection Efficiency")
PriQE.SetNdivisions(515, "XY")
PriQE.Draw()
canv.Print("QE.eps")
## canv.Print("QE.root")

ObsQE = RMF.ProjectionY("ObsQE", 1, RMF.GetNbinsX())
ObsQE.SetLineColor(kRed)
ObsQE.SetYTitle("Detection Efficiency")
ObsQE.Draw()
## canv.Print("ObsQE.eps")


## ===================================================================
## primary HDU
## -------------------------------------------------------------------

## PrimaryHDU = PrimaryHDU(arange(0))
PrimaryHDU = PrimaryHDU()

## print PrimaryHDU.header

## Write keyword in header
PrimaryHDU.header.update("TELESCOP", "NuSTAR", "Telescope (mission) name")
PrimaryHDU.header.update("INSTRUME", "FPMA",   "Instrument name (FPMA or FPMB)")
## PrimaryHDU.header.update(key="BITPIX", value=-32)
PrimaryHDU.header.update("ORIGIN",   "Caltech",     "Source of FITS file")
PrimaryHDU.header.update("CREATOR",  "NuSTAR team", "Creator")

## print PrimaryHDU.header

## PrimaryHDU.verify_checksum()
## PrimaryHDU.verify_datasum()

## ===================================================================
## MATRIX HDU
## -------------------------------------------------------------------

ENERG_LO = Column(name="ENERG_LO", format="E",  unit="keV")
ENERG_HI = Column(name="ENERG_HI", format="E",  unit="keV")
N_GRP    = Column(name="N_GRP",    format="I",  unit=" ")
F_CHAN   = Column(name="F_CHAN",   format="1I", unit=" ")
N_CHAN   = Column(name="N_CHAN",   format="1I", unit=" ")
MATRIX   = Column(name="MATRIX",   format=str(RMF.GetNbinsY())+"E", unit=" ")

MATRIX_Columns = ColDefs([ENERG_LO, ENERG_HI, N_GRP, F_CHAN, N_CHAN, MATRIX])
MATRIX_HDU = new_table(MATRIX_Columns, nrows=RMF.GetNbinsX())

## Write keywords in header
MATRIX_HDU.header.update("TTYPE1", "ENERG_LO", "label for field   1")
MATRIX_HDU.header.update("TFORM1", "E"       , "data format of field: 4-byte REAL")
MATRIX_HDU.header.update("TUNIT1", "keV",      "physical unit of field")
MATRIX_HDU.header.update("TTYPE2", "ENERG_HI", "label for field   2")
MATRIX_HDU.header.update("TFORM2", "E",        "data format of field: 4-byte REAL")
MATRIX_HDU.header.update("TUNIT2", "keV",      "physical unit of field")
MATRIX_HDU.header.update("TTYPE3", "N_GRP",    "label for field   3")
MATRIX_HDU.header.update("TFORM3", "I",        "data format of field: 2-byte INTEGER")
## MATRIX_HDU.header.update("TUNIT3", "      ",   "physical unit of field")
MATRIX_HDU.header.update("TTYPE4", "F_CHAN",   "label for field   4")
MATRIX_HDU.header.update("TFORM4", "1I",       "data format of field: 2-byte INTEGER")
## MATRIX_HDU.header.update("TUNIT4", "      ",   "physical unit of field")
MATRIX_HDU.header.update("TLMIN4", "0",        "Minimum value legally allowed in column 4")
MATRIX_HDU.header.update("TLMAX4", str(RMF.GetNbinsY()-1), "Maximum value legally allowed in column 4")
MATRIX_HDU.header.update("TTYPE5", "N_CHAN",   "label for field   5")
MATRIX_HDU.header.update("TFORM5", "1I",       "data format of field: 2-byte INTEGER")
## MATRIX_HDU.header.update("TUNIT5", "      ",   "physical unit of field")
MATRIX_HDU.header.update("TTYPE6", "MATRIX",   "label for field   6")
MATRIX_HDU.header.update("TFORM6", str(RMF.GetNbinsY())+"E", "data format of field: 4-byte REAL")
## MATRIX_HDU.header.update("TUNIT6", "      ",   "physical unit of field")

del MATRIX_HDU.header["TUNIT3"]
del MATRIX_HDU.header["TUNIT4"]
del MATRIX_HDU.header["TUNIT5"]
del MATRIX_HDU.header["TUNIT6"]

MATRIX_HDU.header.update("EXTNAME",  "MATRIX", "name of this binary table extension")
WriteHeaderKeyword(MATRIX_HDU.header)

## Fill values in columns
RMFGetBinContent = RMF.GetBinContent
RMFNbinsX = RMF.GetNbinsX()
RMFNbinsY = RMF.GetNbinsY()

ENERG_LOs = MATRIX_HDU.data.field("ENERG_LO")
ENERG_HIs = MATRIX_HDU.data.field("ENERG_HI")
N_GRPs    = MATRIX_HDU.data.field("N_GRP")
F_CHANs   = MATRIX_HDU.data.field("F_CHAN")
N_CHANs   = MATRIX_HDU.data.field("N_CHAN")
MATRIXs   = MATRIX_HDU.data.field("MATRIX")

for i in xrange(RMFNbinsX) :
    ENERG_LOs[i] = PriEneAxis.GetBinLowEdge(i+1)
    ENERG_HIs[i] = PriEneAxis.GetBinUpEdge(i+1)
    N_GRPs   [i] = 1
    F_CHANs  [i] = 0
    N_CHANs  [i] = RMFNbinsY

    aMatrixColumn = MATRIXs[i]
    for j in xrange(RMFNbinsY) :
        aMatrixColumn[j] = RMFGetBinContent(i+1, j+1)

## ===================================================================
## EBOUNDS HDU
## -------------------------------------------------------------------

CHANNEL = Column(name="CHANNEL", format="J", unit="chan")
E_MIN   = Column(name="E_MIN",   format="E", unit="keV")
E_MAX   = Column(name="E_MAX",   format="E", unit="keV")

EBOUNDS_Columns = ColDefs([CHANNEL, E_MIN, E_MAX])
EBOUNDS_HDU = new_table(EBOUNDS_Columns, nrows=RMF.GetNbinsY())

## Write keywords in header
EBOUNDS_HDU.header.update("TTYPE1", "CHANNEL", "label for field   1")
EBOUNDS_HDU.header.update("TFORM1", "J",       "data format of field: 2-byte INTEGER")
EBOUNDS_HDU.header.update("TLMIN1", "0",       "Minimum value legally allowed in column 1")
EBOUNDS_HDU.header.update("TLMAX1", str(RMF.GetNbinsY()-1), "Maximum value legally allowed in column 1")
EBOUNDS_HDU.header.update("TUNIT1", "chan",    "physical unit of field")
EBOUNDS_HDU.header.update("TTYPE2", "E_MIN",   "label for field   2")
EBOUNDS_HDU.header.update("TFORM2", "E",       "data format of field: 4-byte REAL")
EBOUNDS_HDU.header.update("TUNIT2", "keV",     "physical unit of field")
EBOUNDS_HDU.header.update("TTYPE3", "E_MAX",   "label for field   3")
EBOUNDS_HDU.header.update("TFORM3", "E",       "data format of field: 4-byte REAL")
EBOUNDS_HDU.header.update("TUNIT3", "keV",     "physical unit of field")

EBOUNDS_HDU.header.update("EXTNAME", "EBOUNDS", "name of this binary table extension")
WriteHeaderKeyword(EBOUNDS_HDU.header)

## Fill values in columns
CHANNELs = EBOUNDS_HDU.data.field("CHANNEL")
E_MINs   = EBOUNDS_HDU.data.field("E_MIN")
E_MAXs   = EBOUNDS_HDU.data.field("E_MAX")

for i in xrange(RMFNbinsY) :
    CHANNELs[i] = i
    E_MINs  [i] = ObsEneAxis.GetBinLowEdge(i+1)
    E_MAXs  [i] = ObsEneAxis.GetBinUpEdge(i+1)

## ===================================================================

HDUList = HDUList([PrimaryHDU, MATRIX_HDU, EBOUNDS_HDU])
HDUList.writeto(OutFitsName, clobber=True, checksum=True)

## ===================================================================
