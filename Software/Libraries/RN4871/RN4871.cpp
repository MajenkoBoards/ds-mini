#include <RN4871.h>
#include <errno.h>

/*! Low level command-response routine.
 *
 *  Writes a command then reads up until a new-line. If a newline is not found
 *  within 1 second it times out and returns false.  An optional response buffer
 *  will receive the response the command replied with.
 *
 *  On success returns true. On error returns false. errno is set to indicate
 *  what went wrong:
 * 
 *  EBUSY: Timed out in communication
 *  EINVAL: Command replied with ERR
 */
bool RN4871::command(const char *command, const char *data, char *resp) {
    // Flush any noise from the incoming buffer
    while (_dev->available()) {
        (void)_dev->read();
    }

    _dev->print(command);
    if (data != NULL) {
        _dev->print(",");
        _dev->print(data);
    }
    _dev->print("\r");

    uint32_t timeout = millis();
    char line[5] = {0, 0, 0, 0, 0}; // Enough room for ERR and a bit.
    int lpos = 0;
    int inch;
    errno = 0;
    while (millis() - timeout < 1000) { // 1 second timeout for a response
        inch = _dev->read();
        if (inch < 0) continue;
        if (inch == '\r') {
            if (line[0] == 'E' && line[1] == 'R' && line[2] == 'R') {
                errno = EINVAL;
                return false;
            } else {
                return true;
            }
        }
        if (resp != NULL) {
            resp[lpos] = inch;
            resp[lpos+1] = 0;
        }
        if (lpos < 4) {
            line[lpos] = inch;
            line[lpos+1] = 0;
        }
        lpos++;
    }
    errno = EBUSY;
    return false;
}

/* Do initial configuration of the module */

bool RN4871::enterCommandMode() {
    delay(120);
    _dev->print("$$$");
    delay(120);
}

bool RN4871::enterDataMode() {
    _dev->print("---\r");
}

bool RN4871::begin() {
}

/* Set commands */

bool RN4871::setSerializedDeviceName(const char *name) {
    return command("S-", name);
}

bool RN4871::setCommandModeCharacter(const char *character) {
    return command("S$", character);
}

bool RN4871::setDelimiters(const char *pre, const char *post) {
    char temp[strlen(pre) + strlen(post) + 2];
    sprintf(temp, "%s,%s", pre, post);
    return command("S%", temp);
}

bool RN4871::setNVM(int address, const char *hex) {
    char temp[strlen(hex) + 6];
    sprintf(temp, "%04X,%s", address, hex);
    return command("S:", temp);
}

bool RN4871::setAuthenticationMode(int mode) {
    switch (mode) {
        case 1: return command("SA", "1");
        case 2: return command("SA", "2");
        case 3: return command("SA", "3");
    }
    errno = EINVAL;
    return false;
}

bool RN4871::setBaudRate(uint32_t baud) {
    switch (baud) {
        case 921600: return command("SB", "00");
        case 460800: return command("SB", "01");
        case 230400: return command("SB", "02");
        case 115200: return command("SB", "03");
        case  57600: return command("SB", "04");
        case  38400: return command("SB", "05");
        case  28800: return command("SB", "06");
        case  19200: return command("SB", "07");
        case  14400: return command("SB", "08");
        case   9600: return command("SB", "09");
        case   4800: return command("SB", "0A");
        case   2400: return command("SB", "0B");
    }
    errno = EINVAL;
    return false;
}

bool RN4871::setBeacon(int mode) {
    switch (mode) {
        case 0: return command("SC", "0");
        case 1: return command("SC", "1");
        case 2: return command("SC", "2");
    }
    errno = EINVAL;
    return false;
}

bool RN4871::setDISAppearance(int mode) {
    char temp[10];
    sprintf(temp, "%04X", mode);
    return command("SDA", temp);
}

bool RN4871::setDISFirmwareRevision(const char *txt) {
    return command("SDF", txt);
}

bool RN4871::setDISModelName(const char *txt) {
    return command("SDM", txt);
}

bool RN4871::setDISManufacturer(const char *txt) {
    return command("SDN", txt);
}

bool RN4871::setDISSoftwareRevision(const char *txt) {
    return command("SDR", txt);
}

bool RN4871::setDISHardwareRevision(const char *txt) {
    return command("SDH", txt);
}

bool RN4871::setDISSerialNumber(const char *txt) {
    return command("SDS", txt);
}

bool RN4871::factoryReset() {
    return command("SF", "1");
}

bool RN4871::setAdvertisementPower(int p) {
    switch (p) {
        case 0: command("SGA", "0");
        case 1: command("SGA", "1");
        case 2: command("SGA", "2");
        case 3: command("SGA", "3");
        case 4: command("SGA", "4");
        case 5: command("SGA", "5");
    }
    errno = EINVAL;
    return false;
}

bool RN4871::setConnectedPower(int p) {
    switch (p) {
        case 0: command("SGC", "0");
        case 1: command("SGC", "1");
        case 2: command("SGC", "2");
        case 3: command("SGC", "3");
        case 4: command("SGC", "4");
        case 5: command("SGC", "5");
    }
    errno = EINVAL;
    return false;
}

bool RN4871::setDeviceName(const char *name) {
    return command("SN", name);
}

bool RN4871::setPin(const char *pin) {
    return command("SP", pin);
}


bool RN4871::setFeatures(uint16_t bitmap) {
    char temp[6];
    sprintf(temp, "%04X", bitmap);
    return command("SR", temp);
}

