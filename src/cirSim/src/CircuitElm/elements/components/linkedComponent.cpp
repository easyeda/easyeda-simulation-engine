/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "linkedComponent.h"
#include "stringProp.h"

#define tr(str) simulideTr("LinkedComponent",str)

LinkedComponent::LinkedComponent( std::string type, std::string id )
               : Component( type, id )
{
    m_isLinker = true;

    addPropGroup({ "Main", {
new stringProp<LinkedComponent>("Links","Links","", this, &LinkedComponent::getLinks,  &LinkedComponent::setLinks,0,"enum"),
    },groupHidden });
}
LinkedComponent::~LinkedComponent(){}


//void LinkedComponent::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
//{
//    if( !acceptedMouseButtons() ) { event->ignore(); return; }
//
//    event->accept();
//    QMenu* menu = new QMenu();
//
//    if( !parentItem() )
//    {
//        QAction* linkCompAction = menu->addAction( QIcon(":/subcl.png"),tr("Link to Component") );
//        QObject::connect( linkCompAction, &QAction::triggered, [=](){ slotLinkComp(); } );
//
//        menu->addSeparator();
//    }
//    Component::contextMenu( event, menu );
//    menu->deleteLater();
//}
