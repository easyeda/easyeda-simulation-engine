/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "switch.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "e-node.h"
#include "pin.h"
#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"
#include "custombutton.h"

#define tr(str) simulideTr("Switch",str)

Component* Switch::construct( std::string type, std::string id )
{ return new Switch( type, id ); }

LibraryItem* Switch::libraryItem()
{
    return new LibraryItem(
        "Switch (all)",
        "Switches",
        "switch.png",
        "Switch",
        Switch::construct);
}

Switch::Switch( std::string type, std::string id )
      : SwitchBase( type, id )
{
    //m_area = QRectF(-11,-9, 22, 11 );
    //m_proxy->setPos(-8, 4 );

    SetupSwitches( 1, 1 );

    //QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); });

    addPropGroup( { "Main", {
new boolProp<Switch>("DT"        , "Double Throw"   ,""      , this, &Switch::dt,     &Switch::setDt   , propNoCopy ),
new boolProp<Switch>("Checked" ,    "Checked"," ",this,&Switch::checked,&Switch::setChecked),
new intProp <Switch>("Poles"     , "Poles"          ,"_Poles", this, &Switch::poles,  &Switch::setPoles, propNoCopy,"uint" ),
new stringProp <Switch>("Key"       , "Key"            ,""      , this, &Switch::key,    &Switch::setKey ),
    },0} );
}
Switch::~Switch(){}

void Switch::stamp()
{
    MechContact::stamp();
}

void Switch::updateState(){
    if(!m_button) return;
    m_button->setChecked(!m_button->isChecked());
    onbuttonclicked();
}

void Switch::setChecked(bool status)
{
    if(!m_button) return;
    m_button->setChecked(status);
    MechContact::setChecked(status);
}
