#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define PIR_PIN 13      // PIR sensor
#define TRIGGER_PIN 14   // External trigger pin (wire/microphone connected to GPIO4)

const char* ssid = "iQOO Neo9 Pro";
const char* password = "12345679";
const char* serverUrl = "http://192.168.160.5:5000/upload";

bool motionDetected = false;

// CAMERA_MODEL_AI_THINKER pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x", err);
    return;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIGGER_PIN, INPUT); // GPIO4 as input

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  startCamera();
}

void loop() {
  int pirState = digitalRead(PIR_PIN);
  int triggerState = digitalRead(TRIGGER_PIN);

  if ((pirState == HIGH || triggerState == HIGH) && !motionDetected) {
    if (pirState == HIGH) {
      Serial.println("👣 PIR triggered! Capturing photo...");
    } else if (triggerState == HIGH) {
      Serial.println("🎤 Microphone/External trigger detected! Capturing photo...");
    }

    motionDetected = true;
    sendPhotoToServer();
  } else if (pirState == LOW && triggerState == LOW) {
    motionDetected = false;
  }

  delay(200);
}

void sendPhotoToServer() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");

  int response = http.POST(fb->buf, fb->len);

  if (response > 0) {
    Serial.printf("📸 Image sent! Server responded with code: %d\n", response);
  } else {
    Serial.printf("🚫 Failed to send image. Error: %s\n", http.errorToString(response).c_str());
  }

  http.end();
  esp_camera_fb_return(fb);
}