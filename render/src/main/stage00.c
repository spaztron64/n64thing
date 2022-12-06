#include <assert.h>
#include <nusys.h>
#include <math.h>
#include "graphic.h"
//#include "shared.h"
//#include "Celebi.h"
#include "localdef.h"
//#include "hertz.h"
//#include "tomi/header.h"
//#include "tomi/model.inc.c"
//#include "keiki/header.h"
#include "lain/header.h"
#include "lain/model.inc.c"
//#include "keiki/model.inc.c"
#include "assets.h"

//#include "vajssiti.h"

#define     BACK_WD       640
#define     BACK_HT       440
#define     ROWS          2
#define     DELTA         1
//#include "hatsu/model.inc.c"
#include <string.h>



OSTime start_time = 0;
OSTime current_time = 0;
int frames = 0;
int fps = 0;
//u64 buf[90000];  //DisplayList buffer
u64* buf = 0x80400000;
u64* bgbuf = 0x80600000;

/* Prototype all funcs before calling*/
void draw_cube( Dynamic* dynamicp, float t);
void SetViewMtx( Dynamic* );
void debug_console_int(char *name, int variable, int pos);
void debug_console_float(char *name, float variable, int pos);
int lim(u32 input);
int hireso=1;
int antialias=1;

void    DrawBackground(void)
{
	
  
  int           i;
  static int    posx = 0, delta = DELTA;
  
  gSPSegment(glistp++, 0x02, OS_K0_TO_PHYSICAL(bgbuf));
  gDPSetCycleType(glistp++, G_CYC_COPY);
  gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
  gDPSetRenderMode(glistp++, G_RM_NOOP, G_RM_NOOP2);
  gDPSetTexturePersp(glistp++, G_TP_NONE);
  
  for(i = 20; i < (BACK_HT / ROWS); i++)
  {
    gDPLoadTextureTile(glistp++,
                       higu,
                       G_IM_FMT_RGBA,
                       G_IM_SIZ_16b,
                       BACK_WD,
                       BACK_HT,
                       posx,
                       i * ROWS,
                       posx + (SCREEN_WD - 1),
                       i * ROWS + (ROWS - 1),
                       0,
                       G_TX_WRAP, G_TX_WRAP,
                       0, 0,
                       G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++,
                        0 << 2,
                        (i * ROWS) << 2,
                        (SCREEN_WD - 1) << 2,
                        (i * ROWS + (ROWS - 1)) << 2,
                        G_TX_RENDERTILE,
                        posx << 5, (i * ROWS) << 5,
                        4 << 10, 1 << 10);
  }
  posx += delta;
  if(posx + SCREEN_WD >= BACK_WD)
  {
    posx = BACK_WD - SCREEN_WD;
    delta = -DELTA;
  }
  else if(posx <= 0)
  {
    posx = 0;
    delta = DELTA;
  }
  gDPPipeSync(glistp++);
}

int t = 1;
char conbuf[20];

// stuff about the cube
static float cubescale;
float cubepan;
float cubeyaw;

typedef struct {
  float x;
  float y;
  float z;
} Vec3;

static Vec3 campos;
static Vec3 camrot;

static Mtx squidmatrix;

NUContData contdata[1]; /* Read data of 1 controller  */

float View;

Mtx cameraRotation;
Mtx cameraPosition;


