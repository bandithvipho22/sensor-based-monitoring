| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# _Project Overview_
In this project, we create a demo application of a sensor based-monitoring system. This system
is using ESP32-IDF with FreeRTOS for task function managing, and integrated with multiple
sensor such as a PIR sensor, MQ-5 gas sensor, and LCD-I2C.

## Scope of Works
The primary objective of this project conducted with:
+ Using PIR sensor is utilized for motion detection
+ Using MQ-5 gas sensor to be able detect gas concentrations or changes in gas components
+ LCD-I2C display is provide a interface for displaying sensor data
+ Using FreeRTOS to manage the difference type of sensor task function
## How to use
Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── components
│   ├── CMakeLists.txt
│   ├── adc.c
│   └── include
│       └── adc.h
├── main
│   ├── CMakeLists.txt
│   ├── main.c
│   ├── sensor.c
│   └── sensor.h
└── README.md
```

+ Clone the Repo and open it in VScode, make sure you've already setup ESP-IDF
+ Build the code using this command:
```
idf.build
```
+ Write the code into ESP32:
```
idf.py -p COM11 flash monitor 
```
+ Here command to check PORT of ESP32:
```
mode
```
# _Methodology_
## 1). Circuit Connection
This is the table for circuit connection of the PIR sensor, MQ-5 gas sensor, and LCD-I2C
display to ESP32c6:

![image](https://github.com/user-attachments/assets/25266163-2de2-4576-903f-50f6bcff98bd)

## 2). Circuit Diagram
The circuit for this project involves connecting several components to the ESP32 microcontroller,
which is responsible for reading data from the sensors and displaying it on an LCD
screen. Below is an overview of our circuit diagram connection:

![Screenshot 2024-11-21 050943](https://github.com/user-attachments/assets/8221441c-61b8-412e-863e-87a5c7741b67)

## 3). Software setup
This is the task separation of multiple sensor tasks by using ESP32-IDF
with FreeRTOS framework. The system operates in three main tasks:
+ PIR Sensor Task (delay 1000): This task is responsible for reading the motion detection
with state "0" and "1".
+ MQ-5 Sensor Task (delay 2000): This task read the analog voltage across the sensor’s
load resistor using an (ADC). However,I use library "adc.c" that have capability to
calibrate ADC by converting the raw ADC readings to voltage values.
+ LCD-I2C (delay 3000): Display the PIR sensor state (Motion or No-Motion) and
Voltage value of MQ-5 gas sensor.

Note that: The library and configuration of I2C, I follow from this [LINK](https://controllerstech.com/i2c-in-esp32-esp-idf-lcd-1602/)

# _Results_
We added the obstacle in front of the PIR sensor we can see the state was changed
from ’No Motion’ to ’Motion’. Then, we put the fire on MQ-5 sensor the
value of it will increasing up to 1766.

![Screenshot 2024-11-21 073443](https://github.com/user-attachments/assets/1c70eaae-d958-42ca-aba3-03c02a4b5b41)

As the result, in this Demo project sensor based-monitoring system is successful and be able to
work with multiple sensor in the same time by using FreeRTOS with difference delay. However,
we also can scale this project into application such as smart home system by add more sensors
and define threshold of each sensor in specific.
