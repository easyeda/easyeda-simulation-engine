/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "linkedComponent.h"
#include "e-resistor.h"


class LibraryItem;

//直流电机
class DcMotor : public LinkedComponent, public eResistor
{
    public:
        DcMotor( std::string type, std::string id );
        ~DcMotor();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void updateStep() override;

        int rpm() { return m_rpm; }
        void setRpm( int rpm );

        double volt() { return m_voltNom; }
        void setVolt( double v ) { m_voltNom = v; }
        
    protected:
        void updatePos();

        int m_rpm;

        double m_LastVolt;
        double m_voltNom;
        double m_ang;
        double m_motStPs;
        double m_speed;
        double m_delta;

        uint64_t m_lastTime;
        uint64_t m_updtTime;
};

