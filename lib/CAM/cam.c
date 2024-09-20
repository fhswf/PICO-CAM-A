/*****************************************************************************
* | File      	:   cam.c
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
#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"
#include "hm01b0_init.h"
#include "hardware/dma.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "image.pio.h"

int PIN_CAM_SIOC = 5;		// I2C0 SCL
int PIN_CAM_SIOD = 4;		// I2C0 SDA
int PIN_CAM_RESETB = 2;		// Reset
int PIN_CAM_XCLK = 3;		// Clock
int PIN_CAM_VSYNC = 16;     // GP15 hsync  GP14 pixel clock     
int PIN_CAM_Y2_PIO_BASE = 6;// Data GPIO6

#if defined (SOFTWARE_I2C)
#define SCCB_SIC_H()      gpio_put(PIN_CAM_SIOC,1)	//SCL H
#define SCCB_SIC_L()      gpio_put(PIN_CAM_SIOC,0)	//SCL L
#define SCCB_SID_H()      gpio_put(PIN_CAM_SIOD,1)	//SDA H
#define SCCB_SID_L()      gpio_put(PIN_CAM_SIOD,0)	//SDA L
#define SCCB_DATA_IN      gpio_set_dir(PIN_CAM_SIOD, GPIO_IN);
#define SCCB_DATA_OUT     gpio_set_dir(PIN_CAM_SIOD, GPIO_OUT);
#define SCCB_SID_STATE	  gpio_get(PIN_CAM_SIOD)

void sccb_bus_init(void);
void sccb_bus_start(void);
void sccb_bus_stop(void);
void sccb_bus_send_noack(void);
void sccb_bus_send_ack(void);
unsigned char sccb_bus_write_byte(unsigned char data);
unsigned char sccb_bus_read_byte(void);
unsigned char I2C_TIM;
extern uint8_t image_buf[324*324];

/******************************************************************************
Function: Sends a start signal on the SCCB bus
Parameters:
Info:
******************************************************************************/
void sccb_bus_start(void)
{
    SCCB_SID_H();             
    sleep_us(I2C_TIM);
    SCCB_SIC_H();	           
    sleep_us(I2C_TIM);
    SCCB_SID_L();
    sleep_us(I2C_TIM);
    SCCB_SIC_L();	           
    sleep_us(I2C_TIM);
}

/******************************************************************************
Function: Sends a stop signal on the SCCB bus
Parameters:
Info:
******************************************************************************/
void sccb_bus_stop(void)
{
    SCCB_SID_L();
    sleep_us(I2C_TIM);
    SCCB_SIC_H();	
    sleep_us(I2C_TIM);  
    SCCB_SID_H();	
    sleep_us(I2C_TIM);  
}

/******************************************************************************
Function: Sends a no-acknowledge signal on the SCCB bus
Parameters:
Info:
******************************************************************************/
void sccb_bus_send_noack(void)
{	
	SCCB_SID_H();	
	sleep_us(I2C_TIM);	
	SCCB_SIC_H();	
	sleep_us(I2C_TIM);	
	SCCB_SIC_L();	
	sleep_us(I2C_TIM);	
	SCCB_SID_L();	
	sleep_us(I2C_TIM);
}

/******************************************************************************
Function: Sends an acknowledge signal on the SCCB bus
Parameters:
Info:
******************************************************************************/
void sccb_bus_send_ack(void)
{	
	SCCB_SID_L();	
	sleep_us(I2C_TIM);	
	SCCB_SIC_L();	
	sleep_us(I2C_TIM);	
	SCCB_SIC_H();	
	sleep_us(I2C_TIM);	
	SCCB_SIC_L();	
	sleep_us(I2C_TIM);	
	SCCB_SID_L();	
	sleep_us(I2C_TIM);
}


/******************************************************************************
Function: Writes a byte of data on the SCCB bus
Parameters:
    data: Byte of data to be written
******************************************************************************/
unsigned char sccb_bus_write_byte(unsigned char data)
{
	unsigned char i;
	unsigned char tem;
	for(i = 0; i < 8; i++) 
	{
		if((data<<i) & 0x80)
		{
			SCCB_SID_H();
		}
		else 
		{
			SCCB_SID_L();
		}
		sleep_us(I2C_TIM);
		SCCB_SIC_H();	
		sleep_us(I2C_TIM);
		SCCB_SIC_L();	
	}
	SCCB_DATA_IN;
	sleep_us(I2C_TIM);
	SCCB_SIC_H();	
	sleep_us(I2C_TIM);
	if(SCCB_SID_STATE)
	{
		tem = 0;              
	}
	else 
	{
		tem = 1;               
	}

	SCCB_SIC_L();	
	sleep_us(I2C_TIM);	
	SCCB_DATA_OUT;
	return tem;  
}

