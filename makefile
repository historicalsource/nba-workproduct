#---------------------------------------
#
# Top level Makefile for NBA Game
#
# $Revision: 27 $
#
PHX :=	phx:

ifeq	($(DUAL_BOOT),YES)
ifndef	NBA_DRIVE
derr:
	@echo Aborting -- NBA_DRIVE environment variable not set
else
PHX :=	$(NBA_DRIVE)
endif
endif

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
PCOPYP :=	/u
endif
ifeq	($(TARGET_SYS),VEGAS)
PREFIX :=	v
PCOPYP :=	/u /c
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
DBG		:=	-DDEBUG
DBG_MSG :=	DEBUG
SUFFIX	:=	d
endif
ifeq	($(BUILDMODE),RELEASE)
DBG		:=
DBG_MSG	:=	RELEASE
SUFFIX	:=	r
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

SS :=		@c:/vss/win32/ss

#---------------------------------------
# Set to Movie File Groups to be processed
#  Ex: movies/*.wms
#      movies/*.cnt
#
MFG_WMS :=
MFG_CNT :=

#
# Phony make targets that don't want any implicit dependencies
#
.PHONY:	nba all allart everything\
	bin cpe exe map\
	makebin makecpe makeexe makemap\
	delbin delcpe delexe prompt done\
	update_disk update_game release_disk\
	copy_data copy_data_bak copy_game copy_game_bak copy_movies do_catcnts\
	filesys_chk rfilesys_chk wipe_target\
	clean clobber rebuild_msg rebuild relink_msg relink\
	clean_all_msg clean_all clobber_all_msg clobber_all\
	rebuild_all_msg rebuild_all relink_all_msg relink_all\
	clean_allart clobber_allart rebuild_allart\
	clean_everything clobber_everything rebuild_everything\
	get get_all

#---------------------------------------
#
# What we are going to build
#
# Default target
#
ifndef	CPE
nba:	prompt makecpe makeexe done
	@echo >nul

else
ifeq	($(CPE),0)
nba:	prompt delcpe makeexe done
	@echo >nul

else
ifeq	($(CPE),1)
nba:	prompt delbin makecpe done
	@echo >nul

else
ifeq	($(CPE),2)
nba:	prompt delcpe delexe makebin done
	@echo >nul

else
nba:
	@echo Aborting -- if specified, CPE environment variable must be:
	@echo 	0 to make .BIN & .EXE
	@echo 	1 to make .CPE & .MAP & .SYM
	@echo 	2 to make .BIN
endif
endif
endif
endif

#
# Alternate targets
#
all:
	@echo MAKING ALL
	@$(MAKE) --no-print-directory -C ani3d
	@$(MAKE) --no-print-directory -C . nba
	@echo MAKING ALL DONE

allart:
	@echo MAKING ALLART
	@$(MAKE) --no-print-directory -C anim
	@$(MAKE) --no-print-directory -C img2d
	@$(MAKE) --no-print-directory -C img3d
	@$(MAKE) --no-print-directory -C models
	@$(MAKE) --no-print-directory -C . all
	@echo MAKING ALLART DONE

everything:
	@echo MAKING EVERYTHING
	@$(MAKE) --no-print-directory -C /video/tools
	@$(MAKE) --no-print-directory -C /video/lib
	@$(MAKE) --no-print-directory -C . allart
	@echo MAKING EVERYTHING DONE

bin:	prompt makebin done

cpe:	prompt makecpe done

exe:	prompt makeexe done

map:	prompt makemap done

#
# Operations
#
delbin:
	@echo DELETING `$(BASE_NAME).BIN'
	@if EXIST $(BASE_NAME).bin del $(BASE_NAME).bin

delcpe:
	@echo DELETING `$(BASE_NAME).CPE'
	@if EXIST $(BASE_NAME).cpe del $(BASE_NAME).cpe

delexe:
	@echo DELETING `$(BASE_NAME).EXE'
	@if EXIST $(BASE_NAME).exe del $(BASE_NAME).exe

makebin:
	@$(MAKE) --no-print-directory -f nba.mak makebin

makecpe:
	@$(MAKE) --no-print-directory -f nba.mak makecpe

makeexe:
	@$(MAKE) --no-print-directory -f nba.mak makeexe

makemap:
	@$(MAKE) --no-print-directory -f nba.mak makemap

prompt:
	@echo $(DBG_MSG) VERSION OF `NBA' USING $(GRX_HARDWARE) ON $(TARGET_SYS)

