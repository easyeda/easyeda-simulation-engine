/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "inductor.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleProp.h"
#include "intProp.h"

#define tr(str) simulideTr("Inductor",str)

Component* Inductor::construct( std::string type, std::string id )
{ return new Inductor( type, id ); }

LibraryItem* Inductor::libraryItem()
{
    return new LibraryItem(
        "Inductor",
        "Reactive",
        "inductor.png",
        "Inductor",
        Inductor::construct);
}

Inductor::Inductor( std::string type, std::string id )
        : Reactive( type, id )
{
    //m_pin[0]->setLength( 4 );
    //m_pin[1]->setLength( 4 );

    m_value = m_inductance = 1; // H

    addPropGroup( { "Main", {
        new doubleProp<Inductor>( "Inductance", "Inductance"     , "H"     , this, &Inductor::value,    &Inductor::setValue ),
        new doubleProp<Inductor>( "Resistance", "Resistance"     , "µΩ"     , this, &Inductor::resist  , &Inductor::setResist ),
        new doubleProp<Inductor>( "InitVolt"  ,"Initial Current", "A"     , this, &Inductor::initCurr, &Inductor::setInitCurr ),
        // new intProp <Inductor>( "AutoStep"  , "Auto Step"      , "_Steps", this, &Inductor::autoStep, &Inductor::setAutoStep,0,"uint" )
    },0 } );

    //setShowProp("Inductance");
    //setPropStr( "Inductance", "1" );
}
Inductor::~Inductor(){}

void Inductor::setCurrentValue( double c )
{
    m_inductance = c;
    m_changed = true;
}
