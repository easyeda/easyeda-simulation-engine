/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "mosfet.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "iopin.h"

#include "doubleProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("Mosfet",str)

Component* Mosfet::construct( std::string type, std::string id )
{ return new Mosfet( type, id ); }

LibraryItem* Mosfet::libraryItem()
{
    return new LibraryItem(
        "Mosfet",
        "Transistors",
        "mosfet.png",
        "Mosfet",
        Mosfet::construct);
}

Mosfet::Mosfet( std::string type, std::string id )
      : Component( type, id )
      , eMosfet( id )
{
    //m_area = QRectF(-12, -14, 28, 28 );
    //setLabelPos( 18, 0, 0 );

    m_pin.resize(3);
    m_ePin[0] = m_pin[0] = new Pin(id+"-Dren", 0, this );
    m_ePin[1] = m_pin[1] = new Pin(id+"-Sour", 1, this );
    m_ePin[2] = m_pin[2] = new IoPin(id+"-Gate", 0, this, input );
    
    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new boolProp<Mosfet>("P_Channel", "P Channel","", this, &Mosfet::pChannel,  &Mosfet::setPchannel ),
new boolProp<Mosfet>("Depletion", "Depletion","", this, &Mosfet::depletion, &Mosfet::setDepletion ),
    },0} );
    addPropGroup( { "Electric", {
new doubleProp<Mosfet>("RDSon"    , "RDSon"    ,"Ω", this, &Mosfet::rdson,     &Mosfet::setRDSon),
new doubleProp<Mosfet>("Threshold", "Threshold","V", this, &Mosfet::threshold, &Mosfet::setThreshold )
    },0} );
}
Mosfet::~Mosfet(){}

void Mosfet::updateStep()
{
    //if( Circuit::self()->animate() ) update();

    if( !m_changed ) return;
    m_changed = false;

    updateValues();
    voltChanged();
}
