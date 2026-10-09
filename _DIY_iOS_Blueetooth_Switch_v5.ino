/*********************************************************************
 * DIY BLE Switch Module – nRF52 (Adafruit Feather 52 / Bluefruit)
 *
 * 4 externe Schalter (Mono-Klinkenbuchsen, INPUT_PULLUP)
 * 1 Modus-Button zum Durchschalten der Modi
 * LED rot  – blinkt X-mal bei Moduswechsel
 * LED blau – zeigt BLE-Verbindungsstatus
 *
 * ---------------------------------------------------------------
 * KONFIGURATION: Nur diesen Abschnitt anpassen
 * ---------------------------------------------------------------
 *********************************************************************/
#include <bluefruit.h>

// --- Pin-Definitionen -------------------------------------------
const int PIN_S1        = 7;   // Schalter 1 (Klinkenbuchse)
const int PIN_S2        = 11;   // Schalter 2 (Klinkenbuchse)
const int PIN_S3        = 15;   // Schalter 3 (Klinkenbuchse)
const int PIN_S4        = 16;   // Schalter 4 (Klinkenbuchse)
const int PIN_MODE_BTN  = PIN_A5;   // Modus-Wechsel-Button  (A5 = P0.29)
const int PIN_LED_RED   = PIN_A0;   // Externe rote LED  – Modus-Anzeige (A0 = P0.26)
const int PIN_LED_BLUE  = PIN_A1;   // Externe blaue LED – BLE-Status    (A1 = P0.27)

// --- BLE-Gerätename ---------------------------------------------
const char* DEVICE_NAME = "DIY_Switch";

// --- Anzahl Modi ------------------------------------------------
const int NUM_MODES = 3;

// --- Aktionstypen -----------------------------------------------
#define TYPE_NONE     0   // Schalter deaktiviert
#define TYPE_KEY      1   // Tastatur-Zeichen (HID keyPress)
#define TYPE_CONSUMER 2   // Consumer-Control (z.B. Play/Pause)

// --- HID Consumer-Codes -----------------------------------------
#define CONTROL_PLAY_PAUSE      0x00CD
#define CONTROL_SCAN_NEXT       0x00B5
#define CONTROL_SCAN_PREVIOUS   0x00B6
#define CONTROL_MUTE            0x00E2
#define AC_FORWARD              0x0225
#define AC_BACK                 0x0224
#define CONSUMER_BROWSER_HOME   0x0223

// --- Modus-Tabelle ----------------------------------------------
// Aufbau pro Eintrag: { TYPE, Wert }
// TYPE_KEY      -> Wert = ASCII-Zeichen (z.B. ' ', '\n', '\t', 27=Esc)
// TYPE_CONSUMER -> Wert = HID Consumer-Code (z.B. CONTROL_PLAY_PAUSE)
// TYPE_NONE     -> Wert wird ignoriert

struct Action {
  uint8_t  type;
  uint16_t value;
};

// [Modus][Schalter S1..S4]
const Action modeConfig[NUM_MODES][4] = {
  // Modus 0: Switch Control (Bedienungshilfen iOS/Android)
  {
    { TYPE_KEY,      ' '   },   // S1 – Leertaste  (Select / Aktivieren)
    { TYPE_KEY,      '\n'  },   // S2 – Enter
    { TYPE_KEY,      '\t'  },   // S3 – Tab        (Vorwärts navigieren)
    { TYPE_KEY,      27    },   // S4 – Escape     (Zurück / Abbrechen)
  },
  // Modus 1: Mediensteuerung
  {
    { TYPE_CONSUMER, CONTROL_PLAY_PAUSE    },   // S1 – Play/Pause
    { TYPE_CONSUMER, CONTROL_SCAN_NEXT     },   // S2 – Nächster Titel
    { TYPE_CONSUMER, CONTROL_SCAN_PREVIOUS },   // S3 – Vorheriger Titel
    { TYPE_CONSUMER, CONSUMER_BROWSER_HOME },   // S4 – Home
  },
  // Modus 2: Benutzerdefiniert – hier frei belegen
  {
    { TYPE_KEY,      'a'   },   // S1
    { TYPE_KEY,      'b'   },   // S2
    { TYPE_NONE,     0     },   // S3 – deaktiviert
    { TYPE_NONE,     0     },   // S4 – deaktiviert
  },
};

// ---------------------------------------------------------------
// Ab hier: Laufzeit-Logik – normalerweise nicht anfassen
// ---------------------------------------------------------------

BLEDis          bledis;
BLEHidAdafruit  blehid;

// Pin-Array für die 4 Schalter
const int switchPins[4] = { PIN_S1, PIN_S2, PIN_S3, PIN_S4 };

// Debounce-Status
bool prevSwitchState[4] = { HIGH, HIGH, HIGH, HIGH };
bool prevModeBtnState   = HIGH;

// Aktiver Modus
int currentMode = 0;

// BLE-Verbindungsstatus
bool bleConnected = false;

// Timing für blaues BLE-Blinken (non-blocking)
unsigned long lastBlueToggle = 0;
bool          blueState      = false;

// ---------------------------------------------------------------
// BLE Callbacks
// ---------------------------------------------------------------
void connectCallback(uint16_t conn_hdl) {
  (void) conn_hdl;
  bleConnected = true;
  digitalWrite(PIN_LED_BLUE, HIGH);   // Dauerhaft an = verbunden
  Serial.println("BLE verbunden");
}

