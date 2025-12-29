/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-resistor.h"
#include "comp2pin.h"

class LibraryItem;

//灯泡组件，根据电流变化控制其亮度
class Lamp : public Comp2Pin, public eResistor
{
    public:
        Lamp( std::string type, std::string id );
        ~Lamp();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        double maxCurrent()             { return m_maxCurrent; }
        void  setMaxCurrent( double c ) { m_maxCurrent = c; }
        virtual std::string getElementStatusData() override;

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

    private:
        virtual void updateVI() override;
        void updateBright();
        double overCurrent() { return m_avgCurrent/m_maxCurrent; }

        uint64_t m_prevStep;
        uint32_t m_intensity;

        double m_maxCurrent;
        double m_avgCurrent;
        double m_totalCurrent;
        double m_lastCurrent;
        double m_lastPeriod;
        double m_brightness;
};


