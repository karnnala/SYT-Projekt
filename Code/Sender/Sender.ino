#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

const int BUTTON = 13;

uint8_t receiverMac[] = {0x00, 0x70, 0x07, 0x17, 0x0A, 0xE8};

typedef struct struct_message {
  bool pressed;
} struct_message;

struct_message dataToSend;
esp_now_peer_info_t peerInfo;

bool lastButtonState = HIGH;

void onDataSent(const wifi_tx_info_t info, esp_now_send_status_t status) {
  Serial.print("Sendestatus: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Erfolg");
  } else {
    Serial.println("Fehler");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUTTON, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  Serial.println("Sender startet...");
  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Fehler bei esp_now_init()");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fehler beim Hinzufuegen des Peers");
    return;
  }

  Serial.println("Sender bereit");
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    dataToSend.pressed = true;

    esp_err_t result = esp_now_send(receiverMac, (uint8_t)&dataToSend, sizeof(dataToSend));

    Serial.print("esp_now_send(): ");
    if (result == ESP_OK) {
      Serial.println("OK");
    } else {
      Serial.println("FEHLER");
    }

    delay(250);
  }

  lastButtonState = currentButtonState;
}