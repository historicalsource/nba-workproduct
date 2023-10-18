/*
 *		$Archive: /video/Nba/teamdata.c $
 *		$Revision: 307 $
 *		$Date: 4/24/99 7:52p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_teamdata_c = "$Workfile: teamdata.c $ $Revision: 307 $";
#endif

/*
 *		USER INCLUDES
 */
#include <stdio.h>
#include <goose/sprites.h>

#include "ani3d.h"
#include "game.h"
#include "player.h"
#include "hdextrn.h"

#include "include/plyrtext.h"
#include "include/tmsel.h"
#define	IMAGEDEFINE	1
#include "include/mugshots.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

char mg_NULL = 0;
char mn_NULL = 0;
char nm_NULL  = 0;

char logo_atl = 0;
char logo_bos = 0;
char logo_cha = 0;
char logo_chi = 0;
char logo_cle = 0;
char logo_dal = 0;
char logo_den = 0;
char logo_det = 0;
char logo_gol = 0;
char logo_hou = 0;
char logo_ind = 0;
char logo_lac = 0;
char logo_lal = 0;
char logo_mia = 0;
char logo_mil = 0;
char logo_min = 0;
char logo_njn = 0;
char logo_nyk = 0;
char logo_orl = 0;
char logo_phi = 0;
char logo_pho = 0;
char logo_por = 0;
char logo_sac = 0;
char logo_san = 0;
char logo_sea = 0;
char logo_tor = 0;
char logo_uta = 0;
char logo_van = 0;
char logo_was = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Static Player Data for each team roster

#define SPE(num)	num
#define SHO(num)	num
#define DUN(num)	num
#define PAS(num)	num
#define DRI(num)	num
#define BLO(num)	num
#define STE(num)	num
#define POW(num)	num
#define CLU(num)	num
#define DRO(num)	num
#define HOT(num)	num
#define TAL(ft, in)	((ft * 12) + in)

#define NUM(num)	0x##num
#define POS(num)	num
#define FLG(num)	num
#define SOU(num)	num

#define HSC(num)	num##F
#define HLM(str)	&limb_##str
#define MUG(str)	&##str
//#define MUG(str)	&mg_##str
#define MIN(str)	&mn_##str
#define NAM(str)	&nm_##str
#define TXT(str)	#str

#define WGH(num)	num
#define TPT(num)	num

#define GRD	0
#define FWD	1
#define CTR	2

#define UNUSED	0

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Atlanta Hawks
const struct stat_plyr_data spd_atl[] = {
 { SPE(15),    SHO(16),   DUN(8),	  	PAS(14),    DRI(12),
   BLO(10),    STE(13),   POW(10), 		CLU(18),     DRO(9),     HOT(4),
   TAL(6, 8),  NUM(8),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.12),   HLM(ssmit), MUG(ssmit), MIN(NULL),  NAM(ssmit), TXT(ssmit),
   "Smith", "Steve",       WGH(9),   TPT(14)	// 8 G 6-8 215 3/31/69 Michigan State '91 7 
 },
 { SPE(18),     SHO(11),     DUN(1),     PAS(16),     DRI(16),
   BLO(2),     STE(19),     POW(5),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 1),  NUM(10),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.12),   HLM(mblay), MUG(mblay), MIN(NULL),  NAM(mblay), TXT(mblay),
   "Blaylock", "Mookie",   WGH(6),   TPT(15)	// 10 G 6-1 185 3/20/67 Oklahoma '89 9 
 },
 { SPE(9),     SHO(12),     DUN(13),     PAS(7),     DRI(4),
   BLO(19),     STE(8),     POW(17),    CLU(9),     DRO(9),     HOT(1),
   TAL(7, 2),  NUM(55),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.17),  HLM(dmuto), MUG(dmuto), MIN(NULL),  NAM(dmuto), TXT(dmuto),
   "Mutombo", "Dikembe",   WGH(12),   TPT(2)	// 55 C 7-2 261 6/25/66 Georgetown '91 7 
 },
 { SPE(8),     SHO(7),     DUN(10),    PAS(8),     DRI(9),
   BLO(13),    STE(10),    POW(14),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 9),  NUM(44),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.12),   HLM(ahend), MUG(ahend), MIN(NULL),  NAM(ahend), TXT(ahend),
   "Henderson", "Alan",    WGH(11),   TPT(3)	// 44 F 6-9 235 12/2/72 Indiana '95 3 
 },
 { SPE(12),     SHO(9),     DUN(9),     PAS(9),     DRI(9),
   BLO(9),     STE(15),     POW(14),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 8),  NUM(20),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.19),   HLM(lelli), MUG(lelli), MIN(NULL),  NAM(lelli), TXT(lelli),
   "Ellis", "LaPhonso",    WGH(12),   TPT(9)	// 20 LaPhonso Ellis # F 6-8 240 5/5/70 Notre Dame '92 6 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Boston Celtics
const struct stat_plyr_data spd_bos[] = {
 { SPE(18),     SHO(13),     DUN(3),     PAS(19),     DRI(17),
   BLO(2),     STE(17),     POW(5),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 1),  NUM(7),     POS(GRD),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.07),  HLM(kande), MUG(kande), MIN(NULL),  NAM(kande), TXT(kande),
   "Anderson", "Kenny",    WGH(6),   TPT(12)	// 7 G 6-1 168 10/9/70 Georgia Tech '91 7 
 },
 { SPE(15),     SHO(12),     DUN(15),     PAS(10),     DRI(13),
   BLO(9),     STE(7),     POW(16),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 9),  NUM(8),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(awalk), MUG(awalk), MIN(NULL),  NAM(awalk), TXT(awalk),
   "Walker", "Antoine",    WGH(13),   TPT(9)	// 8 F 6-9 245 8/12/76 Kentucky '96 2 
 },
 { SPE(14),     SHO(13),     DUN(14),     PAS(12),     DRI(12),
   BLO(8),     STE(9),     POW(8),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 7),  NUM(5),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.03),   HLM(rmerc), MUG(rmerc), MIN(NULL),  NAM(rmerc), TXT(rmerc),
   "Mercer", "Ron",        WGH(8),   TPT(11)	// 5 G 6-7 210 5/18/76 Kentucky '97 5 
 },
 { SPE(14),     SHO(15),     DUN(3),     PAS(12),     DRI(15),
   BLO(2),     STE(14),     POW(1),    CLU(9),     DRO(9),     HOT(8),
   TAL(5, 11), NUM(11),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.12),  HLM(dbarr), MUG(dbarr), MIN(NULL),  NAM(dbarr), TXT(dbarr),
   "Barros", "Dana",       WGH(5),   TPT(16)	// 11 G 5-11 163 4/13/67 Boston College '89 9 
 },
 { SPE(10),     SHO(11),     DUN(9),     PAS(5),     DRI(7),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 11),  NUM(40),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.15),   HLM(tbatt), MUG(tbatt), MIN(NULL),  NAM(tbatt), TXT(tbatt),
   "Battie", "Tony", 	   WGH(10),   TPT(5)	// 40 C 6-11 240 2/11/76 Texas Tech '97 1 
 },
 { SPE(15),     SHO(14),     DUN(14),     PAS(11),     DRI(10),
   BLO(9),     STE(14),     POW(11),    CLU(9),     DRO(9),     HOT(10),
   TAL(6, 7),  NUM(34),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.15),   HLM(ppier), MUG(ppier), MIN(NULL),  NAM(ppier), TXT(ppier),
   "Pierce", "Paul", 	   WGH(9),   TPT(11)
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Charlotte Hornets
const struct stat_plyr_data spd_cha[] = {
 { SPE(17),     SHO(14),     DUN(15),     PAS(5),     DRI(11),
   BLO(8),     STE(14),     POW(5),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 6),  NUM(6),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.12),   HLM(ejone), MUG(ejone), MIN(NULL),  NAM(ejone), TXT(ejone),
   "Jones", "Eddie",       WGH(6),   TPT(13)	// 6 Eddie Jones  F 6-6 200 10/20/71 Temple '94 4 
 },
 { SPE(8),     SHO(10),     DUN(12),     PAS(15),     DRI(16),
   BLO(11),     STE(3),     POW(18),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 8),  NUM(14),    POS(FWD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.12),  HLM(amaso), MUG(amaso), MIN(NULL),  NAM(amaso), TXT(amaso),
   "Mason", "Anthony",     WGH(14),   TPT(5)	// 14 F 6-8 250 12/14/66 Tennessee State '88 9 
 },
 { SPE(14),     SHO(9),     DUN(3),     PAS(9),     DRI(10),
   BLO(3),     STE(9),     POW(7),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 5),  NUM(13),    POS(GRD),   FLG(BLK),              SOU(UNUSED),
   HSC(1.08),   HLM(bphil), MUG(bphil), MIN(NULL),  NAM(bphil), TXT(bphil),
   "Phills", "Bobby",      WGH(7),   TPT(9)	// 13 F 6-5 226 12/20/69 Southern University '91 7 
 },
 { SPE(8),     SHO(15),     DUN(11),     PAS(9),     DRI(4),
   BLO(12),     STE(9),     POW(15),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 10), NUM(44),    POS(FWD),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.20),   HLM(dcole), MUG(dcole), MIN(NULL),  NAM(dcole), TXT(dcole),
   "Coleman", "Derrick",   WGH(14),   TPT(9)	// 44 F 6-10 260 6/21/67 Syracuse '90 8 
 },
 { SPE(11),     SHO(14),     DUN(15),     PAS(5),     DRI(3),
   BLO(15),     STE(4),     POW(14),    CLU(9),     DRO(9),     HOT(16),
   TAL(7, 0),  NUM(41),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.20),   HLM(ecamp), MUG(ecamp), MIN(NULL),  NAM(ecamp), TXT(ecamp),
   "Campbell", "Elden",       WGH(15),   TPT(4)	// 41 Elden Campbell  ???
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Chicago Bulls
const struct stat_plyr_data spd_chi[] = {
 { SPE(14),     SHO(16),     DUN(9),     PAS(16),     DRI(14),
   BLO(11),     STE(12),     POW(11),    CLU(12),     DRO(9),     HOT(9),
   TAL(6, 11), NUM(7),     POS(GRD),   FLG(WHT|BIG|LEFT),     SOU(UNUSED),
   HSC(1.10),   HLM(tkuko), MUG(tkuko), MIN(NULL),  NAM(tkuko), TXT(tkuko),
   "Kukoc", "Toni",        WGH(10),   TPT(13)	// 7 F 6-11 232 9/18/68 Croatia 5 
 },
 { SPE(14),     SHO(12),     DUN(7),     PAS(9),     DRI(11),
   BLO(3),     STE(9),     POW(8),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 6),  NUM(9),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.12),   HLM(rharp), MUG(rharp), MIN(NULL),  NAM(rharp), TXT(rharp),
   "Harper", "Ron",        WGH(8),   TPT(11)	// 9 G 6-6 216 1/20/64 Miami (Ohio) '86 12 
 },
 { SPE(14),     SHO(13),     DUN(18),     PAS(11),     DRI(13),
   BLO(7),     STE(11),     POW(7),    CLU(10),     DRO(9),     HOT(3),
   TAL(6, 6),  NUM(31),     POS(GRD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.16),   HLM(bbarr), MUG(bbarr), MIN(NULL),  NAM(bbarr), TXT(bbarr),
   "Barry", "Brent",       WGH(6),   TPT(11)	// 0 G 6-6 195 12/31/71 Oregon State '95 3 
 },
 { SPE(10),    SHO(9),     DUN(9),     PAS(9),     DRI(4),
   BLO(15),     STE(9),     POW(15),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 11), NUM(28),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
  HSC(1.14),   HLM(alang), MUG(alang), MIN(NULL),  NAM(alang), TXT(alang),
   "Lang", "Andrew",	   WGH(12),   TPT(3)	// 50 F 6-10 235 8/19/73 Alabama '96 2 
 },
 { SPE(15),     SHO(12),     DUN(7),     PAS(12),     DRI(12),
   BLO(9),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 9),  NUM(18),    POS(GRD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.20),   HLM(kdavi), MUG(kdavi), MIN(NULL),  NAM(kdavi), TXT(kdavi),
   "David", "Kornel",	   WGH(13),   TPT(9)	// 18 F 6-9 235 10/22/71 null R 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Cleveland Cavaliers
const struct stat_plyr_data spd_cle[] = {
 { SPE(15),     SHO(12),     DUN(19),     PAS(7),     DRI(7),
   BLO(16),     STE(6),     POW(16),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 10), NUM(4),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(skemp), MUG(skemp), MIN(NULL),  NAM(skemp), TXT(skemp),
   "Kemp", "Shawn",        WGH(14),   TPT(2)	// 4 F 6-10 280 11/26/69 High School - Concord HS (IN) 9 
 },
 { SPE(18),     SHO(12),     DUN(1),     PAS(15),     DRI(18),
   BLO(1),     STE(12),     POW(3),    CLU(9),     DRO(9),     HOT(9),
   TAL(5, 10), NUM(12),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.03),  HLM(bknig), MUG(bknig), MIN(NULL),  NAM(bknig), TXT(bknig),
   "Knight", "Brevin",     WGH(4),   TPT(9)	// 12 G 5-10 173 11/8/75 Stanford '97 1 
 },
 { SPE(15),     SHO(14),     DUN(3),     PAS(12),     DRI(16),
   BLO(2),     STE(11),     POW(3),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 5),  NUM(3),     POS(GRD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.085),   HLM(bsura), MUG(bsura), MIN(NULL),  NAM(bsura), TXT(bsura),
   "Sura", "Bob",          WGH(7),   TPT(14)	// 3 G 6-5 200 3/25/73 Florida State '95 3 
 },
 { SPE(8),     SHO(11),     DUN(12),   PAS(12),     DRI(2),
   BLO(18),    STE(7),     POW(17),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 3),  NUM(11),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.13),   HLM(zilga), MUG(zilga), MIN(NULL),  NAM(zilga), TXT(zilga),
   "Ilgauskas", "Zydrunas",WGH(17),   TPT(1)	// 11 C 7-3 260 6/5/75 Atletas (Lithuania) 1 
 },
 { SPE(14),     SHO(15),     DUN(5),     PAS(12),     DRI(15),
   BLO(4),     STE(12),     POW(4),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 6),  NUM(1),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(wpers), MUG(wpers), MIN(NULL),  NAM(wpers), TXT(wpers),
   "Person", "Wesley",     WGH(7),   TPT(12)	// 1 F 6-6 195 3/28/71 Auburn '94 4 
 },
 { SPE(10),     SHO(13),     DUN(6),     PAS(8),     DRI(9),
   BLO(5),     STE(12),     POW(6),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 7),  NUM(20),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.13),   HLM(jnewm), MUG(jnewm), MIN(NULL),  NAM(jnewm), TXT(jnewm),
   "Newman", "Johnny",     WGH(10),   TPT(9)	// 20 Johnny Newman  F 6-7 210 11/28/63 Richmond '86 12 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Dallas Mavericks
