/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"

#include "e-resistor.h"

struct diodeData_t
{
    double satCur;
    double emCoef;
    double brkDow;
    double resist;
};

class eNode;

class eDiode : public eResistor
{
    public:
        eDiode( std::string id );
        ~eDiode();

        virtual void stamp() override;
        virtual void voltChanged() override;

        double threshold() { return m_vCriti; }
        void   setThreshold( double vCrit );

        double brkDownV(){ return m_bkDown; }
        void   setBrkDownV( double bkDown );

        double satCur() { return m_satCur; }
        void   setSatCur( double satCur );

        double emCoef() { return m_emCoef; }
        void   setEmCoef( double emCoef );

        double maxCurrent() { return m_maxCur; }
        void   setMaxCurrent( double cur ) { m_maxCur = cur; }

        std::string model() { return m_model; }
        void setModel( std::string model );
        void setModelData( diodeData_t data );

 static void getModels();

    protected:
        double limitStep( double vnew, double vold, double scale, double vc );
        void SetParameters( double sc, double ec, double bv, double sr );
        void updateValues();

        bool m_converged;

        double m_vt;
        double m_satCur;
        double m_emCoef;
        double m_vScale;
        double m_vdCoef;
        double m_vzCoef;
        double m_zOfset;
        double m_vCriti;
        double m_vzCrit;

        double m_bkDown;
        double m_maxCur;

        double m_voltPN;
        double m_bAdmit;

        std::string m_diodeType;
        std::string m_model;

        static std::unordered_map<std::string, diodeData_t> m_diodes;
        static std::unordered_map<std::string, diodeData_t> m_zeners;
        static std::unordered_map<std::string, diodeData_t> m_leds;
};


