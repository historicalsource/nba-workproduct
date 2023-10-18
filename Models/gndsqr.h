VERTEX gndsquare_obj_vtx[] =
{
	{-7.0f,0.0f,7.0f},
	{-7.0f,0.0f,-7.0f},
	{7.0f,0.0f,7.0f},
	{7.0f,0.0f,-7.0f}
};

#ifdef JONHEY
ST gndsquare_obj_st[] =
{
	{0.0f * 256.0F,1.0f * 256.0F},
	{0.0f * 256.0F,0.0f * 256.0F},
	{1.0f * 256.0F,1.0f * 256.0F},
	{1.0f * 256.0F,0.0f * 256.0F}
};
#else
ST gndsquare_obj_st[] =
{
	{0.0f,1.0f},
	{0.0f,0.0f},
	{1.0f,1.0f},
	{1.0f,0.0f}
};
#endif

#ifdef JONHEY
TRI gndsquare_obj_tris[] =
{
	{0,1,2, 0,1,2, 0},
	{3,2,1, 3,2,1, -12},
	{0, 0, 0, 0, 0, 0, -20}
};
#else
TRI gndsquare_obj_tris[] =
{
	{0,3,6, 0,1,2, 0},
	{9,6,3, 3,255,255, -12},
	{0, 0, 0, 0, 0, 0, -20}
};
#endif

VERTEX gndsquare_obj_bbox[] =
{
	{-7.0f, 0.0f, -7.0f},
	{-7.0f, 0.0f,  7.0f},
	{-7.0f, 0.0f, -7.0f},
	{-7.0f, 0.0f,  7.0f},
	{ 7.0f, 0.0f, -7.0f},
	{ 7.0f, 0.0f,  7.0f},
	{ 7.0f, 0.0f, -7.0f},
	{ 7.0f, 0.0f,  7.0f}
};

LIMB limb_gndsquare_obj = 
{
	4,				/* vertex count */
	2,				/* triangle count */
	NULL,	/* vertex normals */
	gndsquare_obj_vtx,	/* vertices */
	gndsquare_obj_st,	/* texture coordinates */
	gndsquare_obj_tris,	/* triangles */
	NULL, /* bounding box */
	NULL	/* strip */
};
