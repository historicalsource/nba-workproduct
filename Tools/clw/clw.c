#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "iffr.h"
#include "uvr.h"

//#define  DEBUG_STRIPS
#define  DEBUG_STRIPS_STATS
#define  FIRST_TRIANGLE_NEGATIVE 0

int tricmp( const void *p1, const void *p2 );
void strip(char *np,char *papp);

int		num_strip_verts;
int		num_strip_info;



/* model source data structures */




typedef struct _STRIP_TMP {
	short v;
	short st;
	short new_strip;
	short text;
} STRIP_TMP;


typedef struct _STRIP_VERTS {
	short v;
	short st;
} STRIP_VERTS;


typedef struct _STRIP_INFO {
	short num_verts;
	short text;
//	short strip_index;
} STRIP_INFO;


typedef struct _STRIP {
	int		num_strip_verts;
	int		num_strip_info;
	STRIP_VERTS *strip_verts;
	STRIP_INFO  *strip_info;
} STRIP;






typedef struct _VERTEX {
	float	x,y,z;
} VERTEX;

typedef struct _ST {
	float	s,t;
} ST;

typedef struct _TRI {
	unsigned short	v1,v2,v3;
	unsigned short	t1,t2,t3;
	short			texture;
} TRI;

typedef enum {
	PS_NONE,
	PS_NAME,
	PS_SCALE,
	PS_SCALEX,
	PS_SCALEY,
	PS_SCALEZ,
	PS_APPEND,
	PS_MAP,
	PS_MAPW,
	PS_MAPH
} pstate;

enum {
	MODE_GENERIC_MODEL,
	MODE_HEAD_MODEL,
	MODE_LIMB_MODEL
};

VERTEX	bvtx[VTX_MAX];
TRI 		btri[FACE_MAX];
ST			bst[MAX_TRIANGLES * 3];

int	nvtx = 0, ntri = 0, nst = 0;
int	nface = 0, nsurf = 0;
vec3d		vdb[VTX_MAX];		/* vertex list */
grface		fdb[FACE_MAX];		/* face list */
lwSurface	sdb[SURF_MAX];		/* surface list */
flags_t		flags;
char		textures[MAX_TEXTURES][TEXTURE_LEN];
vertex_t	uvpairs[MAX_TRIANGLES*3];
int			to_unique[MAX_TEXTURES];
int			from_unique[MAX_TEXTURES];
int			num_unique, is_unique;
int			nfiles,nuvpairs,ntriangles;
int			zsort_mode;
int			no_limbs;
int			no_textures;
float		sfx = 1.0f;
float		sfy = 1.0f;
float		sfz = 1.0f;

static char writebuf[256], datfile[256];

static void print_help(void);

int operating_mode;
	FILE *dat;

VERTEX	bbox_data[8];
int num_bbox;




