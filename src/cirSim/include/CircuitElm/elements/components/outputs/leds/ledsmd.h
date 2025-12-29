/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "ledbase.h"

//表面贴装LED（SMD LED）
class LedSmd : public LedBase
{
    public:
        LedSmd( std::string type, std::string id,ePin* pin0=NULL, ePin* pin1=NULL );    // QRectF area, 
        ~LedSmd();
};

