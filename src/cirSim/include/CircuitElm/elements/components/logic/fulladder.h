/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once 

#include "logiccomponent.h"
#include "e-element.h"


class LibraryItem;

//全加器
class FullAdder : public LogicComponent
{
    public:
        FullAdder( std::string type, std::string id );
        ~FullAdder();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        void stamp() override;
        void voltChanged() override;
        void runEvent() override { IoComponent::runOutputs(); }

        int bits() { return m_bits; }
        void setBits( int b );

        Pin* getPin( std::string pinName ) override;

    private:

        int m_bits;

        IoPin* m_ciPin;
        IoPin* m_coPin;
};
