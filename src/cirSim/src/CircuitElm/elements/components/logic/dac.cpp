/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "dac.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "iopin.h"

#include "doubleProp.h"
#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("DAC",str)

Component* DAC::construct( std::string type, std::string id )
{ return new DAC( type, id ); }

LibraryItem* DAC::libraryItem()
{
    return new LibraryItem(
        "DAC",
        "Other Logic",
        "3to1.png",
        "DAC",
        DAC::construct );
}

DAC::DAC( std::string type, std::string id )
   : LogicComponent( type, id )
{    
    m_width  = 4;
    m_height = 9;

    //setLabelPos(-16,-80, 0);
    setNumInps( 8 );       // Create Input Pins
    setNumOuts( 1, "Out" );
    m_maxVolt = 5;

    addPropGroup( { "Main", {
        new intProp <DAC>("Num_Bits", "Size"             ,"_Bits", this
                         , &DAC::numInps, &DAC::setNumInps, propNoCopy,"uint" ),

        new doubleProp<DAC>("Vref", "Reference Voltage","V", this, &DAC::maxVolt, &DAC::setMaxVolt )
    }, groupNoCopy } );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new boolProp<IoComponent>("Invert_Inputs", "Invert Inputs","", this
                                 , &IoComponent::invertInps, &IoComponent::setInvertInps, propNoCopy) 
    };
    //将两类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    addPropGroup({ "Electric",allProps,0 });

    addPropGroup( { "Timing", IoComponent::edgeProps(),0 } );
}
DAC::~DAC(){}

void DAC::stamp()
{
    for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->changeCallBack( this );

    m_outPin[0]->setOutState( true );
    m_val = -1;
}

void DAC::voltChanged()
{
    m_val = 0;

    for( uint32_t i=0; i<m_inpPin.size(); ++i )
        if( m_inpPin[i]->getInpState() ) m_val += pow( 2, i );

    Simulator::self()->addEvent( m_delayBase*m_delayMult, this );
}

void DAC::runEvent()
{
    double v = m_maxVolt*m_val/m_maxValue;

    m_outPin[0]->setOutHighV( v );
    m_outPin[0]->setOutState( true );
}

void DAC::setNumInps( int inputs )
{
    if( inputs < 1 ) return;
    m_maxValue = pow( 2, inputs )-1;
    IoComponent::setNumInps( inputs, "D" );
    IoComponent::setNumOuts( 1, "Out" );
}
