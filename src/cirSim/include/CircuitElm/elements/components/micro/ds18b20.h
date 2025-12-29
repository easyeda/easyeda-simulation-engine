/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-element.h"
#include "component.h"

class LibraryItem;
class IoPin;

class Ds18b20 : public Component , public eElement
{
    public:
        Ds18b20( std::string type, std::string id );
        ~Ds18b20();

        enum w1State_t{
            W1_IDLE=0,
            W1_ROM_CMD,
            W1_FUN_CMD,
            W1_SEARCH,
            W1_MATCH,
            W1_DATA,
            W1_BUSY
        };
        enum w1Command_t {
            W1_SEARCH_ROM = 0xF0,
            W1_SEARCH_ALM = 0xEC,
            W1_READ_ROM   = 0x33,
            W1_MATCH_ROM  = 0x55,
            W1_SKIP_ROM   = 0xCC,
        };
        enum dsCommand_t {
            DS_CONVERT = 0x44,
            DS_WR_SCRP = 0x4E,
            DS_RD_SCRP = 0xBE,
            DS_CP_SCRP = 0x48,
            DS_REC_E2  = 0xB8,
            DS_RD_POW  = 0xB4
        };

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        std::string getROM() { return arrayToHex( m_ROM, 8 ); }
        void setROM( std::string ROMstr );

        double tempInc() { return m_tempInc; }
        void setTempInc( double inc ) { m_tempInc = inc; }

        double temp() { return m_temp; }
        void setTemp( double t );

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

    //public slots:
    //    void upbuttonclicked();
    //    void downbuttonclicked();

    private:
 static std::string arrayToHex( uint8_t* data, uint32_t len );

        void dataReceived();
        void dataSent();
        void sendData( uint8_t data, int size=8);
        void writeBit();
        void readBit( uint8_t bit );
        void pulse( uint64_t time, uint64_t witdth ); // Time in us
        void romCommand( uint8_t cmd );
        void funCommand( uint8_t cmd );

        // ROM commands
        void searchROM();       // F0h
        void skipROM();         // CCh
        void readROM();         // 33h
        void matchROM();        // 55h
        void alarmSearch();     // ECh

        // Function commands
        void convertTemp();     // 44h
        void writeScratchpad(); // 4Eh
        void readScratchpad();  // BEh
        void copyScratchpad();  // 48h
        void recallE2();        // B8h
        void readPowerSupply(); // B4h

        void sendSearchBit();
        bool bitROM( uint32_t bitIndex );
        void generateROM( uint8_t familyCode );
        uint8_t crc8( uint8_t* addr, uint8_t len );

        w1State_t m_state;
        uint8_t   m_lastCommand;

        bool m_lastIn;
        bool m_write;
        bool m_pullDown;
        bool m_bitROM;
        bool m_parPower;
        bool m_alarm;

        uint64_t m_lastTime;
        uint64_t m_busyTime;
        uint8_t  m_rxReg;
        uint8_t  m_txReg;
        uint8_t  m_lastBit;
        uint8_t  m_bitSearch;
        uint64_t m_bitIndex;

        int m_pulse;
        int m_byte;

        double m_temp;
        double m_tempInc;

        //QFont m_font;
        IoPin* m_inpin;

        int m_resolution; // 9-12 bits
        uint8_t m_scratchpad[9];
        uint8_t m_ROM[8];

        int8_t  m_TH;  // TH register, alarm trigger register, can be stored in internal EEPROM
        int8_t  m_TL;  // TL register, alarm trigger register, can be stored in internal EEPROM
        uint8_t m_CFG; // Config register, can be stored in internal EEPROM

        std::vector<uint8_t> m_txBuff;
};

