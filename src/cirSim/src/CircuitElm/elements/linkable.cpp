/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include <iostream>
#include <algorithm>
#include <sstream>

#include "linkable.h"
#include "component.h"
#include "circuit.h"

Linkable* Linkable::m_selecComp = NULL;

Linkable::Linkable()
    //: m_linkCursor(QPixmap(":/link.png"), 10, 10)
{}
Linkable::~Linkable()
{
    if (m_selecComp == this) Linkable::compSelected(NULL); // Cancel link to components
     for( Component* comp : m_linkedComp ) comp->setLinkedTo( nullptr );
}

std::string Linkable::getLinks()
{
    std::string links;

    for (int i = 0; i < m_linkedComp.size(); ++i)
    {
        Component* comp = m_linkedComp.at(i);
        links.append(comp->getUid() + ",");
    }
    return links;
}

void Linkable::setLinks(std::string links)
{
    m_linkedStr = links;
}


Component* Linkable::getLinkedComp(size_t index)
{
    if (index < m_linkedComp.size()) 
    {
        return m_linkedComp[index];
    }
    return nullptr;
}

bool Linkable::hasLinks()
{
    return !m_linkedStr.empty();
}

void Linkable::createLinks(std::unordered_set<Component*>* compList)
{
    std::istringstream iss(m_linkedStr);
    std::string uid;
    while (std::getline(iss, uid, ',')) 
    {
        if (uid.empty()) continue;

        for (Component* comp : *compList) 
        {
            if (comp->getUid().find(uid) != std::string::npos) 
            {
                 std::cout << "Linkable::createLinks " << uid << std::endl;
                if( comp->setLinkedTo( this ) ) m_linkedComp.push_back(comp);
                break;
            }
        }
    }
}

void Linkable::startLinking()  // Start linking Components
{
    if (m_selecComp) m_selecComp->compSelected(NULL); // Finish previous linking
    m_selecComp = this;
    showLinked( true );
    // for (int i = 0; i < m_linkedComp.size(); ++i) // Set numbers for visualization
    // {
    //     Component* comp = m_linkedComp.at(i);
    //     comp->m_linkNumber = i;
    //     //comp->update();
    // }
}

void Linkable::stopLinking() // Static
{
    if (!m_selecComp) return;
    m_selecComp->compSelected(NULL);

    //QGuiApplication::restoreOverrideCursor();
}

void Linkable::compSelected(Component* comp)
{
    if (comp)  // One Component was selected to link
    {
        bool linked = (std::find(m_linkedComp.begin(), m_linkedComp.end(), comp) != m_linkedComp.end());

        if (linked)
        {
            comp->setLinkedTo( nullptr );
            comp->m_linkNumber = -1;
            m_linkedComp.erase(
                std::remove(m_linkedComp.begin(), m_linkedComp.end(), comp),
                m_linkedComp.end()
            );
        }
        else {
            if( comp->setLinkedTo( this ) )
                m_linkedComp.push_back(comp);
        }
        showLinked( true );
    }
    else       // End of linking Components
    {
        m_selecComp = nullptr;
        showLinked( false );
    }
}

void Linkable::showLinked(bool show)
{
        if( show ){
        for( int i=0; i<m_linkedComp.size(); ++i ) // Set numbers for visualization
        {
            Component* comp = m_linkedComp.at( i );
            comp->m_linkNumber = i;
        }
    }else{
        for( Component* comp : m_linkedComp )  // Clear numbers for visualization
            comp->m_linkNumber = -1;
    }
}
