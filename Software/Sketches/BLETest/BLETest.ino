#include <RN4871.h>

RN4871 BLE(Serial1);

void setup() {
    Serial.begin(115200);
    pinMode(PIN_BLUETOOTH_POWER, OUTPUT);
    digitalWrite(PIN_BLUETOOTH_POWER, HIGH);
    delay(1000);
    Serial1.begin(115200);
    BLE.begin();
    BLE.enterCommandMode();
    BLE.factoryReset();
    delay(1000);
    BLE.enterCommandMode();
    BLE.setDeviceName("DSMini0005");
    BLE.setFeatures(RN4871::Feature::NoPrompt);
    BLE.setServices(RN4871::Service::DIS | RN4871::Service::TransparentUART);
    BLE.setDISAppearance(GAP::Thermometer::Generic);
    BLE.setDISFirmwareRevision("1.0");
    BLE.setDISSoftwareRevision("1.0");
    BLE.setDISHardwareRevision("0.1Beta");
    BLE.setDISModelName("DSMini");
    BLE.setDISManufacturer("Majenko Technologies");
    BLE.setDISSerialNumber("1");
    BLE.reboot();
    BLE.enterDataMode();
}

void loop() {
    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }
    while (Serial.available()) {
        Serial1.write(Serial.read());
    }
}
