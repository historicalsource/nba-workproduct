#---------------------------------------
#
# Makefile for NBA Img3d - Images
#
# $Revision: 162 $
#

#---------------------------------------
#
# Useful macros
#
NULL	:=
PRCNT	:=	$(NULL)%$(NULL)

#---------------------------------------
#
# Target modules
#

# ALC # All-Stars (Court)

LIST= \
	atl		(Atlanta Hawks)         \
	bos		(Boston Celtics)        \
	cha		(Charlotte Hornets)     \
	chi		(Chicago Bulls)         \
	cle		(Clevland Cavaliers)    \
	dal		(Dallas Mavericks)      \
	den		(Denver Nuggets)        \
	det		(Detroit Pistons)       \
	gol		(Golden_State Warriors) \
	hou		(Houston Rockets)       \
	ind		(Indianapolis Pacers)   \
	lac		(Los_Angeles Clippers)  \
	lal		(Los_Angeles Lakers)    \
	mia		(Miama Heat)            \
	mil		(Milwaukee Bucks)       \
	min		(Minnesota Timberwolves)\
	njn		(New_Jersey Nets)       \
	nyk		(New_York Knicks)       \
	orl		(Orlando Magic)         \
	phi		(Philadelphia 76ers)    \
	pho		(Phoenix Suns)          \
	por		(Portland Trailblazers) \
	sac		(Sacramento Kings)      \
	san		(San_Antonio Spurs)     \
	sea		(Seattle Supersonics)   \
	tor		(Toronto Raptors)       \
	uta		(Utah Jazz)             \
	van		(Vancouver Grizzlies)   \
	was		(Washington Wizards)
#
#	ale		(All-Stars East)        \
#	alw		(All-Stars West)

FOUT= (% %) "% %" '% %'

TEAMS= $(strip $(filter-out $(FOUT),$(LIST)))

#
# Courts
#
CRTA := $(wildcard ???a*.tga)
TEMP := $(patsubst %,%a$(PRCNT).tga,$(TEAMS))
TENP := $(filter $(TEMP),$(CRTA))
CRTA := $(patsubst %.tga,%.3df,$(TENP))

CRTN := $(wildcard ???n*.tga)
TEMP := $(patsubst %,%n$(PRCNT).tga,$(TEAMS))
TENP := $(filter $(TEMP),$(CRTN))
CRTN := $(patsubst %.tga,%.3df,$(TENP))

#
# Court Specific Texture Replacements
#
TABL = $(patsubst %,%tabl.3df,$(TEAMS))
GREF = $(patsubst %,%gref.3df,$(TEAMS))
TOPC = $(patsubst %,%topc.3df,$(TEAMS))
CUSH = $(patsubst %,%cush.3df,$(TEAMS))
BANR = $(patsubst %,%bannr.3df,$(TEAMS))

#
# Crowd
#
CROWD = $(patsubst %.tga,%.3df,$(wildcard crd*.tga))

