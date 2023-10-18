#---------------------------------------
#
# Makefile for NBA Game
#
# $Revision: 65 $
#
ifeq	($(GRX_HARDWARE),)
gerr:
	@echo GRX_HARDWARE environment variable is NOT set
endif
ifneq	($(GRX_HARDWARE),VOODOO1)
ifneq	($(GRX_HARDWARE),VOODOO2)
ifneq	($(GRX_HARDWARE),BANSHEE)
gberr:
	@echo GRX_HARDWARE environment variable unknown: '$(GRX_HARDWARE)'
endif
endif
endif

ifeq	($(TARGET_SYS),SEATTLE)
PREFIX :=	s
SOUND_P :=	s
endif
ifeq	($(TARGET_SYS),VEGAS)
SOUND_P :=	v
PREFIX :=	v
ifeq	($(GRX_HARDWARE),BANSHEE)
PREFIX :=	b
endif
endif
ifeq	($(TARGET_SYS),)
terr:
	@echo TARGET_SYS environment variable is NOT set
	@fail
endif
ifeq	($(PREFIX),)
unkterr:
	@echo TARGET_SYS environment variable unknown: '$(TARGET_SYS)'
	@fail
endif

ifeq	($(BUILDMODE),DEBUG)
DBG :=		-DDEBUG
DBG_MSG :=	DEBUG
SUFFIX :=	d
endif
ifeq	($(BUILDMODE),RELEASE)
DBG :=
DBG_MSG :=	RELEASE
SUFFIX :=	r
endif
ifeq	($(BUILDMODE),)
berr:
	@echo BUILDMODE environment variable is NOT set
	@fail
endif
ifeq	($(SUFFIX),)
unkberr:
	@echo BUILDMODE environment variable unknown: '$(BUILDMODE)'
	@fail
endif

BASE_NAME :=	$(PREFIX)nba$(SUFFIX)

#---------------------------------------
#
# localset.mak isn't under version control.  Everyone's is different.
# It's for defining things like NODRONES or NODRONE_DEFENSE and such.
#
ifneq	($(findstring localset.mak, $(wildcard *.mak)),)
include	localset.mak
endif

#---------------------------------------
#
# Useful macros
#
NULL	:=
SPACE	:=	$(NULL) $(NULL)
TAB 	:=	$(NULL)	$(NULL)
COMMA	:=	,
DOLLAR	:=	$$
DASH	:=	-
FILL	:=	|

define NEWLINE


endef

#---------------------------------------
#
# C source files
#
CSRCS :=	main.c attract.c coin.c arena.c plyrseq.c sysfont.c teamdata.c\
		sndinfo.c mainproc.c cambot.c nba.c data.c picinfo.c drone.c\
		detect.c player.c plyrinfo.c sounds.c replay.c pmisc.c\
		grab.c dribbler.c debug.c globals.c record.c select.c\
		audinfo.c ball.c creatp.c plyrhead.c movie.c inthand.c\
		adjust.c opmsg.c particle.c transit.c vsscreen.c freegame.c putil.c base.c\
		trivia.c game_sel.c sweeps.c

#
# Assembly source files
#
ASRCS :=	getgaddr.s

#
# Object files
#
ifeq	($(TARGET_SYS),SEATTLE)
OBJS :=		\video\lib\crt0.obj $(CSRCS:.c=.o) $(ASRCS:.s=.o)
else
OBJS :=		\video\lib\vcrt0.obj $(CSRCS:.c=.o) $(ASRCS:.s=.o)
endif

# ? #
# ? # Any EXE files to include in compile-time dependencies
# ? #
# ? EXES :=		$(wildcard *.exe)
# ? EXE_DEPS :=	$(filter-out $(BASE_NAME).exe, $(EXES))

#
# Run-time origin
#
ORG :=		80100000

#---------------------------------------
#
# System libraries
#
SYS_LIBS :=	/video/lib/$(PREFIX)goose$(SUFFIX).a\
		/video/lib/$(PREFIX)glide$(SUFFIX).a\
		/video/lib/$(SOUND_P)sound$(SUFFIX).a

