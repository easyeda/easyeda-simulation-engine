/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-diode.h"
#include "component.h"
#include "linkedComponent.h"

class LibraryItem;

//二极管
class Diode :  public eDiode,public LinkedComponent
{
    public:
        Diode( std::string type, std::string id, bool zener=false );
        ~Diode();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void voltChanged() override;

        virtual bool setPropStr( std::string prop, std::string val ) override;

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;

        virtual double res() override{ return m_resistor->res(); }
        virtual void setResSafe( double resist ) override { m_resistor->setResSafe(resist);}

    private:
        bool m_isZener;

        eNode* m_midEnode;
        eResistor* m_resistor;
};
