/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "e-node.h"
#include "pin.h"
#include "e-pin.h"
#include "connector.h"
#include "e-element.h"
#include "circmatrix.h"
#include "simulator.h"

eNode::eNode( std::string id )
{
    m_id = id;
    m_nodeNum = 0;

    m_voltChEl     = NULL;
    m_nonLinEl     = NULL;
    m_firstAdmit   = NULL;
    m_firstSingAdm = NULL;
    m_firstCurrent = NULL;
    m_nodeAdmit    = NULL;

    if( !id.empty() ) Simulator::self()->addToEnodeList( this );
}
eNode::~eNode()
{
    clearElmList( m_voltChEl );
    clearElmList( m_nonLinEl );
    clearConnList( m_firstAdmit );
    clearConnList( m_firstSingAdm );
    clearConnList( m_firstCurrent );
    clearConnList( m_nodeAdmit );
}

void eNode::initialize()
{
    m_voltChanged  = true; // Used for wire animation
    // m_switched     = false;
    m_single       = false;
    m_changed      = false;
    m_currChanged  = false;
    m_admitChanged = false;
    m_nodeGroup = -1;
    nextCH = NULL;
    m_volt = 0;

    //清除与电压变化相关的元素。
    clearElmList( m_voltChEl );
    m_voltChEl = NULL;

    //清除与非线性元素相关的元素。
    clearElmList( m_nonLinEl );
    m_nonLinEl = NULL;

    //清除与第一允许状态相关的连接。
    clearConnList( m_firstAdmit );
    m_firstAdmit = NULL;

    //清除与单一允许状态相关的连接。
    clearConnList( m_firstSingAdm );
    m_firstSingAdm = NULL;

    //清除与电流相关的连接。
    clearConnList( m_firstCurrent );
    m_firstCurrent = NULL;

    //清除与节点允许状态相关的连接。
    clearConnList( m_nodeAdmit );
    m_nodeAdmit = NULL;

    //清除所有节点列表中的元素。
    m_nodeList.clear();
}

void eNode::addConnection( ePin* epin, int node )
{
    if( node == m_nodeNum ) return;// 消息来自当前节点，不需要处理

    Connection* first = m_firstAdmit; // 创建连接列表，将 m_firstAdmit 赋值给 first
    while( first )
    {
        // 如果连接已经在列表中，直接返回
        if( first->epin == epin ) return; // Connection already in the list
        first = first->next;
    }
    //创建一个新的 Connection 对象
    Connection* conn = new Connection( epin, node );
    // 将新的 Connection 对象添加到 m_firstAdmit 列表的开头
    conn->next = m_firstAdmit;  // Prepend
    m_firstAdmit = conn;

    //创建节点导纳（admittance）列表，将 m_nodeAdmit 赋值给 first
    first = m_nodeAdmit;            // Create list of admitances to nodes (reusing Connection class)
    while( first )
    {
        // 如果节点已经在列表中，直接返回
        if( first->node == node ) return; // Node already in the list
        first = first->next;
    }
    
    // 创建另一个新的 Connection 对象（复用 Connection 类）
    conn = new Connection( epin, node );
    //将新的 Connection 对象添加到 m_nodeAdmit 列表的开头
    conn->next = m_nodeAdmit;  // Prepend
    m_nodeAdmit = conn;

    //检查 m_nodeList 是否已经包含该节点
    if (std::find(m_nodeList.begin(), m_nodeList.end(), node) == m_nodeList.end())
    {
        // 如果没有找到，添加到 m_nodeList
        m_nodeList.push_back(node);
    }
}

void eNode::stampAdmitance( ePin* epin, double admit ) // Be sure msg doesn't come from this node
{
    Connection* conn = m_firstAdmit;
    while( conn )
    {
        if( conn->epin == epin ) { conn->value = admit; break; } // Connection found
        conn = conn->next;
    }

    // if( admit == 0 ) m_switched = true;
    m_admitChanged = true;
    changed();
}

