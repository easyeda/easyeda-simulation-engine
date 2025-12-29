/***************************************************************************
 *   Copyright (C) 2025 by easyEDA                                         *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU Affero General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Affero General Public      *
 *   License along with this program. If not, see                          *
 *   <http://www.gnu.org/licenses/>.                                       *
 ***************************************************************************/

#include "wire.h"
#include "itemlibrary.h"
#include "iopin.h"
#include "simulator.h"


#define tr(str) simulideTr("Wire",str)

Component* Wire::construct( std::string type, std::string id )
{ return new Wire( type, id ); }

LibraryItem* Wire::libraryItem()
{
    return new LibraryItem(
        "Wire",
        "Meters",
        "wire.png",
        "Wire",
        Wire::construct);
}

Wire::Wire( std::string type, std::string id )
           : Component( type, id ), eResistor( id )
{

    m_switchPins = false;

    m_pin.resize( 3 );
    m_ePin[0] = m_pin[0] = new Pin(id+"-lPin", 0, this);

    m_ePin[1] = m_pin[1] = new Pin(id+"-rPin", 1, this);

    m_pin[2] = m_outPin = new IoPin(id+"-outnod", 0, this, source );
    m_outPin->setOutHighV( 0 );
    m_outPin->setOutState( true );

    Simulator::self()->addToUpdateList( this );
    m_unit = "A";
    m_current = 0;
    setRes( 1e-9 );
}

Wire::~Wire(){}

void Wire::updateStep()
{
    m_current = current();
    m_outPin->setOutHighV( m_current );
    m_outPin->setOutState( true );
}

double Wire::getElementData()
{
    return current();
}
