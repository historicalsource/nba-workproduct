#define SCALE 0.1446258f

SNODE bbplyr_skel[] =
{
	/* 0 */
	{0.0f, 0.0f, 0.0f,		/* Prot */
	0,						/* flags */
	{1, 9, 12}},	 		/* children */

	/* 1 */
	{0.0000f * SCALE, 12.6205 * SCALE, -0.0030f * SCALE,	/* torso */
	0,						/* flags */
	{2-1, 5-1, 8-1}},		/* children */

	/* 2 */
	{-16.5144f * SCALE, 49.2014f * SCALE, -0.2362f * SCALE,	/* right shoulder */
	0,						/* flags */
	{3-2, 0, 0}},			/* children */

	/* 3 */
	{-14.6365f * SCALE, 80.2949f * SCALE, 0.9908f * SCALE, /* right elbow */
	0,						/* flags */
	{4-3, 0, 0}},			/* children */

	/* 4 */
	{-14.6365f * SCALE, 106.9382f * SCALE, 2.1010f * SCALE,	/* right wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 5 */
	{16.5144f * SCALE, 49.2014f * SCALE, -0.2362f * SCALE,	/* left shoulder */
	0,						/* flags */
	{6-5, 0, 0}},			/* children */

	/* 6 */
	{14.6365f * SCALE, 80.2949f * SCALE, 0.9908f * SCALE,	/* left elbow */
	0,						/* flags */
	{7-6, 0, 0}},			/* children */

	/* 7 */
	{14.6386f * SCALE, 106.9382f * SCALE, 2.1010f * SCALE,	/* left wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 8 */
	{0.0f * SCALE, 56.7539f * SCALE, 0.5834f * SCALE,		/* head */
	(SN_HEAD_SCALE),						/* flags */
	{0, 0, 0}},				/* children */

	/* 9 */
	{-9.2219f * SCALE, 0.8180f * SCALE, 0.5843f * SCALE,		/* right hip */
	0,						/* flags */
	{10-9, 0, 0}},			/* children */

	/* 10 */
	{-7.0496f * SCALE, 45.4570f * SCALE, 1.1708f * SCALE,	/* right knee */
	0,						/* flags */
	{11-10, 0, 0}},			/* children */

	/* 11 */
	{-6.3404f * SCALE, 87.6455f * SCALE, -0.8995 * SCALE,	/* right ankle */
	SN_TURBO_LIGHT,						/* flags */
	{0, 0, 0}},				/* children */

	/* 12 */
	{9.2219f * SCALE, 0.8180f * SCALE, 0.5843f * SCALE,		/* left hip */
	0,						/* flags */
	{13-12, 0, 0}},			/* children */

	/* 13 */
	{7.0496f * SCALE, 45.4570f * SCALE, 1.1708f * SCALE,	/* left knee */
	0,						/* flags */
	{14-13, 0, 0}},			/* children */

	/* 14 */
	{6.3404f * SCALE, 87.6455f * SCALE, -0.8995 * SCALE,	/* left ankle */
	SN_TURBO_LIGHT,						/* flags */
	{0, 0, 0}},				/* children */
};

SNODE tophalf_skel[] =
{
	/* 0 */
	{0.0f, 0.0f, 0.0f,		/* Prot */
	SN_NODRAW,				/* flags */
	{1, 0, 0}},		 		/* children */

	/* 1 */
	{0.0000f * SCALE, 12.6205 * SCALE, -0.0030f * SCALE,	/* torso */
	0,						/* flags */
	{2-1, 5-1, 8-1}},		/* children */

	/* 2 */
	{-16.5144f * SCALE, 49.2014f * SCALE, -0.2362f * SCALE,	/* right shoulder */
	0,						/* flags */
	{3-2, 0, 0}},			/* children */

	/* 3 */
	{-14.6365f * SCALE, 80.2949f * SCALE, 0.9908f * SCALE, /* right elbow */
	0,						/* flags */
	{4-3, 0, 0}},			/* children */

	/* 4 */
	{-14.6365f * SCALE, 106.9382f * SCALE, 2.1010f * SCALE,	/* right wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 5 */
	{16.5144f * SCALE, 49.2014f * SCALE, -0.2362f * SCALE,	/* left shoulder */
	0,						/* flags */
	{6-5, 0, 0}},			/* children */

	/* 6 */
	{14.6365f * SCALE, 80.2949f * SCALE, 0.9908f * SCALE,	/* left elbow */
	0,						/* flags */
	{7-6, 0, 0}},			/* children */

	/* 7 */
	{14.6386f * SCALE, 106.9382f * SCALE, 2.1010f * SCALE,	/* left wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 8 */
	{0.0f * SCALE, 56.7539f * SCALE, 0.5834f * SCALE,		/* head */
	(SN_HEAD_SCALE),						/* flags */
	{0, 0, 0}},				/* children */
};

SNODE bothalf_skel[] =
{
	/* 0 */
	{0.0f, 0.0f, 0.0f,		/* Prot */
	0,						/* flags */
	{9, 12, 0}},	 		/* children */

	/* 1-8 */
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},

	/* 9 */
	{-9.2219f * SCALE, 0.8180f * SCALE, 0.5843f * SCALE,		/* right hip */
	0,						/* flags */
	{10-9, 0, 0}},			/* children */

	/* 10 */
	{-7.0496f * SCALE, 45.4570f * SCALE, 1.1708f * SCALE,	/* right knee */
	0,						/* flags */
	{11-10, 0, 0}},			/* children */

	/* 11 */
	{-6.3404f * SCALE, 87.6455f * SCALE, -0.8995 * SCALE,	/* right ankle */
	SN_TURBO_LIGHT,						/* flags */
	{0, 0, 0}},				/* children */

	/* 12 */
	{9.2219f * SCALE, 0.8180f * SCALE, 0.5843f * SCALE,		/* left hip */
	0,						/* flags */
	{13-12, 0, 0}},			/* children */

	/* 13 */
	{7.0496f * SCALE, 45.4570f * SCALE, 1.1708f * SCALE,	/* left knee */
	0,						/* flags */
	{14-13, 0, 0}},			/* children */

	/* 14 */
	{6.3404f * SCALE, 87.6455f * SCALE, -0.8995 * SCALE,	/* left ankle */
	SN_TURBO_LIGHT,						/* flags */
	{0, 0, 0}},				/* children */
};

// PLAYER MODELS

extern LIMB limb_horn;
LIMB *hornet_limbs[] =
{
	&limb_waist_h,
	&limb_torso_h,
	&limb_arm_urh,
	&limb_arm_lrh,
	&limb_hand_rh,
	&limb_arm_ulh,
	&limb_arm_llh,
	&limb_hand_lh,
	NULL,
	&limb_leg_urh,
	&limb_leg_lrh,
	&limb_foot_rh,
	&limb_leg_ulh,
	&limb_leg_llh,
	&limb_foot_lh
};

extern LIMB limb_alian;
LIMB *alien_limbs[] =
{
	&limb_waist_a,
	&limb_torso_a,
	&limb_arm_ura,
	&limb_arm_lra,
	&limb_hand_ra,
	&limb_arm_ula,
	&limb_arm_lla,
	&limb_hand_la,
	NULL,
	&limb_leg_ura,
	&limb_leg_lra,
	&limb_foot_ra,
	&limb_leg_ula,
	&limb_leg_lla,
	&limb_foot_la
};

extern LIMB limb_boome;
LIMB *pacers_limbs[] =
{
	&limb_waist_b,
	&limb_torso_b,
	&limb_arm_urb,
	&limb_arm_lrb,
	&limb_hand_rb,
	&limb_arm_ulb,
	&limb_arm_llb,
	&limb_hand_lb,
	NULL,
	&limb_leg_urb,
	&limb_leg_lrb,
	&limb_foot_rb,
	&limb_leg_ulb,
	&limb_leg_llb,
	&limb_foot_lb
};

extern LIMB limb_crunc;
LIMB *timberwolves_limbs[] =
{
	&limb_waist_c,
	&limb_torso_c,
	&limb_arm_urc,
	&limb_arm_lrc,
	&limb_hand_rc,
	&limb_arm_ulc,
	&limb_arm_llc,
	&limb_hand_lc,
	NULL,
	&limb_leg_urc,
	&limb_leg_lrc,
	&limb_foot_rc,
	&limb_leg_ulc,
	&limb_leg_llc,
	&limb_foot_lc
};

extern LIMB limb_raptr;
LIMB *raptors_limbs[] =
{
	&limb_waist_r,
	&limb_torso_r,
	&limb_arm_urr,
	&limb_arm_lrr,
	&limb_hand_rr,
	&limb_arm_ulr,
	&limb_arm_llr,
	&limb_hand_lr,
	NULL,
	&limb_leg_urr,
	&limb_leg_lrr,
	&limb_foot_rr,
	&limb_leg_ulr,
	&limb_leg_llr,
	&limb_foot_lr
};

extern LIMB limb_benny;
LIMB *bulls_limbs[] =
{
	&limb_waist_cb,
	&limb_torso_cb,
	&limb_arm_urcb,
	&limb_arm_lrcb,
	&limb_hand_rcb,
	&limb_arm_ulcb,
	&limb_arm_llcb,
	&limb_hand_lcb,
	NULL,
	&limb_leg_urcb,
	&limb_leg_lrcb,
	&limb_foot_rcb,
	&limb_leg_ulcb,
	&limb_leg_llcb,
	&limb_foot_lcb
};

extern LIMB limb_jam;
LIMB *biggy_smalls_limbs[] =
{
	&limb_waist_j,
	&limb_torso_j,
	&limb_arm_urj,
	&limb_arm_lrj,
	&limb_hand_rj,
	&limb_arm_ulj,
	&limb_arm_llj,
	&limb_hand_lj,
	NULL,
	&limb_leg_urj,
	&limb_leg_lrj,
	&limb_foot_rj,
	&limb_leg_ulj,
	&limb_leg_llj,
	&limb_foot_lj
};

extern LIMB limb_nikko;
LIMB *nikko_limbs[] =
{
	&limb_waist_n,
	&limb_torso_n,
	&limb_arm_urn,
	&limb_arm_lrn,
	&limb_hand_rn,
	&limb_arm_uln,
	&limb_arm_lln,
	&limb_hand_ln,
	NULL,
	&limb_leg_urn,
	&limb_leg_lrn,
	&limb_foot_rn,
	&limb_leg_uln,
	&limb_leg_lln,
	&limb_foot_ln
};

extern LIMB limb_hbear;
LIMB *rockets_limbs[] =
{
	&limb_waist_hb,
	&limb_torso_hb,
	&limb_arm_urhb,
	&limb_arm_lrhb,
	&limb_hand_rhb,
	&limb_arm_ulhb,
	&limb_arm_llhb,
	&limb_hand_lhb,
	NULL,
	&limb_leg_urhb,
	&limb_leg_lrhb,
	&limb_foot_rhb,
	&limb_leg_ulhb,
	&limb_leg_llhb,
	&limb_foot_lhb
};

extern LIMB limb_hawk;
LIMB *hawks_limbs[] =
{
	&limb_waist_ah,
	&limb_torso_ah,
	&limb_arm_urah,
	&limb_arm_lrah,
	&limb_hand_rah,
	&limb_arm_ulah,
	&limb_arm_llah,
	&limb_hand_lah,
	NULL,
	&limb_leg_urah,
	&limb_leg_lrah,
	&limb_foot_rah,
	&limb_leg_ulah,
	&limb_leg_llah,
	&limb_foot_lah
};

extern LIMB limb_goril;
LIMB *suns_limbs[] =
{
	&limb_waist_pg,
	&limb_torso_pg,
	&limb_arm_urpg,
	&limb_arm_lrpg,
	&limb_hand_rpg,
	&limb_arm_ulpg,
	&limb_arm_llpg,
	&limb_hand_lpg,
	NULL,
	&limb_leg_urpg,
	&limb_leg_lrpg,
	&limb_foot_rpg,
	&limb_leg_ulpg,
	&limb_leg_llpg,
	&limb_foot_lpg
};

extern LIMB limb_sly;
LIMB *nets_limbs[] =
{
	&limb_waist_s,
	&limb_torso_s,
	&limb_arm_urs,
	&limb_arm_lrs,
	&limb_hand_rs,
	&limb_arm_uls,
	&limb_arm_lls,
	&limb_hand_ls,
	NULL,
	&limb_leg_urs,
	&limb_leg_lrs,
	&limb_foot_rs,
	&limb_leg_uls,
	&limb_leg_lls,
	&limb_foot_ls
};

extern LIMB limb_femal;
LIMB *nana_limbs[] =
{
	&limb_waist_f,
	&limb_torso_f,
	&limb_arm_urf,
	&limb_arm_lrf,
	&limb_hand_rf,
	&limb_arm_ulf,
	&limb_arm_llf,
	&limb_hand_lf,
	NULL,
	&limb_leg_urf,
	&limb_leg_lrf,
	&limb_foot_rf,
	&limb_leg_ulf,
	&limb_leg_llf,
	&limb_foot_lf
};

extern LIMB limb_retro;
LIMB *retro_limbs[] =
{
	&limb_waist_rt,
	&limb_torso_rt,
	&limb_arm_urrt,
	&limb_arm_lrrt,
	&limb_hand_rrt,
	&limb_arm_ulrt,
	&limb_arm_llrt,
	&limb_hand_lrt,
	NULL,
	&limb_leg_urrt,
	&limb_leg_lrrt,
	&limb_foot_rrt,
	&limb_leg_ulrt,
	&limb_leg_llrt,
	&limb_foot_lrt
};

extern LIMB limb_wizrd;
LIMB *wizard_limbs[] =
{
	&limb_waist_w,
	&limb_torso_w,
	&limb_arm_urw,
	&limb_arm_lrw,
	&limb_hand_rw,
	&limb_arm_ulw,
	&limb_arm_llw,
	&limb_hand_lw,
	NULL,
	&limb_leg_urw,
	&limb_leg_lrw,
	&limb_foot_rw,
	&limb_leg_ulw,
	&limb_leg_llw,
	&limb_foot_lw
};

extern LIMB limb_oman;
LIMB *oldman_limbs[] =
{
	&limb_waist_om,
	&limb_torso_om,
	&limb_arm_urom,
	&limb_arm_lrom,
	&limb_hand_rom,
	&limb_arm_ulom,
	&limb_arm_llom,
	&limb_hand_lom,
	NULL,
	&limb_leg_urom,
	&limb_leg_lrom,
	&limb_foot_rom,
	&limb_leg_ulom,
	&limb_leg_llom,
	&limb_foot_lom
};

extern LIMB limb_sasqu;
LIMB *sonics_limbs[] =
{
	&limb_waist_ss,
	&limb_torso_ss,
	&limb_arm_urss,
	&limb_arm_lrss,
	&limb_hand_rss,
	&limb_arm_ulss,
	&limb_arm_llss,
	&limb_hand_lss,
	NULL,
	&limb_leg_urss,
	&limb_leg_lrss,
	&limb_foot_rss,
	&limb_leg_ulss,
	&limb_leg_llss,
	&limb_foot_lss
};

extern LIMB limb_clown;
LIMB *clown_limbs[] =
{
	&limb_waist_cc,
	&limb_torso_cc,
	&limb_arm_urcc,
	&limb_arm_lrcc,
	&limb_hand_rcc,
	&limb_arm_ulcc,
	&limb_arm_llcc,
	&limb_hand_lcc,
	NULL,
	&limb_leg_urcc,
	&limb_leg_lrcc,
	&limb_foot_rcc,
	&limb_leg_ulcc,
	&limb_leg_llcc,
	&limb_foot_lcc
};

extern LIMB limb_pumpk;
LIMB *pumpkin_limbs[] =
{
	&limb_waist_p,
	&limb_torso_p,
	&limb_arm_urp,
	&limb_arm_lrp,
	&limb_hand_rp,
	&limb_arm_ulp,
	&limb_arm_llp,
	&limb_hand_lp,
	NULL,
	&limb_leg_urp,
	&limb_leg_lrp,
	&limb_foot_rp,
	&limb_leg_ulp,
	&limb_leg_llp,
	&limb_foot_lp
};

extern LIMB limb_rocky;
LIMB *rocky_limbs[] =
{
	&limb_waist_dr,
	&limb_torso_dr,
	&limb_arm_urdr,
	&limb_arm_lrdr,
	&limb_hand_rdr,
	&limb_arm_uldr,
	&limb_arm_lldr,
	&limb_hand_ldr,
	NULL,
	&limb_leg_urdr,
	&limb_leg_lrdr,
	&limb_foot_rdr,
	&limb_leg_uldr,
	&limb_leg_lldr,
	&limb_foot_ldr
};

extern LIMB limb_frank;
LIMB *frank_limbs[] =
{
	&limb_waist_uf,
	&limb_torso_uf,
	&limb_arm_uruf,
	&limb_arm_lruf,
	&limb_hand_ruf,
	&limb_arm_uluf,
	&limb_arm_lluf,
	&limb_hand_luf,
	NULL,
	&limb_leg_uruf,
	&limb_leg_lruf,
	&limb_foot_ruf,
	&limb_leg_uluf,
	&limb_leg_lluf,
	&limb_foot_luf
};

extern LIMB limb_mummy;
LIMB *mummy_limbs[] =
{
	&limb_waist_um,
	&limb_torso_um,
	&limb_arm_urum,
	&limb_arm_lrum,
	&limb_hand_rum,
	&limb_arm_ulum,
	&limb_arm_llum,
	&limb_hand_lum,
	NULL,
	&limb_leg_urum,
	&limb_leg_lrum,
	&limb_foot_rum,
	&limb_leg_ulum,
	&limb_leg_llum,
	&limb_foot_lum
};
extern LIMB limb_bride;
LIMB *bride_limbs[] =
{
	&limb_waist_tb,
	&limb_torso_tb,
	&limb_arm_urtb,
	&limb_arm_lrtb,
	&limb_hand_rtb,
	&limb_arm_ultb,
	&limb_arm_lltb,
	&limb_hand_ltb,
	NULL,
	&limb_leg_urtb,
	&limb_leg_lrtb,
	&limb_foot_rtb,
	&limb_leg_ultb,
	&limb_leg_lltb,
	&limb_foot_ltb
};
extern LIMB limb_wolfm;
LIMB *wolfm_limbs[] =
{
	&limb_waist_wm,
	&limb_torso_wm,
	&limb_arm_urwm,
	&limb_arm_lrwm,
	&limb_hand_rwm,
	&limb_arm_ulwm,
	&limb_arm_llwm,
	&limb_hand_lwm,
	NULL,
	&limb_leg_urwm,
	&limb_leg_lrwm,
	&limb_foot_rwm,
	&limb_leg_ulwm,
	&limb_leg_llwm,
	&limb_foot_lwm
};

extern LIMB limb_horse;
LIMB *horse_limbs[] =
{
	&limb_waist_ho,
	&limb_torso_ho,
	&limb_arm_urho,
	&limb_arm_lrho,
	&limb_hand_rho,
	&limb_arm_ulho,
	&limb_arm_llho,
	&limb_hand_lho,
	NULL,
	&limb_leg_urho,
	&limb_leg_lrho,
	&limb_foot_rho,
	&limb_leg_ulho,
	&limb_leg_llho,
	&limb_foot_lho
};

extern LIMB limb_ubear;
LIMB *ubear_limbs[] =
{
	&limb_waist_ub,
	&limb_torso_ub,
	&limb_arm_urub,
	&limb_arm_lrub,
	&limb_hand_rub,
	&limb_arm_ulub,
	&limb_arm_llub,
	&limb_hand_lub,
	NULL,
	&limb_leg_urub,
	&limb_leg_lrub,
	&limb_foot_rub,
	&limb_leg_ulub,
	&limb_leg_llub,
	&limb_foot_lub
};

LIMB *plyr_limbs_rm[] =
{
	&limb_waist_rm,
	&limb_torso_rm,
	&limb_arm_urrm,
	&limb_arm_lrrm,
	&limb_hand_rrm,
	&limb_arm_ulrm,
	&limb_arm_llrm,
	&limb_hand_lrm,
	NULL,
	&limb_leg_urrm,
	&limb_leg_lrrm,
	&limb_foot_rrm,
	&limb_leg_ulrm,
	&limb_leg_llrm,
	&limb_foot_lrm
};

LIMB *creff_limbs[] =
{
	&limb_waist_rf,
	&limb_torso_rf,
	&limb_arm_urrf,
	&limb_arm_lrrf,
	&limb_hand_rrf,
	&limb_arm_ulrf,
	&limb_arm_llrf,
	&limb_hand_lrf,
	NULL,
	&limb_leg_urrf,
	&limb_leg_lrrf,
	&limb_foot_rrf,
	&limb_leg_ulrf,
	&limb_leg_llrf,
	&limb_foot_lrf
};
LIMB *cheerleader_limbs[] =
{
	&limb_waist_cl,
	&limb_torso_cl,
	&limb_arm_urcl,
	&limb_arm_lrcl,
	&limb_hand_rcl,
	&limb_arm_ulcl,
	&limb_arm_llcl,
	&limb_hand_lcl,
	NULL,
	&limb_leg_urcl,
	&limb_leg_lrcl,
	&limb_foot_rcl,
	&limb_leg_ulcl,
	&limb_leg_llcl,
	&limb_foot_lcl
};
LIMB *creature_limbs[] =
{
	&limb_waist_uc,
	&limb_torso_uc,
	&limb_arm_uruc,
	&limb_arm_lruc,
	&limb_hand_ruc,
	&limb_arm_uluc,
	&limb_arm_lluc,
	&limb_hand_luc,
	NULL,
	&limb_leg_uruc,
	&limb_leg_lruc,
	&limb_foot_ruc,
	&limb_leg_uluc,
	&limb_leg_lluc,
	&limb_foot_luc
};

LIMB *hornet_reflect_limbs[] = {
	&limb_rft_lch,
	&limb_rft_rch,
	&limb_rlg_llch,
	&limb_rlg_lrch
};

LIMB *alien_reflect_limbs[] = {
	&limb_rft_la,
	&limb_rft_ra,
	&limb_rlg_lla,
	&limb_rlg_lra
};

LIMB *pacers_reflect_limbs[] = {
	&limb_rft_lb,
	&limb_rft_rb,
	&limb_rlg_llb,
	&limb_rlg_lrb
};

LIMB *timberwolves_reflect_limbs[] = {
	&limb_rft_lc,
	&limb_rft_rc,
	&limb_rlg_llc,
	&limb_rlg_lrc
};

LIMB *raptors_reflect_limbs[] = {
	&limb_rft_ltr,
	&limb_rft_rtr,
	&limb_rlg_lltr,
	&limb_rlg_lrtr
};

LIMB *bulls_reflect_limbs[] = {
	&limb_rft_lcb,
	&limb_rft_rcb,
	&limb_rlg_llcb,
	&limb_rlg_lrcb
};

LIMB *biggy_smalls_reflect_limbs[] = {
	&limb_rft_lj,
	&limb_rft_rj,
	&limb_rlg_llj,
	&limb_rlg_lrj
};

LIMB *nikko_reflect_limbs[] = {
	&limb_rft_ln,
	&limb_rft_rn,
	&limb_rlg_lln,
	&limb_rlg_lrn
};

LIMB *rockets_reflect_limbs[] = {
	&limb_rft_lhb,
	&limb_rft_rhb,
	&limb_rlg_llhb,
	&limb_rlg_lrhb
};

LIMB *hawks_reflect_limbs[] = {
	&limb_rft_lah,
	&limb_rft_rah,
	&limb_rlg_llah,
	&limb_rlg_lrah
};

LIMB *suns_reflect_limbs[] = {
	&limb_rft_lpg,
	&limb_rft_rpg,
	&limb_rlg_llpg,
	&limb_rlg_lrpg
};

LIMB *nets_reflect_limbs[] = {
	&limb_rft_ls,
	&limb_rft_rs,
	&limb_rlg_lls,
	&limb_rlg_lrs
};

LIMB *nana_reflect_limbs[] = {
	&limb_rft_lf,
	&limb_rft_rf,
	&limb_rlg_llf,
	&limb_rlg_lrf
};

LIMB *wizard_reflect_limbs[] = {
	&limb_rft_lw,
	&limb_rft_rw,
	&limb_rlg_llw,
	&limb_rlg_lrw
};

LIMB *oldman_reflect_limbs[] = {
	&limb_rft_lom,
	&limb_rft_rom,
	&limb_rlg_llom,
	&limb_rlg_lrom
};

LIMB *sonics_reflect_limbs[] = {
	&limb_rft_lss,
	&limb_rft_rss,
	&limb_rlg_llss,
	&limb_rlg_lrss
};

LIMB *clown_reflect_limbs[] = {
	&limb_rft_lcc,
	&limb_rft_rcc,
	&limb_rlg_llcc,
	&limb_rlg_lrcc
};

LIMB *pumpkin_reflect_limbs[] = {
	&limb_rft_lp,
	&limb_rft_rp,
	&limb_rlg_llp,
	&limb_rlg_lrp
};

LIMB *rocky_reflect_limbs[] = {
	&limb_rft_ldr,
	&limb_rft_rdr,
	&limb_rlg_lldr,
	&limb_rlg_lrdr
};

LIMB *frank_reflect_limbs[] = {
	&limb_rft_luf,
	&limb_rft_ruf,
	&limb_rlg_lluf,
	&limb_rlg_lruf
};

LIMB *mummy_reflect_limbs[] = {
	&limb_rft_lum,
	&limb_rft_rum,
	&limb_rlg_llum,
	&limb_rlg_lrum
};

LIMB *bride_reflect_limbs[] = {
	&limb_rft_ltb,
	&limb_rft_rtb,
	&limb_rlg_lltb,
	&limb_rlg_lrtb
};

LIMB *wolfm_reflect_limbs[] = {
	&limb_rft_lwm,
	&limb_rft_rwm,
	&limb_rlg_llwm,
	&limb_rlg_lrwm
};

LIMB *horse_reflect_limbs[] = {
	&limb_rft_lho,
	&limb_rft_rho,
	&limb_rlg_llho,
	&limb_rlg_lrho
};

LIMB *ubear_reflect_limbs[] = {
	&limb_rft_lub,
	&limb_rft_rub,
	&limb_rlg_llub,
	&limb_rlg_lrub
};

LIMB *plyr_reflect_limbs_rm[] = {
	&limb_rft_lrm,
	&limb_rft_rrm,
	&limb_rlg_llrm,
	&limb_rlg_lrrm
};

LIMB *creature_reflect_limbs[] = {
	&limb_rft_luc,
	&limb_rft_ruc,
	&limb_rlg_lluc,
	&limb_rlg_lruc
};
