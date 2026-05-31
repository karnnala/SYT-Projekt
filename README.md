# ESP32-Glücksspiel

SYT-Projekt | Von: Felix Batoha, Jakob Hofbauer, Alwin Franke

### Materialien:

- Esp32 2x

- Taster

- OLED Display

- Jumper Wire 6x

- Breadboard 2x

### Aufbau & Teilerklärung:

Ein Esp32 wurde durch Jumper Wires über GPIO 13 und Ground zu einem Taster auf einem Breadboard verbunden. Durch diesen Taster kann man den Stromfluss fließen lassen oder ihn Unterbrechen.

Der andere Esp32 wurde mit Jumper Kabeln über GPIO 21, 22, Ground und 3,3V zu dem OLED Display auf dem Breadboard (Steckbrett) verbunden. Das OLED Display hat 128x64 Pixel und 1,3 Zoll.

Der Code für die Esp32’s wurde mit Arduino IDE geschrieben wurde, welches u.A ein Programm für das Programmieren von Esp32’s ermöglicht(C/C++). Arduino ist eine
Open-Source-Plattform für Elektronik und Mikrocontroller-Projekte.

Der Esp32 ist ein Mikrocontroller, und kann per Mikro USB Kabel mit einem Computer verbunden werden. Seine Pins können mit Jumper Kabeln mit einem Breadboard verbunden werden. Anschließend kann dieser per Software (z.B Arduino) Code nutzen um z.B durch einen auf dem Breadboard platzierten Taster einen Input einzunehmen, eine Kalkulation durchzuführen, und den Output auf einem platzierten Display ausgeben.

### Code

###### Sender (Button-ESP32)

```cpp
WiFi.mode(WIFI_STA);
esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
```

Der ESP32 wird auf WLAN-Kanal 1 gesetzt. Beide Geräte müssen denselben Kanal nutzen, damit ESP-NOW funktioniert.

```cpp
memcpy(peerInfo.peer_addr, receiverMac, 6);
esp_now_add_peer(&peerInfo);
```

ESP-NOW ist ein Protokoll von Espressif. Es schickt kleine Datenpakete direkt von MAC zu MAC – ohne Router, ohne Internet. Hier wird die MAC-Adresse des Empfängers gespeichert.

```cpp
if (lastButtonState == HIGH && currentButtonState == LOW) {
  dataToSend.pressed = true;
  esp_now_send(receiverMac, (uint8_t*)&dataToSend, sizeof(dataToSend));
}
```

Nur beim Drücken (HIGH→LOW) wird gesendet. Das verhindert, dass beim Halten dauernd Pakete geschickt werden.

---

### Empfänger (Display-ESP32)

```cpp
void onDataRecv(...) {
  if (random(0, 5) == 3) {
    number = 1;
    losses++;
  } else {
    number++;
    wins++;
  }
}
```

**Gewinnlogik:** Bei jedem Knopfdruck gibt es eine 1-in-5-Chance zu verlieren. Verlieren setzt den Score auf 1 zurück. Gewinnen erhöht ihn um 1.

```cpp
if (number > highscore) highscore = number;
```

Der Highscore wird nur überschrieben, wenn der aktuelle Score höher ist.

```cpp
display.setTextSize(3);
display.setCursor((SCREEN_WIDTH - numText.length() * 18) / 2, 24);
display.print(numText);
display.display();
```

**Display:** Die Zahl wird zentriert in großer Schrift angezeigt. `display()` überträgt es erst dann auf den Bildschirm.

```cpp
WiFi.begin(ssid, password);
server.on("/", handleRoot);
server.begin();,0
```

**Webserver:** Der Empfänger verbindet sich mit WLAN und startet einen Webserver auf Port 80. Jeder im selben Netz kann die Statistik im Browser öffnen.

```cpp
new Chart(document.getElementById('c'), {type:'doughnut', ...})
```

Die Webseite lädt Chart.js und zeigt Wins/Losses als Donut-Diagramm. Die Seite aktualisiert sich alle 2 Sekunden automatisch.