void eNode::addSingAdm( ePin* epin, int node, double admit )
{
    Connection* conn = new Connection( epin, node );
    conn->next = m_firstSingAdm;  // Prepend
    m_firstSingAdm = conn;
    conn->value = admit;

    conn = NULL;
    Connection* first = m_nodeAdmit;   // Create list of admitances to nodes (reusing Connection class)
    while( first ){
        if( first->node == node )  { conn = first; break; }// Node already in the list
        first = first->next;
    }
    if( !conn ){
        conn = new Connection( epin, node );
        conn->next = m_nodeAdmit;  // Prepend
        m_nodeAdmit = conn;
    }
    //if( !m_nodeList.contains( node ) ) m_nodeList.append( node ); // Used by CircMatrix
    if (std::find(m_nodeList.begin(), m_nodeList.end(), node) == m_nodeList.end()) 
    {
        // 如果没有找到，添加到 m_nodeList
        m_nodeList.push_back(node);
    }
    m_admitChanged = true;
    changed();
}

void eNode::stampSingAdm( ePin* epin, double admit )
{
    Connection* conn = m_firstSingAdm;
    while( conn )
    {
        if( conn->epin == epin ) { conn->value = admit; break; } // Connection found
        conn = conn->next;
    }
    /// if( admit == 0 ) m_switched = true;
    m_admitChanged = true;
    changed();
}

void eNode::createCurrent( ePin* epin )
{
    Connection* first = m_firstCurrent;

    while( first )
    {
        if( first->epin == epin ) return; // Element already in the list
        first = first->next;
    }
    Connection* conn = new Connection( epin );
    conn->next = m_firstCurrent;  // Prepend
    m_firstCurrent = conn;
}

void eNode::stampCurrent( ePin* epin, double current ) // Be sure msg doesn't come from this node
{
    Connection* conn = m_firstCurrent;
    while( conn ){
        if( conn->epin == epin ) { conn->value = current; break; } // Connection found
        conn = conn->next;
    }
    m_currChanged = true;
    changed();
}

void eNode::changed()
{
    if( m_changed ) return;
    m_changed = true;
    Simulator::self()->addToChangedNodes( this );
}

void eNode::stampMatrix()
{
    if( m_nodeNum < 0 ) return;
    m_changed = false;

    if( m_admitChanged )
    {
        m_totalAdmit = 0;
        // if( m_switched ) m_totalAdmit += 1e-12; // Weak connection to ground

        if( m_single )
        {
            Connection* conn = m_firstAdmit;
            while( conn ){ m_totalAdmit += conn->value; conn = conn->next; } // Calculate total admitance
        }
        else
        {
            Connection* na = m_nodeAdmit;
            //清空节点的所有导纳值
            while( na )
            { na->value = 0; na = na->next; } // Clear nodeAdmit

            Connection* conn = m_firstAdmit; // Full Admitances
            while( conn )
            {
                double adm = conn->value;
                int  enode = conn->node;

                if( enode >= 0 )
                {        // Calculate admitances to nodes
                    na = m_nodeAdmit;
                    while( na )
                    {
                        if( na->node == enode ){ na->value += adm; break; }
                        na = na->next;
                    }
                }
                m_totalAdmit += adm;    // Calculate total admitance
                conn = conn->next;
            }
            CircMatrix::self()->stampDiagonal( m_nodeGroup, m_nodeNum, m_totalAdmit ); // Stamp diagonal

            conn = m_firstSingAdm;      // Single admitance values
            while( conn )
            {
                double adm = conn->value;
                int  enode = conn->node;

                if( enode > 0 )
                {        // Add sinle admitance to node
                    na = m_nodeAdmit;
                    while( na )
                    {
                        if( na->node == enode ){ na->value += adm; break; }
                        na = na->next;
                    }
                }
                conn = conn->next;
            }
            na = m_nodeAdmit;
            while( na )
            {                  // Stamp non diagonal
                int    enode = na->node;
                double admit = na->value;
                if( enode > 0 ) CircMatrix::self()->stampMatrix( m_nodeNum, enode, -admit );
                na = na->next;
            }
        }
        m_admitChanged = false;
    }
    if( m_currChanged )
    {
        m_totalCurr  = 0;

        Connection* conn = m_firstCurrent;
        while( conn )
        { m_totalCurr += conn->value; conn = conn->next; } // Calculate total current

        if( !m_single ) CircMatrix::self()->stampCoef(  m_nodeGroup, m_nodeNum, m_totalCurr );
        m_currChanged  = false;
    }
    if( m_single ) solveSingle();
}