/**********************************************************************/
int main(int argc, char *args[])
{
	int		i,j,os;
	pstate pmode = PS_NONE;
	int cf_square = FALSE;
	int cf_1x2 = FALSE;
	int cf_w = 0;
	int cf_h = 0;
	char empty_string[] = "";
	char xxx_string[] = "xxx";
	char *np = xxx_string, *papp = empty_string;
	float	sf,tf;
	char *filespec = NULL;

#ifdef JONHEY
	float max_x,max_y,max_z,min_x,min_y,min_z;
	float vx,vy,vz;

#endif

	operating_mode = MODE_GENERIC_MODEL;
	/* parse command line */
	for (i = 1; i < argc; i++) {
		switch (args[i][0]) {
		case '-':
			switch (args[i][1]) {
			case 'b':				/* for dump a head model into a binary .DAT file */
				operating_mode = MODE_HEAD_MODEL;
				break;
			case 'e':				/* for generating a limb header file */
				operating_mode = MODE_LIMB_MODEL;
				break;
			case 'm':	/* scale factor */
				if (cf_square || cf_1x2)
					fprintf(stderr,"WARNING: %s will be overridden by a previous option\n", args[i]);
				switch (args[i][2]) {
				case '\0':
					pmode = PS_MAP;
					break;
				case 'w':
					pmode = PS_MAPW;
					break;
				case 'h':
					pmode = PS_MAPH;
					break;
				default:
					fprintf(stderr,"WARNING: Unrecognized command line option: %s\n", args[i]);
					break;
				}
				break;
			case 'n':	/* name */
				pmode = PS_NAME;
				break;
			case 's':	/* assume square textures */
				cf_square = TRUE;
				break;
			case 'f':	/* scale factor */
				switch (args[i][2]) {
				case '\0':
					pmode = PS_SCALE;
					break;
				case 'x':
					pmode = PS_SCALEX;
					break;
				case 'y':
					pmode = PS_SCALEY;
					break;
				case 'z':
					pmode = PS_SCALEZ;
					break;
				default:
					fprintf(stderr,"WARNING: Unrecognized command line option: %s\n", args[i]);
					break;
				}
				break;
			case 'h':	/* help */
				print_help();
				exit(1);
				break;
			case 'a':	/* append string to pset names */
				pmode = PS_APPEND;
				break;
			case 'z':	/* z-sort triangles */
				zsort_mode = TRUE;
				break;
			case 'x':	/* assume 2x1 textures */
				cf_1x2 = TRUE;
				break;
			case 'l':	/* no limb list */
				no_limbs = TRUE;
				break;
			case 't':	/* no texture list */
				no_textures = TRUE;
				break;
			default:
				fprintf(stderr,"WARNING: Unrecognized command line option: %s\n", args[i]);
				break;
			}
			break;
		default:
			switch (pmode) {
			case PS_NAME:
				np = args[i];
				break;
			case PS_SCALE:
				sfx = sfy = sfz = atof(args[i]);
				break;
			case PS_SCALEX:
				sfx = atof(args[i]);
				break;
			case PS_SCALEY:
				sfy = atof(args[i]);
				break;
			case PS_SCALEZ:
				sfz = atof(args[i]);
				break;
			case PS_APPEND:
				papp = args[i];
				break;
			case PS_NONE:
				filespec = args[i];
				break;
			case PS_MAP:
				cf_w = cf_h = atoi(args[i]);
				break;
			case PS_MAPW:
				cf_w = atoi(args[i]);
				break;
			case PS_MAPH:
				cf_h = atoi(args[i]);
				break;
			default:
				break;
			}
			pmode = PS_NONE;
			break;
		}
	}
	
	if (!filespec) {
		fprintf(stderr, "ERROR: File not specified.  clw -h for info.\n");
		exit(1);
	}
	
	sprintf(writebuf, "%s.lwo", filespec);
	read_lwo(writebuf);
	
	sprintf(writebuf, "%s.uv", filespec);
	read_uv(writebuf);
	
	/*
	 * to_unique maps original texture list into list of unique indices.
	 * from_unique maps the other way, to first instance of each texture
	 */
	num_unique = 0;
	for(i = 0; i < nsurf; i++) {
		is_unique = TRUE;
		for(j = 0; (j < i) && is_unique; j++) {
			if (!strcmp(textures[i], textures[j])) {
				is_unique = FALSE;
				j--;
			}
		}
		
		if (is_unique) {
			num_unique++;
			from_unique[num_unique-1] = i;
		}
		to_unique[i] = j;
	}

	/* to_unique at this point maps duplicate references onto
	 * the first instance of each texture.  NOW map them to
	 * what will be our final texture list.
	 */
	for (i=0; i<num_unique; i++)
	{
		for( j=0; j<nsurf; j++)
		{
			if (to_unique[j] == from_unique[i])
				to_unique[j] = i;
		}
	}
	
	/* update surface indices */
	for (i = 0; i < nface; i++)
		fdb[i].surface = to_unique[fdb[i].surface];




	/* if dumping a head models to a .DAT file */
	if (operating_mode == MODE_HEAD_MODEL) {
		sprintf(datfile, "%s%s.dat", np, papp);
		dat = fopen(datfile, "wb");
		if (dat == NULL) {
			fprintf(stderr,"could not create file %s\n", datfile);
			exit(1);
		}
	} else
		dat = NULL;


	
#ifdef JONHEY
	/* mangle the surface indices */
	os = fdb[0].surface;
	for (i = 1; i < nface; i++) {
		if (fdb[i].surface == os)
			fdb[i].surface = os;

		else
			os = fdb[i].surface;
	}

	qsort( fdb,	nface, sizeof(grface),tricmp);

	strip(np,papp);

#endif


	/* mangle the surface indices */
	os = fdb[0].surface;
	for (i = 1; i < nface; i++) {
		if (fdb[i].surface == os)
			fdb[i].surface = -1;
		else
			os = fdb[i].surface;
	}


	

	/* output the VERTEX list */
	if (operating_mode == MODE_HEAD_MODEL) {
		for (i = 0; i < nvtx; i++) {
			bvtx[i].x = vdb[i].x * sfx;
			bvtx[i].y = vdb[i].y * sfy;
			bvtx[i].z = vdb[i].z * sfz * -1.0f;
		}
		fwrite(bvtx, sizeof(bvtx[0]), nvtx, dat);
	} else {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("VERTEX %s%s_vtx[] =\n{\n", np, papp);
		for (i = 0; i < nvtx; i++) {
			printf("\t{%ff,%ff,%ff}%s",
					vdb[i].x * sfx,
					vdb[i].y * sfy,
					vdb[i].z * sfz * -1.0f,
					(i == (nvtx-1) ? "\n};\n" : ",\n"));
		}
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
		printf("\n");
	}

	
	/* output the TRI list */
	if (operating_mode == MODE_HEAD_MODEL) {
		for (i = 0; i < nface; i++) {

#ifndef JONHEY
			btri[i].v1 = fdb[i].vtxi[0] * 3;
			btri[i].v2 = fdb[i].vtxi[1] * 3;
			btri[i].v3 = fdb[i].vtxi[2] * 3;
#else
			btri[i].v1 = fdb[i].vtxi[0];
			btri[i].v2 = fdb[i].vtxi[1];
			btri[i].v3 = fdb[i].vtxi[2];
#endif
			btri[i].t1 = fdb[i].tvtx[0];
			btri[i].t2 = fdb[i].tvtx[1];
			btri[i].t3 = fdb[i].tvtx[2];
#ifndef JONHEY
			btri[i].texture = fdb[i].surface * 4;
#else
			btri[i].texture = fdb[i].surface;
#endif
		}
		btri[i].v1 = btri[i].v2 = btri[i].v3 = 0;
		btri[i].t1 = btri[i].t2 = btri[i].t3 = 0;
		btri[i].texture = -20;
//remove tris
//		fwrite(btri, sizeof(btri[0]), nface + 1, dat);
	} else {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("TRI %s%s_tris[] =\n{\n", np, papp);
		for (i = 0; i < nface; i++) {

#ifndef JONHEY
			printf("\t{%3d,%3d,%3d, %2d,%2d,%2d, %d},\n",
				   fdb[i].vtxi[0] * 3,
				   fdb[i].vtxi[1] * 3,
				   fdb[i].vtxi[2] * 3,
				   fdb[i].tvtx[0],
				   fdb[i].tvtx[1],
				   fdb[i].tvtx[2],
				   fdb[i].surface * 4);
#else
			printf("\t{%3d,%3d,%3d, %2d,%2d,%2d, %d},\n",
				   fdb[i].vtxi[0] ,
				   fdb[i].vtxi[1] ,
				   fdb[i].vtxi[2] ,
				   fdb[i].tvtx[0],
				   fdb[i].tvtx[1],
				   fdb[i].tvtx[2],
				   fdb[i].surface);
#endif

		}
		printf("\t{0, 0, 0, 0, 0, 0, -20}\n};\n");
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
		printf("\n");
	}
	




	/* get the S and T scaling factor */
	if (cf_square) {
		sf = 256.0f;
		tf = 256.0f;
	} else if (cf_1x2) {
		sf = 256.0f;
		tf = 128.0f;
	} else {
		sf = pow2(cf_w);
		tf = pow2(cf_h);
	}
	
	/* output the S and T list */
	if (operating_mode == MODE_HEAD_MODEL) {
		for (i = 0; i < nuvpairs; i++) {
			bst[i].s = uvpairs[i].u * sf;
			bst[i].t = uvpairs[i].v * tf;
		}
		fwrite(&nuvpairs, sizeof(nuvpairs), 1, dat);
		fwrite(bst, sizeof(bst[0]), nuvpairs, dat);
	} else {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("ST %s%s_st[] =\n{\n", np, papp);
		for (i = 0; i < nuvpairs; i++) {
			printf("\t{%ff,%ff}%s",
					uvpairs[i].u * sf,
					uvpairs[i].v * tf,
					(i == (nuvpairs-1) ? "\n};\n" : ",\n"));
		}
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
		printf("\n");
	}


	#ifdef JONHEY
	if (operating_mode == MODE_HEAD_MODEL) {

		if (nvtx > 8)
			{
	
			max_x = vdb[0].x * sfx;
			max_y = vdb[0].y * sfy;
			max_z = vdb[0].z * sfz * -1.0f;
			min_x = vdb[0].x * sfx;
			min_y = vdb[0].y * sfy;
			min_z = vdb[0].z * sfz * -1.0f;
				
			for (i = 0; i < nvtx; i++) 
				{

				vx = vdb[i].x * sfx;
				vy = vdb[i].y * sfy;
				vz = vdb[i].z * sfz * -1.0f;


				if (vx < min_x)
					min_x = vx;

				if (vy < min_y)
					min_y = vy;

				if (vz < min_z)
					min_z = vz;

				if (vx > max_x)
					max_x = vx;

				if (vy > max_y)
					max_y = vy;

				if (vz > max_z)
					max_z = vz;

				}
	

			bbox_data[0].x = min_x;
			bbox_data[0].y = min_y;
			bbox_data[0].z = min_z;
			bbox_data[1].x = min_x;
			bbox_data[1].y = min_y;
			bbox_data[1].z = max_z;
			bbox_data[2].x = min_x;
			bbox_data[2].y = max_y;
			bbox_data[2].z = min_z;
			bbox_data[3].x = min_x;
			bbox_data[3].y = max_y;
			bbox_data[3].z = max_z;
			bbox_data[4].x = max_x;
			bbox_data[4].y = min_y;
			bbox_data[4].z = min_z;
			bbox_data[5].x = max_x;
			bbox_data[5].y = min_y;
			bbox_data[5].z = max_z;
			bbox_data[6].x = max_x;
			bbox_data[6].y = max_y;
			bbox_data[6].z = min_z;
			bbox_data[7].x = max_x;
			bbox_data[7].y = max_y;
			bbox_data[7].z = max_z;

			num_bbox = 8;

			}
		else
			{
			num_bbox = 0;
			}

		fwrite(&num_bbox, sizeof(num_bbox), 1, dat);
		if(num_bbox > 0)
			{
			fwrite(&bbox_data[0], sizeof(bbox_data[0]), num_bbox, dat);
			}
		fclose(dat);

	} else {
		/* output the bounding boxes */
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");

		printf("VERTEX %s%s_bbox[] =\n{\n", np, papp);

		max_x = vdb[0].x * sfx;
		max_y = vdb[0].y * sfy;
		max_z = vdb[0].z * sfz * -1.0f;
		min_x = vdb[0].x * sfx;
		min_y = vdb[0].y * sfy;
		min_z = vdb[0].z * sfz * -1.0f;
			
		for (i = 0; i < nvtx; i++) 
			{

			vx = vdb[i].x * sfx;
			vy = vdb[i].y * sfy;
			vz = vdb[i].z * sfz * -1.0f;


			if (vx < min_x)
				min_x = vx;

			if (vy < min_y)
				min_y = vy;

			if (vz < min_z)
				min_z = vz;

			if (vx > max_x)
				max_x = vx;

			if (vy > max_y)
				max_y = vy;

			if (vz > max_z)
				max_z = vz;

			}
	
			printf("\t{%ff,%ff,%ff},\n",min_x,min_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,min_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,max_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,max_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,min_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,min_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,max_y,min_z);
			printf("\t{%ff,%ff,%ff}\n};\n",max_x,max_y,max_z);

		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");

		printf("\n");

	}
	#endif
	
	/* output the LIMB data */
	printf("LIMB limb_%s%s =\n{\n", np, papp);
	printf("\t%d,\n", nvtx);
	printf("\t%d,\n", nface);
	printf("#ifdef MODEL_DATA_ST_COUNT\n");
	printf("\t%d,\n", nuvpairs);
	printf("#else\n");
	printf("\tNULL,\n");
	printf("#endif\n");
	if (operating_mode == MODE_GENERIC_MODEL) {
		printf("\t%s%s_vtx,\n", np, papp);
		printf("\t%s%s_st,\n", np, papp);
		printf("\t%s%s_tris,\n", np, papp);
#ifdef JONHEY
		if (nvtx > 8)
			{
			printf("\t%s%s_bbox,\n", np, papp);
			}
		else
			{
			printf("\tNULL,\n");
			}
		printf("\t&%s%s_strip,\n", np, papp);
#endif
	} else {
		printf("#ifdef MODEL_DATA\n");
		printf("\t%s%s_vtx,\n", np, papp);
		printf("\t%s%s_st,\n", np, papp);
		printf("\t%s%s_tris,\n", np, papp);
	#ifdef JONHEY
		if (nvtx > 8)
			{
			printf("\t%s%s_bbox,\n", np, papp);
			}
		else
			{
			printf("\tNULL,\n");
			}
								  
		printf("\t&%s%s_strip,\n", np, papp);
	#endif
		printf("#else\n");
		printf("\tNULL,\n");
		printf("\tNULL,\n");
		printf("\tNULL,\n");
	#ifdef JONHEY
		printf("\tNULL,\n");

		printf("\t&%s%s_strip,\n", np, papp);
	#endif
		printf("#endif\n");
	}
	printf("};\n\n");
	
	/* output the TEXTURE data */
	if ((operating_mode != MODE_HEAD_MODEL) && !no_textures) {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("char *%s_textures[] =\n{\n", np);
		for (i = 0; i < num_unique; i++)
			printf("\t\"%s\",\n", textures[from_unique[i]]);
		printf("\tNULL\n};\n");
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
	}
	return 0;
}