const struct stat_plyr_data spd_dal[] = {
 { SPE(17),     SHO(15),     DUN(16),     PAS(9),     DRI(15),
   BLO(7),     STE(9),     POW(4),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 7),  NUM(4),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.05),  HLM(mfinl), MUG(mfinl), MIN(NULL),  NAM(mfinl), TXT(mfinl),
   "Finley", "Michael",    WGH(6),   TPT(11)	// 4 Michael Finley  F 6-7 215 3/6/73 Wisconsin '95 3 
 },
 { SPE(15),     SHO(16),     DUN(9),     PAS(5),     DRI(9),
   BLO(7),     STE(12),     POW(11),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 7),  NUM(23),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(cceba), MUG(cceba), MIN(NULL),  NAM(cceba), TXT(cceba),
   "Ceballos", "Cedric",   WGH(9),   TPT(14)	// 23 Cedric Ceballos  F 6-7 220 8/2/69 CS-Fullerton '90 8 
 },
 { SPE(9),     SHO(13),     DUN(9),     PAS(7),     DRI(4),
   BLO(9),     STE(5),     POW(14),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 9),  NUM(45),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.05),  HLM(agree), MUG(agree), MIN(NULL),  NAM(agree), TXT(agree),
   "Green", "A.C.",        WGH(10),   TPT(14)	// 45 A.C. Green  F 6-9 225 10/4/63 Oregon State '85 13 
 },
 { SPE(7),     SHO(10),     DUN(16),     PAS(10),     DRI(2),
   BLO(19),     STE(3),     POW(9),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 6),  NUM(44),    POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),  HLM(sbrad), MUG(sbrad), MIN(NULL),  NAM(sbrad), TXT(sbrad),
   "Bradley", "Shawn",     WGH(8),   TPT(3)	// 44 Shawn Bradley  C 7-6 263 3/22/72 Brigham Young '91 5 
 },
 { SPE(17),     SHO(14),     DUN(2),     PAS(12),     DRI(14),
   BLO(2),     STE(14),     POW(3),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 3),  NUM(13),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),  HLM(snash), MUG(snash), MIN(NULL),  NAM(snash), TXT(snash),
   "Nash", "Steve",		   WGH(7),   TPT(12)	// 13 Steve Nash  G 6-3 195 2/7/74 Santa Clara '96 2 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Denver Nuggets
const struct stat_plyr_data spd_den[] = {
 { SPE(15),     SHO(14),   DUN(18),     PAS(5),     DRI(9),
   BLO(11),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 9),  NUM(24),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),   HLM(amcdy), MUG(amcdy), MIN(NULL),  NAM(amcdy), TXT(amcdy),
   "McDyess", "Antonio",   WGH(9),   TPT(12)	// 24 Antonio McDyess  F 6-9 220 9/7/74 Alabama '95 3 
 },
 { SPE(18),     SHO(15),     DUN(1),     PAS(16),     DRI(18),
   BLO(1),     STE(14),     POW(2),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 1),  NUM(31),    POS(GRD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(0.96),   HLM(nvane), MUG(nvane), MIN(NULL),  NAM(nvane), TXT(nvane),
   "Van Exel", "Nick",     WGH(6),   TPT(12)	// 31 Nick Van Exel  G 6-1 190 11/27/71 Cincinnati '93 5 
 },
 { SPE(11),     SHO(12),     DUN(10),     PAS(12),     DRI(14),
   BLO(2),     STE(14),     POW(4),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 5),  NUM(23),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(bstit), MUG(bstit), MIN(NULL),  NAM(bstit), TXT(bstit),
   "Stith", "Bryant",      WGH(9),   TPT(12)	// 23 Bryant Stith  G 6-5 208 12/10/70 Virginia '92 6 
 },
 { SPE(16),     SHO(13),     DUN(4),     PAS(15),     DRI(14),
   BLO(2),     STE(14),     POW(3),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 3),  NUM(4),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.065),   HLM(cbill), MUG(cbill), MIN(NULL),  NAM(cbill), TXT(cbill),
   "Billups", "Chauncy",   WGH(8),   TPT(10)	// 3 Chauncey Billups  G 6-3 202 9/25/76 Colorado '97 1 
 },
 { SPE(11),     SHO(15),     DUN(7),     PAS(7),     DRI(4),
   BLO(9),     STE(5),     POW(15),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 7),  NUM(15),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),   HLM(dfort), MUG(dfort), MIN(NULL),  NAM(dfort), TXT(dfort),
   "Fortson", "Danny",   WGH(11),   TPT(5)
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Detroit Pistons
const struct stat_plyr_data spd_det[] = {
 { SPE(17),     SHO(16),     DUN(15),     PAS(18),     DRI(15),
   BLO(11),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 8),  NUM(33),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.12),   HLM(ghill), MUG(ghill), MIN(NULL),  NAM(ghill), TXT(ghill),
   "Hill", "Grant",        WGH(10),   TPT(6)	// 33 Grant Hill  F 6-8 225 10/5/72 Duke '94 4 
 },
 { SPE(18),     SHO(13),     DUN(1),     PAS(15),     DRI(19),
   BLO(2),     STE(15),     POW(4),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 2),  NUM(1),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.05),   HLM(lhunt), MUG(lhunt), MIN(NULL),  NAM(lhunt), TXT(lhunt),
   "Hunter", "Lindsey",    WGH(6),   TPT(12)	// 1 Lindsey Hunter  G 6-2 195 12/3/70 Jackson State '93 5 
 },
 { SPE(7),     SHO(12),     DUN(7),     PAS(12),     DRI(11),
   BLO(14),     STE(7),     POW(16),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 11), NUM(32),    POS(FWD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.20), HLM(claet), MUG(claet), MIN(NULL),  NAM(claet), TXT(claet),
   "Laettner", "Christian",WGH(13),   TPT(14)	// 32 Christian Laettner  F 6-11 245 8/17/69 Duke '92 6 
 },
 { SPE(11),     SHO(12),     DUN(12),     PAS(15),     DRI(2),
   BLO(14),     STE(5),     POW(16),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 11), NUM(8),     POS(CTR),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.10),   HLM(bdele), MUG(bdele), MIN(NULL),  NAM(bdele), TXT(bdele),
   "Dele", "Bison",	 	   WGH(14),   TPT(4)	// 8 Bison Dele  C 6-11 260 4/6/69 Arizona 7 (Brian Williams!)
 },
 { SPE(16),     SHO(13),     DUN(19),     PAS(10),     DRI(11),
   BLO(7),     STE(14),     POW(9),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 6),  NUM(42),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.07),   HLM(jstac), MUG(jstac), MIN(NULL),  NAM(jstac), TXT(jstac),
   "Stackhouse", "Jerry",  WGH(9),   TPT(15)	// 42 Jerry Stackhouse  G 6-6 218 11/5/74 North Carolina '95 3 
 },
 { SPE(13),     SHO(14),     DUN(9),     PAS(12),     DRI(9),
   BLO(4),     STE(12),     POW(9),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 3),  NUM(4),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.06),   HLM(jduma), MUG(jduma), MIN(NULL),  NAM(jduma), TXT(jduma),
   "Dumars", "Joe",        WGH(8),   TPT(15)	// 4 Joe Dumars  G 6-3 195 5/24/63 McNeese State '85 13 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Golden State Warriors
const struct stat_plyr_data spd_gol[] = {
 { SPE(14),    SHO(17),    DUN(3),     PAS(12),     DRI(14),
   BLO(4),     STE(11),     POW(9),    CLU(9),     DRO(9),     HOT(12),
   TAL(6, 5),  NUM(9),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.15),  HLM(jstar), MUG(jstar), MIN(NULL),  NAM(jstar), TXT(jstar),
   "Starks", "John",       WGH(7),   TPT(15)	// 9 John Starks  G 6-5 185 8/10/65 Oklahoma State '88 9 
 },
 { SPE(12),     SHO(11),     DUN(9),     PAS(12),     DRI(3),
   BLO(16),     STE(7),     POW(15),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 9),  NUM(3),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.10),   HLM(dmars), MUG(dmars), MIN(NULL),  NAM(dmars), TXT(dmars),
   "Marshall", "Donyell",  WGH(10),   TPT(7)	// 3 Donyell Marshall  F 6-9 230 5/18/73 Connecticut '94 4 
 },
 { SPE(8),     SHO(9),     DUN(9),     PAS(3),     DRI(3),
   BLO(17),     STE(2),     POW(16),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 11), NUM(25),    POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.14),   HLM(edamp), MUG(edamp), MIN(NULL),  NAM(edamp), TXT(edamp),
   "Dampier", "Erick",     WGH(16),   TPT(2)	// 25 Erick Dampier  C 6-11 265 7/14/74 Mississippi State '96 2 
 },
 { SPE(14),     SHO(14),    DUN(1),     PAS(14),     DRI(13),
   BLO(5),     STE(9),     POW(4),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 2),  NUM(12),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.07),   HLM(bcole), MUG(bcole), MIN(NULL),  NAM(bcole), TXT(bcole),
   "Coles", "Bimbo",       WGH(7),   TPT(12)	// 12 Bimbo Coles  G 6-2 182 4/22/68 Virginia Tech '90 8 
 },
 { SPE(15),    SHO(12),    DUN(1),     PAS(13),     DRI(15),
   BLO(2),     STE(17),     POW(3),    CLU(9),     DRO(9),     HOT(17),
   TAL(5, 6),  NUM(1),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.05),  HLM(mbogu), MUG(mbogu), MIN(NULL),  NAM(mbogu), TXT(mbogu),
   "Bogues", "Muggsy",       WGH(2),   TPT(12)	// 1 Muggsy Bogues ???
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Houston Rockets
const struct stat_plyr_data spd_hou[] = {
 { SPE(15),     SHO(16),     DUN(18),     PAS(11),     DRI(11),
   BLO(11),     STE(16),     POW(14),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 7),  NUM(33),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(spipp), MUG(spipp), MIN(NULL),  NAM(spipp), TXT(spipp),
   "Pippen", "Scottie",    WGH(11),   TPT(12)	// 33 Scottie Pippen  F 6-7 228 9/25/65 Central Arkansas '87 11 
 },
 { SPE(12),     SHO(16),     DUN(4),     PAS(14),     DRI(4),
   BLO(7),     STE(11),     POW(19),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 6),  NUM(4),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.185),   HLM(cbark), MUG(cbark), MIN(NULL),  NAM(cbark), TXT(cbark),
   "Barkley", "Charles",   WGH(16),   TPT(15)	// 4 Charles Barkley  F 6-6 252 2/20/63 Auburn '84 14 
 },
 { SPE(11),     SHO(14),     DUN(13),     PAS(12),     DRI(2),
   BLO(17),     STE(10),     POW(18),    CLU(9),     DRO(9),     HOT(1),
   TAL(7, 0),  NUM(34),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.145),   HLM(holaj), MUG(holaj), MIN(NULL),  NAM(holaj), TXT(holaj),
   "Olajuwon", "Hakeem",   WGH(14),   TPT(3)	// 34 Hakeem Olajuwon  C 7-0 255 1/21/63 Houston '84 14 
 },
 { SPE(9),     SHO(15),     DUN(6),     PAS(4),     DRI(4),
   BLO(3),     STE(3),     POW(4),    CLU(19),     DRO(9),     HOT(10),
   TAL(6, 7),  NUM(8),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.08),   HLM(ejoh1), MUG(ejoh1), MIN(NULL),  NAM(ejoh1), TXT(ejoh1),
   "Johnson", "Eddie",     WGH(7),   TPT(18)	// 8 Eddie Johnson  G 6-7 215 5/1/59 Illinois '81 16 
 },
 { SPE(13),     SHO(16),     DUN(1),     PAS(12),     DRI(15),
   BLO(1),     STE(12),     POW(4),    CLU(9),     DRO(9),     HOT(12),
   TAL(6, 3),  NUM(12),    POS(GRD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.08),   HLM(mmalo), MUG(mmalo), MIN(NULL),  NAM(mmalo), TXT(mmalo),
   "Maloney", "Matt",      WGH(9),   TPT(14)	// 12 Matt Maloney  G 6-3 200 12/6/71 Pennsylvania '95 2 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Indianapolis Pacers
const struct stat_plyr_data spd_ind[] = {
 { SPE(13),     SHO(16),    DUN(10),   PAS(9),     DRI(10),
   BLO(2),     STE(8),     POW(8),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 7),  NUM(31),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(0.98),  HLM(rmill), MUG(rmill), MIN(NULL),  NAM(rmill), TXT(rmill),
   "Miller", "Reggie",     WGH(7),   TPT(15)	// 31 Reggie Miller  G 6-7 185 8/24/65 UCLA '87 11 
 },
 { SPE(10),     SHO(10),     DUN(15),     PAS(3),     DRI(5),
   BLO(16),     STE(4),     POW(17),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 11), NUM(32),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),  HLM(ddavi), MUG(ddavi), MIN(NULL),  NAM(ddavi), TXT(ddavi),
   "Davis", "Dale",        WGH(12),   TPT(5)	// 32 Dale Davis  F 6-11 230 3/25/69 Clemson '91 7 
 },
 { SPE(14),     SHO(15),     DUN(3),     PAS(12),     DRI(16),
   BLO(4),     STE(12),     POW(8),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 8),  NUM(5),     POS(GRD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.13),   HLM(jrose), MUG(jrose), MIN(NULL),  NAM(jrose), TXT(jrose),
   "Rose", "Jalen",        WGH(9),   TPT(12)	// 5 Jalen Rose  G 6-8 210 1/30/73 Michigan '94 4 
 },
 { SPE(9),     SHO(13),     DUN(9),     PAS(7),     DRI(6),
   BLO(11),    STE(6),     POW(18),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 9),  NUM(14),    POS(FWD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.18),   HLM(sperk), MUG(sperk), MIN(NULL),  NAM(sperk), TXT(sperk),
   "Perkins", "Sam",       WGH(13),   TPT(15)	// 14 Sam Perkins  F 6-9 260 6/14/61 North Carolina '84 14 
 },
 { SPE(13),     SHO(15),     DUN(1),     PAS(12),     DRI(15),
   BLO(1),     STE(15),     POW(6),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 3),  NUM(13),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),   HLM(mjack), MUG(mjack), MIN(NULL),  NAM(mjack), TXT(mjack),
   "Jackson", "Mark",      WGH(8),   TPT(14)	// 13 Mark Jackson  G 6-3 185 4/1/65 St. John's (N.Y.) '87 11 
 },
 { SPE(10),     SHO(17),    DUN(3),     PAS(15),     DRI(3),
   BLO(3),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(14),
   TAL(6, 7),  NUM(17),    POS(FWD),   FLG(WHT|LEFT|BIG),     SOU(UNUSED),
   HSC(1.10),   HLM(cmull), MUG(cmull), MIN(NULL),  NAM(cmull), TXT(cmull),
   "Mullin", "Chris",      WGH(7),   TPT(15)	// 17 Chris Mullin  F 6-7 215 7/30/63 St. John's (N.Y.) '85 13 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Los Angeles Clippers
const struct stat_plyr_data spd_lac[] = {
 { SPE(12),     SHO(13),     DUN(9),     PAS(11),     DRI(9),
   BLO(11),     STE(9),     POW(15),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 9),  NUM(23),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.18),   HLM(mtayl), MUG(mtayl), MIN(NULL),  NAM(mtayl), TXT(mtayl),
   "Taylor", "Maurice",    WGH(17),   TPT(8)	// 23 Maurice Taylor  F 6-9 260 10/30/76 Michigan '97 1 
 },
 { SPE(16),     SHO(9),     DUN(3),     PAS(15),     DRI(14),
   BLO(4),     STE(14),     POW(13),    CLU(9),     DRO(9),     HOT(1),
   TAL(5, 11), NUM(15),    POS(GRD),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.08),  HLM(dmart), MUG(dmart), MIN(NULL),  NAM(dmart), TXT(dmart),
   "Martin", "Darrick",    WGH(6),   TPT(12)	// 15 Darrick Martin  G 5-11 170 3/6/71 UCLA '92 4 
 },
 { SPE(7),     SHO(13),     DUN(11),     PAS(9),     DRI(5),
   BLO(15),     STE(9),     POW(17),    CLU(9),     DRO(9),     HOT(17),
   TAL(7, 2),  NUM(11),    POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.16),   HLM(svran), MUG(svran), MIN(NULL),  NAM(svran), TXT(svran),
   "Vrankovic", "Stojko",  WGH(17),   TPT(7)	// 11 Stojko Vrankovic  C 7-2 270 1/22/64 Croatia 4 
 },
 { SPE(12),     SHO(9),     DUN(15),     PAS(11),     DRI(9),
   BLO(9),     STE(4),     POW(13),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 7),  NUM(7),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.15), HLM(lmurr), MUG(lmurr), MIN(NULL),  NAM(lmurr), TXT(lmurr),
   "Murray", "Lamond",     WGH(11),   TPT(14)	//  Lamond Murray  F 6-7 236 4/20/73 California '94 4 
 },
 { SPE(12),     SHO(11),     DUN(9),     PAS(9),     DRI(12),
   BLO(9),     STE(3),     POW(16),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 7),  NUM(54),    POS(FWD),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.25),   HLM(rroge), MUG(rroge), MIN(NULL),  NAM(rroge), TXT(rroge),
   "Rogers", "Rodney",     WGH(16),   TPT(10)	// 54 Rodney Rogers  F 6-7 255 6/20/71 Wake Forest '93 5 
 },
};
 
// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Los Angeles Lakers
const struct stat_plyr_data spd_lal[] = {
 { SPE(10),     SHO(15),     DUN(18),     PAS(10),     DRI(10),
   BLO(18),     STE(2),     POW(19),    CLU(9),     DRO(9),     HOT(5),
   TAL(7, 1),  NUM(34),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.22), HLM(sonea), MUG(sonea), MIN(NULL),  NAM(sonea), TXT(sonea),
   "O'Neal", "Shaquille",  WGH(18),   TPT(2)	// 34 Shaquille O'Neal  C 7-1 315 3/6/72 Louisiana State '92 6 
 },
 { SPE(12),     SHO(18),     DUN(13),     PAS(9),     DRI(9),
   BLO(5),     STE(11),     POW(11),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 8),  NUM(41),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),  HLM(grice), MUG(grice), MIN(NULL),  NAM(grice), TXT(grice),
   "Rice", "Glen",         WGH(8),   TPT(17)	// 41 G 6-8 220 5/28/67 Michigan '89 9 
 },
 { SPE(17),     SHO(15),     DUN(17),     PAS(14),     DRI(17),
   BLO(5),     STE(14),     POW(3),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 7),  NUM(8),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.08),  HLM(kbrya), MUG(kbrya), MIN(NULL),  NAM(kbrya), TXT(kbrya),
   "Bryant", "Kobe",       WGH(7),   TPT(12)	// 8 Kobe Bryant  G 6-7 215 8/23/78 High School - Lower Merion (PA) 2  
 },
 { SPE(12),     SHO(12),     DUN(5),     PAS(13),     DRI(6),
   BLO(9),     STE(10),     POW(14),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 7),  NUM(17),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),  HLM(rifox), MUG(rfox),  MIN(NULL),  NAM(rfox),  TXT(rifox),
   "Fox", "Rick",          WGH(13),   TPT(13)	// 17 Rick Fox  F 6-7 242 7/24/69 North Carolina '91 7 
 },
 { SPE(9),     SHO(5),     DUN(11),     PAS(1),     DRI(1),
   BLO(16),     STE(6),     POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 0),  NUM(40),    POS(CTR),   FLG(WHT|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.15),   HLM(tknig), MUG(tknig), MIN(NULL),  NAM(tknig), TXT(tknig),
   "Knight", "Travis",     WGH(12),   TPT(4)	// 40 Travis Knight  C 7-0 235 9/13/74 Connecticut '96 2 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Miami Heat
const struct stat_plyr_data spd_mia[] = {
 { SPE(15),     SHO(13),     DUN(0),     PAS(5),     DRI(8),
   BLO(1),     STE(10),     POW(9),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 0),  NUM(10),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(thard), MUG(thard), MIN(NULL),  NAM(thard), TXT(thard),
   "Hardaway", "Tim",      WGH(8),   TPT(14)	// 10 Tim Hardaway  G 6-0 195 9/1/66 Texas-El Paso '89 9 
 },
 { SPE(12),     SHO(14),     DUN(7),     PAS(3),     DRI(2),
   BLO(9),     STE(12),     POW(15),    CLU(4),     DRO(9),     HOT(16),
   TAL(6, 10), NUM(33),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(amour), MUG(amour), MIN(NULL),  NAM(amour), TXT(amour),
   "Mourning", "Alonzo",   WGH(14),   TPT(1)	// 33 Alonzo Mourning  C 6-10 261 2/8/70 Georgetown '92 6 
 },
 { SPE(14),     SHO(14),     DUN(5),     PAS(4),     DRI(5),
   BLO(3),     STE(6),     POW(4),    CLU(4),     DRO(9),     HOT(14),
   TAL(6, 4),  NUM(21),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.12),   HLM(vlena), MUG(vlena), MIN(NULL),  NAM(vlena), TXT(vlena),
   "Lenard", "Voshon",     WGH(7),   TPT(13)	// 21 Voshon Lenard  G 6-4 205 5/14/73 Minnesota '95 3 
 },
 { SPE(10),     SHO(12),     DUN(1),     PAS(8),     DRI(10),
   BLO(1),     STE(15),     POW(3),    CLU(2),     DRO(9),     HOT(1),
   TAL(6, 3),  NUM(30),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.13),   HLM(tport), MUG(tport), MIN(NULL),  NAM(tport), TXT(tport),
   "Porter", "Terry",      WGH(7),   TPT(12)	// 30 Terry Porter  G 6-3 195 4/8/63 Wis.-Stevens Point '85 13 
 },
 { SPE(9),     SHO(9),     DUN(13),     PAS(12),     DRI(5),
   BLO(10),     STE(1),    POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 11), NUM(42),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.18),   HLM(pbrow), MUG(pbrow), MIN(NULL),  NAM(pbrow), TXT(pbrow),
   "Brown", "P.J.",        WGH(13),   TPT(5)	// 42 P.J. Brown  F 6-11 240 10/14/69 Louisiana Tech '92 5 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Milwaukee Bucks
const struct stat_plyr_data spd_mil[] = {
 { SPE(13),     SHO(15),     DUN(9),     PAS(6),     DRI(8),
   BLO(7),     STE(4),     POW(13),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 7),  NUM(13),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.22),  HLM(grobi), MUG(grobi), MIN(NULL),  NAM(grobi), TXT(grobi),
   "Robinson", "Glenn",    WGH(12),   TPT(15)	// 13 Glenn Robinson  F 6-7 230 1/10/73 Purdue '94 4 
 },
 { SPE(15),     SHO(13),     DUN(5),     PAS(12),     DRI(13),
   BLO(1),     STE(9),     POW(11),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 5),  NUM(34),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.20),  HLM(ralle), MUG(ralle), MIN(NULL),  NAM(ralle), TXT(ralle),
   "Allen", "Ray",         WGH(8),   TPT(12)	// 34 Ray Allen  G 6-5 205 7/20/75 Connecticut '96 2
 },
 { SPE(9),     SHO(7),     DUN(11),     PAS(4),     DRI(2),
   BLO(14),     STE(4),     POW(14),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 11), NUM(40),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.25),   HLM(ejoh3), MUG(ejoh3), MIN(NULL),  NAM(ejoh3), TXT(ejoh3),
   "Johnson", "Ervin",     WGH(13),   TPT(2)	// 40 Ervin Johnson  C 6-11 245 12/21/67 New Orleans '93 5 
 },
 { SPE(17),     SHO(16),     DUN(1),     PAS(13),     DRI(12),
   BLO(1),     STE(11),     POW(2),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 3),  NUM(10),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.2),  HLM(scass), MUG(scass), MIN(NULL),  NAM(scass), TXT(scass),
   "Cassel", "Sam",        WGH(7),   TPT(12)	// 10 Sam Cassell  G 6-3 185 11/18/69 Florida State '93 5 
 },
 { SPE(11),     SHO(11),     DUN(13),     PAS(4),     DRI(2),
   BLO(7),     STE(6),     POW(13),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 10), NUM(25),    POS(FWD),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.02),  HLM(cgatl), MUG(cgatl), MIN(NULL),  NAM(cgatl), TXT(cgatl),
   "Gatling", "Chris",     WGH(12),   TPT(8)	// 15 Chris Gatling  F 6-10 230 9/3/67 Old Dominion '91 7 
 },
 { SPE(8),     SHO(12),    DUN(12),    PAS(5),     DRI(2),
   BLO(8),     STE(11),    POW(15),    CLU(9),     DRO(9),    HOT(9),
   TAL(6, 10), NUM(1),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.08),   HLM(tthom), MUG(tthom), MIN(NULL),  NAM(tthom), TXT(tthom),
   "Thomas", "Tim", 	   WGH(11),   TPT(10)
 },
 { SPE(11),     SHO(12),     DUN(2),     PAS(8),     DRI(6),
   BLO(9),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 5),  NUM(30),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.09),   HLM(dcurr), MUG(dcurr), MIN(NULL),  NAM(dcurr), TXT(dcurr),
   "Curry", "Dell",        WGH(8),   TPT(17)	// 30 Dell Curry  G 6-5 205 6/25/64 Virginia Tech '86 12 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Minnesota Timberwolves
const struct stat_plyr_data spd_min[] = {
 { SPE(15),     SHO(12),     DUN(17),     PAS(6),     DRI(5),
   BLO(15),     STE(4),     POW(13),    CLU(9),     DRO(9),     HOT(2),
   TAL(7, 0), NUM(21),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.20),   HLM(kgarn), MUG(kgarn), MIN(NULL),  NAM(kgarn), TXT(kgarn),
   "Garnett", "Kevin",     WGH(7),   TPT(7)	// 21 Kevin Garnett  F 6-11 220 5/19/76 High School - Farragut Academy (IL) 3 
 },
 { SPE(17),     SHO(12),     DUN(1),     PAS(8),     DRI(14),
   BLO(1),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(16),
   TAL(5, 11), NUM(7),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.17),   HLM(tbran), MUG(tbran), MIN(NULL),  NAM(tbran), TXT(tbran),
   "Brandon", "Terrell",   WGH(6),   TPT(10)	//  Terrell Brandon  G 5-11 173 5/20/70 Oregon '91 7 
 },
 { SPE(6),     SHO(7),     DUN(14),    PAS(5),     DRI(2),
   BLO(15),     STE(5),     POW(13),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 11), NUM(22),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.18),   HLM(dgarr), MUG(dgarr), MIN(NULL),  NAM(dgarr), TXT(dgarr),
   "Garret", "Dean",       WGH(14),   TPT(3)	// 22 Dean Garrett  C 6-11 250 11/27/66 Indiana '88 2 
 },
 { SPE(14),     SHO(8),     DUN(16),     PAS(6),     DRI(12),
   BLO(10),     STE(9),     POW(8),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 10), NUM(32),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.26),   HLM(jsmit), MUG(jsmit), MIN(NULL),  NAM(jsmit), TXT(jsmit),
   "Smith", "Joe",         WGH(8),   TPT(9)	// 32 Joe Smith  F 6-10 225 7/26/75 Maryland '95 3 
 },
 { SPE(15),     SHO(10),     DUN(1),     PAS(9),     DRI(15),
   BLO(1),     STE(14),     POW(2),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 1),  NUM(24),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.015),   HLM(bjack), MUG(bjack), MIN(NULL),  NAM(bjack), TXT(bjack),
   "Jackson", "Bobby",     WGH(6),   TPT(8)	// 24 Bobby Jackson  G 6-1 185 3/13/73 Minnesota '97 1 
 },
 { SPE(13),     SHO(12),     DUN(2),     PAS(8),     DRI(3),
   BLO(6),     STE(9),     POW(4),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 8),  NUM(2),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.09),   HLM(mseal), MUG(mseal), MIN(NULL),  NAM(mseal), TXT(mseal),
   "Sealy", "Malik",       WGH(8),   TPT(10)	// 14 Malik Sealy  G 6-8 200 2/1/70 St. John's (N.Y.) '92 5 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// New Jersey Nets
const struct stat_plyr_data spd_njn[] = {
 { SPE(17),     SHO(13),     DUN(1),     PAS(13),     DRI(18),
   BLO(1),     STE(7),     POW(1),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 2),  NUM(33),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.15),   HLM(smarb), MUG(smarb), MIN(NULL),  NAM(smarb), TXT(smarb),
   "Marbury", "Stephon",   WGH(8),   TPT(9)	// 3 Stephon Marbury  G 6-2 180 2/20/77 Georgia Tech '96 2 
 },
 { SPE(14),     SHO(14),     DUN(12),     PAS(11),     DRI(9),
   BLO(13),     STE(8),     POW(15),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 10), NUM(44),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(khorn), MUG(kvanh), MIN(NULL),  NAM(kvanh), TXT(khorn),
   "Van Horn", "Keith",    WGH(13),   TPT(7)	// 44 Keith Van Horn  F 6-10 250 10/23/75 Utah '97 1 
 },
 { SPE(12),     SHO(9),     DUN(13),     PAS(7),     DRI(4),
   BLO(12),     STE(9),     POW(16),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 10), NUM(55),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.04),   HLM(jwill), MUG(jwill), MIN(NULL),  NAM(jwill), TXT(jwill),
   "Williams", "Jayson",   WGH(14),   TPT(6)	// 55 Jayson Williams  F 6-10 245 2/22/68 St. John's (N.Y.) '90 8 
 },
 { SPE(13),     SHO(15),     DUN(8),     PAS(13),     DRI(12),
   BLO(2),     STE(12),     POW(4),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 5),  NUM(13),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.04),   HLM(kgill), MUG(kgill), MIN(NULL),  NAM(kgill), TXT(kgill),
   "Gill", "Kendall",      WGH(9),   TPT(12)	// 13 Kendall Gill  F 6-5 216 5/25/68 Illinois '90 8 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// New York Knicks
