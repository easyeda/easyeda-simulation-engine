/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */
#pragma once

#include "pch.h"
#include "e-node.h"

class eElement;
class Pin;

class ePin
{
    public:
        ePin( std::string id, int index );
        virtual ~ePin();
        
        //表示引脚是否已连接到一个节点
        bool isConnected() { return (m_enode!=NULL); }

        //获取引脚的电压。
        virtual double getVoltage();

        //返回一个指向引脚所连接的节点 (eNode) 的指针。
        eNode* getEnode() { return m_enode; }
        //设置引脚所连接的节点 (eNode)。
        void   setEnode( eNode* enode );
        //设置元器件另一侧的节点
        void   setEnodeComp( eNode* enode ); // The enode at other side of component  

        //添加或移除电压变化时的回调函数。
        void changeCallBack( eElement* el , bool cb=true );

        //指示引脚是否被标记为反向
        bool inverted() { return m_inverted; }
        //设置引脚是否被标记为反向。
        virtual void setInverted( bool inverted ) { m_inverted = inverted; }

        //在节点上填入导纳值。
        inline void stampAdmitance( double data ) { if( m_enode ) m_enode->stampAdmitance( this, data ); }

        //添加单独的导纳值。
        void addSingAdm( int node, double admit );
        //填入单独的导纳值。
        void stampSingAdm( double admit );

        //创建电流。
        void createCurrent();
        //用于在节点上填入电流值。
        inline void stampCurrent( double data ) { if( m_enode ) m_enode->stampCurrent( this, data ); }
        
        //返回引脚的标识符。
        std::string getId()  { return m_id; }
        //设置引脚的标识符。
        void setId( std::string id );

        //返回一个指向实际的 Pin 对象的指针
        virtual Pin* getPin(){ return nullptr; }

        //设置引脚的索引。
        void setIndex( int i ) { m_index = i; }

    protected:
        eNode* m_enode;     // My eNode  指向引脚所连接的节点 (eNode) 的指针。
        eNode* m_enodeComp; // eNode at other side of my component  元器件另一侧引脚连接的节点

        std::string m_id;   //引脚的唯一标识符。
        int m_index;        //引脚的索引

        bool m_inverted;    //引脚是否被标记为反向。
};

