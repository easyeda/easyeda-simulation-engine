/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

class LibraryItem;
class eDiode;
class eResistor;

//双向触发二极管，主要用于触发三端双向可控硅（TRIAC）。
class Diac : public Component, public eElement
{
    public:
        Diac( std::string type, std::string id );
        ~Diac();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        double resOn() { return m_resOn; }
        void setResOn( double r ) { m_resOn = r; m_changed = true; }

        double resOff() { return m_resOff; }
        void setResOff( double r ) { m_resOff = r; m_changed = true; }

        double brkVolt() { return m_brkVolt; }
        void setBrkVolt( double v ) { m_brkVolt = v; m_changed = true; }

        double holdCurr() { return m_holdCurr; }
        void setHoldCurr( double v ) { m_holdCurr = v; m_changed = true; }


    private:

        double m_resOn;
        double m_resOff;
        double m_brkVolt;
        double m_holdCurr;

        bool m_state;

        eNode* m_midEnode;
        eResistor* m_resistor;

        eDiode* m_diode1;
        eDiode* m_diode2;
};
