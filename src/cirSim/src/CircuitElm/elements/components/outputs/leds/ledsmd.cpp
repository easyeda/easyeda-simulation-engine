/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */
#include "ledsmd.h"
#include "e-pin.h"
#include "label.h"

LedSmd::LedSmd( std::string type, std::string id, ePin* pin0, ePin* pin1 )     //, QRectF area
      : LedBase( type, id )
{
    //m_area = area;
    //m_valLabel->setEnabled( false );

    if( !pin0 ) pin0 = new ePin( m_elmId+"-ePin"+std::to_string(0), 0 );
    if( !pin1 ) pin1 = new ePin( m_elmId+"-ePin"+std::to_string(1), 1 );
    setEpin( 0, pin0 );
    setEpin( 1, pin1 );
}
LedSmd::~LedSmd(){}

//void LedSmd::drawBackground( QPainter* p )
//{
//    p->drawRoundedRect( m_area, 0, 0);
//}
//
//void LedSmd::drawForeground( QPainter* p )
//{
//    p->drawRoundedRect( m_area, 0, 0 );
//}
