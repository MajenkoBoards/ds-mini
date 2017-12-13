#ifndef _EERAM_DTWI_H
#define _EERAM_DTWI_H

#include <Arduino.h>
#include <DTWI.h>

#define EERAM_SRAM_ADDRESS      0x50
#define EERAM_CONTROL_ADDRESS   0x18

class EERAM {
    private:
        DTWI *_dtwi;

        void writeConfig(uint8_t val);

    public:

        EERAM(DTWI *d) : _dtwi(d) {}
        EERAM(DTWI &d) : _dtwi(&d) {}
        
        void begin();
        uint8_t read(uint16_t addr);
        size_t read(uint16_t addr, uint8_t *data, size_t len);
        void write(uint16_t addr, uint8_t v);
        void write(uint16_t addr, uint8_t *data, size_t len);
};

#endif
