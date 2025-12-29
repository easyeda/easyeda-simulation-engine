/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "gate.h"

class LibraryItem;

//缓冲器（Buffer）
class Buffer : public Gate
{
    public:
        Buffer( std::string type, std::string id );
        ~Buffer();

        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();

        virtual void setTristate( bool t ) override;
};

