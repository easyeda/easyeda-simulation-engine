/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "rail.h"
#include "simulator.h"
#include "iopin.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleProp.h"

#define tr(str) simulideTr("Rail",str)

Component* Rail::construct( std::string type, std::string id )
{ return new Rail( type, id ); }

LibraryItem* Rail::libraryItem()
{
    return new LibraryItem(
        "Rail",
        "Sources",
        "rail.png",
        "Rail",
        Rail::construct );
}

Rail::Rail( std::string type, std::string id )
    : Component( type, id )
    , eElement( id )
{

    m_changed = false;

    m_pin.resize(1);
    m_pin[0] = m_out = new IoPin(id+"-outnod", 0, this, source );



    addPropGroup( { "Main", {
new doubleProp<Rail>( "Voltage", "Voltage","V", this, &Rail::volt, &Rail::setVolt )
    }, 0} );

}
Rail::~Rail() { delete m_out; }

void Rail::setVolt( double v )
{
    Simulator::self()->pauseSim();
    m_volt = v;
    stamp();
    Simulator::self()->resumeSim();
}

void Rail::stamp()
{
    m_out->setOutHighV( m_volt );
    m_out->setOutState( true );
}

