/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "diode.h"
#include "itemlibrary.h"
#include "linkedComponent.h"
#include "simulator.h"
#include "e-node.h"
#include "pin.h"

#include "doubleProp.h"
#include "stringProp.h"

#define tr(str) simulideTr("Diode",str)

Component* Diode::construct( std::string type, std::string id )
{ return new Diode( type, id ); }

LibraryItem* Diode::libraryItem()
{
    return new LibraryItem(
        "Diode",
        "Rectifiers",
        "diode.png",
        "Diode",
        Diode::construct);
}

Diode::Diode( std::string type, std::string id, bool zener )
     : LinkedComponent( type, id ),eDiode( id )
{
    //m_area = QRect(-12, -8, 24, 16 );

    m_pin.resize(2);
    m_pin[0] = new Pin( id+"-lPin", 0, this);
    m_pin[1] = new Pin( id+"-rPin", 1, this);

    //setValLabelPos(-16, 6, 0 );
    //setLabelPos(-16,-24, 0 );

    for (const auto& pair : m_diodes) 
    {
        m_enumUids.push_back(pair.first);
    }
    m_enumNames = m_enumUids; // 复制键的列表

    // Pin0--eDiode--ePin1--midEnode--ePin2--eResistor--Pin1
    m_ePin[0] = m_pin[0];
    setNumEpins( 3 );

    m_resistor = new eResistor( m_elmId+"-resistor");
    m_resistor->setEpin( 0, m_ePin[2] );
    m_resistor->setEpin( 1, m_pin[1] );

    m_isZener = zener;
    if( zener )
    {
        m_diodeType = "zener";
        setModel( "Zener Default" );
    }
    else
    {
        m_diodeType = "diode";
        setModel( "Diode Default" );
    }
    Simulator::self()->addToUpdateList( this );

    addPropGroup({ "Main", {
new stringProp<Diode>("Model","Model","", this, &Diode::model,  &Diode::setModel,0,"enum"),
    },0 });
    addPropGroup({ "Electric", {
new doubleProp<Diode>("Threshold" , "Forward Voltage","V", this, &Diode::threshold,  &Diode::setThreshold),
new doubleProp<Diode>("MaxCurrent", "Max Current"    ,"A", this, &Diode::maxCurrent, &Diode::setMaxCurrent),
new doubleProp<Diode>("Resistance", "Resistance"     ,"Ω", this, &Diode::res,        &Diode::setResSafe),
    },0 });
    addPropGroup({ "Advanced", {
new doubleProp<Diode>("BrkDownV"  , "Breakdown Voltage"   ,"V" , this, &Diode::brkDownV, &Diode::setBrkDownV),
new doubleProp<Diode>("SatCurrent", "Saturation Current"  ,"nA", this, &Diode::satCur,   &Diode::setSatCur),
new doubleProp<Diode>("EmCoef"    , "Emission Coefficient",""  , this, &Diode::emCoef,   &Diode::setEmCoef),
    },0 });
}
Diode::~Diode()
{
    delete m_resistor;
}

bool Diode::setPropStr( std::string prop, std::string val )
{
    if( prop =="Zener_Volt" ) //  Old: TODELETE
    {
        double zenerV = std::stod(val);
        m_isZener = zenerV > 0;
        eDiode::setBrkDownV( zenerV );
    }
    else return Component::setPropStr( prop, val );
    return true;
}

void Diode::initialize()
{
    m_crashed = false;
    m_warning = false;

    m_midEnode = new eNode( m_elmId+"-mideNode");
    eDiode::initialize();

    //update();
}

void Diode::stamp()
{
    m_ePin[1]->setEnode( m_midEnode );
    m_ePin[2]->setEnode( m_midEnode );

    eDiode::stamp();
}

void Diode::updateStep()
{
    if( m_current > m_maxCur )
    {
        m_warning = true;
        m_crashed = m_current > m_maxCur*2;
        //update();
    }else{
        //if( m_warning ) update();
        m_warning = false;
        m_crashed = false;
    }
    if( m_changed ) voltChanged(); // m_changed cleared at eDiode::voltChanged
}

void Diode::voltChanged()
{
    eDiode::voltChanged();
    if( !m_converged ) return;

    if( !m_linkedComp.empty() )
    {
        double current = m_resistor->current();
        for( Component* comp : m_linkedComp ) comp->setLinkedValue( current );
    }



}
