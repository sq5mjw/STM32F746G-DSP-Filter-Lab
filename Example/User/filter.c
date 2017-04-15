/**
  ******************************************************************************
  * @modified by SQ5MJW 15-April-2017
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

#define AUDIO_BLOCK_SIZE   ((uint32_t)512)

#define SIGNAL_SAMPLES		AUDIO_BLOCK_SIZE/2
#define AUDIO_BUFFER_IN    AUDIO_REC_START_ADDR     /* In SDRAM */
#define AUDIO_BUFFER_OUT   (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 2)) /* In SDRAM */
#define NUM_TAPS_FIR			((uint16_t)232)

#define numStages_IIR 4

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
		-0.004453105074640686,
		0.000000000000000000,
		0.004453105074640686,
		-1.973679161804757110,
		-0.991810591848408807,
		-0.004452222937777849,
		0.000000000000000000,
		0.004452222937777849,
		-1.972596132974067550,
		-0.991614119349027545,
		-0.005858430043101291,
		0.000000000000000000,
		0.005858430043101291,
		-1.976003438507583530,
		-0.993341323812330090,
		-0.005855630317908647,
		0.000000000000000000,
		0.005855630317908647,
		-1.972950775650190950,
		-0.992866609134726907,
		-0.007525188393173548,
		0.000000000000000000,
		0.007525188393173548,
		-1.978961485700691860,
		-0.995712268573466841,
		-0.007521625161054671,
		0.000000000000000000,
		0.007521625161054671,
		-1.974578419062708920,
		-0.995240791481975196,
		-0.008580022140128688,
		0.000000000000000000,
		0.008580022140128688,
		-1.982072427651418560,
		-0.998517771500437079,
		-0.008578338347309741,
		0.000000000000000000,
		0.008578338347309741,
		-1.977214002573235210,
		-0.998321816638574888
};

float pCoeffs_Bessel[NUM_TAPS_IIR] = {
		-0.015280280809286444,
		0.000000000000000000,
		0.015280280809286444,
		-1.952596570282258750,
		-0.970335110029151737,
		-0.015263982435826997,
		0.000000000000000000,
		0.015263982435826997,
		-1.950286006214408290,
		-0.969300123552040760,
		-0.015772145288135569,
		0.000000000000000000,
		0.015772145288135569,
		-1.956755262835553570,
		-0.973313059200168862,
		-0.015725043462833190,
		0.000000000000000000,
		0.015725043462833190,
		-1.949995039788087190,
		-0.970406363830487173,
		-0.016862366051165541,
		0.000000000000000000,
		0.016862366051165541,
		-1.962944210991035780,
		-0.978391155179873961,
		-0.016791405969291406,
		0.000000000000000000,
		0.016791405969291406,
		-1.952297454708791680,
		-0.974273896886119095,
		-0.018970071167194957,
		0.000000000000000000,
		0.018970071167194957,
		-1.972126322384573750,
		-0.986486282989031182,
		-0.018895879446399106,
		0.000000000000000000,
		0.018895879446399106,
		-1.958794754280556160,
		-0.982628146968802407
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
   BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 245,  (uint8_t *)logmsg("Audio buffer size ",AUDIO_BLOCK_SIZE,10), LEFT_MODE);
   BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 235, (uint8_t *)"Audio record Init OK", LEFT_MODE);
  }
  else
  {
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)" AUDIO RECORD INIT FAILED", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65, (uint8_t *)" Please reset the board ", CENTER_MODE);
  }

  /* Display the state on the screen */
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 225, (uint8_t *)"Line-In --> Line-Out", LEFT_MODE);

  if (ftype==0)
  {
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"Filtering OFF", LEFT_MODE);
  }
  else if (ftype==1)
  {
	  arm_fir_init_q15(&FIRstructINT, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT2, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT2, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT3, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT3, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT4, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT4, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT5, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT5, SIGNAL_SAMPLES);


	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"FIR filtering is ON, BandPass x5", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_FIR,10), LEFT_MODE);
  }
  else if (ftype==2)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Papoulis, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"IIR filtering is ON, type: Papoulis", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)logmsg("Order ",2*numStages_IIR,20), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,20), LEFT_MODE);

  }

  else if (ftype==3)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Bessel, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"IIR filtering is ON, type: Bessel", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 205, (uint8_t *)logmsg("Order ",2*numStages_IIR,20), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 195, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,20), LEFT_MODE);
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
    else if (ftype==2 || ftype==3){
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
    else if (ftype==2 || ftype==3){
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
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Set Audio Demo description */
  BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"SQ5MJW DSP Filter Lab v0.5", LEFT_MODE);
  BSP_LCD_SetFont(&Font12);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
}



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
