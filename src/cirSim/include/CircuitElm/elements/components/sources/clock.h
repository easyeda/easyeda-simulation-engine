/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#include "clock-base.h"

class LibraryItem;

//时钟源
class Clock : public ClockBase
{
    public:
        Clock( std::string type, std::string id );
        ~Clock();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        void updateStep() override;
        void runEvent() override;

        bool m_state;
};

