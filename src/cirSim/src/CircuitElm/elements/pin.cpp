/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include <math.h>


#include "pin.h"
#include "bus.h"
#include "node.h"
//#include "lachannel.h"
#include "connector.h"
//#include "connectorline.h"
#include "circuit.h"
#include "simulator.h"

Pin::Pin(std::string id, int index, Component* parent)
   :// QGraphicsItem( parent )
     ePin( id, index )
   //, m_label( parent )
{
    //m_area = QRect(-3, -3, 11, 6);

    m_component = parent;
    m_pinState = undef_state;
    m_pinType  = pinNormal;

    m_blocked = false;
    m_isBus   = false;
    m_unused  = false;
    //m_animate = false;
    m_warning = false;
    
    //m_dataCannel = NULL;
    my_connector = NULL;
    m_conPin     = NULL;

    m_overScore = -1;
    Circuit::self()->addPin( this, id );

    m_component->addSignalPin( this );
}
Pin::~Pin()
{
    Circuit::self()->remPin( m_id );
}

void Pin::connectorRemoved()
{
    setConnector( NULL );
    //if( !Circuit::self()->undoRedo() ) m_component->pinMessage( 1 ); // Used by node to remove   undoredo用于检查是否有撤销重做动作
     m_component->remSignalPin( this );      //after conn removed it can't auto-connect again
}

void Pin::setUnused( bool unused )
{
    m_unused = unused;
    //if( unused ) setCursor( Qt::ArrowCursor );
    //else         setCursor( Qt::CrossCursor );
    //update();
}

void Pin::registerPinsW( eNode* enode, int n )     // Called by component, calls conPin
{
    if( m_blocked ) return;   //该引脚已经被处理过，直接返回以避免重复注册。
    m_blocked = true;

    //调用基类 ePin 的 setEnode 方法将该引脚注册到指定的电路节点 enode。
    if( !m_isBus ) ePin::setEnode( enode );    

    //连接到的引脚（m_conPin 不为空），则调用该连接引脚的 registerEnode 方法，将其也注册到相同的电路节点 enode。
    if( m_conPin ) m_conPin->registerEnode( enode, n ); // Call pin at other side of Connector   

    m_blocked = false;
}

void Pin::registerEnode( eNode* enode, int n )     // Called by m_conPin
{
    if( m_blocked ) return;  //m_blocked 成员变量为 true，表示该引脚已经被处理过，直接返回以避免重复注册。
    m_blocked = true;

    //如果该引脚不是总线类型的引脚（m_isBus 为 false），则调用基类 ePin 的 setEnode 方法将该引脚注册到指定的电路节点 enode，
    // 并将 n 设置为该引脚的索引 m_index。
    if( !m_isBus )
    {
        ePin::setEnode( enode );
        n = m_index;
    }
    //调用该引脚所属组件（节点/总线）的 registerEnode 方法，将组件的其他引脚也注册到相同的电路节点 enode
    m_component->registerEnode( enode, n );
    //if( m_dataCannel ) m_dataCannel->registerEnode( enode, n );   ----------------------将数据通道也注册到节点

    m_blocked = false;
}

void  Pin::setConnector( Connector* connector )
{
    my_connector = connector;
    m_warning = false;

    if( my_connector )
    {
        //setCursor( Qt::ArrowCursor );
        if( m_isBus ) my_connector->setIsBus( true );
    }else{
        m_conPin = NULL;
        //setCursor( Qt::CrossCursor );
    }
}

void Pin::removeConnector()
{
    if( my_connector ) Circuit::self()->removeConnector( my_connector );
    setConnector( NULL );
}

Pin* Pin::connectPin( bool connect )      // Auto-Connect
{
    //Pin* _pin = NULL;
    //QList<QGraphicsItem*> list = this->collidingItems();
    //while( !list.isEmpty() )
    //{
    //    QGraphicsItem* it = list.takeLast();
    //    if( it->type() == 65536+3 )          // Pin found
    //    {
    //        Pin* pin =  qgraphicsitem_cast<Pin*>( it );

    //        if( pin->parentItem() == this->parentItem() ) continue;
    //        if( fabs(scenePos().x()-pin->scenePos().x()) > 3 ) continue;
    //        if( fabs(scenePos().y()-pin->scenePos().y()) > 3 ) continue;
    //        if( m_isBus != pin->isBus() ) continue; // Only connect Bus to Bus
    //        if( pin->connector() ) continue;
    //        if( pin->unused() ) continue;
    //        if( !connect && pin->pinType() < pinSocket
    //          && pin->isVisible() && !pin->isObscuredBy( m_component ) ) _pin = pin;
    //        if( connect )
    //        {
    //            Circuit::self()->newconnector( this, true );
    //            Circuit::self()->closeconnector( pin, true );
    //        }
    //        break;
    //    }
    //    else if( connect && (it->type() == UserType+2) ) // ConnectorLine
    //    {
    //        ConnectorLine* line =  qgraphicsitem_cast<ConnectorLine*>( it );
    //        if( m_isBus != line->connector()->isBus() ) continue;
    //        Circuit::self()->newconnector( this );
    //        line->connectToWire( QPoint( scenePos().x(), scenePos().y()) );
    //        break;
    //    }
    //}
    //return _pin;
    return nullptr;
}

//void Pin::isMoved()
//{
//    if( my_connector ) my_connector->updateConRoute( this, scenePos() );
//    else if( this->isVisible() && !my_connector )
//    {                                       // Auto-Connect
//        if( !Circuit::self()->isBusy()
//         && (QApplication::queryKeyboardModifiers() == Qt::ShiftModifier) )
//            connectPin( true );
//    }
//    setLabelPos();
//}



void Pin::setIsBus( bool bus )
{
    if( m_isBus == bus ) return;
    /// if( !bus ) return;          // Why?
    m_isBus = bus;
    
    if( my_connector ) my_connector->setIsBus( true );
    if( m_conPin ) m_conPin->setIsBus( true );
    
    m_component->pinMessage( 2 );         // Propagate Is Bus (Node)

    //update();
}

//void Pin::setVisible( bool visible )
//{
//    m_label.setVisible( visible );
//    QGraphicsItem::setVisible( visible );
//}

void Pin::warning( bool w )
{
    m_warning = w;
    if     (  m_warning ) Simulator::self()->addToUpdateList( this );
    //else if( !m_animate ) Simulator::self()->remFromUpdateList( this );
    //update();
}


void Pin::updateStep()
{
    if( m_unused ) return;
    //if( m_PinChanged )
    //    update();
}

