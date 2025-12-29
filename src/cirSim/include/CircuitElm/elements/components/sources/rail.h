/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

class LibraryItem;
class IoPin;

//稳压源
class Rail : public Component, public eElement
{
    public:
        Rail( std::string type, std::string id );
        ~Rail();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        double volt() { return m_volt; }
        void setVolt( double v );

        virtual void stamp() override;

    private:
        double m_volt;

        IoPin* m_out;
};

