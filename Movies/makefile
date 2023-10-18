MOVIES=star.mov atts.mov mway.mov
moviedir=\\chivid1_fs\image1\wwf\nba\movies
moviedirdd=\\\\chivid1_fs\\image1\\wwf\\nba\\movies
MOVIEIN=.
MOVIEOUT=.
MOVIECNT=.

.PHONEY:	delobs

all:	update_disk

movies:	$(MOVIES)

star.mov:	delobs
	copy $(moviedir)\$(basename  $@)\$(basename $@)*.tga /y
	tgasplit $(basename $@)t.tga
	texus -mn -t argb1555 -o $(basename $@)1.3df $(basename $@)1.tga
	texus -mn -t argb1555 -o $(basename $@)2.3df $(basename $@)2.tga
	3df2wms $(basename $@)1.3df
	3df2wms $(basename $@)2.3df
	-movie 65 $(basename $@)0001 -b 0
ifneq	($(wildcard *.cnt),)
	@attrib -r $(basename $@).cnt
	@catcnt $(basename $@).cnt
	@attrib +r $(basename $@).cnt
endif
	del $(basename $@)1.tga
	del $(basename $@)2.tga
	del $(basename $@)1.3df
	del $(basename $@)2.3df
	del $(basename $@)*.tga

mway.mov:	delobs
	reseq $(basename $@) $(MOVIEIN)\\$(basename $@) 0
ifneq	($(wildcard *.cnt),)
	@attrib -r $(basename $@).cnt
	@catcnt $(basename $@).cnt
	@attrib +r $(basename $@).cnt
endif

atts.mov: delobs
	reseq $(basename $@) $(moviedirdd)\\$(basename $@) 0
ifneq	($(wildcard *.cnt),)
	@attrib -r $(basename $@).cnt
	@catcnt $(basename $@).cnt
	@attrib +r $(basename $@).cnt
endif


catcnts:	delobs
ifneq	($(wildcard *.cnt),)
	@attrib -r *.cnt
	@catcnt $(wildcard *.cnt)
	@attrib +r *.cnt
endif


update_disk:	delobs
	@psyreset
ifneq	($(wildcard *.cnt),)
	@attrib -r *.cnt
	catcnt $(wildcard *.cnt)
	@attrib +r *.cnt
endif
	@pcopy *.wms phx: /u
	@pcopy *.cnt phx: /u
	@writem . star
	@writem . atts
	@writem . mway

delobs:
	@echo DELETING OBSOLETE FILES
ifneq	($(wildcard 1pom.*),)
	@attrib -r 1pom.*
	@del 1pom.*
endif
ifneq	($(wildcard snap.*),)
	@attrib -r snap.*
	@del snap.*
endif
ifneq	($(wildcard frun.*),)
	@attrib -r frun.*
	@del frun.*
endif
	@echo OBSOLETE FILE DELETION DONE
