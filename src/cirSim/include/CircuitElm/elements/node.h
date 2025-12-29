/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "pin.h"

class Node : public Component
{
    public:
        Node( std::string type, std::string id, uint16_t n = 3);
        ~Node();

        //返回节点引脚
        Pin* getPin( int pin ) const { return m_pin[pin]; }

        //将节点注册到一个电子节点（eNode）。
        virtual void registerEnode( eNode* enode, int n=-1 ) override;

        void pinMessage( int rem=1 ) override;
        bool checkRemove();


    private:
        void joinConns( int co0, int c1);

        uint16_t num;   //节点数
        bool m_isBus;
};

