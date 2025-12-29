/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "varresistor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "doubleProp.h"

#define tr(str) simulideTr("VarResistor",str)

Component* VarResistor::construct( std::string type, std::string id )
{ return new VarResistor( type, id ); }

LibraryItem* VarResistor::libraryItem()
{
    return new LibraryItem(
        "Variable Resistor",
        "Resistors",
        "varresistor.png",
        "VarResistor",
        VarResistor::construct);
}

VarResistor::VarResistor( std::string type, std::string id )
           : VarResBase( type, id  )
{
    setVal( 0 );

    addPropGroup( { "Main", {
new doubleProp<VarResistor>( "Min_Resistance", "Minimum Resistance", "Ω", this, &VarResistor::minVal, &VarResistor::setMinVal ),
new doubleProp<VarResistor>( "Max_Resistance", "Maximum Resistance", "Ω", this, &VarResistor::maxVal, &VarResistor::setMaxVal ),
new doubleProp<VarResistor>( "Value_Ohm"     , "Current Value"    , "Ω", this, &VarResistor::getVal, &VarResistor::setVal ),
new doubleProp<VarResistor>( "Dial_Step"     , "Dial Step"         , "Ω", this, &VarResistor::getStep,&VarResistor::setStep ),
    },0 } );
    addPropGroup( { "Dial", Dialed::dialProps(), groupNoCopy } );
}
VarResistor::~VarResistor(){}

void VarResistor::updateStep()
{
    if( !m_needUpdate ) return;
    m_needUpdate = false;

    eResistor::setRes( m_value );
    //if( m_propDialog ) m_propDialog->updtValues();
    //else setValLabelText( getPropStr( showProp() ) );
}

//void VarResistor::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
//{
//    if( m_hidden ) return;
//    Component::paint( p, option, widget );
//
//    p->drawRect( QRectF(-11,-4.5, 22, 9 ) );
//    p->drawLine(-6, 6, 8,-8 );
//    p->drawLine( 8,-6, 8,-8 );
//    p->drawLine( 8,-8, 6,-8 );
//
//    Component::paintSelected( p );
//}
