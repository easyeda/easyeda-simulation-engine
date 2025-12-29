/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "bcdtodec.h"
#include "itemlibrary.h"
#include "circuit.h"
#include "iopin.h"

#include "boolProp.h"

//#define tr(str) simulideTr("BcdToDec",str)

Component* BcdToDec::construct( std::string type, std::string id )
{ return new BcdToDec( type, id ); }

LibraryItem* BcdToDec::libraryItem()
{
    return new LibraryItem(
        "Decoder(4 to 10/16)",
        "Converters",
        "2to3g.png",
        "BcdToDec",
        BcdToDec::construct );
}

BcdToDec::BcdToDec( std::string type, std::string id )
        : LogicComponent( type, id )
{
    m_width  = 4;
    m_height = 11;

    m_tristate = true;
    m_16Bits   = false;

    setNumInps( 4,"S" );
    setNumOuts( 10 );
    createOePin( "IU01OE ", id+"-in4"); // Output Enable

    addPropGroup( { "Main", {
        new boolProp<BcdToDec>("_16_Bits", "16 Bits","", this
                              , &BcdToDec::is16Bits, &BcdToDec::set_16bits, propNoCopy )
    },groupNoCopy} );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new boolProp<BcdToDec>("Invert_Inputs", "Invert Inputs","", this, &BcdToDec::invertInps
            , &BcdToDec::setInvertInps, propNoCopy) 
    };
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    std::vector<comProperty*> outputType = IoComponent::outputType();
    //将四类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size() + outputProps.size() + outputType.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());
    allProps.insert(allProps.end(), outputType.begin(), outputType.end());

    addPropGroup({ "Electric",allProps,0 });

    addPropGroup( { "Timing", IoComponent::edgeProps(),0 } );
}
BcdToDec::~BcdToDec(){}

void BcdToDec::stamp()
{
    for( int i=0; i<4; ++i ) m_inpPin[i]->changeCallBack( this );
    LogicComponent::stamp();

    m_outPin[0]->setOutState( true );
    m_outValue = 1;
}

void BcdToDec::voltChanged()
{
    LogicComponent::updateOutEnabled();

    int dec = 0;
    for( int i=0; i<4; ++i )
        if( m_inpPin[i]->getInpState() ) dec += pow( 2, i );

    m_nextOutVal = 1<<dec;
    scheduleOutPuts( this );
}

void BcdToDec::set_16bits( bool set )
{
    m_16Bits = set;
    setNumOuts( m_16Bits ? 16 : 10 );
    //m_oePin->setY( m_area.y()-8 );
}
