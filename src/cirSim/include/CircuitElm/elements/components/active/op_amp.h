/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

class LibraryItem;
class IoPin;

class OpAmp : public Component, public eElement
{
    public:
        OpAmp( std::string type, std::string id );
        ~OpAmp();
        
        // 静态方法，用于构建 OpAmp 对象
        static Component* construct( std::string type, std::string id );
        // 静态方法，用于获取库项目
        static LibraryItem* libraryItem();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        // 获取和设置增益
        double gain() { return m_gain; }
        void setGain( double g ) { m_gain = g; m_changed = true; }

        // 获取和设置输出阻抗
        double outImp() { return m_outImp; }
        void setOutImp( double imp );

        // 获取和设置正输入电压
        double voltPos() { return m_voltPosDef; }
        void setVoltPos( double v ) { m_voltPosDef = v; m_changed = true; }

        // 获取和设置负输入电压
        double voltNeg() { return m_voltNegDef; }
        void setVoltNeg( double v ) { m_voltNegDef = v; m_changed = true; }

        // 获取和设置电源引脚状态
        bool powerPins() {return m_powerPins; }
        void setPowerPins( bool set );

        // 获取和设置引脚切换状态
        bool switchPins() { return m_switchPins; }
        void setSwitchPins( bool s );


    protected:
        // 更新属性方法,图形界面
        void udtProperties();

        bool m_powerPins;   // 电源引脚状态
        bool m_switchPins;  // 引脚切换状态

        double m_accuracy;  // 精度
        double m_gain;      // 增益
        double m_k;         // 一个常数，具体作用视实现而定
        double m_voltPos;   // 正输入电压
        double m_voltNeg;   // 负输入电压
        double m_voltPosDef;// 默认正输入电压
        double m_voltNegDef;// 默认负输入电压
        double m_lastOut;   // 上次输出电压
        double m_lastIn;    // 上次输入电压
        double m_outImp;    // 输出阻抗

        IoPin* m_inputP;    // 正输入引脚
        IoPin* m_inputN;    // 负输入引脚
        IoPin* m_output;    // 输出引脚
};

