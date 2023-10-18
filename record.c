/*
 *		$Archive: /video/Nba/record.c $
 *		$Revision: 64 $
 *		$Date: 9/17/99 11:02p $
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
char *ss_record_c = "$Workfile: record.c $ $Revision: 64 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include	<glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include	"fontid.h"
#include	"game.h"
#include	"nbacmos.h"
#include	"audits.h"
#include	"externs.h"
#include	"putil.h"
#include	"select.h"

#define	 NULL	0
#define ALLTEAMS_MASK	((1 << NUM_TEAMS)-1)
#define ALTLADDER_MASK	((1 << (NUM_TEAMS + 1))-1)

void update_other_cmos_to_disk(int sleep_allowed);

static int team_tier( int );

//
// Global ram definitions
//
extern int do_grand_champ;						// Flg to do grand champ screen
extern int was_awarded_attr[4];

extern int four_plr_ver;

plyr_record_t	*rec_sort_ram = NULL;	// pointer to the array of PLAYER RECORDs from cmos into ram
extern int qtr_pstatus[];
extern const int team_ladder[8][5];
		// pass completions
extern float pass_rating(int, int, int, int, int);
extern player_record_info selected_players[];
static int cmp_winstreak(int a, int b);
static plyr_record_t  default_records[] = {
/*
       plyr_name
                plyr_pin_nbr
                       teams_defeated
                         last_used
                           games_played
                             games_won
                               two_ptrs_attempted
                                 two_ptrs_made
                                   three_ptrs_attempted
                                     three_ptrs_made
                                       dunks_attempted
                                         dunks_made
                                           rebounds
                                             assists
                                               steals
                                                 blocks
                                                   turnovers
                                                     points_scored
                                                       points_allowed
                                                         trivia_pts
                                                           winstreak
                                                             world_records
                                                               is_creatp_record
                                                                 height
                                                                   weight
                                                                     power
                                                                       speed
                                                                         twop
                                                                           threep
                                                                             steal
                                                                               block
                                                                                 dunks
                                                                                   dribble
                                                                                     team
                                                                                       player_index
                                                                                         jersey_city
                                                                                           jersey_style
                                                                                             jersey_num
                                                                                               privileges
                                                                                                 nickname
                                                                                                   unused_attribute_points
                                                                                                     unused_privilege_points
                                                                                                       hotspot*/
	/* these records are fakes for the high score list */
	  {"TURMEL","0322",0,1,5,3,4,3,6,1,2,4,5,2,2,5,1,121,294,3,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"GATSON","1111",0,1,5,2,3,1,6,2,2,1,1,5,1,4,1,127,309,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"GUIDO ","6765",0,1,5,1,4,1,6,3,2,1,1,1,1,3,1,110,303,3,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"DANIEL","0604",0,1,5,2,3,2,6,2,2,2,2,2,2,2,1,122,300,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"JAPPLE","6660",0,1,5,2,4,4,6,5,2,5,2,2,2,1,1,126,289,3,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"SAL   ","0201",0,1,5,3,3,0,6,0,2,4,0,4,3,4,1,121,297,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"JENIFR","3333",0,1,5,2,3,1,6,0,2,2,1,3,4,3,1,124,310,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"E GEER","1105",0,1,5,1,3,1,6,1,2,3,1,1,2,2,1,116,289,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"MATT G","1006",0,1,5,1,4,2,6,4,2,2,2,2,2,1,1,120,288,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"TIMMYB","3314",0,1,5,1,3,1,6,3,2,1,1,4,2,2,1,121,297,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"GENTIL","1228",0,1,5,2,3,1,6,4,2,1,2,1,1,1,1,118,306,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"ROOT  ","6000",0,1,5,1,3,1,6,4,2,4,1,1,1,1,1,113,299,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"JONHEY","8823",0,1,5,2,3,1,6,1,2,1,4,1,2,1,1,116,291,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"LIPTAK","0114",0,1,5,1,3,1,6,1,2,1,3,1,1,1,1,114,303,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  {"LYNCH ","0000",0,1,5,1,3,0,6,2,2,2,2,2,0,1,1,107,302,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  /* these records are REAL be careful with them */
	  /* NOTE sound bank 55 is EJK, 56 is CM, 57 is UTB 58 is pumpkinhead */
	  {"KEENAN","3141",0,1,5,2,3,2,6,2,2,3,0,0,0,2,1,122,297,4,1,0,1,8,8,14,9,18,9,8,11,9,9,TEAM_SECRET,PINTO_CHARACTER,CUSTOM_UNIFORM_TEAM,BLACK_UCOLOR,0x35,0,55,0,2,0},
	  {"CARYM!","0702",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,8,8,8,8,8,8,8,8,TEAM_SECRET,REF_CHARACTER,TEAM_ATL,AWAY_JERSEY,0x00,0,56,8,2,0},
	  {"OLVRBK","882 ",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,8,8,8,8,8,8,8,8,TEAM_SECRET,JON_H_CHARACTER,TEAM_ATL,AWAY_JERSEY,0x96,0,57,8,2,0},
	  {"VILLY ","0814",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,8,16,9,10,14,9,17,11,TEAM_SECRET,WIZARD_CHARACTER,CUSTOM_UNIFORM_TEAM,PURPLE_UCOLOR,0x14,0,51,0,2,0},
	  {"JOSHUA","1128",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,16,18,8,9,8,12,18,9,TEAM_SECRET,PUMPKIN_CHARACTER,TEAM_ATL,AWAY_JERSEY,0x0,PRIV_HIDEATTR,58,0,1,0},
	  {"K DAY ","4242",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,16,14,8,14,9,9,14,10,TEAM_SECRET,WIZARD_CHARACTER,CUSTOM_UNIFORM_TEAM,RED_UCOLOR,0x42,0,12,0,2,0},
	  {"TOBIAS","4321",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,16,14,8,14,9,9,14,9,TEAM_SECRET,FRANK_CHARACTER,TEAM_ATL,AWAY_JERSEY,0x24,PRIV_HIDEATTR,0,0,1,0},
	  {"HERMAN","0224",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,10,16,8,16,9,10,11,9,TEAM_SECRET,CREATURE_CHARACTER,TEAM_ATL,AWAY_JERSEY,0x8,PRIV_HIDEATTR,35,0,1,0},
	  {"HO    ","8888",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,16,10,8,11,9,16,11,9,TEAM_SECRET,DEN_MASCOT,TEAM_ATL,AWAY_JERSEY,0x00,PRIV_HIDEATTR,0,0,1,0},
	  {"CARLOS","1213",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,9,10,8,16,9,9,11,16,TEAM_SECRET,SEA_MASCOT,TEAM_ATL,AWAY_JERSEY,0x00,PRIV_HIDEATTR,0,0,1,0},
	  {"MXP   ","0937",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,9,10,8,16,16,9,11,9,TEAM_SECRET,JAM_CHARACTER,TEAM_BOS,AWAY_JERSEY,0x00,PRIV_JERSEY|PRIV_HIDEATTR,7,0,1,0},
	  {"FED   ","3316",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,16,8,9,16,8,16,9,9,11,9,TEAM_SECRET,UTA_MASCOT,TEAM_ATL,AWAY_JERSEY,0x33,PRIV_HIDEATTR,3,0,1,0},
	  {"NIK   ","1117",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,8,16,16,8,9,12,8,9,9,TEAM_SECRET,CLOWN_CHARACTER,CUSTOM_UNIFORM_TEAM,BLUE_UCOLOR,0x23,PRIV_JERSEY|PRIV_HIDEATTR,33,0,1,0},
	  {"SNAKE ","1221",0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,11,3,10,14,8,16,19,10,10,16,TEAM_SECRET,CREATURE_CHARACTER,CUSTOM_UNIFORM_TEAM,BLUE_UCOLOR,0x13,PRIV_HIDEATTR,0,0,1,0}
};

static world_record_t  default_world_records[] = {
	{"TURMEL","0322",5},					// most rebounds in game
	{"GUIDO ","6765",3},					// most 3 pointers in game
	{"JAPPLE","6660",5},					// most dunks in game
	{"GATSON","1111",5},					// most assists in game
	{"JENIFR","3333",4},					// most steals
	{"DANIEL","0604",10},					// most points in game
};

//
// Variables
//
plyr_record_t		tmp_plyr_record;
world_record_t		tmp_world_rec;

//externs
//extern tdata_t				tdata_blocks[];					// team bookeeping structures (2)
extern plyr_record_t		player1_data;
extern plyr_record_t		player2_data;
extern plyr_record_t		player3_data;
extern plyr_record_t		player4_data;

extern plyr_record_t 		*plyr_data_ptrs[];
extern plyr_record_t 		*plyr_stat_ptrs[];


//-------------------------------------------------------------------------------------------------
// This routine sets up cmos.. for the AUDITS, ADJUSTMENTS
//     HIGH SCORES TABLES...and then the PLAYER RECORDS
//
// INPUT:	nothing
// RETURNS: error code (see cmos.h for error codes)
//-------------------------------------------------------------------------------------------------
int init_cmos(void)
{
	struct cmos_config_info	cmos_init_struct;
	int	i;
	FILE *fp;
	
	cmos_init_struct.cmos_size = CMOS_SIZE;
	cmos_init_struct.max_audits = NEXT_AUD + 1;
	cmos_init_struct.max_adjustments = NUM_ADJUSTMENTS;
	cmos_init_struct.num_gcr_tables = GEN_REC_ARRAYS;
	cmos_init_struct.user_data_size = 0;
	cmos_init_struct.gcr[PLYR_RECS_ARRAY].entry_size = sizeof(plyr_record_t);
	cmos_init_struct.gcr[PLYR_RECS_ARRAY].num_entries = NUM_PLYR_RECORDS;
	
	cmos_init_struct.gcr[WORLD_RECS_ARRAY].entry_size = sizeof(world_record_t);
	cmos_init_struct.gcr[WORLD_RECS_ARRAY].num_entries = NUM_WORLD_RECORDS;
	
	fp = fopen("audits.fmt", "wb");
	if (fp == NULL) {
		fprintf(stderr, "Can not open file AUDITS.FMT\r\n");
#ifdef DEBUG
		lockup();
#endif
	} else {
		fprintf(fp, "cmos size %d audits %d adjustments %d gcrtables %d cmosdone\r\n",
				cmos_init_struct.cmos_size,
				cmos_init_struct.max_audits,
				cmos_init_struct.max_adjustments,
				cmos_init_struct.num_gcr_tables);
		
		for(i = 0; i < cmos_init_struct.num_gcr_tables; i++) {
			fprintf(fp, "cmosgcr entrysize %d numentries %d cmosgcrdone\r\n",
					cmos_init_struct.gcr[i].entry_size,
					cmos_init_struct.gcr[i].num_entries);
		}
		fprintf(fp, "\r\n");
		
		write_audit_info(fp);
		fflush(fp);
		fclose(fp);
	}
 	return config_cmos(&cmos_init_struct);
}	/* init_cmos */

//-------------------------------------------------------------------------------------------------
// 								This routine checks the player records for validity
//
// INPUT:		nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
int validate_plyr_records(void)
{
	int rec_num;
	int status;
	
	status = 0;
 	for (rec_num = 0; rec_num < NUM_PLYR_RECORDS; rec_num++) {
		if (get_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record)) {
			/* bad record...clear it */
			status = 1;
			clear_cmos_record((char *)&tmp_plyr_record, sizeof(plyr_record_t));
			if (set_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record)) {
#ifdef DEBUG
				fprintf(stderr, "Error occured in trying to set cmos record.\r\n");
				lockup();
#endif
			}
	  	}
	}
	if (status) {
		set_default_plyr_records();
		set_default_world_records();
	}
	return status;
}	/* validate_plyr_records */

