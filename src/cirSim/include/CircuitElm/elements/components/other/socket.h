/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "connbase.h"
class LibraryItem;

class Socket : public ConnBase
{
    public:
        Socket( std::string type, std::string id );
        ~Socket();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        void updateStep() override;

        void updatePins() override { connectPins( false ); }
        void connectPins( bool connect );

};


