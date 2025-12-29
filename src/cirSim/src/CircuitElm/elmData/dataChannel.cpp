/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "dataChannel.h"
#include "simulator.h"
#include "e-node.h"
#include "pin.h"

dataChannel::dataChannel(plotBase* plotBase,std::string& id)
    : eElement(id) , Updatable()
{
    m_plotBase = plotBase;
    m_ePin.resize(2);
    m_pin = NULL;
    m_ePin[1] = NULL;
    m_chTunnel = "";
    m_trigIndex = 0;
    m_pauseOnCond = false;
}
dataChannel::~dataChannel() {}

void dataChannel::stamp()    // Called at Simulation Start
{
    m_bufferCounter = 0;
    m_trigIndex = 0;
    bool connected = false;

    eNode* enode = m_ePin[0]->getEnode();
    if (enode) {
        enode->voltChangedCallback(this);
        connected = true;
    }
    //m_plotBase->display()->connectChannel(m_channel, connected);

    if (!m_ePin[1]) return;
    m_ePin[1]->changeCallBack(this);
}

bool dataChannel::isBus()
{
    if (m_pin) return m_pin->isBus();
    return false;
}