void print_help(void)
{
	printf("clw [options] <filename>\n");
	printf("  -n xxx      name polyset\n");
	printf("  -s          square textures\n");
	printf("  -x          2x1 textures\n");
	printf("  -f[xyz]     scale factor\n");
}



int tricmp( const void *p1, const void *p2 )
{
	grface		*t1, *t2;

	t1 = (grface *)p1;
	t2 = (grface *)p2;

  	if( t1->surface < t2->surface  )
		return(-1);

  	else if( t1->surface == t2->surface  )
		return(0);

  	else//if( t1->surface > t2->surface  )
		return(1);
}
















int find_max_strip(int *flag);

void find_strip_poly(int poly,int *flag);

void find_adjacent_tris(int poly, int *a, int *b, int *c);

int find_adjacent_side(int poly, int v1_index, int v2_index,
					   int polya, int polyb, int polyc);

int find_adjacent_side2(int poly, int v1_index, int v2_index, int *flag);


int	ploy_strip_flag[500];
int	scratch_flag[500];

STRIP_TMP poly_strip[500][500];

STRIP_INFO strip_info[500];
STRIP_VERTS strip_verts[500];







/******************************************************************************/
/*                                                                            */
/* Function: strip()                                                          */
/*                                                                            */
/******************************************************************************/


