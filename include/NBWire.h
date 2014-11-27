/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Gene Knight <Gene@Telobot.com>

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

#define NBWIRE_BUFFER_LENGTH 32

class TwoWire {
    private:
        static uint8_t rxBuffer[];
        static uint8_t rxBufferIndex;
        static uint8_t rxBufferLength;

        static uint8_t txAddress;
        static uint8_t txBuffer[];
        static uint8_t txBufferIndex;
        static uint8_t txBufferLength;

        // static uint8_t transmitting;
        static void (*user_onRequest)(void);
        static void (*user_onReceive)(int);
        static void onRequestService(void);
        static void onReceiveService(uint8_t*, int);

    public:
        TwoWire();
        void begin();
        void begin(uint8_t);
        void begin(int);
        void beginTransmission(uint8_t);
        //void beginTransmission(int);
        uint8_t endTransmission(uint16_t timeout=0);
        void nbendTransmission(void (*function)(int)) ;
        uint8_t requestFrom(uint8_t, int, uint16_t timeout=0);
        //uint8_t requestFrom(int, int);
        void nbrequestFrom(uint8_t, int, void (*function)(int));
        void send(uint8_t);
        void send(uint8_t*, uint8_t);
        //void send(int);
        void send(char*);
        uint8_t available(void);
        uint8_t receive(void);
        void onReceive(void (*)(int));
        void onRequest(void (*)(void));
};

#define TWI_READY   0
#define TWI_MRX     1
#define TWI_MTX     2
#define TWI_SRX     3
#define TWI_STX     4

#define TW_WRITE    0
#define TW_READ     1

#define TW_MT_SLA_NACK      0x20
#define TW_MT_DATA_NACK     0x30

#define CPU_FREQ            16000000L
#define TWI_FREQ            100000L
#define TWI_BUFFER_LENGTH   32

/* TWI Status is in TWSR, in the top 5 bits: TWS7 - TWS3 */

#define TW_STATUS_MASK              (_BV(TWS7)|_BV(TWS6)|_BV(TWS5)|_BV(TWS4)|_BV(TWS3))
#define TW_STATUS                   (TWSR & TW_STATUS_MASK)
#define TW_START                    0x08
#define TW_REP_START                0x10
#define TW_MT_SLA_ACK               0x18
#define TW_MT_SLA_NACK              0x20
#define TW_MT_DATA_ACK              0x28
#define TW_MT_DATA_NACK             0x30
#define TW_MT_ARB_LOST              0x38
#define TW_MR_ARB_LOST              0x38
#define TW_MR_SLA_ACK               0x40
#define TW_MR_SLA_NACK              0x48
#define TW_MR_DATA_ACK              0x50
#define TW_MR_DATA_NACK             0x58
#define TW_ST_SLA_ACK               0xA8
#define TW_ST_ARB_LOST_SLA_ACK      0xB0
#define TW_ST_DATA_ACK              0xB8
#define TW_ST_DATA_NACK             0xC0
#define TW_ST_LAST_DATA             0xC8
#define TW_SR_SLA_ACK               0x60
#define TW_SR_ARB_LOST_SLA_ACK      0x68
#define TW_SR_GCALL_ACK             0x70
#define TW_SR_ARB_LOST_GCALL_ACK    0x78
#define TW_SR_DATA_ACK              0x80
#define TW_SR_DATA_NACK             0x88
#define TW_SR_GCALL_DATA_ACK        0x90
#define TW_SR_GCALL_DATA_NACK       0x98
#define TW_SR_STOP                  0xA0
#define TW_NO_INFO                  0xF8
#define TW_BUS_ERROR                0x00

//#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))
//#define _SFR_BYTE(sfr) _MMIO_BYTE(_SFR_ADDR(sfr))

#ifndef sbi // set bit
    #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif // sbi

#ifndef cbi // clear bit
    #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif // cbi

extern TwoWire Wire;

