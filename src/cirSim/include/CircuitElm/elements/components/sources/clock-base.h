/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "fixedvolt.h"

//时钟
class ClockBase : public FixedVolt
{
    public:
        ClockBase( std::string type, std::string id );
        ~ClockBase();

        virtual bool setPropStr( std::string prop, std::string val ) override;

        virtual void stamp() override;

        bool alwaysOn() { return m_alwaysOn; }
        void setAlwaysOn( bool on );

        double freq() { return m_freq; }
        virtual void setFreq( double freq );

        bool running() { return m_isRunning; }
        void setRunning( bool running );

        virtual void setOut( bool ) override {;}

        virtual void setLinkedValue( double v, int i=0 ) override;

        //virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

    //public slots:
        virtual void onbuttonclicked() override;

    protected:
        uint64_t getRemainer()
        {
            m_remainder += m_psPerCycleDbl-(double)m_psPerCycleInt;
            uint64_t remainerInt = m_remainder;
            m_remainder -= remainerInt;
            return remainerInt;
        }
        bool m_isRunning;
        bool m_alwaysOn;

        uint64_t m_psPerCycleInt;
        double m_psPerCycleDbl;

        double m_freq;
        double m_remainder;
};

