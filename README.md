# ENG-FINAL-PROJECT-GROUP31
# GROUP MEMBERS 
# AGONDEZE JACKSON 2023/DEE/DAY/1697/G
# MULEMAVU YOWEERI 2023/DEE/DAY/1686/G
# NABAASA BOAZ 2023/DEE/DAY/1685/G
# TUKASHABA BEDAN 2023/DEE/DAY/1203/G
# AGABA MARK  2023/DEE/DAY/0373
# KIRUNDA UMARU 2023/DEE/DAY/0373

# 📘 Project Description: Solar-Powered Cold Storage Monitoring System

This Arduino-based embedded system is designed to monitor and report the conditions within a cold storage unit—particularly tailored for agricultural or food preservation use. The system integrates several hardware components and sensors to measure temperature, humidity, weight, and item ripeness status, and can send SMS alerts based on critical conditions.

🔧 Key Hardware Components Used

HX711 Load Cell Amplifier: Measures the weight of the stored contents.

DHT11 Sensor: Monitors temperature and humidity.

LiquidCrystal_I2C LCD: Displays real-time data.

ESP32-CAM (Status Input Pins): Detects item ripeness using external image processing:

RIPE_STATUS_PIN (pin 7)

RAW_STATUS_PIN (pin 9)

EMPTY_STATUS_PIN (pin 8)

GSM Module (via SoftwareSerial): Sends SMS alerts in case of abnormal conditions.

LED Indicator: Shows system status.


---

📲 System Features

Environmental Monitoring: Measures ambient temperature and humidity inside the cold room.

Weight Measurement: Detects the weight of stored items using a load cell.

Ripeness Detection: Uses input from an external ESP32-CAM to classify stored produce as ripe, unripe, or empty.

SMS Alerts: Sends automated messages if the temperature crosses a certain threshold or if the system detects a critical status (e.g., high temperature, empty storage).

LCD Display: Shows real-time sensor data and system status to the user.


---

📤 Example SMS Trigger

The system checks if conditions go beyond set safety thresholds (e.g., high temperature or empty storage), and sends a message like:

> "Warning! Temperature is too high!"
to a predefined phone number of the farmer.