//-------------------------------------------------------------------------------------------------
//								Sets up the first X records in cmos to some default values
//
// INPUT:	  nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void set_default_plyr_records(void)
{
	int rec_num, i;
	
	i = sizeof(default_records) / sizeof(plyr_record_t);
	for (rec_num=0; rec_num < i; rec_num++)
	{
		if (set_generic_record(PLYR_RECS_ARRAY,rec_num,&default_records[rec_num]))
		{
#ifdef DEBUG
			fprintf(stderr, "Error occured when trying to setup default player records.\r\n");
			lockup();																// write to cmos failed just lockup
#endif
		}
	}
	set_audit(ACTIVE_PLAYER_RECORD_AUD, i);
	
	// Clear the remainder of the records (factory reset purposes)
	clear_cmos_record((char *)&tmp_plyr_record, sizeof(plyr_record_t));
	while (rec_num < NUM_PLYR_RECORDS)
	{
		if (set_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))
		{
#ifdef DEBUG
			fprintf(stderr, "Error occured in trying to set cmos record.\r\n");
			lockup();
#endif
		}
		rec_num++;
	}
}	/* set_default_plyr_records */

//-------------------------------------------------------------------------------------------------
//								Sets up the first X world records in cmos to some default values
//
// INPUT:	  nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void set_default_world_records(void)
{
	int rec_num;
	
	for (rec_num = 0; rec_num < NUM_WORLD_RECORDS; rec_num++) {
		if (set_generic_record(WORLD_RECS_ARRAY,rec_num,&default_world_records[rec_num])) {
#ifdef DEBUG
			fprintf(stderr, "Error occured when trying to setup default world records.\r\n");
			lockup();
#endif
		}
	}
}	/* set_default_world_records */

