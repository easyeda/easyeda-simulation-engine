/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "ledbase.h"
#include "linkable.h"

class LibraryItem;

class Led : public LedBase, public Linkable
{
    public:
        Led( std::string type, std::string id );
        ~Led();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void voltChanged() override;
        void slotLinkComp() { Linkable::startLinking(); }
};

