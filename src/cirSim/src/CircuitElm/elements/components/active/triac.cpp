/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "triac.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "e-diode.h"
#include "e-node.h"
#include "pin.h"

#include "doubleProp.h"

#define tr(str) simulideTr("Triac",str)

Component* Triac::construct( std::string type, std::string id )
{ return new Triac( type, id ); }

LibraryItem* Triac::libraryItem()
{
    return new LibraryItem(
        "Triac",
        "Rectifiers",
        "triac.png",
        "Triac",
        Triac::construct );
}

Triac::Triac( std::string type, std::string id )
     : Component( type, id )
     , eElement( id )
{
    //m_area =  QRectF( -8, -16, 16, 32 );
    //setLabelPos(-12,-30, 0 );

    m_holdCurr = 0.0082;
    m_trigCurr = 0.01;
    m_gateRes  = 100;

    // Pin0--|--ePin0--diode1--ePin1--midEnode--ePin4--resistor--Pin1
    //       |--ePin3--diode2--ePin2--|                         |
    // Pin2-----------------------------------resistGa--ePin5---|
    m_pin.resize( 3 );
    m_pin[0] = new Pin(id+"-lPin", 0, this);
    m_pin[1] = new Pin(id+"-rPin", 1, this);
    m_pin[2] = new Pin(id+"-gPin", 2, this);
    //m_pin[2]->setPinAngle(-26 );
    //m_pin[2]->setLength( 10 );

    setNumEpins( 6 );

    m_diode1 = new eDiode( id+"-dio1" );
    m_diode1->setEpin( 0, m_ePin[0] );
    m_diode1->setEpin( 1, m_ePin[1] );
    m_diode1->setModel( "Diode Default" );

    m_diode2 = new eDiode( id+"-dio2" );
    m_diode2->setEpin( 0, m_ePin[2] );
    m_diode2->setEpin( 1, m_ePin[3] );
    m_diode2->setModel( "Diode Default" );

    m_resistor = new eResistor( m_elmId+"-resistor");
    m_resistor->setEpin( 0, m_ePin[4] );
    m_resistor->setEpin( 1, m_pin[1] );

    m_resistGa = new eResistor( m_elmId+"-gateRes");
    m_resistGa->setEpin( 0, m_pin[2] );
    m_resistGa->setEpin( 1, m_ePin[5] );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new doubleProp<Triac>("GateRes" , "Gate Resistance","Ω", this, &Triac::gateRes , &Triac::setGateRes  ),
new doubleProp<Triac>("TrigCurr", "Trigger Current","A", this, &Triac::trigCurr, &Triac::setTrigCurr ),
new doubleProp<Triac>("HoldCurr", "Holding Current","A", this, &Triac::holdCurr, &Triac::setHoldCurr )
    },0} );
}
Triac::~Triac()
{
    delete m_diode1;
    delete m_diode2;
    delete m_resistor;
    delete m_resistGa;
}

void Triac::initialize()
{
    m_midEnode = new eNode( m_elmId+"-mideNode");
}

void Triac::stamp()
{
    m_state = false;

    eNode* nodeT2 = m_pin[0]->getEnode();
    eNode* nodeT1 = m_pin[1]->getEnode();
    eNode* nodeG = m_pin[2]->getEnode();

    m_diode1->getEpin(0)->setEnode( nodeT2 );
    m_diode1->getEpin(1)->setEnode( m_midEnode );

    m_diode2->getEpin(0)->setEnode( m_midEnode );
    m_diode2->getEpin(1)->setEnode( nodeT2 );

    m_resistor->getEpin(0)->setEnode( m_midEnode );
    m_resistGa->getEpin(1)->setEnode( nodeT1 );

    if( nodeT2 ) nodeT2->addToNoLinList( this );
    if( nodeT1 ) nodeT1->addToNoLinList( this );
    if( nodeG ) nodeG->addToNoLinList( this );

    m_resistor->setRes( 10e5 );
    m_resistGa->setRes( m_gateRes );
}

void Triac::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    m_resistGa->setRes( m_gateRes );
    voltChanged();
}

void Triac::voltChanged()
{
    double current1 = m_resistor->current();
    double currentG = m_resistGa->current();
    bool state = m_state;
//qDebug() << "Triac::voltChanged"<<current1 <<currentG;
    if( std::fabs(current1) < m_holdCurr ) state = false; /// Mingw needs std::fabs
    if( std::fabs(currentG) > m_trigCurr ) state = true;

    if( m_state != state )
    {
        m_state = state;
        double res = state ? 0.01 : 10e5;
        m_resistor->setRes( res );
    }
}
