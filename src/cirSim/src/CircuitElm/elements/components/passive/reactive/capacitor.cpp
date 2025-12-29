/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "capacitor.h"
#include "itemlibrary.h"

#define tr(str) simulideTr("Capacitor",str)

Component* Capacitor::construct( std::string type, std::string id )
{ return new Capacitor( type, id ); }

LibraryItem* Capacitor::libraryItem()
{
    return new LibraryItem(
        "Capacitor",
        "Reactive",
        "capacitor.png",
        "Capacitor",
        Capacitor::construct);
}

Capacitor::Capacitor( std::string type, std::string id )
         : CapacitorBase( type, id )
{}
Capacitor::~Capacitor(){}
