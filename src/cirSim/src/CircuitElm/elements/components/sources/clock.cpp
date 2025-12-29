/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */



#include "clock.h"
#include "iopin.h"
#include "simulator.h"
#include "itemlibrary.h"

#include "doubleProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("Clock",str)

Component* Clock::construct( std::string type, std::string id )
{ return new Clock( type, id ); }

LibraryItem* Clock::libraryItem()
{
    return new LibraryItem(
        "Clock",
        "Sources",
        "clock.png",
        "Clock",
        Clock::construct );
}

Clock::Clock( std::string type, std::string id )
     : ClockBase( type, id )
{
    remPropGroup( "Main" );
    addPropGroup( { "Main", {
new doubleProp<Clock>( "Voltage"  , "Voltage"  ,"V" , this, &Clock::volt,     &Clock::setVolt ),
new doubleProp<Clock>( "Freq"     , "Frequency","Hz", this, &Clock::freq,     &Clock::setFreq ),
new boolProp<Clock>( "Always_On", "Always On",""  , this, &Clock::alwaysOn, &Clock::setAlwaysOn ),
    }, 0} );
}
Clock::~Clock(){}

void Clock::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    Simulator::self()->cancelEvents( this );
    m_outPin->setOutState( false );
    m_state = false;

    if( m_isRunning ) Simulator::self()->addEvent( m_psPerCycleInt/2, this );
}


void Clock::runEvent()
{
    m_state = !m_state;
    m_outPin->setOutState( m_state );

    uint64_t remainerInt = getRemainer();

    if( m_isRunning ) Simulator::self()->addEvent( m_psPerCycleInt/2+remainerInt, this );
}
