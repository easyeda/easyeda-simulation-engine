/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ledrgb.h"
#include "connector.h"
#include "circuit.h"
#include "simulator.h"
#include "itemlibrary.h"
//#include "circuitwidget.h"
#include "pin.h"

#include "boolProp.h"
#include "doubleProp.h"

//#define tr(str) simulideTr("LedRgb",str)

Component* LedRgb::construct( std::string type, std::string id )
{ return new LedRgb( type, id ); }

LibraryItem* LedRgb::libraryItem()
{
    return new LibraryItem(
        "Led Rgb",
        "Leds",
        "ledrgb.png",
        "LedRgb",
        LedRgb::construct);
}

LedRgb::LedRgb( std::string type, std::string id )
      : Component( type, id )
      , eElement( id )
{
    //m_graphical = true;
    //m_color = QColor(0,0,0);
    bright[0] = 15;
    bright[1] = 15;
    bright[2] = 15;

    setComCathode( true );

    m_pin.resize( 4 );
    for( int i=0; i<3; ++i )
    {
        m_pin[i] = new Pin(  m_id+"-Pin"+std::to_string(i), 0, this);
        //m_pin[i]->setColor( QColor(180*(i==0),120*(i==1),160*(i==2)) );
    }
    m_pin[3] = new Pin( m_id+"-Pin"+std::to_string(3), 0, this);

    for( int i=0; i<3; ++i )
    {
        std::string ledId = id+"_Led"+std::to_string(i);
        m_led[i] = new eLed( ledId );
        m_led[i]->setEpin( 0, new ePin( ledId+"-ePin"+std::to_string(0), 0 ) );
        m_led[i]->setEpin( 1, new ePin( ledId+"-ePin"+std::to_string(1), 1 ) );
        if( i > 0 ) m_led[i]->setThreshold( 3.5 );
    }
    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Electric", {
new boolProp<LedRgb>( "CommonCathode", "Common Cathode","",  this, &LedRgb::isComCathode, &LedRgb::setComCathode, propNoCopy),
new comProperty( "", "Red:","","",0),
new doubleProp<LedRgb>( "Threshold_R" , "Forward Voltage","V", this, &LedRgb::threshold_R,  &LedRgb::setThreshold_R ),
new doubleProp<LedRgb>( "MaxCurrent_R", "Max Current"    ,"A", this, &LedRgb::maxCurrent_R, &LedRgb::setMaxCurrent_R ),
new doubleProp<LedRgb>( "Resistance_R", "Resistance"     ,"Ω", this, &LedRgb::res_R,        &LedRgb::setRes_R ),
new comProperty( "", "Green:","","",0),
new doubleProp<LedRgb>( "Threshold_G" , "Forward Voltage","V", this, &LedRgb::threshold_G,  &LedRgb::setThreshold_G ),
new doubleProp<LedRgb>( "MaxCurrent_G", "Max Current"    ,"A", this, &LedRgb::maxCurrent_G, &LedRgb::setMaxCurrent_G ),
new doubleProp<LedRgb>( "Resistance_G", "Resistance"     ,"Ω", this, &LedRgb::res_G,        &LedRgb::setRes_G ),
new comProperty( "", "Blue:","","",0),
new doubleProp<LedRgb>( "Threshold_B" , "Forward Voltage","V", this, &LedRgb::threshold_B,  &LedRgb::setThreshold_B ),
new doubleProp<LedRgb>( "MaxCurrent_B", "Max Current"    ,"A", this, &LedRgb::maxCurrent_B, &LedRgb::setMaxCurrent_B ),
new doubleProp<LedRgb>( "Resistance_B", "Resistance"     ,"Ω", this, &LedRgb::res_B,        &LedRgb::setRes_B )
    }, 0} );
}
LedRgb::~LedRgb(){}

void LedRgb::stamp()
{
    eNode* comEnode = m_pin[3]->getEnode();
    for( int i=0; i<3; ++i )
    {
        eNode* node = m_pin[i]->getEnode();
        eLed* led   = m_led[i];

        if( m_commonCathode ){
            led->getEpin(0)->setEnode( node );
            led->getEpin(1)->setEnode( comEnode );
        }else{
            led->getEpin(1)->setEnode( node );
            led->getEpin(0)->setEnode( comEnode );
}   }   }

void LedRgb::updateStep()
{
    m_warning = false;
    m_crashed = false;
    bool updt = false;

    for( int i=0; i<3; ++i )
    {
        double brg = m_led[i]->brightness()*255+15;
        m_led[i]->updateBright();
        m_warning |= m_led[i]->overCurrent() > 1.5;
        m_crashed |= m_led[i]->overCurrent() > 2;
        bright[i] = m_led[i]->brightness()*255+15;
        if( bright[i] > 255 ) bright[i] = 255;
        updt |= (brg != bright[i]) | m_warning;
    }
    //if( updt ) update();
}

void LedRgb::setComCathode( bool cat )
{
    if( Simulator::self()->isRunning() ) 
        Simulator::self()->stopSim();
    m_commonCathode = cat;
    //if( cat ) m_area = QRect(-8, -10, 20, 20 );
    //else      m_area = QRect(-12, -10, 20, 20 );

    //Circuit::self()->update();
}

void LedRgb::setThreshold_R( double threshold )
{ 
    if( threshold < 1e-6 ) threshold = 1e-6;
    m_led[0]->setThreshold( threshold );
}

void LedRgb::setMaxCurrent_R( double current )
{
    if( current < 1e-6 ) current = 1e-6;
    m_led[0]->setMaxCurrent( current );
}

void LedRgb::setRes_R( double resist )
{
    if( resist == 0 ) resist = 1e-14;
    m_led[0]->setRes( resist );
}

void LedRgb::setThreshold_G( double threshold )
{
    if( threshold < 1e-6 ) threshold = 1e-6;
    m_led[1]->setThreshold( threshold );
}

void LedRgb::setMaxCurrent_G( double current )
{
    if( current < 1e-6 ) current = 1e-6;
    m_led[1]->setMaxCurrent( current );
}

void LedRgb::setRes_G( double resist )
{
    if( resist == 0 ) resist = 1e-14;
    m_led[1]->setRes( resist );
}

void LedRgb::setThreshold_B( double threshold )
{
    if( threshold < 1e-6 ) threshold = 1e-6;
    m_led[2]->setThreshold( threshold );
}

void LedRgb::setMaxCurrent_B( double current )
{
    if( current < 1e-6 ) current = 1e-6;
    m_led[2]->setMaxCurrent( current );
}

void LedRgb::setRes_B( double resist )
{
    if( resist == 0 ) resist = 1e-14;
    m_led[2]->setRes( resist );
}

