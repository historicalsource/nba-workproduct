#---------------------------------------
#
# Top level Makefile for NBA Img3d
#
# $Revision: $
#

#
# Phony make targets that don't want any implicit dependencies
#
.PHONY:	all clean clobber rebuild get

#
# Default target
#
all:
	@echo `NBA\IMG3D' TOP LEVEL
	@echo CHECKING CURRENT BUILD OF `TOOLS\3DF2WMS'
	@$(MAKE) --no-print-directory -C /video/tools/3df2wms
ifeq	($(wildcard *.mak),img3d.mak)
	@$(MAKE) --no-print-directory -f img3d.mak
endif
ifeq	($(wildcard *.mak),field.mak)
	@$(MAKE) --no-print-directory -f field.mak
endif
ifeq	($(wildcard *.mak),stadium.mak)
	@$(MAKE) --no-print-directory -f stadium.mak
endif
	@echo `NBA\IMG3D' TOP LEVEL DONE

#---------------------------------------
#
# How to clean
#
clean:
	@echo `NBA\IMG3D' TOP LEVEL CLEAN
ifeq	($(wildcard *.mak),img3d.mak)
	@$(MAKE) --no-print-directory -f img3d.mak clean
endif
ifeq	($(wildcard *.mak),field.mak)
	@$(MAKE) --no-print-directory -f field.mak clean
endif
ifeq	($(wildcard *.mak),stadium.mak)
	@$(MAKE) --no-print-directory -f stadium.mak clean
endif
	@echo `NBA\IMG3D' TOP LEVEL CLEAN DONE

#
# How to clobber
#
clobber:
	@echo `NBA\IMG3D' TOP LEVEL CLOBBER
ifeq	($(wildcard *.mak),img3d.mak)
	@$(MAKE) --no-print-directory -f img3d.mak clobber
endif
ifeq	($(wildcard *.mak),field.mak)
	@$(MAKE) --no-print-directory -f field.mak clobber
endif
ifeq	($(wildcard *.mak),stadium.mak)
	@$(MAKE) --no-print-directory -f stadium.mak clobber
endif
	@echo `NBA\IMG3D' TOP LEVEL CLOBBER DONE

#
# How to rebuild
#
rebuild:
	@echo `NBA\IMG3D' TOP LEVEL REBUILD
ifeq	($(wildcard *.mak),img3d.mak)
	@$(MAKE) --no-print-directory -f img3d.mak rebuild
endif
ifeq	($(wildcard *.mak),field.mak)
	@$(MAKE) --no-print-directory -f field.mak rebuild
endif
ifeq	($(wildcard *.mak),stadium.mak)
	@$(MAKE) --no-print-directory -f stadium.mak rebuild
endif
	@echo `NBA\IMG3D' TOP LEVEL REBUILD DONE

#---------------------------------------
#
# How to get source from the source safe
#
get:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo GETTING `NBA\IMG3D'
	$(SS) cp $$/video/nba/img3d
	$(SS) get img3d -gcd
	@echo GETTING `NBA\IMG3D' DONE
endif
