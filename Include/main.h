/*
 *		$Archive: /video/Nba/Include/main.h $
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

#ifndef __MAIN_H__
#define __MAIN_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_main_h = "$Workfile: main.h $ $Revision: 2 $";
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

/*
 *		TYPEDEFS
 */

/*
 *		GLOBAL PROTOTYPES
 */

/*
 *		GLOBAL VARIABLES
 */

/* file descriptor for the GT64010 */
__EXTERN__ int gt_fd __INIT__(-1);

/* DMA buffers used for rendering */
__EXTERN__ char *dma_buffer1 __INIT__(NULL);
__EXTERN__ char *dma_buffer2 __INIT__(NULL);

/* Number of ticks per second */
__EXTERN__ int tsec __INIT__(100);

/* CRC of the loaded executable */
__EXTERN__ ulong start_crc32 __INIT__(0);

#endif