#
# Game Libraries
#
GAME_LIBS :=	ani3d/$(PREFIX)ani3d$(SUFFIX).a

#
# Libraries to check before linking
#
DEP_LIBS :=	$(SYS_LIBS) $(GAME_LIBS)
ifeq	($(LIB_CHK),0)
DEP_LIBS :=	$(GAME_LIBS)
endif

#
# Linker library files
#
LIBS :=		$(GAME_LIBS) \
		/video/lib/libc.a \
		$(SYS_LIBS)

#---------------------------------------
#
# Linker groups name,(type)
#
GRPS :=		text,\
		data,\
		bss,bss

#
# Linker code & data sections
#
SECTS :=	.text,text\
		textend,text\
		.rdata,data\
		.ctors,data\
		.dtors,data\
		.data,data\
		.sdata,data\
		.sbss,bss\
		.bss,bss\
		memheap,bss

#
# Link file name & dependents
#
LNKFILE :=	$(BASE_NAME).lnk

LNKDEPS :=	nba.mak makefile

#
# Link file parameter headers
#
LNKGRP :=	group

LNKLIB :=	inclib

LNKOBJ :=	include

LNKSECT :=	section.8

LNKMISC :=	org,$(DOLLAR)$(ORG)\
		regs,pc=start

#---------------------------------------
#
# CF file parameter list
#
ifeq	($(TARGET_SYS),SEATTLE)
CFLIST :=	-mhard-float\
		-DJONHEY\
		-D__DJGPP__\
		-fsigned-char\
		-fno-builtin\
		-g\
		-mips4\
		-fomit-frame-pointer\
		-c\
		-G 0\
		-mcpu=r8000\
		-nostdinc\
		-I\video\include\
		-I\video\nba\include\
		-Wall\
		-Wno-unused\
		-Wa,mips4
else
ifeq	($(GRX_HARDWARE),VOODOO2)
CFLIST :=	-mhard-float\
		-DINIT_DOS\
		-DJONHEY\
		-D__DJGPP__\
		-D__GOOSE__\
		-DGLIDE_HARDWARE\
		-DGLIDE_LIB\
		-DGLIDE_USE_C_TRISETUP\
		-DCVG\
		-DGLIDE_HW_TRI_SETUP\
		-DUSE_PACKET_FIFO\
		-DGLIDE3\
		-DGLIDE3_ALPHA\
		-fsigned-char\
		-fno-builtin\
		-mips4\
		-fomit-frame-pointer\
		-c\
		-g\
		-G 0\
		-mcpu=r8000\
		-nostdinc\
		-Iinclude\
		-I\video\include\
		-I\video\include\glide300\
		-I\video\tools\buildrom\
		-I\video\tools\zlib\
		-Wall\
		-Wno-unused\
		-Wa,mips4
else
CFLIST :=	-mhard-float\
		-DINIT_DOS\
		-DJONHEY\
		-D__DOS32__\
		-D__DJGPP__\
		-D__GOOSE__\
		-DGLIDE_HARDWARE\
		-DGLIDE_LIB\
		-DGLIDE_USE_C_TRISETUP\
		-DH3\
		-DH3_B0\
		-DGLIDE_HW_TRI_SETUP\
		-DGLIDE3\
		-DGLIDE3_ALPHA\
		-DGLIDE_INIT_HWC\
		-DGLIDE_PACKET3_TRI_SETUP\
		-DHAL_HW\
		-DMAPPL_DPMI\
		-DPORTIO_DIRECT\
		-DUSE_PACKET_FIFO\
		-fsigned-char\
		-fno-builtin\
		-mips4\
		-fomit-frame-pointer\
		-g\
		-c\
		-G 0\
		-mcpu=r8000\
		-nostdinc\
		-Iinclude\
		-I\video\include\
		-I\video\include\bglide\
		-I\video\tools\buildrom\
		-I\video\tools\zlib\
		-Wall\
		-Wno-unused\
		-Wa,mips4
