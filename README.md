# RC522 RFID Lesemodul mit Raspberry Pi Pico auf PlatformIO

Willkommen zum Projekt, das die Verwendung des RC522 RFID-Lesemoduls mit einem Raspberry Pi Pico unter PlatformIO zeigt und ein Beispiel Projekt.

## Einleitung

Funktionen des Beispiel-Projektes:

- RFID-Tags mit dem RC522 RFID-Lesemodul lesen.
- Die UID der ersten erkannten Karte speichern.
- Nachfolgende Kartenscans gegen die gespeicherte UID authentifizieren.

## Voraussetzungen

### Hardware

- Raspberry Pi Pico
- RC522 RFID-Lesemodul
- Breadboard und Jumper-Kabel
- LEDs (Rot und Grün) und 220Ω-Widerstände

### Verbindungen

Stelle die folgenden Verbindungen her:

| RC522-Pin | Pico-Pin        |
|-----------|-----------------|
| SDA       | GP17 (Pin 22)   |
| SCK       | GP18 (Pin 24)   |
| MOSI      | GP19 (Pin 25)   |
| MISO      | GP16 (Pin 21)   |
| GND       | GND (Pin 3)     |
| RST       | GP20 (Pin 26)   |
| 3.3V      | 3V3 (Pin 36)    |

LED-Verbindungen:

| LED   | Pico-Pin       |
|-------|----------------|
| Rot   | GP11 (Pin 17)  |
| Grün  | GP12 (Pin 16)  |

## Software

### Installation von PlatformIO

1. Installiere die PlatformIO IDE für VSCode (Anleitung: [PlatformIO-Website](https://platformio.org/install/ide?install=vscode)).

### Projekt einrichten

1. Öffne VSCode und klicke auf das PlatformIO-Symbol.
2. Klicke auf "New Project".
3. Benenne das Projekt und wählen Sie "Raspberry Pi Pico" als Board.
4. Wähle "Arduino" als Framework.
5. Klicke auf "Finish".

### Abhängigkeiten hinzufügen

Füge die folgenden Bibliotheken in Ihrer `platformio.ini`-Datei hinzu:

```ini
[env:pico]
platform = raspberrypi
board = pico
framework = arduino
lib_deps =
    computer991/Arduino_MFRC522v2@^2.0.1
```
### Code-Implementierung

Ersetze den Inhalt von src/main.cpp durch den folgenden Code:

```cpp
Code kopieren
#include <Arduino.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>

MFRC522DriverPinSimple ss_pin(17); // Configurable, see typical pin layout above.
MFRC522DriverSPI driver{ss_pin};   // Create SPI driver.
MFRC522 mfrc522{driver};           // Create MFRC522 instance.

byte SavedUid[8] = {0, 0, 0, 0};    // Buffer to store the last scanned UID

void saveUid(byte *uid, byte *SavedUid);
void printUID();
bool compareUid(byte *uid_comparator, byte *uid_reference);

bool cardDetected = false;

#define RED 11
#define GREEN 12

void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC for debugging.
  while (!Serial);       // Wait for serial port to connect (for platforms like ATMEGA32U4).

  mfrc522.PCD_Init();    // Initialize MFRC522 module.

  Serial.println("Tap Authentication Card");

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);

  while (!cardDetected) {//wait until card is Detected and save as the key
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      saveUid(mfrc522.uid.uidByte, SavedUid); // Save the UID of the detected card
      Serial.print("Using: ");
      printUID();
      cardDetected = true; // Set flag to true to exit the loop
    }
    delay(50); // Small delay to prevent high CPU usage
  }
  delay(2000);
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Compare with the last scanned UID
  if (compareUid(mfrc522.uid.uidByte, SavedUid)) {
    Serial.println("Authenticated - Same Card Detected!");
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
  } else {
    Serial.println("Unauthorized Access - Please Remove Device");
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
  }

  // Show the UID on the serial monitor
  printUID();

  // Halt PICC (Proximity Integrated Circuit Card)
  mfrc522.PICC_HaltA();
}

bool compareUid(byte *uid_comparator, byte *uid_reference) {
  for (int i = 0; i < 4; i++) {
    if (uid_comparator[i] != uid_reference[i]) {
      return false;
    }
  }
  return true;
}

void saveUid(byte *uid, byte *SavedUid) {
  for (int i = 0; i < 4; i++) {
    SavedUid[i] = uid[i];
  }
}

void printUID() {
  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
}
```
### Code hochladen

1. Verbinde Ihren Raspberry Pi Pico mit Ihrem Computer. (USB)
2. Klicke auf die Schaltfläche "Upload" (PFEIL) in PlatformIO, um den Code zu kompilieren und auf den Pico hochzuladen.
   
### Testen
1. Öffne den Serial Monitor in PlatformIO.
2. Scanne eine RFID-Karte oder einen Tag am RC522-Modul. Beim nächsten Scannen wird die UID der Karte angezeigt und die grüne LED leuchtet auf, wenn sie mit der gespeicherten UID übereinstimmt. Andernfalls leuchtet die rote LED auf.

## Weitere Informationen

### Bibliotheken

Verwenden Sie die "Arduino_MFRC522v2" Bibliothek zur Ansteuerung des RFID-Lesemoduls.
