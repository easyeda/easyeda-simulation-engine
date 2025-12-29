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
class Pin;

//电流源
class CurrSource : public VarSource
{
    public:
        CurrSource( std::string type, std::string id );
        ~CurrSource();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;

    private:
        Pin* m_outPin;
};

