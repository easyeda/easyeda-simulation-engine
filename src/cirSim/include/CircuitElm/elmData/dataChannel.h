/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-element.h"
#include "updatable.h"

#include <string>
#include <vector>

//用于描述信号的电平状态或边沿事件。
enum cond_t 
{
    C_NONE = 0,
    C_LOW,
    C_RISING,
    C_HIGH,
    C_FALLING
};

class plotBase;
class Pin;

//数据通道
class dataChannel:public eElement, public Updatable
{
    friend class plotBase;
    friend class Oscope;
    friend class LAnalizer;
    friend class PlotDisplay;

public:

    dataChannel(plotBase* plotBase, std::string& id);
    ~dataChannel();

    //用于在仿真过程中更新数据通道的状态。
    virtual void stamp() override;

    //用于设置数据通道的过滤器参数。
    virtual void setFilter(double f) { ; }

    //用于检查数据通道是否表示一个总线。
    bool isBus();
    //用于设置数据通道是否表示一个总线。
    virtual void setIsBus(bool b) { ; }

    //用于获取数据通道的名称。
    std::string getChName() { return m_chTunnel; }

protected:
    std::vector<double> m_buffer;   //用于存储数据通道的数据缓冲区。
    std::vector<uint64_t> m_time;   //用于存储与数据缓冲区中的数据对应的时间戳。

    bool m_connected;      //表示数据通道是否已连接。
    bool m_rising;         //表示数据通道是否在上升沿触发。
    bool m_falling;        //表示数据通道是否在下降沿触发。
    bool m_trigger;        //表示数据通道是否作为触发源。
    int m_trigIndex;       //表示触发事件在数据缓冲区中的索引。

    uint64_t m_risEdge;    //表示上升沿的时间戳。
    uint64_t m_period;     //表示数据通道的周期。

    int m_channel;         //表示数据通道的通道号。
    int m_bufferCounter;   //表示数据缓冲区的计数器。

    bool m_pauseOnCond;    //表示是否在特定条件下暂停仿真。
    //cond_t m_cond;
    cond_t m_lastCond;     //表示数据通道上次的条件状态。

    std::string m_chTunnel;  //表示数据通道的名称。

    Pin* m_pin;              //指向与数据通道关联的引脚。

    plotBase* m_plotBase;    //指向数据通道所属的 PlotBase 对象。

};

