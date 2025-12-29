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

#pragma once

#include "pch.h"
#include "component.h"
#include "circularBuffer.h"

class Simulator;

struct MeterData
{
    std::string id;
    CircularBuffer<std::pair<uint64_t, double>> datas;
    // 复制构造函数
    MeterData(const MeterData& other) : id(other.id), datas(other.datas.maxSize()) {
        auto dataVec = other.datas.getAll();
        for(const auto& item : dataVec) {
            datas.push(item);
        }
    }

    MeterData(const std::string& id, size_t bufferSize = 1000)
        : id(id), datas(bufferSize) {}
};

class Meter: public Component
{
protected:
    std::string m_type;               //仪表类型
    std::string m_id;                 //仪表标识
    int32_t m_speed;                  //采样速度
    int32_t m_ctr;                    //计数器
    int32_t m_length;                 //采样点个数
    std::mutex dataMutex;             // 添加的互斥锁

    Simulator* m_cirSim;              // Pointer to Cirsim instance



public:
    Meter(std::string type ,std::string id);
    ~Meter();
    virtual void timeStep(uint64_t currentTime) = 0;
    virtual void viewDate() = 0;
    virtual std::unordered_map<std::string,MeterData*> getData() {
        std::lock_guard<std::mutex> lock(dataMutex);
        return getDataImpl();
    }
    virtual std::unordered_map<std::string,MeterData*> getDataImpl() = 0;

    virtual int32_t speed()  { return m_speed; }
    virtual void setSpeed(int32_t speed) { m_speed = speed; }

    virtual int32_t ctr() { return m_ctr; }
    virtual void setCtr(int32_t ctr) { m_ctr = ctr; }

    virtual int32_t length() { return m_length; }
    virtual void setLength(int32_t length) { m_speed = length; }
    std::string getType() const { return m_type; }
    virtual std::string getId() const { return m_id; }
};