#
# Team jerseys
#
JERSEYS =\
	$(patsubst %,%jrsyh.3df,$(TEAMS))\
	$(patsubst %,%jrsya.3df,$(TEAMS))\
	$(patsubst %,%jrshf.3df,$(TEAMS))\
	$(patsubst %,%jrsaf.3df,$(TEAMS))\
	chijrsyx.3df\
	miajrsyx.3df\
	miljrsyx.3df\
	minjrsyx.3df\
	orljrsyx.3df\
	phojrsyx.3df\
	sacjrsyx.3df\
	vanjrsyx.3df\
	chijrsxf.3df\
	miajrsxf.3df\
	miljrsxf.3df\
	minjrsxf.3df\
	orljrsxf.3df\
	phojrsxf.3df\
	sacjrsxf.3df\
	vanjrsxf.3df\
	cusjrsbf.3df\
	cusjrsgf.3df\
	cusjrsjf.3df\
	cusjrskf.3df\
	cusjrspf.3df\
	cusjrsrf.3df\
	cusjrstf.3df\
	cusjrswf.3df\
	cusjrsyf.3df\
	cusjrsxf.3df\
	cusjrsyb.3df\
	cusjrsyg.3df\
	cusjrsyj.3df\
	cusjrsyk.3df\
	cusjrsyp.3df\
	cusjrsyr.3df\
	cusjrsyt.3df\
	cusjrsyw.3df\
	cusjrsyy.3df\
	cusjrsyx.3df\
	midjrsyf.3df\
	midjrsym.3df\
	jrsycl_m.3df\
	jrsycl_n.3df\
	cusjrszf.3df\
	cusjrsyz.3df\
	jrsyalib.3df\
	jrsyalig.3df\
	jrsyalik.3df\
	jrsyalip.3df\
	jrsyalir.3df\
	jrsyalit.3df\
	jrsyaliw.3df\
	jrsyaliy.3df\
	jrsywizb.3df\
	jrsywizg.3df\
	jrsywizk.3df\
	jrsywizp.3df\
	jrsywizr.3df\
	jrsywizt.3df\
	jrsywizw.3df\
	jrsywizy.3df\
	jrsywizx.3df\
	jrsyclob.3df\
	jrsyclog.3df\
	jrsyclok.3df\
	jrsyclop.3df\
	jrsyclor.3df\
	jrsyclot.3df\
	jrsyclow.3df\
	jrsycloy.3df\
	jrsypumb.3df\
	jrsypumg.3df\
	jrsypumk.3df\
	jrsypump.3df\
	jrsypumr.3df\
	jrsypumt.3df\
	jrsypumw.3df\
	jrsypumy.3df\
	jrsyfrnk.3df\
	jrsymumy.3df\
	jrsbride.3df\
	jrswolfm.3df\
	jrsycret.3df\
	jrsyreff.3df\
	jrsycl_b.3df\
	jrsycl_g.3df\
	jrsycl_j.3df\
	jrsycl_k.3df\
	jrsycl_p.3df\
	jrsycl_r.3df\
	jrsycl_t.3df\
	jrsycl_w.3df\
	jrsycl_y.3df\
	cha_horn.3df\
	chijrsym.3df\
	indjrsym.3df\
	torjrsym.3df\
	houjrsym.3df\
	atljrsym.3df\
	phojrsym.3df\
	denjrsym.3df\
	seajrsym.3df\
	minjrsym.3df\
	njnjrsym.3df\
	utajrsym.3df

JNUMS =\
	$(patsubst %,%jnumh.3df,$(TEAMS))\
	$(patsubst %,%jnuma.3df,$(TEAMS))\
	chijnumx.3df\
	miajnumx.3df\
	miljnumx.3df\
	minjnumx.3df\
	orljnumx.3df\
	phojnumx.3df\
	sacjnumx.3df\
	vanjnumx.3df\
	alianjrn.3df\
	wizjnumh.3df\
	pumjnumh.3df

PLAYER_REFLECTIONS =\
	rlg_llch.3df\
	rlg_llcb.3df\
	rlg_llb.3df\
	rlg_llc.3df\
	rlg_lltr.3df\
	rlg_llhb.3df\
	rlg_llah.3df\
	rlg_llpg.3df\
	rlg_lls.3df\
	rlg_lldr.3df\
	rlg_llss.3df\
	rlg_llub.3df\
	rlg_lla.3df\
	rlg_llj.3df\
	rlg_lln.3df\
	rlg_llf.3df\
	rlg_llom.3df\
	rlg_llcc.3df\
	rlg_llp.3df\
	rlg_llho.3df\
	rlg_llw.3df\
	rlg_lluf.3df\
	rlg_llum.3df\
	rlg_lltb.3df\
	rlg_llwm.3df\
	rlg_llrm.3df\
	rlg_lluc.3df

#
# Player heads
#
HEADS = $(patsubst %.tga,%.3df,$(wildcard hd_*.tga))

#
# Player flesh
#
FLESH =\
	$(patsubst %.tga,%.3df,$(wildcard fl_*.tga))

#
# Spfx
#
SPFX = $(patsubst %.tga,%.3df,$(wildcard brst*.tga))\
	$(patsubst %.tga,%.3df,$(wildcard onfire*.tga))

