# STM32F746G-DSP-Filter-Lab
DSP filter experiments on the STM32F746, CW audio filter

v0.7 filters: Audio passtrough, FIR filter, IIR filters all of the 12th order: Papoulis, Bessel, Chebyshev, Butterworth, Gauss.
Use the touch screen or the user button to switch to the next filter.

The purpose of this project is to play with the DSP filters which I need for my ham radio cw (morse code) activities. My radio is not narrow enough in receive nor it is easy to modify internally so last thing I can to do is to modify the audio signal. Project is based on the original STM32Cube/Repository/STM32Cube_FW_F7_V1.6.0/Projects/STM32746G-Discovery/Examples/BSP examples. I've modified the STM provided example to work audio line-int to line-out and do some DSP in between. Currently it implements just the 700HZ bandpass filter.

Please note that the code is currently small and experimental - but seems stable :-) Sometimes on start DMA error apears (the same was with the original STM code) - the board reset solves it (too aggresive timing on start ?)
 
I'm using "System Workbench for STM32" - http://www.openstm32.org/

SQ5MJW DSP Filter LAB is a free software, you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3. Be informed it is distributed WITHOUT ANY WARRANTY.