/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "comp2pin.h"
#include "pin.h"

Comp2Pin::Comp2Pin( std::string type, std::string id )
        : Component( type, id )
{
    //m_area = QRectF(-11,-4.5, 22, 9 );

    m_pin.resize(2);
    m_pin[0] = new Pin(id+"-lPin", 0, this);
    m_pin[1] = new Pin(id+"-rPin", 1, this);
    
    //setValLabelPos(-16, 6, 0 );
    //setLabelPos(-16,-24, 0 );
}
