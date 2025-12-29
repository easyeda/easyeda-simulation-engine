/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "connbase.h"
//#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
//#include "pin.h"

#include "intProp.h"

#define tr(str) simulideTr("ConnBase",str)

ConnBase::ConnBase( std::string type, std::string id )
        : Component( type, id )
        , eElement( id )
{
    //m_graphical = true;
    //this->setZValue(-1 );

    m_size = 0;
    setSize( 8 );
    //setLabelPos(-16,-44, 0);

    addPropGroup( { "Main", {
        new intProp<ConnBase>("Size", "Size","_Pins", this
                     , &ConnBase::size, &ConnBase::setSize, propNoCopy,"uint" )
    }, groupNoCopy } );
}
ConnBase::~ConnBase()
{
    m_connPins.clear();
}

void ConnBase::registerEnode( eNode* enode, int n )
{
    if( n < m_size )
    {
        if( m_sockPins[n]->conPin() ) m_sockPins[n]->registerPinsW( enode, n );
    }else{
        int nS = n- m_size;
        if( m_pin[nS]->conPin() ) m_pin[nS]->registerPinsW( enode, nS );
    }
}

void ConnBase::createPins( int c )
{
    int start = m_size;
    m_size = m_size+c;
    m_pin.resize( m_size*2 );
    m_sockPins.resize( m_size );

    for( int i=start; i<m_size; i++ )
    {
        m_pin[i] = new Pin(m_id+"-pin"+std::to_string(i), i, this);
        m_sockPins[i] = new Pin(m_id+"-pin"+std::to_string(i+m_size), i+m_size, this );
        //m_sockPins[i]->setFlag( QGraphicsItem::ItemStacksBehindParent, false );
        //m_sockPins[i]->setLength( 1 );
        m_sockPins[i]->setPinType( m_pinType );
    }
}

void ConnBase::deletePins( int d )
{
    if( d > m_size ) d = m_size;
    int start = m_size-d;

    for( int i=start; i<m_size; i++ )
    {
        deletePin( m_pin[i] );
        deletePin( m_sockPins[i] );
    }
    m_size = m_size-d;
    m_pin.resize( m_size*2 );
    m_sockPins.resize( m_size );
    
    //Circuit::self()->update();
}

void ConnBase::setSize( int size )
{
    //if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();  ---------------------------需要后续更改
    
    if( size == 0 ) size = 8;
    
    if     ( size < m_size ) deletePins( m_size-size );
    else if( size > m_size ) createPins( size-m_size );

    for( int i=m_size; i<m_size*2; i++ )
    {
        m_pin[i] = m_sockPins[i-m_size];
        m_pin[i]->setIndex( i );
    }

    m_connPins.resize( size );
    
    //m_area = QRectF(-4, -28, 8, m_size*8 );

    //Circuit::self()->update();
}

