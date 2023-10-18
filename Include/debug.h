/*
 *		$Archive: /video/Nba/Include/debug.h $
 *		$Revision: 3 $
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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_debug_h = "$Workfile: debug.h $ $Revision: 3 $";
#endif

/*
 *		DEFINES
 */

#ifdef DEBUG
#define DEBUGF(args)	debugf args
#define EXITF(args)		exitf args
#else
#define DEBUGF(args)	((void)0)
#define EXITF(args)		(exit(0))
#endif

/*
 *		GLOBAL PROTOTYPES
 */

void debugf(char *format, ...);
void exitf(char *format, ...);

#endif
