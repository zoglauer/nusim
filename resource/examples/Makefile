#----------------------------------------------------------------
#
#  Makefile for the NuSIM examples directory
#
#  Author: Andreas Zoglauer
#
#----------------------------------------------------------------


#----------------------------------------------------------------
# Definitions:
#

NAME = simulationreport
LATEX = pdflatex

#----------------------------------------------------------------
# Commands:
#

all: clean pdf clean

clean:
	@rm -f *.o
	@rm -f *~
	@rm -f *.ps
	@rm -f *.dvi
	@rm -f *.aux
	@rm -f *.log
	@rm -f *.toc
	@rm -f *.bbl
	@rm -f *.blg
	@rm -f *.pdf
	@rm -f *.lof
	@rm -f *.lot

pdf: *.tex
	@rm -f $(NAME).pdf
	$(LATEX) $(NAME)
	$(LATEX) $(NAME)
#	bibtex $(NAME)
#	$(LATEX) $(NAME)
	@cp $(NAME).pdf $(NUSIM)/doc/SimulationReport.pdf

show: clean pdf clean
	@acroread $(NUSIM)/doc/SimulationReport.pdf


