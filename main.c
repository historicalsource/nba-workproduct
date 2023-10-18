/*
 *		$Archive: /video/Nba/main.c $
 *		$Revision: 28 $
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
char *ss_main_c = "$Workfile: main.c $ $Revision: 28 $";
#endif

#define PREALLOCATE_BLOCKS		/* define PREALLOCATE_BLOCKS to preallocate process, object, sprites, and texures */
/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <glide/sst1init.h>

/*
 *		USER INCLUDES
 */

#include "main.h"
#include "system.h"
#include "audits.h"
#include "debug.h"
#include "mainproc.h"
#include "getgaddr.h"
#include "inthand.h"
#include "ani3d.h"

/*
 *		DEFINES
 */

/* #define BANSHEE_55 */		/* define to set refresh rate to 55 Hz (instead of 57 Hz) */

/* size of the buffers used to DMA data to the rendering engine, 1.5 Meg */
#define	DMA_BUFFER_SIZE					(1024 * 1024)

/* floating point exceptions */
#define	FPU_CAUSE_SPECIAL				(0<<16)
#define	FPU_CAUSE_COP1					(1<<16)
#define	FPU_CAUSE_COP1X_MADDS			(2<<16)
#define	FPU_CAUSE_COP1X_NMADDS			(3<<16)
#define	FPU_CAUSE_COP1X_MSUBS			(4<<16)
#define	FPU_CAUSE_COP1X_NMSUBS			(5<<16)
#define	FPU_CAUSE_COP1X_MADDD			(6<<16)
#define	FPU_CAUSE_COP1X_MSUBD			(7<<16)
#define	FPU_CAUSE_COP1X_NMADDD			(8<<16)
#define	FPU_CAUSE_COP1X_NMSUBD			(9<<16)
#define	FPU_CAUSE_COP1X_LWXC1			(10<<16)
#define	FPU_CAUSE_COP1X_LDXC1			(11<<16)
#define	FPU_CAUSE_COP1X_SWXC1			(12<<16)
#define	FPU_CAUSE_COP1X_SDXC1			(13<<16)
#define	FPU_CAUSE_COP1X_PREFIX			(14<<16)
#define	FPU_CAUSE_COP1X_UNRECOGNIZED	(15<<16)
#define	FPU_CAUSE_LWC1					(16<<16)
#define	FPU_CAUSE_LDC1					(17<<16)
#define	FPU_CAUSE_SWC1					(18<<16)
#define	FPU_CAUSE_SDC1					(19<<16)
#define	FPU_CAUSE_NOT_FPU_INSTRUCTION	(20<<16)

/*
 *		STATIC PROTOTYPES
 */

static void show_banner(void);
static void graphics_init(void);
#if defined(SEATTLE)
static int reboot_vector(int cause, int *regs);
static void set_reboot_vectors(void);
static void int_div0_handler(int cause, int *regs);
static void int_divo_handler(int cause, int *regs);
void tlb_refill_handler(void);
void install_tlb_handler(void(*)(void));
int nba_vblank_handler(void);
void install_int_div0_handler(void (*func)(int cause, int *regs));
void install_int_divo_handler(void (*func)(int cause, int *regs));
void dump_stack_trace(void);
#endif
#if 0
int stream_detect_fifos(void);
#endif

#if defined(VEGAS)
float	hres = 512.0f;
float	vres = 384.0f;
#endif

/*
 *		STATIC VARIABLES
 */

/* generate the a dummy to get the ending address of the text section */
char __tend[] __attribute__((aligned (8), section ("textend"))) = {};

#if defined(SEATTLE)
/* register used as the pointer to the DMA buffers */
register char *cur_dma_ptr asm("$28");

static sst1VideoTimingStruct myVideoResolution = {
	25,			/* hSyncOn */
	636,		/* hSyncOff */
	5,			/* vSyncOn */
	434,		/* vSyncOff */
	90,			/* hBackPorch */
	35,			/* vBackPorch */
	512,		/* xDimension */
	384,		/* yDimension */
	96,			/* memOffset */
	0x0,		/* memFifoEntries_1MB */
	0x100,		/* memFifoEntries_2MB */
	0x100,		/* memFifoEntries_4MB */
	8,			/* tilesInX_Over2 */
	23,			/* vFifoThreshold */
	FXTRUE,		/* video16BPPIsOk */
	FXTRUE,		/* video24BPPIsOk */
	16.5161,	/* clkFreq16bpp */
	33.0323		/* clkFreq24bpp */
};
#endif