void strip(char *np,char *papp)
{
int i;
int j;
int k;
//int d;
//int va;
//int vb;
//int vp;
//int k;
int num_polys;
//int num_strip_verts;
//int l;
#ifdef DEBUG_STRIPS
int kkk;
int lll;
int jjj;
#endif
int jjj;
int kkk;
int poly;
int text;
int text_tmp;
int largest_strip;

/******************************************************************************/



	/* CLEAR the strip */
	for (i = 0; i < nface; i++) 
		{
		for (j = 0; j < nface*2; j++) 
			{
			poly_strip[i][j].v = -1;
			poly_strip[i][j].st = -1;
			poly_strip[i][j].text = 0;
			poly_strip[i][j].new_strip = -1;
			}
		}

	/* Clear the flags */  
	for (i = 0; i < nface; i++) 
		{
		ploy_strip_flag[i] = -1;
		}




	/* Clear the flags */  
	for (i = 0; i < 500; i++) 
		{
		strip_info[i].num_verts = -1;
		strip_info[i].text = -1;
  //		strip_info[i].strip_index = 0;
		}










//	for (i = 0; i < nface; i++) 
		{

//		if (ploy_strip_flag[i] == 1)
//			continue;

		do  {


#ifdef DEBUG_STRIPS
			printf("\n***************************************\n");
#endif
			poly = find_max_strip(ploy_strip_flag);

#ifdef DEBUG_STRIPS
			printf("\npoly: %d\n\n",poly);
#endif			

			if (poly != -1)
				find_strip_poly(poly, ploy_strip_flag);


#ifdef DEBUG_STRIPS
			jjj = 0;
			lll = 0;
			for (kkk = 0; kkk < nface; kkk++) 
				{
			printf("ploy_strip_flag[%d] = %d\n",kkk,ploy_strip_flag[kkk]);
				if( (ploy_strip_flag[kkk]) == -1)
					jjj++;
				else
					lll++;
		   		}

			printf("number found: %d NOT found: %d\n",lll,jjj);
#endif
			} while (poly != -1);


		}

#ifdef DEBUG_STRIPS
for (i = 0; i < nface; i++) 
	printf("ploy_strip_flag[%d] = %d\n",i,ploy_strip_flag[i]);
#endif


//printf("\n\n\n");
//printf("strip list the wrath of conn\n");
//num_strip_verts = 0;
num_polys = 0;

#ifdef DEBUG_STRIPS
	for (i = 0; i < nface; i++) 
		{
		num_polys++;
		printf("poly[%d]:\n",i);
			j = 0;			
			while(poly_strip[i][j].v != -1)
				{
				printf("    %03d  %03d  %03d   %02d\n",
										poly_strip[i][j].v,
										poly_strip[i][j].st,
										poly_strip[i][j].text,
										poly_strip[i][j].new_strip);
				j++;
  //				num_strip_verts++;
				}
    
		}

#endif

//printf("\n\nOUTPUT\n\n");
text = -1;
	if (operating_mode != MODE_GENERIC_MODEL) {
	printf("#ifdef MODEL_DATA\n");
	}

	printf("STRIP_VERTS %s%s_strip_verts[] =\n{\n", np, papp);
jjj = 0;
kkk = 0;
num_strip_verts = 0;

	for (i = 0; i < nface; i++) 
		{
		num_polys++;
			j = 0;			
			k = 0;
			while(poly_strip[i][k].v != -1)
				{
				k++;
				}

			while(poly_strip[i][j].v != -1)
				{

				if(poly_strip[i][j].text == text)
					{
					text_tmp = -1;
					}
				else
					{
					text = poly_strip[i][j].text;
					text_tmp = text;
					}



				if(poly_strip[i][j].new_strip == 1)
					{
					strip_info[kkk].num_verts = k;
					strip_info[kkk].text = text_tmp;
//					strip_info[kkk].strip_index = jjj;
					kkk++;
					}




				if(poly_strip[i][j].new_strip != 1)
					 k = -1;
					
//			printf("\t%d {%3d,%3d,%3d,%3d},\n",
//										jjj,
//										poly_strip[i][j].v,
//										poly_strip[i][j].st,
//										text_tmp,
//										k);
//
			jjj++;
				printf("\t{%3d,%3d},\n",
										poly_strip[i][j].v,
										poly_strip[i][j].st);
				strip_verts[num_strip_verts].v = poly_strip[i][j].v;
				strip_verts[num_strip_verts].st = poly_strip[i][j].st;

	
				j++;
				num_strip_verts++;
				}
		}

		printf("\n};\n\n");


	
	
	printf("STRIP_INFO %s%s_strip_info[] =\n{\n", np, papp);

	kkk = 0;
	num_strip_info = 0;
	largest_strip = 0;
	while (strip_info[kkk].num_verts != -1)
		{

		printf("\t{%3d,%3d},\n",
					strip_info[kkk].num_verts,
					strip_info[kkk].text);

		if(strip_info[kkk].num_verts > largest_strip)
			largest_strip = strip_info[kkk].num_verts;

//		printf("\t{%3d,%3d,%3d},\n",
//					strip_info[kkk].num_verts,
//					strip_info[kkk].text,
//					strip_info[kkk].strip_index);

		num_strip_info++;


		kkk++;
		}

		num_strip_info++;

		printf("\t{ -1, -1}\n};\n\n");



	printf("/* largest_strip: %d */\n\n",largest_strip);


	if (operating_mode == MODE_HEAD_MODEL) {
		fwrite(strip_info, sizeof(strip_info[0]), num_strip_info, dat);
		fwrite(strip_verts, sizeof(strip_verts[0]), num_strip_verts, dat);
		}











	if (operating_mode != MODE_GENERIC_MODEL) {
	printf("#endif\n\n");
	}



	if (operating_mode == MODE_GENERIC_MODEL) 
		{
		printf("STRIP %s%s_strip =\n{\n", np, papp);
		printf("\t%3d,	  /*	num_strip_verts */\n",num_strip_verts);
		printf("\t%3d,	  /*	num_strip_info  */\n",num_strip_info);
 		printf("\t%s%s_strip_verts,\n", np, papp);
		printf("\t%s%s_strip_info\n", np, papp);
		printf("};\n");
		}
	else
		{

		printf("#ifdef MODEL_DATA\n");
		printf("STRIP %s%s_strip =\n{\n", np, papp);
		printf("\t%3d,	  /*	num_strip_verts */\n",num_strip_verts);
		printf("\t%3d,	  /*	num_strip_info  */\n",num_strip_info);
 		printf("\t%s%s_strip_verts,\n", np, papp);
		printf("\t%s%s_strip_info\n", np, papp);
		printf("};\n");
		printf("#else\n");
		printf("STRIP %s%s_strip =\n{\n", np, papp);
		printf("\t%3d,	  /*	num_strip_verts */\n",num_strip_verts);
		printf("\t%3d,	  /*	num_strip_info  */\n",num_strip_info);
 		printf("\tNULL,   /* %s%s_strip_verts   */\n", np, papp);
		printf("\tNULL    /* %s%s_strip_info    */\n", np, papp);

		printf("};\n");
	
		printf("#endif\n");



		}











//#ifdef DEBUG_STRIPS
#ifdef DEBUG_STRIPS_STATS
printf("\n");
printf("/*  num_polys: %d  num_verts: %d   num_strip_verts: %d  vert/poly: %5.3f  */\n",
	num_polys,num_polys*3,num_strip_verts,(float)num_strip_verts/(float)num_polys);
printf("\n");
#endif
//#endif


}







