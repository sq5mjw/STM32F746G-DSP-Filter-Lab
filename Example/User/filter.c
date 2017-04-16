/**
  ******************************************************************************
  *  * @modified by SQ5MJW 16-April-2017 v0.61
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
#define NUM_TAPS_FIR			((uint16_t)232)

#define numStages_IIR 6

#define NUM_TAPS_IIR 5*numStages_IIR

#define BUFSIZE 9

float32_t float_buffer_in [AUDIO_BLOCK_SIZE];
float32_t float_buffer_out [AUDIO_BLOCK_SIZE];
float32_t float_buffer_in2 [AUDIO_BLOCK_SIZE];
float32_t float_buffer_out2 [AUDIO_BLOCK_SIZE];

float32_t pStateIIR[2*numStages_IIR];

// IIR filter design tools sets a0 to 1 and CMSIS library just skips it, because it is always 1
// Also depends on the IIR tool used, but a1 and a2 must be set to negative according to the CMSIS library (not multiplied by -1 (reversed), just negative value)
// Anyway check carefully outputs from your design tool also the order of coefficients, values are as follow: {b0,b1,b2,a1,a2}
// This is:
// Papoulis filter:
// Sampling 16000
// Fc 0,043500 348HZ
// bw 0,00650 52Hz
// poles 8

float pCoeffs_Papoulis[NUM_TAPS_IIR] = {
		-0.003240457770732086,
		0.000000000000000000,
		0.003240457770732086,
		-1.975406964920116250,
		-0.993771592638614698,


		-0.003240211985484264,
		0.000000000000000000,
		0.003240211985484264,
		-1.974881551523074870,
		-0.993696216128733600,


		-0.004245846353737522,
		0.000000000000000000,
		0.004245846353737522,
		-1.976607182911192860,
		-0.994370479810652075,


		-0.004244730743078327,
		0.000000000000000000,
		0.004244730743078327,
		-1.974647922470055490,
		-0.994109205564237097,


		-0.005635159531160897,
		0.000000000000000000,
		0.005635159531160897,
		-1.978025456905806000,
		-0.995294883659062934,


		-0.005633118188975788,
		0.000000000000000000,
		0.005633118188975788,
		-1.974899258987164120,
		-0.994934337090433885,


		-0.006941433007029783,
		0.000000000000000000,
		0.006941433007029783,
		-1.979603895631121050,
		-0.996481529102896002,


		-0.006938929494684109,
		0.000000000000000000,
		0.006938929494684109,
		-1.975597953434586000,
		-0.996122135904428330,


		-0.007943105653454378,
		0.000000000000000000,
		0.007943105653454378,
		-1.981227045659428750,
		-0.997831177376969802,


		-0.007941016430899030,
		0.000000000000000000,
		0.007941016430899030,
		-1.976677591986396140,
		-0.997568724440655252,


		-0.008500636276696231,
		0.000000000000000000,
		0.008500636276696231,
		-1.982760043272776110,
		-0.999224632198767559,


		-0.008499769942841441,
		0.000000000000000000,
		0.008499769942841441,
		-1.978023724859547580,
		-0.999122797218510050,

};

float pCoeffs_Bessel[NUM_TAPS_IIR] = {
		-0.003640452550836155,
		0.000000000000000000,
		0.003640452550836155,
		-1.974303263864971350,
		-0.992779196261073693,


		-0.003640305518536260,
		0.000000000000000000,
		0.003640305518536260,
		-1.974056052046054390,
		-0.992739099430672733,


		-0.003690665024977054,
		0.000000000000000000,
		0.003690665024977054,
		-1.974769070960310740,
		-0.993041517189907186,


		-0.003690230767133188,
		0.000000000000000000,
		0.003690230767133188,
		-1.974029341993310590,
		-0.992924672105022443,


		-0.003796312444146747,
		0.000000000000000000,
		0.003796312444146747,
		-1.975468426682190470,
		-0.993539488121070957,


		-0.003795614088421075,
		0.000000000000000000,
		0.003795614088421075,
		-1.974241948483488550,
		-0.993356720237652979,


		-0.003970504793076140,
		0.000000000000000000,
		0.003970504793076140,
		-1.976440531517044800,
		-0.994310279023987387,


		-0.003969591570185812,
		0.000000000000000000,
		0.003969591570185812,
		-1.974737039701990550,
		-0.994081585959952774,


		-0.004241985410316231,
		0.000000000000000000,
		0.004241985410316231,
		-1.977773821679854380,
		-0.995438621905882615,


		-0.004240959299538852,
		0.000000000000000000,
		0.004240959299538852,
		-1.975606918666449550,
		-0.995197831285605572,


		-0.004687442563022990,
		0.000000000000000000,
		0.004687442563022990,
		-1.979709845372539690,
		-0.997155926155944394,


		-0.004686548658131816,
		0.000000000000000000,
		0.004686548658131816,
		-1.977094726120905750,
		-0.996965766479816162,
};

float pCoeffs_Chebyshev[NUM_TAPS_IIR] = {
		-0.004051561004419270,
		0.000000000000000000,
		0.004051561004419270,
		-1.973776630846860280,
		-0.992122129192613222,


		-0.004051165068747367,
		0.000000000000000000,
		0.004051165068747367,
		-1.973222246978535880,
		-0.992025174823320977,


		-0.004883675434187867,
		0.000000000000000000,
		0.004883675434187867,
		-1.975049095102908050,
		-0.992778817300774552,


		-0.004881987848283199,
		0.000000000000000000,
		0.004881987848283199,
		-1.972969539220091220,
		-0.992435756104118938,


		-0.006062368483494210,
		0.000000000000000000,
		0.006062368483494210,
		-1.976642224718175190,
		-0.993877496728536536,


		-0.006059458305317884,
		0.000000000000000000,
		0.006059458305317884,
		-1.973355318006410730,
		-0.993400395970178485,


		-0.007190447203442614,
		0.000000000000000000,
		0.007190447203442614,
		-1.978490374770336620,
		-0.995340235814818342,


		-0.007186971311545615,
		0.000000000000000000,
		0.007186971311545615,
		-1.974325846323600640,
		-0.994859084230982238,


		-0.008045399329753664,
		0.000000000000000000,
		0.008045399329753664,
		-1.980480869900467230,
		-0.997069592974019203,


		-0.008042524752919175,
		0.000000000000000000,
		0.008042524752919175,
		-1.975819742314921750,
		-0.996713345504271375,


		-0.008506515431048805,
		0.000000000000000000,
		0.008506515431048805,
		-1.982499182724117400,
		-0.998961849740839569,


		-0.008505354696497401,
		0.000000000000000000,
		0.008505354696497401,
		-1.977729913985152080,
		-0.998825538986579775,
};

float pCoeffs_Gauss[NUM_TAPS_IIR] = {
		-0.005599971587503494,
		0.000000000000000000,
		0.005599971587503494,
		-1.970634819894860450,
		-0.988969642885008326,


		-0.005599266777163733,
		0.000000000000000000,
		0.005599266777163733,
		-1.970091052536688370,
		-0.988845171533837330,


		-0.005666536736232694,
		0.000000000000000000,
		0.005666536736232694,
		-1.971840295731055900,
		-0.989784428641117375,


		-0.005664577262107307,
		0.000000000000000000,
		0.005664577262107307,
		-1.970266202781513210,
		-0.989442163679658426,


		-0.005778698217019156,
		0.000000000000000000,
		0.005778698217019156,
		-1.973618843948621080,
		-0.991223813594022007,


		-0.005775935093973398,
		0.000000000000000000,
		0.005775935093973398,
		-1.971177624863994060,
		-0.990749853324773278,


		-0.005902159140741902,
		0.000000000000000000,
		0.005902159140741902,
		-1.975860912722090300,
		-0.993197577868560555,


		-0.005899277506807524,
		0.000000000000000000,
		0.005899277506807524,
		-1.972798404732882370,
		-0.992712665182256870,


		-0.005996959166561173,
		0.000000000000000000,
		0.005996959166561173,
		-1.978449439054095650,
		-0.995604744401821562,


		-0.005994737497061119,
		0.000000000000000000,
		0.005994737497061119,
		-1.975061468641674980,
		-0.995235906690355998,


		-0.006025759939268345,
		0.000000000000000000,
		0.006025759939268345,
		-1.981289522419510620,
		-0.998364806415963590,


		-0.006024865866862376,
		0.000000000000000000,
		0.006024865866862376,
		-1.977889431922803090,
		-0.998216673992252224,
};


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
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Papoulis", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);

  }

  else if (ftype==3)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Bessel, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Bessel", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==4)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Chebyshev, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Chebyshev", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==5)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Gauss", LEFT_MODE);
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
    	arm_q15_to_float((AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), float_buffer_in2, SIGNAL_SAMPLES);
    	arm_biquad_cascade_df2T_f32(&S, float_buffer_in2, float_buffer_out2, SIGNAL_SAMPLES);
    	arm_float_to_q15(float_buffer_out2, AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE), SIGNAL_SAMPLES);
    }

    if (CheckForUserInput() > 0)
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
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"SQ5MJW DSP Filter Lab v0.61", LEFT_MODE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"Press the user button to switch to the next filter", LEFT_MODE);
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
