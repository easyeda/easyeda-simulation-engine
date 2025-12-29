/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "tunnel.h"
#include "linkable.h"
#include "itemlibrary.h"
//#include "propdialog.h"
//#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "e-node.h"
#include "pin.h"
#include "utils.h"

#include "stringProp.h"
#include "boolProp.h"

//#define tr(str) simulideTr("Tunnel",str)

std::unordered_map<std::string, std::vector<Tunnel*>*> Tunnel::m_tunnels;

Component* Tunnel::construct( std::string type, std::string id )
{ return new Tunnel( type, id ); }

LibraryItem* Tunnel::libraryItem()
{
    return new LibraryItem(
        "Tunnel.",
        "Connectors",
        "tunnel.png",
        "Tunnel",
        Tunnel::construct );
}

Tunnel::Tunnel( std::string type, std::string id )
      : Component( type, id )
{
    m_size = 20;
    //m_area = QRect( -m_size-8-4, -4, m_size+4, 8 );
    m_blocked = false;
    m_packed  = false;
    m_show    = false;
    m_name = "";

    m_pin.resize( 1 );
    m_pin[0] = new Pin(id+"-pin", 0, this);
    //m_pin[0]->setLabelColor( Qt::black );
    //m_pin[0]->setLength( 5 );
    //m_pin[0]->setSpace( 4 );

    //setLabelPos(-16,-24, 0);
    addPropGroup( { "Main", {
        new stringProp <Tunnel>("Name" , "Id" ,"", this, &Tunnel::name,  &Tunnel::setName ),
        new boolProp<Tunnel>("IsBus", "Bus","", this, &Tunnel::isBus, &Tunnel::setIsbus )
    }, groupNoCopy} );
    addPropGroup( {"Hidden", {
        new stringProp<Tunnel>( "Uid","","", this, &Tunnel::tunnelUid, &Tunnel::setTunnelUid ),
    }, groupNoCopy | groupHidden} );
}
Tunnel::~Tunnel() {}

eNode* Tunnel::getEnode( std::string n )
{
    std::vector<Tunnel*>* list = m_tunnels.at( n );
    if( !list ) return NULL;
    Tunnel* tunnel= list->front();
    if( tunnel ) return tunnel->getPin()->getEnode();
    return NULL;
}

void Tunnel::clearTunnels()
{
    m_tunnels.clear();
}

void Tunnel::setEnode( eNode* node, int n )
{
    if( m_blocked ) return;
    m_blocked = true;

    if( node ) m_pin[0]->registerPinsW( node, n );
    m_blocked = false;
}

void Tunnel::registerEnode( eNode* enode, int n ) // called by m_pin[0]
{
    if( m_blocked ) return;

    std::vector<Tunnel*>* list = m_tunnels.at( m_name );
    if( !list ) return;

    m_blocked = true;

    for (Tunnel* tunnel : *list)
    {
        if (tunnel != this)
            tunnel->setEnode(enode, n);
    }


    m_blocked = false;
}

void Tunnel::setName( std::string name )
{
    setGroupName( name, true );
}

