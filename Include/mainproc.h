/*
 *		$Archive: /video/Nba/Include/mainproc.h $
 *		$Revision: 12 $
 *		$Date: 3/11/99 10:07p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __MAINPROC_H__
#define __MAINPROC_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_mainproc_h = "$Workfile: mainproc.h $ $Revision: 12 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

/*
 *		USER INCLUDES
 */

#ifndef __SYSTEM_H__
#include "system.h"
#endif

/*
 *		DEFINES
 */

#define	CMOS_ADJ_RESTORED	1
#define	CMOS_DEAD			2

/*
 *		TYPEDEFS
 */

/*
 *		GLOBAL PROTOTYPES
 */

void main_proc(int *args);
void backdoor_proc(int *args);

/*
 *		GLOBAL VARIABLES
 */

__EXTERN__ int cmos_status __INIT__(0);
__EXTERN__ int pcount __INIT__(0);				/* process loop execution count */
__EXTERN__ int four_plr_ver __INIT__(1);		/* 0 for two player, 1 for 4 player */


#define FOVVAL	1024.0f

__EXTERN__ float fovval   __INIT__(FOVVAL);			/* field-of-view value for xform_limb_fs */
__EXTERN__ float fovfac   __INIT__(FOVVAL /512.0f);	/* field-of-view factor */
__EXTERN__ float fovfac2x __INIT__(FOVVAL /256.0f);	/* field-of-view factor (2x) */

#endif