//-------------------------------------------------------------------------------------------------
//					This routine clears (sets to 0) every element in the player record.
//
// INPUT:  ptr. to rec to clear, and number of bytes to clear
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void clear_cmos_record(char *rec, int bytes_in_rec)
{
	while (bytes_in_rec) {
		*rec++ = 0;
		bytes_in_rec--;
	}
}	/* clear_cmos_record */

//-------------------------------------------------------------------------------------------------
//										Put player record into cmos from ram
//
//   INPUT: nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void save_player_records(void)
{
	plyr_record_t *prec;
	plyr_record_t *pstat;
//	tdata_t *trec;
	int rec_num, i, x;
	int gmsk;
	int team_x, oppo_x;
	int score[2];
	
	gmsk = (1 << game_info.game_quarter) - 1;
	score[0] = compute_score(0);
	score[1] = compute_score(1);

	for (x = 0; x < MAX_PLYRS; x++)
		was_awarded_attr[x] = 0;
	for (x = 0; x < MAX_PLYRS; x++)
	{
		team_x = (x >= NUM_PERTEAM);
		oppo_x = !team_x;
		
		prec  = plyr_data_ptrs[x];
		pstat = plyr_stat_ptrs[x];
		
		if ((selected_players[x].record_num != -1) && (qtr_pstatus[x] == gmsk))
		{
			//rec_num = find_record_in_cmos(prec->plyr_name);
			rec_num = selected_players[x].record_num;
	
			//	get_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record);
			memcpy(&tmp_plyr_record,&selected_players[x].cmos_data,sizeof(plyr_record_t));
			tmp_plyr_record.games_played++;
			tmp_plyr_record.last_used = 0;
			
			tmp_plyr_record.points_scored  += score[team_x];
			tmp_plyr_record.points_allowed += score[oppo_x];
			
			if (score[team_x] > score[oppo_x])
			{
				int defeated_team;
				
				defeated_team = game_info.team[oppo_x];
				tmp_plyr_record.games_won++;
				if (tmp_plyr_record.winstreak < 0)
					tmp_plyr_record.winstreak = 1;
				else
					tmp_plyr_record.winstreak++;

				if ((tmp_plyr_record.games_won % 3 == 0) && tmp_plyr_record.is_creatp_record)
				{
					int total;
					
					total = tmp_plyr_record.height + tmp_plyr_record.weight + tmp_plyr_record.power;
					total += tmp_plyr_record.speed + tmp_plyr_record.twop + tmp_plyr_record.threep;
					total += tmp_plyr_record.steal + tmp_plyr_record.block + tmp_plyr_record.dunks;
					total += tmp_plyr_record.dribble + tmp_plyr_record.unused_attribute_points;
					if (total < 120) {
						tmp_plyr_record.unused_attribute_points += 2;
						was_awarded_attr[x] = 1;
					}
				}
				
				if ((!qtr_pstatus[x^2] && !qtr_pstatus[x^3]) ||
					 (team_tier(defeated_team) < 6 ))
				{
					i = tmp_plyr_record.teams_defeated;
					if (defeated_team == TEAM_SECRET) {
						int alt_count;
						
						alt_count = get_alt_ladder_size();
						if (tmp_plyr_record.alt_ladder < alt_count)
							tmp_plyr_record.alt_ladder++;
						if (tmp_plyr_record.alt_ladder == alt_count)
							tmp_plyr_record.teams_defeated |= 1 << defeated_team;
					} else
						tmp_plyr_record.teams_defeated |= 1 << defeated_team;
					if ((tmp_plyr_record.teams_defeated == ALLTEAMS_MASK) && (i != ALLTEAMS_MASK)) {
						do_grand_champ |= (1 << x);
						tmp_plyr_record.alt_ladder = 0;
					} else if ((tmp_plyr_record.teams_defeated == ALTLADDER_MASK) && (i != ALTLADDER_MASK)) {
						do_grand_champ |= (1 << (x + 16));
					}
				}
			}
			else
			{
				if (tmp_plyr_record.winstreak >= 0)
					tmp_plyr_record.winstreak = -1;
				else
					tmp_plyr_record.winstreak--;
			}
			
			// add in game stats
			tmp_plyr_record.two_ptrs_attempted   += pstat->two_ptrs_attempted;
			tmp_plyr_record.two_ptrs_made        += pstat->two_ptrs_made;
			tmp_plyr_record.three_ptrs_attempted += pstat->three_ptrs_attempted;
			tmp_plyr_record.three_ptrs_made      += pstat->three_ptrs_made;
			tmp_plyr_record.dunks_attempted      += pstat->dunks_attempted;
			tmp_plyr_record.dunks_made           += pstat->dunks_made;
			tmp_plyr_record.assists              += pstat->assists;
			tmp_plyr_record.blocks               += pstat->blocks;
			tmp_plyr_record.steals               += pstat->steals;
			tmp_plyr_record.turnovers            += pstat->turnovers;
			tmp_plyr_record.rebounds             += pstat->rebounds;
			tmp_plyr_record.trivia_pts            = prec->trivia_pts;
			
			set_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record);
		}
	}
	inc_cmos_records_used_cnt();		/* why was this commented out? */
	
	/* write out the CMOS to disk */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);
}



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int get_new_record_into_ram(char *name_and_pin)
{
	int rec_num,i;
	
	rec_num = get_free_record_nbr();
	
	clear_cmos_record((char *)&tmp_plyr_record, sizeof(plyr_record_t));
	
	// copy over name and pin
	for (i = 0; i < LETTERS_IN_NAME + PIN_NUMBERS; i++)
		tmp_plyr_record.plyr_name[i] = name_and_pin[i];
	
	// set some default stuff
  //	tmp_plyr_record.cmos_rec_num = rec_num;
	
	return rec_num;
}	/* get_new_record_into_ram */

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void inc_cmos_records_used_cnt(void)
{
	int rec_num;
	
	/* inc all records 'last_used' count */
	for (rec_num = 0; rec_num < NUM_PLYR_RECORDS; rec_num++) {
		if (get_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record)) {
			if ((tmp_plyr_record.games_played > 0) || tmp_plyr_record.is_creatp_record) {
				tmp_plyr_record.last_used++;
				set_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record);
			}
		}
	}
}	// inc_cmos_records_used_cnt
	