const struct stat_plyr_data spd_nyk[] = {
 { SPE(12),     SHO(15),    DUN(14),     PAS(7),     DRI(3),
   BLO(15),     STE(8),     POW(16),    CLU(9),     DRO(9),     HOT(15),
   TAL(7, 1),  NUM(33),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.04),  HLM(pewin), MUG(pewin), MIN(NULL),  NAM(pewin), TXT(pewin),
   "Ewing", "Patrick",     WGH(16),   TPT(6)	// 33 Patrick Ewing  C 7-0 255 8/5/62 Georgetown '85 13 
 },
 { SPE(14),     SHO(17),     DUN(5),     PAS(7),     DRI(13),
   BLO(2),     STE(11),     POW(7),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 6),  NUM(20),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(0.985),   HLM(ahous), MUG(ahous), MIN(NULL),  NAM(ahous), TXT(ahous),
   "Houston", "Allan",     WGH(8),   TPT(17)	// 20 Allan Houston  G 6-6 200 4/20/71 Tennessee '93 5 
 },
 { SPE(15),     SHO(17),     DUN(13),     PAS(3),     DRI(9),
   BLO(8),     STE(9),     POW(8),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 5),  NUM(8),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(lspre), MUG(lspre), MIN(NULL),  NAM(lspre), TXT(lspre),
   "Sprewell", "Latrell",  WGH(8),   TPT(14)	// 8 Latrell Sprewell (s) G 6-5 190 9/8/70 Alabama '92 6 
 },
 { SPE(13),     SHO(13),     DUN(10),     PAS(3),     DRI(9),
   BLO(12),     STE(9),     POW(16),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 7),  NUM(2),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.05),  HLM(ljohn), MUG(ljohn), MIN(NULL),  NAM(ljohn), TXT(ljohn),
   "Johnson", "Larry",     WGH(13),   TPT(9)	// 2 Larry Johnson  F 6-7 235 3/14/69 Nevada-Las Vegas '91 7 
 },
 { SPE(15),     SHO(10),     DUN(1),     PAS(13),     DRI(15),
   BLO(1),     STE(7),     POW(3),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 2),  NUM(21),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(0.985),  HLM(cward), MUG(cward), MIN(NULL),  NAM(cward), TXT(cward),
   "Ward", "Charlie",      WGH(6),   TPT(15)	// 21 Charlie Ward  G 6-2 190 10/12/70 Florida State '94 4 
 },
 { SPE(11),     SHO(8),     DUN(14),     PAS(4),     DRI(2),
   BLO(13),     STE(2),     POW(8),    CLU(9),     DRO(9),     HOT(1),
   TAL(7, 0), NUM(23),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.0),  HLM(mcamb), MUG(mcamb), MIN(NULL),  NAM(mcamb), TXT(mcamb),
   "Camby", "Marcus",      WGH(8),   TPT(5)	// 21 Marcus Camby  F 6-11 220 3/22/74 Massachusetts '96 1 
 },
#if 0		/* waived */
 { SPE(13),     SHO(12),     DUN(1),     PAS(4),     DRI(4),
   BLO(1),     STE(6),     POW(14),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 8),  NUM(3),     POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),   HLM(dscot), MUG(dscot), MIN(NULL),  NAM(dscot), TXT(dscot),
   "Scott", "Dennis",      WGH(10),   TPT(16)	// 4 Dennis Scott  F 6-8 229 9/5/68 Georgia Tech '90 8 
 },
#endif
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Orlando Magic
const struct stat_plyr_data spd_orl[] = {
 { SPE(16),     SHO(17),    DUN(14),    PAS(12),    DRI(16),
   BLO(5),     STE(8),     POW(8),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 7),  NUM(1),     POS(GRD),   FLG(BLK|RIGHT|NORMAL), SOU(UNUSED),
   HSC(1.25),   HLM(ahard), MUG(ahard), MIN(NULL),  NAM(ahard), TXT(ahard),
   "Hardaway", "Anfernee", WGH(8),   TPT(9)	// 1 Anfernee Hardaway  G 6-7 215 7/18/71 Memphis '93 5 
 },
 { SPE(12),     SHO(13),     DUN(12),    PAS(13),    DRI(9),
   BLO(5),    STE(17),    POW(14),     CLU(8),     DRO(9),     HOT(10),
   TAL(6, 6),  NUM(25),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.25),   HLM(nande), MUG(nande), MIN(NULL),  NAM(nande), TXT(nande),
   "Anderson", "Nick",     WGH(11),   TPT(17)	// 25 Nick Anderson  F 6-6 228 1/20/68 Illinois '89 9 
 },
 { SPE(10),     SHO(18),    DUN(18),    PAS(6),     DRI(13),
   BLO(11),    STE(15),    POW(15),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 10), NUM(54),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.25),   HLM(hgran), MUG(hgran), MIN(NULL),  NAM(hgran), TXT(hgran),
   "Grant", "Horace",      WGH(13),   TPT(8)	// 54 Horace Grant  F 6-10 245 7/4/65 Clemson '87 11 
 },
 { SPE(13),     SHO(15),    DUN(14),    PAS(4),     DRI(11),
   BLO(9),     STE(2),     POW(17),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 8),  NUM(45),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.20),   HLM(coutl), MUG(coutl), MIN(NULL),  NAM(coutl), TXT(coutl),
   "Outlaw", "Charles",    WGH(8),   TPT(4)	// 45 Charles Outlaw  F 6-8 210 4/13/71 Houston '93 5 
 },
 { SPE(9),     SHO(11),     DUN(6),    PAS(8),    DRI(6),
   BLO(17),    STE(9),    POW(17),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 11), NUM(24),    POS(CTR),   FLG(WHT|RIGHT|NORMAL), SOU(UNUSED),
   HSC(1.24),   HLM(dscha), MUG(dscha), MIN(NULL),  NAM(dscha), TXT(dscha),
   "Schayes", "Danny",     WGH(15),   TPT(4)	// 24 Danny Schayes  C 6-11 260 5/10/59 Syracuse '81 17 
 },
 { SPE(14),     SHO(12),     DUN(1),     PAS(12),     DRI(15),
   BLO(1),     STE(12),     POW(3),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 2),  NUM(2),    POS(FWD),   FLG(BLK|BIG|LEFT),     SOU(UNUSED),
   HSC(1.22),   HLM(bjarm), MUG(barms), MIN(NULL),  NAM(barms), TXT(bjarm),
   "Armstrong", "B.J.",    WGH(5),   TPT(17)	// 2 G 6-2 185 9/9/67 Iowa '89 9 
 },
 { SPE(14),     SHO(15),     DUN(10),     PAS(11),     DRI(14),
   BLO(3),     STE(7),     POW(8),    CLU(9),     DRO(9),     HOT(8),
   TAL(6, 1),  NUM(10),    POS(FWD),   FLG(BLK|BIG|RIGHT),     SOU(UNUSED),
   HSC(1.13),   HLM(darms), MUG(darms), MIN(NULL),  NAM(barms), TXT(darms),
   "Armstrong", "Darrel",    WGH(5),   TPT(9)
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Philadelphia 76ers
const struct stat_plyr_data spd_phi[] = {
 { SPE(18),    SHO(14),    DUN(3),    PAS(5),    DRI(18),
   BLO(2),     STE(7),     POW(3),     CLU(9),     DRO(9),    HOT(13),
   TAL(6, 0),  NUM(3),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.21),  HLM(aiver), MUG(aiver), MIN(NULL),  NAM(aiver), TXT(aiver),
   "Iverson", "Allen",     WGH(5),   TPT(13)	// 3 Allen Iverson  G 6-0 165 6/7/75 Georgetown '96 2 
 },
 { SPE(13),    SHO(12),    DUN(12),    PAS(2),     DRI(2),
   BLO(18),    STE(8),     POW(14),    CLU(9),     DRO(9),    HOT(1),
   TAL(6, 10), NUM(42),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.22),   HLM(tratl), MUG(tratl), MIN(NULL),  NAM(tratl), TXT(tratl),
   "Ratliff", "Theo",      WGH(8),   TPT(3)	// 42 Theo Ratliff  F 6-10 225 4/17/73 Wyoming '95 3 
 },
 { SPE(12),    SHO(12),    DUN(2),     PAS(9),     DRI(8),
   BLO(1),     STE(8),     POW(10),     CLU(9),     DRO(9),    HOT(17),
   TAL(6, 5),  NUM(8),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.17),   HLM(amcki), MUG(amcki), MIN(NULL),  NAM(amcki), TXT(amcki),
   "McKie", "Aaron",       WGH(8),   TPT(9)	//  Aaron McKie  G 6-5 209 10/2/72 Temple '94 4 
 },
 { SPE(11),     SHO(9),     DUN(13),    PAS(3),     DRI(5),
   BLO(14),    STE(6),     POW(15),    CLU(9),     DRO(9),    HOT(9),
   TAL(7, 1),  NUM(52),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.17),   HLM(mgeig), MUG(mgeig), MIN(NULL),  NAM(mgeig), TXT(mgeig),
   "Geiger", "Matt", 	   WGH(14),   TPT(4)	// 52 Matt Geiger  C 7-1 248 9/10/69 Georgia Tech '92 6 
 },
 { SPE(10),     SHO(6),     DUN(5),     PAS(2),     DRI(2),
   BLO(8),     STE(6),     POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 9),  NUM(40),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.28),   HLM(thill), MUG(thill), MIN(NULL),  NAM(thill), TXT(thill),
   "Hill", "Tyrone",       WGH(12),   TPT(6)	// 42 Tyrone Hill  C 6-9 250 3/19/68 Xavier (Ohio) '90 8 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Phoenix Suns
const struct stat_plyr_data spd_pho[] = {
 { SPE(17),     SHO(11),    DUN(8),    PAS(18),     DRI(18),
   BLO(4),    STE(12),     POW(8),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 4),  NUM(32),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.125),  HLM(jkidd), MUG(jkidd), MIN(NULL),  NAM(jkidd), TXT(jkidd),
   "Kidd", "Jason",        WGH(8),   TPT(9)	// 32 Jason Kidd  G 6-4 212 3/23/73 California '94 4 
 },
 { SPE(12),     SHO(15),    DUN(12),     PAS(14),    DRI(8),
   BLO(13),     STE(7),    POW(15),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 10), NUM(24),    POS(FWD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.21),   HLM(tgugi), MUG(tgugl), MIN(NULL),  NAM(tgugl), TXT(tgugi),
   "Gugliotta", "Tom",     WGH(13),   TPT(9)	// 24 Tom Gugliotta  F 6-10 240 12/19/69 North Carolina State '92 6 
 },
 { SPE(13),     SHO(14),    DUN(5),     PAS(8),     DRI(13),
   BLO(5),    STE(6),     POW(11),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 4),  NUM(3),     POS(GRD),   FLG(WHT|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.128),  HLM(rchap), MUG(rchap), MIN(NULL),  NAM(rchap), TXT(rchap),
   "Chapman", "Rex",       WGH(6),   TPT(18)	// 3 Rex Chapman  G 6-4 195 10/5/67 Kentucky '88 10 
 },
 { SPE(9),     SHO(12),    DUN(15),     PAS(2),     DRI(6),
   BLO(12),    STE(5),    POW(14),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 10), NUM(15),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.14),   HLM(dmann), MUG(dmann), MIN(NULL),  NAM(dmann), TXT(dmann),
   "Manning", "Danny",     WGH(12),   TPT(11)	// 15 Danny Manning  F 6-10 244 5/17/66 Kansas '88 10 
 },
 { SPE(5),     SHO(13),    DUN(10),    PAS(8),     DRI(4),
   BLO(12),     STE(8),    POW(17),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 2),  NUM(13),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.1),   HLM(llong), MUG(llong), MIN(NULL),  NAM(llong), TXT(llong),
   "Longley", "Luc",       WGH(16),   TPT(6)	// 13 Luc Longley  C 7-2 260 1/19/69 New Mexico '91 7 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Portland Trailblazers
const struct stat_plyr_data spd_por[] = {
 { SPE(17),     SHO(17),    DUN(1),     PAS(14),     DRI(18),
   BLO(1),    STE(15),    POW(1),    CLU(9),     DRO(9),     HOT(12),
   TAL(5, 10), NUM(3),     POS(GRD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.083),  HLM(dstou), MUG(dstou), MIN(NULL),  NAM(dstou), TXT(dstou),
   "Stoudamire", "Damon",  WGH(5),   TPT(8)	// 3 Damon Stoudamire  G 5-10 171 9/3/73 Arizona '95 3 
 },
 { SPE(12),     SHO(15),    DUN(13),     PAS(7),     DRI(2),
   BLO(14),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 11), NUM(30),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.18),   HLM(rwall), MUG(rwall), MIN(NULL),  NAM(rwall), TXT(rwall),
   "Wallace", "Rasheed",   WGH(11),   TPT(3)	// 30 Rasheed Wallace  C 6-11 225 9/17/74 North Carolina '95 3 	
 },
 { SPE(7),     SHO(16),    DUN(12),     PAS(11),    DRI(3),
   BLO(18),     STE(7),     POW(18),    CLU(9),     DRO(9),     HOT(4),
   TAL(7, 3),  NUM(11),    POS(CTR),   FLG(WHT|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.18),   HLM(asabo), MUG(asabo), MIN(NULL),  NAM(asabo), TXT(asabo),
   "Sabonis", "Arvydas",   WGH(18),   TPT(4)	// 11 Arvydas Sabonis  C 7-3 292 12/19/64 Lithuania 3 
 },
 { SPE(12),     SHO(11),    DUN(5),    PAS(10),     DRI(10),
   BLO(7),    STE(15),    POW(7),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 5),  NUM(34),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.18),   HLM(iride), MUG(iride), MIN(NULL),  NAM(iride), TXT(iride),
   "Rider", "Isaiah",      WGH(10),   TPT(14)	// 34 Isaiah Rider  G 6-5 215 3/12/71 Nevada-Las Vegas '93 5 
 },
 { SPE(14),     SHO(12),     DUN(3),     PAS(15),     DRI(15),
   BLO(4),     STE(14),     POW(4),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 6),  NUM(19),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.165),   HLM(jjack), MUG(jjack), MIN(NULL),  NAM(jjack), TXT(jjack),
   "Jackson", "Jim",       WGH(8),   TPT(17)	// 22 Jim Jackson # G 6-6 220 10/14/70 Ohio State '92 6 
 },
 { SPE(14),     SHO(17),    DUN(14),     PAS(15),    DRI(15),
   BLO(8),    STE(12),    POW(6),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 8),  NUM(42),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.18),   HLM(wwill), MUG(wwill), MIN(NULL),  NAM(wwill), TXT(wwill),
   "Williams", "Walt",     WGH(10),   TPT(11)	// 42 Walt Williams  G 6-8 230 4/16/70 Maryland '92 6 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Sacramento Kings
const struct stat_plyr_data spd_sac[] = {
 { SPE(12),    SHO(11),    DUN(16),    PAS(15),    DRI(7),
   BLO(15),    STE(6),     POW(17),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 11), NUM(4),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.20),  HLM(cwebb), MUG(cwebb), MIN(NULL),  NAM(cwebb), TXT(cwebb),
   "Webber", "Chris",      WGH(14),   TPT(7)	// 4 Chris Webber  F 6-10 245 3/1/73 Michigan '93 5 
 },
 { SPE(16),    SHO(11),    DUN(6),    PAS(18),    DRI(17),
   BLO(1),    STE(13),     POW(3),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 1), NUM(55),     POS(FWD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.10),  HLM(jwil2), MUG(jwil2), MIN(NULL),  NAM(jwill), TXT(jwil2),
   "Williams", "Jason",    WGH(8),   TPT(10)	// 4 Jason Williams  G 6-1 190 1/18/75 Florida '98
 },
 { SPE(10),     SHO(10),     DUN(4),     PAS(3),     DRI(2),
   BLO(6),     STE(5),     POW(15),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 7),  NUM(34),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.19),   HLM(cwill), MUG(cwill), MIN(NULL),  NAM(cwill), TXT(cwill),
   "Williamson", "Corliss",WGH(15),   TPT(5)	// 34 Corliss Williamson  F 6-7 245 12/4/73 Arkansas '95 3 
 },
 { SPE(10),     SHO(9),     DUN(7),     PAS(12),     DRI(2),
   BLO(12),     STE(4),     POW(14),    CLU(9),     DRO(9),     HOT(2),
   TAL(7, 1),  NUM(21),    POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.16),  HLM(vdiva), MUG(vdiva), MIN(NULL),  NAM(vdiva), TXT(vdiva),
   "Divac", "Vlade",       WGH(15),   TPT(8)	// 21 Vlade Divac  C 7-1 260 2/3/68 Serbia 9 
 },
 { SPE(10),     SHO(7),     DUN(7),     PAS(11),    DRI(2),
   BLO(9),     STE(4),     POW(13),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 9),  NUM(51),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.2),   HLM(lfund), MUG(lfund), MIN(NULL),  NAM(lfund), TXT(lfund),
   "Funderburke", "L.",    WGH(12),   TPT(6)	// 51 Lawrence Funderburke  F 6-9 230 12/15/70 Ohio State '94 1 
 },
