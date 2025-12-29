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

//RS触发器
class FlipFlopRS : public FlipFlopBase
{
    public:
        FlipFlopRS( std::string type, std::string id );
        ~FlipFlopRS();

        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();

        virtual void voltChanged() override;
};
