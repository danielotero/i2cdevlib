/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Francesco Ferrara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=============================================== */

#include <FastWire.h>
#include <I2Cdev.h>

#include <avr/io.h>


bool Fastwire::waitInt() {
    int l = 250;
    while (!(TWCR & (1 << TWINT)) && l-- > 0);
    return l > 0;
}

void Fastwire::setup(int khz, bool pullup) {
    TWCR = 0;
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
        // activate internal pull-ups for twi (PORTC bits 4 & 5)
        // as per note from atmega8 manual pg167
        if (pullup) PORTC |= ((1 << 4) | (1 << 5));
        else        PORTC &= ~((1 << 4) | (1 << 5));
    #elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
        // activate internal pull-ups for twi (PORTC bits 0 & 1)
        if (pullup) PORTC |= ((1 << 0) | (1 << 1));
        else        PORTC &= ~((1 << 0) | (1 << 1));
    #else
        // activate internal pull-ups for twi (PORTD bits 0 & 1)
        // as per note from atmega128 manual pg204
        if (pullup) PORTD |= ((1 << 0) | (1 << 1));
        else        PORTD &= ~((1 << 0) | (1 << 1));
    #endif

    TWSR = 0; // no prescaler => prescaler = 1
    TWBR = ((16000L / khz) - 16) / 2; // change the I2C clock rate
    TWCR = 1 << TWEN; // enable twi module, no interrupt
}

// added by Jeff Rowberg 2013-05-07:
// Arduino Wire-style "beginTransmission" function
// (takes 7-bit device address like the Wire method, NOT 8-bit: 0x68, not 0xD0/0xD1)
uint8_t Fastwire::beginTransmission(uint8_t device) {
    uint8_t twst, retry;
    retry = 2;
    do {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (1 << TWSTA);
        if (!waitInt()) return 1;
        twst = TWSR & 0xF8;
        if (twst != TW_START && twst != TW_REP_START) return 2;

        //Serial.print(device, HEX);
        //Serial.print(" ");
        TWDR = device << 1; // send device address without read bit (1)
        TWCR = (1 << TWINT) | (1 << TWEN);
        if (!waitInt()) return 3;
        twst = TWSR & 0xF8;
    } while (twst == TW_MT_SLA_NACK && retry-- > 0);
    if (twst != TW_MT_SLA_ACK) return 4;
    return 0;
}

uint8_t Fastwire::writeBuf(uint8_t device, uint8_t address, uint8_t *data, uint8_t num) {
    uint8_t twst, retry;

    retry = 2;
    do {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (1 << TWSTA);
        if (!waitInt()) return 1;
        twst = TWSR & 0xF8;
        if (twst != TW_START && twst != TW_REP_START) return 2;

        //Serial.print(device, HEX);
        //Serial.print(" ");
        TWDR = device & 0xFE; // send device address without read bit (1)
        TWCR = (1 << TWINT) | (1 << TWEN);
        if (!waitInt()) return 3;
        twst = TWSR & 0xF8;
    } while (twst == TW_MT_SLA_NACK && retry-- > 0);
    if (twst != TW_MT_SLA_ACK) return 4;

    //Serial.print(address, HEX);
    //Serial.print(" ");
    TWDR = address; // send data to the previously addressed device
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!waitInt()) return 5;
    twst = TWSR & 0xF8;
    if (twst != TW_MT_DATA_ACK) return 6;

    for (uint8_t i = 0; i < num; i++) {
        //Serial.print(data[i], HEX);
        //Serial.print(" ");
        TWDR = data[i]; // send data to the previously addressed device
        TWCR = (1 << TWINT) | (1 << TWEN);
        if (!waitInt()) return 7;
        twst = TWSR & 0xF8;
        if (twst != TW_MT_DATA_ACK) return 8;
    }
    //Serial.print("\n");

    return 0;
}

uint8_t Fastwire::write(uint8_t value) {
    uint8_t twst;
    //Serial.println(value, HEX);
    TWDR = value; // send data
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!waitInt()) return 1;
    twst = TWSR & 0xF8;
    if (twst != TW_MT_DATA_ACK) return 2;
    return 0;
}

