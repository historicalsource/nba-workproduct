/*
 *		$Archive: /video/Nba/debug.c $
 *		$Revision: 5 $
 *		$Date: 4/20/98 11:54p $
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
char *ss_debug_c = "$Workfile: debug.c $ $Revision: 5 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "debug.h"

/*
 *		GLOBAL FUNCTIONS
 */

#ifdef DEBUG
void debugf(char *format, ...)
{
	va_list arg_list;
	
	va_start(arg_list, format);
	vfprintf(stderr, format, arg_list);
	fflush(stderr);
	va_end(arg_list);
}	/* debugf */

void exitf(char *format, ...)
{
	va_list arg_list;
	
	va_start(arg_list, format);
	vfprintf(stderr, format, arg_list);
	fflush(stderr);
	va_end(arg_list);
	exit(0);
}	/* exitf */
#endif
