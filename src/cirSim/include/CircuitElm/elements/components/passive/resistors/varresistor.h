/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#ifndef VARRESISTOR_H
#define VARRESISTOR_H

#include "varresbase.h"

class LibraryItem;

class VarResistor : public VarResBase
{
    public:
        VarResistor( std::string type, std::string id );
        ~VarResistor();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void updateStep() override;
};

#endif