int reload = 0;
int p_active = 0;

static const char *banner_strs[] = {"\n\n",
									"                           Mother GOOSE - R5000",
#if defined(SA1)
									"                                Phoenix SA1",
#elif defined(SEATTLE)
									"                              Phoenix Seattle",
#elif defined(FLAGSTAFF)
									"                             Phoenix Flagstaff",
#elif defined(VEGAS)
									"                               Phoenix Vegas",
#endif
									"                              $Revision: 28 $",
									"\n",
									"                   Copyright (c) 1997-1998 by Midway Games Inc.",
									"                 Portions Copyright (c) 1994 by DJ Delorie.",
									"            Portions Copyright (c) 1996 by 3DFx Interactive Inc.",
									"      Portions Copyright (c) 1996 by Integrated Device Technologies Inc.",
									"   Portions Copyright (c) 1983 by Regents of the University of California.",
									"\n\n"
};

/*
 *		GLOBAL FUNCTIONS
 */

register unsigned int sp asm("$29");

void main(int *args)
{
	ulong start_addr;
	ulong num_bytes;
	unsigned int save_sp;

	reload = args[0];
	p_active = args[1];
//reload=1;	// testing
		
	save_sp = sp;
	
	/* get the starting address of the text section */
	start_addr = get_text_addr();

	/* get the ending address of the text section */
	num_bytes = get_text_end_addr();

	/* figure out how many bytes there are */
	num_bytes -= start_addr;

	/* generate a CRC for the text section */
	start_crc32 = crc((uchar *)start_addr, num_bytes);

	/* Print a long string so <write> will (hopefully) malloc a buffer */
	/* that will handle anything & thereby not frag the heap!          */
	fprintf(stderr,
		"Welcome to NBA Showtime!\r\n"
		"------------------------\r\n"
		"We are currently initializing the environment within which this\r\n"
		"product will attempt to execute. This string of message-text is\r\n"
		"now being displayed in order for the handler to malloc a buffer\r\n"
		"large enough (hopefully) to contain the text of any printf that\r\n"
		"follows. This is to avoid an inopportune realloc of said buffer\r\n"
		"that would perhaps throw a substantial frag on the memory heap.\r\n"
		"\r\n"
		"And for your information, the TEXT section CRC equals %08X.\r\n\r\n"
		, (int)start_crc32);

	fprintf(stderr, "The STACK POINTER is %s (%08x)\n", (save_sp < 0x807FC000) ? "BAD" : "OK", save_sp);
	
	/* set possible use of 49 way joysticks */
	set_49way(1);

#if defined(SEATTLE)
	/* open the GT64010 system controller device */
	gt_fd = open("pci:", O_RDWR | O_BINARY);
	if (gt_fd == -1)
		EXITF(("Can not open the system controller device\n"));
	
	/* show the banner */
	show_banner();
	
	/* allocate memory for the DMA buffers */
	dma_buffer1 = malloc((DMA_BUFFER_SIZE * 2) + 32);
	if (dma_buffer1 == NULL)
		EXITF(("Can not allocate memory for DMA buffers\n"));
	
	/* Adjust the dma buffer pointer such that it aligned on a cache line
	 * This is safe because these buffers are never freed (or they should
	 * never be freed) */
	while (((int)dma_buffer1 & 0x1F) != 0)
		dma_buffer1++;

	/* set the pointer for the second dma buffer */
	dma_buffer2 = dma_buffer1 + DMA_BUFFER_SIZE;
	
	/* Adjust the pointers to be mapped
	 * If you are looking at this code and wondering what is going on here,
	 * this is the straight poop.  The BIOS rom sets up 32 entries (64 pages)
	 * of the TLB (translation lookaside buffer) to map virtual addresses
	 * 0 to 8 Meg to physical addresses 0 to 8 Meg in 256k chunks.  TLB entries
	 * 32 to 48 are setup to map virtual addresses 8 - 12 Meg to physical
	 * addresses 8 - 12 meg.  TLB entries 1 through 31 are set up to be
	 * cached non-coherent cacheable, whereas, all other TLB entries are set up
	 * to be NOT accessible from the processor.  Setting TLB entry 0 to be
	 * NOT accessible does 2 things.  First, it protects the debugger stub and
	 *  BIOS code (loaded from the ROM) from accidently be overwritten by
	 * a dereferenced write to a NULL pointer.  Secondly, it will cause a
	 * TLB exception to be thrown if a data reference to a NULL pointer is
	 * attempted.
	 *
	 * Now what is going on here is really an optimization used for scatter
	 * gather type DMA devices.  DMA devices with scatter gather capability
	 * require physical addresses to be used.  By mapping virtual addresses
	 * to physical address the way it has been done, there is no need to do
	 * a virtual to physical address conversion on addresses used by such
	 * devices (as long as allocated addresses are preconverted).
	 */
	(int)dma_buffer1 &= 0x1FFFFFFF;
	(int)dma_buffer2 &= 0x1FFFFFFF;
#endif
	
	/* initialize the switch handling system */
	switch_init();
	
	/* Do not ignore switches that were closed at powerup */
	clear_stuck_psw();
	
	/* initialize the sprite handling system */
	init_sprites();
	
	/* initialize the graphics library */
	graphics_init();
	
#if defined(SEATTLE)
	/* install the hardware interrupt handler */
	install_interrupt_handler(VERTICAL_RETRACE_HANDLER, nba_vblank_handler);
#endif

#if 0
	if (stream_detect_fifos()) {
#if defined(SEATTLE)
		/* Make sure that the I/O ASIC interrupts are off. */
		inthand_disable_ioasic_interrupt();
		
	 	/* Install the I/O ASIC hardware interrupt handler */
		install_interrupt_handler(IOASIC_HANDLER, interrupt_ioasic_handler);
		
		/* enable I/O ASIC interrupts */
		inthand_enable_ioasic_interrupt();
#endif
	}
#endif

#if defined(SEATTLE)	
	/* install the galileo dma channel 0 handler */
	install_sys_handler(4, galileo_dma0_handler);
	
	/* install handler for TLB refill */
	install_tlb_handler(tlb_refill_handler);
	
#ifndef DEBUG
	/* install the interrupt handler for timer 0 of the GT64010 */
	install_sys_handler(11, galileo_timer3_handler);
	*((volatile int *)0xAC000C1C) |= 0x800;
#endif
	
	/* enable PCI bus parity error detection */
	*((volatile int *)0xAC000C1C) |= 0xC000;
	
	/* install the divide by 0 exception handler */
	install_fpu_handler(FPU_EXC_DIV0, div0_handler);
	
	/* install the unimplemented operation exception handler */
	install_fpu_handler(FPU_EXC_UNIMP, unimplemented_handler);
	
	/* install the integer divide by 0 handler */
	install_int_div0_handler(int_div0_handler);
	
	/* install the integer divide overlow handler */
	install_int_divo_handler(int_divo_handler);
#endif

#if 0
	/* initialize the state of the joyport controls */
	if (joyplay_init()) {
		unsigned char optos;
		
		jp_status(&optos);
		if ((optos & JP_OPTO_CLOSE) && !(optos & JP_OPTO_OPEN))
			joyplay_do_function(JPFUNC_DOOR_CLOSE2);
		joyplay_do_function(JPFUNC_RESET_STATES);
	}
#endif

	config_textures_alloc( FALSE );

#ifdef PREALLOCATE_BLOCKS
	/* pre allocate goose stuff */
	prealloc_processes(60);
	prealloc_objects(30);
	prealloc_textures(230);
	prealloc_sprites(600);
#endif

	/* create the main process */
	qcreate("main", 0, main_proc, 0, 0, 0, 0);
	
#if defined(SEATTLE)
#ifdef DEBUG
	/* allow lockups to occur */
	enable_lockup();
	enable_tlb_dump();
#else
	/* disable lockups */
	disable_lockup();
	set_reboot_vectors();
#endif
#endif
	
	/* start the process dispatcher - NOTE: This NEVER exits */
	DEBUGF(("Starting process dispatcher\n"));
	process_dispatch();
	
	/* process_dispatch should NEVER return */
	EXITF(("process_dispatch returned\n"));
}	/* main */

