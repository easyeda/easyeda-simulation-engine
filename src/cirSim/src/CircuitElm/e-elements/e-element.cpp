/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "e-element.h"
#include "simulator.h"
#include "e-pin.h"
eElement::eElement(const std::string& id)
{
    m_elmId = id;
    nextChanged = NULL;
    nextEvent  = NULL;
    eventTime = 0;
    m_pendingTime = 0;
    added = false;
    m_step = 0;


    if( Simulator::self() ) Simulator::self()->addToElementList( this );
    //添加状态器件
     if( Simulator::self() &&(
        m_elmId.find("Led") != std::string::npos || 
        (m_elmId.find("Seven Segment") != std::string::npos && m_elmId.find("led") == std::string::npos)||
        (m_elmId.find("7-Seg BCD") != std::string::npos && m_elmId.find("in") == std::string::npos) ||
        (m_elmId.find("Lamp") != std::string::npos)
     )) Simulator::self()->addToStatusElementList( this );
}
eElement::~eElement()
{
    //// 首先释放所有动态分配的 ePin 对象
    //for (ePin* pin : m_ePin) 
    //{
    //    delete pin;
    //}

    // 然后清空 vector，移除所有指针
    m_ePin.clear();
    if( !Simulator::self() ) return;
    Simulator::self()->remFromElementList( this );
    Simulator::self()->cancelEvents( this );
}

void eElement::setNumEpins( int n )
{
    m_ePin.resize(n);
    for( int i=0; i<n; ++i )
    {
        if( m_ePin[i] == NULL )
            m_ePin[i] = new ePin(m_elmId + "-ePin" + std::to_string(i), i);
     }   
}

ePin* eElement::getEpin( int num )
{ return m_ePin[num]; }

void eElement::setEpin( int num, ePin* pin )
{ m_ePin[num] = pin; }

void eElement::pauseEvents()
{
    if( eventTime )
    {
        m_pendingTime = eventTime - Simulator::self()->circTime();
        Simulator::self()->cancelEvents( this );
    }
    else m_pendingTime = 0;
}

void eElement::resumeEvents()
{
    if( !m_pendingTime ) return;
    Simulator::self()->addEvent( m_pendingTime, this );
    m_pendingTime = 0;
}