//-------------------------------------------------------------------------------------------------
//										Try to find a matching record in cmos
//
//   INPUT:  ptr. to players entered name and pin number
//  RETURN: matching record number else -1
//-------------------------------------------------------------------------------------------------
int find_record_in_cmos(char *pname_and_pin)
{
	int rec_num;
	
	for (rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++) {
		if (get_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record))
			clear_cmos_record((char *)&tmp_plyr_record, sizeof(plyr_record_t));
		else if (!(compare_record((char *)&tmp_plyr_record.plyr_name, pname_and_pin,LETTERS_IN_NAME+PIN_NUMBERS)))
			return rec_num;
	}
	return -1;
}	/* find_record_in_cmos */

//-------------------------------------------------------------------------------------------------
//									Try to find a matching record in RECORD SORT ram
//
//   INPUT:  ptr. to players entered name and pin number
//  RETURN: matching record number else -1
//-------------------------------------------------------------------------------------------------
int find_record_in_ram(char *pname_and_pin)
{
	int rec_num;
	plyr_record_t *plr_rec;
	
	if (rec_sort_ram != NULL)
	{
		plr_rec = rec_sort_ram;
		for (rec_num = 0; rec_num < NUM_PLYR_RECORDS; rec_num++, plr_rec++)
			if (!(compare_record((char *)plr_rec->plyr_name, pname_and_pin, LETTERS_IN_NAME + PIN_NUMBERS)))
				return(rec_num + 1);
	}
	return(-1);
}