void SetViewMtx( Dynamic* dp)
{
    u16 perspNorm;

    /* The calculation and set-up of the projection-matrix  */
	if(hireso){
    guPerspective(
      &dp->projection,                      //Mtx *m
      &perspNorm,                           //u16 *perspNorm,
      50,                                   //FOV
      (float)SCREEN_WD/(float)SCREEN_HT,    //ASPECT
      10,                                   //near plane clipping
      3000,                                //far plane clipping
      1.0F                                  //matrix object scaling
    );
	}
	else{
    guPerspective(
      &dp->projection,                      //Mtx *m
      &perspNorm,                           //u16 *perspNorm,
      50,                                   //FOV
      (float)SCREEN_WD_LO/(float)SCREEN_HT_LO,    //ASPECT
      10,                                   //near plane clipping
      3000,                                //far plane clipping
      1.0F                                  //matrix object scaling
    );
	}

    
    guLookAt(
      &dp->viewing, 
      // xyz pos of camera
      campos.x,
      campos.y,
      campos.z,

      // XYZ of "looking at"
      sin(camrot.y) + campos.x, 
      -sin(camrot.x)*cos(camrot.y) + campos.y, 
      cos(camrot.x)*cos(camrot.y) + campos.z,

      // "up" direction, never change
      0, 1, 0 
    );

        
    gSPPerspNormalize(glistp++, perspNorm);
    gSPLookAt(glistp++, &dp->viewing);

    gSPMatrix(glistp++, &(dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &(dp->viewing), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);

}

void initStage00()
{
	u32 seg_start = (u32)_tomiSegmentRomStart;
	u32 seg_size = _tomiSegmentRomEnd - _tomiSegmentRomStart;
	nuPiReadRom(seg_start, buf, seg_size);
	
	u32 bgseg_start = (u32)_vajssitiSegmentRomStart;
	u32 bgseg_size = _vajssitiSegmentRomEnd - _vajssitiSegmentRomStart;
	nuPiReadRom(bgseg_start, bgbuf, bgseg_size);
	
  nuDebConDisp(NU_SC_SWAPBUFFER);
  cubescale = 1;
  cubepan = 0;
  cubeyaw = 0;
  campos = (Vec3){.x = 0, .y = 0, .z = 0};
  camrot = (Vec3){.x = 0, .y = 0, .z = 0};
  View = 0;
}

void makeDL00(void)
{
  
  
  /* Specify the display list buffer  */
  glistp = gfx_glist;
  
  /*  The initialization of RCP  */
  gfxRCPInit(hireso);

  /* Clear the frame buffer and the Z-buffer  */
  gfxClearCfb(hireso);

  SetViewMtx(&gfx_dynamic);

  // guRotate(&gfx_dynamic.modeling, 0.0F, 0.0F, 0.0F, 0.0F);

  /* Draw a square  */
  draw_cube(&gfx_dynamic,t);

  /* End the construction of the display list  */
  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  /* Check if all are put in the array  */
  assert(glistp - gfx_glist < GFX_GLIST_LEN);

  /* Activate the RSP task.  Switch display buffers at the end of the task. */
  nuGfxTaskStart(&gfx_glist[0],(s32)(glistp - gfx_glist) * sizeof (Gfx), NU_GFX_UCODE_F3DEX2, NU_SC_NOSWAPBUFFER);
  current_time = (OS_CYCLES_TO_NSEC(osGetTime())) / 1000;
  //debug_console_int("curnt_time",current_time,0);
  debug_console_int("AA",antialias,0);
  frames++;
  if(current_time >= 1000000){
	  osSetTime(0);
	  fps = frames;
	  frames = 0;
  }


  /* DEBUG CONSOLE!!!!!!!!!!!! Display characters on the frame buffer, debug console only */
  nuDebConDisp(NU_SC_SWAPBUFFER);
  
  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);
  debug_console_int("frames",frames,1);
  debug_console_int("fps",fps,2);
  //debug_console_int("texloads",1,3);
  //debug_console_int("triangles",1355,4); //tommy
  //debug_console_int("texloads",25,3);
  //debug_console_int("triangles",24262,4); //hatsu
  ////debug_console_int("texloads",28,2);
  ////debug_console_int("triangles",23434,3); //keiki
  
}

