/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "clock-base.h"
#include "iopin.h"
#include "simulator.h"
//#include "custombutton.h"

#include "boolProp.h"

ClockBase::ClockBase( std::string type, std::string id )
         : FixedVolt( type, id )
{
    //m_area = QRect(-14,-8, 22, 16 );

    // m_graphical = true;
    m_isRunning = false;
    m_alwaysOn  = false;

    m_psPerCycleInt = 0;
    ClockBase::setFreq( 1000 );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Hidden1", {
new boolProp<ClockBase>( "Running", "","", this, &ClockBase::running, &ClockBase::setRunning ),
    }, groupHidden} );
}
ClockBase::~ClockBase(){}

bool ClockBase::setPropStr( std::string prop, std::string val )
{
    if( prop =="Out" ) setRunning( val == "true" );   // Old: TODELETE
    else return Component::setPropStr( prop, val );
    return true;
}

void ClockBase::stamp()
{
    setFreq( m_freq );
    if( !Simulator::self()->isPaused() ) m_changed = true;
}


void ClockBase::setAlwaysOn( bool on )
{
    m_alwaysOn = on;
    if( on ) setRunning( on );
    //m_button->setVisible( !on );
}

void ClockBase::setFreq( double freq )
{
    m_psPerCycleDbl = 1e6*1e6/freq;
    m_psPerCycleInt = m_psPerCycleDbl;
    
    m_freq = freq;
    m_remainder = 0;

    setRunning( m_isRunning || m_alwaysOn );
}

void ClockBase::setRunning( bool running )
{
    running = running && (m_freq>0);
    //m_button->setChecked( running );
    m_isRunning = running;
    m_changed = true;
    //update();
}

void ClockBase::setLinkedValue( double v, int )
{
    setFreq( v );
}


void ClockBase::onbuttonclicked() { setRunning( !m_isRunning ); }
