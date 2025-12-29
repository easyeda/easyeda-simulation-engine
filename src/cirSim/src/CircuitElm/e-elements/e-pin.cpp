/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "e-pin.h"
#include "circuit.h"

ePin::ePin( std::string id, int index )
{
    m_id    = id;
    m_index = index;
    m_enode    = NULL;
    m_enodeComp = NULL;
    m_inverted = false;
}
ePin::~ePin()
{
    if( m_enode ) m_enode->remEpin( this );
}

void ePin::setEnode( eNode* enode )
{
    if( enode == m_enode ) return;

    if( m_enode ) m_enode->remEpin( this );
    if( enode ) enode->addEpin( this );

    m_enode = enode;
}

void ePin::setEnodeComp( eNode* enode )
{
    m_enodeComp = enode;
    if( m_enode && m_enodeComp )
        m_enode->addConnection( this, enode->getNodeNumber() );
}

void ePin::addSingAdm( int node, double admit )
{
    if( m_enode ) m_enode->addSingAdm( this, node, admit );
}

void ePin::stampSingAdm( double admit )
{
    if( m_enode ) m_enode->stampSingAdm( this, admit );
}

void ePin::createCurrent()
{
    if( m_enode ) m_enode->createCurrent( this );
}

void ePin::changeCallBack( eElement* el, bool cb )
{
    if( !m_enode ) return;
    if( cb ) m_enode->voltChangedCallback( el );
    else     m_enode->remFromChangedCallback( el );
}

double ePin::getVoltage()
{
    if( m_enode )     return m_enode->getVolt();
    if( m_enodeComp ) return m_enodeComp->getVolt();
    return 0;
}

void ePin::setId( std::string id )
{
    Circuit::self()->updatePin( this, m_id, id );
    m_id = id;
}