//-------------------------------------------------------------------------------------------------
//					Check to determine if the player has beaten any world records, if
// record was broken the new info is stored and 1 is returned.
//-------------------------------------------------------------------------------------------------
void check_world_records(void)
{
	plyr_record_t	*prec;
	plyr_record_t	*prec2;
	//int	diff;
	int i;

	// check if player beat any WORLD RECORDS...if so... set new world record
	for (i=0; i < MAX_PLYRS; i++)
	{
		prec = plyr_stat_ptrs[i];
		prec2 = plyr_data_ptrs[i];

		if (selected_players[i].record_num != -1)
		{
			// most rebounds
			if (!(get_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < prec->rebounds)
				{
					tmp_world_rec.value = prec->rebounds;
					strncpy(tmp_world_rec.plyr_name,prec2->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec);
				}
			}
			// most 3 pointers in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < prec->three_ptrs_made)
				{
					tmp_world_rec.value = prec->three_ptrs_made;
					strncpy(tmp_world_rec.plyr_name,prec2->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec);
				}
			}
			// most dunks in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < prec->dunks_made)
				{
					tmp_world_rec.value = prec->dunks_made;
					strncpy(tmp_world_rec.plyr_name,prec2->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec);
				}
			}
			// most assists in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < prec->assists)
				{
					tmp_world_rec.value = prec->assists;
					strncpy(tmp_world_rec.plyr_name,prec2->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec);
				}
			}
			// most steals in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < prec->steals)
				{
					tmp_world_rec.value = prec->steals;
					strncpy(tmp_world_rec.plyr_name,prec2->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec);
				}
			}
			// most points in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec)))
			{
				int score;
				
				score = ((prec->two_ptrs_made + prec->dunks_made) * 2) + (prec->three_ptrs_made * 3);
				if (tmp_world_rec.value < score)
				{
					tmp_world_rec.value = score;
					strncpy(tmp_world_rec.plyr_name,prec2->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec);
				}
			}
		}	
	}
}


