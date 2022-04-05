#include <nusys.h>
#include "main.h"
#include "localdef.h"
#include "graphic.h"

/* Declaration of the prototype, STAGE 0 */
void stage00(int);
/*extern prototypes*/
void makeDL00(void);
void initStage00(void);
void updateGame00(void);
int mult = 1;

/* High resolution frame buffer address array. */
u16* HighFrameBuf[2] = {
    (u16*)CFB_HIGH_ADDR0,
    (u16*)CFB_HIGH_ADDR1
};

/* The global variable  */
NUContData contdata[1]; /* Read data of 1 controller  */
u8 contPattern;        /* The pattern connected to the controller  */

/*------------------------
	Main
--------------------------*/
void mainproc(void)
{

  /* The initialization of graphic  */
  nuGfxInit();

  contPattern = nuContInit();


  osCreateViManager(OS_PRIORITY_VIMGR);
  osViSetMode(&osViModeTable[OS_VI_NTSC_HAN1]); 
  osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
  osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
  
  nuGfxDisplayOff();
  nuGfxSetCfb(HighFrameBuf, 2);
  nuGfxSetZBuffer((u16*)(ZBUFFER_ADDR));
  
      nuDebConClear(0);
    nuDebConTextAttr(0, NU_DEB_CON_ATTR_BLINK);
    nuDebConTextPos(0, 13,25);
    nuDebConTextColor(0, NU_DEB_CON_TEXT_RED);
    nuDebConPuts(0, "High Resolution");
    nuDebConTextAttr(0, NU_DEB_CON_ATTR_NORMAL);
  /* Init scene */
  initStage00();

  /* Set call-back  */
  nuGfxFuncSet((NUGfxFunc)stage00);

  /* The screen display ON */
  nuGfxDisplayOn();

  while(1){}
}

/*-----------------------------------------------------------------------------
  The call-back function 

  pendingGfx which is passed from Nusystem as the argument of the call-back 
  function is the total number of RCP tasks that are currently processing 
  and waiting for the process. 
-----------------------------------------------------------------------------*/

void stage00(int pendingGfx)
{
  /* It provides the display process if there is no RCP task that is processing. */
  if(pendingGfx < 2){
    makeDL00();	
  }
  updateGame00();

}

