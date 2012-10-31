#----------------------------------------------------------------
#
#  Makefile
#
#  Switches into the subdirectories of the source path
#
#  Author: Andreas Zoglauer
#
#----------------------------------------------------------------


#----------------------------------------------------------------
# Commands:
#


megalib:
	@$(MAKE) -C megalib

main:
	@$(MAKE) -C main

modules:
	@$(MAKE) -C modules

data:
	@$(MAKE) -C data

orbit:
	@$(MAKE) -C orbit

addon:
	@$(MAKE) -C addon

nulyses:
	@$(MAKE) -C nulyses



#------------------------------------------------------------------------------
# All

link:
	$(MAKE) link -C global
	$(MAKE) link -C megalib
	$(MAKE) link -C main
	$(MAKE) link -C modules
	$(MAKE) link -C data
	$(MAKE) link -C orbit
	$(MAKE) link -C nulyses

all:
	@$(MAKE) -C global
	@$(MAKE) -C orbit
	@$(MAKE) -C megalib
	@$(MAKE) -C data
	@$(MAKE) -C modules
	@$(MAKE) -C main
	@$(MAKE) -C addon
	@$(MAKE) -C nulyses

clean:
	@$(MAKE) clean -C global
	@$(MAKE) clean -C megalib
	@$(MAKE) clean -C main
	@$(MAKE) clean -C modules
	@$(MAKE) clean -C data
	@$(MAKE) clean -C orbit
	@$(MAKE) clean -C addon
	@$(MAKE) clean -C nulyses
	@rm -f *~
	@rm -f \#*



#
#----------------------------------------------------------------



