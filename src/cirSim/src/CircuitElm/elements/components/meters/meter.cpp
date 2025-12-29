/***************************************************************************
 *   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
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

#include "meter.h"
#include "simulator.h"
#include "intProp.h"



Meter::Meter(std::string type ,std::string id)
     :Component(type,id)
{
    m_type = type;
    m_id = id;
    m_ctr = 0;
    m_speed = 1;
    m_length = 100000;
    m_cirSim =  Simulator::self();


    addPropGroup({ "Main", {
new intProp<Meter>("Speed"  , "Speed"      ," " , this, &Meter::speed, &Meter::setSpeed,propNoCopy,"uint" ),
new intProp<Meter>("Ctr" , "Ctr"     ," " , this, &Meter::ctr, &Meter::setCtr,propNoCopy,"uint" ),
new intProp<Meter>("Vcrit", "Vcrit"," ", this, &Meter::length, &Meter::setLength,propNoCopy,"uint" ),
    },groupNoCopy });

    
    if( Simulator::self() ) Simulator::self()->addMeter( this );

}
Meter::~Meter()
{
    if( Simulator::self() ) Simulator::self()->remFromMeter( this );
}
