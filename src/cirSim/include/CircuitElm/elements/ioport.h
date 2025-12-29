/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "iopin.h"
#include "iopin.h"
#include "e-element.h"

class IoPin;
class Component;
class asIScriptEngine;

class IoPort :public eElement
{
        friend class McuCreator;

    public:
        IoPort( std::string name );
        ~IoPort();

        void reset();
        virtual void runEvent() override;

        void scheduleState( uint32_t val, uint64_t time );
        void setOutState( uint32_t val );
        void setOutStatFast( uint32_t val );
        uint32_t getInpState();

        void setDirection( uint32_t val );
        void setPinMode( pinMode_t mode );
        void setPinMode( uint32_t mode ) { setPinMode( (pinMode_t) mode ); }

        void changeCallBack( eElement* el, bool ch );

        IoPin* getPinN( uint8_t i );
        IoPin* getPin( std::string pinName );

        std::string name() { return m_name; }

        // ----------------
        struct outState_t{
            uint64_t time;
            uint32_t     state;
        };
        void trigger();
        void addOutState( uint64_t t, uint32_t s ) { m_outVector.emplace_back( outState_t{t, s} ); }
        void setOutVector( std::vector<outState_t> v ) { m_outVector = v; }

 //static void registerScript( asIScriptEngine* engine );------------预测运行MCU

    protected:
        void createPins( Component* comp, std::string pins, uint32_t pinMask );
        virtual IoPin* createPin( int i, std::string id , Component* comp );

        inline void nextStep();

        std::string m_name;
        std::string m_shortName;

        uint32_t m_pinState;
        uint32_t m_nextState;
        uint32_t m_pinDirection;

        pinMode_t m_pinMode;

        uint8_t m_numPins;
        std::vector<IoPin*> m_pins;

        uint32_t m_index;
        std::vector<outState_t> m_outVector;
};
