/*****************************************************************************/
/*                                                                           */
/* FILE: SRMATH.C                                                            */
/*                                                                           */
/* Space Race                                                                */
/*                                                                           */
/* Ed's trig, matrix and vector functions.                                   */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

/***** INCLUDES **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include	<math.h>
	
/* GOOSE and GLIDE stuff */
#include <goose/goose.h>
#include <glide/glide.h>

/* our stuff */
//#include "include/rtg.h"
#include "srmath.h"

/* table of indices for get_adjoint4x4() */
int adj_index_data[] =   { 5,  9, 13,  6, 10, 14,  7, 11, 15,
			    	   	   2, 10, 14,  1,  9, 13,  3, 11, 15,
					   	   1,  5, 13,  2,  6, 14,  3,  7, 15,
					   	   2,  6, 10,  1,  5,  9,  3,  7, 11,
					   	   6, 10, 14,  4,  8, 12,  7, 11, 15,
					   	   0,  8, 12,  2, 10, 14,  3, 11, 15,
					   	   2,  6, 14,  0,  4, 12,  3,  7, 15,
					   	   0,  4,  8,  2,  6, 10,  3,  7, 11,
					   	   4,  8, 12,  5,  9, 13,  7, 11, 15,
					   	   1,  9, 13,  0,  8, 12,  3, 11, 15,
					   	   0,  4, 12,  1,  5, 13,  3,  7, 15,
					   	   1,  5,  9,  0,  4,  8,  3,  7, 11,
					   	   5,  9, 13,  4,  8, 12,  6, 10, 14,
					   	   0,  8, 12,  1,  9, 13,  2, 10, 14,
					   	   1,  5, 13,  0,  4, 12,  2,  6, 14,
					   	   0,  4,  8,  1,  5,  9,  2,  6, 10};
 

float sr_identity_matrix[] = {	1.0f, 0.0f, 0.0f, 0.0f,
 								0.0f, 1.0f, 0.0f, 0.0f,
 								0.0f, 0.0f, 1.0f, 0.0f,
 								0.0f, 0.0f, 0.0f, 1.0f};
                                
                                
/***** TRIG AND MATH FUNCTIONS ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: rand_range_f()                                                  */
/*                                                                           */
/* Returns a random floating point number between the min and max passed in. */
/* Gives two decimal places of randomness. Note the special case for values  */
/* less than 1.0f.                                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 16 Jul 97 MVB                                                             */
/* 23 Oct 97 MVB - added min == max condition                                */
/*                                                                           */
/*****************************************************************************/

float rand_range_f (float min, float max)

{ /* rand_range_f() */

float range;
float random_value;
float val;
float power;

/*****************************************************************************/

	range = max - min;

	if (range == 0.0f)
		return 0.0f;

	if (range > 1.0f)
		{
		/* add two decimal places of randomness */
		range *= 100.0f;
		random_value = (float)(rand() % (int)range);
		/* return to starting magnitude */
		random_value *= 0.01f;
		val = min + random_value;
		}
	else
		{
		/* multiply until greater than 1 */
		power = 1;
		while (range < 1.0f)
			{
			range *= 10.0;
			power *= 10;
			}
		/* add two decimal places of randomness */
		range *= 100.0f;
		random_value = (float)(rand() % (int)range);
		/* return to post-shift magnitude */
		random_value *= 0.01f;
		/* return to magnitude less than 1 */
		random_value /= power;
		val = min + random_value;
		}

	return val;

} /* rand_range_f() */

/***** END of rand_range_f() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: rand_range()                                                    */
/*                                                                           */
/* Returns a random integer number between the min and max passed in.        */
/*                                                                           */
/* 27 Jul 97 MVB                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int rand_range (int min, int max)

{ /* rand_range() */

int range;
int random_value;
int val;

/*****************************************************************************/

	range = max - min;
	random_value = (rand() % range);
	val = min + random_value;

	return val;

} /* rand_range() */

/***** END of rand_range() ***************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: rand_mod()                                                      */
/*                                                                           */
/* The rand() function returns an integer from 0 to (MAXINT - 1) where       */
/* MAXINT is the biggest integer value on this system.                       */
/*                                                                           */
/* This function returns 0 to (N - 1) where N is the integer passed in.      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 01 Jul 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int rand_mod (int val)

{ /* rand_mod() */

	return (rand() % val);

} /* rand_mod() */

/***** END of rand_mod() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: rand_modf()                                                     */
/*                                                                           */
/* Same as rand_mod() but returns a floating point value.                    */
/*                                                                           */
/* Any value passed in less than 1.0f returns 0.0f.                          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 01 Jul 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

float rand_modf (float val)

{ /* rand_modf() */

/*****************************************************************************/

	if (val < 1.0f)
		return 0.0f;

	return (float)(rand() % (int)val);

} /* rand_modf() */

/***** END of rand_modf() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: absf()                                                          */
/*                                                                           */
/* Floating point absolute value.                                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float absf (float x)

{ /* absf() */

 if(x < 0.0f)
   return(-x);
 else
   return(x);

} /* absf() */

/***** END of absf() *********************************************************/

// took out since now part of standard C
// 01 July 97 MVB

#if 0

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: abs()                                                           */
/*                                                                           */
/* Integer absolute value.                                                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

int abs (int x)

{ /* abs() */

 if(x < 0)
   return(-x);
 else
   return(x);

} /* abs() */

/***** END of abs() **********************************************************/

#endif

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: spherical_to_rect_zy()                                          */
/*                                                                           */
/* Same as spherical_to_rect() but y and z are swapped.                      */
/*                                                                           */
/* INPUT: float x,y,z                                                        */
/*                                                                           */
/* OUTPUT: radius (always positive),                                         */
/*         theta [0..PI]                                                     */
/*         phi [-PI..PI]                                                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Mar 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void spherical_to_rect_zy (float r, float theta, float phi,
						   float *x, float *y, float *z)

{ /* spherical_to_rect_zy() */

float sine_theta;

/*****************************************************************************/

	sine_theta = sine2(theta);
	*x = r * sine_theta * cosine2(phi);
	*z = r * sine_theta * sine2(phi);
	*y = r * cosine2(theta);

} /* spherical_to_rect_zy() */

/***** END of spherical_to_rect_zy() *****************************************/
 
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: spherical_to_rect()                                             */
/*                                                                           */
/* INPUT: float x,y,z                                                        */
/*                                                                           */
/* OUTPUT: radius (always positive),                                         */
/*         theta [0..PI]                                                     */
/*         phi [-PI..PI]                                                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 28 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void spherical_to_rect (float r, float theta, float phi,
						float *x, float *y, float *z)

{ /* spherical_to_rect() */

float sine_theta;

/*****************************************************************************/

	sine_theta = sine2(theta);
	*x = r * sine_theta * cosine2(phi);
	*y = r * sine_theta * sine2(phi);
	*z = r * cosine2(theta);

} /* spherical_to_rect() */

/***** END of spherical_to_rect() ********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: rect_to_spherical_zy()                                          */
/*                                                                           */
/* Same as rect_to_spherical() but y and z are swapped.                      */
/*                                                                           */
/* INPUT: float x,y,z                                                        */
/*                                                                           */
/* OUTPUT: radius (always positive),                                         */
/*         theta [0..PI]                                                     */
/*         phi [-PI..PI]                                                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Mar 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void rect_to_spherical_zy (float x, float y, float z, 
						   float *r, float *theta, float *phi)

{ /* rect_to_spherical_zy() */

	*r = fsqrt( (x*x + y*y + z*z) );
    *theta = arccos( y/(*r) );
    *phi = arctan2(z,x);

} /* rect_to_spherical_zy() */

/***** END of rect_to_spherical_zy() *****************************************/
 
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: rect_to_spherical()                                             */
/*                                                                           */
/* INPUT: float x,y,z                                                        */
/*                                                                           */
/* OUTPUT: radius (always positive),                                         */
/*         theta [0..PI]                                                     */
/*         phi [-PI..PI]                                                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 28 Feb 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void rect_to_spherical (float x, float y, float z, 
						float *r, float *theta, float *phi)

{ /* rect_to_spherical() */

	*r = fsqrt( (x*x + y*y + z*z) );
    *theta = arccos( z/(*r) );
    *phi = arctan2(y,x);

} /* rect_to_spherical() */

/***** END of rect_to_spherical() *********************************************/
  
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: arctan2()                                                       */
/*                                                                           */
/* Returns angle between -PI and PI.                                         */
/*                                                                           */
/* arctan(y/x) in range [-PI, PI].                                           */
/*                                                                           */
/* NOTE: if (y/x) approaches 1 the results become less accurate.             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 27 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

#define TAN_1_3 0.3333333333f
#define TAN_1_5 0.2f
#define TAN_1_7 0.142857142857f
#define TAN_1_9 0.111111111111f
#define TAN_1_11 9.0909090909e-2f
#define TAN_1_13 7.6923076923e-2f
#define TAN_1_15 6.6666666666e-2f
#define TAN_1_17 5.88235294118e-2f
#define TAN_1_19 5.26315789474e-2f
#define TAN_1_21 0.047619047619f
#define TAN_1_23 4.34782608696e-2f
#define TAN_1_25 0.04f
#define TAN_1_27 0.037037037037f

float arctan2 (float y, float x)

{ /* arctan2() */

float result;
float x_1;
float x_2;
float x_3;
float x_5;
float x_7;
float x_9;
float x_11;
float x_13;
float x_15;
float x_17;
float x_19;
float x_21;
float x_23;
float x_25;
float x_27;

float inv_x_1;
float inv_x_2;
float inv_x_3;
float inv_x_5;
float inv_x_7;
float inv_x_9;
float inv_x_11;
float inv_x_13;
float inv_x_15;

/***************************************************************************/

