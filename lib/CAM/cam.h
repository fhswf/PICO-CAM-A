/*****************************************************************************
* | File      	:   cam.h
* | Author      :   
* | Function    :   Hardware underlying interface
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
# copies of theex Software, and to permit persons to  whom the Software is
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
#ifndef _CAM_H_
#define _CAM_H_
#include "stdint.h"
#include "pico/stdio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#define SOFTWARE_I2C 1

enum i2c_mode{
	I2C_MODE_16_8 = 0,
	I2C_MODE_8_8 = 1,
};

struct senosr_reg{
	uint16_t reg;
	uint8_t  val;
};

struct cam_config {
    uint8_t sensor_address;
	i2c_inst_t *sccb;
	enum i2c_mode sccb_mode;
	uint pin_sioc;
	uint pin_siod;
	uint pin_resetb;
	uint pin_xclk;
	uint pin_vsync;
	// Y2, Y3, Y4, Y5, Y6, Y7, Y8, PCLK, HREF
	uint pin_y2_pio_base;
	PIO pio;
	uint pio_sm;
	uint dma_channel;
	uint8_t *image_buf;
	size_t image_buf_size;
};

extern int PIN_LED;
extern int PIN_CAM_SIOC; 
extern int PIN_CAM_SIOD; 
extern int PIN_CAM_RESETB;
extern int PIN_CAM_XCLK;
extern int PIN_CAM_VSYNC;
extern int PIN_CAM_Y2_PIO_BASE;
void cam_init(struct cam_config *config);
void cam_config_struct(struct cam_config *config);
void cam_capture_frame(struct cam_config *config);
void cam_reg_write(struct cam_config *config, uint16_t reg, uint8_t value);
uint8_t cam_reg_read(struct cam_config *config, uint16_t reg);
void cam_regs_write(struct cam_config *config, struct senosr_reg* regs_list);
#endif
