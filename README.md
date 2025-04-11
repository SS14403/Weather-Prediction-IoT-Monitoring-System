# Weather Prediction & IoT Monitoring System

## Overview

This project demonstrates an end-to-end system for real-time weather classification using machine learning and IoT. It includes:

- **Neural Network Model**: Predicts "Sunny" or "Cold" from temperature/humidity data (TensorFlow/Keras).
- **ESP32 Deployment**: Runs the model on-device and transmits predictions via MQTT.
- **Node-RED Dashboard**: Visualizes live sensor data and predictions.

## Hardware Requirements

- ESP32 microcontroller
- AHT21 temperature/humidity sensor
- Breadboard & jumper wires

## Software Requirements

- Arduino IDE (for ESP32 programming)
- TensorFlow Lite for Microcontrollers
- Node-RED (for visualization)
- HiveMQ Cloud (MQTT broker)

## Setup Instructions

### 1. Model Training (Python)

- Train the model using `weathermodel.ipynb` (see Section 1).
- Convert the trained model to TensorFlow Lite format (`weather_model.tflite`).

### 2. ESP32 Deployment

- Flash `deployment_code.ino` to the ESP32 (update Wi-Fi/MQTT credentials).
- Connect the AHT21 sensor to the I²C pins (21/22).

### 3. Node-RED Dashboard

- Import the flow from `flows.json`.
- Deploy to start visualizing data at `http://localhost:1880/ui`.

## Project Structure
├── weathermodel.ipynb # Jupyter Notebook for model training <br>
├── deployment_code.ino # ESP32 code with MQTT integration <br>
├── flows.json # Node-RED flow configuration <br>
├── weather_model.tflite # Trained TensorFlow Lite model <br>
└── README.md # This file <br>

## Troubleshooting

| Issue                        | Solution                                                            |
|------------------------------|---------------------------------------------------------------------|
| MQTT connection fails         | Verify TLS certificates in `deployment_code.ino`.                  |
| AHT21 not detected            | Check I²C wiring and ESP32 pin assignments.                        |
| Model prediction errors       | Ensure input normalization matches training.                       |

## License

Feel free to use this as open source however don't forget to add me as a contributer or star this repo.

---

**Author**: Shehab El-Din Ahmed  
**Contact**: [shehabsoliman144@gmail.com](mailto:shehabsoliman144@gmail.com)
