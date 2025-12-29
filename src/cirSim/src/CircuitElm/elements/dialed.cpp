/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "dialed.h"
#include "simulator.h"
#include "circuit.h"
#include "boolProp.h"
#include "doubleProp.h"
Dialed::Dialed(std::string type, std::string  id)
    : Component(type, id)
{
    //m_areaDial = QRectF(-11, 8, 22, 22);
    //m_areaComp = QRectF(-11, -11, 22, 16);
    //m_area = m_areaComp;
    //m_graphical = true;

    //setValLabelPos(15, -20, 0);
    //setLabelPos(-16, -40, 0);

    //m_proxy = Circuit::self()->addWidget(&m_dialW);
    //m_proxy->setParentItem(this);
    //m_proxy->setPos(QPoint(-m_dialW.width() / 2, 7));
    m_slider = false;

    //QObject::connect(m_dialW.dial(), &QAbstractSlider::valueChanged, [=](int v) { dialChanged(v); });
    dialChanged();

    Simulator::self()->addToUpdateList(this);
}
Dialed::~Dialed() {}

std::vector<comProperty*> Dialed::dialProps()
{
    return {
new boolProp<Dialed>("Slider", "Slider","", this, &Dialed::slider, &Dialed::setSlider),
new doubleProp<Dialed>("Scale" , "Scale" ,"", this, &Dialed::scale , &Dialed::setScale)
    };
}

void Dialed::dialChanged() // Called when dial is rotated//旋转
{
    m_needUpdate = true;
    if (!Simulator::self()->isRunning()) updateStep();
}


void Dialed::setLinkedValue(double v, int)
{
   /* m_dialW.setValue(v);*/
}

void Dialed::setSlider(bool s)
{
    m_slider = s;

    /// QObject::disconnect( m_dialW.dial(), &QAbstractSlider::valueChanged );

    //m_dialW.setType(s ? 1 : 0);

    //QObject::connect(m_dialW.dial(), &QAbstractSlider::valueChanged, [=](int v) { dialChanged(v); });

    updateProxy();
}

void Dialed::setScale(double s)
{
    if (s <= 0) return;
    //m_dialW.setScale(s);
    updateProxy();
}


//void Dialed::setHidden(bool hide, bool hidArea, bool hidLabel)
//{
//    Component::setHidden(hide, hidArea, hidLabel);
//    if (hidArea) m_area = QRectF(0, 0, -4, -4);
//    else if (hide) m_area = m_areaDial;
//    else            m_area = m_areaComp;
//
//  /*  m_proxy->setFlag(QGraphicsItem::ItemStacksBehindParent, hide && !hidArea);*/
//}