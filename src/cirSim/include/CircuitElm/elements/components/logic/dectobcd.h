/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "logiccomponent.h"

class LibraryItem;

//十进制到 BCD 的转换
class DecToBcd : public LogicComponent
{
    public:
        DecToBcd( std::string type, std::string id );
        ~DecToBcd();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        bool is16Bits() { return m_16Bits; }
        void set16bits( bool set );

    private:
        bool m_16Bits;
        int  m_bits;
};


