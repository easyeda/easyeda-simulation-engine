/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#include "switch_base.h"

class LibraryItem;

//普通开关
class Switch : public SwitchBase
{
    public:
        Switch( std::string type, std::string id );
        ~Switch();

    static Component* construct( std::string type, std::string id );
    static LibraryItem* libraryItem();

    virtual void stamp() override;
    virtual void updateState() override;
    virtual void setChecked(bool status) override;
};