/*
 *		STATIC FUNCTIONS
 */

/* function used to print out the copyright notice */
static void show_banner(void)
{
	int	i;

	for (i = 0; i < ARRAY_SIZE(banner_strs); i++)
		fprintf(stderr, "%s\n", banner_strs[i]);
}	/* show_banner */

#if defined(VEGAS)
void resetTextureSystem(void);
void enable_write_merge(void);
unsigned int grSstScreenWidth(void);
unsigned int grSstScreenHeight(void);
#endif

static void graphics_init(void)
{
#if defined(SEATTLE)
	GrHwConfiguration hwconfig;
	sst1VideoTimingStruct *vt;
	uint dip;
	
	/* initialize the dam buffer to use */
	cur_dma_ptr = dma_buffer1;
	
	sst1InitGrxClk_Called = 0;
	
	/* read the dip switches to determine the screen resolution */
	dip = ~read_dip_direct() << DIP_SHIFT;
	switch (((dip_inputs_t *)&dip)->resolution) {
	/* 512x384 57hz (Mid Res) */
	default:
	case DIP_RESOLUTION_512X384:
		DEBUGF(("Setting 512x384 57Hz screen resolution\n"));
		vt = &myVideoResolution;
		tsec = 57;
		is_low_res = FALSE;
		break;
		
	/* 512x256 57hz (Lo Res2) */
	case DIP_RESOLUTION_512X256:
		DEBUGF(("Setting 512x256 57Hz screen resolution\n"));
		vt = &SST_VREZ_512X256_57;
		tsec = 57;
		is_low_res = TRUE;
		break;
		
#ifdef DEBUG
	/* 512 x 384 72hz (VGA) */
	case DIP_RESOLUTION_VGA:
		DEBUGF(("Setting 512x384 72Hz screen resolution\n"));
		vt = &SST_VREZ_512X384_72;
		tsec = 72;
		is_low_res = FALSE;
		break;
		
	/* 400x256 54Hz (Lo Res1) */
	case DIP_RESOLUTION_400X256:
		DEBUGF(("Setting 400x256 72Hz screen resolution\n"));
		vt = &SST_VREZ_400X256_54;
		tsec = 54;
		is_low_res = TRUE;
		break;
#endif
	}

	/* set up video timimg based on timing structure values */
	grSstVidMode(0, vt);
	
	/* initialize the library */
	grGlideInit();
	
	/* figure out what hardware is really out there */
	if (grSstQueryHardware(&hwconfig) == 0)
		EXITF(("graphics_init(): Graphics Hardware initialization failure\n"));
	
	/* select graphics card 0 */
	grSstSelect(0);
	
	/* open the graphics library and set resolution - NOTE:  The values passed */
	/* here are ignored because the grSstVideMode() call is used above. */
	if (grSstOpen(GR_RESOLUTION_400x256, GR_REFRESH_54Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, GR_SMOOTHING_ENABLE, 2) == 0)
		EXITF(("grSstOpen(): ERROR graphics subsystem initialization failure\n"));
	
	/* set the global horizontal and vertical resolution variables */
	hres = vt->xDimension;
	vres = vt->yDimension;
	
	/* initialize the state of the rendering engine */
	grBufferClear(0xFF000000, 0, GR_WDEPTHVALUE_FARTHEST);
	grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_CCRGB);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthMask(FXTRUE);
	
	/* get number of ticks per second as set by glide library */
	tsec = get_tsec();