result = (float) SR_PI2;

	if( x == 0.0f  )
		{
		if (y >= 0.0f) 
			return((float) result);
	    else
			return((float) -result);
	    }

	if( y == 0.0f  )
		{
		if (x >= 0.0f) 
			return((float) 0.0f);
	    else
			return((float) SR_PI);
	    }

	x_1 = (float)y/x;    
	inv_x_1 = (float) 1.0f/x_1;


	if( (x_1 <= 1.0f) && (x_1 >= -1.0f) )
		{
		x_2 = x_1 * x_1;
	    x_3 = x_2 * x_1;
		x_5 = x_3 * x_2;
		x_7 = x_5 * x_2;
		x_9 = x_7 * x_2;
		x_11 = x_9 * x_2;
		x_13 = x_11 * x_2;
		x_15 = x_13 * x_2;
		x_17 = x_15 * x_2;
		x_19 = x_17 * x_2;
		x_21 = x_19 * x_2;
		x_23 = x_21 * x_2;
		x_25 = x_23 * x_2;
		x_27 = x_25 * x_2;

	    result = x_1 - TAN_1_3*x_3 + TAN_1_5*x_5 - TAN_1_7*x_7 
	    		+ TAN_1_9*x_9 - TAN_1_11*x_11 + TAN_1_13*x_13 
	            - TAN_1_15*x_15 + TAN_1_17*x_17 - TAN_1_19*x_19
	            + TAN_1_21*x_21 - TAN_1_23*x_23 + TAN_1_25*x_25 - TAN_1_27*x_27;
		}
	else if(x_1 > 1.0f)
		{
	    inv_x_2 = inv_x_1*inv_x_1;
		inv_x_3 = inv_x_2*inv_x_1;
		inv_x_5 = inv_x_3*inv_x_2;
		inv_x_7 = inv_x_5*inv_x_2;
		inv_x_9 = inv_x_7*inv_x_2;
		inv_x_11 = inv_x_9*inv_x_2;
		inv_x_13 = inv_x_11*inv_x_2;
		inv_x_15 = inv_x_13*inv_x_2;

	    result = SR_PI2 -inv_x_1 + TAN_1_3*inv_x_3 - TAN_1_5*inv_x_5 
	    		+ TAN_1_7*inv_x_7 - TAN_1_9*inv_x_9 + TAN_1_11*inv_x_11
	            - TAN_1_13*inv_x_13 + TAN_1_15*inv_x_15;
	    }
	else
		{
	    inv_x_2 = inv_x_1*inv_x_1;
		inv_x_3 = inv_x_2*inv_x_1;
		inv_x_5 = inv_x_3*inv_x_2;
		inv_x_7 = inv_x_5*inv_x_2;
		inv_x_9 = inv_x_7*inv_x_2;
		inv_x_11 = inv_x_9*inv_x_2;
		inv_x_13 = inv_x_11*inv_x_2;
		inv_x_15 = inv_x_13*inv_x_2;

	    result = -SR_PI2 -inv_x_1 + TAN_1_3*inv_x_3 - TAN_1_5*inv_x_5 
	    		+ TAN_1_7*inv_x_7 - TAN_1_9*inv_x_9 + TAN_1_11*inv_x_11
	            - TAN_1_13*inv_x_13 + TAN_1_15*inv_x_15;
	     }    
	    
	    
	if( (x < 0.0f) && (y >= 0.0f) )		/* quadrant 1 */
		{
		result = result + SR_PI;
	    }
	else if( (x < 0.0f) && (y < 0.0f) ) /* quadrant 2 */
		{
		result = -SR_PI + result;
	    }
	    
	return((float) result);

} /* arctan2() */

/***** END of arctan2() ******************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: arccos()                                                        */
/*                                                                           */
/* input: float between -1.0f and 1.0f		                                 */
/*                                                                           */
/* output: float between 0.0f to PI                                          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 27 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK cleaned up, switch to Horner's Method                       */
/* 14 May 97 EJK added piece wise linear interpolation                       */
/*                                                                           */
/*****************************************************************************/

#define ARCCOS_1  SR_PI2
#define ARCCOS_2  -1.0f
#define ARCCOS_3  -0.166666666666f
#define ARCCOS_4  -0.075f
#define ARCCOS_5  -4.46428571429e-2f
#define ARCCOS_6  -3.03819444444e-2f
#define ARCCOS_7  -2.23721590909e-2f
#define ARCCOS_8  -1.73527644231e-2f

#define ARCCOS_XA  0.8250f  /* piece wise linear interpolation x=0.8250,1.0 */
#define ARCCOS_YA  0.6006f
#define ARCCOS_XB  0.911f
#define ARCCOS_YB  0.4251f
#define ARCCOS_XC  0.95f
#define ARCCOS_YC  0.3176f
#define ARCCOS_XD  0.986f
#define ARCCOS_YD  0.1675f
#define ARCCOS_XE  1.0f
#define ARCCOS_YE  0.0f

#define ARCCOS_XAC  -0.8250f /* piece wise linear interpolation x=-1.0,-0.8250 */
#define ARCCOS_YAC   2.5410f
#define ARCCOS_XBC  -0.911f
#define ARCCOS_YBC  2.7165f
#define ARCCOS_XCC  -0.95f
#define ARCCOS_YCC  2.8240f
#define ARCCOS_XDC  -0.986f
#define ARCCOS_YDC  2.9741f
#define ARCCOS_XEC  -1.0f 
#define ARCCOS_YEC  SR_PI

float arccos (float x)

{ /* arccos() */

float x_2;
float temp = 0.0f;
float t;

/***************************************************************************/

	/* Make sure the input is between [-1.0f, 1.0f] */
	if (x> 1.0f)
    	{
        x = 1.0f;
        }
        
	if (x < -1.0f)
    	{
        x = -1.0f;
        }        

	/* piece wise linear interpolation x=-1.0,-0.8250 */
	if( x < ARCCOS_XAC )
		{ /* if (x < ARCCOS_XAC) */
		if ( x >= ARCCOS_XBC ) //(x < ARCCOS_XAC && x >= ARCCOS_XBC)
			{
			t = (x-ARCCOS_XBC)/(ARCCOS_XBC-ARCCOS_XAC);
			temp = ARCCOS_YBC + t*(ARCCOS_YBC-ARCCOS_YAC);
			return( temp );
		    }

		if ( x >= ARCCOS_XCC ) //(x < ARCCOS_XBC && x >= ARCCOS_XCC)
			{
			t = (x-ARCCOS_XCC)/(ARCCOS_XCC-ARCCOS_XBC);
			temp = ARCCOS_YCC + t*(ARCCOS_YCC-ARCCOS_YBC);
			return( temp );
	    	}

		if ( x >= ARCCOS_XDC ) //(x < ARCCOS_XCC && x >= ARCCOS_XDC)
			{
			t = (x-ARCCOS_XDC)/(ARCCOS_XDC-ARCCOS_XCC);
			temp = ARCCOS_YDC + t*(ARCCOS_YDC-ARCCOS_YCC);
			return( temp );
		    }

		if ( x >= ARCCOS_XEC ) //(x < ARCCOS_XDC && x >= ARCCOS_XEC)
			{
			t = (x-ARCCOS_XEC)/(ARCCOS_XEC-ARCCOS_XDC);
			temp = ARCCOS_YEC + t*(ARCCOS_YEC-ARCCOS_YDC);
			return( temp );
		    }
	    
		} /* if (x < ARCCOS_XAC) */

	/* piece wise linear interpolation x=0.8250,1.0 */
	if(x > ARCCOS_XA)
		{ /* if (x > ARCCOS_XAC) */
		if ( x <= ARCCOS_XB )  //(x > ARCCOS_XA && x <= ARCCOS_XB)
			{
			t = (x-ARCCOS_XA)/(ARCCOS_XB-ARCCOS_XA);
			temp = ARCCOS_YA + t*(ARCCOS_YB-ARCCOS_YA);
			return( temp );
		    }

		if ( x <= ARCCOS_XC ) //(x > ARCCOS_XB && x <= ARCCOS_XC)
			{
			t = (x-ARCCOS_XB)/(ARCCOS_XC-ARCCOS_XB);
			temp = ARCCOS_YB + t*(ARCCOS_YC-ARCCOS_YB);
			return( temp );
		    }

		if ( x <= ARCCOS_XD ) //(x > ARCCOS_XC && x <= ARCCOS_XD)
			{
			t = (x-ARCCOS_XC)/(ARCCOS_XD-ARCCOS_XC);
			temp = ARCCOS_YC + t*(ARCCOS_YD-ARCCOS_YC);
			return( temp );
		    }

		if ( x <= ARCCOS_XE ) // (x > ARCCOS_XD && x <= ARCCOS_XE)
			{
			t = (x-ARCCOS_XD)/(ARCCOS_XE-ARCCOS_XD);
			temp = ARCCOS_YD + t*(ARCCOS_YE-ARCCOS_YD);
			return( temp );
		    }
		        
		} /* if (x > XAC) */

		/* use the talyor series */
		x_2 = x * x;
		temp = (((((((ARCCOS_8*x_2+ARCCOS_7)*x_2+ARCCOS_6)*x_2+ARCCOS_5)*x_2+
	    					ARCCOS_4)*x_2+ARCCOS_3)*x_2+ARCCOS_2)*x+ARCCOS_1);
		return( temp );

} /* arccos() */

/***** END of arccos() *******************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: arcsin()                                                        */
/*                                                                           */
/* input: float between -1.0f and 1.0f		                                 */
/*                                                                           */
/* output: float between -PI/2 to PI/2                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 08 Oct 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float arcsin (float x)

{ /* arcsin() */

float temp;

/***************************************************************************/
	/* Make sure the input is between [-1.0f, 1.0f] */
	if (x> 1.0f)
    	{
        x = 1.0f;
        }
        
	if (x < -1.0f)
    	{
        x = -1.0f;
        }        

	/* What a relationship!!! */
    /* CRC p.142              */
    
	temp = SR_PI2 - arccos(x);

	return( temp );

} /* arcsin() */

/***** END of arcsin() *******************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: cosine()                                                        */
/*                                                                           */
/* X must be between -PI and PI.                                             */
/*                                                                           */
/* Very small angles are clamped to 0.0001 to avoid floating-point exception */
/* errors on the MIPS.                                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK cleaned up, switch to Horner's Method                       */
/*                                                                           */
/*****************************************************************************/

#define COS_1  1.0f
#define COS_2  -0.5f
#define COS_3  4.166666666e-2f
#define COS_4  -1.388888888e-3f

float cosine (float x)

{ /* cosine() */

static float x_2;

/***************************************************************************/

	/* Make sure the input is between [pi, pi] */
	if ( x > SR_PI)
    	{
        x = SR_PI;
        }
        
	if (x < -SR_PI)
    	{
        x = -SR_PI;
        }        


	if(x >= 0.0f && x < SR_PI2)
		{
		x_2 = x*x;    
		return( (((COS_4*x_2 + COS_3)*x_2+COS_2)*x_2+COS_1) );
		}

	if(x >= SR_PI2 && x <= SR_PI)
		{
	    x = SR_PI-x;
	   	x_2 = x*x;    
		return( -(((COS_4*x_2 + COS_3)*x_2+COS_2)*x_2+COS_1) );
		}

	if(x < 0.0f && x >= -SR_PI2)
		{
		x_2 = x*x;    
		return( (((COS_4*x_2 + COS_3)*x_2+COS_2)*x_2+COS_1) );
		}
	 
		/* Else, (x < -SR_PI2 && x >= -SR_PI) */
	    x = SR_PI+x;
		x_2 = x*x;    
		return( -(((COS_4*x_2 + COS_3)*x_2+COS_2)*x_2+COS_1) );

} /* cosine() */

/***** END of cosine() *******************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: sine()                                                          */
/*                                                                           */
/* X must be between -PI and PI.                                             */
/*                                                                           */
/* Very small angles are clamped to 0.0001 to avoid floating-point exception */
/* errors on the MIPS.                                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK cleaned up, switch to Horner's Method                       */
/*                                                                           */
/*****************************************************************************/

float sine (float x)