/******************************************************************************/
/*                                                                            */
/* Function: find_max_strip()                                                 */
/*                                                                            */
/******************************************************************************/


int find_max_strip(int *flag)
{
int i;
int va;
int vb;
int k;
int d;
int j;
int vp;
int max_poly;
int l;
int max_count;
int max_direction;
//int v1;
//int v2;
//int v3;
//int t1;
//int t2;
//int t3;

/******************************************************************************/

//	printf("\n\n");
	max_poly = -1;
	max_count = 0;
	max_direction = 1;


	for (i = 0; i < nface; i++) 
		{

		for (l = 0; l < nface; l++) 
			scratch_flag[l] = flag[l];


		if (scratch_flag[i] == 1)
			continue;

#ifdef DEBUG_STRIPS
		printf("Find_max_strip poly[%d] %3d,%3d,%3d, %2d,%2d,%2d,},\n",
 				i,
			   	fdb[i].vtxi[0] ,
			   	fdb[i].vtxi[1] ,
			   	fdb[i].vtxi[2] ,
			   	fdb[i].tvtx[0],
			  	fdb[i].tvtx[1],
			   	fdb[i].tvtx[2]);
#endif

		j = 0;
		j++;	
#if  (FIRST_TRIANGLE_NEGATIVE)
		va = fdb[i].vtxi[1];
#else
		va = fdb[i].vtxi[2];
#endif

		j++;			
#if  (FIRST_TRIANGLE_NEGATIVE)
		vb = fdb[i].vtxi[2];
#else
		vb = fdb[i].vtxi[1];
#endif


		k = i;
		do {
			d = find_adjacent_side2(k, va, vb, scratch_flag);

#ifdef DEBUG_STRIPS
			printf("   d: %d  k:%d  va:%d  vb:%d\n",d,k,va,vb);
#endif

			if (d != -1)
				{
				k = d;
				j++;

#ifdef DEBUG_STRIPS
				printf("    inside d: %d  k:%d  va:%d  vb:%d\n",d,k,va,vb);
#endif
				
				vp = fdb[d].vtxi[0];
				if ((vp != va) && (vp != vb))
					{
#ifdef DEBUG_STRIPS
					printf("    ==> vp:%d\n",vp);
#endif
					va = vb;
					vb = vp;
					continue;
					}

				vp = fdb[d].vtxi[1];
				if ((vp != va) && (vp != vb))
					{
#ifdef DEBUG_STRIPS
					printf("    ==> vp:%d\n",vp);
#endif
					va = vb;
					vb = vp;
					continue;
					}

				vp = fdb[d].vtxi[2];
				if ((vp != va) && (vp != vb))
					{
#ifdef DEBUG_STRIPS
					printf("    ==> vp:%d\n",vp);
#endif
					va = vb;
					vb = vp;
					continue;
					}
	
				}

			
			if(j > max_count)
				{
				max_count = j;
				max_poly = i;
				max_direction = 1;
				}

			} while(d != -1);
		}


#ifdef DEBUG_STRIPS
		printf("Max poly:%d  count:%d   direction:%d\n",max_poly,max_count,max_direction);
#endif
		return(max_poly);
}



