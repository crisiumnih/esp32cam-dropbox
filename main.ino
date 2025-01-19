#include "esp_camera.h" // ESP32 camera library
#include <WiFi.h>       // WiFi library for ESP32
#include <HTTPClient.h> // HTTP client for making requests
#include <WiFiClientSecure.h> // Secure WiFi client for HTTPS

// Replace with your WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Replace with your Dropbox access token
const char* dropboxAccessToken = "YOUR_DROPBOX_ACCESS_TOKEN";

// Camera pin configuration for AI Thinker model
#define CAMERA_MODEL_AI_THINKER
#if defined(CAMERA_MODEL_AI_THINKER)
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
#else
  #error "Camera model not selected"
#endif

// Initialize the camera
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Adjust settings based on available memory
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // High resolution if PSRAM is available
    config.jpeg_quality = 10;           // Lower number means higher quality
    config.fb_count = 2;                // Two frame buffers
  } else {
    config.frame_size = FRAMESIZE_SVGA; // Lower resolution if no PSRAM
    config.jpeg_quality = 12;           // Slightly lower quality
    config.fb_count = 1;                // Single frame buffer
  }

  // Initialize the camera and apply sensor settings
  esp_err_t err = esp_camera_init(&config);
  

  return err == ESP_OK;
}

// Upload image to Dropbox
void uploadToDropbox(camera_fb_t* fb) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }

  if (!fb || !fb->buf || fb->len == 0) {
    Serial.println("Invalid camera buffer!");
    return;
  }

  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure(); // Bypass SSL certificate validation
  if (!http.begin(client, "https://content.dropboxapi.com/2/files/upload")) {
    Serial.println("Failed to connect to Dropbox API");
    return;
  }

  // Set headers for Dropbox upload
  String apiArg = "{\"autorename\": true,\"mode\": \"add\",\"mute\": false,\"path\": \"/Apps/lampu/image.jpg\",\"strict_conflict\": false}";
  http.addHeader("Authorization", String("Bearer ") + dropboxAccessToken);
  http.addHeader("Dropbox-API-Arg", apiArg);
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("Content-Length", String(fb->len));

  // Upload the image
  int httpResponseCode = http.POST(fb->buf, fb->len);
  Serial.printf("HTTP Response code: %d\n", httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server response: " + response);
  } else {
    Serial.printf("Error during upload: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true); // Enable debug output

  // Initialize camera
  if (!initCamera()) {
    Serial.println("Camera initialization failed");
    return;
  }
  Serial.println("Camera initialized");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb || fb->len == 0) {
    Serial.println("Camera capture failed or returned empty buffer");
    return;
  }
  Serial.printf("Picture taken! Size: %d bytes\n", fb->len);

  // Upload image to Dropbox
  uploadToDropbox(fb);
  esp_camera_fb_return(fb); // Return the frame buffer to the driver for reuse

  Serial.println("All done!");
}

void loop() {
  // Empty loop
  delay(1);
}