//-------------------------------------------------------------------------------------------------
//				    Determines wheter current cmos record matches players record
//
//	This routine assumes that the data to compare resides at the start of the pointer (structs)
//
//   INPUT:  ptr to cmos record, ptr. to player record data in ram, size of data
//  RETURN: 0 = match else non-zero
//-------------------------------------------------------------------------------------------------
int compare_record(char *plr_name_n_pin1, char *plr_name_n_pin2, int n)
{

	while (n)
	{
		if (*plr_name_n_pin1++ != *plr_name_n_pin2++)
			return 1;									/* records are different */
		n--;
	}
	return 0;											/* records are the same */
}

//-------------------------------------------------------------------------------------------------
//									Gets the "least" used record number
//   INPUT: none
//  RETURN: free record number
//-------------------------------------------------------------------------------------------------
int get_free_record_nbr(void)
{
	int rec_num;
	int last_used_cnt;
	int least_used_rec = 0;
	int useable, i;
	
	/* this will FIND the first available record...or allow the LEAST used record to be over written */
	last_used_cnt = -1;
	for (rec_num = 0; rec_num < NUM_PLYR_RECORDS; rec_num++) {
		if (get_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record))
			clear_cmos_record((char *)&tmp_plyr_record, sizeof(plyr_record_t));
		/* an empty record...use it */
		useable = TRUE;
		for (i = 0; i < NUM_PLAYERS; i++) {
			if (selected_players[i].record_num == rec_num) {
				useable = FALSE;
				break;
			}
		}
		if ((tmp_plyr_record.games_played <= 0) && !tmp_plyr_record.is_creatp_record && useable) {
			increment_audit(ACTIVE_PLAYER_RECORD_AUD);
			return rec_num;
		}		
		
		/* find the least used rec.	 */
		if (tmp_plyr_record.last_used >= last_used_cnt) {
			last_used_cnt = tmp_plyr_record.last_used;
			least_used_rec = rec_num;
		}
	}
	increment_audit(PURGED_RECORD_AUD);
	return least_used_rec;
}	/* get_free_record_nbr */