#else

	GrScreenResolution_t resolution = GR_RESOLUTION_512x384;
	unsigned short			status;

	status = read_dip_direct();
	if(!(status & 0x0200))
	{
		resolution = GR_RESOLUTION_512x256;
		is_low_res = 1;
	}
	else
	{
		resolution = GR_RESOLUTION_512x384;
		is_low_res = 0;
	}

	/* set up res-dependent stuff/indirections */
	init_geom( is_low_res );
	
	/* Initialize Glide */
	grGlideInit();

	grVertexLayout(GR_PARAM_XY,  GR_VERTEX_X_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_RGB, GR_VERTEX_R_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_A,   GR_VERTEX_A_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Z,   GR_VERTEX_OOZ_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_W,   GR_VERTEX_OOW_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST0, GR_VERTEX_SOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q0,  GR_VERTEX_OOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST1, GR_VERTEX_SOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q1,  GR_VERTEX_OOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
   
    
	grSstSelect( 0 );
	grSstWinOpen(0, resolution, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1 );
	
	grRenderBuffer(GR_BUFFER_BACKBUFFER);

	// Initialize the state of the rendering engine
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);

	configTextureSystem( -1, -1, -1, -1, 0, 1 );
	resetTextureSystem();

	enable_write_merge();

	hres = (float)grSstScreenWidth();
	vres = (float)grSstScreenHeight();
	tsec = 57;
