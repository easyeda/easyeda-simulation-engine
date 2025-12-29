/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "subcircuit.h"

class ShieldSubc;

//子电路板
class BoardSubc : public SubCircuit
{
    public:
        BoardSubc( std::string type, std::string id );
        ~BoardSubc();

        void attachShield( ShieldSubc* shield );
        void detachShield( ShieldSubc* shield ) 
        {
            // 使用 std::remove 移除指定的 shield，然后调用 erase 移除容器中的 "空" 元素
            m_shields.erase(std::remove(m_shields.begin(), m_shields.end(), shield), m_shields.end());
        }

        virtual void remove() override;

    protected:
        std::vector<ShieldSubc*> m_shields; // A shield attached to this

};
