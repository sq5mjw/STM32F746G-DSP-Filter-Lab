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

#define numStages_IIR 2

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
//BW 80Hz

float pCoeffs_Papoulis[NUM_TAPS_IIR] = {
		-0.005538526107435588,
		0.000000000000000000,
		0.005538526107435588,
		-1.972614610049172160,
		-0.990463588369599357,


		-0.005536429160905935,
		0.000000000000000000,
		0.005536429160905935,
		-1.970797588948368430,
		-0.990088588027614791,


		-0.006869103554089063,
		0.000000000000000000,
		0.006869103554089063,
		-1.979266953556423130,
		-0.996171887584844984,


		-0.006866458841007944,
		0.000000000000000000,
		0.006866458841007944,
		-1.975303879677748680,
		-0.995788345714877177,

};

float pCoeffs_Bessel[NUM_TAPS_IIR] = {
		-0.010067458681671733,
		0.000000000000000000,
		0.010067458681671733,
		-1.963426476569726240,
		-0.981125083961196354,


		-0.010059133255519867,
		0.000000000000000000,
		0.010059133255519867,
		-1.961045241946487660,
		-0.980313728812837737,


		-0.011321729776854677,
		0.000000000000000000,
		0.011321729776854677,
		-1.970608069635592760,
		-0.986863859598200666,


		-0.011300968022947472,
		0.000000000000000000,
		0.011300968022947472,
		-1.963965822436366530,
		-0.985054151629831898,
};

float pCoeffs_Chebyshev[NUM_TAPS_IIR] = {
		-0.006506199917777932,
		0.000000000000000000,
		0.006506199917777932,
		-1.970584304415705200,
		-0.988400013713603753,


		-0.006503135493413760,
		0.000000000000000000,
		0.006503135493413760,
		-1.968648394685735600,
		-0.987934476668654304,


		-0.006996605062655752,
		0.000000000000000000,
		0.006996605062655752,
		-1.978414357617919170,
		-0.995315829865579427,


		-0.006993315637940084,
		0.000000000000000000,
		0.006993315637940084,
		-1.974377657578240570,
		-0.994847886275604720,
};

float pCoeffs_Butterworth[NUM_TAPS_IIR] = {
		-0.007064795369427740,
		0.000000000000000000,
		0.007064795369427740,
		-1.969401704304823750,
		-0.987206194428278949,


		-0.007061120484840203,
		0.000000000000000000,
		0.007061120484840203,
		-1.967418080463037010,
		-0.986692680782246923,


		-0.007091773962736257,
		0.000000000000000000,
		0.007091773962736257,
		-1.977935517157679120,
		-0.994831974905722616,


		-0.007088092402668206,
		0.000000000000000000,
		0.007088092402668206,
		-1.973849652460726080,
		-0.994315526737397204,
};


float pCoeffs_Gauss[NUM_TAPS_IIR] = {
		-0.007360366058861707,
		0.000000000000000000,
		0.007360366058861707,
		-1.968890035698149580,
		-0.986664862870946013,


		-0.007356231636401091,
		0.000000000000000000,
		0.007356231636401091,
		-1.966814797345115770,
		-0.986110639162840807,


		-0.007716053253045429,
		0.000000000000000000,
		0.007716053253045429,
		-1.976975813859470410,
		-0.993756713708329187,


		-0.007710876524695117,
		0.000000000000000000,
		0.007710876524695117,
		-1.972507075581744650,
		-0.993089998694259402,
};

float pCoeffs_Gauss_narrow[NUM_TAPS_IIR] = {
		-0.004593890588267141,
		0.000000000000000000,
		0.004593890588267141,
		-1.973511870989796750,
		-0.991610481294714741,


		-0.004592890687864243,
		0.000000000000000000,
		0.004592890687864243,
		-1.972346196343295550,
		-0.991394648614176699,


		-0.004809388077099495,
		0.000000000000000000,
		0.004809388077099495,
		-1.978568253014311830,
		-0.996029515287870315,


		-0.004808136094008295,
		0.000000000000000000,
		0.004808136094008295,
		-1.975939524313615970,
		-0.995770228224425957,

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
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Papoulis 80Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);

  }

  else if (ftype==3)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Bessel, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Bessel 80Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==4)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Chebyshev, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Chebyshev 80Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==5)
   {
 	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Butterworth, pStateIIR);
 	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Butterworth 80Hz", LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
   }


  else if (ftype==6)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Gauss 80Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 185, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
  }

  else if (ftype==7)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss_narrow, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)"IIR filtering is ON, type: Gauss narrow 50Hz", LEFT_MODE);
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
