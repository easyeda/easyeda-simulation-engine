/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ledbase.h"
//#include "circuitwidget.h"
#include "connector.h"
#include "simulator.h"
#include "e-node.h"
#include "pin.h"

#include "doubleProp.h"
#include "stringProp.h"
#include "boolProp.h"

//#define tr(str) simulideTr("LedBase",str)

eNode LedBase::m_gndEnode("");
int   LedBase::m_overBright = 0;

LedBase::LedBase( std::string type, std::string id )
       : Component( type, id )
       , eLed( id )
{
    //m_graphical = true;
    m_grounded  = false;
    m_intensity = 0;

    m_enumUids = {
        "Yellow",
        "Red",
        "Green",
        "Blue",
        "Orange",
        "Purple",
        "White"
    };

    m_enumNames = {
    "Yellow",
    "Red",
    "Green",
    "Blue",
    "Orange",
    "Purple",
    "White"
    };

    //m_color = QColor( Qt::black );
    setColorStr("Yellow");

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new stringProp <LedBase>("Color"   , "Color"   ,"", this, &LedBase::colorStr, &LedBase::setColorStr,0,"enum" ),
new boolProp<LedBase>("Grounded", "Grounded","", this, &LedBase::grounded, &LedBase::setGrounded, propNoCopy ),
    }, 0} );

    addPropGroup( { "Electric", {
new doubleProp<LedBase>("Threshold" , "Forward Voltage","V", this, &LedBase::threshold,  &LedBase::setThreshold ),
new doubleProp<LedBase>("MaxCurrent", "Max Current"    ,"A", this, &LedBase::maxCurrent, &LedBase::setMaxCurrent ),
new doubleProp<LedBase>("Resistance", "Resistance"     ,"Ω", this, &LedBase::res,        &LedBase::setRes ),
    }, 0} );
}
LedBase::~LedBase(){}

void LedBase::initialize()
{
    m_crashed = false;
    m_warning = false;

    if( m_grounded ) m_ePin[1]->setEnode( &m_gndEnode );

    eLed::initialize();
}

void LedBase::updateStep()
{
    uint32_t intensity = m_intensity;
    eLed::updateBright();

    if( overCurrent() > 1.5 )
    {
        m_warning = true;
        m_crashed = overCurrent() > 2;

    }
    else
    {
        //if( m_warning ) update();
        m_warning = false;
        m_crashed = false;
    }
    //if( intensity != m_intensity ) update();
    if( m_changed )
    {
        m_changed = false;
        voltChanged();
    }

}

void LedBase::setGrounded( bool grounded )
{
    if( grounded == m_grounded ) return;
    m_grounded = grounded;

    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    Pin* pin1 = static_cast<Pin*>(m_ePin[1]);
    //pin1->setEnabled( !grounded );
    //pin1->setVisible( !grounded );
    if( grounded ) pin1->removeConnector();
    else           pin1->setEnode( NULL );
}

void LedBase::setColorStr( std::string foreColor )
{
    int ledColor = getEnumIndex( foreColor );
    m_ledColor = (LedColor)ledColor;
    double thr;
    switch( m_ledColor ) {
        case yellow: thr = 2.4; break;
        case red:    thr = 1.8; break;
        case green:  thr = 3.5; break;
        case blue:   thr = 3.6; break;
        case orange: thr = 2.0; break;
        case purple: thr = 3.5; break;
        case white:  thr = 4.0; break;
        default:     thr = 2.4; break;
    }
    eLed::setThreshold( thr );
    //if( m_showVal && (m_showProperty == "Color") )
    //    setValLabelText( m_enumNames.at( ledColor ) );
}

