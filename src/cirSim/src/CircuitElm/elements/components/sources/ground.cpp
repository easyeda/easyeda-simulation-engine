/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "ground.h"
#include "itemlibrary.h"
#include "iopin.h"

#define tr(str) simulideTr("Ground",str)

Component* Ground::construct( std::string type, std::string id )
{ return new Ground( type, id ); }

LibraryItem* Ground::libraryItem()
{
    return new LibraryItem(
        "Ground (0 V)",
        "Sources",
        "ground.png",
        "Ground",
        Ground::construct );
}

Ground::Ground( std::string type, std::string id )
      : Component( type, id )
{
    //m_area = QRect(-8,-10, 16, 12 );

    m_pin.resize(1);
    IoPin* pin = new IoPin(id+"-Gnd", 0, this, source );
    pin->setOutState( false );
    m_pin[0] = pin;
    
    //setLabelPos(-16, 8, 0);
}
Ground::~Ground() {}

//void Ground::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
//{
//    Component::paint( p, option, widget );
//
//    QPen pen = p->pen();
//    pen.setWidthF( 2.5 );
//    p->setPen(pen);
//
//    p->drawLine(QLineF(-6.6,-8, 6.6,-8) );
//    p->drawLine(QLineF(-4.3,-4, 4.3,-4) );
//    p->drawLine(QLineF(-1.9, 0, 1.9, 0) );
//}
