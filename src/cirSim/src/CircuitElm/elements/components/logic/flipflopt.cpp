/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "flipflopt.h"
#include "itemlibrary.h"
#include "iopin.h"



Component* FlipFlopT::construct( std::string type, std::string id )
{ return new FlipFlopT( type, id ); }

LibraryItem* FlipFlopT::libraryItem()
{
    return new LibraryItem(
        "FlipFlop T",
        "Memory",
        "2to2.png",
        "FlipFlopT",
        FlipFlopT::construct );
}

FlipFlopT::FlipFlopT( std::string type, std::string id )
         : FlipFlopBase( type, id )
{
    m_width  = 3;
    m_height = 3;
    m_dataPins = 1;

    init({         // Inputs:
            "IL01T",
            "IU01S",
            "ID02R",
            "IL02>",
                   // Outputs:
            "OR01Q",
            "OR02!Q"
        });

    m_setPin = m_inpPin[1];
    m_rstPin = m_inpPin[2];
    m_clkPin = m_inpPin[3];

    setSrInv( true );       // Inver Set & Reset pins
    setClockInv( false );   // Don't Invert Clock pin
    setTriggerStr("Clock");
}
FlipFlopT::~FlipFlopT(){}

void FlipFlopT::calcOutput()
{
    bool T = m_inpPin[0]->getInpState();
    if( T ) m_nextOutVal = m_outPin[1]->getOutState()? 1:2; // !Q state = Toggle
}