/******************************************************************************/
/*                                                                            */
/* Function: find_adjacent_side2()                                             */
/*                                                                            */
/******************************************************************************/
						
int find_adjacent_side2(int poly, int v1_index, int v2_index, int *flag)
{
int v1;
int v2;
int t1;
int t2;
int v3;
int t3;
int found_poly;
int count;
int vert1;
int vert2;
int vert3;
int va;
int ta;
int vb;
int tb;

int i;
int j;
int text;

/******************************************************************************/
#ifdef DEBUG_STRIPS
	printf("    find adj side2: %d  side %d %d\n",poly,v1_index,v2_index);
#endif
	flag[poly] = 1;

	found_poly = -1;


	text = fdb[poly].surface;

#if (FIRST_TRIANGLE_NEGATIVE)
	va = v1 = fdb[poly].vtxi[0];
	vb = v2 = fdb[poly].vtxi[1];
	v3 = fdb[poly].vtxi[2];

	ta = t1 = fdb[poly].tvtx[0];
	tb = t2 = fdb[poly].tvtx[1];
	t3 = fdb[poly].tvtx[2];
#else
	va = v1 = fdb[poly].vtxi[0];
	vb = v2 = fdb[poly].vtxi[2];
	v3 = fdb[poly].vtxi[1];

	ta = t1 = fdb[poly].tvtx[0];
	tb = t2 = fdb[poly].tvtx[2];
	t3 = fdb[poly].tvtx[1];
#endif



	vert1 = FALSE;
	vert2 = FALSE;
	vert3 = FALSE;


	if (v1 == v1_index)
		vert1 = TRUE;

	if (v2 == v1_index)
		vert2 = TRUE;

	if (v3 == v1_index)
		vert3 = TRUE;


	if (v1 == v2_index)
		vert1 = TRUE;

	if (v2 == v2_index)
		vert2 = TRUE;

	if (v3 == v2_index)
		vert3 = TRUE;


	if(vert1 == TRUE && vert2 == TRUE)
		{
		va = v1;
		ta = t1;
		vb = v2;
		tb = t2;
		}

	if(vert2 == TRUE && vert3 == TRUE)
		{
		va = v2;
		ta = t2;
		vb = v3;
		tb = t3;
		}

	if(vert1 == TRUE && vert3 == TRUE)
		{
		va = v1;
		ta = t1;
		vb = v3;
		tb = t3;
		}


	for (i = 0; i < nface; i++) {

		count = 0;

		//if ((ploy_strip_flag[i] == 1)  || (i == poly) )
		if ((flag[i] == 1)  || (i == poly) )

			continue;

		if ( (va == fdb[i].vtxi[0] && ta == fdb[i].tvtx[0]) || 
			 (va == fdb[i].vtxi[1] && ta == fdb[i].tvtx[1]) || 
			 (va == fdb[i].vtxi[2] && ta == fdb[i].tvtx[2]) )
			count++;

		if ( (vb == fdb[i].vtxi[0] && tb == fdb[i].tvtx[0]) || 
			 (vb == fdb[i].vtxi[1] && tb == fdb[i].tvtx[1]) || 
			 (vb == fdb[i].vtxi[2] && tb == fdb[i].tvtx[2]) )
			count++;


		if (count == 2 && text == fdb[i].surface)
			{
			found_poly = i;
			flag[i] = 1;

			j++;

#ifdef DEBUG_STRIPS
			printf("  -> found adj side2 poly[%d] %3d,%3d,%3d, %2d,%2d,%2d\n",
				   i,
				   fdb[i].vtxi[0] ,
				   fdb[i].vtxi[1] ,
				   fdb[i].vtxi[2] ,
				   fdb[i].tvtx[0],
				   fdb[i].tvtx[1],
				   fdb[i].tvtx[2]);
#endif

			}
		}

	return(found_poly);

}







