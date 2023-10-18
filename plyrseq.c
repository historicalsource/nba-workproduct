/*
 *		$Archive: /video/Nba/plyrseq.c $
 *		$Revision: 426 $
 *		$Date: 4/23/99 2:33p $
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
char *ss_plyrseq_c = "$Workfile: plyrseq.c $ $Revision: 426 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <math.h>
#include <goose/pmath.h>
#include "ani3d.h"
#include "camdefs.h"
#include "camdata.h"

#ifdef NOSETCAM
asm("
			.equ	DOSETCAM,0
");
#else
asm("
			.equ	DOSETCAM,1 
");
#endif

/*****************************************************************************/

asm("
			.include include/ani3d.equ
			.include anim/gen_seq.equ
			.include include/player.equ
			.include include/sndcalls.equ

			.globl	set_trgt_fwd, rotate_anim
			.globl	turn_player, turn_player_with_vel
			.globl	is_taunt_done, play_rand_taunt
			.globl	neutral_stick_general_run
			.globl	release_puppet, celebrate
			.globl	face_carrier, face_receiver
			.globl	do_printf_in_anim, plyr_bnk_str
			.globl  pick_random_tipoff_anim
			.globl  announce_players_name

#;;;;;;;;;;;;;;;

			.globl	zero_anim
			.byte	SCRIPT
zero_anim:
			.byte	ANI_SETMODE
			.word	0

zero_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_ZERO

			.byte	ANI_RUNTOFRAME,4,1

			.byte	ANI_GOTO
			.word	zero_loop

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; buyin screen stance #1

			.globl	buyin_stance1_anim
			.byte	SCRIPT
buyin_stance1_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_S_LOOP_1
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	4.40
			#.float	1.15

#			.byte	ANI_SETMODE
#			.word	0

#			.byte	ANI_CODE
#			.word	reset_x_position,0,0,0

			.byte	ANI_GOTO
			.word	buyin_stance1_anim

			.byte	ANI_END
	
#;;;;;;;;;;;;;;;;;;;;; buyin screen stance #2

			.globl	buyin_stance2_anim
			.byte	SCRIPT
buyin_stance2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_S_LOOP_2
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	4.40

#			.byte	ANI_SETMODE
#			.word	0

			.byte	ANI_GOTO
			.word	buyin_stance2_anim

			.byte	ANI_END
	
#;;;;;;;;;;;;;;;;;;;;; buyin screen stance #3

			.globl	buyin_stance3_anim
			.byte	SCRIPT
buyin_stance3_anim:
			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
			.word	MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_S_LOOP_3
#			.byte	ANI_RUNTOFRAME,0,1
#
#			.byte	ANI_STREAMTOEND
#			.float	4.40
buyin_stance3:

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_S_LOOP_3
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	4.40

#			.byte	ANI_SETMODE
#			.word	0

#			.byte	ANI_CODE
#			.word	reset_x_position,0,0,0

			.byte	ANI_GOTO
			.word	buyin_stance3

			.byte	ANI_END
			
#;;;;;;;;;;;;;;;;;;;;; buyin screen stance #4

			.globl	buyin_stance4_anim
			.byte	SCRIPT
buyin_stance4_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_S_LOOP_4
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	4.50

#			.byte	ANI_SETMODE
#			.word	0

			.byte	ANI_GOTO
			.word	buyin_stance4_anim

			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;;; defensive breath

			.globl	s_ready_anim
			.globl	run_slow_down_anim
			.byte	SCRIPT
s_ready_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
s_ready_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_S_READY

			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	s_ready_lp
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;;; defensive breath

			.globl	s_ready_anim_int
			.byte	SCRIPT
s_ready_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_S_READY
			.byte	0

			.byte	ANI_RUNTOEND,1
s_ready_lp2:
			.byte	ANI_NEWSEQ
			.word	SEQ_S_READY

			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	s_ready_lp2
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; have runners slow down before entering STANCE

run_slow_down_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_R2STOP_2
			.byte	7

			.byte	ANI_STREAMTOEND
			.float	.95
			
			.byte	ANI_GOTO
			.word	s_breath1_anim_int
			
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath1_anim
			.byte	SCRIPT
s_breath1_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

s_breath1_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.2

			.byte	ANI_GOTO
			.word	s_breath1_lp

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath2_anim
			.byte	SCRIPT
s_breath2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

s_breath2_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH3

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_GOTO
			.word	s_breath2_lp

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath3_anim
			.byte	SCRIPT
s_breath3_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

s_breath3_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_S_CYC_1

			.byte	ANI_STREAMTOEND
			.float	2.5

			.byte	ANI_GOTO
			.word	s_breath3_lp

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath5_anim
			.byte	SCRIPT
s_breath5_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

s_breath5_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_S_CYC_3

			.byte	ANI_STREAMTOEND
			.float	2.0

			.byte	ANI_GOTO
			.word	s_breath5_lp

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath3_anim_int
			.byte	SCRIPT
s_breath3_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_ZEROVELS

			.byte	ANI_JIFSEQ
			.word	SEQ_P_BCK2_R,to_breath3

			.byte	ANI_JIFSEQ
			.word	SEQ_P_BCK2_L,to_breath3

			.byte	ANI_JIFSEQ
			.word	SEQ_SBREATH1,to_breath3

			.byte	ANI_JIFSEQ
			.word	SEQ_SBREATH3,to_breath3

			.byte	ANI_JIFSEQ
			.word	SEQ_S_CYC_3,to_breath3

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_S_CYC_1
			.byte	0

			.byte	ANI_GOTO
			.word 	s_breath3_loop

to_breath3:
			.byte	ANI_INTTOSEQ,16
			.word	SEQ_S_CYC_1
			.byte	0

s_breath3_loop:
			.byte	ANI_STREAMTOEND
			.float	2.5

			.byte	ANI_GOTO
			.word	s_breath1_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath5_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath1_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath2_anim_int

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath5_anim_int
			.byte	SCRIPT
s_breath5_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_JIFSEQ
			.word	SEQ_P_BCK2_R,to_breath5

			.byte	ANI_JIFSEQ
			.word	SEQ_P_BCK2_L,to_breath5

			.byte	ANI_JIFSEQ
			.word	SEQ_SBREATH1,to_breath5

			.byte	ANI_JIFSEQ
			.word	SEQ_SBREATH3,to_breath5

			.byte	ANI_JIFSEQ
			.word	SEQ_S_CYC_1,to_breath5

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_S_CYC_3
			.byte	0
			
			.byte	ANI_GOTO
			.word 	s_breath5_loop

to_breath5:
			.byte	ANI_INTTOSEQ,16
			.word	SEQ_S_CYC_3
			.byte	0

s_breath5_loop:
			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_GOTO
			.word	s_breath1_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath1_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath2_anim_int

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

#			.globl	s_breath1_anim
#			.byte	SCRIPT
#s_breath1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_GOTO
#			.word	s_breath1_loop
#
#s_breath1_int_to:
#			.byte	ANI_INTTOSEQ,16
#			.word	SEQ_SBREATH1
#			.byte	0
#
s_breath1_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.2

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.2

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath3_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath2_anim_int

			.byte	ANI_GOTO
			.word	s_breath1_loop
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath1_anim_int
			.byte	SCRIPT
s_breath1_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_JIFSEQ
			.word	SEQ_P_BCK2_R,to_breath1

			.byte	ANI_JIFSEQ
			.word	SEQ_P_BCK2_L,to_breath1

			.byte	ANI_JIFSEQ
			.word	SEQ_SBREATH3,to_breath1

			.byte	ANI_JIFSEQ
			.word	SEQ_S_CYC_1,to_breath1

			.byte	ANI_JIFSEQ
			.word	SEQ_S_CYC_3,to_breath1

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_SBREATH1
			.byte	0
			
			.byte	ANI_GOTO
			.word 	s_breath1_loop

to_breath1:
			.byte	ANI_INTTOSEQ,16
			.word	SEQ_SBREATH1
			.byte	0

			.byte	ANI_GOTO
			.word	s_breath1_loop
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl	s_breath_dbg_anim_int
			.byte	SCRIPT
s_breath_dbg_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_SBREATH1
			.byte	0

s_breath1_loop_dbg:
			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.2

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.2


			.byte	ANI_GOTO
			.word	s_breath1_loop_dbg
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

#			.globl	s_breath2_anim
#			.byte	SCRIPT
#s_breath2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_GOTO
#			.word	s_breath2_loop
#
#s_breath2_int_to:
#			.byte	ANI_INTTOSEQ,16
#			.word	SEQ_SBREATH3
#			.byte	0
#
s_breath2_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH3

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH3

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath1_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath3_anim_int

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,s_breath5_anim_int

			.byte	ANI_GOTO
			.word	s_breath2_loop
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	s_breath2_anim_int
			.byte	SCRIPT
s_breath2_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_JIFSEQ
			.word	SEQ_SBREATH1,to_breath2

			.byte	ANI_JIFSEQ
			.word	SEQ_S_CYC_3,to_breath2

			.byte	ANI_JIFSEQ
			.word	SEQ_S_CYC_1,to_breath2

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_SBREATH3
			.byte	0
			
			.byte	ANI_GOTO
			.word 	s_breath2_loop

to_breath2:
			.byte	ANI_INTTOSEQ,16
			.word	SEQ_SBREATH3
			.byte	0

			.byte	ANI_GOTO
			.word	s_breath2_loop
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

#			.globl set_pick_1
#			.byte	SCRIPT
#set_pick_1:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_TWOPART
#
#			#.byte	ANI_CODE
#			#.word	set_unint_lowerhalf,1,0,0
#
#			.byte	ANI_NEWSEQ
#			.word 	SEQ_PICK_1
#
#			.byte	ANI_INTSTREAM,40,25
#
#			#.byte	ANI_CODE
#			#.word	set_unint_lowerhalf,0,0,0
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#
#			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# DRIBBLES (shake and bake moves)
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl	dribble_run_cocky_rev
			.byte	SCRIPT
dribble_run_cocky_rev:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_ROTONLY
			#|MODE_ANIDEBUG

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_run_cocky_rev_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_REV1
			.byte	22

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_REV1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.55

			.byte	ANI_NEWSEQ
			.word	SEQ_D_REV1

			.byte	ANI_INTSTREAM,22,12

			.byte	ANI_GOTO
			.word	dribble_run_cocky_rev_hands_done

dribble_run_cocky_rev_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_REV1
			.byte	60

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_REV1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.55

			.byte	ANI_NEWSEQ
			.word	SEQ_D_REV1

			.byte	ANI_INTSTREAM,60,33

dribble_run_cocky_rev_hands_done:			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			#.byte	ANI_GOTO
			#.word	dribble_run_cocky_rev_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_run_cocky
			.byte	SCRIPT
dribble_run_cocky:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			#|MODE_ANIDEBUG

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_run_cocky_r

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_D_COCK_1
			.byte	0

			.byte	ANI_GOTO
			.word	dribble_run_cocky_hands_done

dribble_run_cocky_r:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_D_COCK_1
			.byte	0

dribble_run_cocky_hands_done:
			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_REV1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.55

			.byte	ANI_NEWSEQ
			.word	SEQ_D_COCK_1

			.byte	ANI_GOTO
			.word	dribble_run_cocky_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_run_cocky_2
			.byte	SCRIPT
dribble_run_cocky_2:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float 0.0,0.0,5.0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_D_COCK_2
			.byte	0

dribble_run_cocky_2_loop:
			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_REV1,0,0

			.byte	ANI_STREAMTOEND
			.float 0.65

			.byte	ANI_NEWSEQ
			.word	SEQ_D_COCK_2

			.byte	ANI_GOTO
			.word	dribble_run_cocky_2_loop

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl	around_back_twice
			.byte	SCRIPT
around_back_twice:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,around_back_twice_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR7_L
			.byte	8

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR7_L,0,0

			.byte	ANI_GOTO
			.word	around_back_twice_hands_done

around_back_twice_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR7_R
			.byte	8

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR7_R,0,0

around_back_twice_hands_done:			
			.byte	ANI_SETFRICTION
			.float	0.80

			.byte	ANI_INTSTREAM,31,19

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR7_R,around_back_twice_shake_r1

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	around_back_twice_shake_cont_1

around_back_twice_shake_r1:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

around_back_twice_shake_cont_1:
			.byte	ANI_INTSTREAM,45,29

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR7_R,around_back_twice_shake_r2

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	around_back_twice_shake_cont_2

around_back_twice_shake_r2:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

around_back_twice_shake_cont_2:
			.byte	ANI_INTSTREAM,48,29

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR7_R,around_back_twice_shake_r3

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	around_back_twice_shake_cont_3

around_back_twice_shake_r3:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

around_back_twice_shake_cont_3:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	dribble_turn180_1
			.byte	SCRIPT
dribble_turn180_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_SETFRICTION
			.float	0.48

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_turn180_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_TBCK_L
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_TBCK_L,0,0

			.byte	ANI_GOTO
			.word	dribble_turn180_1_hands_done

dribble_turn180_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_TBCK_R
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_TBCK_R,0,0

dribble_turn180_1_hands_done:			
			.byte	ANI_INTSTREAM,18,9

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,18,9

			.byte	ANI_SETVELS
			.float	0.0,0.0,-1.3

			.byte	ANI_STREAMTOEND
			.float	0.35

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; 

			.globl	dribble_slide_right_r_anim
			.byte	SCRIPT
dribble_slide_right_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SLDR_R

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SLDR_R,0,0

			.byte	ANI_STREAMTOEND
			.float	0.35

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	dribble_slide_right_l_anim
			.byte	SCRIPT
dribble_slide_right_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SLDR_L

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SLDR_L,0,0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_RUN2_L
			.byte	0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl dribble_slide_left_r_anim
			.byte	SCRIPT
dribble_slide_left_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SLDL_R

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SLDL_R,0,0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	dribble_slide_left_l_anim
			.byte	SCRIPT
dribble_slide_left_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SLDL_L

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SLDL_L,0,0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_RUN2_L
			.byte	0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl between_legrun
			.byte	SCRIPT
between_legrun:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float 	0.1
			
			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,between_legrun_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLG1_L
			.byte	7

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLG1_R,0,0

# to the right
			.byte	ANI_INTSTREAM,5,3

 			.byte	ANI_CODE
			.word	shake_defender,1,0,0

			.byte	ANI_GOTO
			.word 	between_legrun_hands_done

between_legrun_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLG1_R
			.byte	7

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLG1_R,0,0

# to the left
			.byte	ANI_INTSTREAM,5,3

 			.byte	ANI_CODE
			.word	shake_defender,0,0,0

between_legrun_hands_done:
			.byte	ANI_INTSTREAM,16,9

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,28

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	between_legrun_anim_l
			.byte	SCRIPT
between_legrun_anim_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.1

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_D_BLG1_L
			.byte	7
			
			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLG1_L,0,0

# to the left
			.byte	ANI_INTSTREAM,5,3

 			.byte	ANI_CODE
			.word	shake_defender,1,0,0

			.byte	ANI_INTSTREAM,16,9

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,28

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
# dribble behind the back twice	#
#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_to_stop_1
			.byte	SCRIPT
dribble_to_stop_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_to_stop_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_DBB1_L
			.byte	14

			.byte	ANI_GOTO
			.word 	dribble_to_stop_1_hands_done

dribble_to_stop_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_DBB1_R
			.byte	14

dribble_to_stop_1_hands_done:			
			.byte	ANI_INTSTREAM,85,40

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
# Dribble Shakes #
#;;;;;;;;;;;;;;;;;;;;;

			.globl	spinback_1
			.byte	SCRIPT
spinback_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.055

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spinback_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN1L
			.byte	10

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN1L,0,0

			.byte	ANI_GOTO
			.word	spinback_1_hands_done

spinback_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN1R
			.byte	10

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN1R,0,0

spinback_1_hands_done:			
			.byte	ANI_INTSTREAM,12,12

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN1R,spinback_1_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	spinback_1_shake_done

spinback_1_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spinback_1_shake_done:
			.byte	ANI_INTSTREAM,12,12

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	spinback_2
			.byte	SCRIPT
spinback_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.05

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spinback_2_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN4L
			.byte	13

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN4L,0,0

			.byte	ANI_GOTO
			.word	spinback_2_hands_done

spinback_2_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN4R
			.byte	13

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN4R,0,0

spinback_2_hands_done:			
			.byte	ANI_INTSTREAM,21,21

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN4R,spinback_2_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	spinback_2_shake_done

spinback_2_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spinback_2_shake_done:
			.byte	ANI_INTSTREAM,20,18

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl cross_over_5
			.byte	SCRIPT
cross_over_5:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,cross_over_5_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR5_L
			.byte	15

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR5_L,0,0

			.byte	ANI_GOTO
			.word	cross_over_5_hands_done

cross_over_5_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR5_R
			.byte	15

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR5_R,0,0

cross_over_5_hands_done:			
			.byte	ANI_SETFRICTION
			.float 	0.06
			#.float 	0.80

			.byte	ANI_INTSTREAM,25,18

			.byte	ANI_CODE
			.word 	plyr_speech,HUH_1,0,0

			#.byte	ANI_INTSTREAM,3,2

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR5_R,cross_over_5_shake_r

			.byte	ANI_CODE
			.word	shake_defender,1,0,0

			.byte	ANI_GOTO
			.word 	cross_over_5_shake_done

cross_over_5_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,0,0,0

cross_over_5_shake_done:
			.byte	ANI_INTSTREAM,23,19

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,32

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl cross_over_6
			.byte	SCRIPT
cross_over_6:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,cross_over_6_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR6_L
			.byte	15

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR6_L,0,0

			.byte	ANI_GOTO
			.word	cross_over_6_hands_done

cross_over_6_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR6_R
			.byte	15

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR6_R,0,0

cross_over_6_hands_done:			
			.byte	ANI_SETFRICTION
			.float	0.05
			#.float	0.80

			.byte	ANI_INTSTREAM,21,19

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			#.byte	ANI_INTSTREAM,3,2

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR6_R,cross_over_6_shake_r

			.byte	ANI_CODE
			.word	shake_defender,1,0,0

			.byte	ANI_GOTO
			.word	cross_over_6_shake_done

cross_over_6_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,0,0,0

cross_over_6_shake_done:
			.byte	ANI_INTSTREAM,15,13
			#.byte	ANI_INTSTREAM,9,9

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	cross_over_3
			.byte	SCRIPT
cross_over_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,cross_over_3_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR3_L
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR3_L,0,0

			.byte	ANI_GOTO
			.word	cross_over_3_hands_done

cross_over_3_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR3_R
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR3_R,0,0

cross_over_3_hands_done:			
			.byte	ANI_SETFRICTION
			.float 	0.06
			#.float 0.80

			.byte	ANI_INTSTREAM,18,14

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR3_R,cross_over_3_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	cross_over_3_shake_done

cross_over_3_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

cross_over_3_shake_done:
			.byte	ANI_INTSTREAM,10,7
			.byte	ANI_INTSTREAM,9,7

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl cross_over_1
			.byte	SCRIPT
cross_over_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,cross_over_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR1_L
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR1_L,0,0

			.byte	ANI_GOTO
			.word	cross_over_1_hands_done

cross_over_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_OVR1_R
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_OVR1_R,0,0

cross_over_1_hands_done:			
			.byte	ANI_SETFRICTION
			.float		0.055
			#.float		0.80

			.byte	ANI_INTSTREAM,9,4
			.byte	ANI_INTSTREAM,20,13

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR1_R,cross_over_1_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	cross_over_1_shake_done

cross_over_1_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

cross_over_1_shake_done:
			.byte	ANI_INTSTREAM,19,15

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl behindback_spin_1
			.byte	SCRIPT
behindback_spin_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,behindback_spin_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SP_T_L
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SP_T_L,0,0

			.byte	ANI_GOTO
			.word	behindback_spin_1_hands_done

behindback_spin_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SP_T_R
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SP_T_R,0,0

behindback_spin_1_hands_done:			
			.byte	ANI_SETFRICTION
			.float	0.2
			#.float	0.4

			.byte	ANI_INTSTREAM,31,17

			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SP_T_R,behindback_spin_1_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	behindback_spin_1_shake_done

behindback_spin_1_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

behindback_spin_1_shake_done:
			.byte	ANI_INTSTREAM,39,21

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,17

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl between_both_legs_1
			.byte	SCRIPT
between_both_legs_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.05

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,between_both_legs_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLEG_L
			.byte	20

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLEG_L,0,0

			.byte	ANI_GOTO
			.word	between_both_legs_1_hands_done

between_both_legs_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLEG_R
			.byte	20

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLEG_R,0,0

between_both_legs_1_hands_done:			
			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,29,15

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR1_R,between_both_legs_1_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	between_both_legs_1_shake_done

between_both_legs_1_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

between_both_legs_1_shake_done:
			.byte	ANI_INTSTREAM,37,19

			.byte	ANI_INTSTREAM,9,5

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_PREP_XITION
			.word	3,27
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl spin_4
			.byte	SCRIPT
spin_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.035

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spin_4_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SPIN5L
			.byte	3

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN5L,0,0

			.byte	ANI_GOTO
			.word 	spin_4_hands_done

spin_4_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SPIN5R
			.byte	3

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN5R,0,0

spin_4_hands_done:			
			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_SETFRICTION
			.float	0.035

			.byte	ANI_INTSTREAM,13,27

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN5R,spin_4_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	spin_4_shake_done

spin_4_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spin_4_shake_done:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN5R,spin_4_thack_l

			.byte	ANI_TURNHACK
			.word	0

			.byte	ANI_GOTO
			.word 	spin_4_thack_done

spin_4_thack_l:
			.byte	ANI_TURNHACK
			.word	0

spin_4_thack_done:
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,33

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	spin_3
			.byte	SCRIPT
spin_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.045

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spin_3_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SPIN3L
			.byte	3

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN3L,0,0

			.byte	ANI_GOTO
			.word 	spin_3_hands_done

spin_3_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SPIN3R
			.byte	3

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN3R,0,0

spin_3_hands_done:			
			.byte	ANI_CODE
			.word 	plyr_speech,HUH_1,0,0

			#.byte	ANI_INTSTREAM,13,27
			.byte	ANI_INTSTREAM,11,25

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN3R,spin_3_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word	spin_3_shake_done

spin_3_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spin_3_shake_done:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN3R,spin_3_thack_l

			.byte	ANI_TURNHACK
			.word	0

			.byte	ANI_GOTO
			.word 	spin_3_thack_done

spin_3_thack_l:
			.byte	ANI_TURNHACK
			.word	0
			#.word 900

spin_3_thack_done:
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word 	3,33

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl spin_2
			.byte	SCRIPT
spin_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spin_2_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SPN1_L
			.byte	15

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2L,0,0

			.byte	ANI_GOTO
			.word	spin_2_hands_done

spin_2_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SPN1_R
			.byte	15

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2R,0,0

spin_2_hands_done:			
			.byte	ANI_CODE
			.word 	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,41,27

			.byte	ANI_JIFSEQ
			.word	SEQ_D_OVR1_R,spin_2_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word 	spin_2_shake_done

spin_2_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spin_2_shake_done:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN2R,spin_2_thack_l

			.byte	ANI_TURNHACK
			.word	124

			.byte	ANI_GOTO
			.word 	spin_2_thack_done

spin_2_thack_l:
			.byte	ANI_TURNHACK
			.word	900

spin_2_thack_done:
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word 	3,33

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl spin_1
			.byte	SCRIPT
spin_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.045

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spin_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN2L
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2L,0,0

			.byte	ANI_GOTO
			.word spin_1_hands_done

spin_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN2R
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2R,0,0

spin_1_hands_done:			
			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,46,27

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN2R,spin_1_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word spin_1_shake_done

spin_1_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spin_1_shake_done:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,18

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# Greg Cutler
			.globl spin_6
			.byte	SCRIPT
spin_6:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.025

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spin_6_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_JUK7_L
			.byte	7

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2L,0,0

			.byte	ANI_GOTO
			.word spin_6_hands_done

spin_6_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_JUK7_R
			.byte	7

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2R,0,0

spin_6_hands_done:			
			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,37,42

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN2R,spin_6_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word spin_6_shake_done

spin_6_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spin_6_shake_done:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	3,28

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# Greg Cutler
			.globl spin_5
			.byte	SCRIPT
spin_5:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_SETFRICTION
			.float	0.04

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,spin_5_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_JUK8_R
			.byte	6

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2L,0,0

			.byte	ANI_GOTO
			.word spin_5_hands_done

spin_5_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_JUK8_L
			.byte	6

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2R,0,0

spin_5_hands_done:			
			.byte	ANI_CODE
			.word	plyr_speech,HUH_1,0,0

			.byte	ANI_INTSTREAM,40,40

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN2R,spin_5_shake_r

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_GOTO
			.word spin_5_shake_done

spin_5_shake_r:
			.byte	ANI_CODE
			.word	shake_defender,1,0,0

spin_5_shake_done:
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	5,18

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl	between_leg_spin_r
			.byte	SCRIPT
between_leg_spin_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLEG_R
			.byte	20
			
			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLEG_R,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_D_BLEG_R
			.byte	ANI_SKIPFRAMES,20

# to the left
			.byte	ANI_SETVELS
			.float	0.209,0.0,0.395
			.byte	ANI_INTSTREAM,29,15
#			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

# stop movement
# and go into spin animation
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN2L
			.byte	0

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2L,0,0

			.byte	ANI_SETVELS
			.float	-0.309,0.0,0.395

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SPIN2L

			.byte	ANI_INTSTREAM,53,23

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
			
#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl between_leg_spin_l
			.byte	SCRIPT
between_leg_spin_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLEG_L
			.byte	20
			
			.byte	ANI_NEWSEQ
			.word	SEQ_D_BLEG_L
			.byte	ANI_SKIPFRAMES,20

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLEG_L,0,0

# to the left
			.byte	ANI_SETVELS
			.float	-0.209,0.0,0.395
			.byte	ANI_INTSTREAM,29,15

			.byte	ANI_CODE
			.word	shake_defender,1,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
# stop movement
# and go into spin animation
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SPIN2R
			.byte	0

			.byte	ANI_SETVELS
			.float	0.359,0.0,0.395

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SPIN2R

			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_SPIN2R,0,0
	
			.byte	ANI_INTSTREAM,53,23

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl between_leg_anim_r
			.byte	SCRIPT
between_leg_anim_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLEG_R
			.byte	26
			
			.byte	ANI_NEWSEQ
			.word	SEQ_D_BLEG_R
			.byte	ANI_SKIPFRAMES,26
			
			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLEG_R,0,0

# to the left
			.byte	ANI_SETVELS
			.float	0.209,0.0,0.395
			.byte	ANI_INTSTREAM,24,15

			.byte	ANI_CODE
			.word	shake_defender,1,0,0

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; shake and bake move

			.globl between_leg_anim_l
			.byte	SCRIPT
between_leg_anim_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_CODE
			.word	ghosting,0,1,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BLEG_L
			.byte	26
			
			.byte	ANI_NEWSEQ
			.word	SEQ_D_BLEG_L
			.byte	ANI_SKIPFRAMES,26
			
			.byte	ANI_CODE
			.word	one_on_one_move_speech,SEQ_D_BLEG_L,0,0

# to the right
			.byte	ANI_SETVELS
			.float	-0.209,0.0,0.395
			.byte	ANI_INTSTREAM,24,15

			.byte	ANI_CODE
			.word	shake_defender,0,0,0

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;; 

			.globl dribble_switch2_r
			.byte	 SCRIPT
dribble_switch2_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			#|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

#			.byte	ANI_SETFRICTION
#			.float	0.05

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.85

			#.byte	ANI_CODE
			#.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_CHG2_R
			.byte	0

			.byte	ANI_NEWSEQ
			.word	SEQ_D_CHG2_R
			.byte	ANI_SKIPFRAMES,0
			
			.byte	ANI_INTSTREAM,3,3
			
#			.byte	ANI_ZEROVELS
			
			.byte	ANI_STREAMTOEND
			.float	0.60

#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_D_RUN2_L
#			.byte	0

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR

#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_D_RUN2_L
#			.byte	AA_PROPFRAME

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl dribble_switch2_l
			.byte	 SCRIPT
dribble_switch2_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			#|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.85

			#.byte	ANI_CODE
			#.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_CHG2_L
			.byte	0

			.byte	ANI_NEWSEQ
			.word	SEQ_D_CHG2_L
			.byte	ANI_SKIPFRAMES,0
			
			.byte	ANI_INTSTREAM,3,3
			
			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_CLRFLAG
			.word	PF_FUNKY_DRIBBLE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
# player is bumping into defender
			.globl	dribble_backin_2
			.byte	SCRIPT
dribble_backin_2:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_backin_2_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_BKN2_L
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_backin_2_loop_frames

dribble_backin_2_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_BKN2_R
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_backin_2_loop_frames

dribble_backin_2_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_BKN2_R,dribble_backin_2_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_BKN2_L

			.byte	ANI_GOTO
			.word	dribble_backin_2_loop_frames

dribble_backin_2_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_BKN2_R

dribble_backin_2_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.65

			.byte	ANI_GOTO
			.word	dribble_backin_2_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
# player is bumping into defender
			.globl	dribble_backin_1
			.byte	SCRIPT
dribble_backin_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			#.byte	ANI_SETVELS
			#.float	0.0,0.0,0.05

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_backin_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BKN1_L
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_backin_1_hands_done

dribble_backin_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_BKN1_R
			.byte	0

dribble_backin_1_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_BKN1_R,dribble_backin_1_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_BKN1_L

			.byte	ANI_GOTO
			.word	dribble_backin_1_loop_frames

dribble_backin_1_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_BKN1_R

dribble_backin_1_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_GOTO
			.word	dribble_backin_1_hands_done
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_backin_1_int
			.byte	SCRIPT
dribble_backin_1_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_backin_1_r_int

			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_BKN1_L
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_backin_1_hands_done
dribble_backin_1_r_int:
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_BKN1_R
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_backin_1_hands_done

###########################################################
			.globl	dribble_run_1
			.byte	SCRIPT
dribble_run_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_run_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_RUN2_L
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_run_1_hands_done

dribble_run_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_RUN2_R
			.byte	0

dribble_run_1_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_RUN2_R,dribble_run_1_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_RUN2_L

			.byte	ANI_GOTO
			.word	dribble_run_1_loop_frames

dribble_run_1_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_RUN2_R

dribble_run_1_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.6122449

			.byte	ANI_GOTO
			.word	dribble_run_1_hands_done
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_run_1_int
			.byte	SCRIPT
dribble_run_1_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_run_1_r_int

			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_RUN2_L
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_run_1_hands_done
dribble_run_1_r_int:
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_RUN2_R
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_run_1_hands_done

###########################################################
			.globl	dribble_walk_1
			.byte	SCRIPT
dribble_walk_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_walk_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_WLK1_L
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_walk_1_hands_done

dribble_walk_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_WLK1_R
			.byte	0

dribble_walk_1_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_WLK1_R,dribble_walk_1_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_WLK1_L

			.byte	ANI_GOTO
			.word	dribble_walk_1_loop_frames

dribble_walk_1_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_WLK1_R

dribble_walk_1_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.65

			.byte	ANI_GOTO
			.word	dribble_walk_1_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_walk_1_int
			.byte	SCRIPT
dribble_walk_1_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_walk_1_r_int

			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_WLK1_L
			.byte	AA_VAR_FRAME

			.byte	ANI_STREAMTOEND
			.float	0.65

			.byte	ANI_GOTO
			.word 	dribble_walk_1_hands_done

dribble_walk_1_r_int:
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_WLK1_R
			.byte	AA_VAR_FRAME

			.byte	ANI_STREAMTOEND
			.float	0.7122449

			.byte	ANI_GOTO
			.word 	dribble_walk_1_hands_done

#;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_run_left
			.byte	SCRIPT
dribble_run_left:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			#|MODE_ANIDEBUG

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

#;;;;;; FIX!!! make sure this code needs to be commented out
			#.byte	ANI_CODE_BNZ
			#.word	ball_in_righthand,0,0,0,dribble_run_left_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SID1_L
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.7

			.byte	ANI_GOTO
			.word 	dribble_run_left_hands_done

dribble_run_left_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SID1_R
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.7

dribble_run_left_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_SID1_R,dribble_run_left_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SID1_L

			.byte	ANI_GOTO
			.word	dribble_run_left_loop_frames

dribble_run_left_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_SID1_R

dribble_run_left_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.7

			.byte	ANI_GOTO
			.word	dribble_run_left_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_run_right
			.byte	SCRIPT
dribble_run_right:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			#|MODE_ANIDEBUG

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0
			
#;;;;;; FIX!!! make sure this goto needs to be here
			.byte	ANI_GOTO
			.word 	dribble_run_right_r

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_run_right_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SID1_L
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.7

			.byte	ANI_GOTO
			.word 	dribble_run_right_hands_done

dribble_run_right_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_SID1_R
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.7

dribble_run_right_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_SID1_R,dribble_run_right_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SID1_L

			.byte	ANI_GOTO
			.word	dribble_run_right_loop_frames

dribble_run_right_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_SID1_R

dribble_run_right_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.7

			.byte	ANI_GOTO
			.word	dribble_run_right_hands_done

			.byte	ANI_END

#####################

			.globl	dribble_run_2
			.byte	SCRIPT
dribble_run_2:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			#|MODE_ANIDEBUG

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_JIFSEQ
			.word	SEQ_RUN_R,SEQ_RUN_L,to_dribrun_2_from_run

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	13

			.byte	ANI_GOTO
			.word 	dribble_run_2_intseq_done

to_dribrun_2_from_run:
			.byte	ANI_INTTOSEQH,1
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	AA_PROPFRAME

dribble_run_2_intseq_done:
			.byte	ANI_VSTREAMTOEND
			.float	0.9

dribble_run_2_loop_frames:
			.byte	ANI_NEWSEQH
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L

			.byte	ANI_VSTREAMTOEND
			.float	0.9

			.byte	ANI_GOTO
			.word	dribble_run_2_loop_frames

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_run_2_int
			.byte	SCRIPT
dribble_run_2_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			#|MODE_ANIDEBUG

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_INTTOSEQH,AA_VAR_COUNT
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	AA_VAR_FRAME
			
			.byte	ANI_VSTREAMTOEND
			.float	0.9

			.byte	ANI_GOTO
			.word 	dribble_run_2_loop_frames

######################################################################
#STANDING DRIBBLES
######################################################################
			.globl	dribble_stand_cocky
			.byte	SCRIPT
dribble_stand_cocky:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_stand_cocky_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_CYCL_1
			.byte	5

			.byte	ANI_GOTO
			.word	dribble_stand_cocky_done

dribble_stand_cocky_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_CYCL_1
			.byte	16

dribble_stand_cocky_done:
			.byte	ANI_STREAMTOEND
			.float	2.5

			.byte	ANI_GOTO
			.word	dribble_snippet_1

			.byte	ANI_END

####################################################

			.globl	dribble_stand_backin
			.byte	SCRIPT
dribble_stand_backin:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_stand_backin_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STD5_L
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_stand_backin_loop_frames

dribble_stand_backin_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STD5_R
			.byte	0

			.byte	ANI_GOTO
			.word	dribble_stand_backin_loop_frames

dribble_stand_backin_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_STD5_R,dribble_stand_backin_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_STD5_L

			.byte	ANI_GOTO
			.word	dribble_stand_backin_loop_frames

dribble_stand_backin_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_STD5_R

dribble_stand_backin_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_GOTO
			.word	dribble_stand_backin_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_stand_6
			.byte	SCRIPT
dribble_stand_6:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_JIFSEQ
			.word	SEQ_D_SPIN3R,SEQ_D_SPIN3L,dribble_stand_6_from_spin3

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_D_STD6_R,SEQ_D_STD6_L
			.byte	0

			.byte	ANI_GOTO
			.word	dribble_stand_6_loop_frames

dribble_stand_6_from_spin3:
			.byte	ANI_INTTOSEQH,8
			.word	SEQ_D_STD6_R,SEQ_D_STD6_L
			.byte	0

			.byte	ANI_GOTO
			.word	dribble_stand_6_loop_frames

dribble_stand_6_hands_done:			
			.byte	ANI_NEWSEQH
			.word	SEQ_D_STD6_R,SEQ_D_STD6_L

dribble_stand_6_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	1.6

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,dribble_snippet_2

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,dribble_stand_cocky

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,dribble_snippet_1

			.byte	ANI_GOTO
			.word	dribble_stand_6_hands_done

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_stand_1
			.byte	SCRIPT
dribble_stand_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_stand_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STND_L
			.byte	0

			.byte	ANI_GOTO
			.word 	dribble_stand_1_hands_done

dribble_stand_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STND_R
			.byte	0

dribble_stand_1_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_STND_R,dribble_stand_1_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_STND_L

			.byte	ANI_GOTO
			.word	dribble_stand_1_loop_frames

dribble_stand_1_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_STND_R

dribble_stand_1_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_GOTO
			.word	dribble_stand_1_hands_done
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_stand_1_int
			.byte	SCRIPT
dribble_stand_1_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_stand_1_r_int

			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_STND_L
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_stand_1_hands_done
dribble_stand_1_r_int:
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_STND_R
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_stand_1_hands_done

## dribble Snippets ##
			.globl	dribble_snippet_2
			.byte	SCRIPT
dribble_snippet_2:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_snippet_2_r

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_D_SNP2_L
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	1.6

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_D_STD6_L
			.byte	11

			.byte	ANI_GOTO
			.word 	dribble_stand_6_loop_frames
			
dribble_snippet_2_r:
			.byte	ANI_INTTOSEQ,7
			.word	SEQ_D_SNP2_R
			.byte	2

			.byte	ANI_STREAMTOEND
			.float	1.4

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_D_STD6_R
			.byte	11

			.byte	ANI_GOTO
			.word 	dribble_stand_6_loop_frames

dribble_snippet_2_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_SNP2_R,dribble_snippet_2_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SNP2_L

			.byte	ANI_GOTO
			.word	dribble_snippet_2_loop_frames

dribble_snippet_2_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_SNP2_R

dribble_snippet_2_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_GOTO
			.word	dribble_snippet_2_hands_done
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_snippet_1
			.byte	SCRIPT
dribble_snippet_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_snippet_1_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SNP1_L
			.byte	0

			.byte	ANI_INTSTREAM,96,115
			#.byte	ANI_STREAMTOEND
			#.float	1.2

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_D_STD6_L
			.byte	19

			.byte	ANI_GOTO
			.word 	dribble_stand_6_loop_frames


dribble_snippet_1_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_D_SNP1_R
			.byte	0

			.byte	ANI_INTSTREAM,96,115

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_D_STD6_R
			.byte	19

			.byte	ANI_GOTO
			.word 	dribble_stand_6_loop_frames
			
dribble_snippet_1_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_SNP1_R,dribble_snippet_1_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_SNP1_L

			.byte	ANI_GOTO
			.word	dribble_snippet_1_loop_frames

dribble_snippet_1_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_SNP1_R

dribble_snippet_1_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	1.2

			.byte	ANI_GOTO
			.word	dribble_snippet_1_hands_done
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_stand_2
			.byte	SCRIPT
dribble_stand_2:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_stand_2_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STD2_L
			.byte	17

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_GOTO
			.word 	dribble_stand_2_hands_done

dribble_stand_2_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STD2_R
			.byte	17

			.byte	ANI_STREAMTOEND
			.float	1.0

dribble_stand_2_hands_done:			
			.byte	ANI_JIFSEQ
			.word	SEQ_D_STD2_R,dribble_stand_2_r_loop

			.byte	ANI_NEWSEQ
			.word	SEQ_D_STD2_L

			.byte	ANI_GOTO
			.word	dribble_stand_2_loop_frames

dribble_stand_2_r_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_STD2_R

dribble_stand_2_loop_frames:
			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,dribble_stand_2_hands_done

			.byte	ANI_GOTO
			.word	dribble_snippet_1

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_stand_2_int
			.byte	SCRIPT
dribble_stand_2_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_stand_2_r_int

			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_STD2_L
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_stand_2_hands_done
dribble_stand_2_r_int:
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_D_STD2_R
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word 	dribble_stand_2_hands_done

############################################################
			.globl	dribble_stand_anim_r
			.byte	SCRIPT
dribble_stand_anim_r:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0
dribble_stand_loop_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_STND_R

			.byte	ANI_INTSTREAM,37,30

			.byte	ANI_GOTO
			.word	dribble_stand_loop_r

			.globl	dribble_stand_anim_int_r
			.byte	SCRIPT
dribble_stand_anim_int_r:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STND_R
			.byte	17

#			.byte	ANI_INTSTREAM,37,30
			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_GOTO
			.word	dribble_stand_loop_r
##
			.globl	dribble_stand_anim_l
			.globl	dribble_stand_anim_int_l
			.byte	SCRIPT
dribble_stand_anim_l:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0
dribble_stand_loop_l:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_STND_L
			
			.byte	ANI_INTSTREAM,37,30
			.byte	ANI_GOTO
			.word	dribble_stand_loop_l

			.byte	SCRIPT
dribble_stand_anim_int_l:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_STND_L
			.byte	0
			.byte	ANI_INTSTREAM,37,30
			
			.byte	ANI_GOTO
			.word	dribble_stand_loop_l

			.globl dribble_steal_react
			.byte	SCRIPT
dribble_steal_react:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING
			
			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dribble_steal_react_r

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_D_REAC_L
			.byte	5
			
			.byte	ANI_GOTO
			.word 	dribble_react_hands_done
	
dribble_steal_react_r:
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_D_REAC_R
			.byte	5
			
dribble_react_hands_done:
			.byte	ANI_SETFRICTION
			.float	0.8
			
			.byte	ANI_CODE
			.word	plyr_speech,HUH_2,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
			
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	dribble_intostand_r_1
			.byte	SCRIPT
dribble_intostand_r_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			
			.byte	ANI_SETFRICTION
			.float	0.08
			#.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_TS1_R
			.byte	8

			.byte	ANI_NEWSEQ
			.word	SEQ_D_TS1_R
			.byte	ANI_SKIPFRAMES,8
			
			.byte	ANI_INTSTREAM,57,32

#			.byte	ANI_STREAMTOEND
#			.float	.75

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_D_STD2_R
			.byte	0

			.byte	ANI_NEWSEQ
			.word	SEQ_D_STD2_R

			.byte	ANI_STREAMTOEND
			.float	.70

			.byte	ANI_GOTO
			.word	dribble_stand_2

			.globl	dribble_intostand_l_1
			.byte	SCRIPT
dribble_intostand_l_1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.08
#			.byte	ANI_ZEROVELS

#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_TS1_L
			.byte	8

			.byte	ANI_NEWSEQ
			.word	SEQ_D_TS1_L
			.byte	ANI_SKIPFRAMES,8

			.byte	ANI_INTSTREAM,57,32

#			.byte	ANI_STREAMTOEND
#			.float	.75

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,7
#			.word	SEQ_D_STND_L
			.word	SEQ_D_STD2_L
			.byte	0

			.byte	ANI_NEWSEQ
			.word	SEQ_D_STD2_L
#			.word	SEQ_D_STND_L

			.byte	ANI_STREAMTOEND
			.float	.70

			.byte	ANI_GOTO
			.word	dribble_stand_2

			.globl	dribble_spider
			.byte	SCRIPT
dribble_spider:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			
			.byte	ANI_CODE
			.word	ghosting,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_D_SPIDER
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.75

dribble_spider_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_D_SPIDER

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_GOTO
			.word	dribble_spider_loop

##########################################################
# CATCHING																							 #
##########################################################
			.globl	catch_run_spin_behind_r
			.byte	SCRIPT
catch_run_spin_behind_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			#.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			#.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SPN1_R
			.byte	0

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

#			.byte	ANI_CLRFLAG
#			.word	PF_CATCHING

			.byte	ANI_INTSTREAM,15,10

#			.byte	ANI_SETFRICTION
#			.float	0.1

			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			#.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.6

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl	catch_run_spin_behind_l
			.byte	SCRIPT
catch_run_spin_behind_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			#.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SPN1_L
			.byte	0

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

#			.byte	ANI_CLRFLAG
#			.word	PF_CATCHING

			.byte	ANI_INTSTREAM,15,10

#			.byte	ANI_SETFRICTION
#			.float	0.1

			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			#.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.6

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_overhead_forward
			.byte	SCRIPT
catch_stand_overhead_forward:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_OVR_HD
			.byte	5

			.byte	ANI_INTSTREAM,6,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_overhead_forward
			.byte	SCRIPT
catch_run_overhead_forward:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_OVR_HD
			.byte	5

			.byte	ANI_INTSTREAM,6,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	0.9

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_forward_1
			.byte	SCRIPT
catch_run_forward_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART
			
			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

#FIX ANIM!!!
			.byte	ANI_NEWSEQ
			.word	SEQ_ZERO

			#.byte	ANI_INTSTREAM,30,7
			.byte	ANI_INTSTREAM,8,AA_BALL_AIRTIME

# ball is caught

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_forward_1
			.byte	SCRIPT
catch_stand_forward_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			
			.byte	ANI_SETFLAG
			.word	PF_CATCHING

#FIX ANIM!!!
			.byte	ANI_NEWSEQ
			.word	SEQ_ZERO

			.byte	ANI_INTSTREAM,8,AA_BALL_AIRTIME

# ball is caught

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl catch_run_forward_2
			.byte	SCRIPT
catch_run_forward_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_FWD1_B
			.byte	9

			.byte	ANI_INTSTREAM,19,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.8

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl catch_stand_forward_2
			.byte	SCRIPT
catch_stand_forward_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_C_FWD1_B
			.byte	9

			.byte	ANI_INTSTREAM,19,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	0.8

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_behind_r
			.byte	SCRIPT
catch_run_behind_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_BCK4_R
			.byte	25

			.byte	ANI_INTSTREAM,18,AA_BALL_AIRTIME

# ball is caught
			#.byte	ANI_INTSTREAM,22,12

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_behind_l
			.byte	SCRIPT
catch_run_behind_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_BCK4_L
			.byte	25

			.byte	ANI_INTSTREAM,18,AA_BALL_AIRTIME

			#.byte	ANI_INTSTREAM,22,12

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_behind_r
			.byte	SCRIPT
catch_stand_behind_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_BCK4_R
			.byte	0

			.byte	ANI_INTSTREAM,43,AA_BALL_AIRTIME

# ball is caught

			.byte	ANI_INTSTREAM,22,12
			#.byte	ANI_STREAMTOEND
			#.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_behind_l
			.byte	SCRIPT
catch_stand_behind_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_BCK4_L
			.byte	0

			.byte	ANI_INTSTREAM,43,AA_BALL_AIRTIME

# ball is caught

			.byte	ANI_INTSTREAM,22,12
			#.byte	ANI_STREAMTOEND
			#.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl	catch_stand_right_4
			.byte	SCRIPT
catch_stand_right_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID5_R
			.byte	12

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,9,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_left_4
			.byte	SCRIPT
catch_stand_left_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID5_L
			.byte	12

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,9,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_right_4
			.byte	SCRIPT
catch_run_right_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID5_R
			.byte	12

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,9,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_left_4
			.byte	SCRIPT
catch_run_left_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID5_L
			.byte	12

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME
			
			.byte	ANI_INTSTREAM,9,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl	catch_stand_right_3
			.byte	SCRIPT
catch_stand_right_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_SID2_R
			.byte	8

			.byte	ANI_INTSTREAM,8,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_left_3
			.byte	SCRIPT
catch_stand_left_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_SID2_L
			.byte	8

			.byte	ANI_INTSTREAM,8,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_right_3
			.byte	SCRIPT
catch_run_right_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID2_R
			.byte	8

			.byte	ANI_INTSTREAM,8,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_left_3
			.byte	SCRIPT
catch_run_left_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID2_L
			.byte	8

			.byte	ANI_INTSTREAM,8,AA_BALL_AIRTIME

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_right_1
			.byte	SCRIPT
catch_stand_right_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SID6_R
			.byte	21

			.byte	ANI_INTSTREAM,14,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,8,8

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_left_1
			.byte	SCRIPT
catch_stand_left_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SID6_L
			.byte	21

			.byte	ANI_INTSTREAM,14,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,8,8

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_right_1
			.byte	SCRIPT
catch_run_right_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SID6_R
			.byte	21

			.byte	ANI_INTSTREAM,14,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,8,8

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_left_1
			.byte	SCRIPT
catch_run_left_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SID6_L
			.byte	21

			.byte	ANI_INTSTREAM,14,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,8,8

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_right_2
			.byte	SCRIPT
catch_stand_right_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_NO_LOOK

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID4_R
			.byte	9

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,6,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_stand_left_2
			.byte	SCRIPT
catch_stand_left_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_NO_LOOK

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID4_L
			.byte	9

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,6,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_right_2
			.byte	SCRIPT
catch_run_right_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID4_R
			.byte	9

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,6,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	catch_run_left_2
			.byte	SCRIPT
catch_run_left_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_SETFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_C_SID4_L
			.byte	9

			.byte	ANI_INTSTREAM,17,AA_BALL_AIRTIME

			.byte	ANI_INTSTREAM,6,3

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING|PF_BOTH_HANDS|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L
			.byte	-106

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

##########################################################
# PASSING																								 #
##########################################################
#;;;;;;;;;;;;;;;;;;;;;
			.globl pass_fullcourt_1
			.byte	SCRIPT
pass_fullcourt_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,pass_fullcourt_1_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_FCP1_L
			.byte	0

			.byte	ANI_GOTO
			.word 	pass_fullcourt_1_cont

pass_fullcourt_1_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_FCP1_R
			.byte	0

pass_fullcourt_1_cont:
			.byte	ANI_INTSTREAM,16,12

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,20,15
	
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float		0.9

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl run_pass_fullcourt_1
			.byte	SCRIPT
run_pass_fullcourt_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,run_pass_fullcourt_1_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_FCP1_L
			.byte	0

			.byte	ANI_GOTO
			.word 	run_pass_fullcourt_1_cont

run_pass_fullcourt_1_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_FCP1_R
			.byte	0

run_pass_fullcourt_1_cont:
			.byte	ANI_INTSTREAM,16,12

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,25,18
	
			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl pass_forward_2
			.byte	SCRIPT
pass_forward_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,pass_forward_2_r

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_FWD1_L
			.byte	24

			.byte	ANI_GOTO
			.word 	pass_forward_2_cont

pass_forward_2_r:
			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_FWD1_R
			.byte	24

pass_forward_2_cont:
			.byte	ANI_INTSTREAM,3,3

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,16,8

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl run_pass_forward_2
			.byte	SCRIPT
run_pass_forward_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_FWD1_R,SEQ_P_FWD1_L
			.byte	24

			.byte	ANI_INTSTREAM,3,3

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
# 1x is used for no pass found for this quad - default pass to go to
			.globl pass_forward_1x
			.globl pass_forward_1x2

			.globl pass_forward_1
			.byte	SCRIPT
pass_forward_1x2:
pass_forward_1x:
pass_forward_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.6

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_FWD2_B
			.byte	4

			.byte	ANI_INTSTREAM,8,4

			.byte	ANI_CODE
			.word	PassRelease,0,YES,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,25,11

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl run_pass_forward_1
			.byte	SCRIPT
run_pass_forward_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_FWD2_B
			.byte	4

			.byte	ANI_INTSTREAM,8,4

			.byte	ANI_CODE
			.word	PassRelease,0,YES,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,13,6

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-104

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl pass_forward_3
			.byte	SCRIPT
pass_forward_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.6

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_FWD3_B
			.byte	7

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,0,YES,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl run_pass_forward_3
			.byte	SCRIPT
run_pass_forward_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_FWD3_B
			.byte	7

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,0,YES,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl cross_body_side_4
			.byte	SCRIPT
cross_body_side_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	plyr_right_of_me,-1,0,0,cross_body_side_4_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_SID5_R
			.byte	9

			.byte	ANI_GOTO
			.word cross_body_side_4_cont

cross_body_side_4_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_SID5_L
			.byte	9

cross_body_side_4_cont:
			.byte	ANI_INTSTREAM,3,3

			.byte	ANI_CODE
			.word	PassRelease,0,YES,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,35,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_cross_body_side_4
			.byte	SCRIPT
run_cross_body_side_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	plyr_right_of_me,-1,0,0,run_cross_body_side_4_r

			.byte	ANI_INTTOSEQ,6
			.word 	SEQ_P_SID5_R
			.byte	9

			.byte	ANI_GOTO
			.word 	run_cross_body_side_4_cont

run_cross_body_side_4_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_SID5_L
			.byte	9

run_cross_body_side_4_cont:
			.byte	ANI_INTSTREAM,3,3

			.byte	ANI_CODE
			.word	PassRelease,0,YES,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,17,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl cross_body_side_3
			.byte	SCRIPT
cross_body_side_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_SID4_L,SEQ_P_SID4_R
			.byte	22

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_cross_body_side_3
			.byte	SCRIPT
run_cross_body_side_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_SID4_L,SEQ_P_SID4_R
			.byte	22

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl cross_body_side_2
			.byte	SCRIPT
cross_body_side_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_P_SID3_L,SEQ_P_SID3_R
			.byte	9

			.byte	ANI_INTSTREAM,3,2

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,22,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_cross_body_side_2
			.byte	SCRIPT
run_cross_body_side_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_P_SID3_L,SEQ_P_SID3_R
			.byte	9

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

# this pass is going to a double dunker
			.globl	dunk_pass_1
			.byte	SCRIPT
dunk_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,2
			.word	SEQ_LAY4_R,SEQ_LAY4_L
			.byte	26

			.byte	ANI_INTSTREAM,16,5

# now pass the ball
			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

# this pass is going to a double dunker
			.globl	dunk_pass_2
			.byte	SCRIPT
dunk_pass_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_ALLY1
			.byte	0

			.byte	ANI_INTSTREAM,5,5

# now pass the ball
			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl forward_bounce_pass_1
			.byte	SCRIPT
forward_bounce_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.08

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,forward_bounce_pass_1_r

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_FWDB_L
			.byte	2

			.byte	ANI_GOTO
			.word 	forward_bounce_pass_1_cont

forward_bounce_pass_1_r:
			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_FWDB_R
			.byte	2

forward_bounce_pass_1_cont:
			.byte	ANI_INTSTREAM,4,4

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,10,5

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl run_forward_bounce_pass_1
			.byte	SCRIPT
run_forward_bounce_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_FWDB_R,SEQ_P_FWDB_L
			.byte	2

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl bounce_pass_side_1
			.byte	SCRIPT
bounce_pass_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,bounce_pass_side_1_r

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_DIAB_L
			.byte	3

			.byte	ANI_GOTO
			.word 	bounce_pass_side_1_cont

bounce_pass_side_1_r:
			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_DIAB_R
			.byte	3

bounce_pass_side_1_cont:
			.byte	ANI_INTSTREAM,3,4

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	1,1,1

			.byte	ANI_INTSTREAM,3,6

			.byte	ANI_INTSTREAM,2,1

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.80

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl run_bounce_pass_side_1
			.byte	SCRIPT
run_bounce_pass_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_DIAB_R,SEQ_P_DIAB_L
			.byte	3

			.byte	ANI_INTSTREAM,3,4

			.byte	ANI_CODE
			.word	PassRelease,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl cross_body_diag_behind_1
			.byte	SCRIPT
cross_body_diag_behind_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	 ANI_INTTOSEQH,3
			.word	 SEQ_P_BCHS_R,SEQ_P_BCHS_L
			.byte	 0

			.byte	 ANI_INTSTREAM,8,8

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl run_cross_body_diag_behind_1
			.byte	SCRIPT
run_cross_body_diag_behind_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,3
			.word 	SEQ_P_BCHS_R,SEQ_P_BCHS_L
			.byte	0

			.byte	ANI_INTSTREAM,8,8

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl cross_body_side_1
			.byte	SCRIPT
cross_body_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_P_CROS_R,SEQ_P_CROS_L
			.byte	2

			.byte	ANI_INTSTREAM,3,5

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_cross_body_side_1
			.byte	SCRIPT
run_cross_body_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,6
			.word 	SEQ_P_CROS_R,SEQ_P_CROS_L
			.byte	2

			.byte	ANI_INTSTREAM,3,6

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl pass_behind_4
			.byte	SCRIPT
pass_behind_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,pass_behind_4_r

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_BCK2_L
			.byte	14

			.byte	ANI_GOTO
			.word 	pass_behind_4_cont

pass_behind_4_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_BCK2_R
			.byte	14

pass_behind_4_cont:
			.byte	ANI_INTSTREAM,6,4

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,14,10

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl pass_behind_1
			.byte	SCRIPT
pass_behind_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_BACK_R,SEQ_P_BACK_L
			.byte	4

			.byte	ANI_INTSTREAM,3,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl run_pass_behind_1
			.byte	SCRIPT
run_pass_behind_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_BACK_R,SEQ_P_BACK_L
			.byte	4

			.byte	ANI_INTSTREAM,3,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl pass_behind_2
			.byte	SCRIPT
pass_behind_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_BACK2R,SEQ_P_BACK2L
			.byte	5

			.byte	ANI_INTSTREAM,3,5

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,7,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,11,14

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_behind_2
			.byte	SCRIPT
run_pass_behind_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_BACK2R,SEQ_P_BACK2L
			.byte	5

			.byte	ANI_INTSTREAM,3,6

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl pass_behind_3
			.byte	SCRIPT
pass_behind_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_OBCK_R,SEQ_P_OBCK_L
			.byte	3

			.byte	ANI_INTSTREAM,7,4

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_behind_3
			.byte	SCRIPT
run_pass_behind_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_OBCK_R,SEQ_P_OBCK_L
			.byte	3

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl pass_behind_back_side_1
			.byte	SCRIPT
pass_behind_back_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_BBCK_R,SEQ_P_BBCK_L
			.byte	6

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_behind_back_side_1
			.byte	SCRIPT
run_pass_behind_back_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_BBCK_R,SEQ_P_BBCK_L
			.byte	6

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl pass_behind_back_side_2
			.byte	SCRIPT
pass_behind_back_side_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_BBKB_R,SEQ_P_BBKB_L
			.byte	8

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_behind_back_side_2
			.byte	SCRIPT
run_pass_behind_back_side_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_BBKB_R,SEQ_P_BBKB_L
			.byte	8

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl chest_pass_side_1
			.byte	SCRIPT
chest_pass_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_P_SID2_R,SEQ_P_SID2_L
			.byte	8

			.byte	ANI_INTSTREAM,7,5

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,6,6

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOFRAME,28,1

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_chest_pass_side_1
			.byte	SCRIPT
run_chest_pass_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	plyr_right_of_me,-1,0,0,run_chest_pass_side_1_r

			.byte	ANI_INTTOSEQ,5
			.WORD	SEQ_P_SID2_R
			.byte	8

			.byte	ANI_GOTO
			.word	run_chest_pass_side_1_hands_done

run_chest_pass_side_1_r:
			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_SID2_L
			.byte	8

run_chest_pass_side_1_hands_done:
			.byte	ANI_INTSTREAM,7,5

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,6,6

			.byte	ANI_INTTOSEQH,4
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-104

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl forward_hardpass_1
			.byte	SCRIPT
forward_hardpass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.08

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,forward_hardpass_1_r

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_P_HARD_L
			.byte	8

			.byte	ANI_GOTO
			.word 	forward_hardpass_1_cont

forward_hardpass_1_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_HARD_R
			.byte	10

forward_hardpass_1_cont:
			.byte	ANI_INTSTREAM,7,10

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.8

			.byte	ANI_SETFRICTION
			.float	0.07

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,12,16

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,13,16

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl run_forward_hardpass_1
			.byte	SCRIPT
run_forward_hardpass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,run_forward_hardpass_1_r

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_P_HARD_L
			.byte	8

			.byte	ANI_GOTO
			.word 	run_forward_hardpass_1_cont

run_forward_hardpass_1_r:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_P_HARD_R
			.byte	10

run_forward_hardpass_1_cont:
			.byte	ANI_INTSTREAM,7,10

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,7,12

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;


			.globl pass_left_1
			.byte	SCRIPT
pass_left_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,pass_left_1_var_1

#FIX ANIM!!!
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_ZERO
			.byte	0

			.byte	ANI_GOTO
			.word 	pass_left_1_var_done
			
pass_left_1_var_1:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_BBK2_R
			.byte	0

pass_left_1_var_done:
			.byte	ANI_INTSTREAM,10,15

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;


			.globl pass_right_1
			.byte	SCRIPT
pass_right_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,pass_right_1_var_1

#FIX ANIM!!!
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_ZERO
			.byte	0

			.byte	ANI_GOTO
			.word 	pass_right_1_var_done
			
pass_right_1_var_1:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_BBK2_L
			.byte	0

pass_right_1_var_done:
			.byte	ANI_INTSTREAM,10,15

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl over_shoulder_pass
			.byte	SCRIPT
over_shoulder_pass:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_P_BCK1_R,SEQ_P_BCK1_L
			.byte	7

			.byte	ANI_INTSTREAM,6,7

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,7,7

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl running_over_shoulder_pass
			.byte	SCRIPT
running_over_shoulder_pass:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_TWOPART|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_P_BCK1_R,SEQ_P_BCK1_L
			.byte	7

			.byte	ANI_INTSTREAM,6,7

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_INTTOSEQH,5
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
# IN AIR PASSES
#;;;;;;;;;;;;;;;;;;;;;
#; In air pass left and right
			.globl air_pass_5
			.byte	SCRIPT
air_pass_5:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE
			.word	dampen_yvel,3,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_right_of_me,-1,0,0,air_pass_5_r

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_PAIR5_R
			.byte	0

			.byte	ANI_GOTO
			.word	air_pass_5_cont

air_pass_5_r:
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_PAIR5_L
			.byte	0

air_pass_5_cont:
			.byte	ANI_INTSTREAM,12,6

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

# redundant to clear??? since flag is cleared in PassRelease...
			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,22,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.85

# code handles transition to next anim
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; In air pass left and right

			.globl	air_pass_4
			.byte	SCRIPT
air_pass_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	dampen_yvel,3,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_right_of_me,-1,0,0,air_pass_4_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR4_R
			.byte	0

			.byte	ANI_GOTO
			.word	air_pass_4_cont

air_pass_4_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR4_L
			.byte	0

air_pass_4_cont:
			.byte	ANI_INTSTREAM,12,5

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

# redundant to clear??? since flag is cleared in PassRelease...
			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,22,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.85

# code handles transition to next anim
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; In air pass forward angles

			.globl	air_pass_3
			.byte	SCRIPT
air_pass_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	dampen_yvel,3,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR3_B
			.byte	9

air_pass_3_cont:
			.byte	ANI_INTSTREAM,3,5

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

# redundant to clear??? since flag is cleared in PassRelease...
			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

# code handles transition to next anim
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; In air pass behind 2nd variation

			.globl	air_pass_2
			.byte	SCRIPT
air_pass_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	dampen_yvel,3,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_right_of_me,-1,0,0,air_pass_2_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR2_L
			.byte	0

			.byte	ANI_GOTO
			.word	air_pass_2_cont

air_pass_2_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR2_R
			.byte	0

air_pass_2_cont:
			.byte	ANI_INTSTREAM,14,6

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

# redundant to clear??? since flag is cleared in PassRelease...
			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,19,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

# code handles transition to next anim
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; In air pass behind

			.globl	air_pass_1
			.byte	SCRIPT
air_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE
			.word	dampen_yvel,3,0,0

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,air_pass_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR1_L
			.byte	0

			.byte	ANI_GOTO
			.word	air_pass_1_cont

air_pass_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR1_R
			.byte	0

air_pass_1_cont:
			.byte	ANI_INTSTREAM,16,6

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

# redundant to clear??? since flag is cleared in PassRelease...
			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,22,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

# code handles transition to next anim
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	jshot_pass_right_1_r
			.byte	SCRIPT
jshot_pass_right_1_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING
			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

#FIX ANIM!!!
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_ZERO
			.byte	5

			.byte	ANI_INTSTREAM,6,5

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

# make sure that this playback speed is
# consistent with above playback speed before the pass
			.byte	ANI_INTSTREAM,17,AA_TOGROUND

#			.byte	ANI_PREP_XITION
#			.word	160,3
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.454

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	jshot_pass_left_1_l
			.byte	SCRIPT
jshot_pass_left_1_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING
			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

#FIX ANIM!!!
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_ZERO
			.byte	5

			.byte	ANI_INTSTREAM,6,5

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.454

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl running_pass_thru_legs_behind
			.byte	SCRIPT
running_pass_thru_legs_behind:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_SETFRICTION
			.float	0.25

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_BTWN_R,SEQ_P_BTWN_L
			.byte	2

			.byte	ANI_INTSTREAM,14,8
#	//7,5
			
			.byte	ANI_CODE
			.word	PassRelease,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

#			.byte	ANI_STREAMTOEND
#			.float	0.75

			.byte	ANI_INTSTREAM,20,15

			.byte	ANI_PREP_XITION
			.word	3,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	overhead_2_pass
			.byte	SCRIPT
overhead_2_pass:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_OVR2_B
			.byte	2

			.byte	ANI_INTSTREAM,31,11

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,28,14

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	overhead_1_pass
			.byte	SCRIPT
overhead_1_pass:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_OVR_HD
			.byte	7

			.byte	ANI_INTSTREAM,20,12

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_STREAMTOEND
			.float	1.1

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	running_overhead_1_pass
			.byte	SCRIPT
running_overhead_1_pass:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_TWOPART

#			.byte	ANI_CODE
#			.word 	face_receiver,0,0,0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_P_OVR_HD
			.byte	7

			.byte	ANI_INTSTREAM,20,14

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,7,7

			.byte	ANI_INTTOSEQH,5
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;


			.globl	chest_pass_anim
			.byte	SCRIPT
chest_pass_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CODE
			.word face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_OUTB1
			.byte	0

			.byte	ANI_INTSTREAM,7,9

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	0.4

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	chest_pass_2
			.byte	SCRIPT
chest_pass_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_CH1_1P
			.byte	9

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	chest_pass_3
			.byte	SCRIPT
chest_pass_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_CH2_1P
			.byte	0

			.byte	ANI_INTSTREAM,20,7

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;


			.globl	right_pass_1_r
			.byte	SCRIPT
right_pass_1_r:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_P_SID1_R
			.byte	19

#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_SID1_R

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTSTREAM,18,8

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;


			.globl	left_pass_1_l
			.byte	SCRIPT
left_pass_1_l:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING
			
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_P_SID1_L
			.byte	19

#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_SID1_L

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTSTREAM,18,8

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING
			
			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_forward_4
			.byte	SCRIPT
run_pass_forward_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.01

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_P_FRD1_R,SEQ_P_FRD1_L
			.byte	6

			.byte	ANI_INTSTREAM,10,10

# now pass the ball
			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_INTSTREAM,15,10

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	chest_pass_1
			.byte	SCRIPT
chest_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_CHEST
			.byte	18

			.byte	ANI_INTSTREAM,15,8

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	running_chest_pass
			.byte	SCRIPT
running_chest_pass:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_CHEST
			.byte	18

			.byte	ANI_INTSTREAM,15,8

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_RUN
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_side_1
			.byte	SCRIPT
run_pass_side_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING
			
			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_SID1_R,SEQ_P_SID1_L
			.byte	19

			.byte	ANI_INTSTREAM,18,8

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_INTTOSEQH,5
			.word SEQ_RUN_R,SEQ_RUN_L
			.byte	-105

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_side_2
			.byte	SCRIPT
run_pass_side_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			#|MODE_TWOPART

			#.byte ANI_CODE
			#.word set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_P_RHT3_R,SEQ_P_LFT3_L
			.byte	3

			.byte	ANI_INTSTREAM,33,15

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_PREP_XITION
			.word	3,30

			#.byte ANI_INTTOSEQH,5
			#.word SEQ_RUN_R,SEQ_RUN_L
			#.byte 30

			#.byte ANI_CODE
			#.word set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_diag_right_1
			.byte	SCRIPT
run_pass_diag_right_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS
			
			.byte	ANI_INTTOSEQ,5
			.word	SEQ_P_DIA1_R
			.byte	6

			.byte	ANI_INTSTREAM,4,3

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_INTTOSEQH,4
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-104

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	run_pass_diag_left_1
			.byte	SCRIPT
run_pass_diag_left_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTTOSEQ,5
			.word 	SEQ_P_DIA1_L
			.byte	6

			.byte	ANI_INTSTREAM,4,3

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_INTTOSEQH,4
			.word 	SEQ_RUN_R,SEQ_RUN_L
			.byte	-104

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#######################################################
# ALLY OOP PASSES                                     #
#######################################################
			.globl	allyoop_pass_1
			.byte	SCRIPT
allyoop_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_CH1_1P
			.byte	9

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

######################################################

			.globl	allyoop_pass_2
			.byte	SCRIPT
allyoop_pass_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_ALLY5
			.byte	3

			.byte	ANI_INTSTREAM,6,5

			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_STREAMTOEND
			.float 1.0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	twohand_ally_pass_1
			.byte	SCRIPT
twohand_ally_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_ALLY1
			.byte	12
			
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_SETYVEL
			.float	0.1
			
			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	overhead_1_pass_ally
			.byte	SCRIPT
overhead_1_pass_ally:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_OVR_HD
			.byte	9

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	0.754

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; In air pass forward angles

			.globl	ally_air_pass_1
			.byte	SCRIPT
ally_air_pass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	face_receiver,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	dampen_yvel,3,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PAIR3_B
			.byte	9

			.byte	ANI_INTSTREAM,3,5

			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;;;

			.globl	running_overhead_1_pass_ally
			.byte	SCRIPT
running_overhead_1_pass_ally:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,1,0,0

			.byte	ANI_CODE
			.word face_receiver,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_P_OVR_HD
			.byte	9

			.byte	ANI_INTSTREAM,18,10

			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	lob1_pass_anim
			.byte	SCRIPT
lob1_pass_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFLAG
			.word	PF_PASSING

			.byte	ANI_ZEROVELS
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_P_CHEST
#			.byte	0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_CHEST

			.byte	ANI_INTSTREAM,33,9

# now pass the ball
			.byte	ANI_CODE
			.word	AllyPassRelease,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING

			.byte	ANI_STREAMTOEND
			.float	0.454

			#.byte	ANI_PREP_XITION
			#.word 16,3

			.byte	ANI_SETMODE
			.word	0

# if stick in neutral make code allow transition
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;
			.globl run_forward_spinpass_1
			.byte	SCRIPT
run_forward_spinpass_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word 	face_receiver,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.20

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_P_PAS1_R,SEQ_P_PAS1_L
			.byte	0

			.byte	ANI_INTSTREAM,16,7

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;; players picks up ball from the ground

			.globl	ball_pickup_1
			.byte	SCRIPT
ball_pickup_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			#|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PICKUP_1
			.byte	0

			.byte	ANI_INTSTREAM,14,6

			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_INTSTREAM,14,8

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END



#;;;;;;;;;;;;;;;;;;; freethrow dribble #1

#			.globl	freethrow_dribble_1_anim
#			.byte	SCRIPT
#freethrow_dribble_1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_UNINT
#
#			.byte	ANI_CODE
#			.word	set_ball_hand,RIGHT_HAND,0,0
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_BOTH_HANDS
#			.byte	ANI_SETFLAG
#			.word	PF_NO_LOOK
#
#			.byte	ANI_ZEROVELS
#ftd1:
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_FT2
#			.byte	0
#
#			.byte	ANI_INTSTREAM,38,76
#			.byte	ANI_INTSTREAM,38,76
#
#			.byte	ANI_INTSTREAM,23,60
#
#			.byte	ANI_STREAMTOEND
#			.float	2.00
#
#			.byte	ANI_GOTO
#			.word	ftd1

#;;;;;;;;;;;;;;;;;;; freethrow shot #1

#			.globl	freethrow_shot_1_anim
#			.byte	SCRIPT
#freethrow_shot_1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT
#			
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	set_ball_hand,RIGHT_HAND,0,0
#
#			.byte	ANI_SETFLAG
#			.word	PF_SHOOTING|PF_NO_LOOK
#
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_FT2_SHT
#			.byte	0
#
#			.byte	ANI_INTSTREAM,21,35
#
#			.byte	ANI_CODE
#			.word	freethrow_release,0,0,0
#			
#			.byte	ANI_CODE
#			.word	freethrow_take_speech,0,0,0
#
#			.byte	ANI_CLRFLAG
#			.word	PF_SHOOTING|PF_NO_LOOK
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_STREAMTOEND
#			.float	1.95
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; freethrow dribble #2

			.globl	freethrow_dribble_2_anim
			.byte	SCRIPT
freethrow_dribble_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0
			
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_FT1

			.byte	ANI_RUNTOFRAME,0,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1
ftd2:
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_FT1
			.byte	0

			.byte	ANI_INTSTREAM,56,112

			.byte	ANI_INTSTREAM,42,90

			.byte	ANI_STREAMTOEND
			.float	2.10

			.byte	ANI_GOTO
			.word	ftd2

#;;;;;;;;;;;;;;;;;;; freethrow shot #2

			.globl	freethrow_shot_2_anim
			.byte	SCRIPT
freethrow_shot_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING|PF_NO_LOOK

#			.byte	ANI_INTTOSEQH,18
#			.word	SEQ_FT1_SHTR,SEQ_FT1_SHTL
#			.byte	4

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,freethrow_2_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,18
			.word	SEQ_FT1_SHTL
			.byte	4

			.byte	ANI_GOTO
			.word	freethrow_2_hands_done

freethrow_2_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,18
			.word	SEQ_FT1_SHTR
			.byte	4

freethrow_2_hands_done:
			.byte	ANI_INTSTREAM,15,33

			.byte	ANI_CODE
			.word	freethrow_release,0,0,0

			.byte	ANI_CODE
			.word	freethrow_take_speech,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	2.25

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; wait for freethrow #1

			.globl	wait_freethrow_1_anim
			.byte	SCRIPT
wait_freethrow_1_anim:
			
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_ZEROVELS
wf1_a:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_WAIT1
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	3.5

			.byte	ANI_GOTO
			.word	wf1_a

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; wait for freethrow #2

			.globl	wait_freethrow_2_anim
			.byte	SCRIPT
wait_freethrow_2_anim:
			
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_ZEROVELS
wf2_a:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_WAIT2
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	3.5

			.byte	ANI_GOTO
			.word	wf2_a

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
				

#;;;;;;;;;;;;;;;;;;; wait for freethrow #3

			.globl	wait_freethrow_3_anim
			.byte	SCRIPT
wait_freethrow_3_anim:
			
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_ZEROVELS
wf3_a:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_WAIT3
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	3.5

			.byte	ANI_GOTO
			.word	wf3_a

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
	
#;;;;;;;;;;;;;;;;;;; wait for freethrow #4

			.globl	wait_freethrow_4_anim
			.byte	SCRIPT
wait_freethrow_4_anim:

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_ZEROVELS
wf4_a:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_WAIT3
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	4.5

			.byte	ANI_GOTO
			.word	wf4_a

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; wait for rebound #1

			.globl	wait_rebound_1_anim
			.byte	SCRIPT
wait_rebound_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	rotate_to_face_rim,0,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_WAIT_1
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	1.30

wr1:
			.byte	ANI_NEWSEQ
			.word	SEQ_WAIT_1

			.byte	ANI_STREAMTOEND
			.float	1.30

			.byte	ANI_GOTO
			.word	wr1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; wait rebound #2

			.globl	wait_rebound_2_anim
			.byte	SCRIPT
wait_rebound_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	rotate_to_face_rim,0,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_WAIT_3
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	1.30

wr2:
			.byte	ANI_NEWSEQ
			.word	SEQ_WAIT_3

			.byte	ANI_STREAMTOEND
			.float	1.30

			.byte	ANI_GOTO
			.word	wr2
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; standing rebound #1

			.globl	quick_rebound_1_anim
			.byte	SCRIPT
quick_rebound_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB1_MIS
			.byte	0

			.byte	ANI_INTSTREAM,11,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetQuickReboundVels,SEQ_RB1_MIS,25,4

			#extend arm
			.byte	ANI_INTSTREAM,14,5

			#attempt to grab ball
			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; quick rebound #2

			.globl	quick_rebound_2_anim
			.byte	SCRIPT
quick_rebound_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB2_MIS
			.byte	0

			.byte	ANI_INTSTREAM,11,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetQuickReboundVels,SEQ_RB2_MIS,23,4

			#extend arm
			.byte	ANI_INTSTREAM,12,5

			#attempt to grab ball outta air
			#.byte	ANI_INTSTREAM,16,8

			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; low rebound #1

			.globl	low_rebound_1_anim
			.byte	SCRIPT
low_rebound_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

#			.byte	ANI_CODE
#			.word	set_ball_hand,RIGHT_HAND,0,0

#			.byte	ANI_CODE
#			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_LWRB1
			.byte	0

			.byte	ANI_INTSTREAM,8,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetQuickReboundVels,SEQ_LWRB1,14,3

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; low rebound #2

			.globl	low_rebound_2_anim
			.byte	SCRIPT
low_rebound_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

#			.byte	ANI_CODE
#			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_LWRB2
			.byte	0

			.byte	ANI_INTSTREAM,6,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetQuickReboundVels,SEQ_LWRB2,12,3

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_QUICK_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; rebound #1

			.globl	rebound1_anim
			.byte	SCRIPT
rebound1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

#			.byte	ANI_SETFLAG
#			.word	PF_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB1_MIS
			.byte	0

#			.byte	ANI_CODE
#			.word	do_printf_in_anim,0,0,0

			.byte	ANI_INTSTREAM,11,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetReboundVels,SEQ_RB1_MIS,28,18

			.byte	ANI_SETFLAG
			.word	PF_REBOUND

			#extend arm
			.byte	ANI_INTSTREAM,15,23

			#attempt to grab ball
			.byte	ANI_INTSTREAM,9,7

			.byte	ANI_INTSTREAM,19,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound 1 grab the ball

			.globl	rebound1_grab_anim
			.byte	SCRIPT
rebound1_grab_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_INTTOSEQH,1
			.word	SEQ_RB1_GRBR,SEQ_RB1_GRBL
			.byte	0

			#grab ball
			.byte	ANI_INTSTREAM,10,1
			
			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_throw_elbows,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.85

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound #2


			.globl	rebound2_anim
			.byte	SCRIPT
rebound2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB2_MIS
			.byte	0

			.byte	ANI_INTSTREAM,11,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetReboundVels,SEQ_RB2_MIS,23,18

			#extend arm
			.byte	ANI_INTSTREAM,12,22

			#attempt to grab ball outta air
			#.byte	ANI_INTSTREAM,16,14

			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; rebound 2 grab the ball

			.globl	rebound2_grab_anim
			.byte	SCRIPT
rebound2_grab_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND
			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			#.byte	ANI_CODE_BNZ
			#.word	check_rebound_frame,SEQ_RB2_MIS,34,0,faster_interp2

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_RB2_GRB
			.byte	0

			#anim to ground...duh
			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_throw_elbows,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound #3

			.globl	rebound3_anim
			.byte	SCRIPT
rebound3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB1_MIS
			.byte	0

			.byte	ANI_INTSTREAM,11,1	#5

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetReboundVels,SEQ_RB3_MIS,29,19

			#extend arm
			.byte	ANI_INTSTREAM,17,19

			.byte	ANI_INTSTREAM,26,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound 3 grab the ball

			.globl	rebound3_grab_anim
			.byte	SCRIPT
rebound3_grab_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_RB2_GRB
			.byte	39

			.byte	1

			.byte	ANI_INTSTREAM,19,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_throw_elbows,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;; rebound #6

			.globl	rebound6_anim
			.byte	SCRIPT
rebound6_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB6_MIS
			.byte	0
		
			.byte	ANI_INTSTREAM,11,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetReboundVels,SEQ_RB6_MIS,24,18

			.byte	ANI_SETFLAG
			.word	PF_REBOUND

			#extend arm
			.byte	ANI_INTSTREAM,12,18

			.byte	ANI_INTSTREAM,34,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound 6 grab the ball

			.globl	rebound6_grab_anim
			.byte	SCRIPT
rebound6_grab_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_BOTH_HANDS

# Didn't look right
#			.byte	ANI_SETFLAG
#			.word	PF_BOTH_HANDS

# Put it in right hand
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_RB6_GRB
			.byte	17

			#grab ball outta air
#			.byte	ANI_INTSTREAM,10,14

			.byte	ANI_INTSTREAM,37,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_throw_elbows,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.85

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;; rebound #5

			.globl	rebound5_anim
			.byte	SCRIPT
rebound5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_RB2_MIS
			.byte	11

			.byte	ANI_SETFLAG
			.word	PF_REBOUND

			.byte	ANI_CODE	   #seq, frame, ticks
			.word	SetReboundVels,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound 4 grab the ball

			.globl	rebound5_grab_anim
			.byte	SCRIPT
rebound5_grab_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_RB5_GRBR,SEQ_RB5_GRBL
			.byte	0

			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_throw_elbows,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;; rebound #7

			.globl	rebound7_anim
			.byte	SCRIPT
rebound7_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_REBOUND

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_INTTOSEQ,1
			.word	SEQ_RB7_MIS
			.byte	0
		
			.byte	ANI_INTSTREAM,8,1

			.byte	ANI_CODE	   #seq, frame, ball 'ticks from now' position
			.word	SetReboundVels,SEQ_RB7_MIS,28,18

#			.byte	ANI_SETFLAG
#			.word	PF_REBOUND

			#extend arm
			.byte	ANI_INTSTREAM,21,14

			#goto ground
			.byte	ANI_INTSTREAM,24,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; rebound #7 grab the ball

			.globl	rebound7_grab_anim
			.byte	SCRIPT
rebound7_grab_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_REBOUND|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_RB8_R
			.byte	9

			.byte	ANI_INTSTREAM,14,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_throw_elbows,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;;;;;;;;;;;;; throwing elbows with ball in hand


			.globl	throw_elbows_wb_anim
			.byte	SCRIPT
throw_elbows_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_TURNHACK
			.word	64
			
			.byte	ANI_NEWSEQ
			.word	SEQ_M_ELBOW4

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_LWRIST,JOINT_LELBOW,13
			.float	1.5,1.5,5.0
			.byte	ANI_INTSTREAM,20,13

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,9,5

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_RWRIST,JOINT_RELBOW,13
			.float	1.5,1.5,3.5
			.byte	ANI_INTSTREAM,20,13

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,9,5

			.byte	ANI_TURNHACK
			.word	-64

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

## throw some wild elbows
			.globl throw_elbows_wb_2_anim
			.byte	SCRIPT
throw_elbows_wb_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_ELBOW_1
			.byte	16

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_RWRIST,JOINT_RELBOW,0
			.float	1.5,1.5,5.0

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0
			
			.byte	ANI_INTSTREAM,14,20

			.byte	ANI_ATTACK_OFF

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_LWRIST,JOINT_LELBOW,0
			.float	1.5,1.5,3.5

			.byte	ANI_CODE
			.word	attack_snd,0,1,0

			.byte	ANI_INTSTREAM,11,14

			.byte	ANI_ATTACK_OFF

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_RWRIST,JOINT_RELBOW,0
			.float	1.5,1.5,5.0

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0
			
			.byte	ANI_INTSTREAM,23,28

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; throws elbows to dude in front of him

			.globl throw_elbows_front_wb_anim
			.byte	SCRIPT
throw_elbows_front_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_TURNHACK
			.word	64
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_ELBOW_1
			.byte	17

			.byte	ANI_CODE
			.word	attack_snd,0,-1,0

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_LELBOW,JOINT_RELBOW,40
			.float	1.5,1.5,5.0

			.byte	ANI_INTSTREAM,44,38

			.byte	ANI_ATTACK_OFF

			.byte	ANI_TURNHACK
			.word	-64

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;; throws elbows to dude in front of him

			.globl kick_anim
			.byte	SCRIPT
kick_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_KICK
			.byte	0

			.byte	ANI_CODE
			.word	attack_snd,0,1,0

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_RKNEE,JOINT_RANKLE,8
			.float	1.0,1.0,5.0

			.byte	ANI_INTSTREAM,6,5
			.byte	ANI_INTSTREAM,2,3

			.byte	ANI_ATTACK_OFF

			.byte	ANI_STREAMTOEND
			.float	.70

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; throws elbows to dude in front of him

			.globl	hit_dude_wb_anim
			.byte	SCRIPT
hit_dude_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_BALLFACE
			.byte	0

			.byte	ANI_CODE
			.word	attack_snd,0,1,0

			.byte	ANI_ATTACK_ON,ATTMODE_ELBOWS,JOINT_LWRIST,JOINT_RWRIST,10
			.float	1.5,1.5,7.0

			.byte	ANI_INTSTREAM,11,4

			.byte	ANI_INTSTREAM,3,5

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,20,13

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	.75

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;; standing ready with ball

			.globl	ready_wb_anim
			.byte	SCRIPT
ready_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_ZEROVELS
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_RDY_WB
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.80
			
ready_wb_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_RDY_WB
			
			.byte	ANI_STREAMTOEND
			.float	0.80
			
			.byte	ANI_GOTO
			.word	ready_wb_loop
			.byte	ANI_END

			.globl	ready_wb_2_anim
			.byte	SCRIPT
ready_wb_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_TRDY_R
			.byte	5

			.byte	ANI_STREAMTOEND
			.float	1.3

ready_wb_2_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_TRDY_R

			.byte	ANI_STREAMTOEND
			.float	1.3

			.byte	ANI_CODE_BNZ
			.word	plyr_in_front_of_me,0,0,0,ready_wb_2_fake_step


			.byte	ANI_GOTO
			.word	ready_wb_2_loop
			.byte	ANI_END

ready_wb_2_fake_step:
			.byte	ANI_JIFSEQ
			.word	SEQ_D_FAKE_R,ready_wb_2_ball_in_r
			.byte	ANI_JIFSEQ
			.word	SEQ_TRDY_R,ready_wb_2_ball_in_r

			#.byte	ANI_CODE_BNZ
			#.word	ball_in_righthand,0,0,0,ready_wb_2_ball_in_r

			.byte	ANI_JIFSEQ
			.word	SEQ_D_FAKE_L,ready_wb_2_fakestep_hands_done

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_FAKE_L
			.byte	0

			.byte	ANI_GOTO
			.word 	ready_wb_2_fakestep_hands_done

ready_wb_2_ball_in_r:
			.byte	ANI_JIFSEQ
			.word	SEQ_D_FAKE_R,ready_wb_2_fakestep_hands_done

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_D_FAKE_R
			.byte	0

ready_wb_2_fakestep_hands_done:
			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_SETMODE
			.word 0

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,1,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word ready_wb_2_anim

#########################################################
# Run variations
			.globl walk_1_anim
			.byte	SCRIPT
walk_1_anim:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_WALK
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.8

walk_1_cyc:
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,walk_1_cyc_no_squeaks

			.byte	ANI_CODE
			.word shoe_squeak,0,0,0

walk_1_cyc_no_squeaks:
			.byte	ANI_NEWSEQ
			.word	SEQ_WALK

			.byte	ANI_STREAMTOEND
			.float	0.8

			.byte	ANI_GOTO
			.word	walk_1_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl boxer_1_anim
			.byte	SCRIPT
boxer_1_anim:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_BOXR_RUN
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	1.1

boxer_1_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_BOXR_RUN

			.byte	ANI_STREAMTOEND
			.float	1.1

			.byte	ANI_GOTO
			.word	boxer_1_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl boxer_run_legs_1_anim
			.byte	SCRIPT
boxer_run_legs_1_anim:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			#.word	SEQ_CKY_RUN1
			#.byte	27
			.word	SEQ_JOG_1
			.byte	20

			.byte	ANI_STREAMTOEND
			#.float	0.5
			.float	0.63

boxer_run_legs_1_cyc:
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,boxer_run_legs_1_cyc_no_squeaks

			.byte	ANI_CODE
			.word shoe_squeak,0,0,0

boxer_run_legs_1_cyc_no_squeaks:
			.byte	ANI_NEWSEQ
			#.word	SEQ_CKY_RUN1
			.word	SEQ_JOG_1

			.byte	ANI_STREAMTOEND
			#.float	0.5
			.float	0.63

			.byte	ANI_GOTO
			.word	boxer_run_legs_1_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl cocky_run_1_anim
			.byte	SCRIPT
cocky_run_1_anim:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_CKY_RUN1
			.byte	27

			.byte	ANI_STREAMTOEND
			.float	0.5

cocky_run_1_cyc:
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,cocky_run_1_cyc_no_squeaks

			.byte	ANI_CODE
			.word shoe_squeak,0,0,0

cocky_run_1_cyc_no_squeaks:
			.byte	ANI_NEWSEQ
			.word	SEQ_CKY_RUN1

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_GOTO
			.word	cocky_run_1_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl jog_1_anim
			.byte	SCRIPT
jog_1_anim:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JOG_1
			.byte	20

			.byte	ANI_CODE
			.word	plyr_taunt_speech,-1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.63

jog_1_cyc:
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,jog_1_cyc_no_squeaks

			.byte	ANI_CODE
			.word shoe_squeak,0,0,0

jog_1_cyc_no_squeaks:
			.byte	ANI_NEWSEQ
			.word	SEQ_JOG_1

			.byte	ANI_STREAMTOEND
			.float	0.63

			.byte	ANI_GOTO
			.word	jog_1_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl jog_2_anim
			.byte	SCRIPT
jog_2_anim:
			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_AFTER_1
			.byte	10

			.byte	ANI_CODE
			.word	plyr_taunt_speech,-1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.65

jog_2_cyc:
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,jog_2_cyc_no_squeaks

			.byte	ANI_CODE
			.word shoe_squeak,0,0,0

jog_2_cyc_no_squeaks:
			.byte	ANI_NEWSEQ
			.word	SEQ_AFTER_1

			.byte	ANI_STREAMTOEND
			.float	0.65

			.byte	ANI_GOTO
			.word	jog_2_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# player was doing a running catch and the 
# pass just got stolen or deflected
			.globl back_into_run_from_catch
			.byte	SCRIPT
back_into_run_from_catch:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_CLRFLAG
			.word	PF_CATCHING

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	-104

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# standard run
			.globl	run_anim
			.byte	SCRIPT
run_anim:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_JIFSEQ
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L,to_run_from_dribble

			.byte	ANI_JIFSEQ
			.word	SEQ_S_READY,SEQ_SBREATH1,SEQ_SBREATH3,SEQ_S_CYC_1,SEQ_S_CYC_3,to_run_from_s_ready

			.byte	ANI_INTTOSEQH,6
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	0

			.byte	ANI_GOTO
			.word to_run_cont

to_run_from_dribble:
			.byte	ANI_INTTOSEQH,1
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	AA_PROPFRAME

			.byte	ANI_GOTO
			.word to_run_cont

to_run_from_s_ready:
			.byte	ANI_INTTOSEQH,6
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	13

to_run_cont:
			.byte	ANI_VSTREAMTOEND
			.float	0.9

run_cyc:
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,3,0,0,run_cyc_no_squeaks

			#.byte	ANI_CODE
			#.word shoe_squeak,0,0,0

run_cyc_no_squeaks:
			.byte	ANI_NEWSEQH
			.word	SEQ_RUN_R,SEQ_RUN_L

			.byte	ANI_VSTREAMTOEND
			.float	0.9

			.byte	ANI_GOTO
			.word	run_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			.globl	run_anim_int
			.byte	SCRIPT
run_anim_int:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_JIFSEQ
			.word	SEQ_S_READY,SEQ_SBREATH1,SEQ_SBREATH3,to_run_int_from_s_ready

			.byte	ANI_JIFSEQ
			.word	SEQ_D_RUN2_R,SEQ_D_RUN2_L,to_run_int_from_drun

			.byte	ANI_JIFSEQ
			.word	SEQ_DSLIDE_R,SEQ_DSLID2_R,SEQ_DSLIDE_L,SEQ_DSLID2_L,SEQ_DSLIDE_B,SEQ_DSLIDE_F,to_run_from_defense

# normal transition to run
			.byte	ANI_INTTOSEQH,AA_VAR_COUNT
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word to_run_cont

to_run_from_defense:
			.byte	ANI_INTTOSEQH,4
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	13

			.byte	ANI_GOTO
			.word to_run_cont

to_run_int_from_s_ready:
			.byte	ANI_INTTOSEQH,3
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	13

			.byte	ANI_VSTREAMTOEND
			.float	0.9

			.byte	ANI_GOTO
			.word	run_cyc

to_run_int_from_drun:
			.byte	ANI_INTTOSEQH,3
			.word	SEQ_RUN_R,SEQ_RUN_L
			.byte	AA_PROPFRAME

			.byte	ANI_VSTREAMTOEND
			.float	0.9

			.byte	ANI_GOTO
			.word	run_cyc

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# turbo run
			.globl	run_animt
			.byte	SCRIPT
run_animt:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_JIFSEQ
			.word	SEQ_S_READY,SEQ_SBREATH1,SEQ_SBREATH3,SEQ_S_CYC_1,SEQ_S_CYC_3,to_trun_from_s_ready

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_TRUN,SEQ_TRUN
			.byte	0

			.byte	ANI_GOTO
			.word to_trun_cont

to_trun_from_s_ready:
			.byte	ANI_INTTOSEQH,6
			.word	SEQ_TRUN,SEQ_TRUN
			.byte	26

to_trun_cont:
			.byte	ANI_STREAMTOEND
			.float	0.75

run_cyct:
			.byte	ANI_NEWSEQH
			.word	SEQ_TRUN,SEQ_TRUN

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_GOTO
			.word	run_cyct

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# turbo run interp into
			.globl	run_anim_intt
			.byte	SCRIPT
run_anim_intt:
#			.byte	ANI_ATTACK_ON,ATTMODE_RUN,JOINT_NECK,JOINT_PELVIS,0
#			.float	0.0,0.0,5.0

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP|PF_BOTH_HANDS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_JIFSEQ
			.word	SEQ_S_READY,SEQ_SBREATH1,SEQ_SBREATH3,SEQ_S_CYC_1,SEQ_S_CYC_3,to_trun_from_s_ready

#normal transition to turbo run
			.byte	ANI_INTTOSEQH,AA_VAR_COUNT
			.word	SEQ_TRUN,SEQ_TRUN
			.byte	AA_VAR_FRAME

			.byte	ANI_GOTO
			.word	to_trun_cont

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; one hand in air block

			.globl	block_1h_anim
			.byte	SCRIPT
block_1h_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,b1_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BLOK1L
			.byte	10

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_BLOK1L,42,JOINT_LWRIST

			.byte	ANI_GOTO
			.word	b1_cont

b1_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BLOK1R
			.byte	10
			
			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_BLOK1R,42,JOINT_RWRIST

b1_cont:
			.byte	ANI_SETFLAG
			.word	PF_IN_AIR_BLOCK

			.byte	ANI_INTSTREAM,14,15

#			.byte	ANI_WAITTRUE
#			.word	swat_plyr_button_hit_timeout

			.byte	ANI_INTSTREAM,5,7

			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_IN_AIR_BLOCK|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; two handed in air block

			.globl	block_2h_anim
			.byte	SCRIPT
block_2h_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_SBLK_1
			.byte	0

			.byte	ANI_INTSTREAM,16,2

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_SBLK_1,28,JOINT_BWRIST

			.byte	ANI_SETFLAG
			.word	PF_IN_AIR_BLOCK

			.byte	ANI_INTSTREAM,12,22

			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_IN_AIR_BLOCK

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,18,8

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; two handed in air block

			.globl	block_2h_2_anim
			.byte	SCRIPT
block_2h_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_2HBLK
			.byte	0

			.byte	ANI_INTSTREAM,8,2

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_2HBLK,24,JOINT_BWRIST

			.byte	ANI_SETFLAG
			.word	PF_IN_AIR_BLOCK

			.byte	ANI_INTSTREAM,18,22

			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_IN_AIR_BLOCK

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; two handed in air block #3

			.globl	block_2h_3_anim
			.byte	SCRIPT
block_2h_3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_2HBLK
			.byte	0

			.byte	ANI_INTSTREAM,8,2

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_2HBLK,18,JOINT_BWRIST

			.byte	ANI_SETFLAG
			.word	PF_IN_AIR_BLOCK

#			.byte	ANI_INTSTREAM,10,20

#			.byte	ANI_INTSTREAM,35,AA_TOGROUND
			.byte	ANI_INTSTREAM,45,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_IN_AIR_BLOCK

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.55

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; one hand in air block (short and quick)


			.globl	quick_block_1h_anim
			.byte	SCRIPT
quick_block_1h_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,qb1_hand_r

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BLOK1L
			.byte	10

			.byte	ANI_GOTO
			.word	qb1_cont
qb1_hand_r:
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BLOK1R
			.byte	10
qb1_cont:
			.byte	ANI_SETYVEL
			.float	.50

#			.byte	ANI_SETFLAG
#			.word	PF_IN_AIR_BLOCK

#			.byte	ANI_INTSTREAM,14,15

			.byte	ANI_INTSTREAM,34,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

#			.byte	ANI_CLRFLAG
#			.word	PF_IN_AIR_BLOCK

			.byte	ANI_STREAMTOEND
			.float	0.55

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; one hand in air block (short and quick)


			.globl	quick_block_2h_1_anim
			.byte	SCRIPT
quick_block_2h_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_SBLK_1
			.byte	0

			.byte	ANI_INTSTREAM,20,5
			
			.byte	ANI_SETYVEL
			.float	.55

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

#			.byte	ANI_CLRFLAG
#			.word	PF_IN_AIR_BLOCK

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; one hand in air block (short and quick)


			.globl	quick_block_2h_2_anim
			.byte	SCRIPT
quick_block_2h_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_SBLK_2
			.byte	0

			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_SETYVEL
			.float	.55

			.byte	ANI_INTSTREAM,26,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
			 

#;;;;;;;;;;;;;;;;;; player trys to bat the ball away (in air) but misses

			.globl	swat_1h_1_anim
			.byte	SCRIPT
swat_1h_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,s1_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BAT_L
			.byte	10

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_BAT_L,34,JOINT_LWRIST
#			.word	SetBlockShotVels,SEQ_S_BAT_L,28,JOINT_LWRIST

			.byte	ANI_GOTO
			.word	s1_cont

s1_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BAT_R
			.byte	10

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_BAT_R,34,JOINT_RWRIST
#			.word	SetBlockShotVels,SEQ_S_BAT_R,28,JOINT_RWRIST
s1_cont:

			#wind up
#			.byte	ANI_INTSTREAM,10,20
			.byte	ANI_INTSTREAM,18,10

			.byte	ANI_WAITTRUE
			.word	swat_plyr_button_hit_timeout

			.byte	ANI_SETFLAG
			.word	PF_SWATING

			#swat at ball
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,9,13

			.byte	ANI_CLRFLAG
			.word	PF_SWATING

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;; player trys to bat the ball away 'in air' but misses #2

			.globl	swat_1h_2_anim
			.byte	SCRIPT
swat_1h_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,s2_hand_r

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_SWAT3L
			.byte	10

			.byte	ANI_CODE	   #seq, frame, join to intercept with
#			.word	SetBlockShotVels,SEQ_S_SWAT3L,26,JOINT_LWRIST
			.word	SetBlockShotVels,SEQ_S_SWAT3L,37,JOINT_LWRIST

			.byte	ANI_GOTO
			.word	s2_cont
s2_hand_r:
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_SWAT3R
			.byte	10

			.byte	ANI_CODE	   #seq, frame, join to intercept with
#			.word	SetBlockShotVels,SEQ_S_SWAT3R,26,JOINT_RWRIST
			.word	SetBlockShotVels,SEQ_S_SWAT3R,37,JOINT_RWRIST
s2_cont:
			#wind up
			.byte	ANI_INTSTREAM,27,14

			.byte	ANI_WAITTRUE
			.word	swat_plyr_button_hit_timeout

			.byte	ANI_SETFLAG
			.word	PF_SWATING

			#swat at ball
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,9,12

			.byte	ANI_CLRFLAG
			.word	PF_SWATING

			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;; player trys to bat the ball away from behind ball handler

			.globl	swat_1h_behind_anim
			.byte	SCRIPT
swat_1h_behind_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_ZEROVELS

#			.byte	ANI_CODE_BNZ
#			.word	plyr_is_righthanded,0,0,0,s2_hand_r

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_BAT_B
			.byte	0

			.byte	ANI_INTSTREAM,7,3

			.byte	ANI_CODE	   #seq, frame, join to intercept with
#			.word	SetBlockShotVels,SEQ_S_BAT_B,19,JOINT_RWRIST
			.word	SetBlockShotVels,SEQ_S_BAT_B,26,JOINT_RWRIST

			.byte	ANI_SETFLAG
			.word	PF_SWATING

			#wind up
			.byte	ANI_INTSTREAM,12,19

			#swat at ball
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,9,12

			.byte	ANI_CLRFLAG
			.word	PF_SWATING

			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;; player trys to bat the ball away (in air) but misses with both hands

			.globl	swat_2h_1_anim
			.byte	SCRIPT
swat_2h_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_2HAND1
			.byte	8

			.byte	ANI_CODE	   #seq, frame, join to intercept with
#			.word	SetBlockShotVels,SEQ_S_2HAND1,28,JOINT_BWRIST
			.word	SetBlockShotVels,SEQ_S_2HAND1,20,JOINT_BWRIST

			#wind up
#			.byte	ANI_INTSTREAM,10,12
			.byte	ANI_INTSTREAM,20,10


			.byte	ANI_WAITTRUE
			.word	swat_plyr_button_hit_timeout

			.byte	ANI_SETFLAG
			.word	PF_SWATING

			#swat at ball
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,10,12

			.byte	ANI_CLRFLAG
			.word	PF_SWATING

			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.80

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;; player trys to bat the ball away but misses with both hands

			.globl	swat_2h_2_anim
			.byte	SCRIPT
swat_2h_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_ball,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_S_2HAND2
			.byte	11

			.byte	ANI_CODE	   #seq, frame, join to intercept with
			.word	SetBlockShotVels,SEQ_S_2HAND2,24,JOINT_BWRIST

			#wind up
#			.byte	ANI_INTSTREAM,13,19
			.byte	ANI_INTSTREAM,14,10

			.byte	ANI_WAITTRUE
			.word	swat_plyr_button_hit_timeout

			.byte	ANI_SETFLAG
			.word	PF_SWATING

			#swat at ball
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,15,12

			.byte	ANI_CLRFLAG
			.word	PF_SWATING

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;

			.globl	dslide_anim_int_r
			.byte	SCRIPT
dslide_anim_int_r:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_DSLIDE_R
			.byte	0

dslide_cyc_r:
			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_NEWSEQ
			.word	SEQ_DSLIDE_R

			.byte	ANI_GOTO
			.word	dslide_cyc_r

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	dslide_anim2_int_r
			.byte	SCRIPT
dslide_anim2_int_r:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_DSLID2_R
			.byte	0

dslide_cyc_r2:
			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_NEWSEQ
			.word	SEQ_DSLID2_R

			.byte	ANI_GOTO
			.word	dslide_cyc_r2

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	dslide_anim_int_l
			.byte	SCRIPT
dslide_anim_int_l:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_DSLIDE_L
			.byte	0

dslide_cyc_l:
			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_NEWSEQ
			.word	SEQ_DSLIDE_L

			.byte	ANI_GOTO
			.word	dslide_cyc_l

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	dslide_anim2_int_l
			.byte	SCRIPT
dslide_anim2_int_l:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_DSLID2_L
			.byte	0

dslide_cyc_l2:
			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_NEWSEQ
			.word	SEQ_DSLID2_L

			.byte	ANI_GOTO
			.word	dslide_cyc_l2

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	dslide_anim_back
			.byte	SCRIPT
dslide_anim_back:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_DSLIDE_B
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	.85

dslide_cyc_b:
			.byte	ANI_NEWSEQ
			.word	SEQ_DSLIDE_B

			.byte	ANI_STREAMTOEND
			.float	.85

			.byte	ANI_GOTO
			.word	dslide_cyc_b

			.byte	ANI_END
			
#;;;;;;;;;;;;;;;;;

			.globl	dslide_anim_forward
			.byte	SCRIPT
dslide_anim_forward:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_DSLIDE_F
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.85

dslide_cyc_f:
			.byte	ANI_NEWSEQ
			.word	SEQ_DSLIDE_F

			.byte	ANI_STREAMTOEND
			.float	0.85

			.byte	ANI_GOTO
			.word	dslide_cyc_f

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; a one handed push

			.globl	push_1h_1_anim
			.byte	SCRIPT
push_1h_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	attack_snd,0,1,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.7

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,push_1h_1r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PUSH_6_L
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_PELVIS,JOINT_LWRIST,12
			.float	0.0,6.5,2.5

			.byte	ANI_GOTO
			.word	push_1h_1_cont

push_1h_1r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PUSH_6_R
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_PELVIS,JOINT_RWRIST,12
			.float	0.0,6.5,2.5

push_1h_1_cont:
			.byte	ANI_INTSTREAM,6,5
			.byte	ANI_INTSTREAM,3,6

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,14,13

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; a two handed push


			.globl	push_2h_1_anim
			.byte	SCRIPT
push_2h_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETVELS
			.float	0.0,0.0,1.8

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_BWRIST,JOINT_PELVIS,11
			.float	6.5,0.0,1.5
#			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_RWRIST,JOINT_LWRIST,11
#			.float	0.0,0.0,1.0

			.byte	ANI_CODE
			.word	attack_snd,0,1,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_PUSH_4
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,12,4

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_INTSTREAM,30,16

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_PREP_XITION
			.word	8,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; defender swipes at ball...down low


			.globl	swipe_low_anim
			.byte	SCRIPT
swipe_low_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	hand_closet_to_ball,0,0,0,swipe_low_hand_r
sl_l_lp:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_SWIPE1_L
			.byte	3

			.byte	ANI_INTSTREAM,11,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sl_l_done

			.byte	ANI_INTSTREAM,7,2

			.byte	ANI_INTSTREAM,19,7

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sl_l_done

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,sl_l_lp
sl_l_done:
			.byte	ANI_CLRFLAG
			.word	PF_STEALING
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

swipe_low_hand_r:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_SWIPE1_R
			.byte	3

			.byte	ANI_INTSTREAM,11,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sl_r_done

			.byte	ANI_INTSTREAM,7,2

			.byte	ANI_INTSTREAM,19,7

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sl_r_done

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,swipe_low_hand_r
sl_r_done:
			.byte	ANI_CLRFLAG
			.word	PF_STEALING
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
			

#;;;;;;;;;;;;;;;;; defender swipes at ball...straight across chest


			.globl	swipe_mid_anim
			.byte	SCRIPT
swipe_mid_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	hand_closet_to_ball,0,0,0,swipe_mid_hand_r

sm_l_lp:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_F_ST2_L
			.byte	9

			.byte	ANI_INTSTREAM,14,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sm_l_done

			.byte	ANI_INTSTREAM,20,8

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sm_l_done

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,sm_l_lp
sm_l_done:
			.byte	ANI_CLRFLAG
			.word	PF_STEALING
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

#			.byte	ANI_PREP_XITION
#			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


swipe_mid_hand_r:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_F_ST2_R
			.byte	9

			.byte	ANI_INTSTREAM,14,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sm_r_done

			.byte	ANI_INTSTREAM,20,8

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sm_r_done

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,swipe_mid_hand_r
sm_r_done:
			.byte	ANI_CLRFLAG
			.word	PF_STEALING
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; defender swipes at ball...upward across chest

			.globl	swipe_high_anim
			.byte	SCRIPT
swipe_high_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	hand_closet_to_ball,0,0,0,swipe_high_hand_r

sh_l_lp:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_F_ST4_L
			.byte	6

			.byte	ANI_INTSTREAM,15,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sh_l_done

			.byte	ANI_INTSTREAM,25,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sh_l_done

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,sh_l_lp

sh_l_done:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_STEALING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


swipe_high_hand_r:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_F_ST4_L
			.byte	6

			.byte	ANI_INTSTREAM,15,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sh_r_done

			.byte	ANI_INTSTREAM,25,9

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,sh_r_done

			.byte	ANI_STREAMTOEND
			.float	0.45

			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,swipe_high_hand_r
sh_r_done:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_STEALING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	swipe3_anim_run
			.byte	SCRIPT
swipe3_anim_run:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

swipe3_button_is_held_run:
			.byte	ANI_SETFLAG
			.word	PF_STEALING

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,swipe3_hand_r_run

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_F_ST2_L
			.byte	9

			.byte	ANI_GOTO
			.word	swipe3_hands_done

swipe3_hand_r_run:

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_F_ST2_R
			.byte	9

swipe3_hands_done:
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,14,9

			.byte	ANI_CLRFLAG
			.word	PF_STEALING

			.byte	ANI_CODE_BZ
			.word	plyr_is_holding_but,BUT_B,0,0,swipe3_end_anim_run

			.byte	ANI_INTSTREAM,18,16

swipe3_continue_anim_run:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_holding_but,BUT_B,0,0,swipe3_button_is_held_run

swipe3_end_anim_run:
			.byte	ANI_CLRFLAG
			.word	PF_STEALING
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_SETMODE
			.word	0
#			.byte	ANI_PREP_XITION
#			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

#FIX!!!  Need variety on butt fall back
#################################################
# defender got faked out		
			.globl	dfake1_butt_anim
			.byte	SCRIPT
dfake1_butt_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_H_FRNT_6
			.byte	0

			.byte	ANI_INTSTREAM,33,36

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			#sliding on ground
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,df1_fast
			.byte	ANI_INTSTREAM,4,3
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,df1_fast
			.byte	ANI_INTSTREAM,4,3
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,df1_fast
			.byte	ANI_INTSTREAM,4,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,df1_fast
			.byte	ANI_INTSTREAM,4,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,df1_fast
			.byte	ANI_INTSTREAM,4,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,df1_fast

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	0.35

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

df1_fast:	
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	.25
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dfake1_r_anim
			.byte	SCRIPT
dfake1_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_DFAKE1_R
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	.85

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	dfake1_l_anim
			.byte	SCRIPT
dfake1_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_DFAKE1_L
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	.85

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

###################################################
# JUMP SHOTS																		  #
###################################################
# Running Jumpshot 3
			.globl	shot_run_leaner_frwd_3
			.byte	SCRIPT
shot_run_leaner_frwd_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETFRICTION
			.float	0.25
			#.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot_run_leaner_frwd_3_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_RUN3_L
			.byte	0

			.byte	ANI_GOTO
			.word shot_run_leaner_frwd_3_hands_done

shot_run_leaner_frwd_3_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_RUN3_R
			.byte	0

shot_run_leaner_frwd_3_hands_done:
			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.45

			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,15,13

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word jumpshot_timeout

			.byte	ANI_INTSTREAM,6,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

###################################################
# Running Jumpshot 1
			.globl	shot_run_leaner_frwd
			.byte	SCRIPT
shot_run_leaner_frwd:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETFRICTION
			.float	0.5
			#.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot_run_leaner_frwd_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_RUN_L
			.byte	10

			.byte	ANI_GOTO
			.word shot_run_leaner_frwd_hands_done

shot_run_leaner_frwd_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_RUN_R
			.byte	10

shot_run_leaner_frwd_hands_done:
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.45

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,4,5

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,3,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

# Running Jumpshot 2
			.globl	shot_run_leaner_frwd_2
			.byte	SCRIPT
shot_run_leaner_frwd_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETFRICTION
			.float	0.25
			#.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot_run_leaner_frwd_2_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_RUN2_L
			.byte	0

			.byte	ANI_GOTO
			.word shot_run_leaner_frwd_2_hands_done

shot_run_leaner_frwd_2_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_RUN2_R
			.byte	0

shot_run_leaner_frwd_2_hands_done:
			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.45

			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,16,13

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word jumpshot_timeout

			.byte	ANI_INTSTREAM,5,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; running leaner shot #1

			.globl	shot_run_leaner_1
			.byte	SCRIPT
shot_run_leaner_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_INTTOSEQH,2
			.word	SEQ_J_RUN1_R,SEQ_J_RUN1_L
			.byte	0

			.byte	ANI_INTSTREAM,6,6

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.015

			.byte	ANI_INTSTREAM,12,19

# wait until shot button is released or a timeout has occured
			#.byte	ANI_WAITTRUE
			#.word jumpshot_timeout

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,5,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.45

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; running leaner shot #2 left (could be baseline leaner)

			.globl	shot_leaner_2_l_anim
			.byte	SCRIPT
shot_leaner_2_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_LEAN_L
			.byte	0

			.byte	ANI_INTSTREAM,8,6

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.015

			.byte	ANI_INTSTREAM,22,19

# wait until shot button is released or a timeout has occured
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.20

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; running leaner shot #2 right (could be baseline leaner)

			.globl	shot_leaner_2_r_anim
			.byte	SCRIPT
shot_leaner_2_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_LEAN_R
			.byte	0

			.byte	ANI_INTSTREAM,8,6

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.015

			.byte	ANI_INTSTREAM,22,19

# wait until shot button is released or a timeout has occured
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.20

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; running leaner shot #2 left (could be baseline leaner)

			.globl	shot_leaner_3_l_anim
			.byte	SCRIPT
shot_leaner_3_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_LEAN3L
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_SETVELS
			.float	0.40,0.0,0.15

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.005

			.byte	ANI_INTSTREAM,31,19

# wait until shot button is released or a timeout has occured
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.20

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; running leaner shot #2 right (could be baseline leaner)

			.globl	shot_leaner_3_r_anim
			.byte	SCRIPT
shot_leaner_3_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_LEAN3R
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_SETVELS
			.float	-0.40,0.0,0.15

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.005

			.byte	ANI_INTSTREAM,31,19

# wait until shot button is released or a timeout has occured
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.20

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; running leaner shot #2 left (could be baseline leaner)

			.globl	shot_leaner_4_l_anim
			.byte	SCRIPT
shot_leaner_4_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_LEAN4L
			.byte	0

			.byte	ANI_INTSTREAM,8,6

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.015

			.byte	ANI_INTSTREAM,22,19

# wait until shot button is released or a timeout has occured
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.20

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; running leaner shot #2 right (could be baseline leaner)

			.globl	shot_leaner_4_r_anim
			.byte	SCRIPT
shot_leaner_4_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_LEAN4R
			.byte	0

			.byte	ANI_INTSTREAM,8,6

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_SETFRICTION
			.float	0.015

			.byte	ANI_INTSTREAM,22,19

# wait until shot button is released or a timeout has occured
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.20

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl shot_hook_1
			.byte	SCRIPT
shot_hook_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING


			.byte	ANI_CODE_BNZ
			.word	maybe_hookshot_right,0,0,0,shot_hook_1_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_HOOK_L
			.byte	0

			.byte	ANI_GOTO
			.word shot_hook_1_hands_done

shot_hook_1_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_HOOK_R
			.byte	0

shot_hook_1_hands_done:
			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,8,4

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0
#			.byte	ANI_SETYVEL
#			.float	1.4

			.byte	ANI_INTSTREAM,6,6

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl shot_hook_2
			.byte	SCRIPT
shot_hook_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_CODE_BNZ
			.word	maybe_hookshot_right,0,0,0,shot_hook_2_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_HOK2_L
			.byte	0

			.byte	ANI_GOTO
			.word	shot_hook_2_hands_done

shot_hook_2_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_HOK2_R
			.byte	0

shot_hook_2_hands_done:
#			.byte	ANI_ZEROVELS
			.byte	ANI_SETFRICTION
			.float	0.15

			.byte	ANI_INTSTREAM,14,5

			.byte	ANI_CODE
			.word	SetShotVels,0,0,0
#			.byte	ANI_SETYVEL
#			.float	1.6

			.byte	ANI_INTSTREAM,14,9

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,19,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			#51 frames left
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl shot_hook_3
			.byte	SCRIPT
shot_hook_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETFRICTION
			.float	0.5

			.byte	ANI_CODE_BNZ
			.word	maybe_hookshot_right,0,0,0,shot_hook_3_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_HOK3_L
			.byte	0

			.byte	ANI_GOTO
			.word	shot_hook_3_hands_done

shot_hook_3_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_J_HOK3_R
			.byte	0

shot_hook_3_hands_done:

			.byte	ANI_INTSTREAM,10,3

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.45
			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,16,9

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,10,9

			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	shot_run_heave_1
			.globl	shot_stand_heave_1
			.byte	SCRIPT
shot_stand_heave_1:
shot_run_heave_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	rotate_to_face_rim,0,0,0

#			.byte	ANI_CODE
#			.word	face_rim,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

#			.byte	ANI_SETVELS
#			.float	0.0,0.0,0.65

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot_stand_heave_1_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_HVE1_L
			.byte	7

			.byte	ANI_GOTO
			.word	shot_stand_heave_1_hands_done

shot_stand_heave_1_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_HVE1_R
			.byte	7

shot_stand_heave_1_hands_done:
			.byte	ANI_CODE
			.word	SetShotVels,2,0,0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,4,3

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
	

#;;;;;;;;;;;;;;;;;

			.globl jumper_react_to_swat
			.byte	SCRIPT
jumper_react_to_swat:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_REACTB
			.byte	0

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_PREP_XITION
			.word	6,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; standing shot -- after a pump-fake

			.globl	shot_stand1_anim
			.byte	SCRIPT
shot_stand1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_FT1_SHTR,SEQ_FT1_SHTL
			.byte	4

			.byte	ANI_INTSTREAM,15,12

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING|PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	1.75

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; jumpshot 1 - Shoot the ball 1

			.globl	shot1_anim
			.byte	SCRIPT
shot1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot1_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT1_L
			.byte	22

			#.byte	ANI_NEWSEQ
			#.word	SEQ_J_SHT1_L

			.byte	ANI_GOTO
			.word	shot1_hands_done

shot1_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT1_R
			.byte	22

			#.byte	ANI_NEWSEQ
			#.word	SEQ_J_SHT1_R

shot1_hands_done:
			#.byte	ANI_SKIPFRAMES,22

			.byte	ANI_INTSTREAM,18,5

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot1_cont_fade
			
			.byte	ANI_GOTO
			.word shot1_cont_nofade

shot1_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot1_hand_r_fade

			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot1_hand_r_fade:
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot1_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht1_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot1_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_L
			.byte	ANI_SKIPFRAMES,40
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_L
			.byte	ANI_SKIPFRAMES,36
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_L
			.byte	ANI_SKIPFRAMES,32
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_L
			.byte	ANI_SKIPFRAMES,28
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_L
			.byte	ANI_SKIPFRAMES,24
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word 	sht1_end			

shot1_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_R
			.byte	ANI_SKIPFRAMES,40
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_R
			.byte	ANI_SKIPFRAMES,36
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_R
			.byte	ANI_SKIPFRAMES,32
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_R
			.byte	ANI_SKIPFRAMES,28
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT1_R
			.byte	ANI_SKIPFRAMES,24
			.byte	1

# play pump fake part of anim
			#.byte	ANI_INTSTREAM,18,10

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht1_end			

sht1_cont:
			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,8,5

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,4,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,19,AA_TOGROUND

sht1_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; jumpshot 2 - player holds his shooting hand in the air after the shot

			.globl	shot2_anim
			.byte	SCRIPT
shot2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot2_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_L

			.byte	ANI_GOTO
			.word shot2_hands_done

shot2_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0
			
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_R

shot2_hands_done:
			.byte	ANI_INTSTREAM,8,7

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot2_cont_fade
			
			.byte	ANI_GOTO
			.word shot2_cont_nofade

shot2_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot2_hand_r_fade

			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot2_hand_r_fade:
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot2_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht2_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot2_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_L
			.byte	ANI_SKIPFRAMES,8
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_L
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_L
			.byte	ANI_SKIPFRAMES,4
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_L
			.byte	ANI_SKIPFRAMES,2
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_L
			#.byte	ANI_SKIPFRAMES,0
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht2_end			

shot2_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_R
			.byte	ANI_SKIPFRAMES,8
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_R
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_R
			.byte	ANI_SKIPFRAMES,4
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_R
			.byte	ANI_SKIPFRAMES,2
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT2_R
			#.byte	ANI_SKIPFRAMES,0
			.byte	1

# play pump fake part of anim
			#.byte	ANI_INTSTREAM,18,10

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht2_end			

sht2_cont:
			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,3,6

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,2,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
#fix!!!
			.byte	ANI_STREAMTOEND
			.float	2.0

sht2_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING|PF_CELEBRATE

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	6,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;; JumpShot 3
## Shot 3 (chad)
# Shoot the ball 1
			.globl	shot3_anim
			.byte	SCRIPT
shot3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot3_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_L

			.byte	ANI_GOTO
			.word shot3_hands_done

shot3_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_R

shot3_hands_done:
			#.byte	ANI_SKIPFRAMES,22

			.byte	ANI_INTSTREAM,29,7

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot3_cont_fade

			.byte	ANI_GOTO
			.word shot3_cont_nofade

shot3_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot3_hand_r_fade

			.byte	ANI_CODE
			.word SetShotVels,1,0,0

# goto the fade away jump shot
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot3_hand_r_fade:
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

# goto the fade away jump shot
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot3_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht3_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot3_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_L
			.byte	ANI_SKIPFRAMES,29
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_L
			.byte	ANI_SKIPFRAMES,24
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_L
			.byte	ANI_SKIPFRAMES,19
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_L
			.byte	ANI_SKIPFRAMES,14
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_L
			.byte	ANI_SKIPFRAMES,9
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht3_end			

shot3_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_R
			.byte	ANI_SKIPFRAMES,29
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_R
			.byte	ANI_SKIPFRAMES,24
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_R
			.byte	ANI_SKIPFRAMES,19
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_R
			.byte	ANI_SKIPFRAMES,14
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT3_R
			.byte	ANI_SKIPFRAMES,9
			.byte	1

# play pump fake part of anim
			#.byte	ANI_INTSTREAM,18,10

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht3_end			

sht3_cont:
			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,7,5

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,5,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,23,AA_TOGROUND

sht3_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;; JumpShot 4
# Willie
			.globl	shot4_anim
			.byte	SCRIPT
shot4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot4_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT4_L
			.byte	0

			.byte	ANI_GOTO
			.word shot4_hands_done

shot4_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT4_R
			.byte	0

shot4_hands_done:
			.byte	ANI_INTSTREAM,13,5

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot4_cont_fade

			.byte	ANI_GOTO
			.word shot4_cont_nofade

shot4_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot4_hand_r_fade

			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot4_hand_r_fade:
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot4_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht4_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot4_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_L
			.byte	ANI_SKIPFRAMES,10
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_L
			.byte	ANI_SKIPFRAMES,8
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_L
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_L
			.byte	ANI_SKIPFRAMES,4
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_L
			.byte	ANI_SKIPFRAMES,2
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht4_end			

shot4_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_R
			.byte	ANI_SKIPFRAMES,10
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_R
			.byte	ANI_SKIPFRAMES,8
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_R
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_R
			.byte	ANI_SKIPFRAMES,4
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT4_R
			.byte	ANI_SKIPFRAMES,2
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht4_end			

sht4_cont:
			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,16,10

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,3,3

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,9,5

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

sht4_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;; JumpShot 5
# Willie
			.globl	shot5_anim
			.byte	SCRIPT
shot5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot5_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT5_L
			.byte	0

			.byte	ANI_GOTO
			.word shot5_hands_done

shot5_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT5_R
			.byte	0

shot5_hands_done:
			.byte	ANI_INTSTREAM,14,5

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot5_cont_fade
			
			.byte	ANI_GOTO
			.word shot5_cont_nofade

shot5_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot5_hand_r_fade

			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot5_hand_r_fade:
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot5_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht5_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot5_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_L
			.byte	ANI_SKIPFRAMES,12
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_L
			.byte	ANI_SKIPFRAMES,9
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_L
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_L
			.byte	ANI_SKIPFRAMES,3
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_L
			.byte	ANI_SKIPFRAMES,1
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht5_end			

shot5_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_R
			.byte	ANI_SKIPFRAMES,12
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_R
			.byte	ANI_SKIPFRAMES,9
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_R
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_R
			.byte	ANI_SKIPFRAMES,3
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT5_R
			.byte	ANI_SKIPFRAMES,1
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht5_end			

sht5_cont:
			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,17,8

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,6,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_CODE
			.word shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

sht5_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;; JumpShot 6
# Willie
			.globl	shot6_anim
			.byte	SCRIPT
shot6_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot6_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT6_L
			.byte	0

			.byte	ANI_GOTO
			.word	shot6_hands_done

shot6_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT6_R
			.byte	0

shot6_hands_done:
			.byte	ANI_INTSTREAM,14,5

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot6_cont_fade
			
			.byte	ANI_GOTO
			.word	shot6_cont_nofade

shot6_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot6_hand_r_fade

			.byte	ANI_CODE
			.word	SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word	shot_fade1_from_jumpshot

shot6_hand_r_fade:
			.byte	ANI_CODE
			.word	SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word	shot_fade1_from_jumpshot

shot6_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht6_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot6_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_L
			.byte	ANI_SKIPFRAMES,12
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_L
			.byte	ANI_SKIPFRAMES,9
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_L
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_L
			.byte	ANI_SKIPFRAMES,3
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_L
			.byte	ANI_SKIPFRAMES,1
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word	sht6_end			

shot6_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_R
			.byte	ANI_SKIPFRAMES,12
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_R
			.byte	ANI_SKIPFRAMES,9
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_R
			.byte	ANI_SKIPFRAMES,6
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_R
			.byte	ANI_SKIPFRAMES,3
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT6_R
			.byte	ANI_SKIPFRAMES,1
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word	sht6_end			

sht6_cont:
			.byte	ANI_CODE
			.word	SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,17,8

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,6,4

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

sht6_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; jumpshot 8 - Shoot the ball 8
# Greg C.
			.globl	shot8_anim
			.byte	SCRIPT
shot8_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot8_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT8_L
			.byte	0

			.byte	ANI_GOTO
			.word	shot8_hands_done

shot8_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_J_SHT8_R
			.byte	0

shot8_hands_done:
			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_CODE_BNZ
			.word	maybe_fadeshot,0,0,0,shot8_cont_fade
			
			.byte	ANI_GOTO
			.word shot8_cont_nofade

shot8_cont_fade:
			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot8_hand_r_fade

			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_L
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot8_hand_r_fade:
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_JSHT1F_R
			.byte	30

			.byte	ANI_GOTO
			.word shot_fade1_from_jumpshot

shot8_cont_nofade:
			.byte	ANI_CODE_BNZ
			.word	maybe_pumpfake,BUT_A,0,0,sht8_cont

			.byte	ANI_CODE
			.word	pump_fake_speech,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot8_fake_r

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_L
			.byte	ANI_SKIPFRAMES,5
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_L
			.byte	ANI_SKIPFRAMES,4
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_L
			.byte	ANI_SKIPFRAMES,3
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_L
			.byte	ANI_SKIPFRAMES,2
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_L
			.byte	ANI_SKIPFRAMES,1
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht8_end			

shot8_fake_r:
			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_R
			.byte	ANI_SKIPFRAMES,5
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_R
			.byte	ANI_SKIPFRAMES,4
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_R
			.byte	ANI_SKIPFRAMES,3
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_R
			.byte	ANI_SKIPFRAMES,2
			.byte	1

			.byte	ANI_NEWSEQ
			.word	SEQ_J_SHT8_R
			.byte	ANI_SKIPFRAMES,1
			.byte	1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,ready_wb_2_fake_step

			.byte	ANI_GOTO
			.word sht8_end			

sht8_cont:
			.byte	ANI_CODE
			.word SetShotVels,0,0,0

			.byte	ANI_INTSTREAM,8,8

# wait until shot button is released or a timeout has occured
			.byte	ANI_WAITTRUE
			.word	jumpshot_timeout

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

sht8_end:
			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float 1.5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

###########################################################
# Shoot the ball 1 fade
			.globl	shot_fade1_anim
			.byte	SCRIPT
shot_fade1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot_fade1_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_JSHT1F_L
			.byte	0

			.byte	ANI_GOTO
			.word	shot_fade1_hands_done

shot_fade1_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_JSHT1F_R
			.byte	0

shot_fade1_hands_done:
			#.byte	ANI_CODE_BNZ
			#.word	plyr_rand,2,0,0,shot_fade1_no_sp_fades

#shot_fade1_no_sp_fades:
			.byte	ANI_INTSTREAM,30,10

#shot_fade1_from_jumpshot:
# set fade away vels
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

shot_fade1_from_jumpshot:

			.byte	ANI_INTSTREAM,11,15

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			#.byte	ANI_SETMODE
			#.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_INTSTREAM,20,10

			#.byte	ANI_CODE
			#.word clear_pumpfake,0,0,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

# Shoot the ball 2 fade
			.globl	shot_fade2_anim
			.byte	SCRIPT
shot_fade2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,shot_fade2_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_JSHT2F_L
			.byte	0

			.byte	ANI_GOTO
			.word shot_fade2_hands_done

shot_fade2_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_JSHT2F_R
			.byte	0

shot_fade2_hands_done:
			#.byte	ANI_CODE_BNZ
			#.word	plyr_rand,2,0,0,shot_fade2_no_sp_fades

#shot_fade2_no_sp_fades:
			.byte	ANI_INTSTREAM,6,6

#shot_fade2_from_jumpshot:
# set fade away vels
			.byte	ANI_CODE
			.word SetShotVels,1,0,0

shot_fade2_from_jumpshot:

			.byte	ANI_INTSTREAM,11,15

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,7,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_JIFSEQ
			.word	SEQ_JSHT2F_R,shot_fade2_thack_r

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_GOTO
			.word shot_fade2_thack_done

shot_fade2_thack_r:
			.byte	ANI_TURNHACK
			.word	-256

shot_fade2_thack_done:
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;

			.globl	shot_behind_rim_anim
			.byte	SCRIPT
shot_behind_rim_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFLAG
			.word	PF_SHOOTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_is_righthanded,0,0,0,sf3_hand_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_JSHT1F_L
			.byte	0

			.byte	ANI_GOTO
			.word	sf3_hands_done

sf3_hand_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_JSHT1F_R
			.byte	0

sf3_hands_done:
			.byte	ANI_INTSTREAM,30,10

#shot_fade1_from_jumpshot:
# set fade away vels
			.byte	ANI_CODE
			.word	SetShotVels,3,0,0

			.byte	ANI_INTSTREAM,11,15

# now shoot the ball
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	shoot_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			.byte	ANI_CLRFLAG
			.word	PF_SHOOTING

			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


##########################
# PRETIPOFF	             #
##########################

#;;;;;;;;;;;;;;;;; player 1 pre-tipoff seq. (runs out last)

			.globl	pre_tip1_anim
			.byte	SCRIPT
pre_tip1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_PRE_TIP1

			.byte	ANI_RUNTOFRAME,0,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	2.2

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	2.2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_PRE_TIP1
			.byte	0

			.byte	ANI_INTSTREAM,39,114

			.byte	ANI_CODE
			.word	announce_players_name,0,0,0

			.byte	ANI_STREAMTOEND
			.float	3.0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player 2 pre-tipoff seq.

			.globl	pre_tip4_anim
			.byte	SCRIPT
pre_tip4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_PRE_TIP4

			.byte	ANI_RUNTOFRAME,0,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.3

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.3

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_PRE_TIP4
			.byte	0

			.byte	ANI_INTSTREAM,36,114

			.byte	ANI_CODE
			.word	announce_players_name,0,0,0

			.byte	ANI_STREAMTOEND
			.float	3.0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player 3 pre-tipoff seq.

			.globl	pre_tip3_anim
			.byte	SCRIPT
pre_tip3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_PRE_TIP3

			.byte	ANI_RUNTOFRAME,0,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_PRE_TIP3
			.byte	0

			.byte	ANI_INTSTREAM,28,80

			.byte	ANI_CODE
			.word	announce_players_name,0,0,0

			.byte	ANI_STREAMTOEND
			.float	3.0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player 4 pre-tipoff seq. (runs out first)

			.globl	pre_tip5_anim
			.byte	SCRIPT
pre_tip5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT
			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_PRE_TIP5

			.byte	ANI_RUNTOFRAME,0,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_SBREATH1

			.byte	ANI_RUNTOFRAME,0,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_PRE_TIP5
			.byte	0

			.byte	ANI_INTSTREAM,14,46

			.byte	ANI_CODE
			.word	announce_players_name,0,0,0

			.byte	ANI_INTSTREAM,8,30

			.byte	ANI_STREAMTOEND
			.float	3.0

			.byte	ANI_END


##########################
# TIPOFF	             #
##########################

#;;;;;;;;;;;;;;;;; referee walks on, throws up ball, runs away

			.globl	referee_tipoff_anim
			.globl	referee_tipoff_anim_delay
			.byte	SCRIPT
referee_tipoff_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
			
			.byte	ANI_NEWSEQ
			.word	SEQ_REFJB1_R

			.byte	ANI_CODE
			.word	do_jump_meters_flag,0,0,0

			.byte	1

			.byte	ANI_WAITTRUE
			.word	referee_wait_timeout

referee_tipoff_anim_delay:
			.byte	ANI_INTSTREAM,60,42
			.byte	ANI_INTSTREAM,60,42
#			.byte	ANI_RUNTOFRAME,120,1

			.byte	ANI_CODE
			.word	do_jump_meters_flag,1,0,0

			.byte	ANI_INTSTREAM,127,92
			.byte	ANI_INTSTREAM,33,16

			.byte	ANI_CODE
			.word	referee_release_ball,0,0,0

			.byte	ANI_STREAMTOEND
			.float	0.40

			.byte	ANI_ZEROVELS

referee_loop:
			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SBREATH1
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	3.0

			.byte	ANI_GOTO
			.word	referee_loop

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player stands ready to jump for the tipoff

			.globl	tipoff_jump_ready_l_anim
			.byte	SCRIPT
tipoff_jump_ready_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_T_JRDY_L

			.byte	ANI_STREAMTOEND
			.float	1.86

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player stands ready to jump for the tipoff

			.globl	tipoff_jump_ready_r_anim
			.byte	SCRIPT
tipoff_jump_ready_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_T_JRDY_R

			.byte	ANI_STREAMTOEND
			.float	1.80

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player jumps for the tipped off ball and MISSES (right side)

			.globl	tipoff_miss_r_anim
			.byte	SCRIPT
tipoff_miss_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_T_JMP3_R
			.byte	15

			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_SETYVEL
			.float	1.2
			
			#jumping up to swat at ball
			.byte	ANI_INTSTREAM,15,9

			#swat for ball
			.byte	ANI_INTSTREAM,8,7

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOFRAME,108,1

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player jumps for the tip'd off ball and MISSES (left side)

			.globl	tipoff_miss_l_anim
			.byte	SCRIPT
tipoff_miss_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_T_JMP3_L
			.byte	15

			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_SETYVEL
			.float	1.2
			
			#jumping up to swat at ball
			.byte	ANI_INTSTREAM,15,9

			#swat for ball
			.byte	ANI_INTSTREAM,8,7

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOFRAME,108,1

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player jumps for the tip'd off ball (left)

			.globl	tipoff_jump_up_l_anim
			.byte	SCRIPT
tipoff_jump_up_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_T_JMP1_L
			.byte	0

			.byte	ANI_INTSTREAM,8,7

			.byte	ANI_CODE
			.word	SetTipoffVels,20,0,0
			
			#jumping up to swat at ball
			.byte	ANI_INTSTREAM,20,19

			.byte	ANI_CODE
			.word	swat_tipoff_ball,0,0,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.0
#			.byte	ANI_SETYVEL
#			.float	0.0

			#swat for ball
			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player jumps for the tip'd off ball (right)

			.globl	tipoff_jump_up_r_anim
			.byte	SCRIPT
tipoff_jump_up_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_T_JMP1_R
			.byte	0

			.byte	ANI_INTSTREAM,8,7
			
			.byte	ANI_CODE
			.word	SetTipoffVels,20,0,0

			#jumping up to swat at ball
			.byte	ANI_INTSTREAM,20,19

			.byte	ANI_CODE
			.word	swat_tipoff_ball,0,0,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.0
#			.byte	ANI_SETYVEL
#			.float	0.0


			#swat for ball
			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.60

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; Randomly select a tipoff stance anim

			.globl	tipoff_random_ready_anim
			.byte	SCRIPT
tipoff_random_ready_anim:
			.byte	ANI_CODE
			.word	pick_random_tipoff_anim,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; general stance for tipoff 'left side'

			.globl	tipoff_ready_stance_anim
			.byte	SCRIPT
tipoff_ready_stance_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,tipoff_ready_r_anim
			
			.byte	ANI_INTTOSEQ,14
			.word	SEQ_T_RDY1_L
			.byte	0
			
			.byte	ANI_STREAMTOEND
			.float	1.10
trl_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_T_RDY1_L

			.byte	ANI_STREAMTOEND
			.float	1.10

			.byte	ANI_GOTO
			.word	trl_anim

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; general stance for tipoff 'right side'

			.byte	SCRIPT
tipoff_ready_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,14
			.word	SEQ_T_RDY1_R
			.byte	0
			
			.byte	ANI_STREAMTOEND
			.float	1.10
trr_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_T_RDY1_R

			.byte	ANI_STREAMTOEND
			.float	1.10

			.byte	ANI_GOTO
			.word	trr_anim

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player stretches elbows waiting for tipoff

			.globl	tipoff_ready_1_anim
			.byte	SCRIPT
tipoff_ready_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_T_RDY2
			.byte	0
			
			.byte	ANI_STREAMTOEND
			.float	1.69

			.byte	ANI_GOTO
			.word	tipoff_ready_stance_anim

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player stretches legs waiting for tipoff
				
			.globl	tipoff_ready_2_anim
			.byte	SCRIPT
tipoff_ready_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_T_RDY3
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	2.56

			.byte	ANI_GOTO
			.word	tipoff_ready_stance_anim

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player readyies for tipoff
				
			.globl	tipoff_ready_3_anim
			.byte	SCRIPT
tipoff_ready_3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_TIP_WUP1
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	5.0

			.byte	ANI_GOTO
			.word	tipoff_ready_stance_anim

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player readyies for tipoff
				
			.globl	tipoff_ready_4_anim
			.byte	SCRIPT
tipoff_ready_4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_TIP_WUP3
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	5.0

			.byte	ANI_GOTO
			.word	tipoff_ready_stance_anim

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player readyies for tipoff
				
			.globl	tipoff_ready_5_anim
			.byte	SCRIPT
tipoff_ready_5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_TIP_WUP5
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	5.0

			.byte	ANI_GOTO
			.word	tipoff_ready_stance_anim

			.byte	ANI_END

#######################################################
# PUT BACK SHOTS                                      #
#######################################################

			.globl	put_back_dunk_1
			.byte	SCRIPT
put_back_dunk_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_DUNKER
#			.word	PF_BOTH_HANDS|PF_ALLYDUNKER

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUI1_R
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL
			.word	35,34,JOINT_BWRIST,-1

			.byte	ANI_INTSTREAM,10,24

			.byte	ANI_INTSTREAM,15,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	put_back_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,22,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER
#			.byte	ANI_CLRFLAG
#			.word	PF_ALLYDUNKER

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;  Higher version than put_back 1

			.globl	put_back_dunk_1a
			.byte	SCRIPT
put_back_dunk_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_DUNKER
#			.word	PF_BOTH_HANDS|PF_ALLYDUNKER

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUI1_R
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL
			.word	45,34,JOINT_BWRIST,-1

			.byte	ANI_INTSTREAM,10,34

			.byte	ANI_INTSTREAM,15,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	put_back_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,22,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER
#			.byte	ANI_CLRFLAG
#			.word	PF_ALLYDUNKER

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;  another put back dunk -- one handed

			.globl	put_back_dunk_2
			.byte	SCRIPT
put_back_dunk_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			#.byte	ANI_CODE
			#.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_1BUT_R,SEQ_K_1BUT_L
			.byte	2

			.byte	ANI_INTSTREAM,4,3

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL2
			.word	40,35,SEQ_K_1BUT_R,SEQ_K_1BUT_L

			.byte	ANI_INTSTREAM,21,32

			.byte	ANI_INTSTREAM,12,7

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	put_back_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,7,13

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER
#			.byte	ANI_CLRFLAG
#			.word	PF_ALLYDUNKER

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;  another put back dunk -- two handed

			.globl	put_back_dunk_3
			.byte	SCRIPT
put_back_dunk_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_DUNKER
#			.word	PF_BOTH_HANDS|PF_ALLYDUNKER

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_NDUNK5
			.byte	9

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL
			.word	40,46,JOINT_BWRIST,-1

			.byte	ANI_INTSTREAM,15,22

			.byte	ANI_INTSTREAM,16,17

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	put_back_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,1,3

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS|PF_DUNKER
#			.word	PF_BOTH_HANDS|PF_ALLYDUNKER

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;  another put back dunk -- one handed

#			.globl	put_back_dunk_4
#			.byte	SCRIPT
#put_back_dunk_4:
#			.byte	ANI_CODE
#			.word	set_ball_hand,RIGHT_HAND,0,0
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_SETFRICTION
#			.float	0.09
#
#			.byte	ANI_SETFLAG
#			.word	PF_DUNKER
##			.word	PF_BOTH_HANDS|PF_ALLYDUNKER
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_K_KISS1R
#			.byte	0
#
#			.byte	ANI_INTSTREAM,4,2
#
#			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
#			.float	1.5,1.5,2.0
#
#			.byte	ANI_DUNK_BALL
#			.word	40,40,JOINT_RWRIST,-1
#
#			.byte	ANI_INTSTREAM,18,32
#
#			.byte	ANI_INTSTREAM,15,7
#
#			.byte	ANI_ATTACK_OFF
#
#			.byte	ANI_CODE
#			.word	SetHangOnRim,YES,NO,0
#
#			.byte	ANI_SETYVEL
#			.float	0.0
#
#			.byte	ANI_CODE
#			.word	put_back_ball_speech,0,0,0
#
#			.byte	ANI_CODE
#			.word	Shot_Release,0,0,0
#
#			#hang on rim
#			.byte	ANI_INTSTREAM,3,4
#
#			.byte	ANI_CODE
#			.word	SetHangOnRim,NO,NO,0
#
#			.byte	ANI_INTSTREAM,9,AA_TOGROUND
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CLRFLAG
#			.word	PF_DUNKER
#
#			.byte	ANI_STREAMTOEND
#			.float	0.50
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#
#			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;  another put back dunk -- two handed

			.globl	put_back_dunk_5
			.byte	SCRIPT
put_back_dunk_5:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_TOMA_R
			.byte	0

			.byte	ANI_INTSTREAM,10,2

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL
			.word	43,40,JOINT_RWRIST,-1

			.byte	ANI_INTSTREAM,23,35

			.byte	ANI_INTSTREAM,16,7

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	put_back_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,1,1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#######################################################
# ALLYOPPS - DOUBLE DUNKS                             #
#######################################################
# player goes up 2 hands above head ready to catch ball
			.globl	ddunk_1
			.byte	SCRIPT
ddunk_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_ALLYDUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_QUIK_6
			.byte	0

			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL2
			.word	90,38,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,9,50
			.byte	ANI_INTSTREAM,1,10
			.byte	ANI_INTSTREAM,1,20
			.byte	ANI_INTSTREAM,17,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#############################################
# player winds 1 hand and holds then slams
			.globl	ddunk_2
			.byte	SCRIPT
ddunk_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			.byte	ANI_SETFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_WIND_L
			.byte	0

			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL
			.word	90,24,JOINT_LWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,11,75
			.byte	ANI_INTSTREAM,8,15

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			#hang on rim
			.byte	ANI_INTSTREAM,2,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

##################################################

			.globl run_forward_spinpass_bb_1
			.byte	SCRIPT
run_forward_spinpass_bb_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word face_receiver,0,0,0

			.byte	ANI_SETFRICTION
			.float 0.20

			.byte	ANI_SETFLAG
			.word	PF_PASSING|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_P_PAS1_R,SEQ_P_PAS1_L
			.byte	0

			.byte	ANI_INTSTREAM,19,8

			.byte	ANI_CODE
			.word	PassRelease,0,0,0

			.byte	ANI_SETFRICTION
			.float 0.0

			.byte	ANI_CLRFLAG
			.word	PF_PASSING|PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float 0.4

			.byte ANI_CODE
			.word get_backboard_ally,0,0,0
			
			#.byte ANI_GOTO
			#.word allyoop_x
			#.word allyoop_2

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;

			.globl	allyoop_x
			.byte	SCRIPT
allyoop_x:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,2
			.word	SEQ_K_1HND_R,SEQ_K_1HND_L
			.byte 0

			.byte	ANI_INTSTREAM,11,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL2
			.word	50,41,SEQ_K_1HND_R,SEQ_K_1HND_L

			.byte	ANI_INTSTREAM,7,30

			.byte	ANI_INTSTREAM,23,20

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,5,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_SETMODE
			.word	0
			
			.byte ANI_STREAMTOEND
			.float 0.65

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player cocks ball really low, then slams

			.globl	allyoop_1
			.byte	SCRIPT
allyoop_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_ALLYDUNKER|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,2
			.word	SEQ_K_ALY3_R,SEQ_K_ALY3_L
			.byte	0

			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_DUNK_BALL2
			.word	70,45,SEQ_K_ALY3_R,SEQ_K_ALY3_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_INTSTREAM,8,10
			.byte	ANI_INTSTREAM,8,50
			.byte	ANI_INTSTREAM,17,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;
# player goes up 2 hands above head ready to catch ball
			.globl	allyoop_2
			.byte	SCRIPT
allyoop_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_ALLYDUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_QUIK_6
			.byte	0

			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL2
			.word	70,38,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,9,60
			.byte	ANI_INTSTREAM,19,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;
# player goes up 2 hands above head ready to catch ball
			.globl	allyoop_3
			.byte	SCRIPT
allyoop_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_ALLYDUNKER|PF_NO_LOOK
			
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_2HREV2
			.byte	0

			.byte	ANI_INTSTREAM,4,4
#			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL2
			.word	70,23,JOINT_BWRIST,-1
#			.word	70,46,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

#			.byte	ANI_INTSTREAM,20,55
#			.byte	ANI_INTSTREAM,16,15
			.byte	ANI_INTSTREAM,10,55
			.byte	ANI_INTSTREAM,8,15

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			#hang on rim
			.byte	ANI_INTSTREAM,7,12
 #			.byte	ANI_INTSTREAM,13,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,23,AA_TOGROUND
#			.byte	ANI_INTSTREAM,42,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_TURNHACK
			.word	512
	
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;
# player winds 1 hand and holds then slams
			.globl	allyoop_4
			.byte	SCRIPT
allyoop_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			.byte	ANI_SETFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_WIND_L
			.byte	0

			.byte	ANI_INTSTREAM,4,4
#			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL
			.word	60,24,JOINT_LWRIST,-1
#			.word	60,45,JOINT_LWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,11,45
			.byte	ANI_INTSTREAM,8,15
#			.byte	ANI_INTSTREAM,20,45
#			.byte	ANI_INTSTREAM,15,15

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			#hang on rim
			.byte	ANI_INTSTREAM,2,6
#			.byte	ANI_INTSTREAM,5,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
#			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;
# player winds 1 hand and holds then slams
			.globl	allyoop_4a
			.byte	SCRIPT
allyoop_4a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			.byte	ANI_SETFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_WIND_R
			.byte	0

			.byte	ANI_INTSTREAM,4,4
#			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_CODE
			.word	flash_plyr,-1,0,0

			.byte	ANI_DUNK_BALL
			.word	60,24,JOINT_RWRIST,-1
#			.word	60,45,JOINT_RWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,11,45
			.byte	ANI_INTSTREAM,8,15
#			.byte	ANI_INTSTREAM,20,45
#			.byte	ANI_INTSTREAM,15,15

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			#hang on rim
			.byte	ANI_INTSTREAM,2,6
#			.byte	ANI_INTSTREAM,5,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
#			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_ALLYDUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

##########################
# RIM HANGS              #
##########################

			.globl	rim_hang_2h1_anim
			.byte	SCRIPT
rim_hang_2h1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT
			#|MODE_ANCHOR

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_K_HNG3
			.byte 0

			.byte	ANI_INTSTREAM,41,60

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	1
			.byte	ANI_SETYVEL
			.float	-0.25

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_ZEROVELS
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;

			.globl	rim_hang_2h2_anim
			.byte	SCRIPT
rim_hang_2h2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_K_HNG6
			.byte	0

			.byte	ANI_INTSTREAM,49,65

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	1

			.byte	ANI_SETYVEL
			.float	-0.35

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_ZEROVELS
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;

			.globl	rim_hang_1h1_anim
			.byte	SCRIPT
rim_hang_1h1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_K_HNG5_R,SEQ_K_HNG5_L
			.byte	0

			#.byte	ANI_CODE
			#.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,34,60

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	1

			.byte	ANI_SETYVEL
			.float	-0.35

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_INTSTREAM,10,15
#			.float	.50

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#			.globl	rim_hang_1h1_anim
#			.byte	SCRIPT
#rim_hang_1h1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CODE
#			.word	SetHangOnRim,YES,NO,0
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_INTTOSEQH,3
#			.word	SEQ_K_HNG5_R,SEQ_K_HNG5_L
#			.byte	0
#
#			.byte	ANI_CODE
#			.word	Shot_Release,0,0,0
#
#			.byte	ANI_INTSTREAM,35,60
#
#			.byte	ANI_CODE
#			.word	SetHangOnRim,NO,NO,0
#
#			.byte	ANI_INTSTREAM,6,AA_TOGROUND
#
#			.byte	ANI_CLRFLAG
#			.word	PF_DUNKER
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_INTSTREAM,10,15
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#
#			.byte	ANI_END

##########################
# DUNKS                  #
##########################
#;;;;;;;;;;;;;;;;; player cocks ball really low, then slams

			.globl	dunk_windup_1
			.byte	SCRIPT
dunk_windup_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_ALY3_R,SEQ_K_ALY3_L
			.byte	0

			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_DUNK_BALL2
			.word	30,45,SEQ_K_ALY3_R,SEQ_K_ALY3_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,33,30
			#.byte	ANI_INTSTREAM,8,10
			#.byte	ANI_INTSTREAM,8,40
			#.byte	ANI_INTSTREAM,17,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	dunk_ball_speech,0,0,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	0.50

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_kwin2
			.byte	SCRIPT
dunk_kwin2:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_WIN2
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_DUNK_BALL
			.word	62,42,JOINT_RWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	5.0,10.0,2.0

			.byte	ANI_INTSTREAM,18,50
			.byte	ANI_INTSTREAM,15,12

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_INTSTREAM,4,8

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_kwin2a
			.byte	SCRIPT
dunk_kwin2a:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_WIN2
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_DUNK_BALL
			.word	65,42,JOINT_RWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	5.0,10.0,2.0

			.byte	ANI_INTSTREAM,10,47
			.byte	ANI_INTSTREAM,23,18

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_INTSTREAM,4,8

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_kwin2b
			.byte	SCRIPT
dunk_kwin2b:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_WIN2
			.byte	0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_DUNK_BALL
			.word	75,42,JOINT_RWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	5.0,10.0,2.0

			.byte	ANI_INTSTREAM,17,37
			.byte	ANI_INTSTREAM,5,30
			.byte	ANI_INTSTREAM,11,8

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,0	#one-hand

			#hang on rim
			.byte	ANI_INTSTREAM,4,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_INTSTREAM,4,8

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dunk_quick_1
			.byte	SCRIPT
dunk_quick_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUI1_R
			.byte	0

			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_DUNK_BALL
			.word	25,34,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,24,24

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,22,25

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

#			.byte	ANI_TURNHACK
#			.word	-256

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dunk_quick_2
			.byte	SCRIPT
dunk_quick_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUI2RV
			.byte	0

			.byte	ANI_INTSTREAM,8,4

			.byte	ANI_DUNK_BALL
			.word	25,28,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,24,24

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,20,25

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,16,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

#			.byte	ANI_TURNHACK
#			.word	-256

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dunk_quick_3
			.byte	SCRIPT
dunk_quick_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_quick_3_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUI3_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_quick_3_hands_done

dunk_quick_3_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUI3_R
			.byte	0

dunk_quick_3_hands_done:			
			.byte	ANI_INTSTREAM,11,4

			.byte	ANI_JIFSEQ
			.word	SEQ_K_QUI3_R,dunk_quick_3_rwrist

			.byte	ANI_DUNK_BALL
			.word	35,18,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_quick_3_wrist_done

dunk_quick_3_rwrist:
			.byte	ANI_DUNK_BALL
			.word	35,18,JOINT_RWRIST,-1

dunk_quick_3_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,34
			#frame 37

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,11,15

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,3,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_JIFSEQ
			.word	SEQ_K_QUI3_R,dunk_quick_3_r_thack

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_GOTO
			.word	dunk_quick_3_r_thack_done

dunk_quick_3_r_thack:
			.byte	ANI_TURNHACK
			.word	256

dunk_quick_3_r_thack_done:
			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dunk_quick_4
			.byte	SCRIPT
dunk_quick_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_QUIK_6
			.byte	0

			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_DUNK_BALL
			.word	25,38,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,28,24

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;

			.globl dunk_360_3
			.byte	SCRIPT
dunk_360_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

#			.byte	ANI_SETVELS
#			.float	0.0,0.0,1.8

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_360_3_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_3603_L
			.byte	11

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_DUNK_BALL
			.word	45,57,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_360_3_cont

dunk_360_3_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_3603_R
			.byte	11

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_DUNK_BALL
			.word	45,57,JOINT_RWRIST,-1

dunk_360_3_cont:
			.byte	ANI_INTSTREAM,38,45

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang
			.byte	ANI_INTSTREAM,1,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;

			.globl	dunk_360_3a
			.byte	SCRIPT
dunk_360_3a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_360_3a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_3603_L
			.byte	11

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_DUNK_BALL
			.word	65,57,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_360_3a_cont

dunk_360_3a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_3603_R
			.byte	11

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_DUNK_BALL
			.word	65,57,JOINT_RWRIST,-1

dunk_360_3a_cont:

			#wind up
			.byte	ANI_INTSTREAM,17,37

			#hold
			.byte	ANI_INTSTREAM,4,8

			#slam ball
			.byte	ANI_INTSTREAM,18,19

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,1,5

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_2hrev_1
			.byte	SCRIPT
dunk_2hrev_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK|PF_NO_LOOK

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HREV1
			.byte	0

			.byte	ANI_INTSTREAM,5,6
#			.byte	ANI_INTSTREAM,16,6

			.byte	ANI_DUNK_BALL
			.word	42,25,JOINT_BWRIST,-1
#			.word	42,50,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	5.0,10.0,2.0

			.byte	ANI_INTSTREAM,19,41
#			.byte	ANI_INTSTREAM,34,41

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,10,10
#			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND
#			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

#			.byte	ANI_PREP_XITION
#			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_2hrev_1a
			.byte	SCRIPT
dunk_2hrev_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_SETFRICTION
			.float	0.09

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HREV1
			.byte	0

			.byte	ANI_INTSTREAM,5,6
#			.byte	ANI_INTSTREAM,16,6

			.byte	ANI_DUNK_BALL
			.word	62,25,JOINT_BWRIST,-1
#			.word	62,50,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	5.0,10.0,2.0

			.byte	ANI_INTSTREAM,19,61
#			.byte	ANI_INTSTREAM,34,61

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,10,10
#			.byte	ANI_INTSTREAM,20,10

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND
#			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_2hrev_2
			.byte	SCRIPT
dunk_2hrev_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_2HREV2
			.byte	0

			.byte	ANI_INTSTREAM,4,6
#			.byte	ANI_INTSTREAM,10,6

			.byte	ANI_DUNK_BALL
			.word	50,23,JOINT_BWRIST,-1
#			.word	50,46,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,49
#			.byte	ANI_INTSTREAM,36,49

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,6,12
#			.byte	ANI_INTSTREAM,13,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,23,AA_TOGROUND
#			.byte	ANI_INTSTREAM,42,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_2hrev_2a
			.byte	SCRIPT
dunk_2hrev_2a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HREV2
			.byte	0

			.byte	ANI_INTSTREAM,4,6
#			.byte	ANI_INTSTREAM,10,6

			.byte	ANI_DUNK_BALL
			.word	65,23,JOINT_BWRIST,-1
#			.word	65,46,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,64
#			.byte	ANI_INTSTREAM,36,64

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,6,12
#			.byte	ANI_INTSTREAM,13,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,23,AA_TOGROUND
#			.byte	ANI_INTSTREAM,42,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_2hpmp_1
			.byte	SCRIPT
dunk_2hpmp_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HPMP1
			.byte	0

			.byte	ANI_INTSTREAM,6,4
#			.byte	ANI_INTSTREAM,15,4
			#off ground

			.byte	ANI_DUNK_BALL
			.word	50,25,JOINT_BWRIST,-1
#			.word	50,50,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind-up
			.byte	ANI_INTSTREAM,10,34
#			.byte	ANI_INTSTREAM,20,34

			#slam ball into rim
			.byte	ANI_INTSTREAM,7,15
#			.byte	ANI_INTSTREAM,14,15

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,1	#one-hand

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			#hang on rim
			.byte	ANI_INTSTREAM,2,12
#			.byte	ANI_INTSTREAM,5,12

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
#			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.40
#			.float	.70

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam

			.globl	dunk_2hpmp_1a
			.byte	SCRIPT
dunk_2hpmp_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HPMP1
			.byte	0

			.byte	ANI_INTSTREAM,6,4
#			.byte	ANI_INTSTREAM,15,4
			#off ground

			.byte	ANI_DUNK_BALL
			.word	65,25,JOINT_BWRIST,-1
#			.word	65,50,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind-up
			.byte	ANI_INTSTREAM,6,25
#			.byte	ANI_INTSTREAM,12,25

			#hold
			.byte	ANI_INTSTREAM,4,24
#			.byte	ANI_INTSTREAM,7,24

			#slam ball into rim
			.byte	ANI_INTSTREAM,8,15
#			.byte	ANI_INTSTREAM,16,15

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

#			.byte	ANI_CODE
#			.word	maybe_do_rim_hang_anim,2,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,1	#one-hand

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			#hang on rim
			.byte	ANI_INTSTREAM,2,12
#			.byte	ANI_INTSTREAM,5,12

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
#			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.4
#			.float	.70

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam #2

			.globl	dunk_2hpmp_2
			.byte	SCRIPT
dunk_2hpmp_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_NO_LOOK|PF_DUNKER
			
			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HPMP2
			.byte	0

			.byte	ANI_INTSTREAM,11,4
			#off ground

			.byte	ANI_DUNK_BALL
			.word	45,43,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind-up
			.byte	ANI_INTSTREAM,19,35

			#slam ball into rim
			.byte	ANI_INTSTREAM,12,9

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,6,8

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.80

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed reverse pump slam #2

			.globl	dunk_2hpmp_2a
			.byte	SCRIPT
dunk_2hpmp_2a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS|PF_NO_LOOK|PF_DUNKER
			
			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2HPMP2
			.byte	0

			.byte	ANI_INTSTREAM,11,4
			#off ground

			.byte	ANI_DUNK_BALL
			.word	65,44,JOINT_BWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind-up
			.byte	ANI_INTSTREAM,12,20

			#hold
			.byte	ANI_INTSTREAM,8,27

			#slam ball
			.byte	ANI_INTSTREAM,14,17

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,6,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.80

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing one handed windmill in air #2

			.globl	dunk_windmill_2
			.byte	SCRIPT
dunk_windmill_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_WIND_R,SEQ_K_WIND_L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
#			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_DUNK_BALL2
			.word	45,24,SEQ_K_WIND_R,SEQ_K_WIND_L
#			.word	45,45,SEQ_K_WIND_R,SEQ_K_WIND_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# arm sling is slower
			.byte	ANI_INTSTREAM,11,35
#			.byte	ANI_INTSTREAM,19,35

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,8,9
#			.byte	ANI_INTSTREAM,15,9

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,6
#			.byte	ANI_INTSTREAM,5,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
#			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing one handed windmill in air #2

			.globl	dunk_windmill_2a
			.byte	SCRIPT
dunk_windmill_2a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_WIND_R,SEQ_K_WIND_L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
#			.byte	ANI_INTSTREAM,11,5
			#frame 12

			.byte	ANI_JIFSEQ
			.word	SEQ_K_WIND_R,dunk_wmill_2a_rwrist

			.byte	ANI_DUNK_BALL
			.word	65,24,JOINT_LWRIST,-1
#			.word	65,45,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_wmill_2a_wrist_done

dunk_wmill_2a_rwrist:
			.byte	ANI_DUNK_BALL
#			.word	65,45,JOINT_RWRIST,-1
			.word	65,24,JOINT_RWRIST,-1

dunk_wmill_2a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,6,24
 #			.byte	ANI_INTSTREAM,12,24

			#hold frame
			.byte	ANI_INTSTREAM,4,24
#			.byte	ANI_INTSTREAM,5,24

			#slam ball
			.byte	ANI_INTSTREAM,9,16
#			.byte	ANI_INTSTREAM,17,16

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,2,18
#			.byte	ANI_INTSTREAM,5,18

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
#			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a 360 degree turn in the air

			.globl	dunk_360_1
			.byte	SCRIPT
dunk_360_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_360_1R,SEQ_K_360_1L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_360_1R,dunk_360_1_rwrist

			.byte	ANI_DUNK_BALL
			.word	45,21,JOINT_LWRIST,-1
#			.word	45,45,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_360_1_wrist_done

dunk_360_1_rwrist:
			.byte	ANI_DUNK_BALL
			.word	45,21,JOINT_RWRIST,-1
#			.word	45,42,JOINT_RWRIST,-1

dunk_360_1_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# body rotates
			.byte	ANI_INTSTREAM,13,37
#			.byte	ANI_INTSTREAM,22,37

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,5,7
#			.byte	ANI_INTSTREAM,8,7

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,6
#			.byte	ANI_INTSTREAM,10,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

#			.byte	ANI_PREP_XITION
#			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a 360 degree turn in the air

			.globl	dunk_360_1a
			.byte	SCRIPT
dunk_360_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_360_1R,SEQ_K_360_1L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
#			.byte	ANI_INTSTREAM,11,5
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_360_1R,dunk_360_1a_rwrist

			.byte	ANI_DUNK_BALL
			.word	65,21,JOINT_LWRIST,-1
#			.word	65,45,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_360_1a_wrist_done

dunk_360_1a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	65,21,JOINT_RWRIST,-1
#			.word	65,42,JOINT_RWRIST,-1

dunk_360_1a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# body rotates
			.byte	ANI_INTSTREAM,13,52
#			.byte	ANI_INTSTREAM,22,52

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,5,12
#			.byte	ANI_INTSTREAM,8,12

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,6
#			.byte	ANI_INTSTREAM,10,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

#			.byte	ANI_PREP_XITION
#			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed 360 move in the air

			.globl	dunk_2h360_1
			.byte	SCRIPT
dunk_2h360_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2H360
			.byte	0

			.byte	ANI_INTSTREAM,8,4
			#off ground

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_DUNK_BALL
			.word	45,46,JOINT_LWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind-up
			.byte	ANI_INTSTREAM,24,35

			#slam ball into rim
			.byte	ANI_INTSTREAM,13,9

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,20,1	#one-hand

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,6,12

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.25

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a two-handed 360 move in the air

			.globl	dunk_2h360_1a
			.byte	SCRIPT
dunk_2h360_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.18

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2H360
			.byte	0

			.byte	ANI_INTSTREAM,8,4
			#off ground

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_DUNK_BALL
			.word	65,46,JOINT_LWRIST,-1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind-up
			.byte	ANI_INTSTREAM,24,45

			#slam ball into rim
			.byte	ANI_INTSTREAM,13,19

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,6,7

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.25

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a tomahawk in the air

			.globl	dunk_tomahawk_1
			.byte	SCRIPT
dunk_tomahawk_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_toma_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_TOMA_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_toma_1_hands_done

dunk_toma_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_TOMA_R
			.byte	0

dunk_toma_1_hands_done:			
			.byte	ANI_INTSTREAM,14,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_TOMA_R,dunk_toma_1_rwrist

			.byte	ANI_DUNK_BALL
			.word	40,50,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_toma_1_wrist_done

dunk_toma_1_rwrist:
			.byte	ANI_DUNK_BALL
			.word	40,50,JOINT_RWRIST,-1

dunk_toma_1_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# arm extends behind back
			.byte	ANI_INTSTREAM,20,29

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,14,10

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	1
#			.byte	ANI_INTSTREAM,2,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

#			.byte	ANI_TURNHACK
#			.word	64

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing a tomahawk in the air #2

			.globl	dunk_tomahawk_1a
			.byte	SCRIPT
dunk_tomahawk_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_NO_LOOK
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_toma_1a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_TOMA_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_toma_1a_hands_done
dunk_toma_1a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_TOMA_R
			.byte	0

dunk_toma_1a_hands_done:			
			.byte	ANI_INTSTREAM,14,4
			#frame 15

			.byte	ANI_JIFSEQ
			.word	SEQ_K_TOMA_R,dunk_toma_1a_rwrist

			.byte	ANI_DUNK_BALL
			.word	60,50,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_toma_1a_wrist_done

dunk_toma_1a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	60,50,JOINT_RWRIST,-1

dunk_toma_1a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_NECK,JOINT_PELVIS,80
			.float	5.0,10.0,2.0

			#wind up
			.byte	ANI_INTSTREAM,12,18

			#hold
			.byte	ANI_INTSTREAM,8,26

			#slam ball
			.byte	ANI_INTSTREAM,15,15

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,30,0	#one-hand

			.byte	ANI_INTSTREAM,1,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.80

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball with one hand, #2

			.globl	dunk_1hand_2
			.byte	SCRIPT
dunk_1hand_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_1hd2_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD2_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_1hd2_hands_done

dunk_1hd2_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD2_R
			.byte	0

dunk_1hd2_hands_done:			
			.byte	ANI_INTSTREAM,15,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1HD2_R,dunk_1hd2_rwrist

			.byte	ANI_DUNK_BALL
			.word	40,47,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_1hd2_wrist_done

dunk_1hd2_rwrist:
			.byte	ANI_DUNK_BALL
			.word	40,47,JOINT_RWRIST,-1

dunk_1hd2_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# arm extends behind back
			.byte	ANI_INTSTREAM,20,32

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,11,7

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,10

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,2,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball with one hand, #2

			.globl	dunk_1hand_2a
			.byte	SCRIPT
dunk_1hand_2a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_1hd2a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD2_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_1hd2a_hands_done

dunk_1hd2a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD2_R
			.byte	0

dunk_1hd2a_hands_done:			
			.byte	ANI_INTSTREAM,15,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1HD2_R,dunk_1hd2_rwrist

			.byte	ANI_DUNK_BALL
			.word	60,47,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_1hd2a_wrist_done

dunk_1hd2a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	60,47,JOINT_RWRIST,-1

dunk_1hd2a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# arm extends behind back
			.byte	ANI_INTSTREAM,20,42

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,11,17

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,10

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,2,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball with one hand, #4

			.globl	dunk_1hand_4
			.byte	SCRIPT
dunk_1hand_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_1hd4_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD4_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_1hd4_hands_done

dunk_1hd4_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD4_R
			.byte	0

dunk_1hd4_hands_done:			
			.byte	ANI_INTSTREAM,7,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1HD4_R,dunk_1hd4_rwrist

			.byte	ANI_DUNK_BALL
			.word	35,20,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_1hd4_wrist_done

dunk_1hd4_rwrist:
			.byte	ANI_DUNK_BALL
			.word	35,20,JOINT_RWRIST,-1

dunk_1hd4_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# arm extends behind back
			.byte	ANI_INTSTREAM,9,28

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,3,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			#.byte	ANI_INTSTREAM,3,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball with one hand, #4

			.globl	dunk_1hand_4a
			.byte	SCRIPT
dunk_1hand_4a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_1hd4a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD4_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_1hd4a_hands_done

dunk_1hd4a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1HD4_R
			.byte	0

dunk_1hd4a_hands_done:			
			.byte	ANI_INTSTREAM,7,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1HD4_R,dunk_1hd4_rwrist

			.byte	ANI_DUNK_BALL
			.word	55,20,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_1hd4a_wrist_done

dunk_1hd4a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	55,20,JOINT_RWRIST,-1

dunk_1hd4a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# arm extends behind back
			.byte	ANI_INTSTREAM,9,35

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,3,19

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			#.byte	ANI_INTSTREAM,3,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a 180 degree turn in air

			.globl	dunk_180_1
			.byte	SCRIPT
dunk_180_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.09

#			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_180_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_180W_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_180_1_hands_done

dunk_180_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_180W_R
			.byte	0

dunk_180_1_hands_done:			
			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_JIFSEQ
			.word	SEQ_K_180W_R,dunk_180_1_rwrist

			.byte	ANI_DUNK_BALL
			.word	40,49,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_180_1_wrist_done

dunk_180_1_rwrist:
			.byte	ANI_DUNK_BALL
			.word	40,49,JOINT_RWRIST,-1

dunk_180_1_wrist_done:

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,33,39

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing a 180 degree turn in air

			.globl	dunk_180_1a
			.byte	SCRIPT
dunk_180_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.09

#			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_180_1a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_180W_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_180_1_hands_done

dunk_180_1a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_180W_R
			.byte	0

dunk_180_1a_hands_done:			
			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_JIFSEQ
			.word	SEQ_K_180W_R,dunk_180_1a_rwrist

			.byte	ANI_DUNK_BALL
			.word	60,49,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_180_1a_wrist_done

dunk_180_1a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	60,49,JOINT_RWRIST,-1

dunk_180_1a_wrist_done:

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,33,59

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,3,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing one handed windmill in air

			.globl	dunk_windmill_1
			.byte	SCRIPT
dunk_windmill_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.12

#			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_wmill_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1MIL_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_wmill_1_hands_done

dunk_wmill_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1MIL_R
			.byte	0

dunk_wmill_1_hands_done:			
			.byte	ANI_INTSTREAM,11,7

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1MIL_R,dunk_wmill_1_rwrist

			.byte	ANI_DUNK_BALL
			.word	40,46,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_wmill_1_wrist_done

dunk_wmill_1_rwrist:
			.byte	ANI_DUNK_BALL
			.word	40,46,JOINT_RWRIST,-1

dunk_wmill_1_wrist_done:

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,35,39

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,17

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; player dunks ball doing one handed windmill in air

			.globl	dunk_windmill_1a
			.byte	SCRIPT
dunk_windmill_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_wmill_1a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1MIL_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_wmill_1a_hands_done

dunk_wmill_1a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1MIL_R
			.byte	0

dunk_wmill_1a_hands_done:			
			.byte	ANI_INTSTREAM,11,6
			#frame 12

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1MIL_R,dunk_wmill_1a_rwrist

			.byte	ANI_DUNK_BALL
			.word	55,46,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_wmill_1a_wrist_done

dunk_wmill_1a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	55,46,JOINT_RWRIST,-1

dunk_wmill_1a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			#wind up
			.byte	ANI_INTSTREAM,12,23

			#hold frame
			.byte	ANI_INTSTREAM,7,17

			#slam ball
			.byte	ANI_INTSTREAM,15,14

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,30,0	#one-hand

			#hang on rim
			.byte	ANI_INTSTREAM,4,17

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing one handed windmill in air

			.globl	dunk_270h_1
			.byte	SCRIPT
dunk_270h_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_270h_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_270H_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_270h_1_hands_done

dunk_270h_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_270H_R
			.byte	0

dunk_270h_1_hands_done:
			.byte	ANI_RUNTOFRAME,5,1			
#			.byte	ANI_INTSTREAM,5,5

			.byte	ANI_DUNK_BALL
			.word	35,21,JOINT_BWRIST,-1

dunk_270h_1_wrist_done:

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,17,34

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,28,30

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player dunks ball doing one handed windmill in air

			.globl	dunk_270h_1a
			.byte	SCRIPT
dunk_270h_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_270h_1a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_270H_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_270h_1a_hands_done

dunk_270h_1a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_270H_R
			.byte	0

dunk_270h_1a_hands_done:
			.byte	ANI_RUNTOFRAME,5,1			

			.byte	ANI_DUNK_BALL
			.word	55,21,JOINT_BWRIST,-1

dunk_270h_1a_wrist_done:

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,17,54

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,28,30

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
			
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	dunk_1_hand_1
			.byte	SCRIPT
dunk_1_hand_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_1HND_R,SEQ_K_1HND_L
			.byte	0

# allow the feet to plant
			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL2
			.word	45,43,SEQ_K_1HND_R,SEQ_K_1HND_L

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,dunk_1_hand_1_other
			
			#.byte	ANI_INTSTREAM,34,45
			.byte	ANI_INTSTREAM,20,40
			.byte	ANI_INTSTREAM,14,5

			.byte	ANI_GOTO
			.word dunk_1_hand_1_done_on_variation

dunk_1_hand_1_other:
			.byte	ANI_INTSTREAM,13,30
			.byte	ANI_INTSTREAM,19,15

dunk_1_hand_1_done_on_variation:
			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,0,0

			.byte	ANI_INTSTREAM,1,7

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,0,0

			.byte	ANI_INTSTREAM,15,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			.byte	ANI_RUNTOFRAME,71,1

			.byte	ANI_CODE
			.word maybe_taunt,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; on impact sticks out his but

			.globl dunk_1hand_but_out
			.byte	SCRIPT
dunk_1hand_but_out:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETFRICTION
			.float	0.55

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_1hand_but_out_r

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_1BUT_L
			.byte	0

			.byte	ANI_GOTO
			.word dunk_1hand_but_out_hands_done

dunk_1hand_but_out_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_1BUT_R
			.byte	0

dunk_1hand_but_out_hands_done:			
# allow the feet to plant
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_JIFSEQ
			.word	SEQ_K_1BUT_R,dunk_1hand_but_out_rwrist

			.byte	ANI_DUNK_BALL
			.word	40,35,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_1hand_but_out_wrist_done

dunk_1hand_but_out_rwrist:
			.byte	ANI_DUNK_BALL
			.word	40,35,JOINT_RWRIST,-1

dunk_1hand_but_out_wrist_done:
			.byte	ANI_INTSTREAM,13,30
			.byte	ANI_INTSTREAM,15,10

			#.byte	ANI_CODE
			#.word dampen_yvel,0,0,0

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,0,0

			.byte	ANI_INTSTREAM,7,10

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,0,0

			.byte	ANI_INTSTREAM,17,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,12,12

			.byte	ANI_CODE
			.word maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.5

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

			.globl dunk_pump_1
			.byte	SCRIPT
dunk_pump_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

#			.byte	ANI_SETVELS
#			.float	0.0,0.0,0.65
			.byte	ANI_SETFRICTION
			.float	0.20

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_pump_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_PMP2_L
			.byte	8

			.byte	ANI_GOTO
			.word	dunk_pump_1_hands_done

dunk_pump_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_PMP2_R
			.byte	8

dunk_pump_1_hands_done:			
# allow the feet to plant
			.byte	ANI_INTSTREAM,6,4

			.byte	ANI_JIFSEQ
			.word	SEQ_K_PMP2_R,dunk_pump_1_rwrist

			.byte	ANI_DUNK_BALL
			.word	35,43,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_pump_1_wrist_done

dunk_pump_1_rwrist:
			.byte	ANI_DUNK_BALL
			.word	35,43,JOINT_RWRIST,-1

dunk_pump_1_wrist_done:
			#.byte	ANI_INTSTREAM,28,35
			.byte	ANI_INTSTREAM,12,20
			.byte	ANI_INTSTREAM,17,15

			.byte	ANI_CODE
			.word dampen_yvel,2,0,0

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,13,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word maybe_taunt,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# a quick dunk variation on dunk_pump_1
			.globl dunk_quick_5
			.byte	SCRIPT
dunk_quick_5:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_SETVELS
			.float	0.0,0.0,0.65

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_quick_5_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_PMP2_L
			.byte	0

			.byte	ANI_GOTO
			.word	dunk_quick_5_hands_done

dunk_quick_5_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_PMP2_R
			.byte	0

dunk_quick_5_hands_done:			
# allow the feet to plant
			.byte	ANI_INTSTREAM,16,10

			.byte	ANI_JIFSEQ
			.word	SEQ_K_PMP2_R,dunk_quick_5_rwrist

			.byte	ANI_DUNK_BALL
			.word	20,43,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word 	dunk_quick_5_wrist_done

dunk_quick_5_rwrist:
			.byte	ANI_DUNK_BALL
			.word	20,43,JOINT_RWRIST,-1

dunk_quick_5_wrist_done:
			.byte	ANI_INTSTREAM,27,20

			.byte	ANI_CODE
			.word 	dampen_yvel,2,0,0

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,13,AA_TOGROUND
# on ground now

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word maybe_taunt,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;; player does a 1 hand pump scissors dunk

			.globl	dunk_scissor_1
			.byte	SCRIPT
dunk_scissor_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_scissor_1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_L
			.byte	8

			.byte	ANI_GOTO
			.word	dunk_scissor_1_hands_done

dunk_scissor_1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_R
			.byte	8

dunk_scissor_1_hands_done:			
			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_JIFSEQ
			.word	SEQ_K_SCR1_R,dunk_scissor_1_rwrist

			.byte	ANI_DUNK_BALL
			.word	35,43,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_scissor_1_wrist_done

dunk_scissor_1_rwrist:
			.byte	ANI_DUNK_BALL
			.word	35,43,JOINT_RWRIST,-1

dunk_scissor_1_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,26,34

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,2,6

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;; player does a 1 hand pump scissors dunk (varyation)

			.globl	dunk_scissor_1a
			.byte	SCRIPT
dunk_scissor_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_ZEROVELS

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_scissor_1a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_L
			.byte	19

			.byte	ANI_DUNK_BALL
			.word	55,43,JOINT_LWRIST,-1

			.byte	ANI_GOTO
			.word	dunk_scissor_1a_cont

dunk_scissor_1a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_R
			.byte	19

			.byte	ANI_DUNK_BALL
			.word	55,43,JOINT_RWRIST,-1

dunk_scissor_1a_cont:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,10,23

			#hold frame
			.byte	ANI_INTSTREAM,2,19

			#slam ball
			.byte	ANI_INTSTREAM,12,12

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_INTSTREAM,2,9

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;; player does a 2 hand pump scissors dunk

			.globl	dunk_scissor_2
			.byte	SCRIPT
dunk_scissor_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_1HD3_R
			.byte	0
			
			.byte	ANI_INTSTREAM,10,4

			.byte	ANI_DUNK_BALL2
			.word	40,44,SEQ_K_1HD3_R,SEQ_K_1HD3_R

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,22,26

			#slam ball
			.byte	ANI_INTSTREAM,13,13

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

#			.byte	ANI_INTSTREAM,2,9

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	2,29

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


##########################
# COMBINED DUNKS         #
##########################

#;;;;;;;;;;;;;;;;; combined dunk #1

			.globl	dunk_combined_1
			.byte	SCRIPT
dunk_combined_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK|PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_360_1R,SEQ_K_360_1L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
#			.byte	ANI_INTSTREAM,11,5
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	65,50,SEQ_K_TOMA_R,SEQ_K_TOMA_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# body rotates
			.byte	ANI_INTSTREAM,6,25
#			.byte	ANI_INTSTREAM,12,25

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,25
			.word	SEQ_K_TOMA_R,SEQ_K_TOMA_L
			.byte	33

			.byte	ANI_INTSTREAM,1,4

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,14,10

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,50,0	#one-hand

			#hang on rim
			.byte	1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #1 (lower and faster)

			.globl	dunk_combined_1a
			.byte	SCRIPT
dunk_combined_1a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK|PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_360_1R,SEQ_K_360_1L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
#			.byte	ANI_INTSTREAM,11,5
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_360_1R,dc1a_rwrist

			.byte	ANI_DUNK_BALL
			.word	45,50,JOINT_LWRIST,SEQ_K_TOMA_L

			.byte	ANI_GOTO
			.word	dc1a_wrist_done

dc1a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	45,50,JOINT_RWRIST,SEQ_K_TOMA_R

dc1a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			# body rotates
			.byte	ANI_INTSTREAM,6,15
#			.byte	ANI_INTSTREAM,12,15

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dc1a_chng
			
			.byte	ANI_INTTOSEQ,19
			.word	SEQ_K_TOMA_L
			.byte	33

			.byte	ANI_GOTO
			.word	dc1a_chng_done

dc1a_chng:
			.byte	ANI_INTTOSEQ,19
			.word	SEQ_K_TOMA_R
			.byte	33

dc1a_chng_done:
#			.byte	ANI_INTSTREAM,1,4
			
			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,14,10
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;;;;;;;; combined dunk #2

			.globl	dunk_combined_2
			.byte	SCRIPT
dunk_combined_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.20

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dc2_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_L
			.byte	8

			.byte	ANI_GOTO
			.word	dc2_hands_done

dc2_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_R
			.byte	8

dc2_hands_done:			
			.byte	ANI_INTSTREAM,9,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_SCR1_R,dc2_rwrist

			.byte	ANI_DUNK_BALL
			.word	60,38,JOINT_BWRIST,SEQ_K_QUIK_6

			.byte	ANI_GOTO
			.word	dc2_wrist_done

dc2_rwrist:
			.byte	ANI_DUNK_BALL
			.word	60,38,JOINT_BWRIST,SEQ_K_QUIK_6

dc2_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,12,23

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,22
			.word	SEQ_K_QUIK_6
			.byte	28

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,10,14
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #2a

			.globl	dunk_combined_2a
			.byte	SCRIPT
dunk_combined_2a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dc2a_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_L
			.byte	8

			.byte	ANI_GOTO
			.word	dc2a_hands_done

dc2a_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_SCR1_R
			.byte	8

dc2a_hands_done:			
			.byte	ANI_INTSTREAM,9,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_SCR1_R,dc2a_rwrist

			.byte	ANI_DUNK_BALL
			.word	45,38,JOINT_BWRIST,SEQ_K_QUIK_6

			.byte	ANI_GOTO
			.word	dc2a_wrist_done

dc2a_rwrist:
			.byte	ANI_DUNK_BALL
			.word	45,38,JOINT_BWRIST,SEQ_K_QUIK_6

dc2a_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,12,22

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,12
			.word	SEQ_K_QUIK_6
			.byte	28

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,10,11
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #3

			.globl	dunk_combined_3
			.byte	SCRIPT
dunk_combined_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.20

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_PMP2_R,SEQ_K_PMP2_L
			.byte	8

			.byte	ANI_INTSTREAM,6,4
			#off ground

			.byte	ANI_JIFSEQ
			.word	SEQ_K_PMP2_R,dc3_rwrist

			.byte	ANI_DUNK_BALL
			.word	65,46,JOINT_LWRIST,SEQ_K_1MIL_L

			.byte	ANI_GOTO
			.word	dc3_wrist_done

dc3_rwrist:
			.byte	ANI_DUNK_BALL
			.word	65,46,JOINT_RWRIST,SEQ_K_1MIL_R

dc3_wrist_done:
			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,12
			.byte	ANI_INTSTREAM,4,7

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_JIFSEQ
			.word	SEQ_K_PMP2_L,dc3_chng

			.byte	ANI_INTTOSEQ,27
			.word	SEQ_K_1MIL_R
			.byte	28

			.byte	ANI_GOTO
			.word	dc3_chng_done
dc3_chng:
			.byte	ANI_INTTOSEQ,27
			.word	SEQ_K_1MIL_L
			.byte	28
dc3_chng_done:

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,18,18
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,14

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #4

			.globl	dunk_combined_4
			.byte	SCRIPT
dunk_combined_4:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.20

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2H360
			.byte	0

			.byte	ANI_INTSTREAM,8,4
			#off ground

			.byte	ANI_DUNK_BALL
			.word	65,38,JOINT_BWRIST,SEQ_K_QUIK_6

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,21,27

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,23
			.word	SEQ_K_QUIK_6
			.byte	28

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			# slam ball into rim ,faster
			.byte	ANI_INTSTREAM,10,14
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,1

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #4a

			.globl	dunk_combined_4a
			.byte	SCRIPT
dunk_combined_4a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFRICTION
			.float	0.20

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_2H360
			.byte	0

			.byte	ANI_INTSTREAM,8,4
			#off ground

			.byte	ANI_DUNK_BALL
			.word	45,38,JOINT_BWRIST,SEQ_K_QUIK_6

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,21,15

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,17
			.word	SEQ_K_QUIK_6
			.byte	28

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,10,12
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #5

			.globl	dunk_combined_5
			.byte	SCRIPT
dunk_combined_5:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY6A_R,SEQ_LAY6A_L
			.byte	10

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,9,5
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	70,58,SEQ_K_3603_R,SEQ_K_3603_L

			# more of layup
			.byte	ANI_INTSTREAM,29,17

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,21
			.word	SEQ_K_3603_R,SEQ_K_3603_L
			.byte	25

			.byte	ANI_INTSTREAM,18,15

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,14,12

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,101,0	#one-hand

			#hang on rim
			.byte	1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #6

			.globl	dunk_combined_6
			.byte	SCRIPT
dunk_combined_6:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK|PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY16_R,SEQ_LAY16_L
			.byte	3

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL2
			.word	70,50,SEQ_K_TOMA_R,SEQ_K_TOMA_L

			.byte	ANI_INTSTREAM,6,20
#			.byte	ANI_INTSTREAM,11,22

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,21
			.word	SEQ_K_TOMA_R,SEQ_K_TOMA_L
			.byte	34

			.byte	ANI_INTSTREAM,5,15

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,11,13

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			#hang on rim
			.byte	1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #7

			.globl	dunk_combined_7
			.byte	SCRIPT
dunk_combined_7:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY4_R,SEQ_LAY4_L
			.byte	7

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_DUNK_BALL2
			.word	55,43,SEQ_K_PMP2_R,SEQ_K_PMP2_L

			.byte	ANI_INTSTREAM,14,17

			.byte	ANI_INTTOSEQH,17
			.word	SEQ_K_PMP2_R,SEQ_K_PMP2_L
			.byte	28

			.byte	ANI_INTSTREAM,14,20

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,101,0	#one-hand

			#hang on rim
			.byte	3

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #8

			.globl	dunk_combined_8
			.byte	SCRIPT
dunk_combined_8:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY9_R,SEQ_LAY9_L
			.byte	0

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_DUNK_BALL2
			.word	60,46,SEQ_K_1MIL_R,SEQ_K_1MIL_L

			.byte	ANI_INTSTREAM,9,27
#			.byte	ANI_INTSTREAM,15,36

			.byte	ANI_INTTOSEQH,13
			.word	SEQ_K_1MIL_R,SEQ_K_1MIL_L
			.byte	28

			.byte	ANI_INTSTREAM,18,19

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			#hang on rim
			.byte	ANI_INTSTREAM,1,5

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #9

			.globl	dunk_combined_9
			.byte	SCRIPT
dunk_combined_9:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK|PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY17_R,SEQ_LAY17_L
			.byte	4

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_DUNK_BALL2
			.word	65,50,SEQ_K_TOMA_R,SEQ_K_TOMA_L

			.byte	ANI_INTSTREAM,10,25

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,18
			.word	SEQ_K_TOMA_R,SEQ_K_TOMA_L
			.byte	22

			.byte	ANI_INTSTREAM,28,21

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,60,0	#one-hand

			#hang on rim
			.byte	ANI_INTSTREAM,1,8

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #10

			.globl	dunk_combined_10
			.byte	SCRIPT
dunk_combined_10:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_WIND_R,SEQ_K_WIND_L
			.byte	0

			.byte	ANI_INTSTREAM,4,5
#			.byte	ANI_INTSTREAM,10,5
			#off ground

			.byte	ANI_DUNK_BALL
			.word	70,46,JOINT_LWRIST,SEQ_K_2H360

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,8,14
#			.byte	ANI_INTSTREAM,17,14

			.byte	ANI_INTSTREAM,4,13
#			.byte	ANI_INTSTREAM,5,13

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,12
			.word	SEQ_K_2H360
			.byte	14

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTSTREAM,35,31

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,0	#one-hand

			#.byte	ANI_CODE
			#.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #11

			.globl	dunk_combined_11
			.byte	SCRIPT
dunk_combined_11:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_K_PMP2_R,SEQ_K_PMP2_L
			.byte	0

			.byte	ANI_INTSTREAM,6,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	70,43,SEQ_K_SCR1_R,SEQ_K_SCR1_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,31,26

			#.byte	ANI_SETMODE
			#.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,12
			.word	SEQ_K_SCR1_R,SEQ_K_SCR1_L
			.byte	23

			.byte	ANI_INTSTREAM,20,32

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,0	#one-hand

			#.byte	ANI_CODE
			#.word	SetHangOnRim,NO,NO,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #12

			.globl	dunk_combined_12
			.byte	SCRIPT
dunk_combined_12:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_NDUNK6
			.byte	5

			.byte	ANI_INTSTREAM,3,3
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	65,47,SEQ_K_180W_R,SEQ_K_180W_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,15,10
			.byte	ANI_INTSTREAM,5,8
			.byte	ANI_INTSTREAM,3,18

			.byte	ANI_INTTOSEQH,17
			.word	SEQ_K_180W_R,SEQ_K_180W_L
			.byte	30

			.byte	ANI_INTSTREAM,17,11

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,2,10
	
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

#			.byte	ANI_SETVELS
#			.float	0.0,0.0,0.15

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #12a

			.globl	dunk_combined_12a
			.byte	SCRIPT
dunk_combined_12a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_NDUNK6
			.byte	5

			.byte	ANI_INTSTREAM,3,3
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,25,SEQ_K_2HREV1,SEQ_K_2HREV1
#			.word	60,48,SEQ_K_2HREV1,SEQ_K_2HREV1

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,15,11
			.byte	ANI_INTSTREAM,5,8
			.byte	ANI_INTSTREAM,3,19

			.byte	ANI_INTTOSEQ,14
			.word	SEQ_K_2HREV1
			.byte	20
#			.byte	41

			.byte	ANI_INTSTREAM,5,7
#			.byte	ANI_INTSTREAM,7,7

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

#			.byte	ANI_CODE
#			.word	maybe_do_rim_hang_anim,-1,10,1	#two-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_INTSTREAM,13,18
#			.byte	ANI_INTSTREAM,26,18
	
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,11,AA_TOGROUND
#			.byte	ANI_INTSTREAM,23,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0
#			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #13

			.globl	dunk_combined_13
			.byte	SCRIPT
dunk_combined_13:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_NDUNK5
			.byte	10

			.byte	ANI_INTSTREAM,5,3
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,44,SEQ_K_1HD3_R,SEQ_K_1HD3_R

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,24,24

			.byte	ANI_INTTOSEQH,15
			.word	SEQ_K_1HD3_R,SEQ_K_1HD3_R
			.byte	21

			.byte	ANI_INTSTREAM,21,20

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,1,5

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,3,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #14

			.globl	dunk_combined_14
			.byte	SCRIPT
dunk_combined_14:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_K_270H_R
			.byte	0

			.byte	ANI_INTSTREAM,3,3
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	63,43,SEQ_K_SCR1_R,SEQ_K_SCR1_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,8,22

			.byte	ANI_INTTOSEQH,20
			.word	SEQ_K_SCR1_R,SEQ_K_SCR1_L
			.byte	25

			.byte	ANI_INTSTREAM,17,20

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,1,5

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #15

			.globl	dunk_combined_15
			.byte	SCRIPT
dunk_combined_15:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_DUNK1
			.byte	9

			#off ground

			.byte	ANI_DUNK_BALL2
			.word	55,21,SEQ_K_270H_R,SEQ_K_270H_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,20,24

			.byte	ANI_INTTOSEQH,15
			.word	SEQ_K_SCR1_R,SEQ_K_SCR1_L
			.byte	20

			.byte	ANI_INTSTREAM,22,20

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,1,10

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #16

			.globl	dunk_combined_16
			.byte	SCRIPT
dunk_combined_16:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT1
			.byte	0

			.byte	ANI_INTSTREAM,9,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	67,57,SEQ_K_3603_L,SEQ_K_3603_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,12,20

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
				
			.byte	1

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTSTREAM,8,10

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	1

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
	
			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTSTREAM,11,10

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,15
			.word	SEQ_K_3603_L,SEQ_K_3603_L
			.byte	51

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,6,9

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,0	#one-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,1,10

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #17

			.globl	dunk_combined_17
			.byte	SCRIPT
dunk_combined_17:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT1
			.byte	0

			.byte	ANI_INTSTREAM,9,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,38,SEQ_K_QUIK_6,SEQ_K_QUIK_6

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,12,10

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS
				
			.byte	1

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTSTREAM,8,11

			.byte	ANI_SETFLAG
			.word	PF_BOTH_HANDS

			.byte	1

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS
	
			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTSTREAM,11,10

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQH,17
			.word	SEQ_K_QUIK_6,SEQ_K_QUIK_6
			.byte	26

			# slam ball into rim (faster)
			.byte	ANI_INTSTREAM,14,9

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,10,1	#one-hand

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_INTSTREAM,4,10

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #18

			.globl	dunk_combined_18
			.byte	SCRIPT
dunk_combined_18:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT11
			.byte	9

			.byte	ANI_INTSTREAM,7,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,43,SEQ_K_PMP2_R,SEQ_K_PMP2_R

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,28,21

			.byte	ANI_INTTOSEQ,19
			.word	SEQ_K_PMP2_R
			.byte	20

			# slam ball into rim
			.byte	ANI_INTSTREAM,22,19
			
			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,4,7

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #19

			.globl	dunk_combined_19
			.byte	SCRIPT
dunk_combined_19:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT12
			.byte	9

			.byte	ANI_INTSTREAM,7,3
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,42,SEQ_K_WIN2,SEQ_K_WIN2

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,23,24

			.byte	ANI_INTTOSEQ,17
			.word	SEQ_K_WIN2
			.byte	25

			# slam ball into rim
			.byte	ANI_INTSTREAM,20,18
			
			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,101,0	#one-hand

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #20


			.globl	dunk_combined_20
			.byte	SCRIPT
dunk_combined_20:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT13
			.byte	14

			.byte	ANI_INTSTREAM,5,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,43,SEQ_K_SCR1_R,SEQ_K_SCR1_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,24,26

			.byte	ANI_INTTOSEQH,14
			.word	SEQ_K_SCR1_R,SEQ_K_SCR1_L
			.byte	21

			# slam ball into rim
			.byte	ANI_INTSTREAM,22,19
			
			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_INTSTREAM,2,7

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #20


			.globl	dunk_combined_20a
			.byte	SCRIPT
dunk_combined_20a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK|PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT13
			.byte	14

			.byte	ANI_INTSTREAM,5,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	75,43,SEQ_K_TOMA_R,SEQ_K_TOMA_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,24,23

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_K_STRT13
			.byte	28

			.byte	ANI_INTSTREAM,15,16

			.byte	ANI_INTTOSEQH,11
			.word	SEQ_K_TOMA_R,SEQ_K_TOMA_L
			.byte	39

			# slam ball into rim
			.byte	ANI_INTSTREAM,10,14
			
			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,101,0	#one-hand

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #21


			.globl	dunk_combined_21
			.byte	SCRIPT
dunk_combined_21:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT2
			.byte	0

			.byte	ANI_INTSTREAM,10,5
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,38,SEQ_K_QUIK_6,SEQ_K_QUIK_6

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,26

			.byte	ANI_INTTOSEQ,19
			.word	SEQ_K_QUIK_6
			.byte	26

			# slam ball into rim
			.byte	ANI_INTSTREAM,12,14
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,YES,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #21a


			.globl	dunk_combined_21a
			.byte	SCRIPT
dunk_combined_21a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT2
			.byte	0

			.byte	ANI_INTSTREAM,10,5
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	70,50,SEQ_K_TOMA_R,SEQ_K_TOMA_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,26

			.byte	ANI_INTTOSEQH,20
			.word	SEQ_K_TOMA_R,SEQ_K_TOMA_L
			.byte	34

			.byte	ANI_INTSTREAM,5,13

			# slam ball into rim
			.byte	ANI_INTSTREAM,11,10
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,0	#one-hand

			#hang on rim
			.byte	1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #21b


			.globl	dunk_combined_21b
			.byte	SCRIPT
dunk_combined_21b:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT2
			.byte	0

			.byte	ANI_INTSTREAM,10,5
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	70,50,SEQ_K_TOMA_R,SEQ_K_TOMA_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,4,4
			.byte	ANI_INTSTREAM,7,22

			.byte	ANI_INTTOSEQH,20
			.word	SEQ_K_TOMA_R,SEQ_K_TOMA_L
			.byte	34

			.byte	ANI_INTSTREAM,5,13

			# slam ball into rim
			.byte	ANI_INTSTREAM,11,10
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	maybe_do_rim_hang_anim,-1,90,0	#one-hand

			#hang on rim
			.byte	1

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #22


			.globl	dunk_combined_22
			.byte	SCRIPT
dunk_combined_22:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT9
			.byte	6

			.byte	ANI_INTSTREAM,6,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,43,SEQ_K_SCR1_R,SEQ_K_SCR1_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,18,26

			.byte	ANI_INTTOSEQH,16
			.word	SEQ_K_SCR1_R,SEQ_K_SCR1_L
			.byte	21

			# slam ball into rim
			.byte	ANI_INTSTREAM,21,17
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;;;;;;;;; combined dunk #22a


			.globl	dunk_combined_22a
			.byte	SCRIPT
dunk_combined_22a:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_DUNKER

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_SETFRICTION
			.float	0.12

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_K_STRT9
			.byte	6

			.byte	ANI_INTSTREAM,6,4
			#off ground

			.byte	ANI_DUNK_BALL2
			.word	60,43,SEQ_K_SCR1_R,SEQ_K_SCR1_L

			.byte	ANI_ATTACK_ON,ATTMODE_DUNK,JOINT_PELVIS,JOINT_RANKLE,80
			.float	1.5,1.5,2.0

			.byte	ANI_INTSTREAM,21,26

			.byte	ANI_INTTOSEQH,16
			.word	SEQ_K_SCR1_R,SEQ_K_SCR1_L
			.byte	21

			# slam ball into rim
			.byte	ANI_INTSTREAM,21,17
			
			.byte	ANI_CODE
			.word	SetHangOnRim,YES,NO,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETYVEL
			.float	0.0

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			#hang on rim
			.byte	ANI_INTSTREAM,4,4

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,0

			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_STREAMTOEND
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END


##########################
# LAYUPS                 #
##########################
#Greg Cutler

			.globl	layup27_anim
			.byte	SCRIPT
layup27_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_LAYUP|PF_NO_LOOK

			.byte	ANI_SETFRICTION
			.float	.04

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup27_r

#			.byte	ANI_CODE
#			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY27_R
			.byte	0

			.byte	ANI_INTSTREAM,2,4

			.byte	ANI_LAYUP
			.word	30,15,JOINT_LWRIST
			.float	0.5

			.byte	ANI_INTSTREAM,8,14

			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_GOTO
			.word	layup27_hands_done
layup27_r:
#			.byte	ANI_CODE
#			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY27_L
			.byte	0

			.byte	ANI_INTSTREAM,2,4

			.byte	ANI_LAYUP
			.word	30,15,JOINT_RWRIST
			.float	0.5

			.byte	ANI_INTSTREAM,8,14

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

layup27_hands_done:
			.byte	ANI_INTSTREAM,5,15

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,2,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#Greg Cutler

			.globl	layup26_anim
			.byte	SCRIPT
layup26_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_LAYUP|PF_NO_LOOK

			.byte	ANI_SETFRICTION
			.float	.04

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup26_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY26_L
			.byte	0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_LAYUP
			.word	40,22,JOINT_LWRIST
			.float	0.5

			.byte	ANI_GOTO
			.word	layup26_hands_done
layup26_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY26_R
			.byte	0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_LAYUP
			.word	40,22,JOINT_RWRIST
			.float	0.5

layup26_hands_done:
			.byte	ANI_INTSTREAM,15,35

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP|PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#Greg Cutler

			.globl	layup25_anim
			.byte	SCRIPT
layup25_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_LAYUP|PF_NO_LOOK

			.byte	ANI_SETFRICTION
			.float	.04

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup25_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY25_L
			.byte	0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_LAYUP
			.word	35,22,JOINT_LWRIST
			.float	0.5

			.byte	ANI_GOTO
			.word	layup25_hands_done
layup25_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY25_R
			.byte	0

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_LAYUP
			.word	35,22,JOINT_RWRIST
			.float	0.5

layup25_hands_done:

			.byte	ANI_INTSTREAM,17,25

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,5,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP|PF_NO_LOOK
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS
			
			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; another layup

			.globl	layup23_anim
			.byte	SCRIPT
layup23_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

#			.byte	ANI_SETVELS
#			.float	0.0,0.0,1.1

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup23_r

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_LAY23_L
			.byte	8

			.byte	ANI_LAYUP
			.word	35,39,JOINT_LWRIST
			.float	0.0

			.byte	ANI_GOTO
			.word	layup23_hands_done
layup23_r:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_LAY23_R
			.byte	8

			.byte	ANI_LAYUP
			.word	35,39,JOINT_RWRIST
			.float	0.0

layup23_hands_done:

			.byte	ANI_INTSTREAM,31,25

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS
			
			.byte	ANI_STREAMTOEND
			.float	.80

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;; another layup

			.globl	layup24_anim
			.byte	SCRIPT
layup24_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup24_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY24_L
			.byte	8

			.byte	ANI_LAYUP
			.word	40,37,JOINT_LWRIST
			.float	0.0

			.byte	ANI_GOTO
			.word	layup24_hands_done
layup24_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY24_R
			.byte	8

			.byte	ANI_LAYUP
			.word	40,37,JOINT_RWRIST
			.float	0.0

layup24_hands_done:

			.byte	ANI_INTSTREAM,15,13

			#hold for a bit
			.byte	ANI_INTSTREAM,8,10

			.byte	ANI_INTSTREAM,5,4

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,24,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS
			
			.byte	ANI_STREAMTOEND
			.float	.80

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; QUICK layup outta dunk #1

			.globl	quick_dunk_layup_1
			.byte	SCRIPT
quick_dunk_layup_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_SETYVEL
			.float	.50

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_LAY1_R,SEQ_LAY1_L
			.byte	24

			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; QUICK layup outta dunk #1

			.globl	auto_layup_1
			.byte	SCRIPT
auto_layup_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.15

			.byte	ANI_SETYVEL
			.float	.850

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_INTTOSEQH,4
			.word	SEQ_LAY1_R,SEQ_LAY1_L
			.byte	24

			.byte	ANI_INTSTREAM,7,7

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; layup outta dunk #1

			.globl	dunk_layup_1
			.byte	SCRIPT
dunk_layup_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_CODE
			.word	dampen_yvel,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_layup_1_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_LAY25_L
			.byte	16

			.byte	ANI_GOTO
			.word	dunk_layup_1_hands_done

dunk_layup_1_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_LAY25_R
			.byte	16

dunk_layup_1_hands_done:
			.byte	ANI_INTSTREAM,4,7

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; layup outta dunk #2

			.globl	dunk_layup_2
			.byte	SCRIPT
dunk_layup_2:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_CODE
			.word	dampen_yvel,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_layup_2_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_LAY16_L
			.byte	14

			.byte	ANI_GOTO
			.word	dunk_layup_2_hands_done

dunk_layup_2_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_LAY16_R
			.byte	14

dunk_layup_2_hands_done:
			.byte	ANI_INTSTREAM,2,5

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND
# on ground now

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;;; layup outta dunk #3

			.globl	dunk_layup_3
			.byte	SCRIPT
dunk_layup_3:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_CODE
			.word	dampen_yvel,1,0,0

			.byte	ANI_CLRFLAG
			.word	PF_DUNKER|PF_ALLYDUNKER|PF_BOTH_HANDS

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,dunk_layup_3_r

			.byte	ANI_CODE
			.word	set_ball_hand,LEFT_HAND,0,0

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_LAY23_L
			.byte	29

			.byte	ANI_GOTO
			.word	dunk_layup_3_hands_done

dunk_layup_3_r:
			.byte	ANI_CODE
			.word	set_ball_hand,RIGHT_HAND,0,0

			.byte	ANI_INTTOSEQ,7
			.word	SEQ_LAY23_R
			.byte	29

dunk_layup_3_hands_done:
			.byte	ANI_INTSTREAM,7,7

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,18,AA_TOGROUND
# on ground now

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	layup9_anim
			.byte	SCRIPT
layup9_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030
			
			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY9_R,SEQ_LAY9_L
			.byte	0

# allow the feet to plant
			.byte	ANI_INTSTREAM,5,5

			#.byte	ANI_SETVELS
			#.float	0.0,0.0,0.75

			.byte	ANI_LAYUP
			.word	30,21,JOINT_RWRIST
			#.float	0.0
			.float	0.475

# time has to match the time fed into ANI_LAYUP
			.byte	ANI_INTSTREAM,12,21
			.byte	ANI_INTSTREAM,3,4

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,6,AA_TOGROUND
# on ground now

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS
			
			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_TURNHACK
			.word	-420

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	layup4_anim
			.byte	SCRIPT
layup4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030
			
			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_INTTOSEQH,3
			.word	SEQ_LAY4_R,SEQ_LAY4_L
			.byte	0

# allow the feet to plant
			.byte	ANI_INTSTREAM,16,8

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.75

			.byte	ANI_LAYUP
			.word	40,43,JOINT_RWRIST
			.float	0.0

# time has to match the time fed into ANI_LAYUP
			#.byte	ANI_INTSTREAM,26,30
			.byte	ANI_INTSTREAM,20,30
			#.byte	ANI_INTSTREAM,1,10
			.byte	ANI_INTSTREAM,6,6

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,9,AA_TOGROUND
# on ground now

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	layup7_anim
			.byte	SCRIPT
layup7_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup7_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY7_L
			.byte	0

			.byte	ANI_GOTO
			.word	layup7_hands_done

layup7_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY7_R
			.byte	0

layup7_hands_done:
# allow the feet to plant
			.byte	ANI_RUNTOFRAME,4,1

			.byte	ANI_LAYUP
			.word	30,15,JOINT_RWRIST
			.float	0.4

# time has to match the time fed into ANI_LAYUP
			.byte	ANI_INTSTREAM,19,25

# SLAM the ball through the net
			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,2,AA_TOGROUND
# on ground now

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP
			
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS
			
			.byte	ANI_STREAMTOEND
			.float	1.5

			.byte	ANI_JIFSEQ
			.word	SEQ_LAY7_R,layup7_thack_r

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_GOTO
			.word layup7_thack_done

layup7_thack_r:
			.byte	ANI_TURNHACK
			.word	-256

layup7_thack_done:
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;;;;;;;; 360 degree turn then layup ball

			.globl	layup6_anim
			.byte	SCRIPT
layup6_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup6_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY6A_L
			.byte	0

			.byte	ANI_INTSTREAM,21,5

			.byte	ANI_LAYUP
			.word	37,52,JOINT_RWRIST
			.float	0.30

			.byte	ANI_GOTO
			.word	layup6_continue
			
layup6_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY6A_R
			.byte	0

			.byte	ANI_INTSTREAM,21,5

			.byte	ANI_LAYUP
			.word	37,52,JOINT_LWRIST
			.float	0.30

layup6_continue:
			.byte	ANI_INTSTREAM,30,23

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,9,13

			.byte	ANI_INTSTREAM,1,AA_TOGROUND

			.byte	ANI_ZEROVELS

			.byte	ANI_RUNTOFRAME,63,1

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;	one handed leap then layup ball

			.globl	layup1_anim
			.byte	SCRIPT
layup1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup1_r

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY1_L
			.byte	0

			.byte	ANI_INTSTREAM,14,5

#			.byte	ANI_SETFLAG
#			.word	PF_LAYUP

			.byte	ANI_LAYUP
			.word	30,36,JOINT_LWRIST
			.float	.35

			.byte	ANI_GOTO
			.word	layup1_continue

layup1_r:
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_LAY1_R
			.byte	0

			.byte	ANI_INTSTREAM,14,5

			.byte	ANI_LAYUP
			.word	30,36,JOINT_RWRIST
			.float	.35

layup1_continue:

			.byte	ANI_INTSTREAM,4,4
			.byte	ANI_INTSTREAM,18,16

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,3,5

			.byte	ANI_INTSTREAM,14,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	2.5

			.byte	ANI_PREP_XITION
			.word	7,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;	one handed leap then layup ball

			.globl	layup16_anim
			.byte	SCRIPT
layup16_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup16_r

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_LAY16_L
			.byte	3

			.byte	ANI_LAYUP
			.word	40,16,JOINT_LWRIST
			.float	.35

			.byte	ANI_GOTO
			.word	layup16_continue

layup16_r:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_LAY16_R
			.byte	3

			.byte	ANI_LAYUP
			.word	40,16,JOINT_RWRIST
			.float	.35

layup16_continue:

			.byte	ANI_INTSTREAM,12,28

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,4,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.45

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;	one handed leap then layup ball

			.globl	layup17_anim
			.byte	SCRIPT
layup17_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_SETFRICTION
			.float	.030

			.byte	ANI_SETFLAG
			.word	PF_LAYUP

			.byte	ANI_CODE_BNZ
			.word	ball_in_righthand,0,0,0,layup17_r

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_LAY17_L
			.byte	5

			.byte	ANI_LAYUP
			.word	40,15,JOINT_LWRIST
			.float	.45

			.byte	ANI_GOTO
			.word	layup17_continue

layup17_r:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_LAY17_R
			.byte	5

			.byte	ANI_LAYUP
			.word	40,15,JOINT_RWRIST
			.float	.45

layup17_continue:

			.byte	ANI_INTSTREAM,17,28

			.byte	ANI_CODE
			.word	Shot_Release,0,0,0

			.byte	ANI_CODE
			.word	layup_ball_speech,0,0,0

			.byte	ANI_INTSTREAM,5,AA_TOGROUND

			.byte	ANI_CLRFLAG
			.word	PF_LAYUP

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_ZEROVELS

			.byte	ANI_STREAMTOEND
			.float	1.45

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

###############################################################
# Player's are falling all over the place
# quad4 works as follows:
# player is: 0-behind, 1-left, 2-front, 3-right


##########################
# FLAILS and FALLS       #
##########################

#;;;;;;;;;;;;;;;;; player got hit by dunker (left)
			
			.globl	h_dunker_hit_me_l_anim
			.byte	SCRIPT
h_dunker_hit_me_l_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS
	
			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_K_REAC_L
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.7

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; player got hit by dunker (right)

			.globl	h_dunker_hit_me_r_anim
			.byte	SCRIPT
h_dunker_hit_me_r_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_K_REAC_R
			.byte	0

			.byte	ANI_STREAMTOEND
			.float	0.7

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; front push react

			.globl	h_pushed_front_anim
			.byte	SCRIPT
h_pushed_front_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85
			.byte	ANI_SETYVEL
			.float	.10

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRNT_3

			.byte	ANI_INTSTREAM,13,6

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
			.byte	1
			
			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.020

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf3_fast

			.byte	ANI_INTSTREAM,11,7

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf3_fast

			.byte	ANI_INTSTREAM,11,7

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf3_fast

			.byte	ANI_INTSTREAM,11,7

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf3_fast

			.byte	ANI_INTSTREAM,11,7

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf3_fast

			.byte	ANI_INTSTREAM,11,8

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf3_fast

			.byte	ANI_INTSTREAM,13,9

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

pf3_fast:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	0.35

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; front push react

			.globl	h_pushed_front2_anim
			.byte	SCRIPT
h_pushed_front2_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRNT_6
			.byte	ANI_SKIPFRAMES,5

			.byte	ANI_INTSTREAM,5,9

			.byte	ANI_INTSTREAM,24,18

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,11,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,11,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,11,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,11,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,11,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,11,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,10,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_INTSTREAM,10,8
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,pf2_fast

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

pf2_fast:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	0.35

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; front push react #3

#			.globl	h_stif_r2_anim
#			.byte	SCRIPT
#h_stif_r2_anim:
#			.byte	ANI_CODE
#			.word	SetHangOnRim,NO,NO,-1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.40
#
#			.byte	ANI_SETFLAG
#			.word	PF_NO_LOOK
#
##			.byte	ANI_CODE
##			.word	face_attacker,0,0,0
#	
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_STIF_R2
#			.byte	0
#
#			.byte	ANI_SETFRICTION
#			.float	.013
#
#			.byte	ANI_INTSTREAM,47,34
#			
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,0
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_STREAMTOEND
#			.float	0.50
#
#			.byte	ANI_CLRFLAG
#			.word	PF_NO_LOOK
#
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END


#;;;;;;;;;;;;;;;;; back push react


			.globl	h_pushed_back_anim
			.byte	SCRIPT
h_pushed_back_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.90
			.byte	ANI_SETYVEL
			.float	.20

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BACK5

			.byte	ANI_INTSTREAM,7,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
			.byte	1
			
			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.024

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,4

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,5

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,7,5

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,8,5

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psb_fast
			.byte	ANI_INTSTREAM,8,5

#			.byte	ANI_INTSTREAM,73,45

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


psb_fast:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	.25

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END
		

#;;;;;;;;;;;;;;;;; front push react -- fouled

			.globl	h_pushed_front3_anim
			.byte	SCRIPT
h_pushed_front3_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85
			.byte	ANI_SETYVEL
			.float	.10

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRNT_3

			.byte	ANI_INTSTREAM,13,6

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
			.byte	1
			
			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.010

			.byte	ANI_STREAMTOEND
			.float	2.3

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; back push react from foul


			.globl	h_pushed_back2_anim
			.byte	SCRIPT
h_pushed_back2_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.90
			.byte	ANI_SETYVEL
			.float	.20

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BACK5

			.byte	ANI_INTSTREAM,7,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
			.byte	1
			
			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.014

#			.byte	ANI_INTSTREAM,73,55

			.byte	ANI_STREAMTOEND
			.float	2.45

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;  player gets hit on side - from foul

			.globl	h_pushed_side2_r_anim
			.byte	SCRIPT
h_pushed_side2_r_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SID1_R

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTSTREAM,7,3

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	.45
			.byte	1
			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_STREAMTOEND
			.float	1.55

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;  player gets hit on side from foul

			.globl	h_pushed_side2_l_anim
			.byte	SCRIPT
h_pushed_side2_l_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETFRICTION
			.float	0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SID1_L

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTSTREAM,7,3

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	.45
			.byte	1
			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_STREAMTOEND
			.float	1.55

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;;;;;;;; pushed from behind and stumble

			.globl	h_back_stumble_anim
			.byte	SCRIPT
h_back_stumble_anim:
			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_RPUSH1

			.byte	ANI_SETFRICTION
			.float	0.01

			.byte	ANI_STREAMTOEND
			.float	0.75

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; pushed from left side and stumble

			.globl	h_side_l_stumble_anim
			.byte	SCRIPT
h_side_l_stumble_anim:
			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_RPUSH5_L

			.byte	ANI_SETFRICTION
			.float	0.01

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; pushed from left side and stumble

			.globl	h_side_r_stumble_anim
			.byte	SCRIPT
h_side_r_stumble_anim:
			.byte	ANI_SETVELS
			.float	0.0,0.0,1.3

			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_RPUSH5_R

			.byte	ANI_SETFRICTION
			.float	0.01

			.byte	ANI_STREAMTOEND
			.float	0.70

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit on side (flipped)

			.globl	h_pushed_side_r_anim
			.byte	SCRIPT
h_pushed_side_r_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SID1_R

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTSTREAM,7,3

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	.45
			.byte	1
			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.025

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast

			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,5
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,5
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,5

#			.byte	ANI_STREAMTOEND
#			.float	.35

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

psr_fast:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	.25

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit on side

			.globl	h_pushed_side_l_anim
			.byte	SCRIPT
h_pushed_side_l_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK
			.byte	ANI_CLRFLAG
			.word	PF_BOTH_HANDS

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SID1_L

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTSTREAM,7,3

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	.45
			.byte	1
			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.025

			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast

			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,12,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,4
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,5
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,5
			.byte	ANI_CODE_BNZ
			.word	non_bh_plyr_is_holding_but,BUT_B,0,0,psr_fast
			.byte	ANI_INTSTREAM,13,5

#			.byte	ANI_STREAMTOEND
#			.float	.35

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

psl_fast:
			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

			.byte	ANI_STREAMTOEND
			.float	.25

			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; hit in front and flail

			.globl	h_front_flail_anim
			.byte	SCRIPT
h_front_flail_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.55
			.byte	ANI_SETYVEL
			.float	.75

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_H_FRNT_1
			.byte	21

			.byte	1
			.byte	ANI_INTSTREAM,17,AA_TOGROUND

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_SETFRICTION
			.float	.15

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

#			.byte	ANI_SETMODE
#			.word	0
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;; hit from behind and flail

			.globl	h_back_flail_anim
			.byte	SCRIPT
h_back_flail_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.55
			.byte	ANI_SETYVEL
			.float	.65

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_H_BACK_1
			.byte	15

			.byte	1
			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.19

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

#			.byte	ANI_SETMODE
#			.word	0
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; hit from left side and flail

			.globl	h_l_side_flail_anim
			.byte	SCRIPT
h_l_side_flail_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SDE1_L

			.byte	ANI_SET_FACE_REL_VEL
			.float	.20
			.byte	ANI_SETYVEL
			.float	.55

#			.byte	ANI_RUNTOFRAME,6,1

			.byte	1
			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.13

			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_RUNTOEND,1

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

#			.byte	ANI_SETMODE
#			.word	0
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; hit from right side and flail

			.globl	h_r_side_flail_anim
			.byte	SCRIPT
h_r_side_flail_anim:
			.byte	ANI_CODE
			.word	SetHangOnRim,NO,NO,-1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_NO_LOOK

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SDE1_R

			.byte	ANI_SET_FACE_REL_VEL
			.float	.20
			.byte	ANI_SETYVEL
			.float	.55

#			.byte	ANI_RUNTOFRAME,6,1

			.byte	1
			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.13

			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_RUNTOEND,1

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CLRFLAG
			.word	PF_NO_LOOK

#			.byte	ANI_SETMODE
#			.word	0
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;; Player Taunts

#			.globl	pumpfist_afterscore_1_anim
#			.byte	SCRIPT
#pumpfist_afterscore_1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_TAUNT1
#			.byte	0
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTSTREAM,25,14
#			
#			.byte	ANI_CODE
#			.word	plyr_speech,YEAH_1,0,0
#
#			.byte	ANI_INTSTREAM,36,27
#			
#			.byte	ANI_CODE
#			.word	plyr_speech,YEAH_2,0,0
#			
#			.byte	ANI_INTSTREAM,12,12
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

#			.globl	taunt_1_anim
#			.byte	SCRIPT
#taunt_1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_TAUNT1
#			.byte	0
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTSTREAM,25,14
#
#			.byte	ANI_CODE
#			.word	plyr_speech,YEAH_1,0,0
#
#			.byte	ANI_INTSTREAM,36,27
#
#			.byte	ANI_CODE
#			.word	plyr_speech,YEAH_2,0,0
#
#			.byte	ANI_INTSTREAM,32,27
#
#			.byte	ANI_CODE
#			.word	plyr_speech,YEAH_3,0,0
#
#			.byte	ANI_STREAMTOEND
#			.float	0.5
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

#			.globl	taunt_2_anim
#			.byte	SCRIPT
#taunt_2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORY
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TAUNT2
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_CODE
#			.word 	plyr_speech,0,0,0
#
#			.byte	ANI_STREAMTOEND
#			.float	1.9
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			#.byte	ANI_SETMODE
#			#.word	0
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

#			.globl	taunt_3_anim
#			.byte	SCRIPT
#taunt_3_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORY
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TAUNT3
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_CODE
#			.word 	plyr_speech,5,0,0
#
#taunt_3_loop:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TAUNT3
#
#			.byte	ANI_STREAMTOEND
#			.float	1.1
#
#			.byte	ANI_GOTO
#			.word taunt_3_loop
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			#.byte	ANI_SETMODE
#			#.word	0
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

#			.globl		taunt_4_anim
#			.byte	SCRIPT
#taunt_4_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_TAUNT4
#			.byte	0
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_SETCAMBOT
#			.word	csi_taunt4
#
#			.byte	ANI_INTSTREAM,10,20
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_rand,3,0,0,taunt_4_other_speech
#
#			.byte	ANI_CODE
#			.word 	plyr_speech,70,0,0
#			
#			.byte	ANI_GOTO
#			.word taunt4_cont_after_speech
#
#taunt_4_other_speech:
#			.byte	ANI_CODE
#			.word 	plyr_speech,16,0,0
#
#taunt4_cont_after_speech:
#			.byte	ANI_STREAMTOEND
#			.float	1.0
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CODE
#			.word	neutral_stick_general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl	hit_in_back_1
			.byte	SCRIPT
hit_in_back_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_H_BACK_1
			.byte	15

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.90

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl hit_in_front_1
			.byte	SCRIPT
hit_in_front_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_H_FRNT_1
			.byte	25

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_STREAMTOEND
			.float	0.95

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl hit_on_left_1
			.byte	SCRIPT
hit_on_left_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_H_SDE1_L
			.byte	0

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;;;;;;

			.globl hit_on_right_1
			.byte	SCRIPT
hit_on_right_1:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING

			.byte	ANI_CLRFLAG
			.word	PF_PLYRFLAGS

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_H_SDE1_R
			.byte	0

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_STREAMTOEND
			.float	1.0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_CODE
			.word	neutral_stick_general_run,0,0,0

			.byte	ANI_END

");

