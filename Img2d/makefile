#---------------------------------------
#
# Top level Makefile for NBA Img2d
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
	@echo `NBA\IMG2D' TOP LEVEL
	@echo CHECKING CURRENT BUILD OF `TOOLS\3DF2WMS'
	@$(MAKE) --no-print-directory -C /video/tools/3df2wms
ifeq	($(wildcard *.mak),img2d.mak)
	@$(MAKE) --no-print-directory -C . -f img2d.mak
endif
	@echo `NBA\IMG2D' TOP LEVEL DONE

#---------------------------------------
#
# How to clean
#
clean:
	@echo `NBA\IMG2D' TOP LEVEL CLEAN
ifeq	($(wildcard *.mak),img2d.mak)
	@$(MAKE) --no-print-directory -C . -f img2d.mak clean
endif
	@echo `NBA\IMG2D' TOP LEVEL CLEAN DONE

#
# How to clobber
#
clobber:
	@echo `NBA\IMG2D' TOP LEVEL CLOBBER
ifeq	($(wildcard *.mak),img2d.mak)
	@$(MAKE) --no-print-directory -C . -f img2d.mak clobber
endif
	@echo `NBA\IMG2D' TOP LEVEL CLOBBER DONE

#
# How to rebuild
#
rebuild:
	@echo `NBA\IMG2D' TOP LEVEL REBUILD
ifeq	($(wildcard *.mak),img2d.mak)
	@$(MAKE) --no-print-directory -C . -f img2d.mak rebuild
endif
	@echo `NBA\IMG2D' TOP LEVEL REBUILD DONE

#---------------------------------------
#
# How to get source from the source safe
#
get:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo GETTING `NBA\IMG2D'
	$(SS) cp $$/video/nba/img2d
	$(SS) get img2d -gcd
	@echo GETTING `NBA\IMG2D' DONE
endif
