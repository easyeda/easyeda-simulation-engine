/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "led.h"
#include "pin.h"
#include "itemlibrary.h"

#include "stringProp.h"

//#define tr(str) simulideTr("Led",str)

Component* Led::construct( std::string type, std::string id )
{ return new Led( type, id ); }

LibraryItem* Led::libraryItem()
{
    return new LibraryItem(
        "Led",
        "Leds",
        "led.png",
        "Led",
        Led::construct);
}

Led::Led( std::string type, std::string id )
   : LedBase( type, id )
{
 /*   m_area = QRect(-8, -10, 20, 20 );*/

    m_isLinker = true;

    m_pin.resize( 2 );
    m_pin[0] = new Pin(m_id+"-lPin", 0, this);
    m_pin[1] = new Pin(m_id+"-rPin", 1, this);
    //m_pin[0]->setLength( 10 );

    setEpin( 0, m_pin[0] );
    setEpin( 1, m_pin[1] );

    addPropGroup( { "Hidden", {
    new stringProp<Led>("Links", "Links","", this, &Led::getLinks , &Led::setLinks )
    }, groupHidden} );
}
Led::~Led(){}

void Led::voltChanged()
{
    eLed::voltChanged();
    if( !m_converged ) return;

    for( Component* comp : m_linkedComp ) comp->setLinkedValue( m_current );
}
