/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "compbase.h"

class Pin;

class Connector : public CompBase
{
    public:
        Connector( std::string type, std::string id, Pin* startpin, Pin* endpin = NULL );
        ~Connector();

        std::string itemID() { return m_id; }

        void dummySetter( std::string ) {;}

        std::string startPinId();
        std::string endPinId();
        Pin* startPin() { return m_startPin;}
        void setStartPin( Pin* pin ) { m_startPin = pin; }
        Pin* endPin() { return m_endPin; }
        void setEndPin( Pin* pin) { m_endPin = pin; }

        double getVoltage();

        void closeCon(Pin* endpin);

        
        void setIsBus( bool bus );
        bool isBus() { return m_isBus; }
        
        //void move( QPointF delta );

        void remove();
        //void remLines();

        bool m_freeLine;

    private:


        int m_actLine;
        int m_lastindex;
        
        bool m_isBus;

        Pin*    m_startPin;
        Pin*    m_endPin;
};


