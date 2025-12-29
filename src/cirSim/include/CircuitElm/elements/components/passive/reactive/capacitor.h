/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "capacitorbase.h"

class LibraryItem;

class Capacitor : public CapacitorBase
{
    public:
        Capacitor( std::string type, std::string id );
        ~Capacitor();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();
};


