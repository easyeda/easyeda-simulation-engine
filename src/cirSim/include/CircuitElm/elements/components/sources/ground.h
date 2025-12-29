/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"

class LibraryItem;

//地
class Ground : public Component
{
    public:
        Ground( std::string type, std::string id );
        ~Ground();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

};

