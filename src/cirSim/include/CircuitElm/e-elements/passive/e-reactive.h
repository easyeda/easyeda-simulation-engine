/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "e-resistor.h"


//反应性（reactive）元件，如电容器或电感器。
class eReactive : public eResistor
{
    public:
        eReactive( std::string id );
        ~eReactive();

        virtual void stamp() override;
        // virtual void voltChanged() override;
        virtual void runEvent() override;

        //获取初始电压
        double initVolt() { return m_InitVolt; }
        //设置初始电压
        void setInitVolt( double v ) { m_InitVolt = v; }

        //获取初始电流
        double initCurr() { return -m_InitCurr; }
        //设置初始电流
        void setInitCurr( double c ) { m_InitCurr = -c; }

    protected:
        // 更新反应步长。
        void updtReactStep();
        
        //更新电流
        virtual double updtRes(){ return 0.0;}
        //更新电阻
        virtual double updtCurr(){ return 0.0;}

        double m_value; // Capacitance or Inductance，反应性元件的值

        double m_InitCurr;  //初始电流
        double m_curSource; //当前电流源值

        double m_InitVolt;  //初始电压值
        double m_volt;      //当前电压值

        double m_tStep;     //时间步长

        uint64_t m_timeStep;
};


