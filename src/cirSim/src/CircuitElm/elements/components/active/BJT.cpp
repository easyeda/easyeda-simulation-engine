#include "BJT.h"
/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "itemlibrary.h"
//#include "circuitwidget.h"
//#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#include "doubleProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("BJT",str)

Component* BJT::construct(std::string type, std::string id)
{
    return new BJT(type, id);
}

LibraryItem* BJT::libraryItem()
{
    return new LibraryItem(
        "BJT",
        "Transistors",
        "bjt.png",
        "BJT",
        BJT::construct);
}

BJT::BJT(std::string type, std::string id)
    : Component(type, id)
    , eBJT(id)
{
    //m_area = QRectF(-12, -14, 28, 28);
    //setLabelPos(18, 0, 0);

    m_pin.resize(3);
    m_ePin[0] = m_pin[0] = new Pin( id + "-collector", 0, this);
    m_ePin[1] = m_pin[1] = new Pin( id + "-emiter", 1, this);
    m_ePin[2] = m_pin[2] = new Pin( id + "-base", 0, this);

    Simulator::self()->addToUpdateList(this);

    addPropGroup({ "Main", {
new boolProp<BJT>("PNP"  , "PNP"      ,"" , this, &BJT::pnp      , &BJT::setPnp),
new doubleProp<BJT>("Gain" , "Gain"     ,"" , this, &BJT::gain     , &BJT::setGain),
new doubleProp<BJT>("Vcrit", "Threshold","V", this, &BJT::threshold, &BJT::setThreshold),
    },0 });
}
BJT::~BJT() {}

void BJT::updateStep()
{
    //if (Circuit::self()->animate()) update();
    if (m_changed) voltChanged(); // m_changed cleared at eDiode::voltChanged
}
