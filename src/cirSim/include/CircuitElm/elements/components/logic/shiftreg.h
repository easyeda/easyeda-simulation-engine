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

//移位寄存器
class ShiftReg : public LogicComponent
{
    public:
        ShiftReg( std::string type, std::string id );
        ~ShiftReg();

        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override{ IoComponent::runOutputs(); }

        bool resetInv() { return m_resetInv; }
        void setResetInv( bool inv );

    private:
        bool m_resetInv;
};