{ /* sine() */

#define SIN_1  1.0f
#define SIN_2  -0.16666666666f
#define SIN_3  8.3333333333e-3f
#define SIN_4  -1.9841269841e-4f

static float x_2;

/***************************************************************************/

	/* Make sure the input is between [pi, pi] */
	if ( x > SR_PI)
    	{
        x = SR_PI;
        }
        
	if (x < -SR_PI)
    	{
        x = -SR_PI;
        }        


	if(x >= 0 && x < SR_PI2)
		{
		if(x < 0.0001f )	x = 0.0001f;
	    x_2 = x*x;
		return( ((((SIN_4*x_2+SIN_3)*x_2+SIN_2)*x_2+SIN_1)*x) );
		}

	if(x >= SR_PI2 && x <= SR_PI)
		{
	    x = SR_PI-x;
		x_2 = x*x;
		return( ((((SIN_4*x_2+SIN_3)*x_2+SIN_2)*x_2+SIN_1)*x) );
		}

	if(x < 0 && x >= -SR_PI2)
		{
	    x = -x;
		x_2 = x*x;
		return( -((((SIN_4*x_2+SIN_3)*x_2+SIN_2)*x_2+SIN_1)*x) );
		}

		/* Else, (x < -SR_PI2 && x >= -SR_PI) */
	    x = SR_PI+x;
		x_2 = x*x;
		return( -((((SIN_4*x_2+SIN_3)*x_2+SIN_2)*x_2+SIN_1)*x) );

} /* sine() */

/***** END of sine() *********************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: cosine2()                                                       */
/*                                                                           */
/* Separate version of cosine() that has more terms and is more accurate.    */
/* But, it is also slower.                                                   */
/*                                                                           */
/* X must be between -PI and PI.                                             */
/*                                                                           */
/* Very small angles are clamped to 0.0001 to avoid floating-point exception */
/* errors on the MIPS.                                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK cleaned up, switch to Horner's Method                       */
/*                                                                           */
/*****************************************************************************/

#define COS2_1  1.0f
#define COS2_2  -0.5f
#define COS2_3  4.166666666e-2f
#define COS2_4  -1.388888888e-3f
#define COS2_5  2.480158730e-5f
#define COS2_6  -2.7557319224e-7f
#define COS2_7  2.08767569879e-9f

float cosine2 (float x)

{ /* cosine2() */

float x_2;

/***************************************************************************/

	/* Make sure the input is between [pi, pi] */
	if ( x > SR_PI)
    	{
        x = SR_PI;
        }
        
	if (x < -SR_PI)
    	{
        x = -SR_PI;
        }        

	if(x >= 0 && x < SR_PI2)
		{
		x_2 = x*x;    
		return( ((((((COS2_7*x_2+COS2_6)*x_2+COS2_5)*x_2+COS2_4)*x_2
	    		 						+COS2_3)*x_2+COS2_2)*x_2+COS2_1) );
		}

	if(x >= SR_PI2 && x <= SR_PI)
		{
	    x = SR_PI-x;
	   	x_2 = x*x;    
		
	   	return( -((((((COS2_7*x_2+COS2_6)*x_2+COS2_5)*x_2+COS2_4)*x_2
	   									+COS2_3)*x_2+COS2_2)*x_2+COS2_1) );

		}

	if(x < 0 && x >= -SR_PI2)
		{
		x_2 = x*x;    
	   	return( ((((((COS2_7*x_2+COS2_6)*x_2+COS2_5)*x_2+COS2_4)*x_2
	   									+COS2_3)*x_2+COS2_2)*x_2+COS2_1) );
		}
	 
		/* Else, (x < -SR_PI2 && x >= -SR_PI) */
	    x = SR_PI+x;
		x_2 = x*x;
	   	return( -((((((COS2_7*x_2+COS2_6)*x_2+COS2_5)*x_2+COS2_4)*x_2
	   									+COS2_3)*x_2+COS2_2)*x_2+COS2_1) );

} /* cosine2() */

/***** END of cosine2() ******************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: sine2()                                                         */
/*                                                                           */
/* Separate version of sine() that has more terms and is more accurate.      */
/* But, it is also slower.                                                   */
/*                                                                           */
/* X must be between -PI and PI.                                             */
/*                                                                           */
/* Very small angles are clamped to 0.0001 to avoid floating-point exception */
/* errors on the MIPS.                                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK cleaned up, switch to Horner's Method                       */
/*                                                                           */
/*****************************************************************************/

#define SIN2_1  1.0f
#define SIN2_2  -0.16666666666f
#define SIN2_3  8.3333333333e-3f
#define SIN2_4  -1.9841269841e-4f
#define SIN2_5  2.7557319224e-6f
#define SIN2_6  -2.5052108385e-8f
#define SIN2_7  1.60590438368e-10f

float sine2 (float x)

{ /* sine2() */

float x_2;

/***************************************************************************/

	/* Make sure the input is between [pi, pi] */
	if ( x > SR_PI)
    	{
        x = SR_PI;
        }
        
	if (x < -SR_PI)
    	{
        x = -SR_PI;
        }        


	if(x >= 0 && x < SR_PI2)
		{
	    x_2=x*x;

		return( (((((((SIN2_7*x_2+SIN2_6)*x_2+SIN2_5)*x_2
						+SIN2_4)*x_2+SIN2_3)*x_2+SIN2_2)*x_2+SIN2_1)*x) );
		}

	if(x >= SR_PI2 && x <= SR_PI)
		{
	    x = SR_PI-x;
		x_2=x*x;
		return( (((((((SIN2_7*x_2+SIN2_6)*x_2+SIN2_5)*x_2
						+SIN2_4)*x_2+SIN2_3)*x_2+SIN2_2)*x_2+SIN2_1)*x) );
		}

	if(x < 0 && x >= -SR_PI2)
		{
	    x = -x;
		x_2=x*x;
		return( -(((((((SIN2_7*x_2+SIN2_6)*x_2+SIN2_5)*x_2
						+SIN2_4)*x_2+SIN2_3)*x_2+SIN2_2)*x_2+SIN2_1)*x) );
		}

		/* Else, (x < -SR_PI2 && x >= -SR_PI) */
	    x = SR_PI+x;
		x_2=x*x;
	    return( -(((((((SIN2_7*x_2+SIN2_6)*x_2+SIN2_5)*x_2
				   		+SIN2_4)*x_2+SIN2_3)*x_2+SIN2_2)*x_2+SIN2_1)*x) );
	
} /* sine2() */

/***** END of sine2() *********************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: srmath_get_closest_point()                                      */
/*                                                                           */
/* This function finds the closest point in a 2-D plane from a               */
/* known point R to a line (define as a point P0 and directional vector V).  */
/*                                                                           */
/* The line is defines as P(t) = P0 + t*V.                                   */
/*              P0 = {x0,z0}                                                 */
/*               V = {vx,vy,vz}                                              */
/*                                                                           */
/* The point is define as R = {rx,rz}                                        */
/*                                                                           */
/* This function assumes that the projected 2-D plane is in the x-z plane,   */
/* so the y component is ignored (vy).                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 13 May 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void  srmath_get_closest_point(float *v, float x0, float z0, float rx, float rz, 
                               float *x1, float *z1)


{ /* srmath_get_closest_point() */

float t;
float vect_p0[3];
float vect_r[3];
float vect_v[3];
float tmp[3];
float temp1;
float temp2;


/*****************************************************************************/

	/* create vector P0 from points x0,z0 */
	vect_p0[0] = x0;
	vect_p0[1] = 0.0f;
    vect_p0[2] = z0;

	/* create vector R from points rx,rz */
	vect_r[0] = rx;
	vect_r[1] = 0.0f;
    vect_r[2] = rz;

	/* create vector V from 3D vector v */
    /* This uses the projection about the Y axis. */
	vect_v[0] = v[0]; /* original x */
	vect_v[1] = 0.0f;
    vect_v[2] = v[2]; /* original z */
  
	/* t = dot((R-P0),V)/dot(V,V) */

	vect_subt (tmp,vect_r,vect_p0);
    
    temp1 = vect_dot (tmp,vect_v);   	/* temp1 = dot((R-P0),V)  */
    temp2 = vect_dot (vect_v,vect_v);	/* temp2 = dot(V,V)       */
    
    t = temp1/temp2;
    
    /* Pclosest = P0 + t*V */
    
    *x1 = vect_p0[0] + t*vect_v[0];
    *z1 = vect_p0[2] + t*vect_v[2];
    

} /* srmath_get_closest_point() */

/***** END of srmath_get_closest_point() *************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_angle_v1v2()                                                */
/*                                                                           */
/* Given 2 vectors with one point in common with each other, find the        */
/* angle between them.                                                       */
/*                                                                           */
/* angle = arccos(dot(A,B)/(|A||B|))                                         */
/* output: returns an angle in radians between 0.0f to PI                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 18 Sep 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float get_angle_v1v2 (float *v1, float *v2)

{ /* get_angle_v1v2() */

float tmp;
float mag_v1;
float mag_v2;
float angle;
float input;

/*****************************************************************************/

	tmp = vect_dot ( v1, v2);	/* dot(A,B) */
	mag_v1 = vect_mag (v1);  	/* |A| */
	mag_v2 = vect_mag (v2);  	/* |B| */
 
 	input = tmp / (mag_v1 * mag_v2);
	angle = arccos (input);   /* arccos(dot(A,B)/(|A||B|)) */

	return(angle);    

} /* get_angle_v1v2() */

/***** END of get_angle_v1v2() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_angle_zx()                                                  */
/*                                                                           */
/* Given a 3D directional vector, return the orthographic projection angle   */
/* about the y axis, ANGLE xz.                                               */
/*                                                                           */
/* angle = arccos(dot(A,B)/(|A||B|))                                         */
/*                                                                           */
/* Remember: If axis of rotation is y, positive rotation is z to x.          */
/*                                                                           */
/* Input is an array of floats, XYZ.                                         */
/*                                                                           */
/* Returns an angle in radians in the range -PI to +PI.                      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 09 Apr 97 EJK                                                             */
/* 07 May 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float get_angle_zx (float *v)

{ /* get_angle_zx() */

float vect_x[3] = {1.0f, 0.0f, 0.0f};
float vect_in[3];
float tmp;
float x;
float in;
float angle;

/*****************************************************************************/

	vect_in[0] = v[0];		/* project input vector along the y axis */
	vect_in[1] = 0.0f;	
	vect_in[2] = v[2];	

	tmp = vect_dot ( vect_in, vect_x);	/* dot(A,B) */
	x = vect_mag (vect_x);			  	/* |A| */
	in = vect_mag (vect_in);		  	/* |B| */

	angle = arccos (tmp / (x * in));   /* arccos(dot(A,B)/(|A||B|)) */

	/* if pointing in -Z direction, correct */	 
	if(vect_in[2] < 0.0f)			  /* correct for direction */
		angle *= -1.0f;
	   
	return(angle);    

} /* get_angle_zx() */

/***** END of get_angle_zx() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_angle_xy()                                                  */
/*                                                                           */
/* Given a 3D directional vector, return the orthographic projection angle   */
/* about the z axis, ANGLE xy.                                               */
/*                                                                           */
/* angle = arccos(dot(A,B)/(|A||B|))                                         */
/*                                                                           */
/* Remember: If axis of rotation is z, positive rotation is x to y.          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 09 Apr 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float get_angle_xy (float *v)

{ /* get_angle_xy() */

