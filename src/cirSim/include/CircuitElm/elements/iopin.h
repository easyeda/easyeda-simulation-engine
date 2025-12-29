/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

//#include <QColor>

#include "pin.h"
#include "e-element.h"
#include "e-node.h"

enum pinMode_t{
    undef_mode=0,    //未定义模式
    input,           // 输入模式
    openCo,          // 开路模式
    output,          // 输出模式
    source           // 源模式
};

class eNode;
class asIScriptEngine;

class IoPin : public Pin, public eElement
{
        friend class Function;
    public:
        IoPin( std::string id, int index, Component* parent, pinMode_t mode=source );
        ~IoPin();

        virtual void initialize() override;    // 初始化函数
        virtual void stamp() override;         // 在导纳矩阵中填值
        virtual void updateStep() override;    // 更新步骤
        virtual void runEvent() override;      // 运行事件

        // 调度状态，指定时间
        virtual void scheduleState( bool state, uint64_t time );

        // pinMode_t pinMode() { return m_pinMode; }   // 获取引脚模式
        void setPinMode( pinMode_t mode );          // 设置引脚模式
        void setPinMode( uint32_t mode ) { setPinMode( (pinMode_t) mode ); }  // 重载设置引脚模式

        void  setInputHighV( double volt ) { m_inpHighV = volt; }   // 设置输入高电压
        void  setInputLowV( double volt ) { m_inpLowV = volt; }     // 设置输入低电压
        virtual void  setInputImp( double imp );                    // 设置输入阻抗
        void setInputAdmit( double a ) { m_admit = a; }

        double outHighV() { return m_outHighV; }                    // 获取输出高电压
        void  setOutHighV( double v ) { m_outHighV = v; }           // 设置输出高电压

        double outLowV() { return m_outLowV; }                      // 获取输出低电压
        void  setOutLowV( double v ) { m_outLowV = v; }             // 设置输出低电压
         
        // void startLH();      // 开始从低到高的过渡
        // void startHL();      // 开始从高到低的过渡

        virtual void  setOutputImp( double imp );   // 设置输出阻抗
        virtual void setOpenImp( double imp );
        virtual void  setImp( double imp );         // 设置阻抗


        virtual bool getInpState();                                                              // 获取输入状态
        virtual bool getOutState() { if( m_step ) return m_nextState; return m_outState; }       // 获取输出状态
        virtual void setOutState( bool high );                                                   //设置输出状态                            
        virtual void toggleOutState( uint64_t time=0 ) { scheduleState( !m_outState, time ); }   // 切换输出状态

        virtual double getVoltage() override;            // 获取电压，
        inline void setVoltage( double volt )            // 内联函数，设置电压
        {
            if( m_outVolt == volt ) return;
            m_outVolt = volt;
            ePin::stampCurrent( m_outVolt*m_admit );
        }
        inline void setOutStatFast( bool state )       //快速设置输出状态
        {
            m_outState = m_nextState = state;
            m_outVolt = state ? m_outHighV : m_outLowV;
            ePin::stampCurrent( m_outVolt*m_admit );
        }

        void setStateZ( bool z );            // 设置高阻状态
        virtual void setPullup( bool up );   // 设置上拉电阻

        virtual void setInverted( bool invert ) override;    // 设置反转

        virtual void controlPin( bool outCtrl , bool dirCtrl ){;}  // 控制引脚

        void skipStamp( bool s ) { m_skipStamp = s; }            //跳过填值

        void setRiseTime( double time ) { m_timeRis = time; }    // 设置上升时间
        void setFallTime( double time ) { m_timeFal = time; }    // 设置下降时间

        static void registerScript( asIScriptEngine* engine );     //脚本函数，预计用于芯片    

    protected:
        //内联函数用于更新引脚状态
        inline void updtState()
        {
            double vddAdmit = m_vddAdmit + m_vddAdmEx;
            double gndAdmit = m_gndAdmit + m_gndAdmEx;
            double current  = m_outHighV*vddAdmit;
            m_admit         = vddAdmit+gndAdmit;
            m_outVolt = current/m_admit;
            ePin::stampAdmitance( m_admit );
            ePin::stampCurrent( current );
        }
        inline void stampAll();   //更新引脚的所有状态到仿真引擎中
        inline void stampVolt( double v) { ePin::stampCurrent( v*m_admit ); }  // 函数用于将给定的电压 v 乘以总导通性 m_admit 并更新到仿真引擎中，这可能影响到连接到引脚的电路节点的电压。

        double m_inpHighV;  // currently in eClockedDevice
        double m_inpLowV;
        //存储引脚的输入高电平和低电平阈值。

        double m_outHighV;
        double m_outLowV;
        double m_outVolt;
        //存储引脚的输出高电平、低电平和当前输出电压。

        double m_vddAdmit;  //  VDD 导通性
        double m_gndAdmit;  //  GND 导通性
        double m_vddAdmEx;  // 额外的 VDD 导通性
        double m_gndAdmEx;  // 额外的GND 导通性

        double m_inputImp;
        double m_outputImp;
        double m_openImp;
        double m_admit;
        //存储引脚的输入阻抗、输出阻抗、开路阻抗和总导通性。

        bool m_inpState;
        bool m_outState;
        bool m_stateZ;
        bool m_nextState;
        bool m_skipStamp;
        // /表示引脚的输入状态、输出状态、是否处于高阻态、下一个状态以及是否跳过戳记更新。

        int m_steps;
        //存储仿真步骤的计数，用于管理时间依赖的事件

        uint64_t m_timeRis;  // Time for Output voltage to switch from 0% to 100%
        uint64_t m_timeFal;  // Time for Output voltage to switch from 100% to 0%
        //存储引脚电压从0%上升到100%和从100%下降到0%所需的时间，用于模拟引脚电压变化的时间特性

        pinMode_t m_pinMode;//存储引脚的工作模式，例如输入、输出、开路等。
        
        static eNode m_gndEnode;//表示地（GND）节点的 eNode 对象
        
};