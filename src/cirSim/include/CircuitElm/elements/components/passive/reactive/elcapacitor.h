/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#ifndef ELCAPACITOR_H
#define ELCAPACITOR_H

#include "capacitorbase.h"

class LibraryItem;

class elCapacitor : public CapacitorBase
{
public:
    elCapacitor(std::string type, std::string id);
    ~elCapacitor();

    static Component* construct(std::string type, std::string id);
    static LibraryItem* libraryItem();

    virtual void updateStep() override;
};

#endif
