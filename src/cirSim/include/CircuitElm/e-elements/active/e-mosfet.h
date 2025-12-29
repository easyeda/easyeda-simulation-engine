/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "e-resistor.h"

class eMosfet : public eResistor
{
    public:
        eMosfet( std::string id );
        ~eMosfet();

        virtual void stamp() override;
        virtual void voltChanged() override;

        bool pChannel() { return m_Pchannel; }
        void setPchannel( bool p ) { m_Pchannel = p; m_changed = true; }

        bool depletion() { return m_depletion; }
        void setDepletion( bool d ) { m_depletion = d; m_changed = true; }

        double rdson() { return m_RDSon; }
        void setRDSon( double rdson );

        double threshold() { return m_threshold; }
        void setThreshold( double th );
        
    protected:
        void updateValues();

        double m_accuracy;
        double m_lastCurrent;
        double m_threshold;
        double m_kRDSon;
        double m_RDSon;
        double m_gateV;
        double m_Gth;

        bool m_Pchannel;
        bool m_depletion;
};


