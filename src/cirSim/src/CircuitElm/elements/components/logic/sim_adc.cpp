/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "sim_adc.h"
#include "itemlibrary.h"
#include "connector.h"
#include "simulator.h"
#include "iopin.h"

#include "doubleProp.h"
#include "intProp.h"

#define tr(str) simulideTr("ADC",str)

Component* ADC::construct( std::string type, std::string id )
{ return new ADC( type, id ); }

LibraryItem* ADC::libraryItem()
{
    return new LibraryItem(
        "ADC",
        "Other Logic",
        "1to3.png",
        "ADC",
        ADC::construct );
}

ADC::ADC( std::string type, std::string id )
   : LogicComponent( type, id )
{
    m_width  = 4;
    m_height = 9;

    //setLabelPos(-16,-80, 0);
    setNumOutputs( 8 );    // Create Output Pins
    m_maxVolt = 5;

    addPropGroup( { "Main", {
new intProp <ADC>("Num_Bits", "Size","_Bits", this
                 , &ADC::numOuts, &ADC::setNumOutputs, propNoCopy,"uint" ),

new doubleProp<ADC>("Vref", "Reference Voltage","V", this, &ADC::maxVolt, &ADC::setMaxVolt ),
    },groupNoCopy} );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size()+ outputProps.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());

    addPropGroup({ "Electric",allProps,0 });
    addPropGroup( { "Timing"   , IoComponent::edgeProps()                            ,0 } );
}
ADC::~ADC(){}

void ADC::stamp()
{
    m_inpPin[0]->changeCallBack( this );
    LogicComponent::stamp();
}

void ADC::voltChanged()
{
    double volt = m_inpPin[0]->getVoltage();
    m_nextOutVal = volt*m_maxValue/m_maxVolt+0.1;
    if( m_nextOutVal > m_maxValue ) m_nextOutVal = m_maxValue;
    LogicComponent::scheduleOutPuts( this );
}

void ADC::setNumOutputs( int outs )
{
    if( outs < 1 ) return;
    m_maxValue = pow( 2, outs )-1;
    IoComponent::setNumOuts( outs, "D" );
    IoComponent::setNumInps( 1, "In" );
}
