/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "e-reactive.h"
#include "e-pin.h"
#include "e-node.h"
#include "simulator.h"
#include "analogclock.h"

eReactive::eReactive( std::string id )
         : eResistor( id )
{
    m_value    = 0;
    m_InitCurr = 0;
    m_InitVolt = 0;
    AnalogClock::self()->addClkElement( this );
}
eReactive::~eReactive(){
    AnalogClock::self()->remClkElement( this );
}

void eReactive::stamp()
{
    eResistor::stamp();

    if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected())
    {
        m_ePin[0]->createCurrent();
        m_ePin[1]->createCurrent();

        updtReactStep();

        m_volt = m_InitVolt;
        m_curSource = m_InitCurr;
        m_curSource = updtCurr();

        if( m_curSource )
        {
            m_ePin[0]->stampCurrent( m_curSource );
            m_ePin[1]->stampCurrent(-m_curSource );
        }
        //m_ePin[0]->changeCallBack( this );
        //m_ePin[1]->changeCallBack( this );
    }
    //m_voltChanged = false;

    updtReactStep();
}

// void eReactive::voltChanged()
// {
//     uint64_t simTime = Simulator::self()->circTime();  //电路仿真时间
//     m_deltaTime = simTime - m_lastTime;                //上次更新反应器电压时间
//     m_lastTime = simTime;                              //更新上次反应器时间

//     //检查时间增量是否小于反应器件时间步长增量，以及步长是否错误
//     if( m_deltaTime < m_reacStep && !m_stepError )
//     {
//         if( m_deltaTime > 10 )
//         {
//             if( m_autoStep )  // Auto-resize step
//             {
//                 m_reacStep = m_deltaTime;
//                 updtReactStep();
//             }
//             else m_stepError = true;
//         }
//     }
//     else if( m_stepError ) m_stepError = false;

//     if( !m_running )
//     {
//         m_running = true;
//         Simulator::self()->addEvent( m_reacStep, this );
//     }
// }

void eReactive::runEvent()
{
    double volt = m_ePin[0]->getVoltage() - m_ePin[1]->getVoltage();

    if( m_volt != volt )
    {
        /*if( m_voltChanged )
        {
            updtReactStep();
            m_voltChanged = false;
            m_lastTime = 0;
        }*/
        m_volt = volt;
        m_curSource = updtCurr();

        m_ePin[0]->stampCurrent( m_curSource );
        m_ePin[1]->stampCurrent(-m_curSource );
    }
}

void eReactive::updtReactStep()
{
    m_timeStep = AnalogClock::self()->getStep(); // Time in ps
    m_tStep = (double)m_timeStep/1e12;         // Time in seconds
    eResistor::setRes( updtRes() );
}
