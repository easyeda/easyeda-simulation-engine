/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "socket.h"
#include "itemlibrary.h"

#include "simulator.h"
#include "circuit.h"
#include "e-node.h"

//#define tr(str) simulideTr("Socket",str)

Component* Socket::construct( std::string type, std::string id )
{ return new Socket( type, id ); }

LibraryItem* Socket::libraryItem()
{
    return new LibraryItem(
        "Socket",
        "Connectors",
        "socket.png",
        "Socket",
        Socket::construct);
}

Socket::Socket( std::string type, std::string id )
      : ConnBase( type, id )
{

    m_pinType = Pin::pinSocket;


    Simulator::self()->addToSocketList( this );
    Simulator::self()->addToUpdateList( this );
}
Socket::~Socket()
{
    Simulator::self()->remFromSocketList( this );
}

void Socket::updateStep()
{
    connectPins( false );
}

void Socket::connectPins(bool connect)
{
    for( int i=0; i<m_size; i++ )
    {
        if( m_sockPins[i]->connector() ) continue;

        Pin* pin = m_sockPins[i]->connectPin( false );
        if( pin ){
            if( connect )
            {
                //std::cout <<"Connecting"<< m_sockPins[i]->pinId()<<"To"<<pin->pinId();
                m_connPins[i] = pin;
                m_sockPins[i]->setConPin( pin );
                pin->setConPin( m_sockPins[i] );
            }
            else if( !m_connPins[i] )  Simulator::self()->stopSim();
        } else {
            if( m_connPins[i] )
            {
                m_sockPins[i]->setConPin( nullptr );
                m_connPins[i]->setConPin( nullptr );
                m_connPins[i] = nullptr;
                Simulator::self()->stopSim();            
            }
        }
    }
}

//void Socket::updatePixmap()
//{
//    int angle = this->rotation();
//    switch( angle ) {
//        case 0:
//        case 180:
//        case -180:
//            m_pinPixmap.load( ":/socket_V.png" );
//            m_pinPixmap = m_pinPixmap.transformed( QTransform().rotate(angle) );
//            break;
//        case 90:
//        case -90:
//            m_pinPixmap.load( ":/socket_H.png" );
//            m_pinPixmap = m_pinPixmap.transformed( QTransform().rotate(angle-180) );
//            break;
//        default: break;
//    }
//}
