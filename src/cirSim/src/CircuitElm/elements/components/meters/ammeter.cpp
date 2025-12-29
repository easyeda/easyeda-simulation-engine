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

#include  "ammeter.h"
#include "itemlibrary.h"
#include "simulator.h"

Component *Ammeter::construct(std::string type, std::string id)
{
    return new Ammeter(type,id);
}

LibraryItem *Ammeter::libraryItem()
{
       return new LibraryItem(
        "Ammeter",
        "Meters",
        " ",
        "Ammeter",
        Ammeter::construct);
}

Ammeter::Ammeter(std::string type,std::string id)
       :Meter(type,id)
{
    currentsData.clear();
}

Ammeter::~Ammeter()
{
    for (auto& [id, ptr] : currentsData) {
        delete ptr;
    }
    currentsData.clear();
}


void Ammeter::timeStep(uint64_t currentTime)
{
    //非电流模式，不进入数据提取
    if(!Simulator::self()->getCurrentModel()) return;

    m_ctr++;
    if (m_ctr >= m_speed) 
    {
        for (auto Element: m_cirSim->getElementList())
        {
            const std::string Id = Element->getId();
            if ((Id.find("Wire") != std::string::npos) && !(Id.find("outnod") != std::string::npos))
            {
                auto it = currentsData.find(Id);
                if(it == currentsData.end()){
                    currentsData[Id] = new MeterData(Id, m_length);
                }                
                const double elementData = Element->getElementData();
                currentsData[Id]->datas.push(std::make_pair(currentTime,elementData));
            }
        }
        m_ctr = 1;
    }
}

void Ammeter::viewDate()
{
}

std::unordered_map<std::string,MeterData*>  Ammeter::getDataImpl()
{
    return currentsData;
}