#
# Image groups to be built with specified parameters
#
IMG_565MN =\
	nbaoob.3df nbalog.3df midtabl.3df midgref.3df\
	scortabl.3df scortopc.3df $(TOPC) $(TABL) $(CUSH) $(BANR)\
	micphone.3df monitor.3df tabletop.3df shcomflt.3df\
	midway.3df black.3df hoop.3df backpad.3df metal4.3df\
	seal.3df cush4.3df colorpad.3df mdwaylit.3df hyperlit.3df mdcomlit.3df backlite.3df\
	ledstrip.3df omnibits.3df omniface.3df omnifont.3df support.3df\
	nbacmlit.3df nbccmlit.3df nbcsptlt.3df showtlit.3df\
	$(patsubst %,%tag.3df,$(TEAMS)) specular.3df stuffflt.3df nbnbcflt.3df bballhlt.3df\
	midtabl.3df nbctabl.3df midtopc.3df nbctopc.3df midcush.3df nbccush.3df

IMG_4444MN = \
	$(SPFX) $(CROWD) $(GREF) $(PLAYER_REFLECTIONS) backcrwd.3df crda0001.3df crdb0001.3df crdc0001.3df\
	chrback.3df chrseat.3df midgref.3df nbcgref.3df\
	pslimb.3df pstorso.3df psshould.3df pshand.3df\
	backrefl.3df gbasrefl.3df gpstrefl.3df goalshad.3df\
\
	bracket.3df bumper.3df metal1.3df whitpost.3df\
	metal2.3df metal3.3df target.3df glass.3df\
	$(patsubst %,%jumbo.3df,$(TEAMS))

IMG_4444 =\
	omnibotm.3df omnitron.3df p_shadow.3df stad01.3df\
	shadow.3df goals.3df flchibul.3df scorshad.3df scorref2.3df namea.3df nameh.3df\
	c_x_yel.3df \
	alln01.3df alln02.3df alln03.3df alln04.3df alln05.3df\
	alln06.3df alln07.3df alln08.3df alln10.3df alla11.3df


IMG_4444M2 =\
	net.3df $(JNUMS)

IMG_4444MD =\

IMG_AI88 =\
	bball_a.3df

IMG_4444M3 =\
	pcircl_1.3df pcircl_2.3df pcircl_3.3df pcircl_4.3df pcircr_2.3df\
	pcircl_a.3df pcircl_b.3df pcircl_c.3df

IMG_565 =\
	$(JERSEYS) $(HEADS) $(FLESH) midcmflt.3df midwyflt.3df nbacmflt.3df\
	nbccmflt.3df nbcspflt.3df eastconf.3df westconf.3df barrier2.3df blankflt.3df nbaflt.3df

COURTSNA =\
	$(CRTN) bball_1.3df bballrwb.3df\
	balcony1.3df concret3.3df crowdlow.3df hallway.3df pillar.3df\
	roof01.3df roof02.3df stadium1.3df stepdark.3df tile1.3df trans1.3df step.3df\
	advertis.3df fnceref1.3df halfloor.3df fnceref2.3df fnceref3.3df pllarref.3df\
	eastadvt.3df westadvt.3df yard.3df stepred.3df stepblue.3df steppurp.3df\
	midn01.3df midn02.3df midn03.3df midn04.3df midn05.3df midn06.3df midn07.3df\
	midn08.3df midn10.3df nbcn01.3df nbcn02.3df nbcn03.3df nbcn04.3df nbcn05.3df\
	nbcn06.3df nbcn07.3df nbcn08.3df nbcn10.3df

COURTSA =\
	$(CRTA)	baselogo.3df nbaflogo.3df rail2.3df bball_A.3df chainnet.3df rwbnet.3df \
	parkgate.3df

