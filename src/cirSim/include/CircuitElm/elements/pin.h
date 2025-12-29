/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "updatable.h"
#include "e-pin.h"

enum pinState_t{
    undef_state=0,
    driven_low,    // State is open high, but driven low externally
    open_low,
    open_high,
    out_low,
    out_high,
    input_low,
    input_high
};

class Connector;
//class laChannel;

class Pin :public ePin, public Updatable
{
    public:
        Pin(std::string id, int index, Component* parent = 0);
        ~Pin();

        enum pinType_t
        {
            pinNormal=0,     //普通类型的引脚
            pinSocket,       //插座类型的引脚
            pinHeader,       //排针类型的引脚
        };

        //enum { Type = UserType + 3 };
        //int type() const override { return Type; }



        std::string pinId() { return m_id; }
        
        bool unused() { return m_unused; }
        void setUnused( bool unused );

        
        //返回引脚所属组件
        Component* component() { return m_component; }

        //返回引脚相连的连接器
        Connector* connector() { return my_connector; }
        void setConnector( Connector* c );
        void removeConnector();  //移除与引脚相连的连接器

        //设置与引脚相连的另一个引脚。
        void setConPin( Pin* pin ) { m_conPin = pin; }
        //返回与引脚相连的另一个引脚。
        Pin* conPin(){ return m_conPin; }

        Pin* connectPin( bool connect );

        //返回引脚的标签文本。
        std::string getLabelText() { return m_labelText; }
        // virtual void setLabelText( std::string label, bool over=true );



        //设置引脚的标识符。
        void setPinId( std::string id ) { m_id = id; }


        //处理连接器被移除的情况。
        void connectorRemoved();

        //将引脚注册到节点。
        void registerEnode( eNode* enode, int n=-1 );
        //用于将一个引脚注册到一个节点。
        void registerPinsW( eNode* enode, int n=-1 );
        
        //设置引脚是否为总线。
        void setIsBus( bool bus );
        //返回引脚是否为总线。
        bool isBus() { return m_isBus; }

        //设置引脚的类型。
        void setPinType( pinType_t ty ) { m_pinType = ty; }
        //返回引脚的类型。
        pinType_t pinType() { return m_pinType; }

        //设置引脚连接到的逻辑分析器通道。
        //void setDataChannel( laChannel* ch ) { m_dataCannel = ch; }

        inline void setPinState( pinState_t st ) { m_pinState = st; /*m_PinChanged = true;*/ }

        //设置警告状态。
        void warning( bool w );

        //在仿真的每个步骤中更新引脚的状态。
        virtual void updateStep() override;

        //返回指向当前 Pin 对象的指针。
        virtual Pin* getPin() override { return this; }



    protected:

        pinType_t  m_pinType;  //表示引脚的类型。
        pinState_t m_pinState; //表示引脚的状态。

        int m_overScore;


        bool m_blocked;        //表示引脚是否被阻塞。
        bool m_isBus;          //表示引脚是否为总线。
        bool m_unused;         //表示引脚是否未使用。
        bool m_warning;        //表示引脚是否处于警告状态。

        double  m_opCount;     //双精度浮点变量，表示操作计数。

        std::string m_labelText;   //表示引脚的标签文本。
        

        Connector* my_connector;   //指向与引脚相连的连接器。
        Component* m_component;     // 指向引脚所属的组件。
        //laChannel* m_dataCannel;    // connect to Logic Analyzer  指向引脚连接到的逻辑分析器通道。
        Pin*       m_conPin;        // Pin at the other side of connector  指向与引脚相连的另一个引脚。

};

