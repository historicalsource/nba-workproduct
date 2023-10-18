/*****************************************************************************/
/*                                                                           */
/* FILE: SRMATH.H                                                            */
/*                                                                           */
/* Space Race                                                                */
/*                                                                           */
/* Function prototypes and defines for Ed math routines.                     */
/*                                                                           */
/* 12 Feb 97 EJK                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

/***** DEFINES ***************************************************************/

#define SR_PI      3.14159265359f
#define SR_PI4     0.785398163398f
#define SR_3PI4    2.35619449019f
#define SR_PI2     1.5707963268f
#define SR_2PI     6.28318530718f

#define SR_PI3     1.0471975512f
#define SR_2PI3    2.09439510239f

#define SR_PI8     0.392699081699f
#define SR_2PI8    0.785398163398f
#define SR_3PI8    1.1780972451f
#define SR_4PI8    1.5707963268f
#define SR_5PI8    1.9634954085f
#define SR_6PI8    2.35619449019f
#define SR_7PI8    2.74889357189f


#define SR_INV_PI  (1.0f/SR_PI)
#define SR_INV_PI2 (1.0f/SR_PI2)


#define SR_FVRES2 	192.0f
#define SR_FVRES	384.0f
#define SR_FHRES    512.0f
#define SR_FHRES2   256.0f

#define SR_4X4_MATRIX_SIZE 16

/**** MATH FUNCTIONS in SRMATH_S.S *******************************************/

//float fsqrt (float f);
//float frsqrt (float f);
#define	frsqrt	rsqrt


/**** MATH FUNCTIONS *********************************************************/

float absf (float x);

int rand_mod (int val);
int rand_range (int min, int max);
float rand_modf (float val);
float rand_range_f (float min, float max);

float cosine (float x);
float sine (float x);
float cosine2 (float x);
float sine2 (float x);
float arctan2(float y, float x);
float arccos(float x);
float arcsin (float x);

void rect_to_spherical(float x, float y, float z, 
						float *r, float *theta, float *phi);

void spherical_to_rect(float r, float theta, float phi,
						float *x, float *y, float *z );

void rect_to_spherical_zy(float x, float y, float z, 
						float *r, float *theta, float *phi);

void spherical_to_rect_zy(float r, float theta, float phi,
						float *x, float *y, float *z );

float get_angle_zx (float *v);
float get_angle_xy (float *v);
float get_angle_yz (float *v);
float get_angle_v1v2 (float *v1, float *v2);

void  srmath_get_closest_point(float *v, float x0, float z0, float rx, float rz, 
                               float *x1, float *z1);


/**** MATRIX FUNCTIONS *******************************************************/

float get_determinant_3x3 (float *m);
float get_determinant_4x4 (float *m);
float adj_sub_det3x3 (float *m,int *a);
void get_adjoint4x4(float *output,float *input);
void get_adjoint3x3(float *output,float *input);
int get_inv_matrix4x4(float *output,float *input);
int get_inv_matrix3x3(float *output,float *input);
void get_orthogonal_matrix (float *out, float *r);


void matrix_find_dof (float *result,float *vect_dof);
void matrix_find_dof_vup (float *result, float *vect_dof, float *vect_vup);

void matrix_combine_row (float *result,float *x,float *y,float *z);
void matrix_combine_row_3x3 (float *result,float *x,float *y,float *z);
void matrix_combine_col (float *result,float *x,float *y,float *z);
void matrix_combine_col_3x3 (float *result,float *x,float *y,float *z);
void matrix_combine_33_into_44 (float *result, float *m);

void matrix_equate (float *m, float *n);
void matrix_subt (float *out, float *m, float *n);
void matrix_subt2 ( float *m, float *n);
void matrix_add (float *out, float *m, float *n);
void matrix_add2 ( float *m, float *n);
void matrix_transpose (float *m, float *n);

void matrix_44x41 (float *result,float *m, float *v);
void matrix_44x41_w (float *result,float *m, float *v);
void matrix_44x41_wxyz (float *result, float *m, float *v);
void matrix_44x41_wf (float *result,float *m, float *v);
void matrix_44x41_zero (float *result, float *m);
void matrix_44x41_unit_x (float *result, float *m);
void matrix_44x41_unit_y (float *result, float *m);
void matrix_44x41_unit_z (float *result, float *m);

void matrix_44x44 (float *result,float *m, float *n);
void matrix_44x44_transpose (float *result, float *m, float *n);

void matrix_33x31 (float *result, float *m, float *v);
void matrix_33x33 (float *result, float *m, float *n);

void matrix_trans (float *result, float *m, float x,float y,float z);
void matrix_scale (float *result, float *m, float x,float y,float z);
void matrix_mpy (float *result, float factor);
void matrix_mpy2 (float *result, float *m, float factor);

void matrix3x3_mpy (float *result, float factor);

void matrix_rot_arb (float *result, float *vect_dof, float *vect_vup, float angle);

void matrix_rotx (float *result,float *m, float x);
void matrix_roty (float *result,float *m, float x);
void matrix_rotz (float *result,float *m, float x);
void matrix_set_identity (float *result);

/**** VECTOR FUNCTIONS *******************************************************/

void vect_set (float *u, float x, float y, float z);
void vect_set_x (float *u);
void vect_set_y (float *u);
void vect_set_z (float *u);
void vect_clear (float *u);

void vect_equate (float *u,float *v);
void vect_neg_equate (float *u, float *v);

float vect_dot (float *u, float *v);
void vect_cross (float *result,float *u,float *v);
void vect_subt (float *result,float *u,float *v);
void vect_add (float *result,float *u,float *v);
void vect_norm (float *v);
void vect_negate (float *v);
void vect_mpy (float *v,float s);
void vect_mpy2 (float *result,float *v,float s);
float vect_mag ( float *v );

/**** MISC FUNCTIONS *******************************************************/

int srmath_test_pt_vs_norm ( float *pt_a, float *pt_b, float *pt_c, float *test_pt);
float srmath_area_triangle ( float *pt_a, float *pt_b, float *pt_c);

/***** END of SRMATH.H *******************************************************/




