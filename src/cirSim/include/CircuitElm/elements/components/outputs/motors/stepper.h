/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "linkedComponent.h"
#include "e-element.h"
#include "e-resistor.h"
#include "pin.h"

class LibraryItem;

//步进电机
class Stepper : public LinkedComponent, public eElement
{
    public:
        Stepper( std::string type, std::string id );
        ~Stepper();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void updateStep() override;

        int steps() { return m_steps; }
        void setSteps( int steps ); //" 4, 8,16,32"
        
        double res()  { return m_res; }
        void setRes( double rows );

        bool bipolar() { return m_bipolar; }
        void setBipolar( bool bi );

        //virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;
    protected:
        int m_steps;
        double m_res;
        int m_ang;
        int m_stpang;
        int m_Ppos;

        bool m_bipolar;
        
        eResistor m_resA1;
        eResistor m_resA2;
        eResistor m_resB1;
        eResistor m_resB2;
        
        Pin m_pinA1;
        Pin m_pinA2;
        Pin m_pinCo;
        Pin m_pinB1;
        Pin m_pinB2;
        
        ePin m_ePinA1Co;
        ePin m_ePinA2Co;
        ePin m_ePinB1Co;
        ePin m_ePinB2Co;
};