float vect_x[3] = {1.0f,0.0f,0.0f};
float vect_in[3];
float tmp;
float x;
float in;
float angle;

/*****************************************************************************/

	vect_in[0] = v[0];		/* project input vector along the z axis */
	vect_in[1] = v[1];	
	vect_in[2] = 0.0f;	

	tmp = vect_dot (vect_in,vect_x);  /* dot(A,B) */
	x = vect_mag (vect_x);			  /* |A| */
	in = vect_mag (vect_in);		  /* |B| */

	angle = arccos(tmp/(x*in));     /* arccos(dot(A,B)/(|A||B|)) */

	if(vect_in[1] < 0.0f)			  /* correct for direction */
		angle *= -1.0f;

	return(angle);
     
} /* get_angle_xy() */

/***** END of get_angle_xy() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_angle_yz()                                                  */
/*                                                                           */
/* Given a 3D directional vector, return the orthographic projection angle   */
/* about the x axis, ANGLE yz.                                               */
/*                                                                           */
/* angle = arccos(dot(A,B)/(|A||B|))                                         */
/*                                                                           */
/* Remember: If axis of rotation is x, positive rotation is y to z.          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 09 Apr 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float get_angle_yz (float *v)

{ /* get_angle_yz() */

float vect_y[3] = {0.0f,1.0f,0.0f};
float vect_in[3];
float tmp;
float y;
float in;
float angle;

/*****************************************************************************/

	vect_in[0] = 0.0f;		/* project input vector along the x axis */
	vect_in[1] = v[1];	
	vect_in[2] = v[2];	

	tmp = vect_dot (vect_in,vect_y);  /* dot(A,B) */
	y = vect_mag (vect_y);			  /* |A| */
	in = vect_mag (vect_in);		  /* |B| */

	angle = arccos(tmp/(y*in));     /* arccos(dot(A,B)/(|A||B|)) */

	if(vect_in[2] < 0.0f)			  /* correct for direction */
		angle *= -1.0f;
	   
	return(angle);    

} /* get_angle_yz() */

/***** END of get_angle_yz() *************************************************/

/***** MATRIX FUNCTIONS ******************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_determinant_4x4()                                           */
/*                                                                           */
/* Finds the determinant of a 4x4 maxtix.                                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 May 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float get_determinant_4x4 (float *m)

{ /* get_determinant_4x4() */
float A;
float B;
float C;
float D;
float E;
float F;
float result;
/*****************************************************************************/

	/*           | m0  m1  m2  m3  |      */
	/*       m = | m4  m5  m6  m7  |      */
	/*           | m8  m9  m10 m11 |      */
	/*           | m12 m13 m14 m15 |      */


	/* det(m) =  m0*[m5(m10*m15-m14*m11) -m6*(m9*m15-m13*m11) +m7*(m9*m14-m13*m10)] */
	/*          -m1*[m4(m10*m15-m14*m11) -m6*(m8*m15-m12*m11) +m7*(m8*m14-m12*m10)] */
	/*           m2*[m4(m9*m15-m13*m11)  -m5*(m8*m15-m12*m11) +m7*(m8*m13-m12*m9) ] */
	/*          -m3*[m4(m9*m14-m13*m10)  -m5*(m8*m14-m12*m10) +m6*(m8*m13-m12*m9) ] */

	/* let: A = m10*m15 - m14*m11 */
	/*      B = m8*m15  - m12*m11 */
	/*      C = m8*m13  - m12*m9  */
	/*      D = m8*m14  - m12*m10 */
	/*      E = m9*m15  - m13*m11 */
	/*      F = m9*m14  - m13*m10 */

	/* det(m) =  m0*[ m5*A - m6*E + m7*F ] */
	/*          -m1*[ m4*A - m6*B + m7*D ] */
	/*           m2*[ m4*E - m5*B + m7*C ] */
	/*          -m3*[ m4*F - m5*D + m6*C ] */


	A = m[10]*m[15] - m[14]*m[11];
	B = m[8]*m[15]  - m[12]*m[11];
	C = m[8]*m[13]  - m[12]*m[9]; 
	D = m[8]*m[14]  - m[12]*m[10];
	E = m[9]*m[15]  - m[13]*m[11];
	F = m[9]*m[14]  - m[13]*m[10];

    result =   m[0]*( m[5]*A - m[6]*E + m[7]*F )
             - m[1]*( m[4]*A - m[6]*B + m[7]*D )
             + m[2]*( m[4]*E - m[5]*B + m[7]*C )
             - m[3]*( m[4]*F - m[5]*D + m[6]*C );

	return(result);
        
} /* get_determinant_4x4() */

/***** END of get_determinant_4x4() ******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_determinant_3x3()                                           */
/*                                                                           */
/* Finds the determinant of a 3x3 maxtix.                                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 May 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float get_determinant_3x3 (float *m)

{ /* get_determinant_3x3() */
float result;

	/*           | m0 m1 m2 |      */
	/*       m = | m3 m4 m5 |      */
	/*           | m6 m7 m8 |      */

	/* det(m) = m0*(m4*m8-m7*m5) -m1*(m3*m8-m6*m5) +m2*(m3*m7-m6*m4) */

	result =  m[0]*(m[4]*m[8]-m[7]*m[5]) 
			+ m[1]*(m[6]*m[5]-m[3]*m[8]) 
        	+ m[2]*(m[3]*m[7]-m[6]*m[4]);

	return(result);
        
} /* get_determinant_3x3() */

/***** END of get_determinant_3x3() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_adjoint4x4()                                                */
/*                                                                           */
/* This function finds the adjoint of a matrix.                              */
/*                                                                           */
/* The adjoint matrix(M*) is similar to the inverse, except for a scaling    */
/* factor.  In particular, M^-1 = (1/det(M)) M*, where the det is the        */
/* determinant of the original matrix. If the matrix is singular, the        */
/* det = 0, so there is no inverse matrix. However, computing the adjoint    */
/* does not require the determinant.  The two main advantages of the adjoint */
/* are that it always exits, and that it is easier and faster to calculate   */
/* than the inverse.                                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void get_adjoint4x4(float *output,float *input)

{ /* get_adjoint4x4() */

int i;
        
	for (i=0;i<16;i++)
    	{
        output[i] = adj_sub_det3x3(input, &adj_index_data[9*i]);
        }                
       
} /* get_adjoint4x4() */

/***** END of get_adjoint4x4() ***********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_adjoint3x3()                                                */
/*                                                                           */
/* This function finds the adjoint of a matrix.                              */
/*                                                                           */
/* The adjoint matrix(M*) is similar to the inverse, except for a scaling    */
/* factor.  In particular, M^-1 = (1/det(M)) M*, where the det is the        */
/* determinant of the original matrix. If the matrix is singular, the        */
/* det = 0, so there is no inverse matrix. However, computing the adjoint    */
/* does not require the determinant.  The two main advantages of the adjoint */
/* are that it always exits, and that it is easier and faster to calculate   */
/* than the inverse.                                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void get_adjoint3x3(float *output,float *input)

{ /* get_adjoint3x3() */

	output[0] = (input[4]*input[8]-input[7]*input[5]);
	output[1] = (input[7]*input[2]-input[1]*input[8]);
	output[2] = (input[1]*input[5]-input[2]*input[4]);
	output[3] = (input[6]*input[5]-input[3]*input[8]);
	output[4] = (input[0]*input[8]-input[6]*input[2]);
	output[5] = (input[3]*input[2]-input[0]*input[5]);
	output[6] = (input[3]*input[7]-input[6]*input[4]);
	output[7] = (input[1]*input[6]-input[0]*input[7]);
	output[8] = (input[0]*input[4]-input[3]*input[1]);
       
} /* get_adjoint3x3() */

/***** END of get_adjoint3x3() ***********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: adj_sub_det3x3()                                                */
/*                                                                           */
/* Finds the determinant of a 3x3 maxtix.                                    */
/* Given an 4x4 matrix m, and an array a[9] of indices defining the          */
/* 3x3 matrix.                                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float adj_sub_det3x3 (float *m,int *a)

{ /* adj_sub_det3x3() */
float result;

	/*           | m0 m1 m2 |      */
	/*       m = | m3 m4 m5 |      */
	/*           | m6 m7 m8 |      */

	/* det(m) = m0*(m4*m8-m7*m5) -m1*(m3*m8-m6*m5) +m2*(m3*m7-m6*m4) */
    
	result =   m[a[0]]*(m[a[4]]*m[a[8]]-m[a[7]]*m[a[5]]) 
    		 + m[a[1]]*(m[a[6]]*m[a[5]]-m[a[3]]*m[a[8]]) 
             + m[a[2]]*(m[a[3]]*m[a[7]]-m[a[6]]*m[a[4]]);

	return(result);
        
} /* adj_sub_det3x3() */

/***** END of adj_sub_det3x3() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_inv_matrix3x3()                                             */
/*                                                                           */
/* Finds the inverse of a 3x3 matrix.  If there is a valid inverse           */
/* this function returns a TRUE, If the matrix is singular(no inverse) it    */
/* returns a FALSE.                                                          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

int get_inv_matrix3x3(float *output,float *input)

{ /* get_inv_matrix3x3() */

float scale;
float det;

	det = get_determinant_3x3 (input);
    
    if(det == 0.0f)
    	{
        return(FALSE);
        }
	else
    	{        
		get_adjoint3x3(output,input);
        scale = 1.0f/det;
		matrix3x3_mpy (output,scale);
        return(TRUE);
        }
        

} /* get_inv_matrix3x3() */

/***** END of get_inv_matrix3x3() ********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_inv_matrix4x4()                                             */
/*                                                                           */
/* Finds the inverse of a 4x4 matrix.  If there is a valid inverse           */
/* this function returns a TRUE, If the matrix is singular(no inverse) it    */
/* returns a FALSE.                                                          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

int get_inv_matrix4x4(float *output,float *input)

{ /* get_inv_matrix4x4() */

float scale;
float det;

	det = get_determinant_4x4 (input);
    if(det == 0.0f)
    	{
        return(FALSE);
        }
	else
    	{        
		get_adjoint4x4(output,input);
        scale = 1.0f/det;
		matrix_mpy (output,scale);
        return(TRUE);
        }

} /* get_inv_matrix4x4() */

/***** END of get_inv_matrix4x4() ********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_orthogonal_matrix()                                         */
/*                                                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void get_orthogonal_matrix (float *out, float *r)

{ /* get_orthogonal_matrix() */

#define ORTHO_1 -0.5f
#define ORTHO_2 +0.375f
#define ORTHO_3 -0.3125f
#define ORTHO_4 +0.2734375f
#define ORTHO_5 -0.24609375f
#define ORTHO_6 +0.2255859375f


float x[16];
float tmp1[16];
float tmp2[16];
float sum[16];

