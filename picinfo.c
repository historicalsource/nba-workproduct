/*
 *		$Archive: /video/Nba/picinfo.c $
 *		$Revision: 12 $
 *		$Date: 9/10/99 6:24p $
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
char *ss_picinfo_c = "$Workfile: picinfo.c $ $Revision: 12 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ioctl.h>
#include <goose/goose.h>

/*
 *		USER INCLUDES
 */

#include "game.h"

extern int sysfont_height;
extern int reload;

#define DEV_PIC			494
//#define NBA25_PIC		467
#define NBA25_PIC		494		// Blitz 2000 Crud
#define NBA39_PIC		498

/*
 *		STATIC FUNCTIONS
 */

void show_pic_info(float *y_pos)
{
	int	serial_number;
	int	game_number;
	int	month;
	int	day;
	int	year;
	int	dom;
	float x_pos;
	int	fd;
	
	/* open the pic device */
	if ((fd = open("pic:", O_RDONLY)) < 0) {
		serial_number = -1;
		game_number = -1;
		dom = -1;
	} else {
		/* get the serial number */
		_ioctl(fd, FIOCGETSERIALNUMBER, (int)&serial_number);
		
		/* get the game number */
		_ioctl(fd, FIOCGETGAMENUMBER, (int)&game_number);
		
		/* get the date of manufactor code */
		_ioctl(fd, FIOCGETDOM, (int)&dom);
	}
	
	/* close the pic device */
	close(fd);
	
	/* ensure the pic is for this game */
	if ((game_number != DEV_PIC) && (game_number != NBA25_PIC) && (game_number != NBA39_PIC)) {
		while (TRUE)
			;
	}
	
	/* calculate the month, day, and year of manufacture */
	dom--;
	day = (dom % 31) + 1;
	month = ((dom % 372) / 31) + 1;
	year = (dom / 372) + 80;
	
	x_pos = SPRITE_HRES / 2.0F;
	*y_pos -= sysfont_height;
	set_text_position(x_pos, *y_pos, 160.0F);
	set_text_justification_mode(HJUST_CENTER | VJUST_CENTER);
	if (!reload)
	{	
		oprintf("%dcGame Serial Number: %dc%d",
				WHITE,
				LT_GREEN,
				serial_number);
		*y_pos -= sysfont_height;
		set_text_position(x_pos, *y_pos, 150.0F);
		oprintf("%dcDate of Manufacture: %dc%02d/%02d/%02d",
				WHITE,
				LT_GREEN,
				month,
				day,
				(year % 100));
		
		/* display the text */
		sleep(1);
	}
}	/* show_pic_info */
