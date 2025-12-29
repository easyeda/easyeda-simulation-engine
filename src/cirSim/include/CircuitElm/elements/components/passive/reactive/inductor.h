/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "reactive.h"

class LibraryItem;

class Inductor : public Reactive
{
    public:
        Inductor( std::string type, std::string id );
        ~Inductor();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        double indCurrent() { return m_curSource; }

        virtual void setCurrentValue( double c ) override;

        Pin* getPin( int n ) { return m_pin[n]; }
        
    protected:
        virtual double updtRes()  override { return m_inductance/m_tStep; }
        virtual double updtCurr() override { return m_curSource - m_volt*m_admit; }

        double m_inductance;
};
