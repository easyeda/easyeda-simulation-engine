/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-resistor.h"
#include "component.h"

class LibraryItem;

//音频输出
class AudioOut : public Component, public eResistor
{
    public:
        AudioOut( std::string type, std::string id );
        ~AudioOut();
    
 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void runEvent() override;

        bool buzzer() { return m_buzzer; }
        void setBuzzer( bool b ) { m_buzzer = b; }

    private:
        int m_dataSize;
        int m_dataCount;

        bool m_started;
        bool m_buzzer;
};



