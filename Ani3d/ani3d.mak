#---------------------------------------
#
# Makefile for NBA Ani3d Library
#
# $Revision: 6 $
#
ifneq	($(BUILDMODE), RELEASE)
DBG=		-DDEBUG
DBG_MSG=	DEBUG
SUFFIX=		d
else
DBG=
DBG_MSG=	RELEASE
SUFFIX=		r
endif

ifeq	($(TARGET_SYS),SEATTLE)
PREFIX=		s
CF_FILE=	ani3d.cf
endif
ifeq	($(TARGET_SYS),VEGAS)
ifeq	($(GRX_HARDWARE),VOODOO2)
PREFIX=		v
CF_FILE=	vani3d.cf
endif
ifeq	($(GRX_HARDWARE),BANSHEE)
PREFIX=		b
CF_FILE=	bani3d.cf
endif
endif

BASE_NAME=	$(PREFIX)ani3d$(SUFFIX)


#---------------------------------------
#
# Assembler command line
#
AS=	@asmmips /q /of.rdata /zd /l /oc+

#
# Command line options for archiver
#
ARFLAGS=	/u

#
# Archiver command line
#
AR=	@redir -o /dev/null psylib

#
# Command line options for C compiler
#
CFLAGS=	-D$(TARGET_SYS) -D$(BUILDMODE) -D$(GRX_HARDWARE) @$(CF_FILE) -O

#
# C compiler command lines
#
CC3 =	@ccmips $(CFLAGS)3
CC2 =	@ccmips $(CFLAGS)2
CC1 =	@ccmips $(CFLAGS)1
CC0 =	@ccmips $(CFLAGS)0

#---------------------------------------
#
# C source modules
#
#CSRCS =	pipe3d.c anim.c
CSRCS =	pipe3d.c anim.c geom.c srmath.c clip.c

#
# Assembly source modules
#
#ASRCS =	geom.s
ASRCS = math.s srmath_s.s

#
# Object modules
#
OBJS = 	$(addprefix $(PREFIX)$(SUFFIX)/, $(ASRCS:.s=.o)) $(addprefix $(PREFIX)$(SUFFIX)/, $(CSRCS:.c=.o))

DEPS =	$(addprefix $(PREFIX)$(SUFFIX)/, $(CSRCS:.c=.d))

#
# Phony target for prompt
#
.PHONY:	$(PREFIX)$(SUFFIX) dep

#
# Tell make NOT to delete .o files when done
#
.PRECIOUS:	$(OBJS)

	
$(PREFIX)$(SUFFIX):
	@cmdir $(PREFIX)$(SUFFIX)
	@$(MAKE) --no-print-directory -f ani3d.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) dep

dep:	$(DEPS)
	@$(MAKE) --no-print-directory -f ani3d.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) $(BASE_NAME).a

define NEWLINE


endef
#
# What we are going to build
#
$(BASE_NAME).a:	$(OBJS)
	@if EXIST update.bat del update.bat
	$(foreach OBJ_FILE, $?,@echo>>update.bat $(AR) $(ARFLAGS) $(@) $(OBJ_FILE)$(NEWLINE))
	@if EXIST update.bat update.bat
	@if EXIST update.bat del update.bat


#
# Dependencies and rules
#
$(PREFIX)$(SUFFIX)/%.o:	%.s
	@echo $< to $@
	$(AS) $<,$@

$(PREFIX)$(SUFFIX)/%.o:	%.c
	@echo $< to $@
	$(CC3) -o $@ $<

$(PREFIX)$(SUFFIX)/%.d:	%.c
	@echo $< to $@
	@mkdephdr $(PREFIX)$(SUFFIX)/ > $(PREFIX)$(SUFFIX)\$(PREFIX)$(SUFFIX).dh
	$(CC3) -M -o $(PREFIX)$(SUFFIX)/tmp.d $<
	@copy $(PREFIX)$(SUFFIX)\$(PREFIX)$(SUFFIX).dh+$(PREFIX)$(SUFFIX)\tmp.d $(PREFIX)$(SUFFIX)\$(notdir $@) > NUL:
	@del $(PREFIX)$(SUFFIX)\*.dh
	@del $(PREFIX)$(SUFFIX)\tmp.d

#
# Dependency files
#
clobber:
	@if EXIST $(BASE_NAME).a del $(BASE_NAME).a


clean:	clobber
	@-deltree /y $(PREFIX)$(SUFFIX)


rebuild:	clean
	@$(MAKE) --no-print-directory -f ani3d.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE)


#
# Dependency files
#
ifneq	($(wildcard $(PREFIX)$(SUFFIX)/*.d),)
include	$(wildcard $(PREFIX)$(SUFFIX)/*.d)
endif
