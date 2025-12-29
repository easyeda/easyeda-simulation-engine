/***************************************************************************
 *   Copyright (C) 2018 by c
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "elcapacitor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "e-pin.h"

#define tr(str) simulideTr("elCapacitor",str)

Component* elCapacitor::construct(std::string type, std::string id)
{
    return new elCapacitor(type, id);
}

LibraryItem* elCapacitor::libraryItem()
{
    return new LibraryItem(
        "Electrolytic Capacitor",
        "Reactive",
        "elcapacitor.png",
        "elCapacitor",
        elCapacitor::construct);
}

elCapacitor::elCapacitor(std::string type, std::string id)
    : CapacitorBase(type, id)
{
    Simulator::self()->addToUpdateList(this);
}
elCapacitor::~elCapacitor() {}

void elCapacitor::updateStep()
{
    double volt = m_ePin[0]->getVoltage() - m_ePin[1]->getVoltage();
    bool crashed = (volt < -1e-6);
    if (crashed || (crashed != m_crashed))
    {
        m_crashed = crashed;
    }
}