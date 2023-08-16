#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include "MQ135.h"

#include "WiFiConnect.h"
#include "MQTT.h"

const char* MOTION_JSON_NAME = "motion";
const char* ODOUR_LEVEL_JSON_NAME = "odourLevel";
const char* ODOUR_LEVEL_PPM_JSON_NAME = "odourLevelPpm";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
const unsigned long delayPeriod = 2000; // 2 second delay period
unsigned long lastMotionTriggerTime = 0;
const unsigned long motionCooldownPeriod = 15000; // 30 seconds cooldown period

// Hardware pins
const int motionSensorPin = D0;
const int gasSensorPin = A0;

// Create MQ135 gas sensor object
MQ135 gasSensor = MQ135(A0);

#pragma region Function handlers
String getOdourLevelString() {
  float airQuality = gasSensor.getPPM();

  if (airQuality <= 6) {
    return "Good";
  } else if (airQuality > 6 && airQuality <= 12) {
    return "Moderate";
  } else if (airQuality > 12) {
    return "Bad";
  } else {
    return "Unknown";
  }
}

float getOdourLevelPpm() {
  float airQuality = gasSensor.getPPM();

  Serial.println("Air Quality Index: " + String(airQuality));

  return airQuality;
}

// Get sensor readings and populate readings object
void readOdourSensorAndPopulateReadingsObject() {
  String odourLevel = getOdourLevelString();
  float odourLevelPpm = getOdourLevelPpm();

  readings[ODOUR_LEVEL_JSON_NAME] = String(odourLevel);
  readings[ODOUR_LEVEL_PPM_JSON_NAME] = String(odourLevelPpm);
}

void readMotionSensorAndPopulateReadingsObject() {
  bool isMotion = digitalRead(D0);
  readings[MOTION_JSON_NAME] = boolean(isMotion);
}

JSONVar getSensorReadingsForWebSocketClients() {
  JSONVar readingsForWebSocketClients;

  String odourLevel = getOdourLevelString();
  float odourLevelPpm = getOdourLevelPpm();
  bool isMotion = digitalRead(D0);

  readingsForWebSocketClients[MOTION_JSON_NAME] = boolean(isMotion);
  readingsForWebSocketClients[ODOUR_LEVEL_JSON_NAME] = String(odourLevel);
  readingsForWebSocketClients[ODOUR_LEVEL_PPM_JSON_NAME] = String(odourLevelPpm);

  return readingsForWebSocketClients;
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String sensorReadings = JSON.stringify(getSensorReadingsForWebSocketClients());
    Serial.print(sensorReadings);
    ws.textAll(sensorReadings);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
#pragma endregion

#pragma region Initializers
// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
   Serial.println("LittleFS mounted successfully");
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void initWebServer() {
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  // Start server
  server.begin();
}

void initPins() {
  pinMode(motionSensorPin, INPUT);
}
#pragma endregion

#pragma region Setup and Loop
void setup() {
  Serial.begin(9600);

  Serial.println("Starting ESP8266...");
  Serial.println("Connecting to WiFi...");

  if (connectToWiFi()) {
    Serial.println("Connected to the WiFi network");
    Serial.println("IP Address: " + WiFi.localIP().toString());

    initFS();
    initWebServer();
    initWebSocket();
    initPins();
    
    ConnectToMqttBroker();
  } else {
    Serial.println("Could not connect to the WiFi network.");
  }

  // Initialise a default value for motion
  readings[MOTION_JSON_NAME] = boolean(false);

  Serial.println("Started ESP8266.");
}

void loop() {
  LoopMqttClient();

  unsigned long currentTime = millis();
  bool isMotion = digitalRead(motionSensorPin) == HIGH;

  // Check if the cooldown period has passed since the last motion trigger
  if (isMotion || (currentTime - lastMotionTriggerTime) >= motionCooldownPeriod) {
    readMotionSensorAndPopulateReadingsObject();
    lastMotionTriggerTime = currentTime;
  }

  readOdourSensorAndPopulateReadingsObject();

  String stringifiedReadings = JSON.stringify(readings);

  Serial.println(stringifiedReadings);

  ws.textAll(stringifiedReadings);

  PublishMessageToMqttBroker(stringifiedReadings, STATE_TOPIC);

  ws.cleanupClients();

  delay(delayPeriod);
}
#pragma endregion