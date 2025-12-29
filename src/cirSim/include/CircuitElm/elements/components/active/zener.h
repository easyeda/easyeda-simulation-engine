/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "diode.h"

class LibraryItem;
//稳压管
class Zener : public Diode
{
    public:
        Zener( std::string type, std::string id );
        ~Zener();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();
 // 获取所有键的函数
 static std::vector<std::string> getKeys();
};

