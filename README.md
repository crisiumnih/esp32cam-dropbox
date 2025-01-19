# ESP32-CAM Dropbox Image Uploader
Capture images with ESP32-CAM and upload them to Dropbox.

### What It Does

  - Takes photos using the ESP32-CAM.

- Connects to Wi-Fi and uploads the photos to Dropbox.

- Press reset capture again.

### Current Problems

  - Greenish Photos

  Photos sometimes look too green. This can be fixed by adjusting camera settings like brightness and white balance.

- Manual Reset Needed

  Right now, you need to press the reset button to take another photo. A future update will let the button take photos directly.

### Future Plans

  1. Improve photo quality by tweaking camera settings.

  2. Make the button capture and upload photos without resetting the ESP32.

#### Replace these placeholders with your details:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* dropboxAccessToken = "YOUR_DROPBOX_TOKEN";
```
