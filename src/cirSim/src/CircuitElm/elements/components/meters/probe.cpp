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

#include "probe.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "stringProp.h"
#include "e-pin.h"


Component* Probe::construct(std::string type, std::string id)
{
    return new Probe(type, id);
}

LibraryItem* Probe::libraryItem()
{
    return new LibraryItem(
        "Probe",
        "Meters",
        " ",
        "Probe",
        Probe::construct);
}


Probe::Probe(std::string type,std::string id)
     :Meter(type,id)
{
    probes.clear();
    addPropGroup( { "Main", {
        new stringProp <Probe>("voltMes","voltMes","V", this, &Probe::voltMes,  &Probe::setVoltMes ),
    },0} );
}

Probe::~Probe()
{
    for (auto& [id, ptr] : probes) {
        delete ptr; // 删除实际指针
    }
    probes.clear(); // 清空容器
}

void Probe::timeStep(uint64_t currentTime)
{
    m_ctr++;
    if (m_ctr >= m_speed) 
    {
        for (auto nodeList: m_cirSim->getNodeList())
        {
            if (nodeList->itemId().find("eNodeSim") != std::string::npos)
            {
                auto pins = nodeList->getEpins();
                for (auto pin: pins )
                {
                    const double volt = nodeList->getVolt();
                    const std::string id = pin->getId();
                    if(probes.find(id) == probes.end()){continue;}
                    probes[id]->datas.push(std::make_pair(currentTime,volt)); // Input
                }
            }
        }
        m_ctr = 1;
    }
}

void Probe::viewDate()
{
}

std::unordered_map<std::string,MeterData*> Probe::getDataImpl()
{
    return probes;
}

//每个probe都是一个单独的实例，下面的else只是为了修改当前探针的探测节点
void Probe::setVoltMes(std::string voltMes)
{
    const std::string meterId = m_id+'-'+voltMes;
    auto it = probes.find(meterId);
    if(it == probes.end()){
        m_voltMes = voltMes;
        probes[m_voltMes] = new MeterData(meterId, m_length);
    }
}

