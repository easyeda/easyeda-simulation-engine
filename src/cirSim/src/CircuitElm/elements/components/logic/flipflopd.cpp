/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "flipflopd.h"
#include "itemlibrary.h"
#include "iopin.h"

#define tr(str) simulideTr("FlipFlopD",str)

Component* FlipFlopD::construct( std::string type, std::string id )
{ return new FlipFlopD( type, id ); }

LibraryItem* FlipFlopD::libraryItem()
{
    return new LibraryItem(
        "FlipFlopD",
        "Memory",
        "2to2.png",
        "FlipFlopD",
        FlipFlopD::construct );
}

FlipFlopD::FlipFlopD( std::string type, std::string id )
         : FlipFlopBase( type, id )
{
    m_width  = 3;
    m_height = 3;
    m_dataPins = 1;

    init({         // Inputs:
            "IL01D",
            "IU01S",
            "ID02R",
            "IL02>",
                   // Outputs:
            "OR01Q",
            "OR02!Q"
        });

    m_setPin   = m_inpPin[1];
    m_rstPin = m_inpPin[2];
    m_clkPin   = m_inpPin[3];

    setSrInv( true );       // Inver Set & Reset pins
    setClockInv( false );   // Don't Invert Clock pin
    setTrigger( Clock );
}
FlipFlopD::~FlipFlopD(){}

void FlipFlopD::calcOutput()
{
    m_nextOutVal = m_inpPin[0]->getInpState()? 1:2; // D state
}
