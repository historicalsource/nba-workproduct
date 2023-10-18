#
# Assorted directories
#
BUILDDIR=	.

#
# C Sources Modules
#
CSRCS=	num.c

#
# Objet Modules
#
OBJS=	$(CSRCS:.c=.obj)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/num.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET) dummy

#
# Dummy target
#
dummy:
	@echo REN DONE


#
# Module rules and dependencies
#
$(BUILDDIR)/num.exe:	$(OBJS)
	@echo Linking $@
	@gcc -O3 -Wall -o $@ $(OBJS)
	@echo $@ built

%.d:	%.c
	@echo Generating dependencies for $<
	@gcc -M $< | sed 's/$*.o/&bj $@/g' > $@

%.obj:	%.c
	@echo Compiling $<
	@gcc -c -O3 -o $@ $<

include	$(CSRCS:.c=.d)

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo Cleaning NUM
	@if EXIST num.obj del num.obj
	@if EXIST num.d   del num.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering NUM
	@if EXIST $(BUILDDIR)\num.exe del $(BUILDDIR)\num.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@$(MAKE) --no-print-directory
