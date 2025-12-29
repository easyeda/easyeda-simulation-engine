/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

class LibraryItem;
class eNode;

//总线
class Bus : public Component, public eElement
{
    public:
        Bus( std::string type, std::string id );
        ~Bus();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();
        
        int numLines() { return m_numLines; }
        //设置总线线数
        void setNumLines( int lines );

        int startBit() { return m_startBit; }
        void setStartBit( int bit );

        virtual void registerEnode( eNode* enode, int n=-1 ) override;
        
    protected:
        int m_height;

        int m_numLines;
        int m_startBit;
        
        Pin* m_busPin0;
        Pin* m_busPin1;
};


