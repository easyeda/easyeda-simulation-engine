/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "gate.h"
#include "connector.h"
#include "circuit.h"
#include "iopin.h"

#include "boolProp.h"

//#define tr(str) simulideTr("Gate",str)

Gate::Gate( std::string type, std::string id, int inputs )
    : LogicComponent( type, id )
{
    m_width = 2;
    m_initState = false;
    m_minInputs = inputs;

    setNumOuts( 1,"",-1 );
    setNumInputs( inputs );  // Create Input Pins

    /// m_rndPD = true; // Randomize Propagation Delay:
}
Gate::~Gate(){}

std::vector<comProperty*> Gate::outputProps()
{
    std::vector<comProperty*> outProps = IoComponent::outputProps();
    outProps.insert(outProps.begin(),new boolProp<Gate>( "initHigh", "Initial High State","", this, &Gate::initHigh, &Gate::setInitHigh ) );

    return outProps;
}

void Gate::stamp()
{
    LogicComponent::stamp();
    for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->changeCallBack( this );

    m_outPin[0]->setOutState( m_initState );

    m_nextOutVal = m_outValue = m_initState;
}

void Gate::voltChanged()
{
    if( m_tristate ) LogicComponent::updateOutEnabled();

    int inputs = 0;

    for( uint32_t i=0; i<m_inpPin.size(); ++i )
    {
        bool state = m_inpPin[i]->getInpState();
        if( state ) inputs++;
    }
    bool out = calcOutput( inputs ); // In each gate type

    m_nextOutVal = out? 1:0;

    scheduleOutPuts( this );
}

bool Gate::calcOutput( int inputs )
{
    return ((uint32_t)inputs == m_inpPin.size()); // Default for: Buffer, Inverter, And, Nand
}

void Gate::setNumInputs( int inputs )
{
    if( inputs < m_minInputs ) return;
    IoComponent::setNumInps( inputs, "" );
    //m_outPin[0]->setY( 0 );
    //m_area = QRect( -11, -4*m_inpPin.size(), 19, 4*2*m_inpPin.size() );
}