// FIX!!!!!
// { SPE(14),    SHO(11),    DUN(4),     PAS(15),    DRI(11),
//   BLO(3),     STE(6),     POW(2),    CLU(9),     DRO(9),     HOT(17),
//   TAL(6, 4),  NUM(24),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
//   HSC(1.17),   HLM(tdehe), MUG(tdehe), MIN(NULL),  NAM(tdehe), TXT(tdehe),
//   "Dehere", "Terry",      WGH(6),   TPT(9)	// 24 Terry Dehere  G 6-4 190 9/12/71 Seton Hall '93 5 
// },

// NOT TO BE IN GAME!!!
// { SPE(5),     SHO(9),     DUN(9),     PAS(9),     DRI(9),
//   BLO(9),     STE(9),     POW(16),    CLU(9),     DRO(9),     HOT(11),
//   TAL(6, 9),  NUM(8),     POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
//   HSC(1.07),  HLM(omill), MUG(omill), MIN(NULL),  NAM(omill), TXT(omill),
//   "Miller", "Oliver",     WGH(10),   TPT(9)
// },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// San Antonio Spurs
const struct stat_plyr_data spd_san[] = {
 { SPE(11),     SHO(15),     DUN(15),     PAS(8),     DRI(3),
   BLO(18),     STE(7),     POW(18),    CLU(9),     DRO(9),     HOT(14),
   TAL(7,2),   NUM(50),    POS(CTR),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.22),   HLM(drobi), MUG(drobi), MIN(NULL),  NAM(drobi), TXT(drobi),
   "Robinson", "David",    WGH(15),   TPT(4)	// 50 David Robinson  C 7-1 250 8/6/65 Navy '87 9 
 },
 { SPE(14),     SHO(12),     DUN(16),     PAS(12),     DRI(10),
   BLO(8),     STE(7),     POW(12),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 8),  NUM(32),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.18),   HLM(selli), MUG(selli), MIN(NULL),  NAM(selli), TXT(selli),
   "Elliot", "Sean",       WGH(9),   TPT(14)	// 32 Sean Elliott  F 6-8 220 2/2/68 Arizona '89 9 
 },
 { SPE(12),     SHO(15),     DUN(15),     PAS(5),     DRI(2),
   BLO(18),     STE(9),     POW(16),    CLU(9),     DRO(9),     HOT(3),
   TAL(7, 1),  NUM(21),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.2),  HLM(tdunc), MUG(tdunc), MIN(NULL),  NAM(tdunc), TXT(tdunc),
   "Duncan", "Tim",        WGH(15),   TPT(3)	// 21 Tim Duncan  C 7-0 248 4/25/76 Wake Forest '97 1 
 },
 { SPE(7),     SHO(5),     DUN(9),     PAS(6),     DRI(2),
   BLO(15),     STE(3),     POW(15),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 0),  NUM(41),    POS(CTR),   FLG(WHT|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.22),   HLM(wperd), MUG(wperd), MIN(NULL),  NAM(wperd), TXT(wperd),
   "Perdue", "Will",       WGH(15),   TPT(4)	// 41 Will Perdue  C 7-0 240 8/29/65 Vanderbilt '88 10 
 },
 { SPE(11),     SHO(16),     DUN(1),     PAS(7),     DRI(6),
   BLO(3),     STE(6),     POW(2),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 3),  NUM(4),     POS(GRD),   FLG(WHT|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.1),  HLM(skerr), MUG(skerr), MIN(NULL),  NAM(skerr), TXT(skerr),
   "Kerr", "Steve",        WGH(6),   TPT(17)	// 4 Steve Kerr  G 6-3 181 9/27/65 Arizona '88 10 
 },
 { SPE(16),     SHO(7),     DUN(1),     PAS(11),     DRI(14),
   BLO(1),     STE(9),     POW(1),    CLU(9),     DRO(9),     HOT(17),
   TAL(5, 11), NUM(6),     POS(GRD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.15),   HLM(ajohn), MUG(ajohn), MIN(NULL),  NAM(ajohn), TXT(ajohn),
   "Johnson", "Avery",     WGH(8),   TPT(9)	// 6 Avery Johnson  G 5-11 180 3/25/65 Southern University '88 10 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Seattle Supersonics
const struct stat_plyr_data spd_sea[] = {
 { SPE(18),     SHO(13),     DUN(3),     PAS(12),     DRI(18),
   BLO(5),     STE(18),     POW(10),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 4),  NUM(20),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.08),   HLM(gpayt), MUG(gpayt), MIN(NULL),  NAM(gpayt), TXT(gpayt),
   "Payton", "Gary",       WGH(6),   TPT(15)	// 20 Gary Payton  G 6-4 180 7/23/68 Oregon State '90 8 
 },
 { SPE(10),     SHO(15),     DUN(12),     PAS(3),     DRI(4),
   BLO(9),     STE(3),     POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 11), NUM(42),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.26),  HLM(vbake), MUG(vbake), MIN(NULL),  NAM(vbake), TXT(vbake),
   "Baker", "Vin",         WGH(14),   TPT(5)	// 42 Vin Baker  F 6-11 250 11/23/71 Hartford '93 5 
 },
 { SPE(9),     SHO(11),     DUN(7),     PAS(5),     DRI(4),
   BLO(8),     STE(7),     POW(13),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 10), NUM(11),    POS(FWD),   FLG(WHT|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.23),   HLM(dschr), MUG(dschr), MIN(NULL),  NAM(dschr), TXT(dschr),
   "Schrempf", "Detlef",   WGH(9),   TPT(7)	// 11 Detlef Schrempf  F 6-10 235 1/21/63 Washington '85 13 
 },
 { SPE(9),     SHO(16),     DUN(1),     PAS(2),     DRI(2),
   BLO(1),     STE(5),     POW(3),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 7),  NUM(3),     POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.27),   HLM(delli), MUG(delli), MIN(NULL),  NAM(delli), TXT(delli),
   "Ellis", "Dale",        WGH(8),   TPT(18)	// 3 Dale Ellis  G 6-7 215 8/6/60 Tennessee '83 15 
 },
 { SPE(9),     SHO(9),     DUN(7),     PAS(6),     DRI(4),
   BLO(6),     STE(2),     POW(8),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 9),  NUM(30),    POS(FWD),   FLG(BLK|LEFT|BIG),     SOU(UNUSED),
   HSC(1.16),   HLM(bowen), MUG(bowen), MIN(NULL),  NAM(bowen), TXT(bowen),
   "Owens", "Billy",       WGH(10),   TPT(7)	// 30 Billy Owens  F 6-9 225 5/1/69 Syracuse '91 7 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Toronto Raptors
const struct stat_plyr_data spd_tor[] = {
 { SPE(17),     SHO(15),     DUN(19),     PAS(13),     DRI(14),
   BLO(3),     STE(14),     POW(5),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 7),  NUM(15),     POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.2),   HLM(vcart), MUG(vcart), MIN(NULL),  NAM(vcart), TXT(vcart),
   "Carter", "Vince",	       WGH(9),   TPT(11)
 },
 { SPE(10),     SHO(14),     DUN(7),     PAS(7),     DRI(9),
   BLO(12),     STE(9),     POW(18),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 9),  NUM(34),    POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.10),  HLM(coakl), MUG(coakl), MIN(NULL),  NAM(coakl), TXT(coakl),
   "Oakley", "Charles",    WGH(15),   TPT(5)	// 34 Charles Oakley  F 6-9 245 12/18/63 Virginia Union '85 13 
 },
 { SPE(11),     SHO(10),     DUN(7),     PAS(12),     DRI(6),
   BLO(3),     STE(7),     POW(6),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 6),  NUM(13),    POS(GRD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.28),  HLM(dchri), MUG(dchri), MIN(NULL),  NAM(dchri), TXT(dchri),
   "Christie", "Doug",     WGH(9),   TPT(12)	// 13 Doug Christie  G 6-6 205 5/9/70 Pepperdine '92 6 
 },
 { SPE(14),     SHO(12),     DUN(12),     PAS(6),     DRI(11),
   BLO(7),     STE(12),     POW(6),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 8),  NUM(1),     POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.05),   HLM(tmcgr), MUG(tmcgr), MIN(NULL),  NAM(tmcgr), TXT(tmcgr),
   "McGrady", "Tracy",     WGH(9),   TPT(11)	   // Tracy McGrady  F 6-8 210 5/24/79 High School - Mount Zion Christian Academy 1
 },
 { SPE(10),     SHO(11),     DUN(6),     PAS(3),     DRI(2),
   BLO(14),     STE(2),     POW(17),    CLU(9),     DRO(9),     HOT(3),
   TAL(7, 1),  NUM(42),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.11),   HLM(kwill), MUG(kwill), MIN(NULL),  NAM(kwill), TXT(kwill),
   "Willis", "Kevin",      WGH(16),   TPT(6)	// 42 Kevin Willis  F 7-0 245 9/6/62 Michigan State '84 14 
 },
 { SPE(14),     SHO(13),     DUN(1),     PAS(13),     DRI(14),
   BLO(1),     STE(13),     POW(1),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 2),  NUM(7),     POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(dbrow), MUG(dbrow), MIN(NULL),  NAM(dbrow), TXT(dbrow),
   "Brown", "Dee",	       WGH(6),   TPT(17)	// 7 Dee Brown  G 6-2 185 11/29/68 Jacksonville '90 8 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Utah Jazz
const struct stat_plyr_data spd_uta[] = {
 { SPE(14),     SHO(13),     DUN(2),     PAS(17),     DRI(14),
   BLO(1),     STE(15),     POW(2),    CLU(9),     DRO(9),     HOT(8),
   TAL(6, 1),  NUM(12),    POS(GRD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.120),   HLM(jstoc), MUG(jstoc), MIN(NULL),  NAM(jstoc), TXT(jstoc),
   "Stockton", "John",     WGH(7),   TPT(15)	// 12 John Stockton  G 6-1 175 3/26/62 Gonzaga '84 14 
 },
 { SPE(12),     SHO(14),     DUN(14),     PAS(7),     DRI(2),
   BLO(14),     STE(3),     POW(18),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 9),  NUM(32),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.04),  HLM(kmalo), MUG(kmalo), MIN(NULL),  NAM(kmalo), TXT(kmalo),
   "Malone", "Karl",       WGH(15),   TPT(3)	// 32 Karl Malone  F 6-9 256 7/24/63 Louisiana Tech '85 13 
 },
 { SPE(7),     SHO(5),     DUN(12),     PAS(4),     DRI(2),
   BLO(17),     STE(3),     POW(18),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 2),  NUM(0),     POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.2),  HLM(goste), MUG(goste), MIN(NULL),  NAM(goste), TXT(goste),
   "Ostertag", "Greg",     WGH(17),   TPT(3)	// 0 Greg Ostertag  C 7-2 280 3/6/73 Kansas '95 3 
 },
 { SPE(9),     SHO(15),     DUN(2),     PAS(8),     DRI(3),
   BLO(3),     STE(12),     POW(5),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 4),  NUM(14),    POS(GRD),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.18),   HLM(jhorn), MUG(jhorn), MIN(NULL),  NAM(jhorn), TXT(jhorn),
   "Hornacek", "Jeff",     WGH(8),   TPT(12)	// 14 Jeff Hornacek  G 6-4 190 5/3/63 Iowa State '86 12 
 },
 { SPE(14),     SHO(6),     DUN(9),     PAS(6),     DRI(8),
   BLO(7),     STE(7),     POW(9),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 7),  NUM(3),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.25),   HLM(bruss), MUG(bruss), MIN(NULL),  NAM(bruss), TXT(bruss),
   "Russell", "Byron",     WGH(9),   TPT(8)	// 3 Bryon Russell  F 6-7 225 12/31/70 Long Beach State '93 5 
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Vancouver Grizzlies
const struct stat_plyr_data spd_van[] = {
 { SPE(15),     SHO(13),     DUN(10),     PAS(14),     DRI(14),
   BLO(8),     STE(9),     POW(12),    CLU(9),     DRO(9),     HOT(15),
   TAL(6, 9),  NUM(3),     POS(FWD),   FLG(BLK|BIG|RIGHT),              SOU(UNUSED),
   HSC(1.1),   HLM(sabdu), MUG(sabdu), MIN(NULL),  NAM(sabdu), TXT(sabdu),
   "Abdur-Rahim", "Shareef", WGH(9), TPT(11)	// 3 Shareef Abdur-Rahim  F 6-9 230 12/11/76 California '96 2 
 },
 { SPE(8),     SHO(13),     DUN(5),     PAS(6),     DRI(2),
   BLO(13),     STE(4),     POW(18),    CLU(9),     DRO(9),     HOT(9),
   TAL(7, 1),  NUM(50),    POS(CTR),   FLG(WHT|BIG|RIGHT),              SOU(UNUSED),
   HSC(1.25),  HLM(breev), MUG(breev), MIN(NULL),  NAM(breev), TXT(breev),
   "Reeves", "Bryant",     WGH(17),   TPT(4)	// 50 Bryant Reeves  C 7-0 275 6/8/73 Oklahoma State '95 3 
 },
 { SPE(11),     SHO(7),     DUN(5),     PAS(12),     DRI(1),
   BLO(9),     STE(6),     POW(15),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 9),  NUM(44),    POS(FWD),   FLG(BLK|BIG|RIGHT),              SOU(UNUSED),
   HSC(1.15),  HLM(tmass), MUG(tmass), MIN(NULL),  NAM(tmass), TXT(tmass),
   "Massenburg", "Tony",   WGH(14),   TPT(6)	// 44 Tony Massenburg  F 6-9 250 7/13/67 Maryland '90 6 
 },
 { SPE(10),     SHO(8),     DUN(4),     PAS(7),     DRI(2),
   BLO(12),     STE(2),     POW(15),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 11), NUM(1),     POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.2),   HLM(cpark), MUG(cpark), MIN(NULL),  NAM(cpark), TXT(cpark),
   "Parks", "Cherokee",    WGH(12),   TPT(5)	// 1 Cherokee Parks  F 6-11 240 10/11/72 Duke '95 3 
 },
 { SPE(14),     SHO(12),     DUN(5),     PAS(12),     DRI(13),
   BLO(5),     STE(14),     POW(10),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 6),  NUM(2),     POS(GRD),   FLG(BLK|BIG|RIGHT),              SOU(UNUSED),
   HSC(1.0),   HLM(dwest), MUG(dwest), MIN(NULL),  NAM(dwest), TXT(dwest),
   "West", "Doug",         WGH(10),   TPT(10)	// 2 Doug West  G 6-6 220 5/27/67 Villanova '89 9  
 },
 { SPE(16),     SHO(13),     DUN(4),     PAS(14),     DRI(14),
   BLO(2),     STE(14),     POW(5),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 2),  NUM(10),     POS(GRD),   FLG(BLK|BIG|RIGHT),              SOU(UNUSED),
   HSC(1.1),   HLM(mbibb), MUG(mbibb), MIN(NULL),  NAM(mbibb), TXT(mbibb),
   "Bibby", "Mike",         WGH(6),   TPT(12)
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound

// Washington Wizards
const struct stat_plyr_data spd_was[] = {
 { SPE(10),    SHO(12),    DUN(14),    PAS(6),     DRI(2),
   BLO(13),    STE(8),     POW(15),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 10),  NUM(5),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.33),   HLM(jhowa), MUG(jhowa), MIN(NULL),  NAM(jhowa), TXT(jhowa),
   "Howard", "Juwan",      WGH(14),   TPT(6)	// 5 Juwan Howard  F 6-9 240 2/7/73 Michigan '94 4 
 },
 { SPE(18),    SHO(14),    DUN(4),     PAS(16),    DRI(18),
   BLO(1),     STE(15),    POW(3),     CLU(6),     DRO(9),    HOT(6),
   TAL(6, 3),  NUM(1),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.05),   HLM(rstri), MUG(rstri), MIN(NULL),  NAM(rstri), TXT(rstri),
   "Strickland", "Rod",    WGH(6),   TPT(15)	// 1 Rod Strickland # G 6-3 185 7/11/66 DePaul '88 10 
 },
 { SPE(14),    SHO(14),    DUN(12),    PAS(3),     DRI(11),
   BLO(2),     STE(12),    POW(3),     CLU(6),     DRO(9),     HOT(15),
   TAL(6, 7),  NUM(40),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.2),   HLM(cchea), MUG(cchea), MIN(NULL),  NAM(cchea), TXT(cchea),
   "Cheaney", "Calbert",   WGH(9),   TPT(8)	// 40 Calbert Cheaney  G 6-7 215 7/17/71 Indiana '93 5 
 },
 { SPE(12),    SHO(17),    DUN(7),     PAS(12),     DRI(11),
   BLO(3),     STE(12),    POW(10),    CLU(12),     DRO(9),     HOT(10),
   TAL(6, 5),  NUM(2),     POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.25),  HLM(mrich), MUG(mrich), MIN(NULL),   NAM(mrich), TXT(mrich),
   "Richmond", "Mitch",    WGH(14),   TPT(15)	// 2 Mitch Richmond  G 6-5 220 6/30/65 Kansas State '88 10 
 },
 { SPE(10),    SHO(12),    DUN(11),    PAS(3),     DRI(0),
   BLO(9),     STE(4),     POW(15),    CLU(3),     DRO(9),     HOT(17),
   TAL(6, 7),  NUM(35),    POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.05),  HLM(tmurr), MUG(tmurr), MIN(NULL),  NAM(tmurr), TXT(tmurr),
   "Murray", "Tracy",  WGH(11),   TPT(17)
 },
};

// speed, shot, dunk, pass, dribble,
// block, steal, power, clutch, drone, hotspot,
// tall, number, position, flags, sound,
// szheadtx, p_headlm, f_headsc, pii_mug, pii_mini, pii_name,
// szlast, szfirst, weight, rebound
const struct stat_plyr_data secret_team[] = {
/* CHA_MASCOT */
 { SPE(17),     SHO(14),     DUN(15),     PAS(9),     DRI(11),
   BLO(11),     STE(14),     POW(12),    CLU(10),     DRO(9),     HOT(1),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(horn), MUG(chugo), MIN(NULL), NAM(hugoh), TXT(horn),
   "Hugo", "",    WGH(13),   TPT(13)
 },
/* CHI_MASCOT */
 { SPE(14),     SHO(12),     DUN(15),     PAS(10),     DRI(11),
   BLO(12),     STE(11),     POW(17),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(benny), MUG(benny), MIN(NULL), NAM(benny), TXT(benny),
   "Benny", "",    WGH(13),   TPT(12)
 },
/* IND_MASCOT */
 { SPE(16),     SHO(16),     DUN(14),     PAS(9),     DRI(10),
   BLO(13),     STE(10),     POW(13),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.2),   HLM(boome), MUG(boomr), MIN(NULL), NAM(boome), TXT(boome),
   "Boomer", "",    WGH(13),   TPT(15)
 },
/* MIN_MASCOT */
 { SPE(16),     SHO(13),     DUN(17),     PAS(10),     DRI(10),
   BLO(15),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(crunc), MUG(crunc), MIN(NULL), NAM(crunc), TXT(crunc),
   "Crunch", "",    WGH(13),   TPT(13)
 },
/* TOR_MASCOT */
 { SPE(12),     SHO(12),     DUN(14),     PAS(12),     DRI(11),
   BLO(15),     STE(11),     POW(17),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(raptr), MUG(rapto), MIN(NULL), NAM(thera), TXT(raptr),
   "The Raptor", "",    WGH(13),   TPT(15)
 },
/* HOU_MASCOT */
 { SPE(15),     SHO(16),     DUN(18),     PAS(11),     DRI(11),
   BLO(11),     STE(16),     POW(14),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(hbear), MUG(hbear), MIN(NULL), NAM(turbo), TXT(hbear),
   "Turbo", "",    WGH(13),   TPT(12)
 },
/* ATL_MASCOT */
 { SPE(15),     SHO(16),     DUN(15),     PAS(14),     DRI(12),
   BLO(10),     STE(13),     POW(10),    CLU(18),     DRO(9),     HOT(7),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(hawk), MUG(ahawk), MIN(NULL), NAM(hawk), TXT(hawk),
   "Hawk", "",    WGH(13),   TPT(14)
 },
/* PHO_MASCOT */
 { SPE(16),     SHO(11),     DUN(17),     PAS(18),     DRI(18),
   BLO(16),     STE(12),     POW(16),    CLU(9),     DRO(9),     HOT(8),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(goril), MUG(goril), MIN(NULL), NAM(thego), TXT(goril),
   "The Gorilla", "",    WGH(13),   TPT(11)
 },
/* NJN_MASCOT */
 { SPE(17),     SHO(13),     DUN(14),     PAS(13),     DRI(18),
   BLO(12),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 6),  NUM(1),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(sly), MUG(njsly), MIN(NULL), NAM(slyth), TXT(sly),
   "Sly", "",    WGH(13),   TPT(12)
 },
/* DEN_MASCOT */
 { SPE(15),     SHO(14),     DUN(16),     PAS(9),     DRI(9),
   BLO(11),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(10),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(rocky), MUG(rocky), MIN(NULL), NAM(rocky), TXT(rocky),
   "Rocky", "",    WGH(13),   TPT(12)
 },
/* SEA_MASCOT */
 { SPE(12),     SHO(13),     DUN(16),     PAS(12),     DRI(14),
   BLO(9),     STE(11),     POW(18),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(sasqu), MUG(sasqu), MIN(NULL), NAM(sasqu), TXT(sasqu),
   "Sasquatch", "",    WGH(13),   TPT(12)
 },
/* UTA_MASCOT */
 { SPE(13),     SHO(14),     DUN(13),     PAS(14),     DRI(12),
   BLO(15),     STE(13),     POW(16),    CLU(9),     DRO(9),     HOT(12),
   TAL(6, 6),  NUM(0),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(ubear), MUG(ubear), MIN(NULL), NAM(bear), TXT(ubear),
   "Bear", "",    WGH(13),   TPT(11)
 },
/* ALIEN_CHARACTER */
 { SPE(18),     SHO(10),     DUN(6),     PAS(12),     DRI(17),
   BLO(7),     STE(15),     POW(8),    CLU(10),     DRO(9),     HOT(13),
   TAL(5, 5),  NUM(56),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.2),   HLM(alian), MUG(alien), MIN(NULL), NAM(small), TXT(alian),
   "Smalls", "",    WGH(12),   TPT(15)
 },
/* JAM_CHARACTER */
 { SPE(12),     SHO(10),     DUN(18),     PAS(10),     DRI(7),
   BLO(12),     STE(8),     POW(19),    CLU(5),     DRO(9),     HOT(14),
   TAL(7, 7),  NUM(58),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.2),   HLM(jam), MUG(jalie), MIN(NULL), NAM(biggy), TXT(jam),
   "Biggy", "",    WGH(17),   TPT(8)
 },
/* NIKKO_CHARACTER */
 { SPE(17),     SHO(10),     DUN(15),     PAS(10),     DRI(5),
   BLO(12),     STE(15),     POW(10),    CLU(7),     DRO(9),     HOT(15),
   TAL(6, 6),  NUM(66),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(nikko), MUG(nikko), MIN(NULL), NAM(nikko), TXT(nikko),
   "Nikko", "",    WGH(13),   TPT(12)
 },
/* LIA_CHARACTER */
 { SPE(14),     SHO(14),     DUN(16),     PAS(12),     DRI(14),
   BLO(6),     STE(16),     POW(9),    CLU(16),     DRO(9),     HOT(16),
   TAL(5, 8),  NUM(12),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(0.95),   HLM(femal), MUG(lmont), MIN(NULL), NAM(liamo), TXT(femal),
   "Montelongo", "Lia",    WGH(10),   TPT(14)
 },
/* KERRI_CHARACTER */
 { SPE(14),     SHO(14),     DUN(16),     PAS(12),     DRI(14),
   BLO(6),     STE(16),     POW(9),    CLU(16),     DRO(9),     HOT(17),
   TAL(5, 8),  NUM(20),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.00),  HLM(khosk), MUG(khosk), MIN(NULL), NAM(kerri), TXT(khosk),
   "Hoskins", "Kerri",    WGH(10),   TPT(14)
 },
/* RETRO_CHARACTER */
 { SPE(15),     SHO(17),     DUN(15),     PAS(17),     DRI(17),
   BLO(9),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 6),  NUM(70),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(retro), MUG(retro), MIN(NULL), NAM(retro), TXT(retro),
   "Retro Rob", "",    WGH(13),   TPT(15)
 },
/* OLDMAN_CHARACTER */
 { SPE(10),     SHO(14),     DUN(5),     PAS(12),     DRI(12),
   BLO(16),     STE(12),     POW(8),    CLU(18),     DRO(9),     HOT(2),
   TAL(6, 6),  NUM(1),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.10),   HLM(oman), MUG(grand), MIN(NULL), NAM(grand), TXT(oman),
   "Grandpa", "",    WGH(13),   TPT(13)
 },
/* CLOWN_CHARACTER */
 { SPE(17),     SHO(12),     DUN(16),     PAS(7),     DRI(6),
   BLO(12),     STE(11),     POW(13),    CLU(14),     DRO(9),     HOT(3),
   TAL(6, 6),  NUM(84),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(clown), MUG(clown), MIN(NULL), NAM(crisp), TXT(clown),
   "Crispy", "",    WGH(13),   TPT(10)
 },
/* PUMPKIN_CHARACTER */
 { SPE(13),     SHO(13),     DUN(13),     PAS(13),     DRI(13),
   BLO(13),     STE(13),     POW(13),    CLU(13),     DRO(9),     HOT(4),
   TAL(6, 6),  NUM(31),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(pumpk), MUG(jacko), MIN(NULL), NAM(jacko), TXT(pumpk),
   "Jacko", "",    WGH(13),   TPT(13)
 },
/* HORSE_CHARACTER */
 { SPE(19),     SHO(11),     DUN(16),     PAS(10),     DRI(6),
   BLO(16),     STE(12),     POW(17),    CLU(16),     DRO(9),     HOT(5),
   TAL(6, 6),  NUM(66),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.10),   HLM(horse), MUG(horse), MIN(NULL), NAM(horse), TXT(horse),
   "Horse", "",    WGH(12),   TPT(9)
 },
/* WIZARD_CHARACTER */
 { SPE(13),     SHO(14),     DUN(13),     PAS(12),     DRI(13),
   BLO(16),     STE(8),     POW(15),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 6),  NUM(36),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(wizrd), MUG(wizar), MIN(NULL), NAM(thewi), TXT(wizrd),
   "The Wiz", "",    WGH(13),   TPT(12)
 },
/* FRANK_CHARACTER */
 { SPE(12),     SHO(12),     DUN(16),     PAS(10),     DRI(14),
   BLO(17),     STE(10),     POW(18),    CLU(10),     DRO(9),     HOT(7),
   TAL(6, 6),  NUM(31),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.05),   HLM(frank), MUG(frank), MIN(NULL), NAM(frank), TXT(frank),
   "Frankenstein", "",    WGH(14),   TPT(12)
 },
/* MUMMY_CHARACTER */
 { SPE(13),     SHO(12),     DUN(16),     PAS(10),     DRI(13),
   BLO(14),     STE(10),     POW(16),    CLU(10),     DRO(9),     HOT(8),
   TAL(6, 6),  NUM(32),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.05),   HLM(mummy), MUG(mummy), MIN(NULL), NAM(themu), TXT(mummy),
   "The Mummy", "",    WGH(13),   TPT(11)
 },
/* BRIDE_CHARACTER */
 { SPE(14),     SHO(14),     DUN(16),     PAS(12),     DRI(14),
   BLO(6),     STE(12),     POW(14),    CLU(16),     DRO(9),     HOT(9),
   TAL(6, 6),  NUM(35),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(bride), MUG(bride), MIN(NULL), NAM(thebr), TXT(bride),
   "The Bride", "",    WGH(12),   TPT(12)
 },
/* WOLFMAN_CHARACTER */
 { SPE(14),     SHO(10),     DUN(16),     PAS(10),     DRI(8),
   BLO(14),     STE(12),     POW(16),    CLU(10),     DRO(9),     HOT(10),
   TAL(6, 6),  NUM(41),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(wolfm), MUG(wolfm), MIN(NULL), NAM(wolfm), TXT(wolfm),
   "The Wolfman", "",    WGH(13),   TPT(13)
 },
/* REF_CHARACTER */
 { SPE(14),     SHO(14),     DUN(9),     PAS(10),     DRI(12),
   BLO(16),     STE(13),     POW(10),    CLU(9),     DRO(9),     HOT(11),
   TAL(5, 6),  NUM(77),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(creff), MUG(cmedn), MIN(NULL), NAM(there), TXT(creff),
   "The Ref", "",    WGH(8),   TPT(15)
 },
/* CHEERLEADER_LM_CHARACTER */
 { SPE(15),     SHO(14),     DUN(12),     PAS(12),     DRI(14),
   BLO(6),     STE(12),     POW(8),    CLU(16),     DRO(9),     HOT(12),
   TAL(5, 8),  NUM(12),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(0.95),   HLM(femal), MUG(lmont), MIN(NULL), NAM(liamo), TXT(femal),
   "Montelongo", "Lia",    WGH(10),   TPT(13)
 },
