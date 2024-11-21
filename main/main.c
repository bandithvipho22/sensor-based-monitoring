#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"

// Task delays (ms)
#define PIR_TASK_DELAY 1000
#define MQ5_TASK_DELAY 2000
#define LCD_TASK_DELAY 3000

//I2C
#define I2C_MASTER_SCL_IO           GPIO_NUM_22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_NUM_21      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                      /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

char buffer[10];
float num = 12.34;

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// Log tags for each task
static const char *TAG_PIR = "PIR_SENSOR";
static const char *TAG_MQ5 = "MQ5_SENSOR";
static const char *TAG_LCD = "LCD_TASK";

// Global variables for the latest sensor readings
int pir_state = 0;  // PIR state (0: No Motion, 1: Motion)
int mq5_value = 0;  // MQ-5 sensor value (analog)

// Task Prototypes
void pir_sensor_task(void *pvParameter);
void mq5_sensor_task(void *pvParameter);
void lcd_task(void *pvParameter);

// PIR Sensor Task
void pir_sensor_task(void *pvParameter) {
    while (1) {
        pir_state = read_pir_sensor();  // Update the global pir_state
        ESP_LOGI(TAG_PIR, "PIR Sensor State: %d", pir_state);
        vTaskDelay(pdMS_TO_TICKS(PIR_TASK_DELAY));
    }
}

// MQ-5 Smoke Sensor Task
void mq5_sensor_task(void *pvParameter) {
    while (1) {
        mq5_value = read_mq5_sensor();  // Update the global mq5_value
        ESP_LOGI(TAG_MQ5, "MQ-5 Sensor Value (Analog): %d", mq5_value);
        vTaskDelay(pdMS_TO_TICKS(MQ5_TASK_DELAY));
    }
}

// LCD Update Task
void lcd_task(void *pvParameter) {
    ESP_ERROR_CHECK(i2c_master_init());
    lcd_init();

    while (1) {
        // Read the latest values of the sensors from the global variables
        int current_mq5_value = mq5_value;
        int current_pir_state = pir_state;

        ESP_LOGI(TAG_LCD, "MQ5 Value: %d, PIR State: %s",
                 current_mq5_value, current_pir_state ? "Motion" : "No Motion");

        lcd_clear(); // Clear the LCD before updating

        // Prepare the data for display
        char line1[16], line2[16];
        sprintf(line1, "MQ5: %d", current_mq5_value);
        sprintf(line2, "PIR: %s", current_pir_state ? "Motion" : "No Motion");

        // Update the LCD
        lcd_put_cur(0, 0);  // Move to the first row, first column
        lcd_send_string(line1);

        lcd_put_cur(1, 0);  // Move to the second row, first column
        lcd_send_string(line2);

        // Log the display update
        ESP_LOGI(TAG_LCD, "LCD updated successfully.");

        vTaskDelay(pdMS_TO_TICKS(LCD_TASK_DELAY));
    }
}


void app_main() {
    // Initialize sensors
    ESP_LOGI("INIT", "Initializing sensors...");
    sensor_init();
    ESP_LOGI("INIT", "Sensors initialized successfully!");

    // Initialize the LCD
    lcd_init();
    ESP_LOGI("INIT", "LCD initialized successfully!");

    // Create FreeRTOS tasks
    xTaskCreate(pir_sensor_task, "PIR Sensor Task", 2048, NULL, 5, NULL);
    xTaskCreate(mq5_sensor_task, "MQ5 Sensor Task", 2048, NULL, 5, NULL);
    xTaskCreate(lcd_task, "LCD Task", 2048, NULL, 5, NULL);
    ESP_LOGI("INIT", "Tasks created successfully!");
}


