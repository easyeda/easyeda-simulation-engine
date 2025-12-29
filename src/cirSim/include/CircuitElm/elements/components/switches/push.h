/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "push_base.h"

class LibraryItem;

//按钮开关
class Push : public PushBase
{
    public:
        Push( std::string type, std::string id );
        ~Push();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();
};

