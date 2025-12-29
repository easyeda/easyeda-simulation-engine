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

class AndGate : public Gate
{
    public:
        AndGate( std::string type, std::string id );
        ~AndGate();
        
        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();
};