/******************************************************************************
Function: Reads a byte of data from the SCCB bus
Parameters:
******************************************************************************/
unsigned char sccb_bus_read_byte(void)
{	
	unsigned char i;
	unsigned char read = 0;
	SCCB_DATA_IN;
	for(i = 8; i > 0; i--)
	{		     
		sleep_us(I2C_TIM);
		SCCB_SIC_H();
		sleep_us(I2C_TIM);
		read = read << 1;
		if(SCCB_SID_STATE)
		{
			read += 1; 
		}
		SCCB_SIC_L();
		sleep_us(I2C_TIM);
	}	
    SCCB_DATA_OUT;
	return read;
}

/******************************************************************************
Function: Writes a 16-bit register value to the camera sensor
Parameters:
    slave_address: I2C slave address of the camera sensor
    regID: Register ID (16 bits) to write to
    regDat: Register data (8 bits) to write
******************************************************************************/
unsigned char wrSensorReg16_8( uint8_t slave_address, int regID, int regDat)
{
	sccb_bus_start();
	if(0==sccb_bus_write_byte(slave_address<<1))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(10);
  if(0==sccb_bus_write_byte(regID>>8))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(10);
  if(0==sccb_bus_write_byte(regID))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(10);
  if(0==sccb_bus_write_byte(regDat))
	{
		sccb_bus_stop();
		return(0);
	}
  sccb_bus_stop();
	
  return(1);
}

/******************************************************************************
Function: Reads an 8-bit register value from the camera sensor
Parameters:
    slave_address: I2C slave address of the camera sensor
    regID: Register ID (16 bits) to read from
    regDat: Pointer to store the read register data (8 bits)
******************************************************************************/
unsigned char rdSensorReg16_8(uint8_t slave_address, unsigned int regID, unsigned char* regDat)
{
	sccb_bus_start();                  
	if(0==sccb_bus_write_byte(slave_address<<1))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(20);
	sleep_us(20);
  if(0==sccb_bus_write_byte(regID>>8))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(20);
  if(0==sccb_bus_write_byte(regID))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(20);
	sccb_bus_stop();
	
	sleep_us(20);
	
	
	sccb_bus_start();                 
	if(0==sccb_bus_write_byte((slave_address<<1)|0x01))
	{
		sccb_bus_stop();
		return(0);
	}
	sleep_us(20);
  *regDat=sccb_bus_read_byte();
  sccb_bus_send_noack();
  sccb_bus_stop();
  return(1);
}
#endif


/******************************************************************************
Function: Initializes the camera configuration structure
Parameters:
    config: Pointer to a struct cam_config containing camera configuration settings
******************************************************************************/
void cam_config_struct(struct cam_config *config){
	config->sccb = i2c0;
    config->sccb_mode = I2C_MODE_16_8;
    config->sensor_address = 0x24;
    config->pin_sioc = PIN_CAM_SIOC;
    config->pin_siod = PIN_CAM_SIOD;
    config->pin_resetb = PIN_CAM_RESETB;
    config->pin_xclk = PIN_CAM_XCLK;
    config->pin_vsync = PIN_CAM_VSYNC;
    config->pin_y2_pio_base = PIN_CAM_Y2_PIO_BASE;

    config->pio = pio0;
    config->pio_sm = 0;

    config->dma_channel = 0;
    config->image_buf = image_buf;
    config->image_buf_size = sizeof(image_buf);
}

