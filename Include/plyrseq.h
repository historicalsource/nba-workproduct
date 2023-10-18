/*
 *		$Archive: /video/Nba/Include/plyrseq.h $
 *		$Revision: 161 $
 *		$Date: 4/05/99 5:56p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __PLYRSEQ_H__
#define __PLYRSEQ_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_plyrseq_h = "$Workfile: plyrseq.h $ $Revision: 161 $";
#endif

// BASKETBALL ANIMS

// TAUNTS
extern char
	taunt_1_anim[],
	taunt_2_anim[],
	taunt_3_anim[],
	taunt_4_anim[],
	taunt_5_anim[],
	pumpfist_afterscore_1_anim[],

// DRIBBLES
	dribble_stand_cocky[],
	spin_5[],
	spin_6[],
	cross_over_7[],

	spin_4[],
	spin_3[],
	spin_2[],
	dribble_run_right[],
	dribble_run_left[],

	dribble_snippet_3[],
	dribble_stand_backin[],
	dribble_backin_2[],

	dribble_run_cocky[],
	dribble_run_cocky_2[],
	dribble_run_cocky_rev[],

	dribble_spider_tween[],
	dribble_stand_7[],
	dribble_stand_7_int[],
	around_back_twice[],
	dribble_walk_1[],
	dribble_walk_1_int[],
	dribble_stand_6[],
	dribble_stand_6_int[],

	dribble_snippet_2[],
	dribble_snippet_1[],
	dribble_spider[],
	dribble_backin_1[],
	spin_anim_r[],
	spin_anim_l[],
	spin_1[],
	between_both_legs_1[],
	behindback_spin_1[],
	spinback_1[],
	spinback_2[],
	cross_over_1[],
	cross_over_3[],
	cross_over_5[],
	cross_over_6[],
	dribble_turn180_1[],
	iverson_spin_1[],

	dribble_slide_right_r_anim[],
	dribble_slide_right_l_anim[],
	dribble_slide_left_r_anim[],
	dribble_slide_left_l_anim[],

	between_legrun[],
	between_legrun_anim_r[],
	between_legrun_anim_l[],

	dribble_to_stop_1[],

	dribble_switch2_r[],
	dribble_switch2_l[],

	dribble_run_1[],
	dribble_run_1_int[],
	dribble_run_1_turbo[],

	dribble_run_2[],
	dribble_run_2_int[],
	dribble_run_2_turbo[],

	dribble_stand_1[],
	dribble_stand_1_int[],

	dribble_stand_2[],
	dribble_stand_2_int[],

	dribble_stand_3[],
	dribble_stand_3_int[],

	between_leg_spin_r[],
	between_leg_spin_l[],
	between_leg_anim_r[],
	between_leg_anim_l[],

	dribble_intostand_r_1[],
	dribble_intostand_l_1[],

// DUNKS
	ddunk_1[],
	ddunk_2[],
	ddunk_3[],
	put_back_dunk_1[],
	put_back_dunk_1a[],
	put_back_dunk_2[],
	put_back_dunk_3[],
	put_back_dunk_4[],
	put_back_dunk_5[],

	dunk_windup_1[],
	dunk_scissor_1[],
	dunk_scissor_1a[],
	dunk_scissor_2[],
	dunk_windmill_1[],
	dunk_windmill_1a[],
	dunk_windmill_2[],
	dunk_windmill_2a[],
	dunk_tomahawk_1[],
	dunk_tomahawk_1a[],
	dunk_360_3[],
	dunk_360_3a[],
	dunk_2hpmp_1[],
	dunk_2hpmp_1a[],
	dunk_2hpmp_2[],
	dunk_2hpmp_2a[],
	dunk_2hpmp_3[],
	dunk_2hpmp_3a[],
	dunk_360_1[],
	dunk_360_1a[],

	dunk_pump_4[],
	dunk_180_1[],
	dunk_180_1a[],
	dunk_1hand_but_out[],
	dunk_1_hand_1[],
	dunk_pump_1[],
	dunk_270h_1[],
	dunk_270h_1a[],
	dunk_1802_b[],
	dunk_2hrev_1[],
	dunk_2hrev_1a[],
	dunk_2hrev_2[],
	dunk_2hrev_2a[],
	dunk_1hand_4[],
	dunk_1hand_4a[],
	dunk_1hand_2[],
	dunk_1hand_2a[],
	dunk_2h360_1[],
	dunk_kwin2[],
	dunk_kwin2a[],
	dunk_kwin2b[],
	dunk_2h360_1a[],

	dunk_quick_1[],
	dunk_quick_2[],
	dunk_quick_3[],
	dunk_quick_4[],
	dunk_quick_5[],

	dunk_combined_1[],
	dunk_combined_1a[],
	dunk_combined_2[],
	dunk_combined_2a[],
	dunk_combined_3[],
	dunk_combined_3a[],
	dunk_combined_4[],
	dunk_combined_4a[],
	dunk_combined_5[],
	dunk_combined_5a[],
	dunk_combined_6[],
	dunk_combined_6a[],
	dunk_combined_7[],
	dunk_combined_7a[],
	dunk_combined_8[],
	dunk_combined_8a[],
	dunk_combined_9[],
	dunk_combined_9a[],
	dunk_combined_10[],
	dunk_combined_10a[],

	dunk_combined_11[],
	dunk_combined_12[],
	dunk_combined_12a[],
	dunk_combined_13[],
	dunk_combined_14[],
	dunk_combined_15[],

	dunk_combined_16[],
	dunk_combined_17[],
	dunk_combined_18[],
	dunk_combined_19[],
	dunk_combined_20[],
	dunk_combined_20a[],
	dunk_combined_21[],
	dunk_combined_21a[],
	dunk_combined_21b[],
	dunk_combined_21c[],
	dunk_combined_22[],
	dunk_combined_22a[],
	dunk_combined_23[],
	dunk_combined_23a[],
	dunk_combined_24[],
	dunk_combined_25[],
	dunk_combined_26[],
	dunk_combined_27[],
	dunk_combined_28[],
	dunk_combined_29[],
	dunk_combined_30[],


// DUNK RIM HANGS
	rim_hang_2h1_anim[],
	rim_hang_2h2_anim[],
	rim_hang_1h1_anim[],

// ALLYOOPS
	allyoop_x[],
	allyoop_1[],
	allyoop_2[],
	allyoop_2a[],
	allyoop_3[],
	allyoop_4[],
	allyoop_4a[],
	allyoop_5[],
	allyoop_6[],
	allyoop_7[],
	allyoop_8[],
	allyoop_9[],
	allyoop_10[],
	allyoop_pass_1[],
	allyoop_pass_2[],
	allyoop_run_pass_2[],
	twohand_ally_pass_1[],
	allyoop_1_r[],
	running_overhead_1_pass_ally[],
	overhead_1_pass_ally[],
	run_forward_spinpass_bb_1[],

// FREE_THROW anims
	wait_freethrow_1_anim[],
	wait_freethrow_2_anim[],
	wait_freethrow_3_anim[],
	wait_freethrow_4_anim[],
	freethrow_dribble_1_anim[],
	freethrow_dribble_2_anim[],
	freethrow_dribble_3_anim[],
	freethrow_shot_1_anim[],
	freethrow_shot_2_anim[],
	freethrow_shot_3_anim[],

// JUMP SHOTS
	shot_run_leaner_frwd_3[],
	shot_run_leaner_frwd_2[],
	shot_run_leaner_frwd[],
	shot_run_leaner_1[],
	shot_stand_heave_1[],
	shot_hook_1[],
	shot_run_heave_1[],
	shot1_anim[],
	shot2_anim[],
	shot3_anim[],
	shot4_anim[],
	shot5_anim[],
	shot6_anim[],
	shot8_anim[],
	shot_stand1_anim[],
	shot_fade1_anim[],
	shot_fade2_anim[],
	shot_behind_rim_anim[],
	shot_stand_heave_2[],
	shot_hook_2[],
	shot_hook_3[],
	shot_leaner_2_l_anim[],
	shot_leaner_2_r_anim[],
	shot_leaner_3_l_anim[],
	shot_leaner_3_r_anim[],
	shot_leaner_4_l_anim[],
	shot_leaner_4_r_anim[],

// LAY UPS
	auto_layup_1[],
	quick_dunk_layup_1[],
	dunk_layup_1[],
	dunk_layup_2[],
	dunk_layup_3[],
	layup7_anim[],
	layup9_anim[],
	layup14_anim[],
	layup15_anim[],
	layup16_anim[],
	layup17_anim[],
	layup4_anim[],
	layup6_anim[],
	layup1_anim[],
	layup23_anim[],
	layup24_anim[],
	layup25_anim[],
	layup26_anim[],
	layup27_anim[],
	layup28_anim[],

// DEFENSE MOVES
	dfake1_butt_anim[],
	dslide_anim2_int_r[],
	dslide_anim2_int_l[],

	steal_2hnd_anim[],
	dslide_anim_int_r[],
	dslide_anim_int_l[],
	dslide_anim_back[],
	dslide_anim_forward[],
	dstand_anim[],
	swat_2_release[],
	swat_2_anim_int[],
	dfake1_r_anim[],
	dfake1_l_anim[],

	swipe_low_anim[],
	swipe_mid_anim[],
	swipe_high_anim[],
	swipe3_anim_run[],

// BLOCKS
	quick_block_1h_anim[],
	quick_block_2h_1_anim[],
	quick_block_2h_2_anim[],
	block_1h_anim[],
	block_2h_anim[],
	block_2h_2_anim[],
	block_2h_3_anim[],

// SWATS
	swat_1h_1_anim[],
	swat_1h_1a_anim[],
	swat_1h_2_anim[],
	swat_1h_2a_anim[],
	swat_2h_1_anim[],
	swat_2h_1a_anim[],
	swat_2h_2_anim[],
	swat_2h_2a_anim[],
	swat_1h_behind_anim[],

// PASSES
	run_pass_side_2[],
	run_pass_side_3[],
	run_pass_side_4[],
	run_pass_side_5[],
	run_pass_side_6[],
	run_pass_behind_4[],
	run_pass_behind_5[],
	
	run_pass_forward_4[],
	pass_fullcourt_1[],
	run_pass_fullcourt_1[],

	pass_diag_right_1,
	run_pass_diag_right_1,

	pass_diag_left_1[],
	run_pass_diag_left_1[],

	air_pass_1[],
	air_pass_2[],
	air_pass_3[],
	air_pass_4[],
	air_pass_5[],
	air_pass_6[],

	ally_air_pass_1[],

	pass_forward_3[],
	run_pass_forward_3[],

	pass_forward_2[],
	run_pass_forward_2[],

	pass_forward_1[],
	pass_forward_1x[],
	pass_forward_1x2[],
	run_pass_forward_1[],

	cross_body_side_2[],
	run_cross_body_side_2[],

	cross_body_side_3[],
	run_cross_body_side_3[],

	cross_body_side_4[],
	run_cross_body_side_4[],

	dunk_pass_1[],
	dunk_pass_2[],
	dunk_pass_3[],
	dunk_pass_4[],
	dunk_pass_5[],

	forward_bounce_pass_1[],
	run_forward_bounce_pass_1[],

	bounce_pass_side_1[],
	run_bounce_pass_side_1[],

	cross_body_diag_behind_1[],
	run_cross_body_diag_behind_1[],

	cross_body_side_1[],
	run_cross_body_side_1[],

	pass_behind_4[],

	pass_behind_1[],
	run_pass_behind_1[],

	pass_behind_2[],
	run_pass_behind_2[],

	pass_behind_3[],
	run_pass_behind_3[],

	pass_behind_back_side_1[],
	run_pass_behind_back_side_1[],

	pass_behind_back_side_2[],
	run_pass_behind_back_side_2[],

	pass_behind_diag_1[],
	run_pass_behind_diag_1[],

	pass_side_1[],
	run_pass_side_1[],

	bounce_pass_side_2[],
	run_bounce_pass_side_2[],

	pass_behind_2[],
	run_pass_behind_2[],

	chest_pass_side_1[],
	run_chest_pass_side_1[],
// date ???
	chest_pass_2[],
	chest_pass_3[],
	chest_pass_4[],

	pass_up_1[],
	forward_hardpass_1[],
	run_forward_hardpass_1[],
	forward_pass_1[],
	pass_left_1[],
	pass_right_1[],

	run_forward_spinpass_1[],
	running_pass_thru_legs_behind[],
	over_shoulder_pass[],
	chest_pass_diagonal_r[],
	chest_pass_diagonal_l[],
	running_over_shoulder_pass[],

	//chest_pass_r[],
	//chest_pass_l[],
	//running_chest_pass_r[],
	//running_chest_pass_l[],

	jshot_pass_right_1_r[],
	jshot_pass_left_1_l[],

	overhead_1_pass[],
	overhead_2_pass[],
	running_overhead_1_pass[],
	chest_pass_anim[],
	right_pass_1_r[],
	left_pass_1_l[],
	lob1_pass_anim[],
	chest_pass_1[],
	running_chest_pass[],
	running_left_pass_1_l[],
	running_right_pass_1_r[],

// CATCHES
	catch_run_spin_behind_r[],
	catch_run_spin_behind_l[],

	catch_run_right_4[],
	catch_run_left_4[],
	catch_stand_right_4[],
	catch_stand_left_4[],

	catch_run_right_3[],
	catch_run_left_3[],
	catch_stand_right_3[],
	catch_stand_left_3[],

	catch_stand_forward_2[],
	catch_run_forward_2[],

	catch_run_right_2[],
	catch_run_left_2[],
	catch_stand_right_2[],
	catch_stand_left_2[],

	catch_stand_right_1[],
	catch_stand_left_1[],
	catch_run_right_1[],
	catch_run_left_1[],
	catch_stand_overhead_forward[],
	catch_run_overhead_forward[],
	catch_run_behind_l[],
	catch_run_behind_r[],
	catch_stand_behind_l[],
	catch_stand_behind_r[],
	catch_stand_forward_1[],
	catch_run_forward_1[],
	catch_onehand_r[],
	catch_onehand_l[],
	catch_running_onehand_r[],
	catch_running_onehand_l[],

// MISC...
	back_into_run_from_catch[],
	jog_2_anim[],
	jog_1_anim[],
	walk_1_anim[],
	cocky_run_1_anim[],
	boxer_1_anim[],
	boxer_run_legs_1_anim[],
	push_1h_1_anim[],
	push_1h_2_anim[],
	push_2h_1_anim[],
	push_2h_2_anim[],

	buyin_stance1_anim[],
	buyin_stance1a_anim[],
	buyin_stance2_anim[],
	buyin_stance3_anim[],
	buyin_stance4_anim[],
	throw_elbows_wb_anim[],
	throw_elbows_wb_2_anim[],
	hit_dude_wb_anim[],
	kick_anim[],
	s_breath1_anim[],
	s_breath2_anim[],
	s_breath3_anim[],
	s_breath5_anim[],
	s_breath1_anim_int[],
	s_breath2_anim_int[],
	s_breath3_anim_int[],
	s_breath5_anim_int[],

	pre_tip1_anim[],
	pre_tip3_anim[],
	pre_tip4_anim[],
	pre_tip5_anim[],
	referee_tipoff_anim[],
	tipoff_jump_ready_l_anim[],
	tipoff_jump_ready_r_anim[],
	tipoff_jump_up_l_anim[],
	tipoff_jump_up_r_anim[],
	tipoff_miss_r_anim[],
	tipoff_miss_l_anim[],
	tipoff_random_ready_anim[],
	tipoff_ready_stance_anim[],
	tipoff_ready_1_anim[],
	tipoff_ready_2_anim[],
	tipoff_ready_3_anim[],
	tipoff_ready_4_anim[],
	tipoff_ready_5_anim[],

	hit_in_back_1[],
	hit_in_front_1[],
	hit_on_left_1[],
	hit_on_right_1[],
	ball_pickup_1[],
	set_pick_1[],
	dribble_steal_react[],
	jumper_react_to_swat[],
	s_breath_dbg_anim_int[],
	s_ready_anim[],
	s_ready_anim_int[],

	run_slow_down_anim[],

	zero_anim[],

	ready_wb_anim[],
	ready_wb_2_anim[],
	ready_anim_int[],
	breath_anim[],
	breath_anim_int[],
	h_face_anim[],
	flail_0_anim[],
	flail_1_anim[],
	flail_2_anim[],
	h_dunker_hit_me_l_anim[],
	h_dunker_hit_me_r_anim[],
	shotblock_reaction_l[],
	shotblock_reaction_r[],
	h_pushed_side_l_anim[],
	h_pushed_side_r_anim[],
	h_stif_r1_anim[],
	h_stif_r2_anim[],
	h_pushed_front_anim[],
	h_pushed_front2_anim[],
	h_pushed_back_anim[],

	h_back_stumble_anim[],
	h_side_l_stumble_anim[],
	h_side_r_stumble_anim[],


	h_pushed_front3_anim[],
	h_pushed_back2_anim[],
	h_pushed_side2_l_anim[],
	h_pushed_side2_r_anim[],

	get_up_c_anim[],
	get_up_b_anim[],

	back_peddle[],
	run_anim[],
	run_anim_int[],
	run_animt[],
	run_anim_intt[],

	run_backpedal[],

	rebound1_anim[],
	rebound1_grab_anim[],
	rebound2_anim[],
	rebound2_grab_anim[],
	rebound3_anim[],
	rebound3_grab_anim[],
	rebound4_anim[],
	rebound4_grab_anim[],
	rebound5_anim[],
	rebound5_grab_anim[],
	rebound6_anim[],
	rebound6_grab_anim[],
	rebound7_anim[],
	rebound7_grab_anim[],
	quick_rebound_1_anim[],
	quick_rebound_2_anim[],
	low_rebound_1_anim[],
	low_rebound_2_anim[],
	wait_rebound_1_anim[],
	wait_rebound_2_anim[],

// FALLS & FLAILS
	h_front_flail_anim[],
	h_back_flail_anim[],
	h_l_side_flail_anim[],
	h_r_side_flail_anim[],

// used to find where the joint offsets are when a new dunk
// is implemented
	test_anim[]
	;
#endif
