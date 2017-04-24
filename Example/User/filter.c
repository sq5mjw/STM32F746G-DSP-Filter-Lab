/**
  ******************************************************************************
  *  * @modified by SQ5MJW 23-April-2017 v0.83
  * @file    BSP/Src/filter.c
  * @original author  MCD Application Team
  * @version 0.5
  * @date    30-December-2016 
  * @brief   This example code shows how to use the audio feature in the
  *          stm32746g_discovery driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include "arm_math.h"
//#include "arm_math.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF = 1,
  BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

#define VOLFIR 60
#define VOLIIR 74

#define AUDIO_BLOCK_SIZE   ((uint32_t)32)
#define SIGNAL_SAMPLES		AUDIO_BLOCK_SIZE/2
#define AUDIO_BUFFER_IN    AUDIO_REC_START_ADDR     /* In SDRAM */
#define AUDIO_BUFFER_OUT   (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 2)) /* In SDRAM */
#define float_buffer_in    (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 4))
#define float_buffer_out   (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 6))
#define NUM_TAPS_FIR			((uint16_t)232)
#define numStages_IIR 6
#define NUM_TAPS_IIR 5*numStages_IIR
#define BUFSIZE 9

float32_t pStateIIR [2*numStages_IIR];
arm_biquad_cascade_df2T_instance_f32 S;

unsigned char logmsgbuff[128];
char buf[BUFSIZE];

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t  audio_rec_buffer_state;

