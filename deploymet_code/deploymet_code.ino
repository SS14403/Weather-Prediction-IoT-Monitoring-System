#include "weatherModel.h"
#include <tflm_esp32.h>
#include <eloquent_tinyml.h>
#include <Wire.h>
#include "Adafruit_AHTX0.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "Shehab";
const char* password = "no1likeMe";

// MQTT broker details
const char* mqtt_broker = "82a83cfb21334cfa9d99cde33db5de1e.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "Shehab";
const char* mqtt_password = "123Ss123";

// MQTT topics
const char* temperature_topic = "temperature";
const char* humidity_topic = "humidity";

// Create instances
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
long previous_time = 0;
void setupMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT Broker.");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Sensor and model objects7
Adafruit_AHTX0 aht;
#define ARENA_SIZE 2000
#define TF_NUM_OPS 3
Eloquent::TF::Sequential<TF_NUM_OPS, ARENA_SIZE> tf;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
    }
   Serial.println("");
   Serial.println("Connected to Wi-Fi");

    wifiClient.setInsecure(); // For testing only

    setupMQTT();

    // Initialize AHT21 sensor
    Wire.begin(21, 22); // ESP32 I2C pins
    if (!aht.begin()) {
        Serial.println("Couldn't find AHT21 sensor :(");
        while (1);
    }
    Serial.println("AHT21 sensor found!");
    
    // Initialize TensorFlow Lite model
    tf.setNumInputs(2);
    tf.setNumOutputs(1);
    tf.resolver.AddFullyConnected();
    tf.resolver.AddSoftmax();
    tf.resolver.AddLogistic();  

    while (!tf.begin(weather_model_tflite).isOk()){ 
        Serial.println(tf.exception.toString());
    }
    
    Serial.println("Model loaded successfully");
}

void loop() {
    // Read sensor data
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    
    float temperature_reading = temp.temperature;
    float humidity_reading = humidity.relative_humidity;
    
    // Prepare input for the model 
    float input[2] = {
        temperature_reading / 50.0,
        humidity_reading / 100.0
    };
    
    // Make prediction
    float prediction = tf.predict(input);
    
    // Interpret prediction
    String weatherCondition = (prediction < 0.5) ? "Sunny" : "Cold";
    float confidence = (prediction < 0.5) ? prediction : (1.0 - prediction);
    
    // Print results
    Serial.print("Temperature: ");
    Serial.print(temperature_reading);
    Serial.print(" C, Humidity: ");
    Serial.print(humidity_reading);
    Serial.print(" %, Prediction: ");
    Serial.print(weatherCondition);
    Serial.print(" (");
    Serial.print(confidence * 100);
    Serial.println("%)");

    
    if (!mqttClient.connected()) {
    reconnect();
    }
    mqttClient.loop();

    long now = millis();
    if (now - previous_time > 5000) { // Publish every 5 seconds
     previous_time = now;

     // Convert to strings
     String temperature_str = String(temperature_reading);
     String humidity_str = String(humidity_reading);

    // Publish to MQTT topics
    mqttClient.publish(temperature_topic, temperature_str.c_str());
    mqttClient.publish(humidity_topic, humidity_str.c_str());
  }

delay(2000);
}
