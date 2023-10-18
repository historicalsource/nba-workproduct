#---------------------------------------
#
# Makefile for NBA Img2d - Images
#
# $Revision: 41 $
#

SHELL=

#---------------------------------------
#
# Target Object destination path
#
OBJPATH=	..\include

#
# Target Object modules
#
OBJS= \
	$(OBJPATH)\amode.h    $(OBJPATH)\bigfnt.h   $(OBJPATH)\cplyr.h \
	$(OBJPATH)\eoq.h      $(OBJPATH)\inits.h    $(OBJPATH)\mugshots.h \
	$(OBJPATH)\nbafont.h  $(OBJPATH)\optscrn.h  $(OBJPATH)\plyrinfo.h \
	$(OBJPATH)\plyrtext.h $(OBJPATH)\ptlimg.h   $(OBJPATH)\scorinfo.h \
	$(OBJPATH)\selplr.h   $(OBJPATH)\tmsel.h    $(OBJPATH)\vs.h \
	$(OBJPATH)\joinin.h   $(OBJPATH)\trivia.h   $(OBJPATH)\endgame.h \
	$(OBJPATH)\gamesel.h  $(OBJPATH)\sweepstk.h

#
# Phony make targets that don't want any implicit dependencies
#
.PHONY:	all prompt clean clobber rebuild_msg rebuild

#---------------------------------------
#
# Useful macros
#
define NEWLINE


endef

#
# Target Object File Name (base.ext)
#
define TOFN
$(notdir $(subst \,/,$@))
endef

#
# Target Object Base Name (base)
#
define TOBN
$(basename $(TOFN))
endef

#
# Default LOD file process
#  Use $(subst ...) to invoke, subbing PARMS with any additional Composit
#  command line options or with a null-string if none are needed
#
#  Ex: $(subst PARMS,-clipon,$(LODPROC))
#      $(subst PARMS,-t -oneh,$(LODPROC))
#      $(subst PARMS,,$(LODPROC))
#
define LODPROC
echo Generating `$(TOFN)'
if EXIST $@ del $@
if EXIST $(TOBN).bat del $(TOBN).bat
c -newpack -quiet_batch_file PARMS -lfile $(TOBN).lod >$(TOBN).log
if NOT EXIST $(TOBN).bat fail
call $(TOBN)
del *.3df
endef

#---------------------------------------
#
# What we are going to build
#
all:	prompt $(OBJS)
	@echo MAKING `NBA\IMG2D' DONE

prompt:
	@echo MAKING `NBA\IMG2D'

#---------------------------------------
#
# Dependencies and rules
#

#
# List of WIF image files needed by <amode.lod>
#
AMODE_IMGS= upfront1.wif upfront6.wif transit.wif nbacgrfx.wif

$(OBJPATH)\amode.h:	amode.lod $(AMODE_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <bigfnt.lod>
#
BIGFNT_IMGS= bastbig.wif

$(OBJPATH)\bigfnt.h:	bigfnt.lod $(BIGFNT_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <cplyr.lod>
#
CPLYR_IMGS= cplyr98.wif cpmug.wif nbaflash.wif

$(OBJPATH)\cplyr.h:	cplyr.lod $(CPLYR_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <eoq.lod>
#
ENDGAME_IMGS= splshani.wif

$(OBJPATH)\endgame.h:	endgame.lod $(ENDGAME_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <eoq.lod>
#
EOQ_IMGS= eofq.wif

$(OBJPATH)\eoq.h:		eoq.lod $(EOQ_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <inits.lod>
#
INITS_IMGS= upfront3.wif

$(OBJPATH)\inits.h:	inits.lod $(INITS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <joinin.lod>
#
JOININ_IMGS= join_in.wif

$(OBJPATH)\joinin.h:	joinin.lod $(JOININ_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <mugshots.lod>
#
MUGSHOTS_IMGS= mugshots.wif

$(OBJPATH)\mugshots.h:	mugshots.lod $(MUGSHOTS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <nbafont.lod>
#
NBAFONT_IMGS= bastfont.wif intersta.wif chattnt.wif

$(OBJPATH)\nbafont.h:	nbafont.lod $(NBAFONT_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <optscrn.lod>
#
OPTSCRN_IMGS= nbaflash.wif upfront1.wif upfront6.wif

$(OBJPATH)\optscrn.h:	optscrn.lod $(OPTSCRN_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <plyrinfo.lod>
#
PLYRINFO_IMGS= arrows2.wif infboxes.wif

$(OBJPATH)\plyrinfo.h:	plyrinfo.lod $(PLYRINFO_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <plyrtext.lod>
#
PLYRTEXT_IMGS= pnames.wif

$(OBJPATH)\plyrtext.h:	plyrtext.lod $(PLYRTEXT_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <ptl.lod>
#
PTLIMG_IMGS= ptl.wif

$(OBJPATH)\ptlimg.h:	ptlimg.lod $(PTLIMG_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <scorinfo.lod>
#
SCORINFO_IMGS= NBACgrfx.wif upfront2.wif upfront6.wif

$(OBJPATH)\scorinfo.h:	scorinfo.lod $(SCORINFO_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <selplr.lod>
#
SELPLR_IMGS= upfront5.wif

$(OBJPATH)\selplr.h:	selplr.lod $(SELPLR_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <trivia.lod>
#
TRIVIA_IMGS= trivia.wif

$(OBJPATH)\trivia.h:	trivia.lod $(TRIVIA_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <tmsel.lod>
#
TMSEL_IMGS= upfront2.wif

$(OBJPATH)\tmsel.h:	tmsel.lod $(TMSEL_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <vs.lod>
#
VS_IMGS= upfront4.wif

$(OBJPATH)\vs.h:	vs.lod $(VS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#
# List of WIF image files needed by <gamesel.lod>
#
GAMESEL_IMGS= gamesel.wif

$(OBJPATH)\gamesel.h:	gamesel.lod $(GAMESEL_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))


#
# List of WIF image files needed by <sweepstk.lod>
#
SWEEPS_IMGS= sweepstk.wif

$(OBJPATH)\sweepstk.h:	sweepstk.lod $(SWEEPS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))


#---------------------------------------
#
# How to clean
#
clean:	clobber
	@echo CLEANING `NBA\IMG2D'
	@if EXIST *.log del *.log
	@if EXIST *.bat del *.bat
	@if EXIST *.wms del *.wms
	@if EXIST *.tga del *.tga
	@if EXIST *.3df del *.3df
	@echo CLEANING `NBA\IMG2D' DONE

#
# How to clobber
#
clobber:
	@echo CLOBBERING `NBA\IMG2D'
	@$(foreach DELOBJ, $(OBJS), if exist $(DELOBJ) del $(DELOBJ)$(NEWLINE))
	@echo CLOBBERING `NBA\IMG2D' DONE

#
# How to rebuild
#
rebuild_msg:
	@echo REBUILDING `NBA\IMG2D'

rebuild:	rebuild_msg clean all
	@echo REBUILDING `NBA\IMG2D' DONE