//-------------------------------------------------------------------------------------------------
//						This routine gets all the players records into MALLOC'd ram
//
//   INPUT: none
// RETURNS: ptr. to JMALLOC'd ram
//-------------------------------------------------------------------------------------------------
void get_all_records(void)
{
	int rec_num;
	plyr_record_t *rec_ram;
	
	if (!rec_sort_ram)
		rec_sort_ram = (plyr_record_t *)malloc((NUM_PLYR_RECORDS * sizeof(plyr_record_t)));
	
	if (rec_sort_ram != NULL) {
		rec_ram = rec_sort_ram;
		
		for (rec_num = 0; rec_num < NUM_PLYR_RECORDS; rec_num++,rec_ram++)
			get_generic_record(PLYR_RECS_ARRAY, rec_num, rec_ram);
	} else {
#ifdef DEBUG
		fprintf(stderr, "get_all_records() - Can not allocate memory for player records\r\n");
		lockup();
#endif
	}
}	/* get_all_records */

static int cmp_winstreak(int a, int b)
{
	if ((a < 0) && (b < 0)) {
		a = -a;
		b = -b;
		return b - a;
	} else if ((a >= 0) && (b >= 0))
		return a - b;
	else if (a < 0)
		return -1;
	else
		return +1;
}	/* cmp_winstreak */

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to TRIVIA POINTS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_trivia(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->trivia_pts - rec1->trivia_pts);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_wins(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->games_won - rec1->games_won);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_win_streaks(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = cmp_winstreak(rec2->winstreak, rec1->winstreak);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_win_percent(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp, prcnt1, prcnt2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
//	if (rec1->games_played <= 0)
//		prcnt1 = 0;
//	else
	prcnt1 = ((rec1->games_won * 1000)/rec1->games_played);
	
//	if (rec2->games_played <= 0)
//		prcnt2 = 0;
//	else
	prcnt2 = ((rec2->games_won * 1000)/rec2->games_played);
		
	tmp = (int)(prcnt2 - prcnt1);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_games_played(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->games_played - rec1->games_played);
	if (!tmp)
	{
		tmp = (int)rec2->games_won - rec1->games_won;
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//				This routine sorts the player records according to MOST POINTS SCORED PER GAME
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_points_scored(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp,gms_plyd1,gms_plyd2;


	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	gms_plyd1 = (int)rec1->games_played;
	if (!gms_plyd1)																				// player hasn't played a game (empty record) put on bottom
		gms_plyd1 = 500;																		// Just trick the sort into thinking this player sucks.

	gms_plyd2 = (int)rec2->games_played;
	if (!gms_plyd2)																				// player hasn't played a game (empty record) put on bottom
		gms_plyd2 = 500;																		// Just trick the sort into thinking this player sucks.

	tmp = ((rec2->points_scored*100) / gms_plyd2) - ((rec1->points_scored*100) / gms_plyd1);
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//				This routine sorts the player records according to LEAST POINTS ALLOWED PER GAME
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_points_allowed(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp,gms_plyd1,gms_plyd2,pts_alwd1,pts_alwd2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	gms_plyd1 = (int)rec1->games_played;
	pts_alwd1 = (int)rec1->points_allowed;
	if (!gms_plyd1)
	{																									// player hasn't played a game (empty record) put on bottom
		gms_plyd1 = 1;																		// Just trick the sort into thinking this player sucks.
		pts_alwd1 = 500;																	// Worst player ever
	}

	gms_plyd2 = (int)rec2->games_played;
	pts_alwd2 = (int)rec2->points_allowed;
	if (!gms_plyd2)
	{																									// player hasn't played a game (empty record) put on bottom
		gms_plyd2 = 1;																		// Just trick the sort into thinking this player sucks.
		pts_alwd2 = 500;																	// Worst player ever
	}

	tmp = (int)(((pts_alwd1*100) / gms_plyd1) - ((pts_alwd2*100) / gms_plyd2));
	if (!tmp)
	{
		tmp = (int)(rec1->games_won - rec2->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}


//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_teams_defeated(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	tmp = (calc_tms_def(rec2->teams_defeated) - calc_tms_def(rec1->teams_defeated));
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_dunk_percent(plyr_record_t *rec1, plyr_record_t *rec2)
{
	//float	r1 ,r2;
	int		tmp;
	//int		dm1,da1;
	//int		dm2,da2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	tmp = (int)(rec2->dunks_made - rec1->dunks_made);
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);

//	da1 = (int)rec1->dunks_attempted;
//	dm1 = (int)rec1->dunks_made;
//	if (!da1)
//	{
//		da1 = 1;
//		dm1 = 500;
//	}
//	da2 = (int)rec2->dunks_attempted;
//	dm2 = (int)rec2->dunks_made;
//	if (!da2)
//	{
//		da2 = 1;
//		dm2 = 500;
//	}
//
//	r1 = (int)dm1/da1;
//	r2 = (int)dm2/da2;
//	tmp = (int)(1000.0f * (r2 - r1));
//
//	if (!tmp)
//	{
//		tmp = (int)(rec2->games_won - rec1->games_won);
//		if (!tmp)
//			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
//	}
//	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_3pt_percent(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int		tmp;
	//float	r1, r2;
	//int		tpm1,tpa1;
	//int		tpm2,tpa2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;


	tmp = (int)(rec2->three_ptrs_made - rec1->three_ptrs_made);
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);

//	tpa1 = (int)rec1->three_ptrs_attempted;
//	tpm1 = (int)rec1->three_ptrs_made;
//	if (!tpa1)
//	{
//		tpa1 = 1;
//		tpm1 = 500;
//	}
//	tpa2 = (int)rec2->three_ptrs_attempted;
//	tpm2 = (int)rec2->three_ptrs_made;
//	if (!tpa2)
//	{
//		tpa2 = 1;
//		tpm2 = 500;
//	}
//		
//	r1 = (int)tpm1/tpa1;
//	r2 = (int)tpm2/tpa2;
//	tmp = (int)(1000.0f * (r2 - r1));
//	
//	if (!tmp)
//	{
//		tmp = (int)(rec2->games_won - rec1->games_won);
//		if (!tmp)
//			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
//	}
//	return (tmp);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int calc_tms_def(int tms_def)
{
	int bit_num, cnt;
	
	cnt = 0;
	for (bit_num = 0; bit_num < (NUM_TEAMS + 1); bit_num++)
		if (tms_def & (1 << bit_num))
			cnt++;
	return cnt;
}	/* calc_tms_def */


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
static int team_tier( int team )
{
	int i, tier;
	
	if (team == TEAM_SECRET)
		return 8;
	for (tier = 0; tier < 9; tier++)
		for (i = 0; team_ladder[tier][i] != -1; i++)
			if (team_ladder[tier][i] == team)
				return tier;
	return -1;
}
