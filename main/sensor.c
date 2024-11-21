#include "sensor.h"
#include "adc.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "adc.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "unistd.h"


// GPIO Pins for sensors
#define PIR_SENSOR_PIN GPIO_NUM_18
#define MQ5_SENSOR_CHANNEL ADC_CHANNEL_0 
// #define MQ5_SENSOR_DIGITAL_PIN GPIO_NUM_13

// ADC channels for the MQ-5 sensor
static adc_channel_t mq5_channel = ADC_CHANNEL_0;

// I2C
#define SLAVE_ADDRESS_LCD 0x4E>>1 // change this according to ur setup
esp_err_t err;

#define I2C_NUM I2C_NUM_0

static const char *TAG = "LCD";

// Initialize all sensors
void sensor_init(void) {
    // Initialize PIR sensor
    gpio_set_direction(PIR_SENSOR_PIN, GPIO_MODE_INPUT);
    // Initialize MQ-5 sensor
    adc1_init(ADC_BITWIDTH_12, &mq5_channel, 1);

}

// Read PIR sensor value
int read_pir_sensor(void) {
    return gpio_get_level(PIR_SENSOR_PIN);
}

// Read MQ-5 sensor value (in millivolts, using calibration if available)
int read_mq5_sensor(void) {
    return adc1_read(mq5_channel, 10); // 10 samples for averaging
}

// LCD-I2C ==================================================
void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	err = i2c_master_write_to_device(I2C_NUM, SLAVE_ADDRESS_LCD, data_t, 4, 1000);
	if (err!=0) ESP_LOGI(TAG, "Error in sending command");
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	err = i2c_master_write_to_device(I2C_NUM, SLAVE_ADDRESS_LCD, data_t, 4, 1000);
	if (err!=0) ESP_LOGI(TAG, "Error in sending data");
}

void lcd_clear (void)
{
	lcd_send_cmd (0x01);
	usleep(5000);
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void)
{
	// 4 bit initialisation
	usleep(50000);  // wait for >40ms
	lcd_send_cmd (0x30);
	usleep(5000);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	usleep(200);  // wait for >100us
	lcd_send_cmd (0x30);
	usleep(10000);
	lcd_send_cmd (0x20);  // 4bit mode
	usleep(10000);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	usleep(1000);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	usleep(1000);
	lcd_send_cmd (0x01);  // clear display
	usleep(1000);
	usleep(1000);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	usleep(1000);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	usleep(1000);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
