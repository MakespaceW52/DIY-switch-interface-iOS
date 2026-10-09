# DIY BLE Switch Interface Module

> **A DIY Bluetooth accessibility switch interface for people with motor impairments — inspired by commercial devices like the AbleNet Blue2.**

![Eine erwachsene Person übergibt eine leuchtende Oktopus-Platine an ein Kind.](https://www.tjfbg.de/fileadmin/tjfbg/user_upload/aktuelles/2025/bk_/TINCON_News.png)\
Photo: © TINCON
---

Bei Fragen / for questions contact: makespace@tjfbg.de

## English

### What is this?

The DIY BLE Switch Interface Module is an open-source Bluetooth HID adapter that connects up to **4 external switches** (via 3.5mm mono jack sockets) to iOS devices via Bluetooth Low Energy. A mode button cycles through configurable action profiles. Two LEDs indicate the current mode and Bluetooth connection status.

This project was developed at **Meko Mitte**, in their public makerspace for children and young people in Berlin, as an affordable and adaptable alternative to commercial assistive technology devices.

### Use Cases

- **Switch Control (iOS)** — Navigate the operating system's built-in accessibility features using one or more external switches (e.g. sip-and-puff, head switch, hand switch)
- **Media Control** — Play/pause, next/previous track, home button — for people who cannot easily reach device controls
- **Custom Input** — Any key combination or consumer HID command, freely configurable per switch and per mode
- **AAC Support** — Can be used as a switch interface for augmentative and alternative communication (AAC) apps

### Components

| Component | Description |
|---|---|
| **Adafruit Feather nRF52832 Bluefruit LE** | Main microcontroller board with integrated BLE (nRF52832 chip). Runs the Arduino sketch and handles all Bluetooth HID communication. |
| **3.5mm Mono Jack Sockets (×4)** | Input connectors for external switches. Tip → GPIO pin, Sleeve → GND. Compatible with most standard assistive technology switches. |
| **Push Button (momentary, NO)** | Mode selection button. Cycles through the configured action profiles. Each mode change is confirmed by the red LED blinking. |
| **LED red (3mm diffuse)** | Mode indicator. Blinks X times to confirm the active mode (1× = Mode 0, 2× = Mode 1, 3× = Mode 2). |
| **LED blue (3mm diffuse)** | Bluetooth status indicator. Slow blink = searching for device. Steady on = connected. Brief off = command sent. |
| **Resistor 330Ω** | Current limiting resistor for the red LED. |
| **Resistor 100Ω** | Current limiting resistor for the blue LED (higher forward voltage). |
| **Toggle Switch SPDT (2MS1)** | Power switch. Connected to the EN pin of the Feather to disable the 3.3V regulator when off. Protects the LiPo battery from deep discharge. |
| **LiPo Battery** | Powers the module wirelessly. The Feather's onboard charging circuit charges the battery via USB. |
| **Custom PCB** | KiCad-designed carrier board for all components. Gerber files included. |
| **3D Printed Enclosure** | Printable housing (FDM, PETG recommended). Source files and STLs included. |

### Modes (configurable in code)

| Mode | S1 | S2 | S3 | S4 |
|---|---|---|---|---|
| 0 – Switch Control | Space | Enter | Tab | Escape |
| 1 – Media | Play/Pause | Next | Previous | Home |
| 2 – Custom | 'a' | 'b' | disabled | disabled |

### Pin Assignment

| Function | Pin | Note |
|---|---|---|
| Switch 1 | 7 | INPUT_PULLUP |
| Switch 2 | 11 | INPUT_PULLUP |
| Switch 3 | 15 | INPUT_PULLUP |
| Switch 4 | 16 | INPUT_PULLUP |
| Mode Button | A5 (P0.29) | INPUT_PULLUP |
| LED red | A0 (P0.26) | via 330Ω |
| LED blue | A1 (P0.27) | via 100Ω |
| Power Switch | EN pin | SPDT to GND |

### Build Your Own

1. **Order the PCB** — Upload the Gerber files from `/pcb/` to a PCB manufacturer (e.g. JLCPCB, PCBWay, Aisler)
2. **Print the enclosure** — Print the files from `/case/` in PLA. Layer height 0.2mm, 3–4 wall lines.
3. **Solder the components** — Follow the schematic in kicad files in `/pcb/`
4. **Flash the firmware** — Open `_DIY_iOS_Blueetooth_Switch_v5.ino` in Arduino IDE, select board `Adafruit Bluefruit nRF52832 Feather`, upload
5. **Pair via Bluetooth** — The device appears as `DIY_Switch`. Enable Switch Control or accessibility features on your device and connect.

> **Note on power:** The EN-pin switch cuts the 3.3V regulator. The LiPo charging circuit remains active. Always use a protected LiPo battery.

### License

Open source — built with ❤️ at Meko Mitte for and with the community.

---

## Deutsch

### Was ist das?

Das DIY BLE Switch Interface Module ist ein Open-Source Bluetooth-HID-Adapter, der bis zu **4 externe Schalter** (über 3,5mm Mono-Klinkenbuchsen) per Bluetooth Low Energy mit iOS verbindet. Ein Modus-Button schaltet zwischen konfigurierbaren Aktionsprofilen um. Zwei LEDs zeigen den aktiven Modus und den Bluetooth-Verbindungsstatus an.

Das Projekt entstand im **Meko Mitte**, in deren öffentlichem Makespace für Kinder und Jugendliche in Berlin, als erschwingliche und anpassbare Alternative zu kommerziellen Hilfsmittelgeräten.

### Anwendungsszenarien

- **Switch Control (iOS)** — Steuerung der eingebauten Bedienungshilfen des Betriebssystems mit einem oder mehreren externen Schaltern (z.B. Sip-and-Puff, Kopfschalter, Handschalter)
- **Mediensteuerung** — Play/Pause, Nächster/Vorheriger Titel, Home-Taste — für Menschen, die Gerätebedienelemente schwer erreichen können
- **Benutzerdefinierte Eingabe** — Beliebige Tastenkombinationen oder Consumer-HID-Befehle, frei konfigurierbar pro Schalter und pro Modus
- **UK / Unterstützte Kommunikation** — Einsatz als Schalterinterface für UK-Apps (Unterstützte und Alternative Kommunikation)

### Komponenten

| Komponente | Beschreibung |
|---|---|
| **Adafruit Feather nRF52832 Bluefruit LE** | Haupt-Mikrocontroller-Board mit integriertem BLE (nRF52832-Chip). Führt den Arduino-Sketch aus und übernimmt die gesamte Bluetooth-HID-Kommunikation. |
| **3,5mm Mono-Klinkenbuchsen (×4)** | Eingänge für externe Schalter. Tip → GPIO-Pin, Sleeve → GND. Kompatibel mit den meisten Standard-Hilfsmittelschaltern. |
| **Drucktaster (momentan, NO)** | Modus-Wahltaster. Schaltet durch die konfigurierten Aktionsprofile. Jeder Moduswechsel wird durch Blinken der roten LED bestätigt. |
| **LED rot (3mm diffus)** | Modus-Anzeige. Blinkt X-mal zur Bestätigung des aktiven Modus (1× = Modus 0, 2× = Modus 1, 3× = Modus 2). |
| **LED blau (3mm diffus)** | Bluetooth-Statusanzeige. Langsames Blinken = Suche nach Gerät. Dauerhaft an = verbunden. Kurz aus = Befehl gesendet. |
| **Widerstand 330Ω** | Vorwiderstand für die rote LED. |
| **Widerstand 100Ω** | Vorwiderstand für die blaue LED (höhere Flussspannung). |
| **Kippschalter SPDT (2MS1)** | Ein-/Ausschalter. Am EN-Pin des Feather angeschlossen, um den 3,3V-Regler abzuschalten. Schützt den LiPo-Akku vor Tiefentladung. |
| **LiPo-Akku** | Versorgt das Modul kabellos mit Strom. Der integrierte Ladekreis des Feather lädt den Akku per USB. |
| **Eigene Platine (PCB)** | In KiCad entworfene Trägerplatine für alle Komponenten. Gerber-Dateien inklusive. |
| **3D-gedrucktes Gehäuse** | Druckbares Gehäuse (FDM, PETG empfohlen). Quelldateien und STLs inklusive. |

### Modi (im Code konfigurierbar)

| Modus | S1 | S2 | S3 | S4 |
|---|---|---|---|---|
| 0 – Switch Control | Leertaste | Enter | Tab | Escape |
| 1 – Medien | Play/Pause | Weiter | Zurück | Home |
| 2 – Benutzerdefiniert | 'a' | 'b' | deaktiviert | deaktiviert |

### Pinbelegung

| Funktion | Pin | Hinweis |
|---|---|---|
| Schalter 1 | 7 | INPUT_PULLUP |
| Schalter 2 | 11 | INPUT_PULLUP |
| Schalter 3 | 15 | INPUT_PULLUP |
| Schalter 4 | 16 | INPUT_PULLUP |
| Modus-Button | A5 (P0.29) | INPUT_PULLUP |
| LED rot | A0 (P0.26) | über 330Ω |
| LED blau | A1 (P0.27) | über 100Ω |
| Ein-/Ausschalter | EN-Pin | SPDT nach GND |

### Selbstbau

1. **Platine bestellen** — Gerber-Dateien aus `/pcb/` bei einem PCB-Hersteller hochladen (z.B. JLCPCB, PCBWay, Aisler)
2. **Gehäuse drucken** — Dateien aus `/case/` in PLA drucken.
3. **Komponenten löten** — Schaltplan in kicad datei `/pcb/` als Vorlage nutzen
4. **Firmware flashen** — `_DIY_iOS_Blueetooth_Switch_v5.ino` in der Arduino IDE öffnen, Board `Adafruit Bluefruit nRF52832 Feather` auswählen, hochladen
5. **Bluetooth koppeln** — Das Gerät erscheint als `DIY_Switch`. Switch Control oder Bedienungshilfen am Zielgerät aktivieren und verbinden.

> **Hinweis zum Akku:** Der EN-Pin-Schalter trennt den 3,3V-Regler. Der LiPo-Ladekreis bleibt aktiv. Bitte immer geschützte LiPo-Akkus verwenden.

### Lizenz

Open Source — mit ❤️ bei Meko Mitte für und mit der Community entstanden.
