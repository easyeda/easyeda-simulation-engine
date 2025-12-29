/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "varsource.h"

class LibraryItem;
class IoPin;

//电压源
class VoltSource : public VarSource
{
    public:
        VoltSource( std::string type, std::string id );
        ~VoltSource();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        void updateStep();    // virtual   override        不确定是否需要这个

    private:
        IoPin* m_outPin;

};