done:
	@echo $(DBG_MSG) VERSION OF `NBA' USING $(GRX_HARDWARE) ON $(TARGET_SYS) DONE

#---------------------------------------
#
# What we are going to build on the target drive
#  Primary & backup files are clumped in order to get the best possible
#  separation (with PCOPY anyway) on the target drive
#
###update_disk:	do_catcnts filesys_chk copy_data copy_data_bak
###	@pcopy filesys.chk $(PHX)
###
###update_game:	do_catcnts rfilesys_chk copy_game copy_game_bak
###	@pcopy filesys.chk $(PHX)
###
###release_disk:	wipe_target do_catcnts rfilesys_chk copy_movies copy_data copy_game copy_data_bak copy_game_bak
###	@pcopy filesys.chk $(PHX)


copy_data:
	@pcopy models/*.dat        $(PHX) $(PCOPYP)
	@pcopy models/*.bod        $(PHX) $(PCOPYP)
	@pcopy anim/*.ani          $(PHX) $(PCOPYP)
	@pcopy img2d/*.wms         $(PHX) $(PCOPYP)
	@pcopy img3d/*.wms         $(PHX) $(PCOPYP)
	@pcopy sounds/*.bnk        $(PHX) $(PCOPYP)
	@pcopy trivia/q??.txt      $(PHX) $(PCOPYP)
	@pcopy sweeps/*.txt        $(PHX) $(PCOPYP)
ifneq	($(MFG_WMS),)
	@pcopy $(MFG_WMS)          $(PHX) $(PCOPYP)
endif
ifneq	($(MFG_CNT),)
	@pcopy $(MFG_CNT)          $(PHX) $(PCOPYP)
endif
	@pcopy ../diag/img2d/*.wms $(PHX) $(PCOPYP)
	@pcopy ../diag/*.bnk       $(PHX) $(PCOPYP)
	@pcopy ../lib/sound/*.bin  $(PHX) $(PCOPYP)

###copy_data_bak:
###	@pcopy models/*.dat        $(PHX)*.bak $(PCOPYP)
###	@pcopy models/*.bod        $(PHX)*.bak $(PCOPYP)
###	@pcopy anim/*.ani          $(PHX)*.bak $(PCOPYP)
###	@pcopy img2d/*.wms         $(PHX)*.bak $(PCOPYP)
###	@pcopy img3d/*.wms         $(PHX)*.bak $(PCOPYP)
###	@pcopy sounds/*.bnk        $(PHX)*.bak $(PCOPYP)
###	@pcopy trivia/q??.txt      $(PHX)*.bak $(PCOPYP)
###ifneq	($(MFG_WMS),)
###	@pcopy $(MFG_WMS)          $(PHX)*.bak $(PCOPYP)
###endif
###ifneq	($(MFG_CNT),)
###	@pcopy $(MFG_CNT)          $(PHX)*.bak $(PCOPYP)
###endif
###	@pcopy ../diag/img2d/*.wms $(PHX)*.bak $(PCOPYP)
###	@pcopy ../diag/*.bnk       $(PHX)*.bak $(PCOPYP)
###	@pcopy ../lib/sound/*.bin  $(PHX)*.bak $(PCOPYP)

copy_game:
	@pcopy ../diag/$(BASE_NAME).exe $(PHX)diag.exe /u /c
	@pcopy $(BASE_NAME).exe         $(PHX)game.exe /u /c

###copy_game_bak:
###	@pcopy ../diag/$(BASE_NAME).exe $(PHX)diag.bak /u /c
###	@pcopy $(BASE_NAME).exe         $(PHX)game.bak /u /c

copy_movies:
ifneq	($(MFG_WMS),)
	@$(MAKE) --no-print-directory -C movies update_disk
endif

do_catcnts:
ifneq	($(MFG_CNT),)
	@$(MAKE) --no-print-directory -C movies catcnts
endif


###filesys_chk:
###	@genfchk img2d/*.wms img3d/*.wms sounds/*.bnk /video/lib/sound/*.bin $(MFG_WMS) $(MFG_CNT) /video/diag/img2d/*.wms /video/diag/*.bnk

###rfilesys_chk:
###	@genfchk img2d/*.wms img3d/*.wms sounds/*.bnk /video/lib/sound/*.bin $(MFG_WMS) $(MFG_CNT) /video/diag/img2d/*.wms /video/diag/*.bnk /video/diag/$(BASE_NAME).bin


wipe_target:
	@tdel *.*
	@tdel *

#---------------------------------------
#
# How to clean
#
clean:	clobber
	@echo CLEANING `NBA'
	@if EXIST *.o  del *.o
	@if EXIST *.d  del *.d
	@if EXIST *.cf del *.cf
	@if EXIST *.lnk del *.lnk
	@echo CLEANING `NBA' DONE

#
# How to clobber
#
clobber:
	@echo CLOBBERING `NBA'
	@if EXIST $(BASE_NAME).bin del $(BASE_NAME).bin
	@if EXIST $(BASE_NAME).cpe del $(BASE_NAME).cpe
	@if EXIST $(BASE_NAME).exe del $(BASE_NAME).exe
	@if EXIST $(BASE_NAME).map del $(BASE_NAME).map
	@if EXIST $(BASE_NAME).sym del $(BASE_NAME).sym
	@echo CLOBBERING `NBA' DONE

#
# How to rebuild
#
rebuild_msg:
	@echo REBUILDING `NBA'