/* Private function prototypes -----------------------------------------------*/
static void AudioLoopback_SetHint(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Audio Play demo
  * @param  None
  * @retval None
  */

arm_fir_instance_q15 FIRstructINT, FIRstructINT2, FIRstructINT3, FIRstructINT4,FIRstructINT5 ;

q15_t pFIRStateINT[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT2[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT3[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT4[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT5[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];


static int firCoeffINT_BP[NUM_TAPS_FIR] = {
		22,
		26,
		28,
		28,
		26,
		20,
		10,
		-1,
		-15,
		-30,
		-45,
		-59,
		-69,
		-75,
		-75,
		-69,
		-55,
		-35,
		-10,
		19,
		51,
		83,
		111,
		133,
		147,
		150,
		140,
		118,
		84,
		40,
		-11,
		-68,
		-123,
		-174,
		-215,
		-242,
		-252,
		-243,
		-213,
		-163,
		-97,
		-17,
		69,
		156,
		238,
		306,
		354,
		378,
		373,
		339,
		275,
		186,
		76,
		-44,
		-170,
		-289,
		-392,
		-470,
		-515,
		-522,
		-489,
		-416,
		-306,
		-168,
		-11,
		154,
		315,
		458,
		572,
		646,
		673,
		649,
		574,
		451,
		290,
		100,
		-103,
		-306,
		-491,
		-645,
		-754,
		-807,
		-801,
		-733,
		-606,
		-430,
		-217,
		17,
		256,
		481,
		674,
		820,
		906,
		924,
		872,
		753,
		574,
		349,
		94,
		-171,
		-427,
		-654,
		-834,
		-952,
		-1000,
		-971,
		-869,
		-701,
		-478,
		-218,
		60,
		335,
		585,
		792,
		940,
		1016,
		1016,
		940,
		792,
		585,
		335,
		60,
		-218,
		-478,
		-701,
		-869,
		-971,
		-1000,
		-952,
		-834,
		-654,
		-427,
		-171,
		94,
		349,
		574,
		753,
		872,
		924,
		906,
		820,
		674,
		481,
		256,
		17,
		-217,
		-430,
		-606,
		-733,
		-801,
		-807,
		-754,
		-645,
		-491,
		-306,
		-103,
		100,
		290,
		451,
		574,
		649,
		673,
		646,
		572,
		458,
		315,
		154,
		-11,
		-168,
		-306,
		-416,
		-489,
		-522,
		-515,
		-470,
		-392,
		-289,
		-170,
		-44,
		76,
		186,
		275,
		339,
		373,
		378,
		354,
		306,
		238,
		156,
		69,
		-17,
		-97,
		-163,
		-213,
		-243,
		-252,
		-242,
		-215,
		-174,
		-123,
		-68,
		-11,
		40,
		84,
		118,
		140,
		150,
		147,
		133,
		111,
		83,
		51,
		19,
		-10,
		-35,
		-55,
		-69,
		-75,
		-75,
		-69,
		-59,
		-45,
		-30,
		-15,
		-1,
		10,
		20,
		26,
		28,
		28,
		26,
		22,
	};


// IIR filter design tools sets a0 to 1 and CMSIS library just skips it, because it is always 1
// Also depends on the IIR tool used, but a1 and a2 must be set to negative according to the CMSIS library (not multiplied by -1 (reversed), just negative value)
// Anyway check carefully outputs from your design tool also the order of coefficients, values are as follow: {b0,b1,b2,a1,a2}

float pCoeffs_Papoulis[NUM_TAPS_IIR] = {
		-0.004809079189758538,
		0.000000000000000000,
		0.004809079189758538,
		-1.972621180769796640,
		-0.991014143380059220,
		-0.004808077619045298,
		0.000000000000000000,
		0.004808077619045298,
		-1.971553963343847030,
		-0.990807748204756034,
		-0.006270951413705633,
		0.000000000000000000,
		0.006270951413705633,
		-1.974347870268532650,
		-0.991934760202094590,
		-0.006267386476686452,
		0.000000000000000000,
		0.006267386476686452,
		-1.971219989176736350,
		-0.991370860928448838,
		-0.008299902353179954,
		0.000000000000000000,
		0.008299902353179954,
		-1.976428657857877360,
		-0.993318394025093143,
		-0.008293509649630227,
		0.000000000000000000,
		0.008293509649630227,
		-1.971544377628177580,
		-0.992553325985339630,
		-0.010212155709167994,
		0.000000000000000000,
		0.010212155709167994,
		-1.978716667780299420,
		-0.995055798563271976,
		-0.010204399336458534,
		0.000000000000000000,
		0.010204399336458534,
		-1.972546017074517490,
		-0.994300030255368394,
		-0.011680190093799269,
		0.000000000000000000,
		0.011680190093799269,
		-1.981044792988907010,
		-0.997002585454860202,
		-0.011673815711014026,
		0.000000000000000000,
		0.011673815711014026,
		-1.974139564301675030,
		-0.996458478204333531,
		-0.012497329029943860,
		0.000000000000000000,
		0.012497329029943860,
		-1.983229792769316990,
		-0.998994738653404490,
		-0.012494847962433209,
		0.000000000000000000,
		0.012494847962433209,
		-1.976155953778969690,
		-0.998796410403948509,
};

float pCoeffs_Bessel[NUM_TAPS_IIR] = {
		-0.025780280677518329,
		0.000000000000000000,
		0.025780280677518329,
		-1.932100198929704640,
		-0.949792161690966252,
		-0.025725202982874500,
		0.000000000000000000,
		0.025725202982874500,
		-1.928584696129458820,
		-0.947762999816776919,
		-0.026205283073678632,
		0.000000000000000000,
		0.026205283073678632,
		-1.936955324112739070,
		-0.953286853377207866,
		-0.026042986053004747,
		0.000000000000000000,
		0.026042986053004747,
		-1.926575138476635860,
		-0.947382867691737296,
		-0.027037754588143267,
		0.000000000000000000,
		0.027037754588143267,
		-1.943094834372619540,
		-0.958176594399364867,
		-0.026777927515371860,
		0.000000000000000000,
		0.026777927515371860,
		-1.926364829073373920,
		-0.948968721056582254,
		-0.028372673654210513,
		0.000000000000000000,
		0.028372673654210513,
		-1.950604584733402590,
		-0.964530485833045237,
		-0.028035102125815094,
		0.000000000000000000,
		0.028035102125815094,
		-1.928448589749886420,
		-0.953054724533463959,
		-0.030420242334975974,
		0.000000000000000000,
		0.030420242334975974,
		-1.959834296771775450,
		-0.972674379560309443,
		-0.030044018792036709,
		0.000000000000000000,
		0.030044018792036709,
		-1.933750389758369440,
		-0.960644790933933401,
		-0.033743338380468710,
		0.000000000000000000,
		0.033743338380468710,
		-1.971924983069929690,
		-0.983697442622315066,
		-0.033418613371953482,
		0.000000000000000000,
		0.033418613371953482,
		-1.944540444735642250,
		-0.974230947137184611,
};

float pCoeffs_Chebyshev[NUM_TAPS_IIR] = {
		-0.005991540730286605,
		0.000000000000000000,
		0.005991540730286605,
		-1.970256065301738470,
		-0.988609278905328126,
		-0.005989890305448891,
		0.000000000000000000,
		0.005989890305448891,
		-1.969088702110876280,
		-0.988336957413720629,
		-0.007203093550829018,
		0.000000000000000000,
		0.007203093550829018,
		-1.972091703192099480,
		-0.989626751777556768,
		-0.007197677757002333,
		0.000000000000000000,
		0.007197677757002333,
		-1.968720898362509080,
		-0.988882680578817008,
		-0.008923882517727671,
		0.000000000000000000,
		0.008923882517727671,
		-1.974435076915363570,
		-0.991274583868968162,
		-0.008914729196442190,
		0.000000000000000000,
		0.008914729196442190,
		-1.969232107619814710,
		-0.990257822977029134,
		-0.010574020742414940,
		0.000000000000000000,
		0.010574020742414940,
		-1.977116688089808560,
		-0.993416140202281861,
		-0.010563189715231673,
		0.000000000000000000,
		0.010563189715231673,
		-1.970630353419772000,
		-0.992398578625570793,
		-0.011825941452947648,
		0.000000000000000000,
		0.011825941452947648,
		-1.979972188194653480,
		-0.995908643357051537,
		-0.011817083120724816,
		0.000000000000000000,
		0.011817083120724816,
		-1.972840620627834610,
		-0.995162648658739490,
		-0.012501014642234336,
		0.000000000000000000,
		0.012501014642234336,
		-1.982848802094768100,
		-0.998611902047683242,
		-0.012497588702914812,
		0.000000000000000000,
		0.012497588702914812,
		-1.975704644658095700,
		-0.998338229559642687,
};

float pCoeffs_Butterworth[NUM_TAPS_IIR] = {
		-0.012462382528959034,
		0.000000000000000000,
		0.012462382528959034,
		-1.957356328958664850,
		-0.975587227997376072,
		-0.012454940220142315,
		0.000000000000000000,
		0.012454940220142315,
		-1.955883199204155880,
		-0.975004624998990210,
		-0.012479198889426090,
		0.000000000000000000,
		0.012479198889426090,
		-1.960332119396708480,
		-0.977757121559415410,
		-0.012458872533616411,
		0.000000000000000000,
		0.012458872533616411,
		-1.956125430870043540,
		-0.976164532217394187,
		-0.012503025653798513,
		0.000000000000000000,
		0.012503025653798513,
		-1.964525741023045220,
		-0.981273778362731886,
		-0.012475269531483670,
		0.000000000000000000,
		0.012475269531483670,
		-1.958179005236898580,
		-0.979095397243573751,
		-0.012531630110548257,
		0.000000000000000000,
		0.012531630110548257,
		-1.969621578148413430,
		-0.985852521915587809,
		-0.012503873530606138,
		0.000000000000000000,
		0.012503873530606138,
		-1.961987202324168900,
		-0.983668935734516414,
		-0.012563218735417830,
		0.000000000000000000,
		0.012563218735417830,
		-1.975302314021319510,
		-0.991195541165531768,
		-0.012542882010162501,
		0.000000000000000000,
		0.012542882010162501,
		-1.967323524761294350,
		-0.989591042205550830,
		-0.012596505210648363,
		0.000000000000000000,
		0.012596505210648363,
		-1.981262390465300840,
		-0.997008112271294489,
		-0.012589046283379476,
		0.000000000000000000,
		0.012589046283379476,
		-1.973799128812205210,
		-0.996417741301603988,
};


float pCoeffs_Gauss[NUM_TAPS_IIR] = {
		-0.012485651035428426,
		0.000000000000000000,
		0.012485651035428426,
		-1.951191482077207830,
		-0.969215596273066526,
		-0.012618504961251871,
		0.000000000000000000,
		0.012618504961251871,
		-1.961410054291755630,
		-0.979528562458715446,
		-0.012736522206452015,
		0.000000000000000000,
		0.012736522206452015,
		-1.959793754115776610,
		-0.976767935098835083,
		-0.012716894320586916,
		0.000000000000000000,
		0.012716894320586916,
		-1.955991637395758650,
		-0.975262666294993474,
		-0.013042400721138360,
		0.000000000000000000,
		0.013042400721138360,
		-1.964320394310401060,
		-0.980436712640744834,
		-0.013010457972045577,
		0.000000000000000000,
		0.013010457972045577,
		-1.957673362781657820,
		-0.978035479571554012,
		-0.013386076653488406,
		0.000000000000000000,
		0.013386076653488406,
		-1.969595754368445920,
		-0.985101512287057446,
		-0.013351786249607870,
		0.000000000000000000,
		0.013351786249607870,
		-1.961316667676810570,
		-0.982578029896057803,
		-0.013679434286743531,
		0.000000000000000000,
		0.013679434286743531,
		-1.975424582462036000,
		-0.990533016650801401,
		-0.013652816649876836,
		0.000000000000000000,
		0.013652816649876836,
		-1.966659905941249290,
		-0.988605623486068397,
		-0.013860346090154515,
		0.000000000000000000,
		0.013860346090154515,
		-1.981576621509285600,
		-0.996500842390987796,
		-0.013849507162685466,
		0.000000000000000000,
		0.013849507162685466,
		-1.973359239516237150,
		-0.995721568894982201,
};

float pCoeffs_Gauss_narrow[NUM_TAPS_IIR] = {
		-0.020384435620409078,
		0.000000000000000000,
		0.020384435620409078,
		-1.941978696337710810,
		-0.959988771273521113,
		-0.020360705611723633,
		0.000000000000000000,
		0.020360705611723633,
		-1.939825061254447420,
		-0.958871225401540128,
		-0.020754239163339853,
		0.000000000000000000,
		0.020754239163339853,
		-1.944800812072398970,
		-0.961826081899713192,
		-0.020682677166735466,
		0.000000000000000000,
		0.020682677166735466,
		-1.938347234424074190,
		-0.958509641616587804,
		-0.021517069239109406,
		0.000000000000000000,
		0.021517069239109406,
		-1.948349316921433430,
		-0.964415314631653309,
		-0.021396624456065470,
		0.000000000000000000,
		0.021396624456065470,
		-1.937618246445108160,
		-0.959016865984012701,
		-0.022790538796785841,
		0.000000000000000000,
		0.022790538796785841,
		-1.952776161381973410,
		-0.967878511049898305,
		-0.022619782385799783,
		0.000000000000000000,
		0.022619782385799783,
		-1.937806306600082130,
		-0.960626753542492717,
		-0.024846915246898849,
		0.000000000000000000,
		0.024846915246898849,
		-1.958424387179125150,
		-0.972510484301599476,
		-0.024625140515276138,
		0.000000000000000000,
		0.024625140515276138,
		-1.939269834796285120,
		-0.963830201477228421,
		-0.028481592441719061,
		0.000000000000000000,
		0.028481592441719061,
		-1.966219529542057700,
		-0.979115429548457317,
		-0.028213809956285370,
		0.000000000000000000,
		0.028213809956285370,
		-1.942917349990701980,
		-0.969909836013353011,
};

const char * logmsg(text,value, base)
{
	logmsgbuff[0]='\0';
	char locmessage[128];
	strcat(logmsgbuff,text);
	itoa (value,locmessage,base);
    strcat(logmsgbuff,locmessage);
    return logmsgbuff;
}

void filterinit (void)
{
	  AudioLoopback_SetHint();

	  /* Initialize Audio RecINPUT_DEVICE_INPUT_LINE order INPUT_DEVICE_INPUT_LINE_1 */
	  if (BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_INPUT_LINE_1, OUTPUT_DEVICE_HEADPHONE, DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR) == AUDIO_OK)
	  {
	   BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	   BSP_AUDIO_IN_SetVolume(VOLFIR);
	   BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 235,  (uint8_t *)logmsg("Audio buffer size ",AUDIO_BLOCK_SIZE,10), LEFT_MODE);
	   BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 225, (uint8_t *)"Audio record Init OK", LEFT_MODE);
	  }
	  else
	  {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
	    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)" AUDIO RECORD INIT FAILED", CENTER_MODE);
	    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65, (uint8_t *)" Please reset the board ", CENTER_MODE);
	  }

	  /* Display the state on the screen */
	  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"Line-In --> Line-Out", LEFT_MODE);

	  /* Initialize SDRAM buffers */
	  memset((uint16_t*)AUDIO_BUFFER_IN, 0, AUDIO_BLOCK_SIZE*2);
	  memset((uint16_t*)AUDIO_BUFFER_OUT, 0, AUDIO_BLOCK_SIZE*2);
	  audio_rec_buffer_state = BUFFER_OFFSET_NONE;

	  /* Start Recording */
	  // Number of elements not size,
	  BSP_AUDIO_IN_Record((uint16_t*)AUDIO_BUFFER_IN, AUDIO_BLOCK_SIZE);

	  /* Start Playback */
	  // Data in bytes, left and right elements
	  BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	  BSP_AUDIO_OUT_Play((uint16_t*)AUDIO_BUFFER_OUT, AUDIO_BLOCK_SIZE * 2);

}

