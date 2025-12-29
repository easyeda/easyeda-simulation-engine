/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "reactive.h"

class CapacitorBase : public Reactive
{
    public:
        CapacitorBase( std::string type, std::string id );
        ~CapacitorBase();

        virtual void setCurrentValue( double c ) override;

    protected:
        virtual double updtRes()  override { return m_tStep/m_capacitance; }
        virtual double updtCurr() override { 
            double current = m_volt*m_admit; 
            return current;
        }
        double m_capacitance;
};