/******************************************************************************
Function: Initializes the camera module with the specified configuration
Parameters:
    config: Pointer to a struct cam_config containing camera configuration settings
******************************************************************************/
void cam_init(struct cam_config *config){
	gpio_set_function(config->pin_xclk, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(config->pin_xclk);
	// 6 cycles (0 to 5), 125 MHz / 6 = ~20.83 MHz wrap rate
	pwm_set_wrap(slice_num, 9);
	pwm_set_gpio_level(config->pin_xclk, 3);
	pwm_set_enabled(slice_num, true);
#ifndef SOFTWARE_I2C
	// SCCB I2C @ 100 kHz
	gpio_set_function(config->pin_sioc, GPIO_FUNC_I2C);
	gpio_set_function(config->pin_siod, GPIO_FUNC_I2C);
	i2c_init(config->sccb, 100 * 100);
	// i2c_init(config->sccb, 100 * 1000);
#else
	gpio_init(config->pin_sioc);
	gpio_init(config->pin_siod);
	gpio_set_dir(config->pin_sioc, GPIO_OUT);
	gpio_set_dir(config->pin_siod, GPIO_OUT);
#endif

	// Initialise reset pin
	gpio_init(config->pin_resetb);
	gpio_set_dir(config->pin_resetb, GPIO_OUT);

	// Reset camera, and give it some time to wake back up
	gpio_put(config->pin_resetb, 0);
	sleep_ms(100);
	gpio_put(config->pin_resetb, 1);
	sleep_ms(100);
	// Initialise the camera itself over SCCB
	cam_regs_write(config, hm01b0_324x244);
	// Enable image RX PIO
	uint offset = pio_add_program(config->pio, &image_program);
	image_program_init(config->pio, config->pio_sm, offset, config->pin_y2_pio_base);
}

/******************************************************************************
Function: Captures a frame from the camera and stores it in the specified image buffer
Parameters:
    config: Pointer to a struct cam_config containing camera configuration settings
******************************************************************************/
void cam_capture_frame(struct cam_config *config) {
	dma_channel_config c = dma_channel_get_default_config(config->dma_channel);
	channel_config_set_read_increment(&c, false);
	channel_config_set_write_increment(&c, true);
	channel_config_set_dreq(&c, pio_get_dreq(config->pio, config->pio_sm, false));
	channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
	
	dma_channel_configure(
		config->dma_channel, &c,
		config->image_buf,
		&config->pio->rxf[config->pio_sm],
		config->image_buf_size,
		false
	);
	// Wait for vsync rising edge to start frame
	while (gpio_get(config->pin_vsync) == true);
	while (gpio_get(config->pin_vsync) == false);
	
	dma_channel_start(config->dma_channel);
	pio_sm_set_enabled(config->pio, config->pio_sm, true);
	dma_channel_wait_for_finish_blocking(config->dma_channel);
	pio_sm_set_enabled(config->pio, config->pio_sm, false);
}

/******************************************************************************
Function: Writes a value to a specific register in the camera using SCCB I2C
Parameters:
    config: Pointer to a struct cam_config containing camera configuration settings
    reg: Register address to write to
    value: Value to write to the register
******************************************************************************/
void cam_reg_write(struct cam_config *config, uint16_t reg, uint8_t value) {
	uint8_t data[3];
	uint8_t length =0;
	switch (config->sccb_mode){
		case I2C_MODE_16_8:
			data[0] = (uint8_t)(reg>>8)&0xFF;
			data[1] = (uint8_t)(reg)&0xFF;
			data[2] = value;
			length = 3;
			break;
		case I2C_MODE_8_8:
			data[0] = (uint8_t)(reg)&0xFF;
			data[1] = value;
			length = 2; 
			break;
	}
	//printf("length: %x data[0]: = %x  data[1] = %x data[2] = %x\r\n", length, data[0],data[1],data[2]);
#ifndef SOFTWARE_I2C
	int ret = i2c_write_blocking(config->sccb, config->sensor_address, data, length, false);
#else
	int ret = wrSensorReg16_8(config->sensor_address, reg, value);
#endif
	//printf("ret: %x\r\n", ret);
}

/******************************************************************************
Function: Reads the value from a specific register in the camera using SCCB I2C
Parameters:
    config: Pointer to a struct cam_config containing camera configuration settings
    reg: Register address to read from
******************************************************************************/
uint8_t cam_reg_read(struct cam_config *config, uint16_t reg) {
	uint8_t data[2];
	uint8_t length;
	switch (config->sccb_mode){
		case I2C_MODE_16_8:
			data[0] = (uint8_t)(reg>>8)&0xFF;
			data[1] = (uint8_t)(reg)&0xFF;
			length = 2;
		case I2C_MODE_8_8:
			data[0] = (uint8_t)reg&0xFF;
			length = 1;
	}
	i2c_write_blocking(config->sccb, config->sensor_address, data, length, false);

	uint8_t value;
	i2c_read_blocking(config->sccb, config->sensor_address, &value, 1, false);

	return value;
}

/******************************************************************************
Function: Writes a list of sensor registers to configure the camera
Parameters:
    config: Pointer to a struct cam_config containing camera configuration settings
    regs_list: Pointer to a struct sensor_reg array containing register settings
******************************************************************************/
void cam_regs_write(struct cam_config *config, struct senosr_reg* regs_list) {
	while (1) {
		uint16_t reg = regs_list->reg;
		uint8_t value = regs_list->val;
		
		if (reg == 0xFFFF && value == 0xFF) {
			break;
		}
		//printf("reg: 0x%04x , val: 0x%02x\r\n",reg, value);
		cam_reg_write(config, reg, value);

		regs_list++;
		sleep_ms(10); //GJW 20230823
	}
}