void filter (int ftype)
{
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_FillRect(0,70,300,200);
  if (ftype==0)
  {
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"Filtering OFF", LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLFIR);
  }
  else if (ftype==1)
  {
	  arm_fir_init_q15(&FIRstructINT, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT2, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT2, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT3, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT3, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT4, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT4, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT5, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT5, SIGNAL_SAMPLES);

	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"FIR filtering is ON, BandPass x5", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_FIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLFIR);

  }
  else if (ftype==2)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Papoulis, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Papoulis BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==3)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Bessel, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Bessel BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==4)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Chebyshev, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Chebyshev BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==5)
   {
 	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Butterworth, pStateIIR);
 	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Butterworth BW 150Hz", LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
   }


  else if (ftype==6)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Gauss BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==7)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss_narrow, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Gauss BW 100Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  while (1)
  {
    /* Wait end of half block recording */
    while(audio_rec_buffer_state != BUFFER_OFFSET_HALF)
    {
    BSP_PB_GetState(BUTTON_KEY);
    }

    audio_rec_buffer_state = BUFFER_OFFSET_NONE;

    /* Copy recorded 1st half block */
    if (ftype==0){
    	// No DSP
       	memcpy((uint16_t *)(AUDIO_BUFFER_OUT),(uint16_t *)(AUDIO_BUFFER_IN),AUDIO_BLOCK_SIZE);
    }
    else if (ftype==1){
    	// DSP FIR
        arm_fir_fast_q15(&FIRstructINT, (q15_t *)AUDIO_BUFFER_IN, (q15_t *)AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT2, (q15_t *)AUDIO_BUFFER_OUT, (q15_t *)AUDIO_BUFFER_IN, SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT3, (q15_t *)AUDIO_BUFFER_IN, (q15_t *)AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
        }
    else if (ftype>=2){
    	// DSP IIR
    	arm_q15_to_float (AUDIO_BUFFER_IN, float_buffer_in, SIGNAL_SAMPLES);
    	arm_biquad_cascade_df2T_f32(&S, float_buffer_in, float_buffer_out, SIGNAL_SAMPLES);
    	arm_float_to_q15 (float_buffer_out, AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
    	}

    /* Wait end of one block recording */
    while(audio_rec_buffer_state != BUFFER_OFFSET_FULL)
    {
    	BSP_PB_GetState(BUTTON_KEY);
    }
    audio_rec_buffer_state = BUFFER_OFFSET_NONE;

    /* Copy recorded 2nd half block */
    if (ftype==0){
    	memcpy((uint16_t *)((AUDIO_BUFFER_OUT) + (AUDIO_BLOCK_SIZE)),(uint16_t *)((AUDIO_BUFFER_IN) + (AUDIO_BLOCK_SIZE)),AUDIO_BLOCK_SIZE);
    }
    else if (ftype==1){
    	//DSP FIR
        arm_fir_fast_q15(&FIRstructINT, (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT2, (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT3, (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
    }
    else if (ftype>=2){
    	// DSP IIR
    	arm_q15_to_float((AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), float_buffer_in, SIGNAL_SAMPLES);
    	arm_biquad_cascade_df2T_f32(&S, float_buffer_in, float_buffer_out, SIGNAL_SAMPLES);
    	arm_float_to_q15(float_buffer_out, AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE), SIGNAL_SAMPLES);
    }

    if (CheckForUserInputTP() > 0)
    {
    	return;
    }
  }
}

/**
  * @brief  Display Audio Record demo hint
  * @param  None
  * @retval None
  */
static void AudioLoopback_SetHint(void)
{
  /* Clear the LCD */
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  /* Set Audio Demo description */
  BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font20);
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"SQ5MJW Filter v0.83          700Hz", LEFT_MODE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_DisplayStringAt(0, 22, (uint8_t *)"Use the touch screen to select the filter", LEFT_MODE);
  BSP_LCD_SetTextColor(LCD_COLOR_DARKMAGENTA);
  BSP_LCD_FillCircle(320, 20, 10);
  BSP_LCD_DisplayStringAt(340, 15, (uint8_t *)"OFF", LEFT_MODE);
  BSP_LCD_FillCircle(320, 50, 10);
  BSP_LCD_DisplayStringAt(340, 45, (uint8_t *)"FIR x5", LEFT_MODE);
  BSP_LCD_FillCircle(320, 80, 10);
  BSP_LCD_DisplayStringAt(340, 75, (uint8_t *)"Papoulis 150Hz", LEFT_MODE);
  BSP_LCD_FillCircle(320, 110, 10);
  BSP_LCD_DisplayStringAt(340, 105, (uint8_t *)"Bessel 150Hz", LEFT_MODE);
  BSP_LCD_FillCircle(320, 140, 10);
  BSP_LCD_DisplayStringAt(340, 135, (uint8_t *)"Chebyshev 150Hz", LEFT_MODE);
  BSP_LCD_FillCircle(320, 170, 10);
  BSP_LCD_DisplayStringAt(340, 165, (uint8_t *)"Butterworth 150Hz", LEFT_MODE);
  BSP_LCD_FillCircle(320, 200, 10);
  BSP_LCD_DisplayStringAt(340, 195, (uint8_t *)"Gauss 150Hz", LEFT_MODE);
  BSP_LCD_FillCircle(320, 230, 10);
  BSP_LCD_DisplayStringAt(340, 225, (uint8_t *)"Gauss 100Hz", LEFT_MODE);
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