/* CHEERLEADER_KH_CHARACTER */
 { SPE(15),     SHO(14),     DUN(12),     PAS(12),     DRI(14),
   BLO(6),     STE(12),     POW(8),    CLU(16),     DRO(9),     HOT(13),
   TAL(5, 8),  NUM(22),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(0.95),   HLM(khosk), MUG(khosk), MIN(NULL), NAM(kerri), TXT(khosk),
   "Hoskins", "Kerri",    WGH(10),   TPT(13)
 },
/* CHEERLEADER_JH_CHARACTER */
 { SPE(16),     SHO(14),     DUN(12),     PAS(10),     DRI(13),
   BLO(13),     STE(13),     POW(15),    CLU(9),     DRO(9),     HOT(14),
   TAL(5, 6),  NUM(22),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(jhedr), MUG(jhedr), MIN(NULL), NAM(jhedr), TXT(jhedr),
   "Hedrick", "Jennifer",    WGH(7),   TPT(16)
 },
/* PINTO_CHARACTER */
 { SPE(18),     SHO(11),     DUN(16),     PAS(10),     DRI(13),
   BLO(16),     STE(12),     POW(17),    CLU(16),     DRO(9),     HOT(15),
   TAL(6, 6),  NUM(66),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.10),   HLM(horse), MUG(hrse2), MIN(NULL), NAM(hrse2), TXT(horse),
   "Pinto", "",    WGH(12),   TPT(12)
 },
/* CREATURE_CHARACTER */
 { SPE(12),     SHO(12),     DUN(16),     PAS(10),     DRI(14),
   BLO(17),     STE(10),     POW(18),    CLU(10),     DRO(9),     HOT(7),
   TAL(6, 6),  NUM(54),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(creat), MUG(creature), MIN(NULL), NAM(creat), TXT(creat),
   "The Creature", "",    WGH(14),   TPT(12)
 },
/* NBA_PLAYER */
 { SPE(9),     SHO(9),     DUN(9),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(16),
   TAL(6, 7),  NUM(22),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(mturm), MUG(mturm), MIN(NULL), NAM(mturm), TXT(mturm),
   "PLACEHOLDER", "RECORD",    WGH(14),   TPT(9)
 },
/* MARK_T_CHARACTER */
 { SPE(17),     SHO(9),     DUN(15),     PAS(9),     DRI(19),
   BLO(9),     STE(13),     POW(12),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 7),  NUM(22),    POS(GRD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(mturm), MUG(mturm), MIN(NULL), NAM(mturm), TXT(mturm),
   "Turmell", "Mark",    WGH(10),   TPT(16)
 },
/* ROB_G_CHARACTER */
 { SPE(15),     SHO(17),     DUN(15),     PAS(9),     DRI(19),
   BLO(10),      STE(15),     POW(8),    CLU(9),     DRO(9),     HOT(1),
   TAL(5, 11),  NUM(20),     POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(0.98),   HLM(rgats), MUG(rgats), MIN(NULL), NAM(rgats), TXT(rgats),
   "Gatson", "Rob",    WGH(6),   TPT(16)
 },
/* MARK_G_CHARACTER */
 { SPE(17),     SHO(15),     DUN(17),     PAS(11),     DRI(7),
   BLO(8),     STE(7),     POW(14),    CLU(16),     DRO(9),     HOT(2),
   TAL(6, 0),  NUM(7),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(mguid), MUG(mguid), MIN(NULL), NAM(mguid), TXT(mguid),
   "Guidarelli", "Mark",    WGH(9),   TPT(13)
 },
/* DAN_T_CHARACTER */
 { SPE(16),     SHO(15),     DUN(12),     PAS(9),     DRI(13),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 7),  NUM(63),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(dthom), MUG(dthom), MIN(NULL), NAM(dthom), TXT(dthom),
   "Thompson", "Daniel",    WGH(10),   TPT(12)
 },
/* JEFF_J_CHARACTER */
 { SPE(19),    SHO(7),    DUN(8),    PAS(12),    DRI(7),
   BLO(12),     STE(7),     POW(17),    CLU(12),     DRO(9),     HOT(4),
   TAL(6, 0),  NUM(69),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(jeffj), MUG(jjohn), MIN(NULL), NAM(jjohn), TXT(jeffj),
   "Johnson", "Jeff",    WGH(10),   TPT(15)
 },
/* SAL_D_CHARACTER */
 { SPE(17),     SHO(14),     DUN(7),     PAS(13),     DRI(13),
   BLO(9),     STE(12),     POW(15),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 0),  NUM(5),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(sdivi), MUG(sdivi), MIN(NULL), NAM(sdivi), TXT(sdivi),
   "Divita", "Sal",    WGH(10),   TPT(15)
 },
/* JENNIFER_H_CHARACTER */
 { SPE(16),     SHO(14),     DUN(12),     PAS(10),     DRI(13),
   BLO(13),     STE(13),     POW(15),    CLU(9),     DRO(9),     HOT(6),
   TAL(5, 6),  NUM(22),    POS(FWD),   FLG(WHT|BIG|RIGHT|FEMALE),   SOU(UNUSED),
   HSC(1.0),   HLM(jhedr), MUG(jhedr), MIN(NULL), NAM(jhedr), TXT(jhedr),
   "Hedrick", "Jennifer",    WGH(7),   TPT(16)
 },
/* EUGENE_G_CHARACTER */
 { SPE(15),     SHO(12),     DUN(12),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 0),  NUM(75),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.1),   HLM(egeer), MUG(egeer), MIN(NULL), NAM(egeer), TXT(egeer),
   "Geer", "Eugene",    WGH(10),   TPT(13)
 },
/* MATT_G_CHARACTER */
 { SPE(16),     SHO(9),     DUN(13),     PAS(9),     DRI(9),
   BLO(9),     STE(14),     POW(14),    CLU(9),     DRO(9),     HOT(8),
   TAL(5, 10),  NUM(12),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(mgilm), MUG(mgilm), MIN(NULL), NAM(mgilm), TXT(mgilm),
   "Gilmore", "Matt",    WGH(10),   TPT(17)
 },
/* TIM_B_CHARACTER */
 { SPE(15),     SHO(14),     DUN(9),     PAS(9),     DRI(14),
   BLO(9),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(9),
   TAL(6, 0),  NUM(77),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.09),   HLM(tbrya), MUG(tbrya), MIN(NULL), NAM(tbrya), TXT(tbrya),
   "Bryant", "Tim",    WGH(10),   TPT(12)
 },
/* JIM_G_CHARACTER */
 { SPE(14),     SHO(13),     DUN(12),     PAS(12),     DRI(12),
   BLO(1),     STE(10),     POW(12),    CLU(9),     DRO(9),     HOT(10),
   TAL(6, 0),  NUM(0),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(jgent), MUG(jgent), MIN(NULL), NAM(jgent), TXT(jgent),
   "Gentile", "Jim",    WGH(5),   TPT(17)
 },
/* JON_H_CHARACTER */
 { SPE(14),    SHO(14),     DUN(9),     PAS(14),     DRI(14),
   BLO(4),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(12),
   TAL(5, 11),  NUM(88),    POS(FWD),   FLG(WHT|NORMAL|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(jhey), MUG(jhey), MIN(NULL), NAM(jhey), TXT(jhey),
   "Hey", "Jon",    WGH(10),   TPT(14)
 },
/* SHAWN_L_CHARACTER */
 { SPE(13),     SHO(12),     DUN(1),     PAS(15),     DRI(13),
   BLO(4),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 1),  NUM(24),    POS(FWD),   FLG(WHT|NORMAL|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(slipt), MUG(slipt), MIN(NULL), NAM(slipt), TXT(slipt),
   "Liptak", "Shawn",    WGH(10),   TPT(13)
 },
/* ISAIAH_T_CHARACTER */
 { SPE(16),     SHO(13),     DUN(1),     PAS(14),     DRI(19),
   BLO(1),     STE(14),     POW(14),    CLU(9),     DRO(9),     HOT(14),
   TAL(6, 2),  NUM(11),    POS(FWD),   FLG(BLK|NORMAL|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(ithom), MUG(ithom), MIN(NULL), NAM(ithom), TXT(ithom),
   "Thomas", "Isiah",    WGH(11),   TPT(13)
 },
/* TIM_K_CHARACTER */
 { SPE(12),     SHO(12),     DUN(9),     PAS(9),     DRI(13),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(15),
   TAL(6,1),  NUM(59),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(tkizr), MUG(tkizr), MIN(NULL), NAM(tkizr), TXT(tkizr),
   "Kitzrow", "Tim",    WGH(10),   TPT(12)
 },
/* TIM_M_CHARACTER */
 { SPE(12),     SHO(12),     DUN(9),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(16),
   TAL(6,9),  NUM(13),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(tmorn), MUG(tmora), MIN(NULL), NAM(timco), TXT(tmorn),
   "TIMCORP", "",    WGH(10),   TPT(13)				/* "Moran", "Tim" */
 },
/* WILLY_M_CHARACTER */
 { SPE(17),     SHO(15),     DUN(19),     PAS(9),     DRI(13),
   BLO(3),     STE(6),     POW(12),    CLU(9),     DRO(9),     HOT(17),
   TAL(6, 4),  NUM(23),    POS(FWD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(wmorr), MUG(wmorr), MIN(NULL), NAM(wmorr), TXT(wmorr),
   "Morris", "Willie",    WGH(10),   TPT(12)
 },
/* CHAD_CHARACTER */
 { SPE(17),     SHO(15),     DUN(17),     PAS(14),     DRI(17),
   BLO(10),     STE(14),     POW(9),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 1),  NUM(24),    POS(GRD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(chad), MUG(cedmo), MIN(NULL), NAM(cedmo), TXT(chad),
   "Edmonds", "Chad",    WGH(6),   TPT(12)
 },
/* CHRIS_S_CHARACTER */
 { SPE(12),     SHO(13),     DUN(9),     PAS(9),     DRI(12),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 6),  NUM(52),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(cskru), MUG(cskru), MIN(NULL), NAM(cskru), TXT(cskru),
   "Skrundz", "Chris",    WGH(10),   TPT(13)
 },
/* BETH_S_CHARACTER */
 { SPE(13),     SHO(12),     DUN(1),     PAS(9),     DRI(9),
   BLO(1),     STE(9),     POW(7),    CLU(9),     DRO(9),     HOT(3),
   TAL(5,6),  NUM(17),    POS(FWD),   FLG(WHT|BIG|RIGHT|FEMALE),   SOU(UNUSED),
   HSC(1.0),   HLM(bsmuk), MUG(bsmuk), MIN(NULL), NAM(bsmuk), TXT(bsmuk),
   "Smukowski", "Beth",    WGH(9),   TPT(13)
 },
/* DAVE_G_CHARACTER */
 { SPE(15),     SHO(14),     DUN(18),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 9),  NUM(4),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(dgros), MUG(dgros), MIN(NULL), NAM(dgros), TXT(dgros),
   "Grossman", "Dave",    WGH(10),   TPT(14)
 },
/* JIM_T_CHARACTER */
 { SPE(13),     SHO(12),     DUN(12),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(5),
   TAL(6, 9),  NUM(26),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.1),   HLM(jtian), MUG(jtian), MIN(NULL), NAM(jtian), TXT(jtian),
   "Tianis", "Jim",    WGH(12),   TPT(12)
 },
/* LARRY_W_CHARACTER */
 { SPE(13),     SHO(12),     DUN(12),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(6),
   TAL(6, 9),  NUM(0),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(lwotm), MUG(mwotm), MIN(NULL), NAM(lwotm), TXT(lwotm),
   "Wotman", "Larry",    WGH(10),   TPT(12)
 },
/* MIKE_L_CHARACTER */
 { SPE(15),     SHO(14),     DUN(9),     PAS(9),     DRI(15),
   BLO(9),     STE(13),     POW(14),    CLU(9),     DRO(9),     HOT(7),
   TAL(6, 9),  NUM(23),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(mlync), MUG(mlync), MIN(NULL), NAM(mlync), TXT(mlync),
   "Lynch", "Mike",    WGH(11),   TPT(12)
 },
/* JASON_S_CHARACTER */
 { SPE(18),     SHO(7),     DUN(7),     PAS(9),     DRI(7),
   BLO(12),     STE(6),     POW(14),    CLU(12),     DRO(9),     HOT(8),
   TAL(6, 9),  NUM(77),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.08),   HLM(jskil), MUG(jskil), MIN(NULL), NAM(jskil), TXT(jskil),
   "Skiles", "Jason",    WGH(11),   TPT(13)
 },
/* PAULO_G_CHARACTER */
 { SPE(17),     SHO(7),     DUN(8),     PAS(12),     DRI(7),
   BLO(12),     STE(13),     POW(16),    CLU(12),     DRO(9),     HOT(9),
   TAL(6, 0),  NUM(45),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(pgarc), MUG(pgarc), MIN(NULL), NAM(pgarc), TXT(pgarc),
   "Garcia", "Paulo",    WGH(10),   TPT(16)
 },
/* BRIAN_L_CHARACTER */
 { SPE(17),     SHO(7),     DUN(8),     PAS(12),     DRI(7),
   BLO(12),     STE(13),     POW(16),    CLU(12),     DRO(9),     HOT(10),
   TAL(6, 0),  NUM(76),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.1),   HLM(bleba), MUG(bleba), MIN(NULL), NAM(bleba), TXT(bleba),
   "Lebaron", "Brian",    WGH(10),   TPT(16)
 },
/* LEX_CHARACTER */
 { SPE(17),     SHO(7),     DUN(17),     PAS(9),     DRI(9),
   BLO(6),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 9),  NUM(72),    POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(agill), MUG(agill), MIN(NULL), NAM(agill), TXT(agill),
   "Gilliam", "Alex",    WGH(12),   TPT(9)
 },
/* GREG_C_CHARACTER */
 { SPE(12),     SHO(9),     DUN(9),     PAS(9),     DRI(15),
   BLO(9),     STE(12),     POW(14),    CLU(9),     DRO(9),     HOT(12),
   TAL(6, 9),  NUM(0),    POS(FWD),   FLG(BLK|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(gcutl), MUG(gcutl), MIN(NULL), NAM(gcutl), TXT(gcutl),
   "Cutler", "Greg",    WGH(10),   TPT(11)
 },
/* JOHN_R_CHARACTER */
 { SPE(13),     SHO(12),     DUN(19),     PAS(9),     DRI(13),
   BLO(9),      STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(11),
   TAL(5, 11),  NUM(86),   POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.1),   HLM(jroot), MUG(jroot), MIN(NULL), NAM(jroot), TXT(jroot),
   "Root", "John",    WGH(10),   TPT(9)
 },
/* ANDY_E_CHARACTER */
 { SPE(16),     SHO(12),     DUN(10),     PAS(9),     DRI(5),
   BLO(6),      STE(7),     POW(16),    CLU(9),     DRO(9),     HOT(14),
   TAL(6, 0),  NUM(21),   POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.1),   HLM(aelof), MUG(aelof), MIN(NULL), NAM(aelof), TXT(aelof),
   "Eloff", "Andy",    WGH(9),   TPT(16)
 },
/* PAUL_M_CHARACTER */
 { SPE(15),     SHO(11),     DUN(10),     PAS(9),     DRI(12),
   BLO(10),      STE(7),     POW(15),    CLU(9),     DRO(9),     HOT(11),
   TAL(6, 2),  NUM(6),   POS(FWD),   FLG(WHT|BIG|RIGHT),   SOU(UNUSED),
   HSC(1.0),   HLM(pmart), MUG(pmart), MIN(NULL), NAM(pmart), TXT(pmart),
   "Martin", "Paul",    WGH(8),   TPT(16)
 },
