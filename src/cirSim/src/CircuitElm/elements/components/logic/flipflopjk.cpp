/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "flipflopjk.h"
#include "itemlibrary.h"
#include "iopin.h"


Component* FlipFlopJK::construct( std::string type, std::string id )
{ return new FlipFlopJK( type, id ); }

LibraryItem* FlipFlopJK::libraryItem()
{
    return new LibraryItem(
        "FlipFlop JK",
        "Memory",
        "3to2.png",
        "FlipFlopJK",
        FlipFlopJK::construct );
}

FlipFlopJK::FlipFlopJK( std::string type, std::string id )
          : FlipFlopBase( type, id )
{
    m_width  = 3;
    m_height = 4;
    m_dataPins = 2;

    init({          // Inputs:
            "IL01J",
            "IL03K",
            "IU01S",
            "ID02R",
            "IL02>",
                   // Outputs:
            "OR01Q",
            "OR03!Q",
        });

    m_setPin   = m_inpPin[2];
    m_rstPin   = m_inpPin[3];
    m_clkPin   = m_inpPin[4];

    setSrInv( true );           // Invert Set & Reset pins
    setClockInv( false );       //Don't Invert Clock pin
    setTriggerStr("Clock");
}
FlipFlopJK::~FlipFlopJK(){}

void FlipFlopJK::calcOutput()
{
    //这里必须是实时值，否则当复位等强制动作执行时，端口状态来不及更新
    bool J = m_inpPin[0]->getInpState();
    bool K = m_inpPin[1]->getInpState();
    bool Q =  m_outPin[0]->getOutState();

    m_Q0 = (J && !Q) || (!K && Q) ;
    m_nextOutVal = m_Q0? 1:2;
}