/*****************************************************************************/

	/* find the transpose: tmp1 = R^T */
	matrix_transpose (tmp1, r);
	
    /* find X = R^T*R */
    matrix_44x44 (x, tmp1, r);
    
    /* find X = R^T*R - I */
	matrix_subt2 (x, sr_identity_matrix);    
    
    /* sum = I */
    matrix_equate (sum, sr_identity_matrix);

	/* tmp = O1*x */    
    matrix_mpy2 ( tmp1, x, ORTHO_1 );
    
    /* sum = I + O1*x */
    matrix_add2 ( sum, tmp1 );
    
	/* tmp1 = x^2 */    
    matrix_44x44 (tmp1, x, x);

   	/* tmp2 = O2*x^2 */    
    matrix_mpy2 ( tmp2, tmp1, ORTHO_2 );
 
    /* sum = I + O1*x + O2*x^2 */
    matrix_add2 ( sum, tmp2 );

	/* tmp2 = x^3 */    
    matrix_44x44 (tmp2, tmp1, x);

   	/* tmp1 = O3*x^3 */    
    matrix_mpy2 ( tmp1, tmp2, ORTHO_3 );
     
    /* sum = I + O1*x + O2*x^2 + O3*x^3  */
    matrix_add2 ( sum, tmp1 );

    /* apply the correction matrix */
    matrix_44x44 (out, r, sum);
    

} /* get_orthogonal_matrix() */

/***** END of get_orthogonal_matrix() ****************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_combine_33_into_44()                                     */
/*                                                                           */
/* This functions takes a 3x3 matrix and converts it into a 4x4 matrix.      */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 27 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_combine_33_into_44 (float *result, float *m)

{ /* matrix_combine_33_into_44() */

	result[3]  = 0.0f;
	result[7]  = 0.0f;
	result[11] = 0.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = 0.0f;
	result[15] = 1.0f;

	result[0] = m[0];
	result[1] = m[1];
	result[2] = m[2];

	result[4] = m[3];
	result[5] = m[4];
	result[6] = m[5];

	result[8]  = m[6];
	result[9]  = m[7];
	result[10] = m[8];

} /* matrix_combine_33_into_44() */

/***** END of matrix_combine_33_into_44() ************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_combine_row()                                            */
/*                                                                           */
/* Uses properties of orthogonal matrices, combines three row vectors into   */
/* one 4x4 matrix rotation matrix.                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_combine_row (float *result, float *x, float *y, float *z)

{ /* matrix_combine_row() */

	result[3]  = 0.0f;
	result[7]  = 0.0f;
	result[11] = 0.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = 0.0f;
	result[15] = 1.0f;

	result[0] = x[0];
	result[1] = x[1];
	result[2] = x[2];

	result[4] = y[0];
	result[5] = y[1];
	result[6] = y[2];

	result[8] = z[0];
	result[9] = z[1];
	result[10] = z[2];

} /* matrix_combine_row() */

/***** END of matrix_combine_row() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_combine_row_3x3()                                        */
/*                                                                           */
/* Uses properties of orthogonal matrices, combines three row vectors into   */
/* one 3x3 matrix rotation matrix.                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 May 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_combine_row_3x3 (float *result, float *x, float *y, float *z)

{ /* matrix_combine_row_3x3() */

	result[0] = x[0];
	result[1] = x[1];
	result[2] = x[2];

	result[3] = y[0];
	result[4] = y[1];
	result[5] = y[2];

	result[6] = z[0];
	result[7] = z[1];
	result[8] = z[2];

} /* matrix_combine_row_3x3() */

/***** END of matrix_combine_row_3x3() ****************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_combine_col()                                            */
/*                                                                           */
/* Uses properties of orthogonal matrices, combines three column vectors     */
/* into one 4x4 matrix rotation matrix.                                      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_combine_col (float *result, float *x, float *y, float *z)

{ /* matrix_combine_col() */

	result[3]  = 0.0f;
	result[7]  = 0.0f;
	result[11] = 0.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = 0.0f;
	result[15] = 1.0f;

	result[0] = x[0];
	result[4] = x[1];
	result[8] = x[2];

	result[1] = y[0];
	result[5] = y[1];
	result[9] = y[2];

	result[2] = z[0];
	result[6] = z[1];
	result[10] = z[2];

} /* matrix_combine_col() */

/***** END of matrix_combine_col() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_combine_col_3x3()                                        */
/*                                                                           */
/* Uses properties of orthogonal matrices, combines three column vectors     */
/* into one 3x3 matrix rotation matrix.                                      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 May 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_combine_col_3x3 (float *result, float *x, float *y, float *z)

{ /* matrix_combine_col_3x3() */

	result[0] = x[0];
	result[3] = x[1];
	result[6] = x[2];

	result[1] = y[0];
	result[4] = y[1];
	result[7] = y[2];

	result[2] = z[0];
	result[5] = z[1];
	result[8] = z[2];

} /* matrix_combine_col_3x3() */

/***** END of matrix_combine_col_3x3() ****************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_find_dof()                                               */
/*                                                                           */
/* Transform an object so that it heads in the direction given by the vector */
/* vector DOF(direction of flight) and NOT BANKED.                           */
/*                                                                           */
/* NOTE: The situation if DOF is in the direction of the y axis is           */
/*       degenerate, because there is an infinite set of possible vectors    */
/*       the horizontal vector.  This degeneracy is reflected in the algebra */
/*       because the cross-products (YxDOF) and (DOFx(YxDOF)) are zero.      */
/*       In this special case, R is not a rotation matrix.                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 01 Apr 97 EJK                                                             */
/* 25 Feb 98 EJK clean up                                                    */
/*                                                                           */
/*****************************************************************************/

void matrix_find_dof (float *result,float *vect_dof)

{ /* matrix_find_dof() */
float rx[3];
float ry[3];
float rz[3];
float vect_y[3];

/*****************************************************************************/

	  /* initialize vect_y */
	  vect_y[0] = 0.0f;
	  vect_y[1] = 1.0f;
	  vect_y[2] = 0.0f;
      
	  /* find rx */
	  vect_cross (rx, vect_y, vect_dof);
	  vect_norm (rx);
	  
      /* find ry */
	  vect_cross (ry, vect_dof, rx);
	  vect_norm (ry);
	  
      /* find rz */
	  vect_equate (rz, vect_dof);
	  vect_norm (rz);
      
	  matrix_combine_col (result, rx, ry, rz);
      
} /* matrix_find_dof() */

/***** END of matrix_find_dof() **********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_find_dof_vup()                                           */
/*                                                                           */
/* Transform an object so that it heads in the direction given by the vector */
/* vector DOF(direction of flight) and VUP(up vector). This function is      */
/* similar to matrix_find_dof() except that it takes account for the up      */
/* vector, VUP. The VUP vector translates the "BANK" information.            */
/*                                                                           */
/* This function actually works in the reverse.  Given a positional vector   */
/* find the transformation to align DOF to Z axis and VUP to the Y axis.     */
/* This will give you the rotation matrix to do the transformation.  But you */
/* actually want the reverse, given a local object, transform to the DOF and */
/* VUP.  Since a rotation matrix's inverse is its transpose, just take the   */
/* transpose of the matrix to find the desired transformation matrix.        */
/*                                                                           */
/* Lost confused ???  See FOLEY, VAN DAM                                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 11 Apr 97 EJK                                                             */
/* 25 Feb 98 EJK speed up                                                    */
/*                                                                           */
/*****************************************************************************/

void matrix_find_dof_vup (float *result, float *vect_dof, float *vect_vup)

{ /* matrix_find_dof_vup() */

float rx[3];
float ry[3];

/*****************************************************************************/
	 
      /* rz = vect_dof */
	  vect_norm (vect_dof);
      
      /* find rx */										    
	  vect_cross (rx, vect_vup, vect_dof);
	  vect_norm (rx);
      
      /* find ry */
	  /* vect_cross(ry,rz,rx); */
	  vect_cross (ry, vect_dof, rx);
      vect_norm(ry);
      
	  /* matrix_combine_col (tmp,rx,ry,rz); */
	  matrix_combine_col (result, rx, ry, vect_dof);
      
} /* matrix_find_dof_vup() */

/***** END of matrix_find_dof_vup() *****************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_33x31()                                                  */
/*                                                                           */
/* Multiplies a 3x3 matrix times a 3x1 vector.                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_33x31 (float *result, float *m, float *v)

{ /* matrix_33x31() */

	result[0] = m[0]*v[0] +m[1]*v[1] +m[2]*v[2];
	result[1] = m[3]*v[0] +m[4]*v[1] +m[5]*v[2];
	result[2] = m[6]*v[0] +m[7]*v[1] +m[8]*v[2];

} /* matrix_33x31() */

/***** END of matrix_33x31() *************************************************/

		
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41()                                                  */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector.                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41 (float *result, float *m, float *v)

{ /* matrix_44x41() */

	result[0] = m[0]*v[0] +m[1]*v[1] +m[2]*v[2] +m[3]*v[3];
	result[1] = m[4]*v[0] +m[5]*v[1] +m[6]*v[2] +m[7]*v[3];
	result[2] = m[8]*v[0] +m[9]*v[1] +m[10]*v[2]+m[11]*v[3];
	result[3] = m[12]*v[0]+m[13]*v[1]+m[14]*v[2]+m[15]*v[3];

} /* matrix_44x41() */

/***** END of matrix_44x41() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_w()                                                */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, with the assumption that w    */
/* is 1. This saves four multiplies.                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_w (float *result, float *m, float *v)

{ /* matrix_44x41_w() */

	result[0] = m[0]*v[0] +m[1]*v[1] +m[2]*v[2] +m[3];
	result[1] = m[4]*v[0] +m[5]*v[1] +m[6]*v[2] +m[7];
	result[2] = m[8]*v[0] +m[9]*v[1] +m[10]*v[2]+m[11];
	result[3] = m[12]*v[0]+m[13]*v[1]+m[14]*v[2]+m[15];

} /* matrix_44x41_w() */

/***** END of matrix_44x41_w() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_wxyz()                                             */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, with the assumption that w    */
/* is 1. This saves four multiplies. And only returns x,y,z components.      */
/* This saves additional 3 Mults and 3 Adds.                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 28 Jul 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_wxyz (float *result, float *m, float *v)

{ /* matrix_44x41_wxyz() */

	result[0] = m[0]*v[0] +m[1]*v[1] +m[2]*v[2] +m[3];
	result[1] = m[4]*v[0] +m[5]*v[1] +m[6]*v[2] +m[7];
	result[2] = m[8]*v[0] +m[9]*v[1] +m[10]*v[2]+m[11];

} /* matrix_44x41_wxyz() */

/***** END of matrix_44x41_wxyz() ********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_zero()                                             */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, which is a zero vector.       */
/* vector = [0,0,0,0].                                                       */
/* This saves additional 16 Mults and 12 Adds over 44x41.                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 Dec 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_zero (float *result, float *m)

{ /* matrix_44x41_zero() */

	result[0] = m[3];
	result[1] = m[7];
	result[2] = m[11];

} /* matrix_44x41_zero() */

/***** END of matrix_44x41_zero() ********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_unit_x()                                           */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, which is a unit x vector.     */
/* vector = [1,0,0,0].                                                       */
/* This saves additional 16 Mults and 8 Adds over 44x41.                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 Dec 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_unit_x (float *result, float *m)

{ /* matrix_44x41_unit_x() */

	result[0] = m[0] + m[3];
	result[1] = m[4] + m[7];
	result[2] = m[8] + m[11];

} /* matrix_44x41_unit_x() */

