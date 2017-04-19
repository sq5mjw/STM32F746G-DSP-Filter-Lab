/**
  ******************************************************************************
  *  * @modified by SQ5MJW 16-April-2017 v0.7
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

float32_t pStateIIR[2*numStages_IIR];

arm_biquad_cascade_df2T_instance_f32 S;

unsigned char logmsgbuff[128];
char buf[BUFSIZE];
volatile double nothing = 1232.324545;


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
		-0.004773899634222752,
		0.000000000000000000,
		0.004773899634222752,
		-1.972955677464933060,
		-0.990851686200530768,
		-0.004773103863654680,
		0.000000000000000000,
		0.004773103863654680,
		-1.972133855375595380,
		-0.990686519215546291,
		-0.006256473525436765,
		0.000000000000000000,
		0.006256473525436765,
		-1.974768945183474230,
		-0.991799744805672123,
		-0.006252862442323348,
		0.000000000000000000,
		0.006252862442323348,
		-1.971717919038504570,
		-0.991227302311382341,
		-0.008305836933994578,
		0.000000000000000000,
		0.008305836933994578,
		-1.976864923055130950,
		-0.993195885946652646,
		-0.008299232210203908,
		0.000000000000000000,
		0.008299232210203908,
		-1.972035081069430260,
		-0.992406105874055866,
		-0.010233951672283111,
		0.000000000000000000,
		0.010233951672283111,
		-1.979159815773817500,
		-0.994942621780261494,
		-0.010225855316726698,
		0.000000000000000000,
		0.010225855316726698,
		-1.973040837185072280,
		-0.994155495801742339,
		-0.011713832898743639,
		0.000000000000000000,
		0.011713832898743639,
		-1.981492391555118800,
		-0.996896175670684892,
		-0.011707078520431524,
		0.000000000000000000,
		0.011707078520431524,
		-1.974643244550702950,
		-0.996321349824475067,
		-0.012539141046894211,
		0.000000000000000000,
		0.012539141046894211,
		-1.983681523663058940,
		-0.998892846300141746,
		-0.012536340329973899,
		0.000000000000000000,
		0.012536340329973899,
		-1.976670038513743680,
		-0.998669735635244526,
};

float pCoeffs_Bessel[NUM_TAPS_IIR] = {
		-0.025487884833101774,
		0.000000000000000000,
		0.025487884833101774,
		-1.933232702386270320,
		-0.950366343873369490,
		-0.025433807716206756,
		0.000000000000000000,
		0.025433807716206756,
		-1.929769725670041460,
		-0.948349971302351658,
		-0.025907483770567383,
		0.000000000000000000,
		0.025907483770567383,
		-1.938021477047913170,
		-0.953831414321570481,
		-0.025748140334924303,
		0.000000000000000000,
		0.025748140334924303,
		-1.927798462836377570,
		-0.947964894209919806,
		-0.026729704285417832,
		0.000000000000000000,
		0.026729704285417832,
		-1.944079967554338540,
		-0.958674271216070739,
		-0.026474622209180804,
		0.000000000000000000,
		0.026474622209180804,
		-1.927610476636459680,
		-0.949525624417526748,
		-0.028048386028479918,
		0.000000000000000000,
		0.028048386028479918,
		-1.951491453844360400,
		-0.964961872132182541,
		-0.027717011224665737,
		0.000000000000000000,
		0.027717011224665737,
		-1.929696854481275150,
		-0.953561428244206977,
		-0.030071201939095172,
		0.000000000000000000,
		0.030071201939095172,
		-1.960599579073534220,
		-0.973014602634123538,
		-0.029701932498268149,
		0.000000000000000000,
		0.029701932498268149,
		-1.934973795545704260,
		-0.961066142477494334,
		-0.033354241094634071,
		0.000000000000000000,
		0.033354241094634071,
		-1.972528232029722830,
		-0.983905892580695607,
};

float pCoeffs_Chebyshev[NUM_TAPS_IIR] = {
		-0.005966558619446529,
		0.000000000000000000,
		0.005966558619446529,
		-1.970565562865220110,
		-0.988434063986562417,
		-0.005965276958237808,
		0.000000000000000000,
		0.005965276958237808,
		-1.969686060207978650,
		-0.988221740991350184,
		-0.007193915363814107,
		0.000000000000000000,
		0.007193915363814107,
		-1.972504947173706170,
		-0.989488092686955589,
		-0.007188454073694912,
		0.000000000000000000,
		0.007188454073694912,
		-1.969219958997831420,
		-0.988736918775342088,
		-0.008933009761068922,
		0.000000000000000000,
		0.008933009761068922,
		-1.974868860980933950,
		-0.991152042004594724,
		-0.008923596191796678,
		0.000000000000000000,
		0.008923596191796678,
		-1.969721303529424090,
		-0.990107570023001538,
		-0.010598826533775262,
		0.000000000000000000,
		0.010598826533775262,
		-1.977561360848037570,
		-0.993305826292016247,
		-0.010587588113477339,
		0.000000000000000000,
		0.010587588113477339,
		-1.971124095447537530,
		-0.992252578715534739,
		-0.011863072506464305,
		0.000000000000000000,
		0.011863072506464305,
		-1.980424825707588040,
		-0.995807602580781381,
		-0.011853780738633189,
		0.000000000000000000,
		0.011853780738633189,
		-1.973345846972594410,
		-0.995027634908612235,
		-0.012547237025077389,
		0.000000000000000000,
		0.012547237025077389,
		-1.983308907093626730,
		-0.998517695112934645,
};

float pCoeffs_Butterworth[NUM_TAPS_IIR] = {
		-0.012251247828727749,
		0.000000000000000000,
		0.012251247828727749,
		-1.956703410540356680,
		-0.974778441127531869,
		-0.012268640172468200,
		0.000000000000000000,
		0.012268640172468200,
		-1.958074577454754770,
		-0.976162274183224099,
		-0.012283195109129508,
		0.000000000000000000,
		0.012283195109129508,
		-1.960925536669865240,
		-0.977864210723312421,
		-0.012264396550881079,
		0.000000000000000000,
		0.012264396550881079,
		-1.957034745896141680,
		-0.976367658957992490,
		-0.012314028188160598,
		0.000000000000000000,
		0.012314028188160598,
		-1.965123579408535100,
		-0.981369384317362092,
		-0.012287258295467299,
		0.000000000000000000,
		0.012287258295467299,
		-1.959014192498612860,
		-0.979235951397667836,
		-0.012352363995187807,
		0.000000000000000000,
		0.012352363995187807,
		-1.970181439470986670,
		-0.985908735191118657,
		-0.012325234011823181,
		0.000000000000000000,
		0.012325234011823181,
		-1.962760220387947290,
		-0.983743345020039128,
		-0.012395876407767324,
		0.000000000000000000,
		0.012395876407767324,
		-1.975807065536882230,
		-0.991196030955821761,
		-0.012375798946551778,
		0.000000000000000000,
		0.012375798946551778,
		-1.968026448382514060,
		-0.989590601923305169,
		-0.012442491909819491,
		0.000000000000000000,
		0.012442491909819491,
		-1.981704364927197700,
		-0.996942075179689136,
		-0.012434933185593555,
		0.000000000000000000,
		0.012434933185593555,
		-1.974421786358793840,
		-0.996336440048870231,
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
		-0.008507645515091852,
		0.000000000000000000,
		0.008507645515091852,
		-1.960927933233222560,
		-0.979041987811167136,
		-0.008568859034632068,
		0.000000000000000000,
		0.008568859034632068,
		-1.967907780294998020,
		-0.986086311148952954,
		-0.008657897393204586,
		0.000000000000000000,
		0.008657897393204586,
		-1.966643590323144950,
		-0.984036308532999504,
		-0.008651784194484816,
		0.000000000000000000,
		0.008651784194484816,
		-1.964387654451411390,
		-0.983341496706486962,
		-0.008859130814779992,
		0.000000000000000000,
		0.008859130814779992,
		-1.969648499707955750,
		-0.986440344088848509,
		-0.008849174489150170,
		0.000000000000000000,
		0.008849174489150170,
		-1.965656545276454060,
		-0.985331734058645714,
		-0.009085663382263170,
		0.000000000000000000,
		0.009085663382263170,
		-1.973248302046904710,
		-0.989605582087147884,
		-0.009074967353800673,
		0.000000000000000000,
		0.009074967353800673,
		-1.968179298650780230,
		-0.988440576404314153,
		-0.009277850855539779,
		0.000000000000000000,
		0.009277850855539779,
		-1.977294639165418030,
		-0.993365960723293528,
		-0.009269545006820399,
		0.000000000000000000,
		0.009269545006820399,
		-1.971774400835511590,
		-0.992476665613766906,
		-0.009393655000920374,
		0.000000000000000000,
		0.009393655000920374,
		-1.981604952172339120,
		-0.997543232703205662,
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

void filter (int ftype)
{

  AudioLoopback_SetHint();

  /* Initialize Audio RecINPUT_DEVICE_INPUT_LINE order INPUT_DEVICE_INPUT_LINE_1 */
  if (BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_INPUT_LINE_1, OUTPUT_DEVICE_HEADPHONE, DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR) == AUDIO_OK)
  {
   BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
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

  if (ftype==0)
  {
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"Filtering OFF", LEFT_MODE);
  }
  else if (ftype==1)
  {
	  arm_fir_init_q15(&FIRstructINT, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT2, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT2, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT3, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT3, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT4, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT4, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT5, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT5, SIGNAL_SAMPLES);


	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"FIR filtering is ON, BandPass x5", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_FIR,10), LEFT_MODE);
  }
  else if (ftype==2)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Papoulis, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Papoulis BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);

  }

  else if (ftype==3)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Bessel, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Bessel BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==4)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Chebyshev, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Chebyshev BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==5)
   {
 	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Butterworth, pStateIIR);
 	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Butterworth BW 150Hz", LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
   }


  else if (ftype==6)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Gauss BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==7)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss_narrow, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Gauss BW 100Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }


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
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"SQ5MJW Filter Lab v0.7       700Hz", LEFT_MODE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_DisplayStringAt(0, 22, (uint8_t *)"Use the touch screen or the user button to switch to the next filter", LEFT_MODE);
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
