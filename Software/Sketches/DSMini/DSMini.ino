/*
 * Prints "Hello World" on the OLED along with the return value of millis()
 * every 100ms.
 *
 * Needs SSD1306 support from DisplayCore
 *
 * * https://github.com/MajenkoLibraries/DisplayCore
 */

#include <DisplayCore.h>
#include <DSPI.h>
#include <SSD1306.h>
#include <EMC1001_DTWI.h>
#include <RTCC.h>
#include <LowPower.h>
#include <EERAM_DTWI.h>


// Default pin modes when saving power
const uint8_t defmodes[NUM_DIGITAL_PINS] = {
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLUP, // User button
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
	INPUT, // RX
	INPUT, // TX
	INPUT_PULLDOWN,
	INPUT_PULLDOWN,
};

#define SAMPLE  0x01
#define BUTTON  0x02
#define SNOOZE  0x04
#define SLEEP   0x08

#define NUM_TEMPS 96

float temperature[NUM_TEMPS] = {0};
volatile uint32_t wakeReason = 0;

DSPI0 spi;
CLICK_OLED_B oled(spi, PIN_C1_CS, PIN_C1_PWM, PIN_C1_RST);

DTWI0 dtwi;
EMC1001 emc(dtwi);;
EERAM eeram(dtwi);

void setup() {
	pinMode(PIN_SENSOR_POWER, OUTPUT);
	digitalWrite(PIN_SENSOR_POWER, HIGH);
// Disabling I2C's power and clock, even when not in use yet, seems to kill it.
// So we need to disable things manually so as to keep I2C working.
// LowPower.disableAllPeripherals();
	LowPower.disableADC();
	LowPower.disableCTMU();
	LowPower.disableComparatorVoltageReference();
	LowPower.disableComparator1();
	LowPower.disableComparator2();
	LowPower.disableComparator3();
	LowPower.disableInputCapture1();
	LowPower.disableInputCapture2();
	LowPower.disableInputCapture3();
	LowPower.disableInputCapture4();
	LowPower.disableInputCapture5();
	LowPower.disableOutputCompare1();
	LowPower.disableOutputCompare2();
	LowPower.disableOutputCompare3();
	LowPower.disableOutputCompare4();
	LowPower.disableOutputCompare5();
	LowPower.disableTimer1();
	LowPower.disableTimer2();
	LowPower.disableTimer3();
	LowPower.enableTimer4();
	LowPower.enableTimer5();
	LowPower.disableUART1();
	LowPower.disableUART2();
	LowPower.disableSPI1();
	LowPower.enableSPI2();
	LowPower.disableI2C1();
	LowPower.enableI2C2();
	LowPower.disableUSB();
	LowPower.enableRTCC();
	initRTC();
	attachInterrupt(1, displayData, FALLING);
	pinMode(12, INPUT_PULLUP);
	loadEERAMData();
	disableSensorPower();

}

void loop() {
    static uint8_t sleepMethod = SLEEP;
    
	disableMemsOsc();
    wakeReason = 0;
    if (sleepMethod == SLEEP) {
    	LowPower.enterSleepMode();
    } else if (sleepMethod == SNOOZE) {
        LowPower.enterIdleMode();
    }
	enableMemsOsc();

    if (wakeReason & SNOOZE) {
        disableSensorPower();
        sleepMethod = SLEEP;
    }

    if (wakeReason & SAMPLE) {
		for (int i = 0; i < NUM_TEMPS - 1; i++) {
			temperature[i] = temperature[i + 1];
		}

		enableSensorPower();
		emc.begin();
		temperature[NUM_TEMPS - 1] = emc.getTemperature();
		emc.end();
		saveEERAMData();
		disableSensorPower();
	}

    if (wakeReason & BUTTON) {
		enableSensorPower();
		oled.initializeDevice();
		oled.startBuffer();
		oled.fillScreen(Color::Black);
		RTCCValue t = RTCC.value();
		oled.setCursor(0, 0);
		oled.printf("%4.2f C %02d:%02d:%02d",
		            temperature[NUM_TEMPS - 1],
		            t.hours(), t.minutes(), t.seconds()
		           );

		for (int i = 0; i < NUM_TEMPS; i += 2) {
			oled.setPixel(i, 31 - 5, Color::White);
		}

		for (int i = 0; i < NUM_TEMPS; i += 8) {
			for (int j = 0; j < 25; j += 5) {
				if (j != 5) {
					oled.setPixel(i, 31 - j, Color::White);
				}
			}
		}

		for (int i = 0; i < NUM_TEMPS - 1; i++) {
			oled.drawLine(i, (31 - 5) - temperature[i] / 2.0, i + 1, (31 - 5) - temperature[i + 1] / 2.0, Color::White);
		}

		oled.endBuffer();
        // The 10 second tick timer must use SNOOZE not SLEEP, otherwise it will have its clock turned off.
        sleepMethod = SNOOZE;
        startTick();
	}
}


