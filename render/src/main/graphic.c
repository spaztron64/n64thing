#include <nusys.h>
#include "graphic.h"

Gfx          gfx_glist[GFX_GLIST_LEN];
Dynamic      gfx_dynamic;
Gfx*         glistp;
u32          gfx_gtask_no = 0;

/*----------------------------------------------------------------------------
  gfxRCPIinit

  The initialization of RSP/RDP
----------------------------------------------------------------------------*/
void gfxRCPInit(int hireso)
{
  /* Setting the RSP segment register  */
  gSPSegment(glistp++, 0, 0x0);  /* For the CPU virtual address  */

  /* Setting RSP */
  

  /* Setting RDP  */
  if(hireso){
	gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(setup_rspstate));
	gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(setup_rdpstate));
	
  }
  else{
	gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(setup_rspstate_lo));
	gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(setup_rdpstate_lo));
  }
}

/*----------------------------------------------------------------------------
  gfxClearCfb

  Setting addresses of the frame buffer/Z-buffer and clear them 

  Using nuGfxZBuffer (the address of the Z-buffer) and nuGfxCfb_ptr (the  
  address of the frame buffer) which are global variables of NuSYSTEM.
----------------------------------------------------------------------------*/
void gfxClearCfb(int hireso)
{
   if(hireso){
  /* Clear the Z-buffer  */
  gDPSetDepthImage(glistp++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPPipeSync(glistp++);
  gDPSetCycleType(glistp++, G_CYC_FILL);
  gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b,SCREEN_WD, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPSetFillColor(glistp++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
  gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
  gDPPipeSync(glistp++);
  
    /* Clear the frame buffer  */
  gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, osVirtualToPhysical(nuGfxCfb_ptr));
  gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
				GPACK_RGBA5551(0, 0, 0, 1)));
  gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
  gDPPipeSync(glistp++);
   }
   else{
	     gDPSetDepthImage(glistp++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPPipeSync(glistp++);
  gDPSetCycleType(glistp++, G_CYC_FILL);
  gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b,SCREEN_WD_LO, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPSetFillColor(glistp++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
  gDPFillRectangle(glistp++, 0, 0, SCREEN_WD_LO-1, SCREEN_HT_LO-1);
  gDPPipeSync(glistp++);
  
    /* Clear the frame buffer  */
  gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD_LO, osVirtualToPhysical(nuGfxCfb_ptr));
  gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
				GPACK_RGBA5551(0, 0, 0, 1)));
  gDPFillRectangle(glistp++, 0, 0, SCREEN_WD_LO-1, SCREEN_HT_LO-1);
  gDPPipeSync(glistp++);
   }
}