/* Draw object  */
void draw_cube(Dynamic* dynamicp, float t)
{
  int i = 0;
  
  /* Create matrices for mult */
  /* CUBE IS AT CENTER OF EARTH */
  guTranslate(&dynamicp->pos,0,0,-100);
  guRotate(&dynamicp->rotx,cubepan,1,0,0);
  guRotate(&dynamicp->roty,cubeyaw,0,1,0);
  guScale(&dynamicp->scale,cubescale,cubescale,cubescale);
 
  /* apply transformation matrices, to stack */
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->pos)),   G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH );
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->scale)), G_MTX_MODELVIEW | G_MTX_MUL  | G_MTX_NOPUSH );
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->rotx)),  G_MTX_MODELVIEW | G_MTX_MUL  | G_MTX_NOPUSH );
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->roty)),  G_MTX_MODELVIEW | G_MTX_MUL  | G_MTX_NOPUSH );

  /* Rendering setup */
  gDPSetFillColor(glistp++, GPACK_RGBA5551(0,0,0,1)<<16 | GPACK_RGBA5551(0,0,0,1));
  gDPFillRectangle(glistp++, 0, 0, 640, 480);
  
  //DrawBackground();
  
  gDPSetTexturePersp(glistp++, G_TP_PERSP);
  if(antialias){
	gDPSetRenderMode(glistp++,G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
  }
  else{
	gDPSetRenderMode(glistp++,G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);

  }
  gSPTexture(glistp++,0x8000, 0x8000, 0, 0, G_ON);
  gDPSetCycleType(glistp++, G_CYC_1CYCLE);
  gDPSetCombineMode(glistp++,G_CC_DECALRGBA, G_CC_DECALRGBA);
  gDPSetTextureFilter(glistp++, G_TF_BILERP);
  gSPClearGeometryMode(glistp++,0xFFFFFFFF);
  //gSPSetGeometryMode(glistp++, G_ZBUFFER | G_LIGHTING | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_CLIPPING);
  gSPSetGeometryMode(glistp++, G_ZBUFFER | G_LIGHTING | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_CLIPPING);

  /* DRAW OBJECT 
  ====================================================
  ====================================================*/ 
  //gSPDisplayList(glistp++, Wtx_Celebi);
  gSPSegment(glistp++, 0x01, OS_K0_TO_PHYSICAL(buf));
  //gSPDisplayList(glistp++, tomi_Tommy_mesh);
  //gSPDisplayList(glistp++, hatsu_xiaoyu_mesh);
  //gSPDisplayList(glistp++, keiki_keiki_mesh);
  gSPDisplayList(glistp++, lain_lain_mesh_mesh);
  /*=================================================
  ====================================================*/

  //gDPFillRectangle(glistp++, 0, 0, 640, 40);
  //gDPFillRectangle(glistp++, 0, 440, 640, 480);

  /* Finalise and exit drawing */
  gSPTexture(glistp++,0, 0, 0, 0, G_OFF);
  gDPPipeSync(glistp++);
  
}

/* Just controls and changes vars for debugging */
void updateGame00()
{

  /* Data reading of controller 1 */
  nuContDataGetEx(contdata,0);

  cubeyaw += contdata->stick_x*0.05;
  cubepan -= contdata->stick_y*0.05;
  
  /* Really basic controls for debugging purposes */
  if(contdata[0].button & START_BUTTON){
    t += 1;
	if(hireso){
		hireso=0;
		
		//gfxRCPInit(hireso);	
		//gfxClearCfb(hireso);
		nuGfxTaskAllEndWait();
		osViSetMode(&osViModeTable[OS_VI_NTSC_LAN1]);
		//nuGfxDisplayOff();
		//nuGfxInit();
		//nuGfxSetCfb(LowFrameBuf, 3);
		//nuGfxDisplayOn();
	}
	else{
		hireso=1;
		nuGfxTaskAllEndWait();
		osViSetMode(&osViModeTable[OS_VI_NTSC_HAN1]);
		//nuGfxDisplayOff();
		//nuGfxInit();
		//nuGfxSetCfb(HighFrameBuf, 2);
		//nuGfxDisplayOn();
		//gfxRCPInit(hireso);
		//gfxClearCfb(hireso);
		//nuGfxInit();
	}
	osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
  osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
  }

  if(contdata[0].button & A_BUTTON){
    cubescale += 0.03;
  }
  if(contdata[0].button & B_BUTTON){
    cubescale -= 0.03;
  }
  if(contdata[0].button & L_TRIG){
    cubescale += 0.005;
  }
  if(contdata[0].button & R_TRIG){
    cubescale -= 0.005;
  }
  if(contdata[0].button & Z_TRIG){
    if(antialias){
		antialias=0;
	}
	else{
		antialias=1;
	}
  }

  // UP/DOWN CAM ROTATION CONTROLS
  camrot.y += 0.06 * ( lim(contdata[0].button & L_CBUTTONS) - lim(contdata[0].button & R_CBUTTONS) );
  camrot.x += 0.06 * ( lim(contdata[0].button & U_CBUTTONS) - lim(contdata[0].button & D_CBUTTONS) );
  campos.x += 1.56 * ( lim(contdata[0].button & L_JPAD) - lim(contdata[0].button & R_JPAD) );
  campos.y += 1.56 * ( lim(contdata[0].button & U_JPAD) - lim(contdata[0].button & D_JPAD) );

}

/* UTILITIES */
void debug_console_int(char *name, int variable, int pos)
{
  nuDebConTextPos(0,1,1*pos);
  sprintf(conbuf,"%s=%02i",name,variable);
  nuDebConCPuts(0, conbuf);
}

void debug_console_float(char *name, float variable, int pos)
{
  nuDebConTextPos(0,1,1*pos);
  sprintf(conbuf,"%s=%f",name,variable);
  nuDebConCPuts(0, conbuf);
}

int lim(u32 input) {
  if(input == 0) {
    return 0;
  }else{
    return 1;
  }
}

