/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "node.h"
#include "connector.h"
#include "circuit.h"

#include "doubleProp.h"
//#include "pointprop.h"
#include "pin.h"

Node::Node( std::string type, std::string id ,uint16_t n)
    : Component( type, id )
{

    m_isBus = false;
    num = n;
    m_pin.resize(num);
    for( int i=0; i<num; i++ )
    {
        m_pin[i] = new Pin(id+"-"+ std::to_string(i), i, this );
    }
}
Node::~Node(){}


void Node::pinMessage( int rem ) // Called by pin
{
    if     ( rem == 1 ) checkRemove();
    else if( rem == 2 ) // Propagate Is Bus
    {
        for( int i=0; i<num; i++) m_pin[i]->setIsBus( true );
        m_isBus = true;
}   }

void Node::registerEnode( eNode* enode, int n )
{
    for( int i=0; i<num; i++ )
        if( m_pin[i]->conPin() ) m_pin[i]->registerPinsW( enode, n );
}

//是否可以移除一个节点
bool Node::checkRemove() // Only remove if there are less than 3 connectors
{
    int con[2] = { 0, 0 };     //---------------------------------------------------------------------------------------------------连接器相关，需要重写
    int conectors = 0;
    int conecteds = 0;

    for( int i=0; i<num; i++ )
    {
        Connector* co = m_pin[i]->connector();
        if( co )
        {
            Pin* coPin = m_pin[i]->conPin();
            if( coPin->component() == this ) // Connector betwen 2 Pins of this node
            {
                co->setStartPin( NULL );
                co->setEndPin( NULL );
                Circuit::self()->removeConnector( co );
                continue;
            }
            if( conecteds == 0 ) { conecteds++; con[0] = i; }
            else con[1] = i;
            conectors++;
        }   
    }
    if( conectors < 3 )
    {
        if( conectors == 2) joinConns( con[0], con[1] );  // 2 Conn
        else                m_pin[con[0]]->removeConnector();

        Circuit::self()->removeNode( this );
        return true;
    }
    else return false;
    return false;   //-------------------------------------------------------------------------------------------------------仅为了补充程序完整性
}


//将两个连接器（Connector）合并为一个。
void Node::joinConns( int c0, int c1 )
{
    Pin* pin0 = m_pin[c0];           //----------------------------------------------------------------------------------------------连接器相关需要重构
    Pin* pin1 = m_pin[c1];

    Connector* con0 = pin0->connector();
    Connector* con1 = pin1->connector();
    if( !con0 || !con1 ) return;

    if( pin1->conPin() != pin0 )
    {
        Connector* con = new Connector( "Connector", "Connector-"+Circuit::self()->newConnectorId(), pin0->conPin() );
        Circuit::self()->conList()->insert( con );

        //QStringList list0 = con0->pointList();
        //QStringList list1 = con1->pointList();
        //QStringList plist;

        //if( pin0 == con0->startPin() ){
        //    while( !list0.isEmpty() )
        //    {
        //        std::string p2 = list0.takeLast();
        //        plist.append(list0.takeLast());
        //        plist.append(p2);
        //}   }
        //else while( !list0.isEmpty() ) plist.append( list0.takeFirst() );

        //if( pin1 == con1->endPin() ){
        //    while( !list1.isEmpty() )
        //    {
        //        std::string p2 = list1.takeLast();
        //        plist.append(list1.takeLast());
        //        plist.append(p2);
        //}   }
        //else while( !list1.isEmpty() ) plist.append( list1.takeFirst() );

        ////con->setPointList( plist );
        //con->closeCon( pin1->conPin() );
        //if( this->isSelected() ) con->setSelected( true );
    }
    con0->setStartPin( NULL );
    con0->setEndPin( NULL );
    Circuit::self()->removeConnector( con0 );

    con1->setStartPin( NULL );
    con1->setEndPin( NULL );
    Circuit::self()->removeConnector( con1 );
}