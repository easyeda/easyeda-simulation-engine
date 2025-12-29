/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-reactive.h"
#include "comp2pin.h"

class Reactive : public Comp2Pin, public eReactive
{
    public:
        Reactive( std::string type, std::string id );
        ~Reactive();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;

        double value() { return m_value; }
        void setValue( double c );

        double resist() { return m_resistor->res(); }
        void setResist( double resist );

        // int autoStep() { return m_autoStep; }
        // void setAutoStep( int a );

        virtual void setCurrentValue( double c ){;}

        virtual void setLinkedValue( double v, int i=0 ) override; 

    private:
        eNode* m_midEnode;
        eResistor* m_resistor;
};
