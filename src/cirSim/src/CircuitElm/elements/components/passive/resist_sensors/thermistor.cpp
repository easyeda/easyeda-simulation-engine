/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "thermistor.h"
#include "itemlibrary.h"

#include "intProp.h"


#define tr(str) simulideTr("Thermistor",str)

Component* Thermistor::construct( std::string type, std::string id )
{ return new Thermistor( type, id ); }

LibraryItem* Thermistor::libraryItem()
{
    return new LibraryItem(
        "Thermistor",
        "Resistive Sensors",
        "thermistor.png",
        "Thermistor",
        Thermistor::construct);
}

Thermistor::Thermistor( std::string type, std::string id )
          : ThermistorBase( type, id  )
{
    m_bVal = 3455;
    m_r25  = 10000;

    addPropGroup( { "Parameters", {
new intProp<Thermistor>( "B"  , "B"  ,""  , this, &Thermistor::bVal, &Thermistor::setBval,0,"uint" ),
new intProp<Thermistor>( "R25", "R25","Ω" , this, &Thermistor::r25,  &Thermistor::setR25, 0,"uint" )
    }, 0} );
    addPropGroup( { "Dial", Dialed::dialProps(), groupNoCopy } );
}
Thermistor::~Thermistor(){}

void Thermistor::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    double t0 = 25+273.15;      // Temp in Kelvin
    double t = m_value+273.15;
    double e = 2.7182;
    //double k = t*t0/(t-t0);
    //double res = m_r25/pow( e, m_bVal/k );
    double k = (t0-t)/(t*t0);
    double res = m_r25* std::pow( e, m_bVal*k );
    eResistor::setRes( res );
  /*  if( m_propDialog ) m_propDialog->updtValues();
    else setValLabelText( getPropStr( showProp() ) );*/
}

void Thermistor::setBval( int bval )
{
    m_bVal = bval;
    m_needUpdate = true;
}

void Thermistor::setR25( int r25 )
{
    m_r25 = r25;
    m_needUpdate = true;
}
