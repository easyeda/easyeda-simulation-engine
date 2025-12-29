/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "zener.h"
#include "itemlibrary.h"

#define tr(str) simulideTr("Zener",str)

Component* Zener::construct( std::string type, std::string id )
{ return new Zener( type, id ); }

LibraryItem* Zener::libraryItem()
{
    return new LibraryItem(
        "Zener Diode",
        "Rectifiers",
        "zener.png",
        "Zener",
        Zener::construct);
}

std::vector<std::string> Zener::getKeys()
{
    std::vector<std::string> keys;
    for (const auto& pair : m_zeners) 
    {
        keys.push_back(pair.first);
    }
    return keys;
}

Zener::Zener( std::string type, std::string id )
     : Diode( type, id, true )
{

    std::vector<std::string> keys = getKeys();
    m_enumUids = keys;
    m_enumNames = keys;
}
Zener::~Zener(){}