uint8_t Fastwire::readBuf(uint8_t device, uint8_t address, uint8_t *data, uint8_t num) {
    uint8_t twst, retry;

    retry = 2;
    do {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (1 << TWSTA);
        if (!waitInt()) return 16;
        twst = TWSR & 0xF8;
        if (twst != TW_START && twst != TW_REP_START) return 17;

        //Serial.print(device, HEX);
        //Serial.print(" ");
        TWDR = device & 0xfe; // send device address to write
        TWCR = (1 << TWINT) | (1 << TWEN);
        if (!waitInt()) return 18;
        twst = TWSR & 0xF8;
    } while (twst == TW_MT_SLA_NACK && retry-- > 0);
    if (twst != TW_MT_SLA_ACK) return 19;

    //Serial.print(address, HEX);
    //Serial.print(" ");
    TWDR = address; // send data to the previously addressed device
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!waitInt()) return 20;
    twst = TWSR & 0xF8;
    if (twst != TW_MT_DATA_ACK) return 21;

    /***/

    retry = 2;
    do {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (1 << TWSTA);
        if (!waitInt()) return 22;
        twst = TWSR & 0xF8;
        if (twst != TW_START && twst != TW_REP_START) return 23;

        //Serial.print(device, HEX);
        //Serial.print(" ");
        TWDR = device | 0x01; // send device address with the read bit (1)
        TWCR = (1 << TWINT) | (1 << TWEN);
        if (!waitInt()) return 24;
        twst = TWSR & 0xF8;
    } while (twst == TW_MR_SLA_NACK && retry-- > 0);
    if (twst != TW_MR_SLA_ACK) return 25;

    for (uint8_t i = 0; i < num; i++) {
        if (i == num - 1)
            TWCR = (1 << TWINT) | (1 << TWEN);
        else
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        if (!waitInt()) return 26;
        twst = TWSR & 0xF8;
        if (twst != TW_MR_DATA_ACK && twst != TW_MR_DATA_NACK) return twst;
        data[i] = TWDR;
        //Serial.print(data[i], HEX);
        //Serial.print(" ");
    }
    //Serial.print("\n");
    stop();

    return 0;
}

void Fastwire::reset() {
    TWCR = 0;
}

uint8_t Fastwire::stop() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    if (!waitInt()) return 1;
    return 0;
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") reading ");
        Serial.print(length, DEC);
        Serial.print(" bytes from 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    int8_t count = 0;
    //uint32_t t1 = millis();

    uint8_t status = Fastwire::readBuf(devAddr << 1, regAddr, data, length);
    if (status == 0) {
        count = length; // success
    } else {
        count = -1; // error
    }

    // check for timeout
    //if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif

    return count;
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (-1 indicates failure)
 */
int8_t I2Cdev::readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") reading ");
        Serial.print(length, DEC);
        Serial.print(" words from 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    int8_t count = 0;
    //uint32_t t1 = millis();

    uint16_t intermediate[(uint8_t)length];
    uint8_t status = Fastwire::readBuf(devAddr << 1, regAddr, (uint8_t *)intermediate, (uint8_t)(length * 2));
    if (status == 0) {
        count = length; // success
        for (uint8_t i = 0; i < length; i++) {
            data[i] = (intermediate[2*i] << 8) | intermediate[2*i + 1];
        }
    } else {
        count = -1; // error
    }

    //if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif

    return count;
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") writing ");
        Serial.print(length, DEC);
        Serial.print(" bytes to 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    Fastwire::beginTransmission(devAddr);
    Fastwire::write(regAddr);

    for (uint8_t i = 0; i < length; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            if (i + 1 < length) Serial.print(" ");
        #endif

        Fastwire::write((uint8_t) data[i]);
    }

    Fastwire::stop();
    //status = Fastwire::endTransmission();

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.println(". Done.");
    #endif
    return 0;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") writing ");
        Serial.print(length, DEC);
        Serial.print(" words to 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif
    uint8_t status = 0;

    Fastwire::beginTransmission(devAddr);
    Fastwire::write(regAddr);

    for (uint8_t i = 0; i < length * 2; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            if (i + 1 < length) Serial.print(" ");
        #endif
        Fastwire::write((uint8_t)(data[i] >> 8));       // send MSB
        status = Fastwire::write((uint8_t)data[i++]);   // send LSB
        if (status != 0) break;
    }

    Fastwire::stop();
    //status = Fastwire::endTransmission();

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.println(". Done.");
    #endif
    return status == 0;
}
