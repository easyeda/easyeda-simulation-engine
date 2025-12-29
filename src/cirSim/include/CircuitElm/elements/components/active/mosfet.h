/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-mosfet.h"
#include "component.h"

class LibraryItem;
//场效应管
class Mosfet : public Component, public eMosfet
{
    public:
        Mosfet( std::string type, std::string id );
        ~Mosfet();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void updateStep() override;
};