/***** END of matrix_44x41_unit_x() ******************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_unit_y()                                           */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, which is a unit y vector.     */
/* vector = [0,1,0,0].                                                       */
/* This saves additional 16 Mults and 8 Adds over 44x41.                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 Dec 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_unit_y (float *result, float *m)

{ /* matrix_44x41_unit_y() */

	result[0] = m[1] + m[3];
	result[1] = m[5] + m[7];
	result[2] = m[9] + m[11];

} /* matrix_44x41_unit_y() */

/***** END of matrix_44x41_unit_y() ******************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_unit_z()                                           */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, which is a unit z vector.     */
/* vector = [0,0,1,0].                                                       */
/* This saves additional 16 Mults and 8 Adds over 44x41.                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 02 Dec 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_unit_z (float *result, float *m)

{ /* matrix_44x41_unit_z() */

	result[0] = m[2] + m[3];
	result[1] = m[6] + m[7];
	result[2] = m[10] + m[11];

} /* matrix_44x41_unit_z() */

/***** END of matrix_44x41_unit_z() ******************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x41_wf()                                               */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x1 vector, with the assumption that w    */
/* is 1. This is for final screen coordinates, so the Z term is eliminated,  */
/* since the Z info is now implied in W.                                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x41_wf (float *result, float *m, float *v)

{ /* matrix_44x41_wf() */

	result[0] = m[0]*v[0] +m[1]*v[1] +m[2]*v[2] +m[3];
	result[1] = m[4]*v[0] +m[5]*v[1] +m[6]*v[2] +m[7];
	result[3] = m[12]*v[0]+m[13]*v[1]+m[14]*v[2]+m[15];

} /* matrix_44x41_wf() */

/***** END of matrix_44x41_wf() **********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_33x33()                                                  */
/*                                                                           */
/* Multiplies a 3x3 matrix times a 3x3 matrix.                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_33x33 (float *result, float *m, float *n)

{ /* matrix_33x33() */

	result[0] = m[0]*n[0] + m[1]*n[3] + m[2]*n[6];
	result[1] = m[0]*n[1] + m[1]*n[4] + m[2]*n[7];
	result[2] = m[0]*n[2] + m[1]*n[5] + m[2]*n[8];
    
	result[3] = m[3]*n[0] + m[4]*n[3] + m[5]*n[6];
	result[4] = m[3]*n[1] + m[4]*n[4] + m[5]*n[7];
	result[5] = m[3]*n[2] + m[4]*n[5] + m[5]*n[8];
    
	result[6] = m[6]*n[0] + m[7]*n[3] + m[8]*n[6];
	result[7] = m[6]*n[1] + m[7]*n[4] + m[8]*n[7];
	result[8] = m[6]*n[2] + m[7]*n[5] + m[8]*n[8];

} /* matrix_33x33() */

/***** END of matrix_33x33() *************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x44()                                                  */
/*                                                                           */
/* Multiplies a 4x4 matrix times a 4x4 matrix.                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x44 (float *result, float *m, float *n)

{ /* matrix_44x44() */

	result[ 0] = m[ 0]*n[ 0] + m[ 1]*n[ 4] + m[ 2]*n[ 8] + m[ 3]*n[12];
	result[ 1] = m[ 0]*n[ 1] + m[ 1]*n[ 5] + m[ 2]*n[ 9] + m[ 3]*n[13];
	result[ 2] = m[ 0]*n[ 2] + m[ 1]*n[ 6] + m[ 2]*n[10] + m[ 3]*n[14];
	result[ 3] = m[ 0]*n[ 3] + m[ 1]*n[ 7] + m[ 2]*n[11] + m[ 3]*n[15];

	result[ 4] = m[ 4]*n[ 0] + m[ 5]*n[ 4] + m[ 6]*n[ 8] + m[ 7]*n[12];
	result[ 5] = m[ 4]*n[ 1] + m[ 5]*n[ 5] + m[ 6]*n[ 9] + m[ 7]*n[13];
	result[ 6] = m[ 4]*n[ 2] + m[ 5]*n[ 6] + m[ 6]*n[10] + m[ 7]*n[14];
	result[ 7] = m[ 4]*n[ 3] + m[ 5]*n[ 7] + m[ 6]*n[11] + m[ 7]*n[15];

	result[ 8] = m[ 8]*n[ 0] + m[ 9]*n[ 4] + m[10]*n[ 8] + m[11]*n[12];
	result[ 9] = m[ 8]*n[ 1] + m[ 9]*n[ 5] + m[10]*n[ 9] + m[11]*n[13];
	result[10] = m[ 8]*n[ 2] + m[ 9]*n[ 6] + m[10]*n[10] + m[11]*n[14];
	result[11] = m[ 8]*n[ 3] + m[ 9]*n[ 7] + m[10]*n[11] + m[11]*n[15];

	result[12] = m[12]*n[ 0] + m[13]*n[ 4] + m[14]*n[ 8] + m[15]*n[12];
	result[13] = m[12]*n[ 1] + m[13]*n[ 5] + m[14]*n[ 9] + m[15]*n[13];
	result[14] = m[12]*n[ 2] + m[13]*n[ 6] + m[14]*n[10] + m[15]*n[14];
	result[15] = m[12]*n[ 3] + m[13]*n[ 7] + m[14]*n[11] + m[15]*n[15];

} /* matrix_44x44() */

/***** END of matrix_44x44() *************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_44x44_transpose()                                        */
/*                                                                           */
/* Multiplies a 4x4 matrix times a the transpose of 4x4 matrix.              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_44x44_transpose (float *result, float *m, float *n)

{ /* matrix_44x44_transpose() */

	result[ 0] = m[ 0]*n[15] + m[ 1]*n[11] + m[ 2]*n[ 7] + m[ 3]*n[ 3];
	result[ 1] = m[ 0]*n[14] + m[ 1]*n[10] + m[ 2]*n[ 6] + m[ 3]*n[ 2];
	result[ 2] = m[ 0]*n[13] + m[ 1]*n[ 9] + m[ 2]*n[ 5] + m[ 3]*n[ 1];
	result[ 3] = m[ 0]*n[12] + m[ 1]*n[ 8] + m[ 2]*n[ 4] + m[ 3]*n[ 0];

	result[ 4] = m[ 4]*n[15] + m[ 5]*n[11] + m[ 6]*n[ 7] + m[ 7]*n[ 3];
	result[ 5] = m[ 4]*n[14] + m[ 5]*n[10] + m[ 6]*n[ 6] + m[ 7]*n[ 2];
	result[ 6] = m[ 4]*n[13] + m[ 5]*n[ 9] + m[ 6]*n[ 5] + m[ 7]*n[ 1];
	result[ 7] = m[ 4]*n[12] + m[ 5]*n[ 8] + m[ 6]*n[ 4] + m[ 7]*n[ 0];

	result[ 8] = m[ 8]*n[15] + m[ 9]*n[11] + m[10]*n[ 7] + m[11]*n[ 3];
	result[ 9] = m[ 8]*n[14] + m[ 9]*n[10] + m[10]*n[ 6] + m[11]*n[ 2];
	result[10] = m[ 8]*n[13] + m[ 9]*n[ 9] + m[10]*n[ 5] + m[11]*n[ 1];
	result[11] = m[ 8]*n[12] + m[ 9]*n[ 8] + m[10]*n[ 4] + m[11]*n[ 0];

	result[12] = m[12]*n[15] + m[13]*n[11] + m[14]*n[ 7] + m[15]*n[ 3];
	result[13] = m[12]*n[14] + m[13]*n[10] + m[14]*n[ 6] + m[15]*n[ 2];
	result[14] = m[12]*n[13] + m[13]*n[ 9] + m[14]*n[ 5] + m[15]*n[ 1];
	result[15] = m[12]*n[12] + m[13]*n[ 8] + m[14]*n[ 4] + m[15]*n[ 0];

} /* matrix_44x44_transpose() */

/***** END of matrix_44x44_transpose() ***************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_equate()                                                 */
/*                                                                           */
/* Copies the source matrix N into the destination matrix M.                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_equate (float *m, float *n)

{ /* matrix_equate() */

    m[0]  = n[0];
    m[1]  = n[1];
    m[2]  = n[2];
    m[3]  = n[3];
    m[4]  = n[4];
    m[5]  = n[5];
    m[6]  = n[6];
    m[7]  = n[7];
    m[8]  = n[8];
    m[9]  = n[9];
    m[10] = n[10];
    m[11] = n[11];
    m[12] = n[12];
    m[13] = n[13];
    m[14] = n[14];
    m[15] = n[15];

} /* matrix_equate() */

/***** END of matrix_equate() ************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_subt()                                                   */
/*                                                                           */
/* The functions take matrix M and subtracts matrix N from it element by     */
/* element and stores the result in matrix OUT.                              */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_subt (float *out, float *m, float *n)

{ /* matrix_subt() */

    out[0]  = m[0]  - n[0];
    out[1]  = m[1]  - n[1];
    out[2]  = m[2]  - n[2];
    out[3]  = m[3]  - n[3];
    out[4]  = m[4]  - n[4];
    out[5]  = m[5]  - n[5];
    out[6]  = m[6]  - n[6];
    out[7]  = m[7]  - n[7];
    out[8]  = m[8]  - n[8];
    out[9]  = m[9]  - n[9];
    out[10] = m[10] - n[10];
    out[11] = m[11] - n[11];
    out[12] = m[12] - n[12];
    out[13] = m[13] - n[13];
    out[14] = m[14] - n[14];
    out[15] = m[15] - n[15];

} /* matrix_subt() */

/***** END of matrix_subt() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_subt2()                                                  */
/*                                                                           */
/* The functions take matrix M and subtracts matrix N from it element by     */
/* element and stores the result in matrix M.                                */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_subt2 (float *m, float *n)

{ /* matrix_subt2() */

    m[0]  -= n[0];
    m[1]  -= n[1];
    m[2]  -= n[2];
    m[3]  -= n[3];
    m[4]  -= n[4];
    m[5]  -= n[5];
    m[6]  -= n[6];
    m[7]  -= n[7];
    m[8]  -= n[8];
    m[9]  -= n[9];
    m[10] -= n[10];
    m[11] -= n[11];
    m[12] -= n[12];
    m[13] -= n[13];
    m[14] -= n[14];
    m[15] -= n[15];

} /* matrix_subt2() */

/***** END of matrix_subt2() *************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_add()                                                    */
/*                                                                           */
/* The functions take matrix M and adds matrix N from it element by          */
/* element and stores the result in matrix OUT.                              */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_add (float *out, float *m, float *n)

