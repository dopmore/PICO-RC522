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

  while (!cardDetected) {
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
