/***************************************************************************
 *   Copyright (C) 2019 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "sr04.h"
#include "iopin.h"
#include "itemlibrary.h"
#include "simulator.h"

Component* SR04::construct( std::string type, std::string id )
{ return new SR04( type, id ); }

LibraryItem* SR04::libraryItem()
{
    return new LibraryItem(
        "HC-SR04",
        "Sensors",
        "sr04_ico.png",
        "SR04",
        SR04::construct);
}

SR04::SR04( std::string type, std::string id )
    : Component( type, id )
    , eElement( id )
{
    //m_graphical = true;

    //m_area = QRect(-10*8,-4*8, 21*8, 9*8 );
    //setBackground("sr04.png");
    //setLabelPos(-16,-48, 0);

    m_pin.resize(5);

    m_inpin = new Pin(id+"-inpin", 0, this );
    //m_inpin->setLabelText( " In v=m" );
    m_pin[0] = m_inpin;

    Pin* vccPin = new Pin(id+"-vccpin", 0, this );
    //vccPin->setLabelText( " Vcc" );
    vccPin->setUnused( true );
    m_pin[1] = vccPin;

    Pin* gndPin = new Pin(id+"-gndpin", 0, this );
    //gndPin->setLabelText( " Gnd" );
    gndPin->setUnused( true );
    m_pin[2] = gndPin;

    m_trigpin = new Pin(id+"-trigpin", 0, this );
    //m_trigpin->setLabelText( " Trig" );
    m_pin[3] = m_trigpin;
    
    m_echo = new IoPin(id+"-outpin", 0, this, output );
    //m_echo->setLabelText( " Echo" );
    m_echo->setOutHighV( 5 );
    m_pin[4] = m_echo;
    
    SR04::initialize();
}
SR04::~SR04(){}

void SR04::stamp()
{
    m_trigpin->changeCallBack( this ); // Register for Trigger Pin changes
}

void SR04::initialize()
{
    m_lastStep = Simulator::self()->circTime();
    m_lastTrig = false;
    m_echouS = 0;
}

void SR04::voltChanged()              // Called when Trigger Pin changes
{
    bool trigState = m_trigpin->getVoltage()>2.5;
    
    if( !m_lastTrig && trigState )                 // Rising trigger Pin
    {
        m_lastStep = Simulator::self()->circTime();
    }
    else if( m_lastTrig && !trigState )            // Triggered
    {
        uint64_t time = Simulator::self()->circTime()-m_lastStep; // in picosecondss

        if( time >= 10*1e6 )     // >=10 uS Trigger pulse
        {
            m_echouS = (m_inpin->getVoltage()*2000/0.344+0.5);
            if     ( m_echouS < 116 )   m_echouS = 116;   // Min range 2 cm = 116 us pulse
            else if( m_echouS > 38000 ) m_echouS = 38000; // Timeout 38 ms
            
            Simulator::self()->addEvent( 200*1e6, this ); // Send echo after 200 us
        }
    }
    m_lastTrig = trigState;
}

void SR04::runEvent()
{
    if( m_echouS )
    {
        m_echo->scheduleState( true, 0 );
        Simulator::self()->addEvent( m_echouS*1e6, this ); // Event to finish echo
        m_echouS = 0;
    }
    else m_echo->scheduleState( false, 0 );
}
