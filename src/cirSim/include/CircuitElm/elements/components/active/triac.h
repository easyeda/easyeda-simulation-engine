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

//三端双向可控硅，用于交流电路中的开关和控制。
class Triac : public Component, public eElement
{
    public:
        Triac( std::string type, std::string id );
        ~Triac();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        double gateRes() { return m_gateRes; }
        void setGateRes( double r ) { m_gateRes = r; m_changed = true; }

        double trigCurr() { return m_trigCurr; }
        void setTrigCurr( double c ) { m_trigCurr = c; m_changed = true; }

        double holdCurr() { return m_holdCurr; }
        void setHoldCurr( double v ) { m_holdCurr = v; m_changed = true; }


    private:
        double m_gateRes;
        double m_trigCurr;
        double m_holdCurr;

        bool m_state;

        eNode*     m_midEnode;
        eResistor* m_resistor;
        eResistor* m_resistGa;

        eDiode* m_diode1;
        eDiode* m_diode2;
};