/* RODMAN_CHARACTER */
 { SPE(14),     SHO(11),     DUN(11),     PAS(13),     DRI(7),
   BLO(14),     STE(11),     POW(18),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 6),  NUM(73),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.22),   HLM(drodb), MUG(drodm), MIN(NULL),  NAM(drodm), TXT(drodb),
   "Rodman", "Dennis",     WGH(13),   TPT(6)	// 91 F 6-6 220 5/13/61 Southeastern Oklahoma State '86 12 
 }
};

#if 0
 { SPE(10),     SHO(9),     DUN(2),     PAS(15),     DRI(9),
   BLO(3),     STE(14),     POW(4),    CLU(9),     DRO(9),     HOT(13),
   TAL(6, 6),  NUM(1),     POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(rrhod), MUG(rrhod), MIN(NULL),  NAM(rrhod), TXT(rrhod),
   "Rhodes", "Rodrick",    WGH(7),   TPT(14)	// 1 Rodrick Rhodes  G 6-6 225 9/24/73 USC '97 1 
 },
 { SPE(3),     SHO(14),     DUN(15),     PAS(3),     DRI(1),
  BLO(15),     STE(2),     POW(14),    CLU(9),     DRO(9),     HOT(14),
   TAL(7, 4),  NUM(45),    POS(CTR),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.0),   HLM(rsmit), MUG(rsmit), MIN(NULL),  NAM(rsmit), TXT(rsmit),
   "Smits", "Rik",         WGH(14),   TPT(9)	// 45 Rik Smits  C 7-4 265 8/23/66 Marist '88 10 
 },
 { SPE(6),     SHO(12),     DUN(5),     PAS(2),     DRI(1),
   BLO(17),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(15),
   TAL(7, 0),  NUM(34),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(molow), MUG(molow), MIN(NULL),  NAM(molow), TXT(molow),
   "Olowokandi", "Michael",WGH(11),   TPT(3)	// 34 Michael Olowokandi  C 7-0 269 4/3/75 U. of Pacific '98 R
 },
 { SPE(10),     SHO(10),     DUN(7),     PAS(4),     DRI(6),
   BLO(7),     STE(5),     POW(14),    CLU(7),     DRO(9),     HOT(16),
   TAL(6, 8),  NUM(24),    POS(GRD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(jmash), MUG(jmash), MIN(NULL),  NAM(jmash), TXT(jmash),
   "Mashburn", "Jamal",    WGH(6),   TPT(8)	// 24 Jamal Mashburn  F 6-8 250 11/29/72 Kentucky '93 5 
 },
 { SPE(8),     SHO(6),     DUN(14),     PAS(5),     DRI(1),
   BLO(17),     STE(11),     POW(13),    CLU(9),     DRO(9),     HOT(17),
   TAL(7, 1),  NUM(22),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(jmcil), MUG(jmcil), MIN(NULL),  NAM(jmcil), TXT(jmcil),
   "McIlvaine", "Jim",     WGH(6),   TPT(5)	// 22 Jim McIlvaine  C 7-1 264 7/30/72 Marquette '94 4 
 },
 { SPE(6),     SHO(16),    DUN(1),     PAS(9),     DRI(9),
   BLO(8),     STE(11),    POW(14),    CLU(9),     DRO(9),     HOT(1),
   TAL(6, 10), NUM(14),    POS(CTR),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(nmoha), MUG(nmoha), MIN(NULL),  NAM(nmoha), TXT(nmoha),
   "Mohammed", "Nazr", 	   WGH(14),   TPT(10)	// 14 Nazr Mohammed  C 6-10 240 9/5/77 Kentucky '98 R 
 },
/* NOT DONE, dbarr is Dana Barros */
 { SPE(5),     SHO(9),     DUN(9),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(16),    CLU(9),     DRO(9),     HOT(2),
   TAL(6, 5),  NUM(12),     POS(FWD),   FLG(BLK|RIGHT|BIG),    SOU(UNUSED),
   HSC(1.07),  HLM(dbar2), MUG(dbar2), MIN(NULL),  NAM(dbar2), TXT(dbar2),
   "Barry", "Drew",     WGH(8),   TPT(9)
 },
/* NO LONGER IN NBA */
 { SPE(2),     SHO(9),     DUN(9),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(14),    CLU(9),     DRO(9),     HOT(3),
   TAL(6, 5),  NUM(35),    POS(FWD),   FLG(BLK|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.0),   HLM(bwell), MUG(bwell), MIN(NULL),  NAM(bwell), TXT(bwell),
   "Wells", "Bubba",       WGH(9),   TPT(9)
 },
/* NO LONGER IN NBA */
 { SPE(9),     SHO(9),     DUN(9),     PAS(9),     DRI(9),
   BLO(9),     STE(9),     POW(13),    CLU(9),     DRO(9),     HOT(4),
   TAL(6, 0),  NUM(12),    POS(GRD),   FLG(BLK),              SOU(UNUSED),
   HSC(1.0),   HLM(bhurl), MUG(bhurl), MIN(NULL),  NAM(bhurl), TXT(bhurl),
   "Hurley", "Bobby",      WGH(8),   TPT(9)
 },
/* NO LONGER IN NBA */
 { SPE(1),     SHO(5),     DUN(5),     PAS(3),     DRI(0),
   BLO(9),     STE(4),     POW(15),    CLU(3),     DRO(9),     HOT(5),
   TAL(7, 7),  NUM(77),    POS(CTR),   FLG(WHT|BIG|RIGHT),    SOU(UNUSED),
   HSC(1.04),  HLM(gmure), MUG(gmure), MIN(NULL),  NAM(gmure), TXT(gmure),
   "Muresan", "Gheorghe",  WGH(15),   TPT(3)	// 77 Gheorghe Muresan # C 7-7 303 2/14/71 Romania 4 
 },
#endif

#undef SPE
#undef SHO
#undef DUN
#undef PAS
#undef DRI
#undef BLO
#undef STE
#undef POW
#undef CLU
#undef DRO
#undef HOT

#undef TAL
#undef NUM
#undef POS
#undef FLG
#undef SOU

#undef HSC
#undef HLM
#undef MUG
#undef MIN
#undef NAM
#undef TXT
#undef WGH
#undef TPT

#undef BLK
#undef WHT
#undef RIGHT
#undef LEFT
#undef BIG
#undef NORMAL

#undef GRD
#undef FWD
#undef CTR

#undef UNUSED


#define NP(array)		(sizeof(array) / sizeof(array[0]))
// Team data (in TEAM CITY alpha order)
#include "include/c_extern.h"
const struct stat_team_data teaminfo[] = {
//  szhome          szname          szabbr szprfx pii_logo   p_spd    court           coutr_tex           num_player   pii_logo2 pii_logo3 pii_cn
  { "Atlanta",      "Hawks",        "ATL", "ATL", &logo_atl, spd_atl, &limb_atlcourt, &atlcourt_textures, NP(spd_atl), &atllogo, &atllogo, &atlword},
  { "Boston",       "Celtics",      "BOS", "BOS", &logo_bos, spd_bos, &limb_boscourt, &boscourt_textures, NP(spd_bos), &boslogo, &boslogo, &bosword},
  { "Charlotte",    "Hornets",      "CHA", "CHA", &logo_cha, spd_cha, &limb_chacourt, &chacourt_textures, NP(spd_cha), &chalogo, &chalogo, &chaword},
  { "Chicago",      "Bulls",        "CHI", "CHI", &logo_chi, spd_chi, &limb_chicourt, &chicourt_textures, NP(spd_chi), &chilogo, &chilogo, &chiword},
  { "Cleveland",    "Cavaliers",    "CLE", "CLE", &logo_cle, spd_cle, &limb_clecourt, &clecourt_textures, NP(spd_cle), &clelogo, &clelogo, &cleword},
  { "Dallas",       "Mavericks",    "DAL", "DAL", &logo_dal, spd_dal, &limb_dalcourt, &dalcourt_textures, NP(spd_dal), &dallogo, &dallogo, &dalword},
  { "Denver",       "Nuggets",      "DEN", "DEN", &logo_den, spd_den, &limb_dencourt, &dencourt_textures, NP(spd_den), &denlogo, &denlogo, &denword},
  { "Detroit",      "Pistons",      "DET", "DET", &logo_det, spd_det, &limb_detcourt, &detcourt_textures, NP(spd_det), &detlogo, &detlogo, &detword},
  { "Golden State", "Warriors",     "G.S", "GOL", &logo_gol, spd_gol, &limb_golcourt, &golcourt_textures, NP(spd_gol), &gollogo, &gollogo, &golword},
  { "Houston",      "Rockets",      "HOU", "HOU", &logo_hou, spd_hou, &limb_houcourt, &houcourt_textures, NP(spd_hou), &houlogo, &houlogo, &houword},
  { "Indiana", 		"Pacers",       "IND", "IND", &logo_ind, spd_ind, &limb_indcourt, &indcourt_textures, NP(spd_ind), &indlogo, &indlogo, &indword},
  { "Los Angeles",  "Clippers",     "LAC", "LAC", &logo_lac, spd_lac, &limb_laccourt, &laccourt_textures, NP(spd_lac), &laclogo, &laclogo, &lacword},
  { "Los Angeles",  "Lakers",       "LAL", "LAL", &logo_lal, spd_lal, &limb_lalcourt, &lalcourt_textures, NP(spd_lal), &lallogo, &lallogo, &lalword},
  { "Miami",        "Heat",         "MIA", "MIA", &logo_mia, spd_mia, &limb_miacourt, &miacourt_textures, NP(spd_mia), &mialogo, &mialogo, &miaword},
  { "Milwaukee",    "Bucks",        "MIL", "MIL", &logo_mil, spd_mil, &limb_milcourt, &milcourt_textures, NP(spd_mil), &millogo, &millogo, &milword},
  { "Minnesota",    "Timberwolves", "MIN", "MIN", &logo_min, spd_min, &limb_mincourt, &mincourt_textures, NP(spd_min), &minlogo, &minlogo, &minword},
  { "New Jersey",   "Nets",         "N.J", "NJN", &logo_njn, spd_njn, &limb_njncourt, &njncourt_textures, NP(spd_njn), &nwjlogo, &nwjlogo, &nwjword},
  { "New York",     "Knicks",       "N.Y", "NYK", &logo_nyk, spd_nyk, &limb_nykcourt, &nykcourt_textures, NP(spd_nyk), &nwylogo, &nwylogo, &nwyword},
  { "Orlando",      "Magic",        "ORL", "ORL", &logo_orl, spd_orl, &limb_orlcourt, &orlcourt_textures, NP(spd_orl), &orllogo, &orllogo, &orlword},
  { "Philadelphia", "76ers",        "PHI", "PHI", &logo_phi, spd_phi, &limb_phicourt, &phicourt_textures, NP(spd_phi), &philogo, &philogo, &phiword},
  { "Phoenix",      "Suns",         "PHO", "PHO", &logo_pho, spd_pho, &limb_phocourt, &phocourt_textures, NP(spd_pho), &phologo, &phologo, &phoword},
  { "Portland",     "Trailblazers", "POR", "POR", &logo_por, spd_por, &limb_porcourt, &porcourt_textures, NP(spd_por), &porlogo, &porlogo, &porword},
  { "Sacramento",   "Kings",        "SAC", "SAC", &logo_sac, spd_sac, &limb_saccourt, &saccourt_textures, NP(spd_sac), &saclogo, &saclogo, &sacword},
  { "San Antonio",  "Spurs",        "S.A", "SAN", &logo_san, spd_san, &limb_sancourt, &sancourt_textures, NP(spd_san), &sanlogo, &sanlogo, &sanword},
  { "Seattle",      "Supersonics",  "SEA", "SEA", &logo_sea, spd_sea, &limb_seacourt, &seacourt_textures, NP(spd_sea), &sealogo, &sealogo, &seaword},
  { "Toronto",      "Raptors",      "TOR", "TOR", &logo_tor, spd_tor, &limb_torcourt, &torcourt_textures, NP(spd_tor), &torlogo, &torlogo, &torword},
  { "Utah",         "Jazz",         "UTA", "UTA", &logo_uta, spd_uta, &limb_utacourt, &utacourt_textures, NP(spd_uta), &utalogo, &utalogo, &utaword},
  { "Vancouver",    "Grizzlies",    "VAN", "VAN", &logo_van, spd_van, &limb_vancourt, &vancourt_textures, NP(spd_van), &vanlogo, &vanlogo, &vanword},
  { "Washington",   "Wizards",      "WAS", "WAS", &logo_was, spd_was, &limb_wascourt, &wascourt_textures, NP(spd_was), &waslogo, &waslogo, &wasword},
  { "Midway",   	"All-Stars",    "MID", "MID", &logo_chi, secret_team, &limb_chicourt, &chicourt_textures, NP(secret_team), &chilogo, &chilogo, &chiword}
};
#undef NP


#define TEAM_XXX	-1
const int team_ladder[][5] = {
//	{TEAM_ATL,TEAM_BOS,TEAM_CHA,TEAM_XXX,-1},
//	{TEAM_CLE,TEAM_DAL,TEAM_DEN,TEAM_XXX,-1},
//	{TEAM_DET,TEAM_GOL,TEAM_HOU,TEAM_IND,-1},
//	{TEAM_LAC,TEAM_LAL,TEAM_MIA,TEAM_MIL,-1},
//	{TEAM_MIN,TEAM_NJN,TEAM_NYK,TEAM_ORL,-1},
//	{TEAM_PHI,TEAM_PHO,TEAM_POR,TEAM_SAC,-1},
//	{TEAM_SAN,TEAM_SEA,TEAM_TOR,TEAM_UTA,-1},
//	{TEAM_VAN,TEAM_WAS,TEAM_XXX,TEAM_XXX,-1},
//	{TEAM_CHI,TEAM_XXX,TEAM_XXX,TEAM_XXX,-1}

//Note:  When a human beats on of the top tier teams, and that team was controlled by a human, then we don't register it as a
//victory on his team ladder.  This forces him to play the best teams when they are controlled by CPU
	{TEAM_LAC,TEAM_DEN,TEAM_VAN,TEAM_NJN,-1},		// Play tier 1 first
	{TEAM_TOR,TEAM_DAL,TEAM_GOL,TEAM_CHI,-1},
	{TEAM_BOS,TEAM_SAC,TEAM_MIN,TEAM_CHA,-1},
	{TEAM_ATL,TEAM_CLE,TEAM_WAS,TEAM_PHO,-1},
	{TEAM_PHI,TEAM_SAN,TEAM_MIL,TEAM_DET,-1},
	{TEAM_NYK,TEAM_SEA,TEAM_ORL,TEAM_XXX,-1},
	{TEAM_HOU,TEAM_LAL,TEAM_XXX,TEAM_XXX,-1},
	{TEAM_UTA,TEAM_POR,TEAM_MIA,TEAM_IND,-1},		// Last (best) team to play
	{TEAM_XXX,TEAM_XXX,TEAM_XXX,TEAM_XXX,-1},		// Last (best) team to play
	};
	
const int beat_all_teams[] = { TEAM_LAL,TEAM_DET,TEAM_POR,TEAM_MIA };	// Teams to play once you are grand champion

