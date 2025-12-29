/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "bus.h"
//#include "connector.h"
#include "simulator.h"
#include "circuit.h"
//#include "circuitwidget.h"
#include "itemlibrary.h"
#include "e-node.h"

#include "intProp.h"

#define tr(str) simulideTr("Bus",str)

Component* Bus::construct( std::string type, std::string id )
{ return new Bus( type, id ); }

LibraryItem* Bus::libraryItem()
{
    return new LibraryItem(
        "Bus",
        "Connectors",
        "bus.png",
        "Bus",
        Bus::construct );
}

Bus::Bus( std::string type, std::string id )
   : Component( type, id )
   , eElement( id )
{
    m_busPin1 = new Pin(m_id+"-busPinI", 1, this );
    //m_busPin1->setFlag( QGraphicsItem::ItemStacksBehindParent, false );
    //m_busPin1->setLength( 1 );
    m_busPin1->setIsBus( true );

    m_numLines = 0;
    m_startBit = 0;
    setNumLines( 8 );       // Create Input Pins

    m_ePin[0] = m_pin[0] = m_busPin0 = new Pin( m_id+"-ePin0", 1, this );
    //m_busPin0->setFlag( QGraphicsItem::ItemStacksBehindParent, false );
    //m_busPin0->setLength( 1 );
    m_busPin0->setIsBus( true );

    addPropGroup( { "Main", {
new intProp<Bus>("Num_Bits" , "Size","_Bits", this
                , &Bus::numLines, &Bus::setNumLines, propNoCopy,"uint" ),

new intProp<Bus>("Start_Bit", "Start Bit","", this, &Bus::startBit, &Bus::setStartBit,0,"uint" )
    }, groupNoCopy } );
}
Bus::~Bus(){}

//关联总线引脚和电子节点
void Bus::registerEnode( eNode* enode, int n )
{
    if( m_busPin0->conPin() ) m_busPin0->registerPinsW( enode, n );
    if( m_busPin1->conPin() ) m_busPin1->registerPinsW( enode, n );

    int i = n + 1 - m_startBit;
    if( i < 0 ) return;
    if( i > m_numLines ) return;

    if( m_pin[i]->conPin() ) m_pin[i]->registerPinsW( enode, -1 );
}

void Bus::setNumLines( int lines )
{
    /*if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();*/

    if( lines == m_numLines ) return;
    if( lines < 1 ) return;

    for( int i=1; i<=m_numLines; i++ ) deletePin( m_pin[i] );

    m_numLines = lines;

    m_pin.resize( lines+2 );
    m_ePin.resize( lines+2 );
    m_signalPin.clear();
    
    for( int i=1; i<=lines; i++ )
    {
        std::string pinId = m_id + "-ePin" + std::to_string(i);
        Pin* pin = new Pin(pinId, m_startBit + i - 1, this);

        //pin->setFontSize( 4 );
        //pin->setLabelColor( QColor( 0, 0, 0 ) );
        //pin->setLabelText( " "+std::string::number( m_startBit+i-1 )+" " );
        m_pin[i]  = pin;
        m_ePin[i] = pin;
        m_signalPin.push_back(pin); // Changed from append to push_back for std::vector
    }
    //m_busPin1->setPos( QPoint( 0 ,-lines*8+8 ) );
    //m_busPin1->isMoved();
    m_pin[ lines+1 ]  = m_busPin1;
    m_ePin[ lines+1 ] = m_busPin1;

    m_height = lines-1;
    //m_area = QRect( -3,-m_height*8-2, 5, m_height*8+4 );

    //setflip();
    //Circuit::self()->update();
}

void Bus::setStartBit( int bit )
{
    //if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();
    if( bit < 0 ) bit = 0;
    m_startBit = bit;

    for( int i=1; i<=m_numLines; i++ )
    {
        m_pin[i]->setIndex( m_startBit+i-1 );
        //m_pin[i]->setLabelText( " "+std::string::number( m_startBit+i-1 ) );
    }
}