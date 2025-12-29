/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include  "pch.h"

class Component;

class Linkable
{
public:
    Linkable();
    ~Linkable();

    std::string getLinks();
    void setLinks(std::string links);

    Component* getLinkedComp(size_t index);

    bool hasLinks();

    void startLinking();
    static void stopLinking();

    static Linkable* m_selecComp;

    virtual void createLinks(std::unordered_set<Component*>* compList);
    virtual void compSelected(Component* comp);

    void showLinked( bool show );

protected:
    std::vector<Component*> m_linkedComp;

    std::string m_linkedStr;
};

