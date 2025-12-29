/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "logicsubc.h"
#include "circuit.h"

#include "doubleProp.h"


LogicSubc::LogicSubc( std::string type, std::string id )
         : SubCircuit( type, id )
{
    m_subcType = Chip::Logic;

    m_inHighV = 2.5;
    m_inLowV  = 2.5;
    m_ouHighV = 5;
    m_ouLowV  = 0;
    m_inImp = 1e9;
    m_ouImp = 40;
    m_propDelay = 10*1000; // 10 ns
    m_timeLH = 3000;
    m_timeHL = 4000;

    addPropGroup( { "Electric", {
    new comProperty( "", "Inputs:","","",0),
    new doubleProp<LogicSubc>( "Input_High_V", "Low to High Threshold","V", this, &LogicSubc::inputHighV, &LogicSubc::setInputHighV ),
    new doubleProp<LogicSubc>( "Input_Low_V" , "High to Low Threshold","V", this, &LogicSubc::inputLowV,  &LogicSubc::setInputLowV ),
    new doubleProp<LogicSubc>( "Input_Imped" , "Input Impedance"      ,"Ω", this, &LogicSubc::inputImp,   &LogicSubc::setInputImp ),
    new comProperty( "", "Outputs:","","",0),
    new doubleProp<LogicSubc>( "Out_High_V", "Output High Voltage","V", this, &LogicSubc::outHighV, &LogicSubc::setOutHighV ),
    new doubleProp<LogicSubc>( "Out_Low_V" , "Output Low Voltage" ,"V", this, &LogicSubc::outLowV,  &LogicSubc::setOutLowV ),
    new doubleProp<LogicSubc>( "Out_Imped" , "Output Impedance"   ,"Ω", this, &LogicSubc::outImp,  &LogicSubc::setOutImp )
    } } );
    addPropGroup( { "Edges", {
    new doubleProp<LogicSubc>( "Tpd_ps", "Gate Delay ","ps", this, &LogicSubc::propDelay, &LogicSubc::setPropDelay ),
    new doubleProp<LogicSubc>( "Tr_ps" , "Rise Time"  ,"ps", this, &LogicSubc::riseTime,  &LogicSubc::setRiseTime ),
    new doubleProp<LogicSubc>( "Tf_ps" , "Fall Time"  ,"ps", this, &LogicSubc::fallTime,  &LogicSubc::setFallTime )
    } } );
}
LogicSubc::~LogicSubc(){}

void LogicSubc::setInputHighV( double volt )
{
    m_inHighV = volt;
    for( Component* c : m_compList )
        c->setPropStr("Input_High_V", std::to_string(volt)+" V");
}

void LogicSubc::setInputLowV( double volt )
{
    m_inLowV = volt;
    for( Component* c : m_compList )
        c->setPropStr("Input_Low_V", std::to_string(volt)+" V");
}

void LogicSubc::setOutHighV( double volt )
{
    m_ouHighV = volt;
    for( Component* c : m_compList )
        c->setPropStr("Out_High_V", std::to_string(volt)+" V");
}

void LogicSubc::setOutLowV( double volt )
{
    m_ouLowV = volt;
    for( Component* c : m_compList )
        c->setPropStr("Out_Low_V", std::to_string(volt)+" V");
}

void LogicSubc::setInputImp( double imp )
{
    m_inImp = imp;
    for( Component* c : m_compList )
        c->setPropStr("Input_Imped", std::to_string(imp)+" Ω");
}

void LogicSubc::setOutImp( double imp )
{
    m_ouImp = imp;
    for( Component* c : m_compList )
        c->setPropStr("Out_Imped", std::to_string(imp)+" Ω");
}

void LogicSubc::setPropDelay( double pd )
{
    m_propDelay = pd*1e12;
    for( Component* c : m_compList )
        c->setPropStr("Tpd_ps", std::to_string(m_propDelay)+" ps");
}

void LogicSubc::setRiseTime( double time )
{
    m_timeLH = time*1e12;
    for( Component* c : m_compList )
        c->setPropStr("Tr_ps", std::to_string(m_timeLH)+" ps");
}

void LogicSubc::setFallTime( double time )
{
    m_timeHL = time*1e12;
    for( Component* c : m_compList )
        c->setPropStr("Tf_ps", std::to_string(m_timeHL)+" ps");
}