endif
endif

#
# CF file name & dependents
#
CFFILE :=	$(BASE_NAME).cf

CFDEPS :=	nba.mak makefile

#
# Process the parameters to ensure consistent whitespacing
#
CFLIST :=	$(strip $(CFLIST))
#
# Process the parameters to keep any same-line spaces
#
CFLIST :=	$(subst $(SPACE)$(DASH),$(TAB)$(DASH),$(CFLIST))
CFLIST :=	$(subst $(SPACE),$(FILL),$(CFLIST))

#---------------------------------------
#
# C flags
#
CFLAGS=	-DNBA -D$(GRX_HARDWARE) -D$(TARGET_SYS) -D$(BUILDMODE) $(LOCALS) $(LOCALS2) @$(CFFILE) -O

#
# Compiler command lines
#
CC3=	@ccmips $(CFLAGS)3
CC2=	@ccmips $(CFLAGS)2
CC1=	@ccmips $(CFLAGS)1
CC0=	@ccmips $(CFLAGS)0

#
# Assembler command line
#
AS2=	@asmmips /q /of.rdata /zd /l /oc+

#
# Linker command line
#
LINK=	@psylink /s /c /m /wl /wm

#
# Phony make targets that don't want any implicit dependencies
#
.PHONY:	all makebin makecpe makeexe makemap $(SYS_LIBS) $(GAME_LIBS)

#
# Tell make NOT to delete .o files when done
#
.PRECIOUS:	$(OBJS)

#---------------------------------------
#
# What we are going to build
#
all:		$(CFFILE) $(LNKFILE) $(BASE_NAME).cpe $(BASE_NAME).exe

makebin:	$(CFFILE) $(LNKFILE) $(BASE_NAME).bin

makecpe:	$(CFFILE) $(LNKFILE) $(BASE_NAME).cpe

makeexe:	$(CFFILE) $(LNKFILE) $(BASE_NAME).exe

makemap:	$(CFFILE) $(LNKFILE) $(BASE_NAME).map

#---------------------------------------
#
# Dependencies and rules
#
$(BASE_NAME).bin:	$(OBJS) $(DEP_LIBS)
	@echo LINKING `$(BASE_NAME).bin'
	$(LINK) /p @$(BASE_NAME).lnk,$(BASE_NAME).bin,,

$(BASE_NAME).cpe:	$(OBJS) $(DEP_LIBS)
	@echo LINKING `$(BASE_NAME).cpe'
	$(LINK) @$(BASE_NAME).lnk,$(BASE_NAME).cpe,$(BASE_NAME).sym,$(BASE_NAME).map

$(BASE_NAME).exe:	$(BASE_NAME).bin
	@echo CONVERTING `$(BASE_NAME).bin' to `$(BASE_NAME).exe'
	@bin2exe $(BASE_NAME).bin 0x$(ORG)

