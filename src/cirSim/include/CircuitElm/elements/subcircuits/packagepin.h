/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pin.h"

class SubPackage;
class PackagePin : public Pin
{
    public:
        PackagePin( std::string id, int index, SubPackage* parent=0 );
        ~PackagePin();

    private:
        SubPackage* m_package;
};
