/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-resistor.h"

class LibraryItem;


// 电压调节器，用于稳定输出电压的电子元件。
class VoltReg : public Component, public eResistor
{
    public:
        VoltReg( std::string type, std::string id );
        ~VoltReg();
        
        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        double outVolt() { return m_vRef; }
        void setOutVolt( double v ) { m_vRef = v; m_changed = true; }


    protected:
        double m_vRef;
        double m_lastCurrent;

        bool m_connected;
};