/******************************************************************************/
/*                                                                            */
/* Function: find_strip_poly()                                                */
/*                                                                            */
/******************************************************************************/


void find_strip_poly(int poly,int *flag)
{
int i;
int va;
int vb;
int k;
int d;
int j;
int vp;

/******************************************************************************/
#ifdef DEBUG_STRIPS
	printf("\n\n");
#endif

	i = poly;

#ifdef DEBUG_STRIPS
		printf("Find_max_strip poly[%d] %3d,%3d,%3d, %2d,%2d,%2d,},\n",
 				i,
			   	fdb[i].vtxi[0] ,
			   	fdb[i].vtxi[1] ,
			   	fdb[i].vtxi[2] ,
			   	fdb[i].tvtx[0],
			  	fdb[i].tvtx[1],
			   	fdb[i].tvtx[2]);
#endif

		j = 0;
		poly_strip[i][j].v = fdb[i].vtxi[0];
		poly_strip[i][j].st = fdb[i].tvtx[0];
		poly_strip[i][j].text = fdb[i].surface;
		poly_strip[i][j].new_strip = 1;

#if (FIRST_TRIANGLE_NEGATIVE)
		j++;	
		va = fdb[i].vtxi[1];
		poly_strip[i][j].v = fdb[i].vtxi[1];
		poly_strip[i][j].st = fdb[i].tvtx[1];
		poly_strip[i][j].text = fdb[i].surface;


		j++;			
		vb = fdb[i].vtxi[2];
		poly_strip[i][j].v = fdb[i].vtxi[2];
		poly_strip[i][j].st = fdb[i].tvtx[2];
		poly_strip[i][j].text = fdb[i].surface;
#else
		j++;	
		va = fdb[i].vtxi[2];
		poly_strip[i][j].v = fdb[i].vtxi[2];
		poly_strip[i][j].st = fdb[i].tvtx[2];
		poly_strip[i][j].text = fdb[i].surface;


		j++;			
		vb = fdb[i].vtxi[1];
		poly_strip[i][j].v = fdb[i].vtxi[1];
		poly_strip[i][j].st = fdb[i].tvtx[1];
		poly_strip[i][j].text = fdb[i].surface;
#endif

		k = i;
		do {

			d = find_adjacent_side2(k, va, vb, flag);
#ifdef DEBUG_STRIPS
			printf("   d: %d  k:%d  va:%d  vb:%d\n",d,k,va,vb);
#endif

			if (d != -1)
				{
				k = d;
				j++;
#ifdef DEBUG_STRIPS
				printf("    inside d: %d  k:%d  va:%d  vb:%d\n",d,k,va,vb);
#endif
				
				vp = fdb[d].vtxi[0];
				if ((vp != va) && (vp != vb))
					{
#ifdef DEBUG_STRIPS
					printf("         ==> vp:%d\n",vp);
#endif
					poly_strip[i][j].v = fdb[d].vtxi[0];
					poly_strip[i][j].st = fdb[d].tvtx[0];
					poly_strip[i][j].text = fdb[d].surface;

					va = vb;
					vb = vp;
					continue;
					}

				vp = fdb[d].vtxi[1];
				if ((vp != va) && (vp != vb))
					{
#ifdef DEBUG_STRIPS
					printf("        ==> vp:%d\n",vp);
#endif
					poly_strip[i][j].v = fdb[d].vtxi[1];
					poly_strip[i][j].st = fdb[d].tvtx[1];
					poly_strip[i][j].text = fdb[d].surface;

					va = vb;
					vb = vp;
					continue;
					}

				vp = fdb[d].vtxi[2];
				if ((vp != va) && (vp != vb))
					{
#ifdef DEBUG_STRIPS
					printf("        ==> vp:%d\n",vp);
#endif

					poly_strip[i][j].v = fdb[d].vtxi[2];
					poly_strip[i][j].st = fdb[d].tvtx[2];
					poly_strip[i][j].text = fdb[d].surface;

					va = vb;
					vb = vp;
					continue;
					}
				}

			} while(d != -1);


}


