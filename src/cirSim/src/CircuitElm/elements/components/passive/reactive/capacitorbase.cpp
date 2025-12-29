/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

// Capacitor model using backward euler approximation
// consists of a current source in parallel with a resistor.



#include "capacitorbase.h"
#include "simulator.h"
#include "pin.h"

#include "doubleProp.h"
#include "intProp.h"

#define tr(str) simulideTr("Capacitor",str)

CapacitorBase::CapacitorBase( std::string type, std::string id )
             : Reactive( type, id )
{
    //m_area = QRectF( -10, -10, 20, 20 );

    //m_pin[0]->setLength( 12 );
    //m_pin[1]->setLength( 12 );

    m_value = m_capacitance = 0.00001; // Farads

    addPropGroup( { "Main", {
        new doubleProp<CapacitorBase>( "Capacitance", "Capacitance"    , "µF"    , this, &CapacitorBase::value   , &CapacitorBase::setValue ),
        new doubleProp<CapacitorBase>( "Resistance" , "Resistance"      ,"µΩ"    , this, &CapacitorBase::resist  , &CapacitorBase::setResist ),
        new doubleProp<CapacitorBase>( "InitVolt"   , "Initial Voltage", "V"    , this, &CapacitorBase::initVolt, &CapacitorBase::setInitVolt ),
        // new intProp <CapacitorBase>( "AutoStep"   , "Auto Step"      ,"_Steps", this, &CapacitorBase::autoStep, &CapacitorBase::setAutoStep,0,"uint" )
    },0 } );

    //setShowProp("Capacitance");
    //setPropStr( "Capacitance", "10 µF" );
}
CapacitorBase::~CapacitorBase(){}

void CapacitorBase::setCurrentValue( double c )
{
    m_capacitance = c;
    m_changed = true;
}
