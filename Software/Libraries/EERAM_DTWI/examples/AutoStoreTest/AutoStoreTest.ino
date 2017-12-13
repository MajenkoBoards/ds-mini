#include <DTWI.h>
#include <EERAM_DTWI.h>

DTWI0 dtwi;
EERAM eeram(dtwi);

void setup() {

  pinMode(PIN_SENSOR_POWER, OUTPUT);
  digitalWrite(PIN_SENSOR_POWER, HIGH);

  eeram.begin();
  Serial.begin(115200);
}

#define EERAM_SIZE (16384 / 8)

void loop() {
  uint8_t data[EERAM_SIZE];

  Serial.print("Bytewise Writing...");
  for (int i = 0; i < EERAM_SIZE; i++) {
    data[i] = rand();
    eeram.write(i, data[i]);
  }
  Serial.println("done");
  
  Serial.print("Cutting power...");
  digitalWrite(PIN_SENSOR_POWER, LOW);
  delay(1000);
  Serial.println("done");

  Serial.print("Sleeping...");
  delay(10000);
  Serial.println("done");
  
  Serial.print("Restoring power...");
  digitalWrite(PIN_SENSOR_POWER, HIGH);
  delay(1000);
  Serial.println("done");
  
  
  Serial.print("Bytewise Reading...");
  for (int i = 0; i < EERAM_SIZE; i++) {
    uint8_t val = eeram.read(i);
    if (val != data[i]) {
      Serial.printf("Mismatch at %d ", i);
    }
  }
  Serial.println("done");

  delay(1000);
}

