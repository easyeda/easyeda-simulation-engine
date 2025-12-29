/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "thermistorbase.h"

#include "doubleProp.h"

#define tr(str) simulideTr("Thermistor",str)

ThermistorBase::ThermistorBase( std::string type, std::string id )
              : VarResBase( type, id  )
{


    m_ptc  = false;

    ThermistorBase::setMaxVal( 200 );
    setVal( 25 );

    addPropGroup( { "Main", {
new doubleProp<ThermistorBase>( "Temp"     , "Current Value","ºC", this, &ThermistorBase::getVal,  &ThermistorBase::setVal ),
new doubleProp<ThermistorBase>( "Min_Temp" , "Minimum Value","ºC", this, &ThermistorBase::minVal,  &ThermistorBase::setMinVal ),
new doubleProp<ThermistorBase>( "Max_Temp" ,"Maximum Value","ºC", this, &ThermistorBase::maxVal,  &ThermistorBase::setMaxVal ),
new doubleProp<ThermistorBase>( "Dial_Step", "Dial Step"   ,"ºC", this, &ThermistorBase::getStep, &ThermistorBase::setStep )
    }, 0} );
}
ThermistorBase::~ThermistorBase(){}

void ThermistorBase::setPtc( bool ptc )
{
    return; // TODO
    m_ptc = ptc;
    m_needUpdate = true;
}

void ThermistorBase::setMinVal( double min )
{
    if( min > m_maxVal ) min = m_maxVal;
    m_minVal = min;

    updtValue();
}

void ThermistorBase::setMaxVal( double max )
{
    if( max < m_minVal ) max = m_minVal;
    m_maxVal = max;

    updtValue();
}