#endif
}	/* graphics_init */


#if defined(SEATTLE)
int reboot_vector(int cause, int *regs)
{
	int	fd;
	void (*cold_start)(void) = (void (*)(void))0xBFC00000;
	
	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if (fd >= 0 && fd < 10) {
		set_audit(CAUSE_AUD + (fd << 1), cause);
		set_audit(PC_AUD + (fd << 1), regs[PC]);
		/* set_audit(AT_AUD, regs[GP1]); */
		set_audit(V0_AUD, regs[GP2]);
		set_audit(V1_AUD, regs[GP3]);
		set_audit(A0_AUD, regs[GP4]);
		set_audit(A1_AUD, regs[GP5]);
		set_audit(A2_AUD, regs[GP6]);
		set_audit(A3_AUD, regs[GP7]);
		set_audit(T0_AUD, regs[GP8]);
		set_audit(T1_AUD, regs[GP9]);
		set_audit(T2_AUD, regs[GP10]);
		set_audit(T3_AUD, regs[GP11]);
		set_audit(T4_AUD, regs[GP12]);
		set_audit(T5_AUD, regs[GP13]);
		set_audit(T6_AUD, regs[GP14]);
		set_audit(T7_AUD, regs[GP15]);
		set_audit(T8_AUD, regs[GP24]);
		set_audit(T9_AUD, regs[GP25]);
		set_audit(S0_AUD, regs[GP16]);
		set_audit(S1_AUD, regs[GP17]);
		set_audit(S2_AUD, regs[GP18]);
		set_audit(S3_AUD, regs[GP19]);
		set_audit(S4_AUD, regs[GP20]);
		set_audit(S5_AUD, regs[GP21]);
		set_audit(S6_AUD, regs[GP22]);
		set_audit(S7_AUD, regs[GP23]);
		set_audit(S8_AUD, regs[GP30]);
		/* set_audit(K0_AUD, regs[GP26]); */
		/* set_audit(K1_AUD, regs[GP27]); */
		/* set_audit(GP_AUD, regs[GP28]); */
		set_audit(SP_AUD, regs[GP29]);
		set_audit(RA_AUD, regs[GP31]);
		set_audit(CURPROC_AUD, (int)(cur_proc->entry_func));
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(TLB_MOD_AUD + ((cause >> 2) & 0x1f) - 1);
	if ((fd = open("wdog:", O_RDONLY, 0)) < 0) {
		/* Do a cold restart */
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	for (;;)
		;
}	/* reboot_vector */

void set_reboot_vectors(void)
{
	install_vector(1, reboot_vector);	// TLB Modification
	install_vector(2, reboot_vector);	// TLB load or fetch
	install_vector(3, reboot_vector);	// TLB store
	install_vector(4, reboot_vector);	// Address error load or fetch
	install_vector(5, reboot_vector);	// Address error store
	install_vector(6, reboot_vector);	// Bus error fetch
	install_vector(7, reboot_vector);	// Bus error data reference
	install_vector(8, reboot_vector);	// System call
	/* install_vector(9, reboot_vector); */	// Break
	install_vector(10, reboot_vector);	// Reserved instruction
	install_vector(11, reboot_vector);	// Coprocessor unusable
	install_vector(12, reboot_vector);	// Arithmetic overflow (integer)
	install_vector(13, reboot_vector);	// Trap
	install_vector(14, reboot_vector);	// Reserved exception
	/* install_vector(15, reboot_vector); */	// FPU Exception
}	/* set_reboot_vectors */

void int_div0_handler(int cause, int *regs)
{
#ifdef DEBUG
	fprintf(stderr, "Integer Divide by 0\r\n");
#else
	int	fd;
	void (*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if (fd >= 0 && fd < 10) {
		set_audit(CAUSE_AUD+(fd<<1), cause);
		set_audit(PC_AUD+(fd<<1), regs[PC]);
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(INTEGER_DIV0_AUD);
	if ((fd = open("wdog:", O_RDONLY, 0)) < 0) {
		/* Do a cold restart */
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	for (;;)
		;
#endif
}	/* int_div0_handler */

void int_divo_handler(int cause, int *regs)
{
#ifdef DEBUG
	fprintf(stderr, "Integer Divide overflow\r\n");
#else
	int	fd;
	void (*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if (fd >= 0 && fd < 10) {
		set_audit(CAUSE_AUD+(fd<<1), cause);
		set_audit(PC_AUD+(fd<<1), regs[PC]);
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(INTEGER_OVERFLOW_AUD);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0) {
		/* Do a cold restart */
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	for (;;)
		;
#endif
}	/* int_divo_handler */

#ifndef DEBUG
#ifndef VERSION11
void do_buffer_swap_reboot(void)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(BUFFER_SWAP_TIMEOUT_AUD);
	if ((fd = open("wdog:", O_RDONLY, 0)) < 0) {
		/* Do a cold restart */
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	for (;;)
		;
}	/* do_buffer_swap_reboot */

void do_dma_timeout_reboot(void)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(DMA_TIMEOUT_AUD);
	if ((fd = open("wdog:", O_RDONLY, 0)) < 0) {
		/* Do a cold restart */
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	for (;;)
		;
}	/* do_dma_timeout_reboot */

void fp_div0_reboot(void)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(DIV0_UNHANDLED_AUD);
	if ((fd = open("wdog:", O_RDONLY, 0)) < 0) {
		/* Do a cold restart */
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	for (;;)
		;
}	/* fp_div0_reboot */
#endif
#endif

extern int pcount;
extern int tick_counter;
static int local_epc;

int nba_vblank_handler(void)
{
	static int last_pcount = -1;
	static int last_tick_count = 0;
	int fd;
	register long pc asm("$8");
	
	/* call the goose vblank handler */
	interrupt_handler();
	
#ifndef DEBUG
	/* if the process system hasn't fired in the last */
	/* twenty seconds, freeze here to trigger watchdog */
	if (pcount == last_pcount) {
		if (tick_counter - last_tick_count > (tsec * 20)) {
			fprintf(stderr, "VBLANK detects frozen proc loop.  Halting.\n");
			increment_audit(VBLANK_FREEZE_AUD);
			fprintf( stderr, "cur_proc: %s\n", cur_proc->process_name );
			
			dump_stack_trace();
			
			asm("	mfc0	$8,$14");
			local_epc = pc;
			
			fprintf( stderr, "PC: %x\n", local_epc );
			get_audit(WATCHDOG_AUDIT_NUM, &fd);
			increment_audit(WATCHDOG_AUDIT_NUM);
			if(fd >= 0 && fd < 10) {
				set_audit(CAUSE_AUD+(fd<<1), 0xabacab);
				set_audit(PC_AUD+(fd<<1), local_epc);
			}
			
			for (;;)
				;
		}
	} else {
		last_pcount = pcount;
		last_tick_count = tick_counter;
	}
#endif
	return 1;
}	/* nba_vblank_handler */

void tlb_refill_handler(void)
{
	int		fd;
	register long	pc asm("$8") = 0;
	
	asm("	mfc0	$8,$14");
	local_epc = pc;
	
	fprintf( stderr, "PC: %x\n", local_epc );
	
	increment_audit(TLB_REFILL_AUD);
	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if (fd >= 0 && fd < 10) {
		set_audit(CAUSE_AUD+(fd<<1), 0xace);
		set_audit(PC_AUD+(fd<<1), local_epc);
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
#ifdef DEBUG
	lockup();
#else
	for (;;)
		;
#endif
}	/* tlb_refill_handler */
#endif

#if defined(VEGAS)
int get_tsec(void)
{
#ifdef BANSHEE
	return(55);
#else
	return(57);
#endif
}
#endif