$(BASE_NAME).map:	$(OBJS)
	@echo MAPPING `$(BASE_NAME)'
	$(LINK) @$(BASE_NAME).lnk,,,$(BASE_NAME).map

$(CFFILE):	$(CFDEPS)
	@if EXIST $(CFFILE) del $(CFFILE)
	@$(foreach ITEM,$(CFLIST),echo>>$(CFFILE) $(subst $(FILL),$(SPACE),$(ITEM))$(NEWLINE))

$(LNKFILE):	$(LNKDEPS)
	@if EXIST $(LNKFILE) del $(LNKFILE)
	@$(foreach ITEM,$(LNKMISC),echo>>$(LNKFILE) $(TAB)$(subst $(COMMA),$(TAB),$(ITEM))$(NEWLINE))
	@echo.>>$(LNKFILE)
	@$(foreach ITEM,$(GRPS),echo>>$(LNKFILE) $(subst $(COMMA),$(TAB)$(LNKGRP)$(TAB),$(ITEM))$(NEWLINE))
	@echo.>>$(LNKFILE)
	@$(foreach ITEM,$(SECTS),echo>>$(LNKFILE) $(TAB)$(LNKSECT)$(TAB)$(ITEM)$(NEWLINE))
	@echo.>>$(LNKFILE)
	@$(foreach ITEM,$(OBJS),echo>>$(LNKFILE) $(TAB)$(LNKOBJ)$(TAB)"$(ITEM)"$(NEWLINE))
	@echo.>>$(LNKFILE)
	@$(foreach ITEM,$(LIBS),echo>>$(LNKFILE) $(TAB)$(LNKLIB)$(TAB)"$(ITEM)"$(NEWLINE))

ani3d/$(PREFIX)ani3d$(SUFFIX).a:
	@$(MAKE) --no-print-directory -C ani3d TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE)

/video/lib/$(PREFIX)goose$(SUFFIX).a:
	@$(MAKE) --no-print-directory -C /video/lib/goose TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) GAME=NBA

/video/lib/$(PREFIX)glide$(SUFFIX).a:
	@$(MAKE) --no-print-directory -C /video/lib/glide TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) GAME=NBA

/video/lib/$(SOUND_P)sound$(SUFFIX).a:
	@$(MAKE) --no-print-directory -C /video/lib/sound TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) GAME=NBA

%.o:	%.c
	@echo $< to $@
	$(CC3) -o $@ $<

%.o:	%.s
	@echo $< to $@
	$(AS2) $<,$@

%.d:	%.c $(CFFILE) $(LNKFILE)
	@echo $< to $@
	$(CC3) -M -o $@ $<
	@tweakd $@

cambot.o:	cambot.c
	@echo $< to $@
	$(CC1) -o $@ $< 

player.o:	player.c
	@echo $< to $@
	$(CC1) -o $@ $< 

nba.o:	nba.c
	@echo $< to $@
	$(CC1) -o $@ $< 

detect.o:	detect.c
	@echo $< to $@
	$(CC1) -o $@ $< 

attract.o:	attract.c
	@echo $< to $@
	$(CC0) -o $@ $< 

arena.o:	arena.c
	@echo $< to $@
	$(CC0) -o $@ $< 

dribbler.o:	dribbler.c
	@echo $< to $@
	$(CC0) -o $@ $< 

drone.o:	drone.c
	@echo $< to $@
	$(CC3) -o $@ $< 

plyrinfo.o:	plyrinfo.c
	@echo $< to $@
	$(CC0) -o $@ $< 

audinfo.o:	audinfo.c
	@echo $< to $@
	$(CC1) -o $@ $< 

ball.o:	ball.c
	@echo $< to $@
	$(CC3) -o $@ $< 
#	$(CC3) -S -fverbose-asm -o x.tmp $<

select.o:	select.c
	@echo $< to $@
	$(CC0) -o $@ $< 

vsscreen.o:	vsscreen.c
	@echo $< to $@
	$(CC0) -o $@ $< 

particle.o:	particle.c
	@echo $< to $@
	$(CC3) -o $@ $<
#	$(CC3) -S -fverbose-asm -o x.tmp $<

sounds.o:	sounds.c
	@echo $< to $@
	$(CC1) -o $@ $< 

trivia.o:	trivia.c
	@echo $< to $@
	$(CC1) -o $@ $< 

plyrseq.o:	anim/gen_seq.equ

ifneq	($(findstring localset.mak, $(wildcard *.mak)),)
arena.o		: localset.mak
attract.o	: localset.mak
ball.o		: localset.mak
cambot.o	: localset.mak
coin.o		: localset.mak
drone.o		: localset.mak
mainproc.o	: localset.mak
nba.o		: localset.mak
player.o	: localset.mak
plyrinfo.o	: localset.mak
pmisc.o		: localset.mak
putil.o		: localset.mak
replay.o	: localset.mak
sndinfo.o	: localset.mak
vsscreen.o	: localset.mak
trivia.o	: localset.mak
endif

#---------------------------------------
#
# Dependency files
#
-include	$(CSRCS:.c=.d)
