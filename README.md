# BAW-PAL

🐾 Smart Pet Monitoring and Feeding System

📌 Project Overview
This project presents an IoT-based smart system designed to monitor and predict pet hydration and feeding behavior. It integrates real-time sensor data, cloud-based storage, and machine learning analytics to provide a proactive pet care solution.

🚀 Key Features
Real-Time Monitoring
Water/food weight via Load Cell + HX711
Pet presence detection via Ultrasonic sensor
Environmental tracking using DHT11 (Temperature + Humidity)

~Cloud Integration
Firebase Realtime Database for sensor data storage and remote access

~Machine Learning Analytics
Predicting temperature & humidity
Forecasting water consumption for the next 6 hours

~Dashboard Visualization
Node-RED interface for real-time and historical data visualization

~Automated Feeding
Motor driver + DC motor controlled by ESP32 to dispense food/water


🔧 How It Works
1. Sensors collect real-time data on:
Water/food weight
Pet proximity

2. Ambient temperature and humidity

3. ESP32 processes sensor readings and uploads them to Firebase every 5 seconds.

4. Python scripts perform EDA and train predictive models using historical data.

5. Node-RED dashboard visualizes real-time and predicted data for user monitoring.

6. DC motor is triggered via motor driver to automate food/water dispensing.


🚫 No codes for Solenoid Valve or Dispensing Motor.
This code does not include a solenoid valve or motor-based mechanism for automatically dispensing food or water. While the system architecture is designed to support automation through hardware like DC motors or valves, the current version only focuses on monitoring and prediction.