void disconnectCallback(uint16_t conn_hdl, uint8_t reason) {
  (void) conn_hdl;
  (void) reason;
  bleConnected = false;
  digitalWrite(PIN_LED_BLUE, LOW);
  Serial.println("BLE getrennt");
}

// ---------------------------------------------------------------
// Setup
// ---------------------------------------------------------------
void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
  }
  pinMode(PIN_MODE_BTN,  INPUT_PULLUP);
  pinMode(PIN_LED_RED,   OUTPUT);
  pinMode(PIN_LED_BLUE,  OUTPUT);
  digitalWrite(PIN_LED_RED,  LOW);
  digitalWrite(PIN_LED_BLUE, LOW);

  Serial.begin(115200);
  unsigned long t = millis();
  while (!Serial && millis() - t < 2000) delay(10);

  Serial.println("DIY BLE Switch Module – Start");
  Serial.print("Modi: ");
  Serial.println(NUM_MODES);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName(DEVICE_NAME);

  // BLE Callbacks registrieren
  Bluefruit.Periph.setConnectCallback(connectCallback);
  Bluefruit.Periph.setDisconnectCallback(disconnectCallback);

  bledis.setManufacturer("Meko Mitte");
  bledis.setModel("BLE Switch v2");
  bledis.begin();

  blehid.begin();

  startAdv();

  // Startmodus anzeigen (rote LED)
  blinkMode(currentMode + 1);
}

// ---------------------------------------------------------------
// BLE Advertising
// ---------------------------------------------------------------
void startAdv() {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

// ---------------------------------------------------------------
// Rote LED blinkt n-mal (Modus-Anzeige)
// ---------------------------------------------------------------
void blinkMode(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_LED_RED, HIGH);
    delay(120);
    digitalWrite(PIN_LED_RED, LOW);
    if (i < times - 1) delay(120);
  }
}

// ---------------------------------------------------------------
// Blaue LED: langsames Blinken wenn nicht verbunden (non-blocking)
// ---------------------------------------------------------------
void updateBlueLED() {
  if (bleConnected) return;   // Verbunden → LED wird im Callback gesteuert
  unsigned long now = millis();
  if (now - lastBlueToggle >= 1000) {
    blueState = !blueState;
    digitalWrite(PIN_LED_BLUE, blueState ? HIGH : LOW);
    lastBlueToggle = now;
  }
}

// ---------------------------------------------------------------
// Consumer-Taste senden (z.B. Play/Pause)
// ---------------------------------------------------------------
void sendConsumer(uint16_t command) {
  for (uint16_t conn_hdl = 0; conn_hdl < BLE_MAX_CONNECTION; conn_hdl++) {
    BLEConnection* connection = Bluefruit.Connection(conn_hdl);
    if (connection && connection->connected() && connection->bonded()) {
      blehid.consumerKeyPress(conn_hdl, command);
      delay(10);
      blehid.consumerKeyRelease(conn_hdl);
    }
  }
  // Kurzes Feedback: blaue LED blinkt kurz ab
  if (bleConnected) {
    digitalWrite(PIN_LED_BLUE, LOW);
    delay(50);
    digitalWrite(PIN_LED_BLUE, HIGH);
  }
  delay(50);
}

// ---------------------------------------------------------------
// Tastatur-Taste senden (z.B. Leertaste, Enter)
// ---------------------------------------------------------------
void sendKey(char key) {
  blehid.keyPress(key);
  delay(10);
  blehid.keyRelease();
  // Kurzes Feedback: blaue LED blinkt kurz ab
  if (bleConnected) {
    digitalWrite(PIN_LED_BLUE, LOW);
    delay(50);
    digitalWrite(PIN_LED_BLUE, HIGH);
  }
  delay(50);
}

// ---------------------------------------------------------------
// Aktion ausführen (liest aus modeConfig)
// ---------------------------------------------------------------
void executeAction(int switchIndex) {
  Action a = modeConfig[currentMode][switchIndex];
  switch (a.type) {
    case TYPE_KEY:
      sendKey((char)a.value);
      Serial.print("KEY S");
      break;
    case TYPE_CONSUMER:
      sendConsumer(a.value);
      Serial.print("CONSUMER S");
      break;
    case TYPE_NONE:
    default:
      return;
  }
  Serial.print(switchIndex + 1);
  Serial.print(" Modus ");
  Serial.println(currentMode);
}

// ---------------------------------------------------------------
// Modus-Button verarbeiten (Flanken-Debounce)
// ---------------------------------------------------------------
void handleModeButton() {
  bool cur = digitalRead(PIN_MODE_BTN);
  if (cur == LOW && prevModeBtnState == HIGH) {
    currentMode = (currentMode + 1) % NUM_MODES;
    Serial.print("Modus gewechselt -> ");
    Serial.println(currentMode);
    delay(20);
    blinkMode(currentMode + 1);
  }
  prevModeBtnState = cur;
}

// ---------------------------------------------------------------
// Loop
// ---------------------------------------------------------------
void loop() {
  handleModeButton();
  updateBlueLED();

  for (int i = 0; i < 4; i++) {
    bool cur = digitalRead(switchPins[i]);
    if (cur == LOW && prevSwitchState[i] == HIGH) {
      executeAction(i);
    }
    prevSwitchState[i] = cur;
  }

  delay(10);
}
