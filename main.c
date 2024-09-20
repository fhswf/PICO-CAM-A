/*****************************************************************************
* | File      	:   main.c
* | Function    :   test Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2024-01-09
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#include <stdio.h>
#include <tusb.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "cam.h"
#include "ImageData.h"
#include "LCD_1in14_V2.h" 
#include "GUI_Paint.h"

uint8_t image_buf[324*324];
uint8_t displayBuf[240*135*2];
uint8_t header[2] = {0x55,0xAA};

#define FLAG_VALUE 666
uint8_t imageReady = 0;

void core1_entry() {

    // Notify core 0 that core 1 is ready
    multicore_fifo_push_blocking(FLAG_VALUE);

    // Wait for acknowledgment from core 0
    uint32_t ack  = multicore_fifo_pop_blocking();
    if (ack != FLAG_VALUE)
    	printf("Error: Core 1 failed to receive acknowledgment from core 0!\n");
	else
		printf("Success: Core 1 Received acknowledgment from core 0!\n");

    // LCD Init
    DEV_Module_Init();
    LCD_1IN14_V2_Init(HORIZONTAL);
    LCD_1IN14_V2_Clear(BLACK);
    UDOUBLE Imagesize = LCD_1IN14_V2_HEIGHT * LCD_1IN14_V2_WIDTH * 2;
    UWORD *BlackImage;
    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    
    // Create a new image cache and draw on the image
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN14_V2.WIDTH, LCD_1IN14_V2.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_DrawImage(gImage_waveshare, 0, 0, 240, 135);
    LCD_1IN14_V2_Display(BlackImage);
    DEV_Delay_ms(500);

    // CAM Init
    struct cam_config config;
    cam_config_struct(&config);
    cam_init(&config);

    // Image Processing
    while (true) {
        cam_capture_frame(&config);

        uint16_t index = 0;
        for (int y = 134; y > 0; y--) {
            for (int x = 0; x < 240; x++) {
                uint16_t c = image_buf[(y)*324+(x)];
                uint16_t imageRGB = (((c & 0xF8) << 8) | ((c & 0xFC) << 3) | ((c & 0xF8) >> 3));
                displayBuf[index++] = (uint16_t)(imageRGB >> 8) & 0xFF;
                displayBuf[index++] = (uint16_t)(imageRGB) & 0xFF;
            }
        }

        // Set the imageReady flag to indicate the image is ready for display
        imageReady = 1;
    }
}

int main() {

    int loops = 20;
    stdio_init_all();
    while (!tud_cdc_connected()) {
        DEV_Delay_ms(100);
        if (--loops == 0)
            break;
    }

    printf("tud_cdc_connected(%d)\n", tud_cdc_connected() ? 1 : 0);
    vreg_set_voltage(VREG_VOLTAGE_1_10);
    set_sys_clock_khz(250000, true);
    multicore_launch_core1(core1_entry);

    // Wait for acknowledgment from core 1
    uint32_t ack = multicore_fifo_pop_blocking();
    if (ack != FLAG_VALUE)
        printf("Error: Core 0 failed to receive acknowledgment from core 1!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("Success: Core 0 Received acknowledgment from core 1!\n");
    }

    // Show Image
    while (1) {
        if (imageReady == 1) {
            LCD_1IN14_V2_Display((uint16_t*)displayBuf);
            // Reset the imageReady flag after displaying the image
            imageReady = 0;
        }
		DEV_Delay_ms(1);
    }
    tight_loop_contents();
}