PCOURTNA =\
	bench1.3df bench2.3df cush4.3df hoop.3df lawn.3df metal1.3df \
	parksky3.3df parn01.3df parn02.3df parn03.3df parn04.3df parn05.3df parn06.3df \
	parn07.3df parn08.3df pkwalla1.3df pkwalla2.3df pkwalla3.3df pkwalla4.3df \
	pkwalla5.3df pkwalla6.3df pkwallb1.3df pkwallb2.3df pkwallb3.3df pkwallb4.3df \
	pkwallb5.3df pkwallb6.3df pole1.3df seal.3df sidewlk1.3df sidewlk2.3df splatter.3df \
	stormfan.3df street.3df trashcan.3df parkbrd.3df rooftop.3df bench3.3df \
	sidewlk3.3df sidewlk4.3df sidewlk5.3df sidewlk6.3df bamboo2.3df \
	bamboo1.3df bch_crt1.3df bch_crt2.3df bch_crt3.3df bch_crt4.3df bch_crt5.3df\
	bch_crt6.3df bch_crt7.3df bch_crt8.3df bch_shr1.3df bch_shr2.3df inland1.3df\
	sky1.3df sky2.3df sky3.3df sky4.3df sky5.3df wave01.3df wave02.3df wave03.3df\
	wave04.3df wave05.3df wave06.3df wave07.3df wave08.3df wave09.3df wave10.3df \
	wave11.3df wave12.3df wave13.3df wave14.3df wave15.3df wave16.3df wave17.3df \
	wave18.3df bch_shr3.3df bch_shr4.3df rocks.3df streetfd.3df yardfd.3df \
	sidewlk7.3df

PCOURTA	=\
	house01.3df house02.3df house03.3df house04.3df house05.3df house06.3df parkfnce.3df \
	grssfrng.3df watrtowr.3df prkgshad.3df palmtre1.3df palmtre2.3df parkgraf.3df b_shadw.3df \
	skyline.3df housefd.3df
#
# Object files
#
3DFS=	$(IMG_565MN) $(IMG_4444MN) $(IMG_4444) $(IMG_1555) $(IMG_565) $(IMG_4444M2) $(IMG_4444M3) \
	$(IMG_565M2) $(IMG_4444MD) $(IMG_AI88) $(COURTSNA) $(COURTSA) $(PCOURTA) $(PCOURTNA)
OBJS=	$(3DFS:.3df=.wms)

#
# Phony make targets that don't want any implicit dependencies
#
.PHONY:	all prompt clean clobber rebuild_msg rebuild

#---------------------------------------
#
# What we are going to build
#
all:	prompt $(OBJS)
	@echo MAKING `NBA\IMG3D' DONE

prompt:
	@echo MAKING `NBA\IMG3D'

#---------------------------------------
#
# Dependencies and rules
#
%.wms:	%.3df
	@echo Converting $< to $@
	@3df2wms $<
	@del $<

%.3df:	%.tga
	@echo Converting $(basename $@).tga to $@
	@if not "$(findstring $@,$(IMG_565MN))"==""  texus >nul -t rgb565 -rn -dn -mn -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_565M2))"==""  texus >nul -t rgb565 -rn -dn -m2 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_4444MN))"=="" texus >nul -t argb4444 -rn -dn -mn -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_4444M2))"=="" texus >nul -t argb4444 -rn -dn -m2 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_4444M3))"=="" texus >nul -t argb4444 -rn -dn -m3 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_4444MD))"=="" texus >nul -t argb4444 -rn -mn -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_4444))"==""   texus >nul -t argb4444 -rn -dn -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_1555))"==""   texus >nul -t argb1555 -rn -dn -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_565))"==""    texus >nul -t rgb565 -rn -dn -m3 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(IMG_AI88))"==""   texus >nul -t ai88 -rn -dn -mn -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(COURTSNA))"==""   texus >nul -t rgb565 -rn -dn -m5 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(COURTSA))"==""    texus >nul -t argb4444 -rn -dn -m2 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(PCOURTA))"==""    texus >nul -t argb4444 -rn -dn -m2 -o $@ $(basename $@).tga
	@if not "$(findstring $@,$(PCOURTNA))"==""   texus >nul -t rgb565 -rn -dn -m2 -o $@ $(basename $@).tga

#---------------------------------------
#
# How to clean
#
clean:	clobber
	@echo CLEANING `NBA\IMG3D'
	@echo CLEANING `NBA\IMG3D' DONE

#
# How to clobber
#
clobber:
	@echo CLOBBERING `NBA\IMG3D'
	@if EXIST *.3df del *.3df
	@if EXIST *.wms del *.wms
	@echo CLOBBERING `NBA\IMG3D' DONE

#
# How to rebuild
#
rebuild_msg:
	@echo REBUILDING `NBA\IMG3D'

rebuild:	rebuild_msg clean all
	@echo REBUILDING `NBA\IMG3D' DONE
