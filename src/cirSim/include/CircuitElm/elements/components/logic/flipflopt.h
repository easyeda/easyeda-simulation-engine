/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "flipflopbase.h"

class LibraryItem;

//T触发器
class FlipFlopT : public FlipFlopBase
{
    public:
        FlipFlopT( std::string type, std::string id );
        ~FlipFlopT();

        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();

    protected:
        void calcOutput();
};

