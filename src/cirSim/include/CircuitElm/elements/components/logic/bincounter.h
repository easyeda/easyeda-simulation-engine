/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "logiccomponent.h"

class LibraryItem;

//二进制计数器
class BinCounter : public LogicComponent
{
    public:
        BinCounter( std::string type, std::string id );
        ~BinCounter();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        bool srInv() { return m_resetInv; }
        void setSrInv( bool inv );

        bool pinSet() { return m_pinSet; }
        void useSetPin( bool set );

        //virtual void initialize() override;
        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        int maxVal() { return m_TopValue; }
        void setMaxVal( int v ) { m_TopValue = v; }

    protected:
        int m_Counter;
        int m_TopValue;

        bool m_resetInv;
        bool m_pinSet;

        IoPin* m_setPin;
        IoPin* m_resetPin;
};

