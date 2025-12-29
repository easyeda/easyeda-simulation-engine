/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */
#pragma once

#include "component.h"
#include "e-bjt.h"



class LibraryItem;
class BJT :
    public Component, public eBJT
{
public:
    BJT(std::string type, std::string id);
    ~BJT();

    static Component* construct(std::string type, std::string id);
    static LibraryItem* libraryItem();

    virtual void updateStep() override;

};