bool RN4871::setServices(uint8_t bitmap) {
    char temp[10];
    sprintf(temp, "%02X", bitmap);
    return command("SS", temp);
}

bool RN4871::setPinFunction(int pin, int function) {
    char temp[10];
    switch(pin) {
        case 12: strcpy(temp, "0A"); break;
        case 13: strcpy(temp, "0B"); break;
        case 16: strcpy(temp, "0C"); break;
        case 17: strcpy(temp, "0D"); break;
        default:
            errno = EINVAL;
            return false;
    }
    strcat(temp, ",");
    switch (function) {
        case 0x00: strcat(temp, "00"); break;
        case 0x01: strcat(temp, "01"); break;
        case 0x02: strcat(temp, "02"); break;
        case 0x03: strcat(temp, "03"); break;
        case 0x04: strcat(temp, "04"); break;
        case 0x05: strcat(temp, "05"); break;
        case 0x06: strcat(temp, "06"); break;
        case 0x07: strcat(temp, "07"); break;
        case 0x08: strcat(temp, "08"); break;
        case 0x09: strcat(temp, "09"); break;
        case 0x0A: strcat(temp, "0A"); break;
        case 0x0B: strcat(temp, "0B"); break;
        case 0x0C: strcat(temp, "0C"); break;
        default:
            errno = EINVAL;
            return false;
    }

    return command("SW", temp);
}

/* Getter functions */

bool RN4871::getNVM(int address, int len, char *buf) {
    char temp[20];
    snprintf(temp, 20, "%04X,%02X", address, len);
    return command("G:", temp, buf);
}

bool RN4871::getConnectionStatus(char *buf) {
    return command("GK", NULL, buf);
}

bool RN4871::getPeerDeviceName(char *buf) {
    return command("GNR", NULL, buf);
}

bool RN4871::getSerializedDeviceName(char *buf) {
    return command("G-", NULL, buf);
}

char RN4871::getCommandModeCharacter() {
    char buf[10];
    if (command("G$", NULL, buf)) return 0;
    return buf[0];
}

bool RN4871::getDelimiters(char *pre, char *post) {
    char temp[20];
    if (!command("G%", NULL, temp)) {
        return false;
    }

    char *a = strtok(temp, ",");
    char *b = strtok(NULL, ",");

    if (a == NULL || b == NULL) {
        errno = ENOMSG;
        return false;
    }

    if (pre != NULL) {
        strcpy(pre, a);
    }

    if (post != NULL) {
        strcpy(post, b);
    }
    return true;
}

int RN4871::getAuthenticationMode() {
    char buf[20];
    if (command("GA", NULL, buf) == false) {
        return -1;
    }
    return atoi(buf);
}

uint32_t RN4871::getBaudRate() {
    char buf[20];
    if (command("GA", NULL, buf) == false) {
        return 0;
    }
    if (!strcasecmp(buf, "00")) return 921600;
    if (!strcasecmp(buf, "01")) return 460800;
    if (!strcasecmp(buf, "02")) return 230400;
    if (!strcasecmp(buf, "03")) return 115200;
    if (!strcasecmp(buf, "04")) return  57600;
    if (!strcasecmp(buf, "05")) return  38400;
    if (!strcasecmp(buf, "06")) return  28800;
    if (!strcasecmp(buf, "07")) return  19200;
    if (!strcasecmp(buf, "08")) return  14400;
    if (!strcasecmp(buf, "09")) return   9600;
    if (!strcasecmp(buf, "0A")) return   4800;
    if (!strcasecmp(buf, "0B")) return   2400;
    errno = EINVAL;
    return 0;
}

int RN4871::getBeacon() {
    char buf[20];
    if (command("GC", NULL, buf) == false) {
        return -1;
    }
    return atoi(buf);
}

bool RN4871::getPin(char *buf) {
    return command("GP", NULL, buf);
}

int RN4871::getFeatures() {
    char buf[10];
    if (!command("GR", NULL, buf)) {
        return -1;
    }
    return strtol(buf, NULL, 16);
}

int RN4871::getServices() {
    char buf[10];
    if (!command("GS", NULL, buf)) {
        return -1;
    }
    return strtol(buf, NULL, 16);
}

/* Action commands */

bool RN4871::echoOn() {
    char buf[20];
    if (!command("+", NULL, buf)) {
        return false;
    }
    if (!strcasecmp(buf, "Echo OFF")) { 
        // It was already on, and we just turned it off, so turn it on again.
        if (!command("+", NULL, buf)) {
            return false;
        }
    }
    return true;
}

bool RN4871::echoOff() {
    char buf[20];
    if (!command("+", NULL, buf)) {
        return false;
    }
    if (!strcasecmp(buf, "Echo ON")) { 
        // It was already off, and we just turned it on, so turn it off again.
        if (!command("+", NULL, buf)) {
            return false;
        }
    }
    return true;
}

bool RN4871::advertise() {
    return command("A", NULL);
}

bool RN4871::advertise(uint32_t interval, uint32_t period) {
    char temp[40];
    period /= 640;
    sprintf(temp, "%04X,%04X", interval & 0xFFFF, period & 0xFFFF);
    return command("A", temp);
}
    
bool RN4871::bond() {
    return command("B", NULL);
}

bool RN4871::connectLastBonded() {
    return command("C", NULL);
}

bool RN4871::connect(const char *address) {
    char temp[strlen(address) + 3];
    sprintf(temp, "0,%s", address);
    return command("C", temp);
}

bool RN4871::reboot() {
    return command("R", "1");
}


