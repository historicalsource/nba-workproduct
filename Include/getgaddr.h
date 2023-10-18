/*
 *		$Archive: /video/Nba/Include/getgaddr.h $
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

#ifndef __GETGADDR_H__
#define __GETGADDR_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_getgaddr_h = "$Workfile: getgaddr.h $ $Revision: 2 $";
#endif

/*
 *		USER INCLUDES
 */

#ifndef __SYSTEM_H__
#include "system.h"
#endif

/*
 *		GLOBAL PROTOTYPES
 */

uint get_text_addr(void);
uint get_text_end_addr(void);

#endif