void initRTC() {
	char months[26] = {4, 0, 0, 12, 0, 2, 0, 0, 0, 1, 0, 0, 3, 11, 10, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0};
	RTCC.begin();
	RTCCValue rv = RTCC.value();
	char aaStr[9] = __TIME__;
	unsigned char hours = (10 * (aaStr[0] - '0')) + aaStr[1] - '0';
	unsigned char minutes = (10 * (aaStr[3] - '0')) + aaStr[4] - '0';
	unsigned char seconds = (10 * (aaStr[6] - '0')) + aaStr[7] - '0';
	rv.time(hours, minutes, seconds);
	char bStr[12] = __DATE__;
	unsigned char year = (10 * (bStr[9] - '0')) + bStr[10] - '0';
	unsigned char month = months[bStr[0] - 'A'];

	if (month == 4 && bStr[1] == 'u') {
		month = 8;
	}

	if (month == 3 && bStr[2] != 'r') {
		month = 5;
	}

	if (month == 1 && bStr[1] == 'u') {
		if (bStr[2] == 'n') { month = 6; }
		else { month = 7; }
	}

	if (bStr[4] == ' ') { bStr[4] = '0'; }

	unsigned char day = (10 * (bStr[4] - '0')) + bStr[5] - '0';
	rv.date(year, month, day);
	rv.setValidity(RTCC_VAL_GCC); // date/time approximated using compilation time: works for developers only!!!
	RTCC.set(rv);
	rv.seconds(0); 
    rv.minutes(0); 
    rv.hours(0); 
	RTCC.alarmSet(rv);
	RTCC.alarmMask(AL_HOUR);
	RTCC.chimeEnable();
	RTCC.alarmEnable();
	RTCC.attachInterrupt(&wake);
}

void wake() {
	wakeReason = SAMPLE;
}

void displayData() {
	wakeReason = BUTTON;
}

void enableSensorPower() {
	pinMode(PIN_SENSOR_POWER, OUTPUT);
	digitalWrite(PIN_SENSOR_POWER, HIGH);
}

void disableSensorPower() {
    
	digitalWrite(PIN_SENSOR_POWER, LOW);

	for (int i = 0; i < NUM_DIGITAL_PINS; i++) {
		pinMode(i, defmodes[i]);
	}
}

void disableMemsOsc() {
	LowPower.switchToLPRC();
	TRISAbits.TRISA3 = 0;
	LATAbits.LATA3 = 0;
}

void enableMemsOsc() {
	TRISAbits.TRISA3 = 0;
	LATAbits.LATA3 = 1;
	LowPower.restoreSystemClock();
}

void loadEERAMData() {
	eeram.begin();
	uint8_t *data = (uint8_t *)temperature;

	for (int i = 0; i < (NUM_TEMPS * sizeof(float)); i++) {
		data[i] = eeram.read(i);
	}

	eeram.end();

	for (int i = 0; i < 96; i++) {
		if (
		    isnan(temperature[i]) ||
		    isinf(temperature[i]) ||
		    (temperature[i] < -100) ||
		    (temperature[i] > 100)
		) {
			temperature[i] = 0;
		}
	}
}

void saveEERAMData() {
	eeram.begin();
	uint8_t *data = (uint8_t *)temperature;

	for (int i = 0; i < (NUM_TEMPS * sizeof(float)); i++) {
		eeram.write(i, data[i]);
	}

	eeram.end();
}

void startTick() {
    LowPower.enableTimer4();

    T4CON = 0;

    // F = 32768 internal RC F_CPU (assume we switch to that soon.
    T4CONbits.TCKPS = 7; // div 256
    // F = 128Hz prescaled

    TMR4 = 0;
    PR4 = 1280; // 128 ticks per second, 10 seconds.

    setIntPriority(_TIMER_4_VECTOR, 4, 0);
    setIntVector(_TIMER_4_VECTOR, tickDone);
    clearIntFlag(_TIMER_4_IRQ);
    setIntEnable(_TIMER_4_IRQ);

    // Turn it on.
    T4CONbits.ON = 1;  
}

void __USER_ISR tickDone() {
    T4CONbits.ON = 0;
    clearIntEnable(_TIMER_4_IRQ);
    wakeReason = SNOOZE;
    LowPower.disableTimer4();
}