void Tunnel::setGroupName( std::string name, bool single )
{
    // 替换列表：{原始字符串, 替换字符串}
    std::vector<std::pair<std::string, std::string>> replacements = {
        {"&#x3C;", "<"}, {"&#x3D;", "="}, {"&#x3E;", ">"},  // 带分号的
        {"&#x3C", "<"}, {"&#x3D", "="}, {"&#x3E", ">"}      // 不带分号的
    };
    // 逐个替换
    for (const auto& [from, to] : replacements) {
        size_t pos = 0;
        while ((pos = name.find(from, pos)) != std::string::npos) {
            name.replace(pos, from.length(), to);
            pos += to.length();  // 避免死循环
        }
    }
    if( name == m_name ) 
        return;
    if (single && Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    removeTunnel(); // Remove old name before setting new one

    m_name = name;
    //if( !m_packed ) 
    //    m_pin[0]->setLabelText( name, false );
    //if( name == "" ) m_size = 20;
    //else  m_size = snapToGrid( m_pin[0]->labelSizeX()+4 );
    //setRotated( m_rotated );

    if( m_tunnels.contains( name ) ) // There is already tunnel with this name
    {
        std::vector<Tunnel*>*list = m_tunnels.at(name);
        if (std::find(list->begin(), list->end(), this) == list->end()) 
        {
            list->push_back(this);
        }
    }
    else   // name doesn't exist: Create a new List for this name
    {
        std::vector<Tunnel*>* list = new std::vector<Tunnel*>();
        list->push_back( this );
        m_tunnels[name] = list;
    }
    //if( single ) Circuit::self()->update();
    //if( m_propDialog ) m_propDialog->updtValues();
}

bool Tunnel::isBus()
{
    return m_pin[0]->isBus();
}

void Tunnel::setIsbus( bool b )
{
    m_pin[0]->setIsBus( b );
}

//void Tunnel::setRotated( bool rot )
//{
//    m_rotated  = rot;
//    if( rot )
//    {
//        m_area = QRect( 4, -4, m_size+4, 8 );
//        m_pin[0]->setPinAngle( 180 );
//    }else 
//    {
//        m_area = QRect( -m_size-8, -4, m_size+4, 8 );
//        m_pin[0]->setPinAngle( 0 );
//    }
//    m_pin[0]->setLabelPos();
//    Circuit::self()->update();
//}

void Tunnel::setPacked( bool p )
{
    m_packed = p;
    //if( p ) m_pin[0]->setLength( 8 );
    //else    m_pin[0]->setLength( 5 );
    //m_pin[0]->setSpace( 8-m_pin[0]->length() );
    //if( this->parent() )
    //{
    //    Component* comp = (Component*)parent();
    //    connect( comp, &Component::flip,
    //               this, &Tunnel::flip(); } );
    //}
}

void Tunnel::removeTunnel()
{
    if (m_name.empty()) return;

    auto it = m_tunnels.find(m_name);
    if (it == m_tunnels.end()) 
        return; // 如果找不到键，直接返回

    std::vector<Tunnel*>* list = it->second;
    list->erase(std::remove(list->begin(), list->end(), this), list->end()); // 移除所有匹配的元素

    if (!list->empty()) return; // 如果列表不为空，直接返回

    m_tunnels.erase(it); // 如果列表为空，从 map 中移除该键
}

void Tunnel::remove()
{
    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    removeTunnel();
    Component::remove();
    //Circuit::self()->update();
}

//void Tunnel::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
//{
//    if( m_show )
//    {
//        QAction* hideAction = menu->addAction( QIcon(":nobreakpoint.png"),tr("Hide group") );
//        QObject::connect( hideAction, &QAction::triggered, [=](){ hideGroup(); } );
//    }else{
//        QAction* showAction = menu->addAction( QIcon(":/breakpoint.png"),tr("Show group") );
//        QObject::connect( showAction, &QAction::triggered, [=](){ showGroup(); } );
//    }
//    QAction* nameAction = menu->addAction( QIcon(":/rename.svg"),tr("Rename group") );
//    QObject::connect( nameAction, &QAction::triggered, [=](){ renameGroup(); } );
//
//    menu->addSeparator();
//    Component::contextMenu( event, menu );
//}

//void Tunnel::renameGroup()
//{
//    bool ok;
//    std::string text = QInputDialog::getText( NULL, tr("Rename Tunnels"),
//                                         tr("New name:"), QLineEdit::Normal,
//                                         m_name, &ok );
//    if( ok && !text.isEmpty() )
//    {
//        QList<Tunnel*>* list = m_tunnels.value( m_name );
//        if( !list ) return;
//        QVector<Tunnel*> tunnels = list->toVector();
//        for( Tunnel* tunnel: tunnels ) tunnel->setGroupName( text, false );
//
//        Circuit::self()->update();
//    }
//}

//void Tunnel::showGroup()
//{
//    for( QList<Tunnel*>* list : m_tunnels.values() )  // Hide other groups
//    {
//        if( !list ) continue;
//        for( Tunnel* tunnel: *list ) tunnel->m_show = false;
//    }
//    showHide( true );
//}

//void Tunnel::showHide( bool show )
//{
//    QList<Tunnel*>* list = m_tunnels.value( m_name );
//    if( !list ) return;
//    for( Tunnel* tunnel: *list ) tunnel->m_show = show;
//    Circuit::self()->update();
//}

//QRectF Tunnel::boundingRect() const
//{
//    if( m_packed ) return QRectF( 0, 0, 0 ,0 );
//    else return Component::boundingRect();
//}
//


