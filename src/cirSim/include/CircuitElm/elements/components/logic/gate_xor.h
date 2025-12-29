/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "gate.h"
#include "component.h"

class LibraryItem;

class XorGate : public Gate
{
    public:
        XorGate( std::string type, std::string id );
        ~XorGate();

        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();

    protected:
        virtual bool calcOutput( int inputs ) override;
};

