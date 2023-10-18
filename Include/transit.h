/*
 *		$Archive: /video/Nba/Include/transit.h $
 *		$Revision: 1 $
 *		$Date: 11/02/98 2:34p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __TRANSIT_H__
#define __TRANSIT_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_transit_h = "$Workfile: transit.h $ $Revision: 1 $";
#endif

/*
 *		DEFINES
 */

#define	PLATE_TRANSITION_TICKS	10

#define	PLATES_COMING_OFF		-1
#define	PLATES_OFF				0
#define	PLATES_COMING_ON		1
#define	PLATES_ON				2

/*
 *		GLOBAL PROTOTYPES
 */

void create_plates(void);
void kill_plates(void);
void transit_proc(int *args);
void turn_transit_on();
void turn_transit_off(int sleep_until_off);

/*
 *		GLOBAL VARIABLES
 */

extern volatile int plates_on;

#endif
