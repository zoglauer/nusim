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

addon:
	@$(MAKE) -C addon



#------------------------------------------------------------------------------
# All

link:
	$(MAKE) link -C megalib
	$(MAKE) link -C main
	$(MAKE) link -C modules
	$(MAKE) link -C data

all:
	@$(MAKE) -C megalib
	@$(MAKE) -C modules
	@$(MAKE) -C data
	@$(MAKE) -C main
	@$(MAKE) -C addon

clean:
	@$(MAKE) clean -C megalib
	@$(MAKE) clean -C main
	@$(MAKE) clean -C modules
	@$(MAKE) clean -C data
	@$(MAKE) clean -C addon
	@rm -f *~
	@rm -f \#*



#
#----------------------------------------------------------------



