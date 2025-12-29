/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "dectobcd.h"
#include "itemlibrary.h"
#include "circuit.h"
#include "iopin.h"

#include "boolProp.h"

#define tr(str) simulideTr("DecToBcd",str)

Component* DecToBcd::construct( std::string type, std::string id )
{ return new DecToBcd( type, id ); }

LibraryItem* DecToBcd::libraryItem()
{
    return new LibraryItem(
        "Encoder(10/16 to 4)",
        "Converters",
        "3to2g.png",
        "DecToBcd",
        DecToBcd::construct );
}

DecToBcd::DecToBcd( std::string type, std::string id )
        : LogicComponent( type, id )
{
    m_width  = 4;
    m_height = 10;

    m_tristate = true;
    m_16Bits = false;
    m_bits = 10;

    std::vector<std::string> pinList = {
        "OR03 A" ,
        "OR04 B" ,
        "OR05 C" ,
         "OR06 D" 
    };

    init( pinList );

    setNumInps( 10,"D", 1 );

    createOePin( "IU03OE ", id+"-in15"); // Output Enable

    addPropGroup( { "Main", {
new boolProp<DecToBcd>("_16_Bits", "16 Bits",""
                      , this, &DecToBcd::is16Bits, &DecToBcd::set16bits, propNoCopy ),
    }, groupNoCopy } );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new boolProp<DecToBcd>("Invert_Inputs", "Invert Inputs","", this
                              , &DecToBcd::invertInps, &DecToBcd::setInvertInps, propNoCopy) 
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
DecToBcd::~DecToBcd(){}

void DecToBcd::stamp()
{
    for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->changeCallBack( this );
    LogicComponent::stamp();
}

void DecToBcd::voltChanged()
{
    LogicComponent::updateOutEnabled();

    int i;
    for( i=m_bits-2; i>=0; --i ) { if( m_inpPin[i]->getInpState() ) break; }
    m_nextOutVal = i+1;
    scheduleOutPuts( this );
}

void DecToBcd::set16bits( bool set )
{
    m_bits =  set ? 16 : 10;
    m_16Bits = set;
    setNumInps( m_16Bits ? 16 : 10, "D", 1 );
    //m_oePin->setY( m_area.y()-8 );
}
