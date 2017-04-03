/**
  ******************************************************************************
  * @modified by SQ5MJW 01-April-2017
  * @file    BSP/Src/filter.c
  * @original author  MCD Application Team
  * @version 0.3
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
#define NUM_TAPS			((uint16_t)352)
#define BUFSIZE 9

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

arm_fir_instance_q15 FIRstructINT, FIRstructINT2, FIRstructINT3;

q15_t pFIRStateINT[NUM_TAPS + AUDIO_BLOCK_SIZE];
q15_t pFIRStateINT2[NUM_TAPS + AUDIO_BLOCK_SIZE];
q15_t pFIRStateINT3[NUM_TAPS + AUDIO_BLOCK_SIZE];

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 16000 Hz

fixed point precision: 16 bits

* 0 Hz - 550 Hz
  gain = 0
  desired attenuation = -80 dB
  actual attenuation = n/a

* 675 Hz - 725 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 850 Hz - 8000 Hz
  gain = 0
  desired attenuation = -80 dB
  actual attenuation = n/a

*/


static int firCoeffINT_BP[NUM_TAPS] = {
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  1,
		  -13,
		  -28,
		  -41,
		  -53,
		  -62,
		  -66,
		  -66,
		  -60,
		  -48,
		  -32,
		  -11,
		  12,
		  37,
		  61,
		  82,
		  99,
		  109,
		  111,
		  104,
		  88,
		  64,
		  33,
		  -3,
		  -42,
		  -81,
		  -116,
		  -145,
		  -164,
		  -172,
		  -166,
		  -147,
		  -115,
		  -72,
		  -20,
		  38,
		  96,
		  150,
		  196,
		  230,
		  248,
		  247,
		  226,
		  187,
		  131,
		  61,
		  -18,
		  -100,
		  -178,
		  -247,
		  -301,
		  -334,
		  -342,
		  -324,
		  -280,
		  -212,
		  -124,
		  -22,
		  86,
		  193,
		  290,
		  369,
		  423,
		  446,
		  436,
		  392,
		  315,
		  211,
		  86,
		  -50,
		  -188,
		  -316,
		  -425,
		  -506,
		  -550,
		  -553,
		  -514,
		  -434,
		  -318,
		  -174,
		  -11,
		  157,
		  318,
		  460,
		  571,
		  641,
		  664,
		  637,
		  560,
		  438,
		  280,
		  97,
		  -99,
		  -292,
		  -466,
		  -609,
		  -709,
		  -757,
		  -748,
		  -681,
		  -562,
		  -398,
		  -200,
		  16,
		  235,
		  439,
		  614,
		  744,
		  819,
		  834,
		  785,
		  676,
		  515,
		  312,
		  84,
		  -153,
		  -380,
		  -581,
		  -740,
		  -844,
		  -885,
		  -859,
		  -768,
		  -618,
		  -422,
		  -192,
		  53,
		  295,
		  515,
		  697,
		  826,
		  893,
		  893,
		  826,
		  697,
		  515,
		  295,
		  53,
		  -192,
		  -422,
		  -618,
		  -768,
		  -859,
		  -885,
		  -844,
		  -740,
		  -581,
		  -380,
		  -153,
		  84,
		  312,
		  515,
		  676,
		  785,
		  834,
		  819,
		  744,
		  614,
		  439,
		  235,
		  16,
		  -200,
		  -398,
		  -562,
		  -681,
		  -748,
		  -757,
		  -709,
		  -609,
		  -466,
		  -292,
		  -99,
		  97,
		  280,
		  438,
		  560,
		  637,
		  664,
		  641,
		  571,
		  460,
		  318,
		  157,
		  -11,
		  -174,
		  -318,
		  -434,
		  -514,
		  -553,
		  -550,
		  -506,
		  -425,
		  -316,
		  -188,
		  -50,
		  86,
		  211,
		  315,
		  392,
		  436,
		  446,
		  423,
		  369,
		  290,
		  193,
		  86,
		  -22,
		  -124,
		  -212,
		  -280,
		  -324,
		  -342,
		  -334,
		  -301,
		  -247,
		  -178,
		  -100,
		  -18,
		  61,
		  131,
		  187,
		  226,
		  247,
		  248,
		  230,
		  196,
		  150,
		  96,
		  38,
		  -20,
		  -72,
		  -115,
		  -147,
		  -166,
		  -172,
		  -164,
		  -145,
		  -116,
		  -81,
		  -42,
		  -3,
		  33,
		  64,
		  88,
		  104,
		  111,
		  109,
		  99,
		  82,
		  61,
		  37,
		  12,
		  -11,
		  -32,
		  -48,
		  -60,
		  -66,
		  -66,
		  -62,
		  -53,
		  -41,
		  -28,
		  -13,
		  1,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0,
		  0
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

void filter (int dofilter)
{
  arm_fir_init_q15(&FIRstructINT, NUM_TAPS, firCoeffINT_BP, pFIRStateINT, SIGNAL_SAMPLES);
  arm_fir_init_q15(&FIRstructINT2, NUM_TAPS, firCoeffINT_BP, pFIRStateINT2, SIGNAL_SAMPLES);
  arm_fir_init_q15(&FIRstructINT3, NUM_TAPS, firCoeffINT_BP, pFIRStateINT3, SIGNAL_SAMPLES);

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
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 225, (uint8_t *)"LINE-IN --> headphones", LEFT_MODE);
  if (dofilter==true){
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"Filtering ON", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(100, BSP_LCD_GetYSize() - 215, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS,10), LEFT_MODE);
  }
  else
  {
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"Filtering OFF", LEFT_MODE);
  }


  /* Initialize SDRAM buffers */
  memset((uint16_t*)AUDIO_BUFFER_IN, 0, AUDIO_BLOCK_SIZE*2);
  memset((uint16_t*)AUDIO_BUFFER_OUT, 0, AUDIO_BLOCK_SIZE*2);
  audio_rec_buffer_state = BUFFER_OFFSET_NONE;

  /* Start Recording */
  // Number of elements not size,
  BSP_AUDIO_IN_Record((uint16_t*)AUDIO_BUFFER_IN, AUDIO_BLOCK_SIZE);

  /* Start Playback */
  // Data in bytes, lefta ndd right elements
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
    if (dofilter==true){
    arm_fir_fast_q15(&FIRstructINT, (q15_t *)AUDIO_BUFFER_IN, (q15_t *)AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
    arm_fir_fast_q15(&FIRstructINT2, (q15_t *)AUDIO_BUFFER_OUT, (q15_t *)AUDIO_BUFFER_IN, SIGNAL_SAMPLES);
    arm_fir_fast_q15(&FIRstructINT3, (q15_t *)AUDIO_BUFFER_IN, (q15_t *)AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
    }
    else {
    memcpy((uint16_t *)(AUDIO_BUFFER_OUT),(uint16_t *)(AUDIO_BUFFER_IN),AUDIO_BLOCK_SIZE);
    }

    /* Wait end of one block recording */
    while(audio_rec_buffer_state != BUFFER_OFFSET_FULL)
    {
    	BSP_PB_GetState(BUTTON_KEY);
    }
    audio_rec_buffer_state = BUFFER_OFFSET_NONE;

    /* Copy recorded 2nd half block */
    if (dofilter==true){
    arm_fir_fast_q15(&FIRstructINT, (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
    arm_fir_fast_q15(&FIRstructINT2, (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
    arm_fir_fast_q15(&FIRstructINT3, (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
    }
    else
    {
     memcpy((uint16_t *)((AUDIO_BUFFER_OUT) + (AUDIO_BLOCK_SIZE)),(uint16_t *)((AUDIO_BUFFER_IN) + (AUDIO_BLOCK_SIZE)),AUDIO_BLOCK_SIZE);
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
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"SQ5MJW DSP Filter Lab v0.3", LEFT_MODE);
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