{ /* matrix_add() */

    out[0]  = m[0]  + n[0];
    out[1]  = m[1]  + n[1];
    out[2]  = m[2]  + n[2];
    out[3]  = m[3]  + n[3];
    out[4]  = m[4]  + n[4];
    out[5]  = m[5]  + n[5];
    out[6]  = m[6]  + n[6];
    out[7]  = m[7]  + n[7];
    out[8]  = m[8]  + n[8];
    out[9]  = m[9]  + n[9];
    out[10] = m[10] + n[10];
    out[11] = m[11] + n[11];
    out[12] = m[12] + n[12];
    out[13] = m[13] + n[13];
    out[14] = m[14] + n[14];
    out[15] = m[15] + n[15];

} /* matrix_add() */

/***** END of matrix_add() ***************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_add2()                                                   */
/*                                                                           */
/* The functions take matrix M and adds matrix N from it element by          */
/* element and stores the result in matrix M.                                */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_add2 ( float *m, float *n)

{ /* matrix_add2() */

    m[0]  += n[0];
    m[1]  += n[1];
    m[2]  += n[2];
    m[3]  += n[3];
    m[4]  += n[4];
    m[5]  += n[5];
    m[6]  += n[6];
    m[7]  += n[7];
    m[8]  += n[8];
    m[9]  += n[9];
    m[10] += n[10];
    m[11] += n[11];
    m[12] += n[12];
    m[13] += n[13];
    m[14] += n[14];
    m[15] += n[15];

} /* matrix_add2() */

/***** END of matrix_add2() **************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_transpose()                                              */
/*                                                                           */
/* Takes the transpose of matrix N into matrix M.                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_transpose(float *m, float *n)

{ /* matrix_transpose() */

    m[0]  = n[0];
    m[1]  = n[4];
    m[2]  = n[8];
    m[3]  = n[12];
    m[4]  = n[1];
    m[5]  = n[5];
    m[6]  = n[9];
    m[7]  = n[13];
    m[8]  = n[2];
    m[9]  = n[6];
    m[10] = n[10];
    m[11] = n[14];
    m[12] = n[3];
    m[13] = n[7];
    m[14] = n[11];
    m[15] = n[15];

} /* matrix_transpose() */

/***** END of matrix_transpose() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_trans()                                                  */
/*                                                                           */
/* Translate the source matrix by X, Y and Z.                                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK - Speed up                                                  */
/*                                                                           */
/*****************************************************************************/

void matrix_trans (float *result, float *m, float x, float y, float z)

{ /* matrix_trans() */

    /* Speed improvement, matrix_44x44(result,trans,m) is done      */
    /* in place. Removing the function call overhead, and reducing  */
    /* the number of mults by 52, and num adds by 36 */

	result[0] = m[0] + x * m[12];
	result[1] = m[1] + x * m[13];
	result[2] = m[2] + x * m[14];
	result[3] = m[3] + x * m[15];

	result[4] = m[4] + y * m[12];
	result[5] = m[5] + y * m[13];
	result[6] = m[6] + y * m[14];
	result[7] = m[7] + y * m[15];
 
 	result[8]  = m[8]  + z * m[12];
	result[9]  = m[9]  + z * m[13];
	result[10] = m[10] + z * m[14];
	result[11] = m[11] + z * m[15];
     
	result[12] = m[12];
	result[13] = m[13];
	result[14] = m[14];
	result[15] = m[15];
    
} /* matrix_trans() */

/***** END of matrix_trans() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_scale()                                                  */
/*                                                                           */
/* Scales source matrix in X, Y and Z.                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK - Speed up                                                  */
/*                                                                           */
/*****************************************************************************/

void matrix_scale (float *result, float *m, float x, float y, float z)

{ /* matrix_scale() */

    /* Speed improvement, matrix_44x44(result,scale,m) is done     */
    /* in place. Removing the function call overhead, and reducing */
    /* the number of mults by 52, and num adds by 48 */

	result[0] = x * m[0];
	result[1] = x * m[1];
	result[2] = x * m[2];
	result[3] = x * m[3];

	result[4] = y * m[4];
	result[5] = y * m[5];
	result[6] = y * m[6];
	result[7] = y * m[7];
 
 	result[8]  = z * m[8] ;
	result[9]  = z * m[9] ;
	result[10] = z * m[10];
	result[11] = z * m[11];
     
	result[12] = m[12];
	result[13] = m[13];
	result[14] = m[14];
	result[15] = m[15];
    
} /* matrix_scale() */

/***** END of matrix_scale() *************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_mpy()                                                    */
/*                                                                           */
/* Scales the entire matrix by a scaler factor.                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_mpy (float *result, float factor)

{ /* matrix_mpy() */

	result[0] *= factor;
	result[1] *= factor;
	result[2] *= factor;
	result[3] *= factor;
	result[4] *= factor;
	result[5] *= factor;
	result[6] *= factor;
	result[7] *= factor;
	result[8] *= factor;
	result[9] *= factor;
	result[10] *= factor;
	result[11] *= factor;
	result[12] *= factor;
	result[13] *= factor;
	result[14] *= factor;
	result[15] *= factor;

} /* matrix_mpy() */

/***** END of matrix_mpy() ***************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_mpy2()                                                   */
/*                                                                           */
/* Scales the entire matrix by a scaler factor and stores ther result in     */
/* different matrix.                                                         */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 17 Aug 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_mpy2 (float *result, float *m, float factor)

{ /* matrix_mpy2() */

	result[0]  = m[0]  * factor;
	result[1]  = m[1]  * factor;
	result[2]  = m[2]  * factor;
	result[3]  = m[3]  * factor;
	result[4]  = m[4]  * factor;
	result[5]  = m[5]  * factor;
	result[6]  = m[6]  * factor;
	result[7]  = m[7]  * factor;
	result[8]  = m[8]  * factor;
	result[9]  = m[9]  * factor;
	result[10] = m[10] * factor;
	result[11] = m[11] * factor;
	result[12] = m[12] * factor;
	result[13] = m[13] * factor;
	result[14] = m[14] * factor;
	result[15] = m[15] * factor;

} /* matrix_mpy2() */

/***** END of matrix_mpy2() **************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix3x3_mpy()                                                 */
/*                                                                           */
/* Scales the entire matrix by a scaler factor.                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix3x3_mpy   (float *result,float factor)

{ /* matrix3x3_mpy() */

	result[0] *= factor;
	result[1] *= factor;
	result[2] *= factor;
	result[3] *= factor;
	result[4] *= factor;
	result[5] *= factor;
	result[6] *= factor;
	result[7] *= factor;
	result[8] *= factor;

} /* matrix3x3_mpy() */

/***** END of matrix3x3_mpy() ************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_rot_arb()                                                */
/*                                                                           */
/* Matrix math for rotation about an arbitrary axis.                         */
/*                                                                           */
/* result - the rotated matrix                                               */
/* vect_dof - the axis of rotation                                           */
/* vect_vup - the up vector for the axis of rotation.                        */
/* angle - angle to rotate                                                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 25 Feb 98 EJK reworked                                                    */
/*                                                                           */
/*****************************************************************************/

void matrix_rot_arb ( float *result, float *vect_dof, float *vect_vup, 
					  float angle )

{ /* matrix_rot_arb() */ 

float dof[16];
float inv_dof[16];
float tmp[16];
float rx[3];
float ry[3];

/*****************************************************************************/

    /* rz = vect_dof */
    vect_norm (vect_dof);
    
    /* find rx */
	vect_cross (rx, vect_vup, vect_dof);
	vect_norm (rx);
    
    /* find ry */
	/* vect_cross (ry, rz, rx); */
	vect_cross (ry, vect_dof, rx);
    vect_norm (ry);
    
    /* find inv_dof */
	/* matrix_combine_row (tmp, rx, ry, rz); */
	matrix_combine_row (inv_dof, rx, ry, vect_dof);
    
    /* since its rotational matrix its transpose is its inverse */
	matrix_transpose (dof, inv_dof);
 
 	/* Here's the real work */
    /* 1) Transform the axis of rotation to z-axis and vect_vup to y-axis. */
    /*    This is done by inv_dof matrix.                                  */
    /* 2) Next rotate the axis about the z axis                            */
    /* 3) Transform the z-axis and y-axis back to the axis of rotation and */
    /*    vup. This is done by dof matrix.                                 */
    
	matrix_rotz (tmp, inv_dof, angle);
	matrix_44x44 (result, dof, tmp);
    
} /* matrix_rot_arb() */

/***** END of matrix_rot_arb() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_rotx()                                                   */
/*                                                                           */
/* Rotates source matrix about X axis.                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK - Speed up                                                  */
/*                                                                           */
/*****************************************************************************/

void matrix_rotx (float *result, float *m, float x)

{ /* matrix_rotx() */

static float cx;
static float sx;

/*****************************************************************************/

    cx = cosine(x);
    sx = sine(x);

    /* Speed improvement, matrix_44x44(result,rot_matrix,m); is done */
    /* in place. Removing the function call overhead, and reducing   */
    /* the number of mults by 48, and num adds by 40 */
    
	result[0] = m[0];
	result[1] = m[1];
	result[2] = m[2];
	result[3] = m[3];
   
	result[4] = cx * m[4] - sx * m[8];
	result[5] = cx * m[5] - sx * m[9];
	result[6] = cx * m[6] - sx * m[10];
	result[7] = cx * m[7] - sx * m[11];
 
 	result[8]  = sx * m[4] + cx * m[8];
	result[9]  = sx * m[5] + cx * m[9];
	result[10] = sx * m[6] + cx * m[10];
	result[11] = sx * m[7] + cx * m[11];

	result[12] = m[12];
	result[13] = m[13];
	result[14] = m[14];
	result[15] = m[15];
    
} /* matrix_rotx() */

/***** END of matrix_rotx() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_roty()                                                   */
/*                                                                           */
/* Rotates source matrix about Y axis.                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK - Speed up                                                  */
/*                                                                           */
/*****************************************************************************/

void matrix_roty (float *result, float *m, float x)

{ /* matrix_roty() */

static float cx;
static float sx;

/*****************************************************************************/
    
    cx = cosine(x);
    sx = sine(x);

    /* Speed improvement, matrix_44x44(result,rot_matrix,m); is done */
    /* in place. Removing the function call overhead, and reducing   */
    /* the number of mults by 48, and num adds by 40 */
    
	result[0] = cx * m[0] + sx * m[8];
	result[1] = cx * m[1] + sx * m[9];
	result[2] = cx * m[2] + sx * m[10];
	result[3] = cx * m[3] + sx * m[11];
   
	result[4] = m[4];
	result[5] = m[5];
	result[6] = m[6];
	result[7] = m[7];
 
 	result[8]  = -sx * m[0] + cx * m[8];
	result[9]  = -sx * m[1] + cx * m[9];
	result[10] = -sx * m[2] + cx * m[10];
	result[11] = -sx * m[3] + cx * m[11];

	result[12] = m[12];
	result[13] = m[13];
	result[14] = m[14];
	result[15] = m[15];
    
} /* matrix_roty() */

/***** END of matrix_roty() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_rotz()                                                   */
/*                                                                           */
/* Rotates source matrix about Z axis.                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 07 Mar 97 EJK - Speed up                                                  */
/*                                                                           */
/*****************************************************************************/

void matrix_rotz (float *result, float *m, float x)

