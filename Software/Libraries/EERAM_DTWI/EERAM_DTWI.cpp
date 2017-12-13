#include <EERAM_DTWI.h>

uint8_t EERAM::read(uint16_t addr) {
    uint8_t state = 0;
    uint8_t val = 0;
    uint32_t ts = millis();
    uint8_t adata[2];
    adata[0] = addr >> 8;
    adata[1] = addr & 0xFF;
    while (1) {
        if (millis() - ts > 100) {
            _dtwi->stopMaster();
            return 0;
        }
        switch (state) {
            case 0: // begin write 
                if (_dtwi->startMasterWrite(EERAM_SRAM_ADDRESS)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (_dtwi->write(adata, 2) == 2) {
                    state = 2;
                }
                break;
            case 2: // Stop Master
                if (_dtwi->stopMaster()) {
                    state = 3;
                }
                break;
            case 3:
                if (_dtwi->startMasterRead(EERAM_SRAM_ADDRESS, 1)) {
                    state = 4;
                }
                break;
            case 4:
                if (_dtwi->available()) {
                    _dtwi->read(&val, 1);
                    state = 5;
                }
                break;
            case 5:
                if (_dtwi->stopMaster()) {
                    return val;
                }
                break;
        }
    }
}

size_t EERAM::read(uint16_t addr, uint8_t *data, size_t len) {
    uint8_t state = 0;
    uint32_t ts = millis();
    uint8_t adata[2];
    adata[0] = addr >> 8;
    adata[1] = addr & 0xFF;
    size_t toread = len;
    uint8_t *datap = data;
    while (1) {
        if (millis() - ts > 100) {
            _dtwi->stopMaster();
            return 0;
        }
        switch (state) {
            case 0: // begin write 
                if (_dtwi->startMasterWrite(EERAM_SRAM_ADDRESS)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (_dtwi->write(adata, 2) == 2) {
                    state = 2;
                }
                break;
            case 2: // Stop Master
                if (_dtwi->stopMaster()) {
                    state = 3;
                }
                break;
            case 3:
                if (_dtwi->startMasterRead(EERAM_SRAM_ADDRESS, len)) {
                    state = 4;
                }
                break;
            case 4:
                if (_dtwi->available()) {
                    size_t numread = _dtwi->read(datap, min(toread, 30));
                    datap += numread;
                    toread -= numread;
                    if (toread == 0) {
                        state = 5;
                    }
                }
                break;
            case 5:
                if (_dtwi->stopMaster()) {
                    return len;
                }
                break;
        }
    }
}

void EERAM::writeConfig(uint8_t val) {
    uint8_t state = 0;
    uint32_t ts = millis();
    uint8_t addr = 0;
    while (1) {
        if (millis() - ts > 100) {
            _dtwi->stopMaster();
        }
        switch (state) {
            case 0: // begin write 
                if (_dtwi->startMasterWrite(EERAM_CONTROL_ADDRESS)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (_dtwi->write(&addr, 1) == 1) {
                    state = 2;
                }
                break;
            case 2: // Send register address
                if (_dtwi->write(&val, 1) == 1) {
                    state = 3;
                }
                break;
            case 3: // Stop Master
                if (_dtwi->stopMaster()) {
                    return;
                }
                break;
        }
    }
}

void EERAM::write(uint16_t addr, uint8_t val) {
    uint8_t state = 0;
    uint32_t ts = millis();
    uint8_t adata[2];
    adata[0] = addr >> 8;
    adata[1] = addr & 0xFF;
    while (1) {
        if (millis() - ts > 100) {
            _dtwi->stopMaster();
        }
        switch (state) {
            case 0: // begin write 
                if (_dtwi->startMasterWrite(EERAM_SRAM_ADDRESS)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (_dtwi->write(adata, 2) == 2) {
                    state = 2;
                }
                break;
            case 2: // Send register address
                if (_dtwi->write(&val, 1) == 1) {
                    state = 3;
                }
                break;
            case 3: // Stop Master
                if (_dtwi->stopMaster()) {
                    return;
                }
                break;
        }
    }
}

void EERAM::write(uint16_t addr, uint8_t *data, size_t len) {
    uint8_t state = 0;
    uint32_t ts = millis();
    uint8_t adata[2];
    adata[0] = addr >> 8;
    adata[1] = addr & 0xFF;
    uint8_t *datap = data;
    size_t towrite = len;
    while (1) {
        if (millis() - ts > 100) {
            _dtwi->stopMaster();
        }
        switch (state) {
            case 0: // begin write 
                if (_dtwi->startMasterWrite(EERAM_SRAM_ADDRESS)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (_dtwi->write(adata, 2) == 2) {
                    state = 2;
                }
                break;
            case 2: { 
                    size_t nwritten = _dtwi->write(datap, min(towrite, 30));
                    towrite -= nwritten;
                    datap += nwritten;
                    if (towrite == 0) {
                        state = 3;
                    }
                }
                break;
            case 3: // Stop Master
                if (_dtwi->stopMaster()) {
                    return;
                }
                break;
        }
    }
}

void EERAM::begin() {
    _dtwi->beginMaster();
    writeConfig(0b00000010); // Enable ASE
}

