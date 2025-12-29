/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-resistor.h"
#include "comp2pin.h"

class LibraryItem;

class Resistor : public Comp2Pin, public eResistor
{
    public:
        Resistor( std::string type, std::string id );
        ~Resistor();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

};