{ /* matrix_rotz() */

float cx;
float sx;

/*****************************************************************************/
    
    cx = cosine(x);
    sx = sine(x);

    /* Speed improvement, matrix_44x44(result,rot_matrix,m); is done */
    /* in place. Removing the function call overhead, and reducing   */
    /* the number of mults by 48, and num adds by 40 */
    
	result[0] = cx * m[0] - sx * m[4];
	result[1] = cx * m[1] - sx * m[5];
	result[2] = cx * m[2] - sx * m[6];
	result[3] = cx * m[3] - sx * m[7];
   
	result[4] = sx * m[0] + cx * m[4];
	result[5] = sx * m[1] + cx * m[5];
	result[6] = sx * m[2] + cx * m[6];
	result[7] = sx * m[3] + cx * m[7];
 
 	result[8]  = m[8];
	result[9]  = m[9];
	result[10] = m[10];
	result[11] = m[11];

	result[12] = m[12];
	result[13] = m[13];
	result[14] = m[14];
	result[15] = m[15];

} /* matrix_rotz() */

/***** END of matrix_rotz() **************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: matrix_set_identity()                                           */
/*                                                                           */
/* Sets 4x4 matrix to 4x4 Identity matrix.                                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 11 Mar 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void matrix_set_identity (float *result)

{ /* matrix_rotz() */

    
	result[0] = 1.0f;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
   
	result[4] = 0.0f;
	result[5] = 1.0f;
	result[6] = 0.0f;
	result[7] = 0.0f;
 
 	result[8]  = 0.0f;
	result[9]  = 0.0f;
	result[10] = 1.0f;
	result[11] = 0.0f;

	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = 0.0f;
	result[15] = 1.0f;

} /* matrix_rotz() */

/***** END of matrix_rotz() **************************************************/

/***** VECTOR FUNCTIONS ******************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_set()                                                      */
/*                                                                           */
/* Copies x,y,z to vector U.                                                 */
/*                                                                           */
/* Used for vector initialization.                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_set (float *u, float x, float y, float z)

{ /* vect_set() */

   u[0] = x;
   u[1] = y;
   u[2] = z;

} /* vect_set() */

/***** END of vect_set() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_set_x()                                                    */
/*                                                                           */
/* Copies unit_x to vector U.                                                */
/*                                                                           */
/* Used for vector initialization.                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_set_x (float *u)

{ /* vect_set_x() */

   u[0] = 1.0f;
   u[1] = 0.0f;
   u[2] = 0.0f;

} /* vect_set_x() */

/***** END of vect_set_x() ***************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_set_y()                                                    */
/*                                                                           */
/* Copies unit_y to vector U.                                                */
/*                                                                           */
/* Used for vector initialization.                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_set_y (float *u)

{ /* vect_set_y() */

   u[0] = 0.0f;
   u[1] = 1.0f;
   u[2] = 0.0f;

} /* vect_set_y() */

/***** END of vect_set_y() ***************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_set_z()                                                    */
/*                                                                           */
/* Copies unit_z to vector U.                                                */
/*                                                                           */
/* Used for vector initialization.                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_set_z (float *u)

{ /* vect_set_z() */

   u[0] = 0.0f;
   u[1] = 0.0f;
   u[2] = 1.0f;

} /* vect_set_z() */

/***** END of vect_set_z() ***************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_clear()                                                    */
/*                                                                           */
/* Copies [0,0,0] to vector U.                                               */
/*                                                                           */
/* Used for vector initialization.                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_clear (float *u)

{ /* vect_clear() */

   u[0] = 0.0f;
   u[1] = 0.0f;
   u[2] = 0.0f;

} /* vect_clear() */

/***** END of vect_clear() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_equate()                                                   */
/*                                                                           */
/* Copies vector V to vector U.                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_equate (float *u, float *v)

{ /* vect_equate() */

   u[0] = v[0];
   u[1] = v[1];
   u[2] = v[2];

} /* vect_equate() */

/***** END of vect_equate() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_neg_equate()                                               */
/*                                                                           */
/* Negates the vector V and places it in U.                                  */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 26 Feb 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_neg_equate (float *u, float *v)

{ /* vect_neg_equate() */

	u[0] = -v[0];
	u[1] = -v[1];
	u[2] = -v[2];

} /* vect_neg_equate() */

/***** END of vect_neg_equate() **********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_dot()                                                      */
/*                                                                           */
/* Takes the dot product of U and V and places it in result.                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 30 Dec 97 EJK function returns the float                                  */
/*                                                                           */
/*****************************************************************************/

float vect_dot (float *u, float *v)

{ /* vect_dot() */

	return( (u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) );

} /* vect_dot() */

/***** END of vect_dot() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_cross()                                                    */
/*                                                                           */
/* Takes the cross product of U and V and places it in result.               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_cross (float *result, float *u, float *v)

{ /* vect_cross() */

	result[0] = u[1] * v[2] - u[2] * v[1];  
	result[1] = u[2] * v[0] - u[0] * v[2];
	result[2] = u[0] * v[1] - u[1] * v[0];

} /* vect_cross() */

/***** END of vect_cross() ***************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_subt()                                                     */
/*                                                                           */
/* Subtracts vector V from vector U.                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_subt (float *result, float *u, float *v)

{ /* vect_subt() */

	result[0] = u[0] - v[0];
	result[1] = u[1] - v[1];
	result[2] = u[2] - v[2];

} /* vect_subt() */

/***** END of vect_subt() ****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_add()                                                      */
/*                                                                           */
/* Adds vectors U and V into vector result.                                  */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_add (float *result, float *u, float *v)

{ /* vect_add() */

	result[0] = u[0] + v[0];
	result[1] = u[1] + v[1];
	result[2] = u[2] + v[2];

} /* vect_add() */

/***** END of vect_add() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_mag()                                                      */
/*                                                                           */
/* Magnitude of a vector V.                                                  */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 09 Apr 97 EJK                                                             */
/* 30 Dec 97 EJK function returns the float                                  */
/* 25 Feb 98 EJK early exit                                                  */
/*                                                                           */
/*****************************************************************************/

float vect_mag ( float *v)

{ /* vect_mag() */

float temp;

/*****************************************************************************/
	
	temp = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];

    /* quick checks to advoid sqrt() */
    if(temp == 1.0f)
    	{
        return (1.0f);
        }
    
    /* quick checks to advoid sqrt() */
    if(temp == 0.0f)
    	{
        return (0.0f);
        }

	return ( fsqrt(temp) );

} /* vect_mag() */

/***** END of vect_mag() ****************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_norm()                                                     */
/*                                                                           */
/* Normalizes the vector V in place.                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/* 14 Dec 97 MVB - the norm of a zero vector is zero, rather than crashing   */
/* 03 Feb 98 EJK - remove the many divides.                                  */
/* 04 Feb 98 EJK - added return address for debug purposes.                  */
/* 25 Feb 98 EJK - early exit                                                */
/*                                                                           */
/*****************************************************************************/

void vect_norm (float *v)

{ /* vect_norm() */

static float temp;
//register int *ra asm("$31");

/*****************************************************************************/

	temp = ((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
    
    /* if it is already normalized => stop the train */
    if (temp == 1.0f)
    	{
        return;
        }

	/* check for (0,0,0) vector */
	if (temp == 0.0f)
		{
		/* it's already (0,0,0) so just leave it */
        fprintf(stderr,"SRMATH.C: vect_norm() NoMag=[0,0,0] \n\r");
//        fprintf(stderr,"SRMATH.C: vect_norm() NoMag=[0,0,0] return addr=%p\n\r",ra);
        return;
		}

	temp = frsqrt (temp);
    
	v[0] = v[0] * temp;
	v[1] = v[1] * temp;
	v[2] = v[2] * temp;

} /* vect_norm() */

/***** END of vect_norm() ****************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_negate()                                                   */
/*                                                                           */
/* Negates the vector V in place.                                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 21 Mar 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_negate (float *v)

{ /* vect_norm() */

	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];

} /* vect_norm() */

/***** END of vect_norm() ****************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_mpy()                                                      */
/*                                                                           */
/* Multiplies a scalar S by a vector V in place.                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 01 Apr 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_mpy (float *v,float s)

{ /* vect_mpy() */

	v[0] = s * v[0];
	v[1] = s * v[1];
	v[2] = s * v[2];

} /* vect_mpy() */

/***** END of vect_mpy() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: vect_mpy2()                                                     */
/*                                                                           */
/* Multiplies a scalar S by a vector V into result.                          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 01 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void vect_mpy2 (float *result, float *v, float s)

{ /* vect_mpy() */

	result[0] = s * v[0];
	result[1] = s * v[1];
	result[2] = s * v[2];

} /* vect_mpy() */

/***** END of vect_mpy() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: srmath_test_pt_vs_norm ()                                       */
/*                                                                           */
/* This function takes 3 pts, a,b,c which define a plane with a positive     */
/* normal using right hand rule.                                             */
/*                                                                           */
/* It determines if a 3-d test point is on the positive side of the plane's  */
/* normal.                                                                   */
/*                                                                           */
/* Return TRUE if test point is on the positive side of the plane,           */
/* else return FALSE.                                                        */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 07 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

int srmath_test_pt_vs_norm ( float *pt_a, float *pt_b, float *pt_c, 
							 float *test_pt)

{ /* srmath_test_pt_vs_norm () */

float vba[3];
float vbc[3];
float v_norm[3];
float vtmp[3];
float dot_result;

/*****************************************************************************/

	/* find the normal */ 
	vect_subt(vba,pt_a,pt_b);
	vect_subt(vbc,pt_c,pt_b);
 	vect_cross(v_norm,vbc,vba);   /* v_norm is the normal for pts a,b,c */
 
	vect_subt(vtmp,test_pt,pt_b); /* get a vector for the test pt */
    
    /* find the dot, to determine which side */
    dot_result = vect_dot(vtmp,v_norm);
    
    if(dot_result < 0.0f)
    	{
        return(FALSE);
        }
	else
    	{
        return(TRUE);
        }        

} /* srmath_test_pt_vs_norm () */ 
 
/***** End of srmath_test_pt_vs_norm *****************************************/
 





/*****************************************************************************/
/*                                                                           */
/* FUNCTION: srmath_area_triangle()                                          */
/*                                                                           */
/* This function takes 3 pts, a,b,c which define a triangle and returns      */
/* the area.                                                                 */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 11 Mar 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

float srmath_area_triangle ( float *pt_a, float *pt_b, float *pt_c)

{ /* srmath_area_triangle () */

float result;
float v1[3];
float v2[3];
float v3[3];
float v4[3];

/*****************************************************************************/

	/* area = | (pa x pb) + (pb x pc) + (pc x pa) | / 2 */

	vect_cross(v1,pt_a,pt_b);
	vect_cross(v2,pt_b,pt_c);
	vect_cross(v3,pt_c,pt_a);

	vect_add(v4,v1,v2);
	vect_add(v1,v4,v3);

	result = vect_mag(v1) * 0.5f;

	return(result);

} /* srmath_area_triangle () */

/***** END of srmath_area_triangle () ****************************************/




































 
 
 
 
/***** END of SRMATH.C *******************************************************/
