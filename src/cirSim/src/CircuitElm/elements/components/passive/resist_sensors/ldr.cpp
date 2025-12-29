/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ldr.h"
#include "itemlibrary.h"

#include "doubleProp.h"
#include "intProp.h"


#define tr(str) simulideTr("Ldr",str)

Component* Ldr::construct( std::string type, std::string id )
{ return new Ldr( type, id ); }

LibraryItem* Ldr::libraryItem()
{
    return new LibraryItem(
        "LDR",
        "Resistive Sensors",
        "ldr.png",
        "LDR",
        Ldr::construct);
}

Ldr::Ldr( std::string type, std::string id )
   : VarResBase( type, id  )
{
    m_r1    = 127410;
    m_gamma = 0.8582;

    setVal( 1 );

    addPropGroup( { "Main", {
new doubleProp<Ldr>( "Lux"      ,  "Current Value","Lux", this, &Ldr::getVal,  &Ldr::setVal ),
new doubleProp<Ldr>( "Min_Lux"  ,  "Minimum Value","Lux", this, &Ldr::minVal,  &Ldr::setMinVal ),
new doubleProp<Ldr>( "Max_Lux"  ,  "Maximum Value","Lux", this, &Ldr::maxVal,  &Ldr::setMaxVal ),
new doubleProp<Ldr>( "Dial_Step",  "Dial Step"    ,"Lux", this, &Ldr::getStep, &Ldr::setStep )
    },0} );
    addPropGroup( {  "Parameters", {
new doubleProp<Ldr>( "Gamma",  "Gamma","" , this, &Ldr::gamma, &Ldr::setGamma ),
new intProp <Ldr>( "R1"   ,  "R1"  ,"Ω", this, &Ldr::r1,    &Ldr::setR1,0,"uint" )
    },0} );
    addPropGroup( {  "Dial", Dialed::dialProps(), 0} );
}
Ldr::~Ldr(){}

void Ldr::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    double res = double(m_r1)*pow( m_value, -m_gamma );
    eResistor::setRes( res );
    //if( m_propDialog ) m_propDialog->updtValues();
    //else setValLabelText( getPropStr( showProp() ) );
}

void Ldr::setR1( int r1 )
{
    m_r1 = r1;
    m_needUpdate = true;
}

void Ldr::setGamma( double ga )
{
    m_gamma = ga;
    m_needUpdate = true;
}
