/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "linkable.h"

class LinkedComponent : public Component, public Linkable
{
    public:
        LinkedComponent( std::string type, std::string id );
        ~LinkedComponent();


        void slotLinkComp() { Linkable::startLinking(); }
};
