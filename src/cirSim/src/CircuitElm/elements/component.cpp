/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "component.h"
#include "label.h"
//#include "mainwindow.h"
#include "connector.h"
//#include "connectorline.h"
//#include "circuitwidget.h"
#include "circuit.h"
#include "utils.h"
//#include "propdialog.h"
#include "linkable.h"

#include "doubleProp.h"
#include "boolProp.h"
#include "intProp.h"
#include "stringProp.h"
//#include "pointprop.h"


#define tr(str) simulideTr("Component",str)

int  Component::m_error = 0;
bool Component::m_boardMode = false;

Component::Component( std::string type, std::string id )
         : CompBase( type, id )
{
    m_help = "";
    m_isMainComp = false;

    m_warning    = false;

    m_isLinker   = false;
    m_linkedTo   = nullptr;
    ctl = true;

    //m_showProperty = "";
    m_linkNumber = -1;




    addPropGroup( { "CompBase", {
    new stringProp <Component>("itemtype","","", this, &Component::itemType,&Component::setItemType ),
    new stringProp <Component>("CircId"  ,"","", this, &Component::getUid,&Component::setUid ),
    new boolProp<Component>("mainComp","","", this, &Component::isMainComp,&Component::setMainComp ),// Related to Subcircuit:
    }, groupHidden | groupNoCopy } );


}

Component::~Component(){}

bool Component::setPropStr( std::string prop, std::string val )
{
    return CompBase::setPropStr(prop, val);
    //return true;
}

void Component::substitution( std::string &propName ) // static, Old: TODELETE
{
    if     ( propName == "Volts"       ) propName = "Voltage";
    else if( propName == "id"          ) propName = "label";
    else if( propName == "Duty_Square" ) propName = "Duty";
    else if( propName == "S_R_Inverted") propName = "Reset_Inverted";
    else if( propName == "Show_res"
          || propName == "Show_Volt"
          || propName == "Show_volt"
          || propName == "Show_Amp"
          || propName == "Show_Ind"
          || propName == "Show_Ind"
          || propName == "Show_Cap" )    propName = "Show_Val";
   /// else if( propName == "Inverted")    propName = "InvertOuts";
}

void Component::setInvertPins(std::string pins)
{
    m_invertedPins = pins;
    std::vector<std::string> pinList = split(pins,',');
    for( std::string pinStr : pinList )
    {
        Pin* pin = Circuit::self()->getPin( m_id+"-"+pinStr );
        if( pin ) pin->setInverted( !pin->inverted() );
    }
}

void Component::remove()
{
    for( uint32_t i=0; i<m_pin.size(); i++ )
        //if( m_pin[i] ) m_pin[i]->removeConnector();

    Circuit::self()->compRemoved( true );
}

void Component::deletePin( Pin* pin )
{
    pin->removeConnector();
    m_signalPin.erase(std::remove(m_signalPin.begin(), m_signalPin.end(), pin), m_signalPin.end());
    delete pin;
}
void Component::addSignalPin( Pin* pin )
{
    // 使用 std::find 检查 pin 是否已经在 m_signalPin 中
    if (std::find(m_signalPin.begin(), m_signalPin.end(), pin) == m_signalPin.end()) 
    {
        m_signalPin.push_back(pin); // 如果不在，添加到 m_signalPin
    }
}

void Component::remSignalPin( Pin* pin )
{
    // 使用 std::remove 移除 pin，然后调用 erase 移除容器中的 "空" 元素
    m_signalPin.erase(std::remove(m_signalPin.begin(), m_signalPin.end(), pin), m_signalPin.end());
}

bool Component::setLinkedTo(Linkable *li)
{
    if( li && m_linkedTo ) return false;
    if( !li ) m_linkNumber = -1;
    m_linkedTo = li;
    return true;
}
