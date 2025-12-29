/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "board.h"
#include "circuit.h"
#include "shield.h"

BoardSubc::BoardSubc( std::string type, std::string id )
         : SubCircuit( type, id )
{
    //m_graphical = true;
    m_subcType = Chip::Board;
    //m_shield = NULL;
}
BoardSubc::~BoardSubc(){}

void BoardSubc::attachShield( ShieldSubc* shield )
{
    if( std::find(m_shields.begin(),m_shields.end(),shield) == m_shields.end())
        m_shields.push_back( shield );
}

void BoardSubc::remove()
{
    for( ShieldSubc* shield : m_shields ) // there is a shield attached to this
    {
        shield->setBoard( NULL );
        Circuit::self()->removeComp( shield );
    }
    SubCircuit::remove();
}
