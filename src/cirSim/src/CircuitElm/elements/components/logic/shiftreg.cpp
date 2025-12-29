/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "shiftreg.h"
#include "itemlibrary.h"
#include "iopin.h"

#include "boolProp.h"

//#define tr(str) simulideTr("ShiftReg",str)

Component* ShiftReg::construct( std::string type, std::string id )
{ return new ShiftReg( type, id ); }

LibraryItem* ShiftReg::libraryItem()
{
    return new LibraryItem(
        "Shift Register" ,
        "Arithmetic",
        "1to3.png",
        "ShiftReg",
        ShiftReg::construct );
}

ShiftReg::ShiftReg( std::string type, std::string id )
        : LogicComponent( type, id )
{
    m_width  = 4;
    m_height = 9;

    init({         // Inputs:
            "IL03DI",
            "IL05>",
            "IL07Rst"
        });

    setNumOuts( 8, "Q" );
    createOePin( "IU01OE ", id+"-in3");

    m_clkPin = m_inpPin[1];

    setResetInv( true );         // Invert Reset Pin

    addPropGroup( { "Main", {
        new boolProp<ShiftReg>("Clock_Inverted", "Clock Inverted","", this, &ShiftReg::clockInv, &ShiftReg::setClockInv ),
        new boolProp<ShiftReg>("Reset_Inverted", "Reset Inverted","", this, &ShiftReg::resetInv, &ShiftReg::setResetInv ),
    }, groupNoCopy} );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    std::vector<comProperty*> outputType = IoComponent::outputType();
    //将三类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + outputProps.size() + outputType.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());
    allProps.insert(allProps.end(), outputType.begin(), outputType.end());

    addPropGroup({ "Electric",allProps,0 });
    addPropGroup( { "Timing"   , IoComponent::edgeProps(),0 } );
}
ShiftReg::~ShiftReg(){}

void ShiftReg::stamp()
{
    m_clkPin->changeCallBack( this );
    m_inpPin[2]->changeCallBack( this ); // Reset
    LogicComponent::stamp();
}

void ShiftReg::voltChanged()
{
    updateOutEnabled();
    updateClock();

    bool clkRising = (m_clkState == Clock_Rising);// Get Clk to don't miss any clock changes
    bool     reset = m_inpPin[2]->getInpState();

    if( reset ) m_nextOutVal = 0;        // Reset shift register
    else if( clkRising )                 // Clock rising edge
    {
        m_nextOutVal <<= 1;

        bool data = m_inpPin[0]->getInpState();
        if( data ) m_nextOutVal += 1;
    }
    scheduleOutPuts( this );
}

void ShiftReg::setResetInv( bool inv )
{
    m_resetInv = inv;
    m_inpPin[2]->setInverted( inv );
}
