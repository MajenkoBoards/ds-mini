/*
 * Copyright (c) 2016, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RN4871_H
#define _RN4871_H

#include <Arduino.h>

class GAP {
    public:
        static const uint16_t Unknown = 0;
        class Phone {
            public:
                static const uint16_t Generic                       = 64;
        };
        class Computer {
            public:
                static const uint16_t Generic                       = 128;
        };
        class Watch {
            public:
                static const uint16_t Generic                       = 192;
                static const uint16_t Sports                        = 193;
        };
        class Clock {
            public:
                static const uint16_t Generic                       = 256;
        };
        class Display {
            public:
                static const uint16_t Generic                       = 320;
        };
        class RemoteControl {
            public:
                static const uint16_t Generic                       = 384;
        };
        class EyeGlasses {
            public:
                static const uint16_t Generic                       = 448;
        };
        class Tag {
            public:
                static const uint16_t Generic                       = 512;
        };
        class Keyring {
            public:
                static const uint16_t Generic                       = 576;
        };
        class MediaPlayer {
            public:
                static const uint16_t Generic                       = 640;
        };
        class BarcodeScanner {
            public:
                static const uint16_t Generic                       = 704;
        };
        class Thermometer {
            public:
                static const uint16_t Generic                       = 768;
                static const uint16_t Ear                           = 769;
        };
        class HeartRate {
            public:
                static const uint16_t Generic                       = 832;
                static const uint16_t Belt                          = 833;
        };
        class BloodPressure {
            public:
                static const uint16_t Generic                       = 896;
                static const uint16_t Arm                           = 897;
                static const uint16_t Wrist                         = 898;
        };
        class HID {
            public:
                static const uint16_t Generic                       = 960;
                static const uint16_t Keyboard                      = 961;
                static const uint16_t Mouse                         = 962;
                static const uint16_t Joystick                      = 963;
                static const uint16_t Gamepad                       = 964;
                static const uint16_t Tablet                        = 965;
                static const uint16_t CardReader                    = 966;
                static const uint16_t Pen                           = 967;
                static const uint16_t BarcodeScanner                = 968;
        };
        class RunningWalking {
            public:
                static const uint16_t Generic                       = 1088;
                static const uint16_t InShoe                        = 1089;
                static const uint16_t OnShoe                        = 1090;
                static const uint16_t OnHip                         = 1091;
        };
        class GlucoseMeter {
            public:
                static const uint16_t Generic                       = 1024;
        };
        class Cycling {
            public:
                static const uint16_t Generic                       = 1152;
                static const uint16_t CyclingComputer               = 1153;
                static const uint16_t SpeedSensor                   = 1154;
                static const uint16_t CadenceSensor                 = 1155;
                static const uint16_t PowerSensor                   = 1156;
                static const uint16_t SpeedAndCadenceSensor         = 1157;
        };
        class PulseOximeter {
            public:
                static const uint16_t Generic                       = 3136;
                static const uint16_t Fingertip                     = 3137;
                static const uint16_t Wrist                         = 3138;
        };
        class WeightScale {
                static const uint16_t Generic                       = 3200;
        };
        class OutdoorSports {
                static const uint16_t Generic                       = 5184;
                static const uint16_t LocationDisplay               = 5185;
                static const uint16_t LocationAndNavigationDisplay  = 5186;
                static const uint16_t LocationPod                   = 5187;
                static const uint16_t LocationAndNavigationPod      = 5188;
        };
};

class RN4871 : public Stream {
    private:
        Stream *_dev;

        bool command(const char *command, const char *data, char *resp = NULL);
        

    public:

        class Service {
            public:
                static const uint8_t DIS                = 0x80;
                static const uint8_t TransparentUART    = 0x40;
                static const uint8_t Beacon             = 0x20;
                static const uint8_t Airpatch           = 0x10;
        };

        class Feature {
            public:
                static const uint16_t FlowControl       = 0x8000;
                static const uint16_t NoPrompt          = 0x4000;
                static const uint16_t FastMode          = 0x2000;
                static const uint16_t NoBeaconScan      = 0x1000;
                static const uint16_t NoConnectScan     = 0x0800;
                static const uint16_t NoDuplicateFilter = 0x0400;
                static const uint16_t PassiveScan       = 0x0200;
                static const uint16_t UARTWithoutACK    = 0x0100;
                static const uint16_t RebootAfterDisc   = 0x0080;
                static const uint16_t RunScriptAtPower  = 0x0040;
                static const uint16_t MLDPStreaming     = 0x0020;
        };

        RN4871(Stream *dev) : _dev(dev) {}
        RN4871(Stream &dev) : _dev(&dev) {}

        bool enterCommandMode();
        bool enterDataMode();
        bool begin();

        bool setSerializedDeviceName(const char *name);
        bool setCommandModeCharacter(const char *character);
        bool setDelimiters(const char *pre, const char *post);
        bool setNVM(int address, const char *hex);
        bool setAuthenticationMode(int mode);
        bool setBaudRate(uint32_t baud);
        bool setBeacon(int mode);
        bool setDISAppearance(int mode);
        bool setDISFirmwareRevision(const char *txt);
        bool setDISModelName(const char *txt);
        bool setDISManufacturer(const char *txt);
        bool setDISSoftwareRevision(const char *txt);
        bool setDISHardwareRevision(const char *txt);
        bool setDISSerialNumber(const char *txt);
        bool factoryReset();
        bool setAdvertisementPower(int p);
        bool setConnectedPower(int p);
        bool setDeviceName(const char *name);
        bool setPin(const char *pin);
        bool setFeatures(uint16_t bitmap);
        bool setServices(uint8_t bitmap);
        bool setPinFunction(int pin, int function);

        bool getNVM(int address, int len, char *buf);
        bool getConnectionStatus(char *buf);
        bool getPeerDeviceName(char *buf);
        bool getSerializedDeviceName(char *buf);
        char getCommandModeCharacter();
        bool getDelimiters(char *pre, char *post);
        int getAuthenticationMode();
        uint32_t getBaudRate();
        int getBeacon();
        bool getPin(char *buf);
        int getFeatures();
        int getServices();
        bool echoOn();
        bool echoOff();
        bool advertise();
        bool advertise(uint32_t interval, uint32_t period);
        bool bond();
        bool connectLastBonded();
        bool connect(const char *address);
        bool reboot();

        size_t write(uint8_t c) { return _dev->write(c); }
        int read() { return _dev->read(); }
        int available() { return _dev->available(); }
        int peek() { return _dev->peek(); }
        void flush() { _dev->flush(); }

};

#endif
