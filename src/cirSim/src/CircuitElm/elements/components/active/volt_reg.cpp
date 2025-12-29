/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "volt_reg.h"
#include "connector.h"
#include "simulator.h"
#include "itemlibrary.h"
#include "pin.h"
#include "e-node.h"

#include "doubleProp.h"

#define tr(str) simulideTr("VoltReg",str)

Component* VoltReg::construct( std::string type, std::string id )
{ return new VoltReg( type, id ); }

LibraryItem* VoltReg::libraryItem()
{
    return new LibraryItem(
        "Volt. Regulator",
        "Other Active",
        "voltreg.png",
        "VoltReg",
        VoltReg::construct );
}

VoltReg::VoltReg( std::string type, std::string id )
       : Component( type, id )
       , eResistor( id )
{
    //m_area = QRect( -11, -8, 22, 19 );

    m_admit = 1e6;

    //setValLabelPos( 15, 12, 0 );
    
    m_pin.resize( 3 );
    m_ePin.resize( 3 );

    m_ePin[0] = m_pin[0] = new Pin(id+"-input", 0, this);
    //m_pin[0]->setLabelText( "I" );


    m_ePin[1] = m_pin[1] = new Pin(id+"-output", 1, this);
    //m_pin[1]->setLabelText( "O" );


    m_ePin[2] = m_pin[2] = new Pin( id+"-ref", 2, this );
    //m_pin[2]->setLength(5);
    //m_pin[2]->setLabelText( "R" );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { "Main", {
new doubleProp<VoltReg>("Voltage", "Output Voltage","V", this, &VoltReg::outVolt, &VoltReg::setOutVolt )
    },0} );

    //setShowProp("Voltage");
    setPropStr( "Voltage", "1.2" );
}
VoltReg::~VoltReg(){}

void VoltReg::stamp()
{
    m_connected = m_ePin[0]->isConnected()
               && m_ePin[1]->isConnected()
               && m_ePin[2]->isConnected();

    if( m_connected )
    {
        m_ePin[0]->getEnode()->addToNoLinList(this);
        m_ePin[1]->getEnode()->addToNoLinList(this);
        m_ePin[2]->getEnode()->addToNoLinList(this);

        m_ePin[0]->createCurrent();
        m_ePin[1]->createCurrent();
    }
    eResistor::stamp();
    m_lastCurrent = 0;
}

void VoltReg::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    if( m_connected ) voltChanged();
}

void VoltReg::voltChanged()
{
    double inVolt  = m_ePin[0]->getVoltage();
    double outVolt = m_ePin[2]->getVoltage()+m_vRef;

    if( inVolt < 1e-6 ) inVolt = 0;
    double delta = inVolt-outVolt;
    if( delta < 0.7 )
    {
        if( inVolt < 0.7 ) delta = inVolt;
        else               delta = 0.7;
    }
    double current = delta*m_admit;
    if (std::fabs(m_lastCurrent - current) < 1e-3) return;
    m_lastCurrent = current;
    Simulator::self()->notCorverged();

    m_pin[0]->stampCurrent( current );
    m_pin[1]->stampCurrent(-current );
}
