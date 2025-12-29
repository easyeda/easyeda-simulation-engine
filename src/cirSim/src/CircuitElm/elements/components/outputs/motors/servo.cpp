/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "servo.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"
#include "e-node.h"

#include "doubleProp.h"

//#define tr(str) simulideTr("Servo",str)

Component* Servo::construct( std::string type, std::string id )
{ return new Servo( type, id ); }

LibraryItem* Servo::libraryItem()
{
    return new LibraryItem(
        ("Servo Motor"),
        "Motors",
        "servo.png",
        "Servo",
        Servo::construct );
}

Servo::Servo( std::string type, std::string id )
     : LogicComponent( type, id )
{
    //m_graphical = true;
    m_width  = 10;
    m_height = 6;
    m_pos = 90;
    m_speed = 0.2;
    m_minPulse = 1000;
    m_maxPulse = 2000;
    
    init({         // Inputs:
            "IL01 V+",
            "IL03 Gnd",
            "IL05 Sig"
        });

    //for( int i=0; i<3;i++ ) m_inpPin[i]->setLabelColor( QColor( 250, 250, 200 ) );

    m_clkPin = m_inpPin[2];         // Input Clock

    //setLabelPos(-16,-40, 0);
    //setShowId( true );
    initialize();
    //m_area = QRect( -40, -40, 96, 80 );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
        new doubleProp<Servo>( "Speed"   , "Speed "     ,"_sec/60º", this, &Servo::speed,    &Servo::setSpeed ),
        new doubleProp<Servo>( "MinPulse", "Min. Pulse Width","_us", this, &Servo::minPulse, &Servo::setMinPulse ),
        new doubleProp<Servo>( "MaxPulse", "Max. Pulse Width","_us", this, &Servo::maxPulse, &Servo::setMaxPulse )
    },0} );
    addPropGroup( { "Electric", IoComponent::inputProps(),0 } );
}
Servo::~Servo(){}

void Servo::stamp()
{
    m_targetPos = 90;
    m_pulseStart = 0;
    m_lastUpdate = Simulator::self()->circTime()/1e6;

    if( m_inpPin[0]->isConnected()
     && m_inpPin[1]->isConnected()
     && m_inpPin[2]->isConnected() )
    {
        m_inpPin[0]->changeCallBack( this, true );
        m_inpPin[1]->changeCallBack( this, true );
    }
    LogicComponent::stamp();
}

void Servo::updateStep()
{
    uint64_t step = Simulator::self()->circTime()/1e6;

    if( m_targetPos != m_pos )
    {
        double updateTime = (step - m_lastUpdate)/1e6;
        double maxMove    = updateTime/m_speed*60; // Maximum to move since last update
        double deltaPos   = m_targetPos - m_pos;
        double absDeltaPos = std::fabs( deltaPos );

        if( absDeltaPos > maxMove ) deltaPos = absDeltaPos/deltaPos*maxMove; // keep sign of deltaPos
        m_pos += deltaPos;
        //Circuit::self()->update();
    }
    m_lastUpdate = step;
    //update();
}

void Servo::voltChanged()
{
    updateClock();

    uint64_t time_us = Simulator::self()->circTime()/1e6;
    
    if(!(m_inpPin[0]->getInpState()-m_inpPin[1]->getInpState()))// not power
    {
        m_targetPos = 90;
        m_pulseStart = 0;
    }
    else if( m_clkState == Clock_Rising )
    {
        m_pulseStart = time_us;
    }
    else if( m_clkState == Clock_Falling )
    {
        if( m_pulseStart == 0 ) return;
        
        double steps = time_us - m_pulseStart;
        m_targetPos = (steps-m_minPulse)*180/(m_maxPulse-m_minPulse); // Map 1mS-2mS to 0-180ª

        if     ( m_targetPos>180 ) m_targetPos = 180;
        else if( m_targetPos<0 )   m_targetPos = 0;
        
        m_pulseStart = 0;
}   }

void Servo::setMinPulse( double w )
{
    if( w >= m_maxPulse ) return;
    m_minPulse = w;
}

void Servo::setMaxPulse( double w )
{
    if( w <= m_minPulse ) return;
    m_maxPulse = w;
}

