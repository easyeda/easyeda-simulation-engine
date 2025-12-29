/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"

class ePin;

class eElement
{
    public:
        eElement(const std::string& id );
        virtual ~eElement();

        //初始化元件状态
        virtual void initialize(){;}
        //填入矩阵
        virtual void stamp(){;}

        //处理元器件事件
        virtual void runEvent(){;}
        //元件电压变化触发
        virtual void voltChanged(){;}

        //设置元件引脚数量
        virtual void setNumEpins( int n );

        //获取指定编号引脚
        virtual ePin* getEpin( int num );
        //设置指定编号引脚
        virtual void setEpin( int num, ePin* pin );

        //获取电子元件数据
        virtual double  getElementData() {;}
        virtual std::string  getElementStatusData() {;}

        //获取元件标识符
        std::string getId(){ return m_elmId; }

        //处理暂停元件事件
        void pauseEvents();
        //恢复元件处理事件
        void resumeEvents();

        static constexpr double cero_doub = 1e-9;   //用于浮点数比较
        static constexpr double high_imp  = 1e14;    //高阻抗，模拟开路

        // Simulator engine
        eElement* nextChanged;   //表示下一个状态发生变化的指针
        bool added;              //表示元件是否已添加到仿真引擎列表中

        eElement* nextEvent;    //指向下一个有事件的元件指针
        uint64_t eventTime;     //表示下一个事件的时间戳

    protected:
        uint64_t m_pendingTime;   //表示元件的待处理时间

        std::vector<ePin*> m_ePin; //储存元件引脚向量

        std::string m_elmId;      //元件唯一标识符号

        bool m_changed;           //表示元件状态是否发生了变化
        double m_step;            //仿真时间步长

};

