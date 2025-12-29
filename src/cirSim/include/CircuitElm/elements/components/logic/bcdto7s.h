/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "bcdbase.h"
class LibraryItem;

//BCD 到 7 段显示的转换
class BcdTo7S : public BcdBase
{
    public:
        BcdTo7S( std::string type, std::string id );
        ~BcdTo7S();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        virtual void stamp() override;
        void updateStep() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        bool pinReset() { return m_useReset; }
        void setPinReset( bool r );

        bool resetInv() { return m_resetInv; }
        void setResetInv( bool inv );

    private:

        bool m_useReset;
        bool m_resetInv;

        IoPin* m_resetPin;
};