void eNode::solveSingle()
{
    double volt = 0;

    if( m_totalAdmit > 0 ) volt = m_totalCurr/m_totalAdmit;

    setVolt( volt );
}

void  eNode::setVolt( double v )
{
    if( m_volt == v ) return;

    m_voltChanged = true; // Used for wire animation
    m_volt = v;

    CallBackElement* linked = m_voltChEl; // VoltChaneg callback
    while( linked )
    {
        eElement* el = linked->element;
        linked = linked->next;
        if( el->added ) continue;
        Simulator::self()->addToChangedList( el );
        el->added = true;
    }
    linked = m_nonLinEl ;              // Non Linear callback
    while( linked )
    {
        eElement* el = linked->element;
        linked = linked->next;
        if( el->added ) continue;
        Simulator::self()->addToNoLinList( el );
        el->added = true;
    }
}


//将目标引脚添加到当前节点的引脚列表中
void eNode::addEpin( ePin* epin )
{ 
    //if( !m_ePinList.contains(epin)) m_ePinList.append(epin); 
    if (std::find(m_ePinList.begin(), m_ePinList.end(), epin) == m_ePinList.end())
    {
        // 如果没有找到，添加到 m_nodeList
        m_ePinList.push_back(epin);
    }
}

void eNode::remEpin( ePin* epin )
{
    // 使用 std::remove 查找并移动要删除的元素到 vector 的末尾
    auto it = std::remove(m_ePinList.begin(), m_ePinList.end(), epin);

    // 如果找到了，使用 erase 方法从 vector 中移除元素
    if (it != m_ePinList.end()) {
        m_ePinList.erase(it);
    }
}

void eNode::clear()
{
    for( ePin* epin : m_ePinList ){
        epin->setEnode( NULL );
        epin->setEnodeComp( NULL );
}   }

std::vector<int>& eNode::getConnections()
{ return m_nodeList; }

void eNode::voltChangedCallback( eElement* el )
{
    CallBackElement* changed = m_voltChEl;
    while( changed )
    {
        if( el == changed->element ) return; // Element already in the list
        changed = changed->next;
    }
    CallBackElement* newLinked = new CallBackElement( el );
    newLinked->next = m_voltChEl; // Prepend
    m_voltChEl = newLinked;
}

void eNode::remFromChangedCallback( eElement* el )
{
    CallBackElement* changed = m_voltChEl;
    CallBackElement* last  = NULL;
    CallBackElement* next  = NULL;

    while( changed ){
        next = changed->next;
        if( el == changed->element )
        {
            if( last ) last->next = next;
            else       m_voltChEl = next;
            delete changed;//changed->next = NULL;
        }
        else last = changed;
        changed = next;
    }
}

void eNode::addToNoLinList( eElement* el )
{
    CallBackElement* changed = m_nonLinEl;
    while( changed )
    {
        if( el == changed->element ) return; // Element already in the list
        changed = changed->next;
    }
    CallBackElement* newLinked = new CallBackElement( el );
    newLinked->next = m_nonLinEl; // Prepend
    m_nonLinEl = newLinked;
    //qDebug() <<m_id<< el->getId();
}


//改变连接器状态，用于图形用户界面上正确地显示这些变化
void eNode::updateConnectors()
{
    if( !m_voltChanged ) return;
    m_voltChanged = false;

    for( ePin* epin : m_ePinList )
    {
        Pin* pin = epin->getPin();
        if( pin  )   //&& pin->isVisible()
        {
            Connector* conn = pin->connector();
            //if( conn ) conn->updateLines();
        }
    }
}

void eNode::clearElmList( CallBackElement* first )
{
    while( first ){
        CallBackElement* del = first;
        first = first->next;
        delete del;
    }
}

void eNode::clearConnList( Connection* first )
{
    while( first )
    {
        Connection* del = first;
        first = first->next;
        delete del;
    }
}
