/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "module.h"
#include "simulator.h"
//#include "circuitwidget.h"
#include "circuit.h"
#include "tunnel.h"

#include "doubleProp.h"

//#define tr(str) simulideTr("ModuleSubc",str)

ModuleSubc::ModuleSubc( std::string type, std::string id )
          : ShieldSubc( type, id )
{
    m_subcType = Chip::Module;
    //setZValue( 2 );

    addPropGroup( { "Main", {
new doubleProp<ModuleSubc>( "Z_Value", "Z Value","", this, &ModuleSubc::zVal, &ModuleSubc::setZVal )
    }} );
}
ModuleSubc::~ModuleSubc(){}

void ModuleSubc::setZVal( double v )
{
    if     ( v < 2 ) v = 2;
    else if( v > 9 ) v = 9;
    //setZValue( v );
}

//void ModuleSubc::attachToBoard()
//{
//    m_boardPos = m_circPos - m_board->pos();
//}

void ModuleSubc::renameTunnels()
{
    for( Tunnel* tunnel : getPinTunnels() ) tunnel->setName( m_id+"-"+tunnel->tunnelUid() );
    for( Tunnel* tunnel : m_subcTunnels ) tunnel->setName( m_id+"-"+tunnel->tunnelUid() );
}
