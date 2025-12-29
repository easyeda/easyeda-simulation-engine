/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#pragma once

#include "component.h"
#include "e-resistor.h"

//机械触点
class MechContact : public Component, public eElement
{
    public:
        MechContact( std::string type, std::string id );
        ~MechContact();

        virtual int poles() { return m_numPoles; }  //返回机械触点的数量。
        virtual void setPoles( int poles );         // 设置机械触点的数量

        virtual bool dt() { return (m_numthrows>1); }  // 返回机械触点是否为双掷
        virtual void setDt( bool dt );                 //设置机械触点是否为双掷
    

        virtual bool checked(){ return m_closed;}
        virtual void setChecked( bool status);

        virtual void stamp() override;
        virtual void remove() override;                //从电路中移除该机械触点
        virtual void updateStep() override             //在每个仿真步骤中更新机械触点的状态
        { 
            if( m_changed ) 
            {
                m_changed = false; 
                //update();       //刷新界面，qt用的
            } 
        }

        void  SetupSwitches( int poles, int throws );  //设置机械触点的开关数量，极数（触点数量）和掷数（刀匝数量））。
        void  SetupButton(); //设置开关的状态（打开或关闭）

    protected:
        virtual void setSwitch( bool on );

        std::vector<eResistor*> m_switches;  //存储与机械触点相关的电阻

        bool m_closed;    //机械触点的当前状态
        bool m_ButHidden; //机械按钮是否隐藏。

        int m_numPoles;     //机械触点的极数。
        int m_numthrows;    //机械触点的掷数。

        int m_pin0;         // 机械触点的引脚编号。
};

