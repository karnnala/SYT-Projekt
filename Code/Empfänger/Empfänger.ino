#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int number = 1;
int highscore = 1;
int wins = 0;
int losses = 0;
String statusText = "Bereit";

const char* ssid = "Cuxxles";
const char* password = "132456777";

WebServer server(80);

typedef struct struct_message {
  bool pressed;
} struct_message;

struct_message incomingData;

void showScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  String hsText = "HS:" + String(highscore);
  display.setCursor(SCREEN_WIDTH - hsText.length() * 6, 0);
  display.print(hsText);

  display.setTextSize(3);
  String numText = String(number);
  display.setCursor((SCREEN_WIDTH - numText.length() * 18) / 2, 24);
  display.print(numText);
  display.display();
}

void handleRoot() {
  int total = wins + losses;
  float winPct  = total > 0 ? (wins  * 100.0 / total) : 0;
  float lossPct = total > 0 ? (losses * 100.0 / total) : 0;

  String html = R"(<!DOCTYPE html><html><head><meta charset='UTF-8'>
<meta http-equiv='refresh' content='2'>
<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
<style>body{background:#111;color:#eee;display:flex;flex-direction:column;align-items:center;font-family:sans-serif}canvas{max-width:300px}</style>
</head><body><h2>Win / Loss</h2>
<canvas id='c'></canvas>
<script>
new Chart(document.getElementById('c'),{type:'doughnut',data:{
  labels:['Gewonnen )" + String(winPct,1) + R"(%','Verloren )" + String(lossPct,1) + R"(%'],
  datasets:[{data:[)" + String(wins) + "," + String(losses) + R"(],backgroundColor:['#4caf50','#f44336']}]
},options:{plugins:{legend:{labels:{color:'#eee',font:{size:16}}}}}});
</script>
<p>Spiele: )" + String(total) + R"(</p>
<p>Score: )" + String(number) + R"(</p>
<p>Highscore: )" + String(highscore) + R"(</p>
<p>Status: )" + statusText + R"(</p>
</body></html>)";

  server.send(200, "text/html", html);
}

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingDataBytes, int len) {
  if (len != sizeof(incomingData)) return;
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  if (incomingData.pressed) {
    if (random(0, 5) == 3) {
      number = 1;
      statusText = "Verloren";
      losses++;
    } else {
      number++;
      statusText = "Gewonnen";
      wins++;
    }

    if (number > highscore) highscore = number;
    showScreen();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nVerbunden! IP: " + WiFi.localIP().toString());

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.display();
  randomSeed(analogRead(34));
  showScreen();

  if (esp_now_init() != ESP_OK) return;
  esp_now_register_recv_cb(onDataRecv);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}