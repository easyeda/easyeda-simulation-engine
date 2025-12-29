/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "connector.h"

#include "simulator.h"
#include "circuit.h"
#include "pin.h"
#include "e-node.h"
#include "utils.h"

#include "stringProp.h"

Connector::Connector(std::string type, std::string id, Pin* startpin, Pin* endpin)
    : CompBase(type, id)
{
    if (id.empty()) std::cout << "ERROR! Connector::Connector empty Id";

    m_actLine = 0;
    m_lastindex = 0;
    m_freeLine = false;
    m_startPin = startpin;
    setIsBus(m_startPin->isBus());

    if (endpin) closeCon(endpin);
    else m_endPin = NULL;

    Circuit::self()->compMap()->insert({id, this});

    addPropGroup( {"Main", {
new stringProp<Connector>( "itemtype"  ,"","", this, &Connector::itemType,   &Connector::dummySetter ),
new stringProp<Connector>( "uid"       ,"","", this, &Connector::getUid,     &Connector::dummySetter ),
new stringProp<Connector>( "startpinid","","", this, &Connector::startPinId, &Connector::dummySetter ),
new stringProp<Connector>( "endpinid"  ,"","", this, &Connector::endPinId,   &Connector::dummySetter ),
    }, 0} );
}
Connector::~Connector()
{
    Circuit::self()->compMap()->erase( m_id );
}

void Connector::closeCon( Pin* endpin )
{
    //if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff(); --------------------------------------后续自己修改

    m_endPin = endpin;
    m_startPin->setConnector( this );
    m_startPin->setConnector( this );
    m_endPin->setConnector( this );
    m_startPin->setConPin( m_endPin );
    m_endPin->setConPin( m_startPin );
    m_endPin->setIsBus( m_isBus );
    m_startPin->setIsBus( m_isBus ); // StartPin will set conPin isBus


}


void Connector::remove()
{
    //if (Simulator::self()->isRunning())  CircuitWidget::self()->powerCircOff();-----------------------------------------------------

    if (m_startPin) m_startPin->connectorRemoved();
    if (m_endPin)   m_endPin->connectorRemoved();
    //remLines();
}


void Connector::setIsBus( bool bus )
{
    m_isBus = bus;
}

std::string Connector::startPinId() { return m_startPin->pinId(); }
std::string Connector::endPinId()   { return m_endPin->pinId(); }

double Connector::getVoltage() { return m_startPin->getVoltage(); }