rebuild:	rebuild_msg clean nba
	@echo REBUILDING `NBA' DONE

#
# How to relink
#
relink_msg:
	@echo RELINKING `NBA'

relink:	relink_msg clobber nba
	@echo RELINKING `NBA' DONE

#---------------------------------------
#
# How to clean all
#
clean_all_msg:
	@echo CLEANING `NBA' AND `NBA\ANI3D'

clean_all:	clean_all_msg clean
	@$(MAKE) --no-print-directory -C ani3d clean
	@echo CLEANING `NBA' AND `NBA\ANI3D' DONE

#
# How to clobber all
#
clobber_all_msg:
	@echo CLOBBERING `NBA' AND `NBA\ANI3D'

clobber_all:	clobber_all_msg clobber
	@$(MAKE) --no-print-directory -C ani3d clobber
	@echo CLOBBERING `NBA' AND `NBA\ANI3D' DONE

#
# How to rebuild all
#
rebuild_all_msg:
	@echo REBUILDING `NBA' AND `NBA\ANI3D'

rebuild_all:	rebuild_all_msg clean_all all
	@echo REBUILDING `NBA' AND `NBA\ANI3D' DONE

#
# How to relink all
#
relink_all_msg:
	@echo RELINKING `NBA' AND `NBA\ANI3D'

relink_all:	relink_all_msg clobber_all all
	@echo RELINKING `NBA' AND `NBA\ANI3D' DONE

#---------------------------------------
#
# How to clean the game directories
#
clean_allart:
	@echo CLEANING `NBA' AND SUBDIRECTORIES
	@$(MAKE) --no-print-directory -C anim clean
	@$(MAKE) --no-print-directory -C img2d clean
	@$(MAKE) --no-print-directory -C img3d clean
	@$(MAKE) --no-print-directory -C models clean
	@$(MAKE) --no-print-directory -C ani3d clean
	@$(MAKE) --no-print-directory -C . clean
	@echo CLEANING `NBA' AND SUBDIRECTORIES DONE

#
# How to clobber the game directories
#
clobber_allart:
	@echo CLOBBERING `NBA' AND SUBDIRECTORIES
	@$(MAKE) --no-print-directory -C anim clobber
	@$(MAKE) --no-print-directory -C img2d clobber
	@$(MAKE) --no-print-directory -C img3d clobber
	@$(MAKE) --no-print-directory -C models clobber
	@$(MAKE) --no-print-directory -C ani3d clobber
	@$(MAKE) --no-print-directory -C . clobber
	@echo CLOBBERING `NBA' AND SUBDIRECTORIES DONE

#
# How to rebuild the game directories
#
rebuild_allart:
	@echo REBUILDING `NBA' AND SUBDIRECTORIES
	@$(MAKE) --no-print-directory -C anim rebuild
	@$(MAKE) --no-print-directory -C img2d rebuild
	@$(MAKE) --no-print-directory -C img3d rebuild
	@$(MAKE) --no-print-directory -C models rebuild
	@$(MAKE) --no-print-directory -C ani3d rebuild
	@$(MAKE) --no-print-directory -C . rebuild
	@echo REBUILDING `NBA' AND SUBDIRECTORIES DONE

#---------------------------------------
#
# How to clean everything
#
clean_everything:
	@echo CLEANING EVERYTHING
	@$(MAKE) --no-print-directory -C /video/tools clean
	@$(MAKE) --no-print-directory -C /video/lib clean
	@$(MAKE) --no-print-directory -C . clean_allart
	@echo CLEANING EVERYTHING DONE

#
# How to clobber everything
#
clobber_everything:
	@echo CLOBBERING EVERYTHING
	@$(MAKE) --no-print-directory -C /video/tools clobber
	@$(MAKE) --no-print-directory -C /video/lib clobber
	@$(MAKE) --no-print-directory -C . clobber_allart
	@echo CLOBBERING EVERYTHING DONE

#
# How to rebuild everything
#
rebuild_everthing:
	@echo REBUILDING EVERYTHING
	@$(MAKE) --no-print-directory -C /video/tools rebuild
	@$(MAKE) --no-print-directory -C /video/lib rebuild
	@$(MAKE) --no-print-directory -C . rebuild_allart
	@echo REBUILDING EVERYTHING DONE

#---------------------------------------
#
# How to get source from the source safe
#
get:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo GETTING `NBA'
	$(SS) cp $$/video/nba
	$(SS) get nba -gcd
	@echo GETTING `NBA' DONE
endif

#
# How to retrieve all game source from the source safe
#
get_all:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo GETTING `NBA' AND SUBDIRECTORIES
	$(SS) cp $$/video/nba
	$(SS) get nba -r -gcd
	@echo GETTING `NBA' AND SUBDIRECTORIES DONE
endif
