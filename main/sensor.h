#ifndef SENSOR_H
#define SENSOR_H

// Function 
void sensor_init(void);
int read_pir_sensor(void);
int read_mq5_sensor(void);
// int read_mq5_sensor_digital(void);

void lcd_init (void);   // initialize lcd
void lcd_send_cmd (char cmd);  // send command to the lcd
void lcd_send_data (char data);  // send data to the lcd
void lcd_send_string (char *str);  // send string to the lcd
void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);
void lcd_clear (void);


#endif // SENSOR_H

