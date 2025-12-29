/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "fulladder.h"
#include "itemlibrary.h"
#include "iopin.h"
#include "intProp.h"

#define tr(str) simulideTr("FullAdder",str)

Component* FullAdder::construct( std::string type, std::string id)
{ return new FullAdder( type, id); }

LibraryItem* FullAdder::libraryItem()
{
    return new LibraryItem(
        "Full Adder",
        "Arithmetic",
        "2to2.png",
        "FullAdder",
        FullAdder::construct );
}

FullAdder::FullAdder( std::string type, std::string id)
         : LogicComponent( type, id )
{
    m_bits   = 0;

    m_inpPin.resize( 1 );
    m_outPin.resize( 1 );

     m_inpPin[0]  = m_ciPin = createPin("IR01Ci", m_id+"-ci");
    m_outPin[0] = m_coPin = createPin("OR03Co", m_id+"-co");
    setBits( 1 );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    addPropGroup( { "Main", {
        new intProp <FullAdder>("Bits", "Size","_bits"
                            , this, &FullAdder::bits, &FullAdder::setBits, propNoCopy,"uint" ),
    }, groupNoCopy } );
    //将两类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + outputProps.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());

    addPropGroup({ "Electric",allProps,0 });
    addPropGroup( { "Timing"   , IoComponent::edgeProps(),0 } );
}
FullAdder::~FullAdder(){}

void FullAdder::stamp()
{
    IoComponent::initState();
    for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->changeCallBack( this );
}

void FullAdder::voltChanged()
{
    int Ci = m_ciPin->getInpState() ? 1 : 0;
    int A = 0, B = 0;
    for (int i = 0; i < m_bits; ++i) {
        if (m_inpPin[1 + i]->getInpState())        A |= 1 << i;
        if (m_inpPin[1 + i + m_bits]->getInpState()) B |= 1 << i;
    }
    int total = A + B + Ci;
    bool Co = total >> m_bits;                     
    int S = total & ((1 << m_bits) - 1);           
    m_nextOutVal = (S << 1) | (Co ? 1 : 0);       
    scheduleOutPuts(this);
}
void FullAdder::setBits( int b )
{
    if( b < 1 ) b = 1;
    if( b == m_bits ) return;

    setNumInps( 1+b*2, "I", 0, 10 ); // Set a label to get Pin 1 full cell below edge
    setNumOuts( 1+b  , "S", 0, 10 );
    if(m_ciPin){
        deletePin( m_inpPin[0]);
        m_inpPin[0]  = m_ciPin;
    }
    if(m_coPin){
        deletePin( m_outPin[0]);
        m_outPin[0]  = m_coPin;
    }
    m_bits = b;
}

Pin* FullAdder::getPin( std::string pinName )
{
    if( pinName == "in0" ) return m_inpPin[1];
    if( pinName == "in1" ) return m_inpPin[2];
    if( pinName == "in2" ) return m_ciPin;
    if( pinName == "out0") return m_outPin[1];
    if( pinName == "out1") return m_coPin;
    /// TODO: find pins from number in pinName
    return nullptr;
}

