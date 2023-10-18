/*
 *		$Archive: /video/Nba/Include/form.h $
 *		$Revision: 2 $
 *		$Date: 4/21/98 12:25a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __FORM_H__
#define __FORM_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_form_h = "$Workfile: form.h $ $Revision: 2 $";
#endif

// offensive formations

fline for_pro_set[] =
{
	{-9.0, -17.5,stance3pt_anim},  	// left guard
	{-9.0, 0.0,center_set_anim},	// center
	{-9.0, 17.5,stance3pt_anim},  	// right guard
	{-15.5, 0.0,qbset_anim},  		// QB
	{-32.5, 10.0,wr_set_anim},  	// RB
	{-13.5, 60.0,wr_set_anim},		// WR1
	{-13.5, -60.0,wr_set_anim},		// WR2
};

fline for_shotgun[] =
{
	{-9.0, -17.5,stance3pt_anim},  	// left guard
	{-9.0, 0.0,center_set_anim},	// center
	{-9.0, 17.5,stance3pt_anim},  	// right guard
	{-47.5, 0.0,qbset_anim},  		// QB
	{-8.0, 30.0,wr_set_anim},  		// WR1 (TE)
	{-13.5, 75.0,wr_set_anim},		// WR2
	{-13.5, -60.0,wr_set2_anim},	// WR3
};

// defensive formations

fline for_std_def[] = 
{
	{-9.0, -17.5,stance3pt_anim},  	// LINE1
	{-9.0, 0.0,stance3pt_anim},	  	// LINE2
	{-9.0, 17.5,stance3pt_anim},  	// LINE3
	{-60.0, 0.0,stance2pt_anim_rnd},	// DB1
	{-20.0, 30.0,stance2pt_anim_rnd},  	// DB2
	{-29.5, 75.0,stance2pt_anim_rnd},	// DB3
	{-29.5, -60.0,stance2pt_anim_rnd},	// DB4
};

#endif
