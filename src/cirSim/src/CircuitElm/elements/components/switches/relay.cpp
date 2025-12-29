/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "relay.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "e-node.h"

#include "doubleProp.h"
#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("Relay",str)

Component* Relay::construct( std::string type, std::string id )
{ return new Relay( type, id ); }

LibraryItem* Relay::libraryItem()
{
    return new LibraryItem(
         "Relay (all)" ,
        "Switches",
        "relay-spst.png",
        "RelaySPST",
        Relay::construct);
}

Relay::Relay( std::string type, std::string id )
     : MechContact( type, id )
{
    m_ePin.resize(4);
    m_pin.resize(2);

    m_pin0 = 4;

    m_inductor = new Inductor( "Inductor", m_id+"-inductor" );
    //m_inductor->setParentItem( this );
    //m_inductor->moveTo( QPointF( 0, 0 ) );
    m_inductor->setValue( 0.1 );  // 100 mH
    m_inductor->setResist( 100 );
    //m_inductor->setShowVal( false );

    m_pin[0] = m_inductor->getPin( 0 );
    m_pin[1] = m_inductor->getPin( 1 );
    addSignalPin( m_pin[0] );
    addSignalPin( m_pin[1] );

    m_trigCurrent = 0.02;
    m_relCurrent  = 0.01;

    //setValLabelPos(-16, 6, 0);
    //setLabelPos(-16, 8, 0);

    SetupSwitches( 1, 1 );

    addPropGroup( { "Main", {
        new boolProp<Relay>( "DT"        , "Double Throw"   ,""      , this, &Relay::dt,     &Relay::setDt, propNoCopy ),
        new boolProp<Relay>("Checked" ,    "Checked"," ",this,&Relay::checked,&Relay::setChecked),
        new intProp <Relay>( "Poles"     , "Poles"         ,"_Poles", this, &Relay::poles,  &Relay::setPoles, propNoCopy,"uint" ),
    }, 0} );
    addPropGroup( { "Electric", {
        new doubleProp<Relay>( "IOn" , "IOn" ,"A", this, &Relay::iTrig, &Relay::setITrig),
        new doubleProp<Relay>( "IOff", "IOff","A", this, &Relay::iRel,  &Relay::setIRel )
    }, 0} );
    addPropGroup( { "Coil", {
        new doubleProp<Inductor>( "Inductance", "Inductance","H", m_inductor, &Inductor::value , &Inductor::setValue ),
        new doubleProp<Inductor>( "Rcoil"     , "Resistance","Ω", m_inductor, &Inductor::resist, &Inductor::setResist),
        // new intProp <Inductor>( "AutoStep"  , "Auto Step" , "_Steps", m_inductor, &Inductor::autoStep, &Inductor::setAutoStep,0,"uint" )
    }, 0} );
}
Relay::~Relay(){}

void Relay::initialize()
{
    m_relayOn = false;

    if( Simulator::self()->isRunning() )
        m_internalEnode = new eNode( m_id+"-internaleNode" );
}

void Relay::stamp()
{
    MechContact::stamp();

    m_inductor->getPin( 0 )->changeCallBack( this );
    m_inductor->getPin( 1 )->changeCallBack( this );
}

void Relay::voltChanged()
{
    double indCurr = fabs( m_inductor->indCurrent() );
    bool relayOn;

    if( m_relayOn ) relayOn = ( indCurr > m_relCurrent );
    else            relayOn = ( indCurr > m_trigCurrent );
    m_relayOn = relayOn;

    if( relayOn != m_closed ) setSwitch( relayOn );
}
