# Modify these three as appropriate for the system

CXX = g++
CC = gcc
CFLAGS = -m32 -Wall -g -I${HEADAS}/include

all:	smooth_rmf

smooth_rmf: smooth_rmf.o RMF.o SPIO.o
	$(CXX) -o smooth_rmf smooth_rmf.o RMF.o SPIO.o -m32 -L${HEADAS}/lib -lCCfits_2.3 -lcfitsio_3.27

.SUFFIXES: .c .cpp .cxx .o

.cpp.o:	$*.h
	${CXX} -c ${CFLAGS} $<

.cxx.o:	$*.h
	${CXX} -c ${CFLAGS} $<

.c.o:	$*.h
	${CC} -c ${CFLAGS} $<

