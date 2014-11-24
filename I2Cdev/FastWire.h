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

#include <stdint.h>

/* Master */
#define TW_START                0x08
#define TW_REP_START            0x10

/* Master Transmitter */
#define TW_MT_SLA_ACK           0x18
#define TW_MT_SLA_NACK          0x20
#define TW_MT_DATA_ACK          0x28
#define TW_MT_DATA_NACK         0x30
#define TW_MT_ARB_LOST          0x38

/* Master Receiver */
#define TW_MR_ARB_LOST          0x38
#define TW_MR_SLA_ACK           0x40
#define TW_MR_SLA_NACK          0x48
#define TW_MR_DATA_ACK          0x50
#define TW_MR_DATA_NACK         0x58

#define TW_OK                   0
#define TW_ERROR                1

class Fastwire {
    private:
        static bool waitInt();

    public:
        static void setup(int khz, bool pullup);
        static uint8_t beginTransmission(uint8_t device);
        static uint8_t write(uint8_t value);
        static uint8_t writeBuf(uint8_t device, uint8_t address, uint8_t *data, uint8_t num);
        static uint8_t readBuf(uint8_t device, uint8_t address, uint8_t *data, uint8_t num);
        static void reset();
        static uint8_t stop();
};
