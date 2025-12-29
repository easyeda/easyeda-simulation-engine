/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#include "pch.h"
#include "e-resistor.h"

class eNode;

class eCoil : public eResistor
{
    public:
        eCoil( int i, int s, int t, double h, double r, std::string id );
        ~eCoil();

        virtual void initialize() override;
        virtual void stamp() override;
        void stampCoil();

        void addIductor( eCoil* coil, double g ); // An inductor that induces a current in this coil
        void stampCurrent( double deltaCurr );
        void setAdmit( double a ) { m_admit = a; }
        void setBaseInd( double i ) { inductance = i*relation*relation; }

        void setEnode( int n, eNode* e );

        double getVolt();

        int index;
        int sign;
        int size;
        double relation;
        double inductance;

    private:
        double m_currSource;

        eResistor m_resistor0;
        eResistor m_resistor1;

        eNode* m_enode0;
        eNode* m_enode1;
};


