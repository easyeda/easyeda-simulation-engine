/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "switch_base.h"
#include "customButton.h"

class PushBase : public SwitchBase
{
    public:
    PushBase( std::string type, std::string id );
    ~PushBase();

    void onButtonPressed();
    void onButtonReleased();
    virtual void updateState(std::string mes) override;
};

