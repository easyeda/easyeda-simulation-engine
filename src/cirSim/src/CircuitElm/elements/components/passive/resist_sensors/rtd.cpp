/***************************************************************************
 *   Copyright (C) 2020 by Benoit ZERR                                     *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified 2020 by Santiago González                                    *
 *   santigoro@gmail.com                                                   *
 *                                                                         */

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "rtd.h"
#include "simulator.h"
#include "itemlibrary.h"

#include "doubleProp.h"


#define tr(str) simulideTr("RTD",str)

Component* RTD::construct( std::string type, std::string id )
{ return new RTD( type, id ); }

LibraryItem* RTD::libraryItem()
{
    return new LibraryItem(
        "RTD",
        "Resistive Sensors",
        "rtd.png",
        "RTD",
        RTD::construct );
}

RTD::RTD( std::string type, std::string id )
   : ThermistorBase( type, id )
{
    addPropGroup( { "Parameters", {
//new IntProp<RTD>( "B" , "B" ,""  , this, &RTD::bVal, &RTD::setBval, "uint" ),
new doubleProp<RTD>( "R0", "R0","Ω" , this, &RTD::getR0,   &RTD::setR0 )
    }, 0} );
    addPropGroup( { "Dial", Dialed::dialProps(), groupNoCopy } );
}
RTD::~RTD() {}

void RTD::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    //m_t0_tau = (double) (Simulator::self()->step())/1e6;

    //uint64_t time = Simulator::self()->circTime();

    //double dt = (double)time/1e6 - m_t0_tau;
    //double dr = sensorFunction( m_temp )-m_resist;

    //double res = m_resist + dr*(1.0 -exp(-dt/m_tau));
    double res = sensorFunction( m_value );
    eResistor::setRes( res );
   //if( m_propDialog ) m_propDialog->updtValues();
   //else setValLabelText( getPropStr( showProp() ) );
   // //m_lastTime = time;
}

double RTD::sensorFunction( double temp )
{
  // linear approximation
  // double r_sense = r0*(1.0+coef_temp*sense); // linear approximation
  // polynomial approximation (more realistic)
  double r_sense = m_r0*(1.0+coef_temp_a*temp+coef_temp_b*temp*temp);
  if (temp < 0) r_sense += m_r0*coef_temp_c*(temp-100)*std::pow(temp,3.0);

  return r_sense;
}
