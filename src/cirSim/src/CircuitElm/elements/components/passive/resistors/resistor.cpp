/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */
#include "resistor.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleProp.h"

#define tr(str) simulideTr("Resistor",str)

Component* Resistor::construct( std::string type, std::string id )
{ return new Resistor( type, id ); }

LibraryItem* Resistor::libraryItem()
{
    return new LibraryItem(
        "Resistor",
        "Resistors",
        "resistor.png",
        "Resistor",
        Resistor::construct);
}

Resistor::Resistor( std::string type, std::string id )
        : Comp2Pin( type, id )
        , eResistor( id )
{
    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];



    addPropGroup( { "Main", {
new doubleProp<Resistor>( "Resistance","Resistance", "Ω", this, &Resistor::getRes, &Resistor::setResSafe )
    }, 0 } );


    setPropStr( "Resistance", "100" );
}
Resistor::~Resistor(){}


