/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "e-element.h"

class eResistor : public eElement
{
    public:
        eResistor( std::string id );
        ~eResistor();

        virtual void stamp() override;

        virtual double res() { return 1/m_admit; }
        virtual void setRes( double resist );
        double getRes() { return 1/m_admit; }
        virtual void setResSafe( double resist );
        
        double admit() { return m_admit; }
        void setAdmit( double admit );
        void stampAdmit();

        double current();

    protected:
        virtual void updateVI();

        double m_admit;
        double m_current;
};


